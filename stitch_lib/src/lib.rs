use makepad_stitch::{Engine, Linker, Module, Store, Instance, Func};

use std::slice;
use core::ffi::c_void;

use makepad_stitch::Val;

use core::str::Utf8Error;

#[allow(non_camel_case_types)]
pub struct Stitch_WasmContext {
    engine : Engine,
}

impl Stitch_WasmContext {
    pub fn new() -> Stitch_WasmContext {
        Self {
            engine : Engine::new(),
        }
    }
}

#[allow(non_camel_case_types)]
pub struct Stitch_WasmRuntime {
    module : Module,
    store : Store,
    linker: Linker,
    userctx : *mut c_void,
    instance : Option<Instance>,
}

// required for panic handler
// impl UnwindSafe for Stitch_WasmRuntime {}
// impl RefUnwindSafe for Stitch_WasmRuntime {}

#[allow(non_camel_case_types)]
#[repr(u8)]
enum HostFnError {
    NONE_OR_RECOVERABLE = 0,
    RETURN_SUCCESS = 1, // technically "success", but terminates the caller wasm instance
    OUT_OF_GAS = 2,
    UNRECOVERABLE = 3,
}


#[repr(C)]
pub struct TrampolineResult {
    result : u64,
    panic : u8,
}

extern "C" {
    fn c_call_0args(fn_pointer : *mut c_void, userctx : *mut c_void) -> TrampolineResult;
    fn c_call_1args(fn_pointer : *mut c_void, userctx : *mut c_void, arg1 : u64) -> TrampolineResult;
    fn c_call_2args(fn_pointer : *mut c_void, userctx : *mut c_void, arg1 : u64, arg2 : u64) -> TrampolineResult;
    fn c_call_3args(fn_pointer : *mut c_void, userctx : *mut c_void, arg1 : u64, arg2 : u64, arg3 : u64) -> TrampolineResult;
    fn c_call_4args(fn_pointer : *mut c_void, userctx : *mut c_void, arg1 : u64, arg2 : u64, arg3 : u64, arg4 : u64) -> TrampolineResult;
    fn c_call_5args(fn_pointer : *mut c_void, userctx : *mut c_void, arg1 : u64, arg2 : u64, arg3 : u64, arg4 : u64, arg5 : u64) -> TrampolineResult;
}

#[derive(Clone)]
pub struct AnnoyingBorrowBypass {
    fn_pointer : *mut c_void,
    userctx : *mut c_void
}

unsafe impl Send for AnnoyingBorrowBypass {}
unsafe impl Sync for AnnoyingBorrowBypass {}

fn handle_trampoline_error(panic : u8) {
    match unsafe {std::mem::transmute(panic)} {
        HostFnError::NONE_OR_RECOVERABLE => (),
        HostFnError::RETURN_SUCCESS => { panic!("Return success"); },
        HostFnError::OUT_OF_GAS => { panic!("out of gas"); },
        _ => {panic!("Unrecoverable") },
    }
}

impl Stitch_WasmRuntime {
    pub fn new(context: &Stitch_WasmContext, bytes : &[u8], userctx : *mut c_void) -> Option<Stitch_WasmRuntime> {
        let store = Store::new(context.engine.clone());
        let module = Module::new(store.engine(), &bytes).ok()?;
        Some(Self {
            module : module,
            store : store,
            linker: Linker::new(),
            userctx : userctx,
            instance : None
        })
    }
    pub fn lazy_link(&mut self) -> Result<(), makepad_stitch::Error>{
        match self.instance {
            Some(_) => {return Ok(()); },
            None => {
                match self.linker.instantiate(&mut self.store, &self.module) {
                    Ok(inst) => {
                        self.instance = Some(inst);
                        return Ok(());
                    },
                    Err(x) => Err(x)
                }
            }
        }
    }

    pub fn link_function_0args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };

        /*
         * Stitch very frustratingly does not support returning a Result from one of these calls.
         * The IntoFunc trait requires that the output be a HostResult, which requires HostVal, which is only instantiated
         * for basic wasm types (i.e. i32, u64...).  The trait isn't exported.  _sigh_.
         */

        let func =
            Func::wrap(&mut self.store, move || -> u64  {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe { c_call_0args(x.fn_pointer, x.userctx) };
                handle_trampoline_error(res.panic);
                return res.result;
            });
        

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_1args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    c_call_1args(x.fn_pointer, x.userctx, arg1)
                };
                handle_trampoline_error(res.panic);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_2args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    c_call_2args(x.fn_pointer, x.userctx, arg1, arg2)
                };
                handle_trampoline_error(res.panic);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_3args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2: u64, arg3: u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    c_call_3args(x.fn_pointer, x.userctx, arg1, arg2, arg3)
                };
                handle_trampoline_error(res.panic);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_4args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3 : u64, arg4 : u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    c_call_4args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4)
                };
                handle_trampoline_error(res.panic);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_5args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    c_call_5args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5)
                };
                handle_trampoline_error(res.panic);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }
}

enum StitchInvokeError {
    None = 0,
    StitchError = 1,
    FuncNExist = 2,
    InputError = 3, // input validation fails
    ReturnTypeError = 4,
    WasmError = 5,
    UnrecoverableSystemError = 6
}

#[repr(C)]
pub struct StitchInvokeResult {
    result : u64,
    error : u32
}

#[repr(C)]
pub struct MemorySlice {
    mem : *mut u8,
    sz : u32
}

#[no_mangle]
pub extern "C" fn stitch_get_memory(runtime : *mut Stitch_WasmRuntime) -> MemorySlice
{
    let r = unsafe { &mut *runtime };

    match r.lazy_link() {
        Ok(_) => (),
        Err(_) => { return MemorySlice {
            mem: std::ptr::null_mut(),
            sz : 0
        };}
    };

    match r.instance.as_mut().expect("lazily linked").exported_mem("memory") {
        Some(mem) => {
            let slice = mem.bytes_mut(&mut r.store);
            MemorySlice {
                mem : slice.as_mut_ptr(),
                sz : slice.len() as u32
            }
        },
        _ => {
            println!("no memory found");
            MemorySlice {
                mem : std::ptr::null_mut(),
                sz : 0
            }
        }
    }
}

fn string_from_parts(bytes : *const u8, len : u32) -> Result<String, Utf8Error> {
    let slice = unsafe { slice::from_raw_parts(bytes, len as usize) };
    match std::str::from_utf8(&slice) {
        Ok(s) => Ok(s.to_owned()),
        Err(x) => Err(x)
    }
}

#[no_mangle]
pub extern "C" fn stitch_link_nargs(
    runtime : *mut Stitch_WasmRuntime, 
    module_name: *const u8, 
    module_name_len : u32,
    method_name : *const u8,
    method_name_len : u32,
    function_pointer: *mut c_void,
    nargs : u8) -> bool
{
    let module = match string_from_parts(module_name, module_name_len) {
        Ok(x) => x,
        _ => {return false;}
    };

    let method = match string_from_parts(method_name, method_name_len) {
        Ok(x) => x,
        _ => {return false;}
    };

    let r = unsafe {&mut *runtime};

    match nargs {
        0 => {r.link_function_0args(function_pointer, &module, &method); },
        1 => {r.link_function_1args(function_pointer, &module, &method); },
        2 => {r.link_function_2args(function_pointer, &module, &method); },
        3 => {r.link_function_3args(function_pointer, &module, &method); },
        4 => {r.link_function_4args(function_pointer, &module, &method); },
        5 => {r.link_function_5args(function_pointer, &module, &method); },
        _ => {return false; },
    }
    return true;
}

#[no_mangle]
pub extern "C" fn stitch_invoke(runtime : *mut Stitch_WasmRuntime, bytes: *const u8, bytes_len : u32) -> StitchInvokeResult
{
    /*
    All I want to do is just catch the panic, return, and promise I'll NEVER EVERY TOUCH THIS MEMORY AGAIN
    If rust wants to clean up, fine, but if it wants to leak memory, also fine.
    This is just for some experimental tests!  I hate writing FFIs.

    I can go through and make a panic handler and suffer not knowing whether
    this is even correct.

    BUT NO

    Rustc seems to link against the wrong libc version on my mac.
    So any panic causes a "failed panic" error (!?)

    ```
    fatal runtime error: failed to initiate panic, error 5
    ```

    Thank u

    i'm so mad
    */

    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let string = match std::str::from_utf8(&slice) {
        Ok(v) => v,
        _ => return StitchInvokeResult { result : 0, error : StitchInvokeError::InputError as u32 }
    };

    let r = unsafe {&mut *runtime};

    match r.lazy_link() {
        Ok(_) => {},
        Err(_) => { return StitchInvokeResult {result : 0, error : StitchInvokeError::StitchError as u32 }; },
    };

    let func = match r.instance.as_mut().expect("lazily linked").exported_func(string) {
        Some(v) => v,
        _ => { return StitchInvokeResult { result : 0, error : StitchInvokeError::FuncNExist as u32 }; }
    };

    let mut res = [Val::I64(0)];

    match func.call(&mut r.store, &[], &mut res) {
        Ok(_) => { 
            match res[0].to_i64() {
                Some(v) => {return StitchInvokeResult { result : v as u64, error : StitchInvokeError::None as u32 }; },
                _ => { return StitchInvokeResult { result : 0, error : StitchInvokeError::ReturnTypeError as u32 }; },
            }
        },
        /*
         * 
         * Errors here are mismatched function types,
         * or anything that comes out of executing wasm.
         * My (limited) understanding from skimmimg the codebase
         * is that errors would be from (legitimate, deterministic) wasm errors,
         * and that allocation errors (the only source of nondeterminism of which I am aware, in this codebase)
         * just become rust panic()s.
         * 
         * But additional checks on the type of the returned error could be applied, if necessary.
         */
        Err(_) => {return StitchInvokeResult { result : 0, error : StitchInvokeError::WasmError as u32};}
    };
}

#[no_mangle]
pub extern "C" fn new_stitch_context() -> *mut Stitch_WasmContext {
    let b = Box::new(Stitch_WasmContext::new());

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_stitch_context(p : *mut Stitch_WasmContext) {

    unsafe { drop(Box::from_raw(p)); }
}

#[no_mangle]
pub fn new_stitch_runtime(bytes: *const u8, bytes_len : u32, context : *mut Stitch_WasmContext, userctx : *mut c_void) -> *mut Stitch_WasmRuntime
{
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    match Stitch_WasmRuntime::new( unsafe{&*context}, &slice, userctx) {
        Some(r) => {
            let b = Box::new(r);
            return Box::into_raw(b);
        },
        None => {
            return core::ptr::null_mut();
        }
    }
}

#[no_mangle]
pub extern "C" fn free_stitch_runtime(p : *mut Stitch_WasmRuntime) {
    unsafe { drop(Box::from_raw(p)) };
}
