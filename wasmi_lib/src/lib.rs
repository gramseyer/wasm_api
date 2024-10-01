use core::ffi::c_void;

pub mod wasmi_context;
pub mod wasmtime_context;
pub mod external_call;
pub mod invoke_result;
pub mod wasmi_runtime;
pub mod wasmtime_runtime;
pub mod memory;
mod common;

use crate::wasmi_runtime::WasmiRuntime;
use crate::wasmtime_runtime::WasmtimeRuntime;

#[no_mangle]
pub extern "C" fn wasmi_consume_gas(
    runtime_void: *mut c_void,
    gas_consumed: u64,
) -> bool {
    assert!(runtime_void != core::ptr::null_mut());

    let runtime: *mut WasmiRuntime =
        unsafe { core::mem::transmute(runtime_void) };

    let r = unsafe { &mut *runtime };

    let cur_fuel: u64 = r.store.get_fuel().unwrap();
    if cur_fuel > gas_consumed {
        r.store.set_fuel(cur_fuel - gas_consumed).unwrap();
        return true;
    }
    r.store.set_fuel(0).unwrap();
    return false;
}

#[no_mangle]
pub extern "C" fn wasmi_get_available_gas(runtime_void: *const c_void) -> u64 {
    assert!(runtime_void != core::ptr::null());
    let runtime: *const WasmiRuntime =
        unsafe { core::mem::transmute(runtime_void) };
    let r = unsafe { &*runtime };
    return r.store.get_fuel().unwrap();
}

#[no_mangle]
pub extern "C" fn wasmi_set_available_gas(runtime_void: *mut c_void, gas: u64) {
    assert!(runtime_void != core::ptr::null_mut());
    let runtime: *mut WasmiRuntime =
        unsafe { core::mem::transmute(runtime_void) };
    let r = unsafe { &mut *runtime };
    r.store.set_fuel(gas).unwrap();
}

#[no_mangle]
pub extern "C" fn wasmtime_consume_gas(
    runtime_void: *mut c_void,
    gas_consumed: u64,
) -> bool {
    assert!(runtime_void != core::ptr::null_mut());

    let runtime: *mut WasmtimeRuntime =
        unsafe { core::mem::transmute(runtime_void) };

    let r = unsafe { &mut *runtime };

    let cur_fuel: u64 = r.store.get_fuel().unwrap();
    if cur_fuel > gas_consumed {
        r.store.set_fuel(cur_fuel - gas_consumed).unwrap();
        return true;
    }
    r.store.set_fuel(0).unwrap();
    return false;
}

#[no_mangle]
pub extern "C" fn wasmtime_get_available_gas(runtime_void: *const c_void) -> u64 {
    assert!(runtime_void != core::ptr::null());
    let runtime: *const WasmtimeRuntime =
        unsafe { core::mem::transmute(runtime_void) };
    let r = unsafe { &*runtime };
    return r.store.get_fuel().unwrap();
}

#[no_mangle]
pub extern "C" fn wasmtime_set_available_gas(runtime_void: *mut c_void, gas: u64) {
    assert!(runtime_void != core::ptr::null_mut());
    let runtime: *mut WasmtimeRuntime =
        unsafe { core::mem::transmute(runtime_void) };
    let r = unsafe { &mut *runtime };
    r.store.set_fuel(gas).unwrap();
}
