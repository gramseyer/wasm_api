
use makepad_stitch::{Engine};

use core::ffi::c_void;

#[allow(non_camel_case_types)]
pub struct Stitch_WasmContext {
    pub engine : Engine,
}

impl Stitch_WasmContext {
    fn new() -> Stitch_WasmContext {
        Self {
            engine : Engine::new(),
        }
    }
}

// Rust FFI needs no_mangle and extern "C"
#[no_mangle]
pub extern "C" fn new_stitch_context() -> *mut c_void {
    let b = Box::new(Stitch_WasmContext::new());

    return unsafe { core::mem::transmute(Box::into_raw(b)) };
}

#[no_mangle]
pub extern "C" fn free_stitch_context(p: *mut c_void) {
    assert!(p != core::ptr::null_mut());

    let runtime: *mut Stitch_WasmContext = unsafe { core::mem::transmute(p) };

    unsafe { drop(Box::from_raw(runtime)) };
}
