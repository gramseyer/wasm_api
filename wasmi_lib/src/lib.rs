use wasmi::{Caller, Config, Engine, Error, Func, Instance, Linker, Module, Store, Val};

use core::ffi::c_void;
use std::slice;

use core::str::Utf8Error;

#[allow(non_camel_case_types)]
#[derive(Clone)]
pub struct Wasmi_WasmContext {
    engine: Engine, // wasmi engines are copyable (they're a wrapper around an Arc<EngineInner>)
}

impl Wasmi_WasmContext {
    pub fn new() -> Wasmi_WasmContext {

        Self {
            engine: Engine::new(&Config::default().consume_fuel(true).floats(false)),
        }
    }
}

impl Drop for Wasmi_WasmContext {
    fn drop(&mut self) {}
}

#[allow(non_camel_case_types)]
pub struct Wasmi_WasmRuntime {
    store: Store<*mut c_void>,
    module: Module,
    linker: Linker<*mut c_void>,
    instance: Option<Instance>,
}

#[allow(dead_code)]
#[repr(u8)]
enum TrampolineError {
    None = 0,
    HostError = 1,
    UnrecoverableSystemError = 2,
}

#[repr(C)]
pub struct TrampolineResult {
    result: u64,
    panic: u8,
}

extern "C" {
    fn c_call_0args(fn_pointer: *mut c_void, userctx: *mut c_void) -> TrampolineResult;
    fn c_call_1args(fn_pointer: *mut c_void, userctx: *mut c_void, arg1: u64) -> TrampolineResult;
    fn c_call_2args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
    ) -> TrampolineResult;
    fn c_call_3args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
    ) -> TrampolineResult;
    fn c_call_4args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
        arg4: u64,
    ) -> TrampolineResult;
    fn c_call_5args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
        arg4: u64,
        arg5: u64,
    ) -> TrampolineResult;
}

#[derive(Clone)]
pub struct AnnoyingBorrowBypass {
    fn_pointer: *mut c_void,
}

unsafe impl Send for AnnoyingBorrowBypass {}
unsafe impl Sync for AnnoyingBorrowBypass {}

fn handle_trampoline_error(result: TrampolineResult) -> Result<u64, Error> {
    let err: TrampolineError = unsafe { std::mem::transmute(result.panic) };
    match err {
        TrampolineError::None => Ok(result.result),
        TrampolineError::HostError => { Err(Error::new("HostError")) },
        _ => { Err(Error::new("UnrecoverableSystemError")) },
    }
}

impl Wasmi_WasmRuntime {
    pub fn new(
        context: &Wasmi_WasmContext,
        bytes: &[u8],
        userctx: *mut c_void,
    ) -> Wasmi_WasmRuntime {
        let store = Store::new(&context.engine, userctx);
        let module = Module::new(&context.engine, &bytes).unwrap();
        Self {
            module: module,
            store: store,
            linker: Linker::new(&context.engine),
            instance: None,
        }
    }
    pub fn lazy_link(&mut self) -> Result<(), wasmi::Error> {
        match self.instance {
            Some(_) => {
                return Ok(());
            }
            None => match self.linker.instantiate(&mut self.store, &self.module) {
                Ok(inst_pre) => match inst_pre.start(&mut self.store) {
                    Ok(inst) => {
                        self.instance = Some(inst);
                        Ok(())
                    }
                    Err(x) => Err(x),
                },
                Err(x) => Err(x),
            },
        }
    }

    pub fn link_function_0args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = AnnoyingBorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        let func = Func::wrap(
            &mut self.store,
            move |caller: Caller<'_, *mut c_void>| -> Result<u64, wasmi::Error> {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe { c_call_0args(x.fn_pointer, caller.data().clone()) };
                return handle_trampoline_error(res);
            },
        );

        match self.linker.define(import_name, fn_name, func) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    pub fn link_function_1args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = AnnoyingBorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        let func = Func::wrap(
            &mut self.store,
            move |caller: Caller<'_, *mut c_void>, arg1: u64| -> Result<u64, wasmi::Error> {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe { c_call_1args(x.fn_pointer, caller.data().clone(), arg1) };
                return handle_trampoline_error(res);
            },
        );

        match self.linker.define(import_name, fn_name, func) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    pub fn link_function_2args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = AnnoyingBorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        let func = Func::wrap(
            &mut self.store,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64|
                  -> Result<u64, wasmi::Error> {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe { c_call_2args(x.fn_pointer, caller.data().clone(), arg1, arg2) };
                return handle_trampoline_error(res);
            },
        );

        match self.linker.define(import_name, fn_name, func) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    pub fn link_function_3args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = AnnoyingBorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        let func = Func::wrap(
            &mut self.store,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64,
                  arg3: u64|
                  -> Result<u64, wasmi::Error> {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res =
                    unsafe { c_call_3args(x.fn_pointer, caller.data().clone(), arg1, arg2, arg3) };
                return handle_trampoline_error(res);
            },
        );

        match self.linker.define(import_name, fn_name, func) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    pub fn link_function_4args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = AnnoyingBorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        let func = Func::wrap(
            &mut self.store,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64,
                  arg3: u64,
                  arg4: u64|
                  -> Result<u64, wasmi::Error> {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe {
                    c_call_4args(x.fn_pointer, caller.data().clone(), arg1, arg2, arg3, arg4)
                };
                return handle_trampoline_error(res);
            },
        );

        match self.linker.define(import_name, fn_name, func) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }

    pub fn link_function_5args(
        &mut self,
        fn_pointer: *mut c_void,
        import_name: &str,
        fn_name: &str,
    ) -> Result<(), Error> {
        let x = AnnoyingBorrowBypass {
            fn_pointer: fn_pointer.clone(),
        };

        let func = Func::wrap(
            &mut self.store,
            move |caller: Caller<'_, *mut c_void>,
                  arg1: u64,
                  arg2: u64,
                  arg3: u64,
                  arg4: u64,
                  arg5: u64|
                  -> Result<u64, wasmi::Error> {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe {
                    c_call_5args(
                        x.fn_pointer,
                        caller.data().clone(),
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                        arg5,
                    )
                };
                return handle_trampoline_error(res);
            },
        );

        match self.linker.define(import_name, fn_name, func) {
            Ok(_) => Ok(()),
            Err(err) => Err(err.into()),
        }
    }
}

enum WasmiInvokeError {
    None = 0,
    WasmiError = 1,
    FuncNExist = 2,
    InputError = 3, // input validation fails
    ReturnTypeError = 4,
    CallError = 5,
    HostError = 6,
    UnrecoverableSystemError = 7,
}

#[repr(C)]
pub struct WasmiInvokeResult {
    result: u64,
    error: u32,
    gas_consumed: u64,
}

#[repr(C)]
pub struct MemorySlice {
    mem: *mut u8,
    sz: u32,
}

#[no_mangle]
pub extern "C" fn wasmi_get_memory(runtime: *mut Wasmi_WasmRuntime) -> MemorySlice {
    let r = unsafe { &mut *runtime };

    match r.lazy_link() {
        Ok(_) => (),
        Err(_) => {
            return MemorySlice {
                mem: std::ptr::null_mut(),
                sz: 0,
            };
        }
    };

    match r
        .instance
        .as_mut()
        .expect("lazily linked")
        .get_memory(&r.store, "memory")
    {
        Some(mem) => {
            let mem_sz = mem.data_size(&r.store) as u32;
            // wasmi docs say that _any_ call to data_ptr or use of store
            // can invalidate returned pointers
            MemorySlice {
                mem: mem.data_ptr(&r.store),
                sz: mem_sz,
            }
        }
        _ => MemorySlice {
            mem: std::ptr::null_mut(),
            sz: 0,
        },
    }
}

#[no_mangle]
pub extern "C" fn wasmi_consume_gas(runtime: *mut Wasmi_WasmRuntime, gas_consumed: u64) -> bool {
    let r = unsafe { &mut *runtime };

    let cur_fuel: u64 = r.store.get_fuel().unwrap();
    if cur_fuel > gas_consumed {
        r.store.set_fuel(cur_fuel - gas_consumed).unwrap();
        return false;
    }
    r.store.set_fuel(0).unwrap();
    return true;
}

#[no_mangle]
pub extern "C" fn wasmi_get_available_gas(runtime: *const Wasmi_WasmRuntime) -> u64 {
    let r = unsafe { &*runtime };
    return r.store.get_fuel().unwrap();
}

fn string_from_parts(bytes: *const u8, len: u32) -> Result<String, Utf8Error> {
    let slice = unsafe { slice::from_raw_parts(bytes, len as usize) };
    match std::str::from_utf8(&slice) {
        Ok(s) => Ok(s.to_owned()),
        Err(x) => Err(x),
    }
}

#[no_mangle]
pub extern "C" fn wasmi_link_nargs(
    runtime: *mut Wasmi_WasmRuntime,
    module_name: *const u8,
    module_name_len: u32,
    method_name: *const u8,
    method_name_len: u32,
    function_pointer: *mut c_void,
    nargs: u8,
) -> bool // true if success
{
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

    let r = unsafe { &mut *runtime };

    let res = match nargs {
        0 => r.link_function_0args(function_pointer, &module, &method),
        1 => r.link_function_1args(function_pointer, &module, &method),
        2 => r.link_function_2args(function_pointer, &module, &method),
        3 => r.link_function_3args(function_pointer, &module, &method),
        4 => r.link_function_4args(function_pointer, &module, &method),
        5 => r.link_function_5args(function_pointer, &module, &method),
        _ => {
            return false;
        }
    };

    match res {
        Ok(_) => true,
        Err(_) => false,
    }
}

#[no_mangle]
pub extern "C" fn wasmi_invoke(
    runtime: *mut Wasmi_WasmRuntime,
    bytes: *const u8,
    bytes_len: u32,
    gas_limit: u64,
) -> WasmiInvokeResult {
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let string = match std::str::from_utf8(&slice) {
        Ok(v) => v,
        _ => {
            return WasmiInvokeResult {
                result: 0,
                error: WasmiInvokeError::InputError as u32,
                gas_consumed: 0,
            }
        }
    };

    let r = unsafe { &mut *runtime };

    match r.lazy_link() {
        Ok(_) => {}
        Err(_) => {
            return WasmiInvokeResult {
                result: 0,
                error: WasmiInvokeError::WasmiError as u32,
                gas_consumed: 0,
            };
        }
    };

    r.store.set_fuel(gas_limit).unwrap();

    let func = match r
        .instance
        .as_mut()
        .expect("lazily linked")
        .get_func(&r.store, string)
    {
        Some(v) => v,
        _ => {
            return WasmiInvokeResult {
                result: 0,
                error: WasmiInvokeError::FuncNExist as u32,
                gas_consumed: 0,
            };
        }
    };

    let mut res = [Val::I64(0)];

    let func_res = func.call(&mut r.store, &[], &mut res);

    let gas_remaining: u64 = r.store.get_fuel().unwrap();

    match func_res {
        Ok(_) => match res[0].i64() {
            Some(v) => {
                return WasmiInvokeResult {
                    result: v as u64,
                    error: WasmiInvokeError::None as u32,
                    gas_consumed: gas_limit - gas_remaining,
                };
            }
            _ => {
                return WasmiInvokeResult {
                    result: 0,
                    error: WasmiInvokeError::ReturnTypeError as u32,
                    gas_consumed: gas_limit - gas_remaining,
                };
            }
        },
        Err(err) => {
            let errstr = err.to_string();
            //janky but it works
            if errstr == "HostError" {
                return WasmiInvokeResult {
                    result: 0,
                    error: WasmiInvokeError::HostError as u32,
                    gas_consumed: gas_limit - gas_remaining,
                };
            }
            if errstr == "UnrecoverableSystemError" {
                return WasmiInvokeResult {
                    result: 0,
                    error: WasmiInvokeError::UnrecoverableSystemError as u32,
                    gas_consumed: gas_limit - gas_remaining,
                };
            }
            return WasmiInvokeResult {
                result: 0,
                error: WasmiInvokeError::CallError as u32,
                gas_consumed: gas_limit - gas_remaining,
            };
        }
    };
}

#[no_mangle]
pub extern "C" fn new_wasmi_context() -> *mut Wasmi_WasmContext {
    let b = Box::new(Wasmi_WasmContext::new());

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_wasmi_context(p: *mut Wasmi_WasmContext) {
    unsafe {
        drop(Box::from_raw(p));
    }
}

#[no_mangle]
pub fn new_wasmi_runtime(
    bytes: *const u8,
    bytes_len: u32,
    context: *mut Wasmi_WasmContext,
    userctx: *mut c_void,
) -> *mut Wasmi_WasmRuntime {
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let b = Box::new(Wasmi_WasmRuntime::new(
        unsafe { &*context },
        &slice,
        userctx,
    ));

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_wasmi_runtime(p: *mut Wasmi_WasmRuntime) {
    unsafe { drop(Box::from_raw(p)) };
}
