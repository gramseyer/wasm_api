use core::ffi::c_void;
use core::slice;
use core::str::Utf8Error;

// Tells rust that it is safe to send this c_void pointer
// across lambda boundaries
#[derive(Clone)]
pub struct BorrowBypass {
    pub fn_pointer: *mut c_void,
}

unsafe impl Send for BorrowBypass {}
unsafe impl Sync for BorrowBypass {}

pub fn string_from_parts(bytes: *const u8, len: u32) -> Result<String, Utf8Error> {
    let slice = unsafe { slice::from_raw_parts(bytes, len as usize) };
    match std::str::from_utf8(&slice) {
        Ok(s) => Ok(s.to_owned()), // TODO check this is a copy and not something weird
        Err(x) => Err(x),
    }
}

#[repr(u8)]
pub enum WasmValueType {
    VOID = 0,
    U64 = 1
}

impl WasmValueType {
    pub fn from_u8(input : u8) -> Option<Self> {
        match input {
            x if x == WasmValueType::VOID as u8 => {return Some(WasmValueType::VOID);},
            x if x == WasmValueType::U64 as u8 => {return Some(WasmValueType::U64);},
            _ => {return None;},
        }
    }
}
