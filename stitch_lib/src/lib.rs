use makepad_stitch::{Engine, Linker, Module, Store, Instance};

use std::rc::Rc;
use std::slice;

use makepad_stitch::Val;

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
    instance : Option<Instance>,
}

impl Stitch_WasmRuntime {
    pub fn new(context: &Rc<Stitch_WasmContext>, bytes : &[u8]) -> Stitch_WasmRuntime {
        let store = Store::new(context.engine.clone());
        let module = Module::new(store.engine(), &bytes).unwrap();
        //let instance = Linker::new().instantiate(&mut store, &module).unwrap();

        println!("init");
        Self {
            context : context.clone(),
            module : module,
            store : store,
            linker: Linker::new(),
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

#[no_mangle]
pub extern "C" fn invoke(runtime : *mut Stitch_WasmRuntime, bytes: *const u8, bytes_len : u32) -> InvokeResult
{
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    let string = match std::str::from_utf8(&slice) {
        Ok(v) => v,
        _ => return InvokeResult { result : 0, error : true }
    };

    let r = unsafe {&mut *runtime};

    match r.lazy_link() {
        Ok(_) => {},
        Err(_) => { return InvokeResult {result : 0, error : true }; },
    };

    let func = match r.instance.as_mut().expect("lazily linked").exported_func(string) {
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
    let b = Box::new(Rc::<Stitch_WasmContext>::new(Stitch_WasmContext::new()));

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_stitch_context(p : *mut Rc<Stitch_WasmContext>) {

    unsafe { drop(Box::from_raw(p)); }
}

#[no_mangle]
pub fn new_stitch_runtime(bytes: *const u8, bytes_len : u32, context : *mut Rc<Stitch_WasmContext>) -> *mut Stitch_WasmRuntime
{
    println!("params {:p} {} {:p}", bytes, bytes_len, context);
    let slice = unsafe { slice::from_raw_parts(bytes, bytes_len as usize) };

    println!("make slice");

    let b = Box::new(Stitch_WasmRuntime::new( unsafe {&*context}, &slice));

    return Box::into_raw(b);
}

#[no_mangle]
pub extern "C" fn free_stitch_runtime(p : *mut Stitch_WasmRuntime) {
    unsafe { drop(Box::from_raw(p)) };
}
