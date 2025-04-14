#pragma once

#include <cstdint>

extern "C"
{
    // Returned from a host function:
    // panic has type HostFnError
    struct TrampolineResult
    {
        uint64_t result;
        uint8_t panic;
    };

    // trampolines
    TrampolineResult c_call_0args(void* function_pointer,
                                  void* host_call_context) noexcept;
    TrampolineResult c_call_1args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1) noexcept;
    TrampolineResult c_call_2args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2) noexcept;
    TrampolineResult c_call_3args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3) noexcept;
    TrampolineResult c_call_4args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4) noexcept;
    TrampolineResult c_call_5args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5) noexcept;
    TrampolineResult c_call_6args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6) noexcept;
    TrampolineResult c_call_7args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7) noexcept;
    TrampolineResult c_call_8args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7,
                                  uint64_t arg8) noexcept;
    TrampolineResult c_call_0args_noret(void* function_pointer,
                                  void* host_call_context) noexcept;
    TrampolineResult c_call_1args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1) noexcept;
    TrampolineResult c_call_2args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2) noexcept;
    TrampolineResult c_call_3args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3) noexcept;
    TrampolineResult c_call_4args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4) noexcept;
    TrampolineResult c_call_5args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5) noexcept;
    TrampolineResult c_call_6args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6) noexcept;
    TrampolineResult c_call_7args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7) noexcept;
    TrampolineResult c_call_8args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7,
                                  uint64_t arg8) noexcept;
}
