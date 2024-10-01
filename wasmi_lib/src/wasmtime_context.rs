use wasmtime::{Caller, Config, Engine, Error, Linker};

use core::ffi::c_void;

use crate::external_call::{HostFnError, TrampolineResult, TrampolineError};
use crate::external_call;
use crate::common::{string_from_parts, BorrowBypass};

// A WasmtimeContext provides shared data structures
// for multiple Wasmtime runtimes.

pub struct WasmtimeContext {
    pub engine: Engine,
    pub linker: Linker<*mut c_void>,
}

fn wasmtime_handle_trampoline_error(result: TrampolineResult) -> Result<u64, wasmtime::Error> {
    let err: HostFnError = unsafe { std::mem::transmute(result.panic) };
    match err {
        HostFnError::NONE_OR_RECOVERABLE => Ok(result.result),
        _ => Err(wasmtime::Error::new(TrampolineError { error : err })),
    }
}

impl WasmtimeContext {
    fn new() -> Option<Self> {
        let engine = Engine::new(
            &Config::default()
                .consume_fuel(true)
                .wasm_backtrace(false)
                .strategy(wasmtime::Strategy::Cranelift)
                .cranelift_opt_level(wasmtime::OptLevel::Speed)
                .cranelift_nan_canonicalization(true) // deterministic nan
        ).ok()?;

        Some(Self {
            engine: engine.clone(),
            linker: Linker::new(&engine),
        })
    }

    fn link_function_0args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = BorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        match self.linker.func_wrap(
            import_name,
            fn_name,
            move |caller: Caller<'_, *mut c_void>| -> Result<u64, wasmtime::Error> {

                let res = unsafe {
                    external_call::c_call_0args(x.clone().fn_pointer, caller.data().clone())
                };

                return wasmtime_handle_trampoline_error(res);
        }) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    fn link_function_1args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = BorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        match self.linker.func_wrap(
            import_name,
            fn_name,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64|
                  -> Result<u64, wasmtime::Error> {
                let res = unsafe {
                    external_call::c_call_1args(
                        x.clone().fn_pointer,
                        caller.data().clone(),
                        arg1,
                    )
                };

                return wasmtime_handle_trampoline_error(res);
            },
        ) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    fn link_function_2args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = BorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        match self.linker.func_wrap(
            import_name,
            fn_name,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64|
                  -> Result<u64, wasmtime::Error> {
                let res = unsafe {
                    external_call::c_call_2args(
                        x.clone().fn_pointer,
                        caller.data().clone(),
                        arg1,
                        arg2,
                    )
                };
                return wasmtime_handle_trampoline_error(res);
            },
        ) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    fn link_function_3args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = BorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        match self.linker.func_wrap(
            import_name,
            fn_name,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64,
                  arg3: u64|
                  -> Result<u64, wasmtime::Error> {
                let res = unsafe {
                    external_call::c_call_3args(
                        x.clone().fn_pointer,
                        caller.data().clone(),
                        arg1,
                        arg2,
                        arg3,
                    )
                };
                return wasmtime_handle_trampoline_error(res);
            },
        ) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    fn link_function_4args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = BorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        match self.linker.func_wrap(
            import_name,
            fn_name,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64,
                  arg3: u64,
                  arg4: u64|
                  -> Result<u64, wasmtime::Error> {
                let res = unsafe {
                    external_call::c_call_4args(
                        x.clone().fn_pointer,
                        caller.data().clone(),
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                    )
                };
                return wasmtime_handle_trampoline_error(res);
            },
        ) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    fn link_function_5args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = BorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        match self.linker.func_wrap(
            import_name,
            fn_name,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64,
                  arg3: u64,
                  arg4: u64,
                  arg5: u64|
                  -> Result<u64, wasmtime::Error> {
                let res = unsafe {
                    external_call::c_call_5args(
                        x.clone().fn_pointer,
                        caller.data().clone(),
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                        arg5,
                    )
                };
                return wasmtime_handle_trampoline_error(res);
            },
        ) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }
}

#[no_mangle]
pub extern "C" fn wasmtime_link_nargs(
    context_void: *mut c_void,
    module_name: *const u8,
    module_name_len: u32,
    method_name: *const u8,
    method_name_len: u32,
    function_pointer: *mut c_void,
    nargs: u8,
) -> bool // true if success
{
    let context: *mut WasmtimeContext =
        unsafe { core::mem::transmute(context_void) };

    assert!(context != core::ptr::null_mut());

    assert!(module_name != core::ptr::null());
    assert!(method_name != core::ptr::null());

    // assert module name & method_name nonzero

    let module = match string_from_parts(module_name, module_name_len) {
        Ok(x) => x,
        _ => {
            return false;
        }
    };

    let method = match string_from_parts(method_name, method_name_len) {
        Ok(x) => x,
        _ => {
            return false;
        }
    };

    let c = unsafe { &mut *context };

    let res = match nargs {
        0 => c.link_function_0args(function_pointer, &module, &method),
        1 => c.link_function_1args(function_pointer, &module, &method),
        2 => c.link_function_2args(function_pointer, &module, &method),
        3 => c.link_function_3args(function_pointer, &module, &method),
        4 => c.link_function_4args(function_pointer, &module, &method),
        5 => c.link_function_5args(function_pointer, &module, &method),
        _ => {
            return false;
        }
    };

    match res {
        Ok(_) => true,
        Err(_) => false,
    }
}

// Rust FFI needs no_mangle and extern "C"
#[no_mangle]
pub extern "C" fn new_wasmtime_context() -> *mut c_void {
    
    let b = match WasmtimeContext::new() {
        Some(x) => {Box::new(x)},
        None => { return core::ptr::null_mut(); },
    };

    return unsafe { core::mem::transmute(Box::into_raw(b)) };
}

#[no_mangle]
pub extern "C" fn free_wasmtime_context(p: *mut c_void) {
    assert!(p != core::ptr::null_mut());

    let runtime: *mut WasmtimeContext = unsafe { core::mem::transmute(p) };

    unsafe { drop(Box::from_raw(runtime)) };
}
