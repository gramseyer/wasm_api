#include "wasm_api/ffi_trampolines.h"

#include "wasm_api/error.h"

#include <utility>

constexpr static uint8_t TrampolineError_NONE = 0;
constexpr static uint8_t TrampolineError_HOSTERROR = 1;
constexpr static uint8_t TrampolineError_UNRECOVERABLE = 2;

namespace detail
{
TrampolineResult
success(uint64_t res)
{
    return TrampolineResult{ res, TrampolineError_NONE };
}

TrampolineResult
error(uint8_t error)
{
    return TrampolineResult{ 0, error };
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
                = (uint64_t(*)(wasm_api::HostCallContext*))(function_pointer);
            return detail::success((*ptr)(user_ctx));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError_HOSTERROR);
        }
        catch (...)
        {
            return detail::error(TrampolineError_UNRECOVERABLE);
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

            auto* ptr = (uint64_t(*)(wasm_api::HostCallContext*, uint64_t))(
                function_pointer);
            return detail::success((*ptr)(user_ctx, arg1));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError_HOSTERROR);
        }
        catch (...)
        {
            return detail::error(TrampolineError_UNRECOVERABLE);
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
                = (uint64_t(*)(wasm_api::HostCallContext*, uint64_t, uint64_t))(
                    function_pointer);
            return detail::success((*ptr)(user_ctx, arg1, arg2));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError_HOSTERROR);
        }
        catch (...)
        {
            return detail::error(TrampolineError_UNRECOVERABLE);
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

            auto* ptr = (uint64_t(*)(
                wasm_api::HostCallContext*, uint64_t, uint64_t, uint64_t))(
                function_pointer);
            return detail::success((*ptr)(user_ctx, arg1, arg2, arg3));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError_HOSTERROR);
        }
        catch (...)
        {
            return detail::error(TrampolineError_UNRECOVERABLE);
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

            auto* ptr = (uint64_t(*)(wasm_api::HostCallContext*,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t))(function_pointer);
            return detail::success((*ptr)(user_ctx, arg1, arg2, arg3, arg4));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError_HOSTERROR);
        }
        catch (...)
        {
            return detail::error(TrampolineError_UNRECOVERABLE);
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

            auto* ptr = (uint64_t(*)(wasm_api::HostCallContext*,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t))(function_pointer);
            return detail::success(
                (*ptr)(user_ctx, arg1, arg2, arg3, arg4, arg5));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError_HOSTERROR);
        }
        catch (...)
        {
            return detail::error(TrampolineError_UNRECOVERABLE);
        }
    }
}
