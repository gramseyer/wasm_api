use makepad_stitch::{Linker, Module, Store, Instance, Func, Val};

use crate::common::{string_from_parts, WasmValueType};

use core::ffi::c_void;
use core::slice;

use crate::external_call;
use crate::external_call::{TrampolineResult, HostFnError};

use crate::stitch_context::Stitch_WasmContext;
use crate::invoke_result::{FFIInvokeResult, InvokeError};

#[allow(non_camel_case_types)]
pub struct Stitch_WasmRuntime {
    module : Module,
    pub store : Store,
    linker: Linker,
    userctx : *mut c_void,
    pub instance : Option<Instance>,
}

fn stitch_handle_trampoline_error(result : &TrampolineResult) {
    match unsafe {std::mem::transmute(result.panic)} {
        HostFnError::NONE_OR_RECOVERABLE => (),
        HostFnError::RETURN_SUCCESS => { panic!("Return success"); },
        HostFnError::OUT_OF_GAS => { panic!("out of gas"); },
        HostFnError::DETERMINISTIC_ERROR => { panic!("deterministic error"); },
        _ => {panic!("Unrecoverable") },
    }
}

#[derive(Clone)]
pub struct AnnoyingBorrowBypass {
    fn_pointer : *mut c_void,
    userctx : *mut c_void
}

unsafe impl Send for AnnoyingBorrowBypass {}
unsafe impl Sync for AnnoyingBorrowBypass {}


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
                let res = unsafe { external_call::c_call_0args(x.fn_pointer, x.userctx) };
                stitch_handle_trampoline_error(&res);
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
                    external_call::c_call_1args(x.fn_pointer, x.userctx, arg1)
                };
                stitch_handle_trampoline_error(&res);
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
                    external_call::c_call_2args(x.fn_pointer, x.userctx, arg1, arg2)
                };
                stitch_handle_trampoline_error(&res);
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
                    external_call::c_call_3args(x.fn_pointer, x.userctx, arg1, arg2, arg3)
                };
                stitch_handle_trampoline_error(&res);
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
                    external_call::c_call_4args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4)
                };
                stitch_handle_trampoline_error(&res);
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
                    external_call::c_call_5args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5)
                };
                stitch_handle_trampoline_error(&res);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_6args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64, arg6: u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_6args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5, arg6)
                };
                stitch_handle_trampoline_error(&res);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_7args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64, arg6: u64, arg7: u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_7args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
                };
                stitch_handle_trampoline_error(&res);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_8args(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64, arg6: u64, arg7: u64, arg8: u64| -> u64 {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_8args(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
                };
                stitch_handle_trampoline_error(&res);
                return res.result;
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_0args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
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
            Func::wrap(&mut self.store, move || -> ()  {
                // This is necessary to stop some part of rust
                // from complaining
                let _y = x.clone();
                let res = unsafe { external_call::c_call_0args_noret(x.fn_pointer, x.userctx) };
                stitch_handle_trampoline_error(&res);
            });
        

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_1args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_1args_noret(x.fn_pointer, x.userctx, arg1)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_2args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_2args_noret(x.fn_pointer, x.userctx, arg1, arg2)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_3args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2: u64, arg3: u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_3args_noret(x.fn_pointer, x.userctx, arg1, arg2, arg3)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_4args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3 : u64, arg4 : u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_4args_noret(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_5args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_5args_noret(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_6args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64, arg6: u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_6args_noret(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5, arg6)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_7args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64, arg6: u64, arg7: u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_7args_noret(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
    }

    pub fn link_function_8args_noret(&mut self, fn_pointer : *mut c_void, import_name : &str, fn_name : &str)
    {
        let x = AnnoyingBorrowBypass {
            fn_pointer : fn_pointer.clone(),
            userctx : self.userctx.clone(),
        };


        let func =
            Func::wrap(&mut self.store, move |arg1: u64, arg2 : u64, arg3:u64, arg4 : u64, arg5: u64, arg6: u64, arg7: u64, arg8: u64| -> () {
                let _y = x.clone();
                let res = unsafe {
                    external_call::c_call_8args_noret(x.fn_pointer, x.userctx, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
                };
                stitch_handle_trampoline_error(&res);
            });

        self.linker.define(import_name, fn_name, func);
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
    nargs : u8,
    ret_type: u8) -> bool
{
    let module = match string_from_parts(module_name, module_name_len) {
        Ok(x) => x,
        _ => {return false;}
    };

    let method = match string_from_parts(method_name, method_name_len) {
        Ok(x) => x,
        _ => {return false;}
    };

    let ret_type_enum = match WasmValueType::from_u8(ret_type) {
        None => {return false;},
        Some(x) => x
    };

    let r = unsafe {&mut *runtime};

    match ret_type_enum {
        WasmValueType::U64 => {
            match nargs {
                0 => r.link_function_0args(function_pointer, &module, &method),
                1 => r.link_function_1args(function_pointer, &module, &method),
                2 => r.link_function_2args(function_pointer, &module, &method),
                3 => r.link_function_3args(function_pointer, &module, &method),
                4 => r.link_function_4args(function_pointer, &module, &method),
                5 => r.link_function_5args(function_pointer, &module, &method),
                6 => r.link_function_6args(function_pointer, &module, &method),
                7 => r.link_function_7args(function_pointer, &module, &method),
                8 => r.link_function_8args(function_pointer, &module, &method),
                _ => {
                    return false;
                }
            }
        },
        WasmValueType::VOID => {
            match nargs {
                0 => r.link_function_0args_noret(function_pointer, &module, &method),
                1 => r.link_function_1args_noret(function_pointer, &module, &method),
                2 => r.link_function_2args_noret(function_pointer, &module, &method),
                3 => r.link_function_3args_noret(function_pointer, &module, &method),
                4 => r.link_function_4args_noret(function_pointer, &module, &method),
                5 => r.link_function_5args_noret(function_pointer, &module, &method),
                6 => r.link_function_6args_noret(function_pointer, &module, &method),
                7 => r.link_function_7args_noret(function_pointer, &module, &method),
                8 => r.link_function_8args_noret(function_pointer, &module, &method),
                _ => {
                    return false;
                }
            }
        },
    };
    return true;
}

#[no_mangle]
pub fn new_stitch_runtime(bytes: *const u8, bytes_len : u32, context_void : *mut c_void, userctx : *mut c_void) -> *mut c_void
{
	assert!(context_void != core::ptr::null_mut());

	let context : *mut Stitch_WasmContext = unsafe { core::mem::transmute(context_void)};

    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    match Stitch_WasmRuntime::new( unsafe{&*context}, &slice, userctx) {
        Some(r) => {
            let b = Box::new(r);
    		return unsafe { core::mem::transmute(Box::into_raw(b)) };
        },
        None => {
            return core::ptr::null_mut();
        }
    }
}

#[no_mangle]
pub extern "C" fn free_stitch_runtime(p : *mut c_void) {
	assert!(p != core::ptr::null_mut());

	let runtime : *mut Stitch_WasmRuntime = unsafe { core::mem::transmute(p)};

    unsafe { drop(Box::from_raw(runtime)) };
}

#[no_mangle]
pub extern "C" fn stitch_invoke(runtime_void : *mut c_void, bytes: *const u8, bytes_len : u32) -> FFIInvokeResult
{
	assert!(runtime_void != core::ptr::null_mut());

	let runtime : *mut Stitch_WasmRuntime = unsafe { core::mem::transmute(runtime_void)};

    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let string = match std::str::from_utf8(&slice) {
        Ok(v) => v,
        _ => { return FFIInvokeResult::error(
                InvokeError::DETERMINISTIC_ERROR,
             );
    	},
    };

    let r = unsafe {&mut *runtime};

    match r.lazy_link() {
        Ok(_) => {},
        Err(_) => { 
        	return FFIInvokeResult::error(
                InvokeError::DETERMINISTIC_ERROR,
            ); 
        },
    };

    let func = match r.instance.as_mut().expect("lazily linked").exported_func(string) {
        Some(v) => v,
        _ => { 
        	return FFIInvokeResult::error(
                InvokeError::DETERMINISTIC_ERROR,
            );
        }
    };

    let mut res = [Val::I64(0)];

    match func.call(&mut r.store, &[], &mut res) {
        Ok(_) => { 
            match res[0].to_i64() {
                Some(v) => { return FFIInvokeResult::success(v as u64); },
                _ => { return FFIInvokeResult::error(
                			InvokeError::DETERMINISTIC_ERROR,
            			); 
           		},
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
        Err(_) => {return FFIInvokeResult::error(
                			InvokeError::DETERMINISTIC_ERROR,
            			); 
    	},
    };
}


