use makepad_stitch::{Engine, Linker, Module, Store, Instance, Func};

use std::rc::Rc;
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
        println!("create wasmcontext");
        Self {
            engine : Engine::new(),
        }
    }
}

impl Drop for Stitch_WasmContext {
    fn drop(&mut self) {
        println!("dropping wasmContext");
    }
}

#[allow(non_camel_case_types)]
pub struct Stitch_WasmRuntime {
    context : Rc<Stitch_WasmContext>,
    module : Module,
    store : Store,
    linker: Linker,
    userctx : *mut c_void,
    instance : Option<Instance>,
}

// required for panic handler
// impl UnwindSafe for Stitch_WasmRuntime {}
// impl RefUnwindSafe for Stitch_WasmRuntime {}

enum TrampolineError {
    None = 0,
    HostError = 1,
    UnrecoverableSystemError = 2,
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
        TrampolineError::None => (),
        TrampolineError::HostError => { panic!("HostError"); },
        _ => { panic!("UnrecoverableSystemError"); },
    }
}

impl Stitch_WasmRuntime {
    pub fn new(context: &Rc<Stitch_WasmContext>, bytes : &[u8], userctx : *mut c_void) -> Stitch_WasmRuntime {
        let store = Store::new(context.engine.clone());
        let module = Module::new(store.engine(), &bytes).unwrap();
        //let instance = Linker::new().instantiate(&mut store, &module).unwrap();

        println!("init");
        Self {
            context : context.clone(),
            module : module,
            store : store,
            linker: Linker::new(),
            userctx : userctx,
            instance : None
        }
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

        #[allow(unused_doc_comments)]
        /**
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

enum InvokeError {
    None = 0,
    StitchError = 1,
    FuncNExist = 2,
    InputError = 3, // input validation fails
    ReturnTypeError = 4,
    WasmError = 5,
    CallError = 6,
    UnrecoverableSystemError = 7
}

#[repr(C)]
pub struct InvokeResult {
    result : u64,
    error : u32
}

#[repr(C)]
pub struct MemorySlice {
    mem : *mut u8,
    sz : u32
}

#[no_mangle]
pub extern "C" fn get_memory(runtime : *mut Stitch_WasmRuntime) -> MemorySlice
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
    nargs : u8)
{
    let module = match string_from_parts(module_name, module_name_len) {
        Ok(x) => x,
        _ => {return;}
    };

    let method = match string_from_parts(method_name, method_name_len) {
        Ok(x) => x,
        _ => {return;}
    };

    let r = unsafe {&mut *runtime};

    match nargs {
        0 => {r.link_function_0args(function_pointer, &module, &method); },
        1 => {r.link_function_1args(function_pointer, &module, &method); },
        2 => {r.link_function_2args(function_pointer, &module, &method); },
        3 => {r.link_function_3args(function_pointer, &module, &method); },
        4 => {r.link_function_4args(function_pointer, &module, &method); },
        5 => {r.link_function_5args(function_pointer, &module, &method); },
        _ => {()},
    }
}

#[no_mangle]
pub extern "C" fn invoke(runtime : *mut Stitch_WasmRuntime, bytes: *const u8, bytes_len : u32) -> InvokeResult
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

    //match std::panic::catch_unwind(|| {
        let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

        let string = match std::str::from_utf8(&slice) {
            Ok(v) => v,
            _ => return InvokeResult { result : 0, error : InvokeError::InputError as u32 }
        };

        let r = unsafe {&mut *runtime};

        match r.lazy_link() {
            Ok(_) => {},
            Err(_) => { return InvokeResult {result : 0, error : InvokeError::StitchError as u32 }; },
        };

        let func = match r.instance.as_mut().expect("lazily linked").exported_func(string) {
            Some(v) => v,
            _ => { return InvokeResult { result : 0, error : InvokeError::FuncNExist as u32 }; }
        };

        let mut res = [Val::I64(0)];

        match func.call(&mut r.store, &[], &mut res) {
            Ok(_) => { 
                match res[0].to_i64() {
                    Some(v) => {return InvokeResult { result : v as u64, error : InvokeError::None as u32 }; },
                    _ => { return InvokeResult { result : 0, error : InvokeError::ReturnTypeError as u32 }; },
                }
            },
            Err(_) => {return InvokeResult { result : 0, error : InvokeError::WasmError as u32};}
        };
   /* }) {
        Ok(v) => { return v; },
        Err(_) => {
            return InvokeResult { result : 0, error : InvokeError::CallError as u32 };
        },
    } */
}

#[no_mangle]
pub extern "C" fn new_stitch_context() -> *mut Rc<Stitch_WasmContext> {
    let b = Box::new(Rc::<Stitch_WasmContext>::new(Stitch_WasmContext::new()));

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_stitch_context(p : *mut Rc<Stitch_WasmContext>) {

    unsafe { drop(Box::from_raw(p)); }
}

#[no_mangle]
pub fn new_stitch_runtime(bytes: *const u8, bytes_len : u32, context : *mut Rc<Stitch_WasmContext>, userctx : *mut c_void) -> *mut Stitch_WasmRuntime
{
    println!("params {:p} {} {:p}", bytes, bytes_len, context);
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    println!("make slice");

    let b = Box::new(Stitch_WasmRuntime::new( unsafe {&*context}, &slice, userctx));

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_stitch_runtime(p : *mut Stitch_WasmRuntime) {
    unsafe { drop(Box::from_raw(p)) };
}
