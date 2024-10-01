use core::ffi::c_void;
use core::slice;
use wasmi::{Instance, Module, Store, Val};

use crate::wasmi_context::WasmiContext;
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
pub struct WasmiRuntime {
    pub store: Store<*mut c_void>,
    pub instance: Instance,
}

fn assert_runtime_not_null(runtime: *const WasmiRuntime) {
    assert!(runtime != core::ptr::null());
}

fn handle_trap_code(code: wasmi::core::TrapCode) -> FFIInvokeResult {
    // all of the wasmi trap codes, in version 0.36,
    // are raised deterministically.
    // For now, at least, we handle all as deterministic errors.
    match code {
        wasmi::core::TrapCode::OutOfFuel => {
            return FFIInvokeResult::error(InvokeError::OUT_OF_GAS_ERROR);
        }
        _ => {
            return FFIInvokeResult::error(
                InvokeError::DETERMINISTIC_ERROR,
            );
        }
    }
}

impl WasmiRuntime {
    fn new(
        bytes: &[u8],
        context: &WasmiContext,
        userctx: *mut c_void,
    ) -> Option<WasmiRuntime> {
        let module = match Module::new(&context.engine, &bytes) {
            Ok(m) => m,
            Err(_) => {
                return None;
            }
        };

        let mut store = Store::new(&context.engine, userctx);

        let instance = match context.linker.instantiate(&mut store, &module) {
            Ok(inst_pre) => match inst_pre.ensure_no_start(&mut store) {
                Ok(inst) => inst,
                Err(_) => {
                    return None;
                }
            },
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
        let func = match self.instance.get_func(&self.store, method) {
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
                match err.as_trap_code() {
                    Some(trap_code) => {
                        return handle_trap_code(trap_code);
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

                return FFIInvokeResult::from_host_error(my_error.error);
            }
        };
    }
}

#[no_mangle]
pub extern "C" fn wasmi_invoke(
    runtime_void: *mut c_void,
    method_name: *const u8,
    method_name_len: u32,
) -> FFIInvokeResult {
    let runtime: *mut WasmiRuntime =
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
pub extern "C" fn new_wasmi_runtime(
    bytes: *const u8,
    bytes_len: u32,
    userctx: *mut c_void,
    wasmi_context_ptr: *const c_void,
) -> *mut c_void {
    if bytes == core::ptr::null() {
        return core::ptr::null_mut();
    }

    assert!(wasmi_context_ptr != core::ptr::null());

    let wasmi_context: &WasmiContext = unsafe {
        &*core::mem::transmute::<_, *mut WasmiContext>(wasmi_context_ptr)
    };

    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let b = match WasmiRuntime::new(&slice, &wasmi_context, userctx) {
        Some(res) => Box::new(res),
        None => {
            return core::ptr::null_mut();
        }
    };

    return unsafe { core::mem::transmute(Box::into_raw(b)) };
}

#[no_mangle]
pub extern "C" fn free_wasmi_runtime(p: *mut c_void) {
    let runtime: *mut WasmiRuntime = unsafe { core::mem::transmute(p) };

    assert_runtime_not_null(runtime);

    unsafe { drop(Box::from_raw(runtime)) };
}
