use core::ffi::c_void;

use wasmi::core::HostError;
use std::error::Error;

use core::fmt;

/**
 * Error policy:
 * Syscalls should not be handled as "errors" returned from the syscall;
 * instead, syscalls return a uint64 that acts as the error code
 * (which the SDK can then check and call `unreachable`).
 *
 * There are two types of errors that we are concerned with here.
 * 1) System errors (which could be nondeterministic, i.e. malloc fail)
 * at which point we need to abort the whole transaction.
 * 2) OutOfGas errors, within the syscall.  We'd like to just return
 * an OutOfGas exception into wasmi, but wasmi does not expose
 * that as a public API, so we return a different error instead if
 * a syscall causes an OutOfGas error.
 * (The syscall will set the fuel to zero before returning in that case.
 * However, we do not have a guarantee that another instruction will execute
 * before the wasm function that called the syscall returns).
 * I'm using Wasmi's HostError API for this extra type of error.
 *
 */

#[allow(non_camel_case_types)]
#[allow(dead_code)]
#[repr(u8)]
#[derive(Clone,Copy,Debug)]
pub enum HostFnError {
    NONE_OR_RECOVERABLE = 0,
    RETURN_SUCCESS = 1, // technically "success", but terminates the caller wasm instance
    OUT_OF_GAS = 2,
    UNRECOVERABLE = 3
}

#[repr(C)]
pub struct TrampolineResult {
    pub result: u64,
    pub panic: u8, // HostFnError
}

#[derive(Debug, Copy, Clone)]
pub struct TrampolineError {
    pub error: HostFnError,
}

impl fmt::Display for TrampolineError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "TrampolineError res={}", self.error as u8)
    }
}

impl Error for TrampolineError {}

impl HostError for TrampolineError {}

extern "C" {
    pub fn c_call_0args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
    ) -> TrampolineResult;
    pub fn c_call_1args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
    ) -> TrampolineResult;
    pub fn c_call_2args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
    ) -> TrampolineResult;
    pub fn c_call_3args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
    ) -> TrampolineResult;
    pub fn c_call_4args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
        arg4: u64,
    ) -> TrampolineResult;
    pub fn c_call_5args(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
        arg4: u64,
        arg5: u64,
    ) -> TrampolineResult;

    pub fn c_call_0args_noret(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
    ) -> TrampolineResult;
    pub fn c_call_1args_noret(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
    ) -> TrampolineResult;
    pub fn c_call_2args_noret(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
    ) -> TrampolineResult;
    pub fn c_call_3args_noret(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
    ) -> TrampolineResult;
    pub fn c_call_4args_noret(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
        arg4: u64,
    ) -> TrampolineResult;
    pub fn c_call_5args_noret(
        fn_pointer: *mut c_void,
        userctx: *mut c_void,
        arg1: u64,
        arg2: u64,
        arg3: u64,
        arg4: u64,
        arg5: u64,
    ) -> TrampolineResult;
}
