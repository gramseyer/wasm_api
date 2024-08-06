use makepad_stitch::{Engine, Linker, Module, Store, Instance};

use std::rc::Rc;
use core::ptr;
use core::mem::ManuallyDrop;
use std::slice;

use makepad_stitch::Val;

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
    context : Rc<Stitch_WasmContext>,
    module : Module,
    store : Store,
    instance : Instance,
}

impl Stitch_WasmRuntime {
    pub fn new(context: &Rc<Stitch_WasmContext>, bytes : &[u8]) -> Stitch_WasmRuntime {
        let mut store = Store::new(context.engine.clone());
        let module = Module::new(store.engine(), &bytes).unwrap();
        let instance = Linker::new().instantiate(&mut store, &module).unwrap();

        Self {
            context : context.clone(),
            module : module,
            store : store,
            instance : instance
        }
    }
}

#[repr(C)]
pub struct InvokeResult {
    result : u64,
    error : bool
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

    match r.instance.exported_mem("memory") {
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

#[no_mangle]
pub extern "C" fn invoke(runtime : *mut Stitch_WasmRuntime, bytes: *const u8, bytes_len : u32) -> InvokeResult
{
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let string = match std::str::from_utf8(&slice) {
        Ok(v) => v,
        _ => return InvokeResult { result : 0, error : true }
    };

    let r = unsafe {&mut *runtime};

    let func = match r.instance.exported_func(string) {
        Some(v) => v,
        _ => { return InvokeResult { result : 0, error : true }; }
    };

    let mut res = [Val::I64(0)];

    match func.call(&mut r.store, &[], &mut res) {
        Ok(_) => {},
        Err(_) => {return InvokeResult { result : 0, error : true};}
    };

    match res[0].to_i64() {
        Some(v) => InvokeResult { result : v as u64, error : false },
        _ => InvokeResult { result : 0, error : true }
    }
}

#[no_mangle]
pub extern "C" fn new_stitch_context() -> *mut Rc<Stitch_WasmContext> {
    let out = std::mem::ManuallyDrop::new(
        Rc::<Stitch_WasmContext>::new(Stitch_WasmContext::new()));

    let o : *mut Rc<Stitch_WasmContext> = &mut ManuallyDrop::into_inner(out);
    return o;
}

#[no_mangle]
pub extern "C" fn free_stitch_context(p : *mut Rc<Stitch_WasmContext>) {
    unsafe { ptr::drop_in_place(p) };
}

#[no_mangle]
pub fn new_stitch_runtime(bytes: *const u8, bytes_len : u32, context : *mut Rc<Stitch_WasmContext>) -> *mut Stitch_WasmRuntime
{
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let out = std::mem::ManuallyDrop::new(
        Stitch_WasmRuntime::new(unsafe {&*context}, &slice));

    let o : *mut Stitch_WasmRuntime = &mut ManuallyDrop::into_inner(out);
    return o;
}

#[no_mangle]
pub extern "C" fn free_stitch_runtime(p : *mut Stitch_WasmRuntime) {
    unsafe { ptr::drop_in_place(p) };
}
