#include "wasm_api/ffi_trampolines.h"

#include "wasm_api/error.h"

#include <utility>
#include <cstdio>

namespace detail
{
TrampolineResult
no_error(uint64_t res)
{
    return TrampolineResult{ res, static_cast<uint8_t>(wasm_api::HostFnError::NONE_OR_RECOVERABLE) };
}

TrampolineResult
unrecoverable_error() {
    return TrampolineResult{ 0, static_cast<uint8_t>(wasm_api::HostFnError::UNRECOVERABLE) };
}

TrampolineResult
out_of_gas() {
    return TrampolineResult{ 0, static_cast<uint8_t>(wasm_api::HostFnError::OUT_OF_GAS) };
}

TrampolineResult
return_success() {
    return TrampolineResult{ 0, static_cast<uint8_t>(wasm_api::HostFnError::RETURN_SUCCESS) };
}

TrampolineResult
deterministic_error() {
    return TrampolineResult{ 0, static_cast<uint8_t>(wasm_api::HostFnError::DETERMINISTIC_ERROR) };
}

TrampolineResult handle_result(wasm_api::HostFnStatus<uint64_t> result) {
    if (result) {
        return no_error(*result);
    }
    switch(result.error()) {
        case wasm_api::HostFnError::OUT_OF_GAS:
            return out_of_gas();
        case wasm_api::HostFnError::RETURN_SUCCESS:
            return return_success();
        case wasm_api::HostFnError::DETERMINISTIC_ERROR:
            return deterministic_error();
        default:
            std::printf("unrecoverable error!\n");
            return unrecoverable_error();
    }
}

TrampolineResult handle_result(wasm_api::HostFnStatus<void> result) {
    if (result) {
        return no_error(0);
    }
    switch(result.error()) {
        case wasm_api::HostFnError::OUT_OF_GAS:
            return out_of_gas();
        case wasm_api::HostFnError::RETURN_SUCCESS:
            return return_success();
        case wasm_api::HostFnError::DETERMINISTIC_ERROR:
            return deterministic_error();
        default:
            std::printf("unrecoverable error!\n");
            return unrecoverable_error();
    }
}

} // namespace detail

namespace wasm_api
{
class HostCallContext;
}

template<typename ret_type, std::same_as<uint64_t>... Args>
TrampolineResult call_internal(void* function_pointer,
    void* host_call_context,
    Args... args) noexcept
{
    try
    {
        wasm_api::HostCallContext* user_ctx
            = reinterpret_cast<wasm_api::HostCallContext*>(
                host_call_context);
        auto* ptr
            = (wasm_api::HostFnStatus<ret_type>(*)(wasm_api::HostCallContext*, Args...))(function_pointer);

        return detail::handle_result((*ptr)(user_ctx, args...));
    }
    catch (...)
    {
        return detail::unrecoverable_error();
    }
}

extern "C"
{

    // trampolines

    TrampolineResult c_call_0args(void* function_pointer,
                                  void* host_call_context) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context);
    }

    TrampolineResult c_call_1args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1);
    }

    TrampolineResult c_call_2args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2);
    }

    TrampolineResult c_call_3args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2, arg3);
    }

    TrampolineResult c_call_4args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2, arg3, arg4);
    }

    TrampolineResult c_call_5args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5);
    }

    TrampolineResult c_call_6args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    TrampolineResult c_call_7args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }

    TrampolineResult c_call_8args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7,
                                  uint64_t arg8) noexcept
    {
        return call_internal<uint64_t>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }

    TrampolineResult c_call_0args_noret(void* function_pointer,
                                  void* host_call_context) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context);
    }

    TrampolineResult c_call_1args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1);
    }

    TrampolineResult c_call_2args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2);
    }

    TrampolineResult c_call_3args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2, arg3);
    }

    TrampolineResult c_call_4args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2, arg3, arg4);
    }

    TrampolineResult c_call_5args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5);
    }

    TrampolineResult c_call_6args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5, arg6);
    }

    TrampolineResult c_call_7args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }

    TrampolineResult c_call_8args_noret(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5,
                                  uint64_t arg6,
                                  uint64_t arg7,
                                  uint64_t arg8) noexcept
    {
        return call_internal<void>(function_pointer, host_call_context, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
}
