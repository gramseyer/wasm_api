use core::ffi::c_void;
use core::slice;
use wasmtime::{Instance, Module, Store, Val};

use crate::wasmtime_context::WasmtimeContext;
use crate::external_call;
use crate::invoke_result::{InvokeError, FFIInvokeResult};

// A WasmiRuntime implements a single WebAssembly module,
// with the ability to invoke exported functions and call
// in to provided syscalls.
//
// Notes:
// Store has an internal ID, for error checking the internals.
// A lot of elements inside are indices to buffers within Store,
// which is obviously broken if the wrong store is used.
// Wasmi guards against this by giving each Store a unique id,
// which is derived from an AtomicU32.fetch_add on a static variable.
// This is fine for our case, as AtomicU32 wraps (does not panic!) on
// overflow.
//
// Store/module/instance store internally an Engine (which is an Arc around
// the actual EngineInner struct), so it's safe for a WasmiRuntime object
// to outlive the parent WasmiContext object (although we will probably
// not take advantage of this)
pub struct WasmtimeRuntime {
    pub store: Store<*mut c_void>,
    pub instance: Instance,
}

fn assert_runtime_not_null(runtime: *const WasmtimeRuntime) {
    assert!(runtime != core::ptr::null());
}

fn handle_trap_code(code: &wasmtime::Trap) -> FFIInvokeResult {
    match code {
        wasmtime::Trap::OutOfFuel => {
            return FFIInvokeResult::error(InvokeError::OUT_OF_GAS_ERROR);
        },
        // wasm atomic feature flags should be disabled
        wasmtime::Trap::HeapMisaligned => {
            panic!("atomic should be disabled");
        },
        wasmtime::Trap::AtomicWaitNonSharedMemory => {
            panic!("atomic should be disabled");
        },

        // nondeterministic
        wasmtime::Trap::Interrupt => {
            panic!("epoch metering should be disabled");
        },

        // wasmtime does not have deterministic stack limits
        wasmtime::Trap::StackOverflow => {
            return FFIInvokeResult::error(InvokeError::UNRECOVERABLE);
        },

        // deterministic
        wasmtime::Trap::MemoryOutOfBounds => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::TableOutOfBounds => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::IndirectCallToNull => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::BadSignature => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::IntegerOverflow => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::IntegerDivisionByZero => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::BadConversionToInteger => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },
        wasmtime::Trap::UnreachableCodeReached => {
            return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
        },

        // no idea what this is for
        wasmtime::Trap::AlwaysTrapAdapter => {
            return FFIInvokeResult::error(InvokeError::UNRECOVERABLE);
        },
        _ => {
            return FFIInvokeResult::error(InvokeError::UNRECOVERABLE);
        },
    }
}

impl WasmtimeRuntime {
    fn new(
        bytes: &[u8],
        context: &WasmtimeContext,
        userctx: *mut c_void,
    ) -> Option<WasmtimeRuntime> {
        let module = match Module::new(&context.engine, &bytes) {
            Ok(m) => m,
            Err(_) => {
                return None;
            }
        };

        let mut store = Store::new(&context.engine, userctx);

        // TODO(geoff): test to ensure start() function doesn't run
        let instance = match context.linker.instantiate(&mut store, &module) {
            Ok(inst) => inst,
            Err(_) => {
                return None;
            }
        };
        Some(Self {
            store: store,
            instance: instance,
        })
    }

    fn invoke(&mut self, method: &str) -> FFIInvokeResult {
        let func = match self.instance.get_func(&mut self.store, method) {
            Some(v) => v,
            _ => {
                return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
            }
        };

        let mut res = [Val::I64(0)];

        let func_res = func.call(&mut self.store, &[], &mut res);

        match func_res {
            Ok(_) => match res[0].i64() {
                Some(v) => {
                    return FFIInvokeResult::success(v as u64);
                }
                _ => {
                    return FFIInvokeResult::error(
                        InvokeError::DETERMINISTIC_ERROR,
                    );
                }
            },
            Err(err) => {
                // wasmtime docs:
                // The “base” error or anyhow::Error::root_cause is a Trap whenever WebAssembly hits a trap, 
                // or otherwise it’s whatever the host created the error 
                // with when returning an error for a host call.

                match err.downcast_ref::<wasmtime::Trap>() {
                    Some (trap) => {
                        return handle_trap_code(trap);
                    }
                    _ => {}
                };

                let my_error = match err
                    .downcast_ref::<external_call::TrampolineError>()
                {
                    Some(trampoline_error) => trampoline_error.clone(),
                    None => {
                        return FFIInvokeResult::error(
                            InvokeError::UNRECOVERABLE,
                        );
                    }
                };

                match my_error.error {
                    external_call::HostFnError::NONE_OR_RECOVERABLE => { panic!("impossible"); },
                    external_call::HostFnError::RETURN_SUCCESS => {
                        return FFIInvokeResult::error(
                            InvokeError::RETURN);
                    },
                    external_call::HostFnError::OUT_OF_GAS => {
                        return FFIInvokeResult::error(
                            InvokeError::OUT_OF_GAS_ERROR);
                    },
                    external_call::HostFnError::UNRECOVERABLE => {
                        return FFIInvokeResult::error(
                            InvokeError::UNRECOVERABLE);
                    }
                }
            }
        };
    }
}

#[no_mangle]
pub extern "C" fn wasmtime_invoke(
    runtime_void: *mut c_void,
    method_name: *const u8,
    method_name_len: u32,
) -> FFIInvokeResult {
    let runtime: *mut WasmtimeRuntime =
        unsafe { core::mem::transmute(runtime_void) };

    assert_runtime_not_null(runtime);

    if method_name == core::ptr::null() {
        return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR);
    }

    let method_name_slice =
        unsafe { slice::from_raw_parts(method_name, method_name_len as usize) };

    let string = match std::str::from_utf8(&method_name_slice) {
        Ok(v) => v,
        _ => return FFIInvokeResult::error(InvokeError::DETERMINISTIC_ERROR),
    };

    let r = unsafe { &mut *runtime };

    r.invoke(string)
}

#[no_mangle]
pub extern "C" fn new_wasmtime_runtime(
    bytes: *const u8,
    bytes_len: u32,
    userctx: *mut c_void,
    wasmtime_context_ptr: *const c_void,
) -> *mut c_void {
    if bytes == core::ptr::null() {
        return core::ptr::null_mut();
    }

    assert!(wasmtime_context_ptr != core::ptr::null());

    let wasmtime_context: &WasmtimeContext = unsafe {
        &*core::mem::transmute::<_, *mut WasmtimeContext>(wasmtime_context_ptr)
    };

    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let b = match WasmtimeRuntime::new(&slice, &wasmtime_context, userctx) {
        Some(res) => Box::new(res),
        None => {
            return core::ptr::null_mut();
        }
    };

    return unsafe { core::mem::transmute(Box::into_raw(b)) };
}

#[no_mangle]
pub extern "C" fn free_wasmtime_runtime(p: *mut c_void) {
    let runtime: *mut WasmtimeRuntime = unsafe { core::mem::transmute(p) };

    assert_runtime_not_null(runtime);

    unsafe { drop(Box::from_raw(runtime)) };
}
