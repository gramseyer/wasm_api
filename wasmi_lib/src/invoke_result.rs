
use crate::external_call;

// Everything but UnrecoverableSystemError
// is a deterministic error that can be handled by a smart
// contract (i.e. in a try-catch handler).
#[repr(u8)]
#[allow(non_camel_case_types)]
pub enum InvokeError {
  NONE = 0,
  DETERMINISTIC_ERROR = 1,
  OUT_OF_GAS_ERROR = 2,
  UNRECOVERABLE = 3,
  RETURN = 4,
}

#[repr(C)]
pub struct FFIInvokeResult {
    result: u64,
    error: u8, // InvokeError
}

impl FFIInvokeResult {
    pub fn error(err: InvokeError) -> FFIInvokeResult {
        Self {
            result: 0,
            error: err as u8,
        }
    }

    pub fn success(res: u64) -> FFIInvokeResult {
        Self {
            result: res,
            error: InvokeError::NONE as u8,
        }
    }

    pub fn from_host_error(err: external_call::HostFnError) -> FFIInvokeResult {
        match err {
            external_call::HostFnError::NONE_OR_RECOVERABLE => { panic!("should call success()"); },
            external_call::HostFnError::RETURN_SUCCESS => {
                return FFIInvokeResult::error(
                    InvokeError::RETURN);
            },
            external_call::HostFnError::OUT_OF_GAS => {
                return FFIInvokeResult::error(
                    InvokeError::OUT_OF_GAS_ERROR);
            },
            external_call::HostFnError::UNRECOVERABLE => {
                return FFIInvokeResult::error(
                    InvokeError::UNRECOVERABLE);
            }
        }
    }
}
