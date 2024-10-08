use core::ffi::c_void;

use crate::wasmi_runtime::WasmiRuntime;
use crate::wasmtime_runtime::WasmtimeRuntime;
use crate::stitch_runtime::Stitch_WasmRuntime;

#[repr(C)]
pub struct MemorySlice {
    mem: *mut u8,
    sz: u32,
}

#[no_mangle]
pub extern "C" fn wasmi_get_memory(runtime_void: *mut c_void) -> MemorySlice {
    assert!(runtime_void != core::ptr::null_mut());

    let runtime: *mut WasmiRuntime =
        unsafe { core::mem::transmute(runtime_void) };

    let r = unsafe { &mut *runtime };

    match r.instance.get_memory(&r.store, "memory") {
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
pub extern "C" fn wasmtime_get_memory(runtime_void: *mut c_void) -> MemorySlice {
    assert!(runtime_void != core::ptr::null_mut());

    let runtime: *mut WasmtimeRuntime =
        unsafe { core::mem::transmute(runtime_void) };

    let r = unsafe { &mut *runtime };

    match r.instance.get_memory(&mut r.store, "memory") {
        Some(mem) => {
            let mem_sz = mem.data_size(&r.store) as u32;
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
pub extern "C" fn stitch_get_memory(runtime_void : *mut c_void) -> MemorySlice
{
    assert!(runtime_void != core::ptr::null_mut());

    let runtime: *mut Stitch_WasmRuntime =
        unsafe { core::mem::transmute(runtime_void) };

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
            MemorySlice {
                mem : std::ptr::null_mut(),
                sz : 0
            }
        }
    }
}