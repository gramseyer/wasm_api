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

TrampolineResult handle_result(wasm_api::HostFnStatus<uint64_t> result) {
    if (result) {
        return no_error(*result);
    }
    switch(result.error()) {
        case wasm_api::HostFnError::OUT_OF_GAS:
            return out_of_gas();
        case wasm_api::HostFnError::RETURN_SUCCESS:
            return return_success();
        default:
            return unrecoverable_error();
    }
}


} // namespace detail

namespace wasm_api
{
class HostCallContext;
}

extern "C"
{

    // trampolines

    TrampolineResult c_call_0args(void* function_pointer,
                                  void* host_call_context) noexcept
    {
        try
        {
            wasm_api::HostCallContext* user_ctx
                = reinterpret_cast<wasm_api::HostCallContext*>(
                    host_call_context);
            auto* ptr
                = (wasm_api::HostFnStatus<uint64_t>(*)(wasm_api::HostCallContext*))(function_pointer);

            return detail::handle_result((*ptr)(user_ctx));
        }
        catch (...)
        {
            return detail::unrecoverable_error();
        }
    }

    TrampolineResult c_call_1args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1) noexcept
    {
        try
        {
            wasm_api::HostCallContext* user_ctx
                = reinterpret_cast<wasm_api::HostCallContext*>(
                    host_call_context);

            auto* ptr = (wasm_api::HostFnStatus<uint64_t>(*)(wasm_api::HostCallContext*, uint64_t))(
                function_pointer);
            return detail::handle_result((*ptr)(user_ctx, arg1));
        }
        catch (...)
        {
            return detail::unrecoverable_error();
        }
    }

    TrampolineResult c_call_2args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2) noexcept
    {
        try
        {
            wasm_api::HostCallContext* user_ctx
                = reinterpret_cast<wasm_api::HostCallContext*>(
                    host_call_context);

            auto* ptr
                = (wasm_api::HostFnStatus<uint64_t>(*)(wasm_api::HostCallContext*, uint64_t, uint64_t))(
                    function_pointer);
            return detail::handle_result((*ptr)(user_ctx, arg1, arg2));
        }
        catch (...)
        {
            return detail::unrecoverable_error();
        }
    }

    TrampolineResult c_call_3args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3) noexcept
    {
        try
        {
            wasm_api::HostCallContext* user_ctx
                = reinterpret_cast<wasm_api::HostCallContext*>(
                    host_call_context);

            auto* ptr = (wasm_api::HostFnStatus<uint64_t>(*)(
                wasm_api::HostCallContext*, uint64_t, uint64_t, uint64_t))(
                function_pointer);
            return detail::handle_result((*ptr)(user_ctx, arg1, arg2, arg3));
        }
        catch (...)
        {
            return detail::unrecoverable_error();
        }
    }

    TrampolineResult c_call_4args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4) noexcept
    {
        try
        {
            wasm_api::HostCallContext* user_ctx
                = reinterpret_cast<wasm_api::HostCallContext*>(
                    host_call_context);

            auto* ptr = (wasm_api::HostFnStatus<uint64_t>(*)(wasm_api::HostCallContext*,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t))(function_pointer);
            return detail::handle_result((*ptr)(user_ctx, arg1, arg2, arg3, arg4));
        }
        catch (...)
        {
            return detail::unrecoverable_error();
        }
    }

    TrampolineResult c_call_5args(void* function_pointer,
                                  void* host_call_context,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5) noexcept
    {
        try
        {
            wasm_api::HostCallContext* user_ctx
                = reinterpret_cast<wasm_api::HostCallContext*>(
                    host_call_context);

            auto* ptr = (wasm_api::HostFnStatus<uint64_t>(*)(wasm_api::HostCallContext*,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t))(function_pointer);
            return detail::handle_result(
                (*ptr)(user_ctx, arg1, arg2, arg3, arg4, arg5));
        }
        catch (...)
        {
            return detail::unrecoverable_error();
        }
    }
}
