#include "wasm_api/ffi_trampolines.h"

#include "wasm_api/error.h"

namespace detail
{
TrampolineResult
success(uint64_t res)
{
    return TrampolineResult{ res, static_cast<uint8_t>(TrampolineError::None) };
}

TrampolineResult
error(TrampolineError error)
{
    return TrampolineResult{ 0, static_cast<uint8_t>(error) };
}

} // namespace detail

extern "C"
{
    // trampolines

    TrampolineResult c_call_0args(void* function_pointer,
                                  void* user_ctx) noexcept
    {
        try
        {
            auto* ptr = (uint64_t(*)(void*))(function_pointer);
            return detail::success((*ptr)(user_ctx));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError::HostError);
        }
        catch (...)
        {
            return detail::error(TrampolineError::UnrecoverableSystemError);
        }
    }

    TrampolineResult c_call_1args(void* function_pointer,
                                  void* user_ctx,
                                  uint64_t arg1) noexcept
    {
        try
        {
            auto* ptr = (uint64_t(*)(void*, uint64_t))(function_pointer);
            return detail::success((*ptr)(user_ctx, arg1));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError::HostError);
        }
        catch (...)
        {
            return detail::error(TrampolineError::UnrecoverableSystemError);
        }
    }

    TrampolineResult c_call_2args(void* function_pointer,
                                  void* user_ctx,
                                  uint64_t arg1,
                                  uint64_t arg2) noexcept
    {
        try
        {
            auto* ptr
                = (uint64_t(*)(void*, uint64_t, uint64_t))(function_pointer);
            return detail::success((*ptr)(user_ctx, arg1, arg2));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError::HostError);
        }
        catch (...)
        {
            return detail::error(TrampolineError::UnrecoverableSystemError);
        }
    }

    TrampolineResult c_call_3args(void* function_pointer,
                                  void* user_ctx,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3) noexcept
    {
        try
        {
            auto* ptr = (uint64_t(*)(void*, uint64_t, uint64_t, uint64_t))(
                function_pointer);
            return detail::success((*ptr)(user_ctx, arg1, arg2, arg3));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError::HostError);
        }
        catch (...)
        {
            return detail::error(TrampolineError::UnrecoverableSystemError);
        }
    }

    TrampolineResult c_call_4args(void* function_pointer,
                                  void* user_ctx,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4) noexcept
    {
        try
        {
            auto* ptr
                = (uint64_t(*)(void*, uint64_t, uint64_t, uint64_t, uint64_t))(
                    function_pointer);
            return detail::success((*ptr)(user_ctx, arg1, arg2, arg3, arg4));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError::HostError);
        }
        catch (...)
        {
            return detail::error(TrampolineError::UnrecoverableSystemError);
        }
    }

    TrampolineResult c_call_5args(void* function_pointer,
                                  void* user_ctx,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3,
                                  uint64_t arg4,
                                  uint64_t arg5) noexcept
    {
        try
        {
            auto* ptr = (uint64_t(*)(
                void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t))(
                function_pointer);
            return detail::success(
                (*ptr)(user_ctx, arg1, arg2, arg3, arg4, arg5));
        }
        catch (wasm_api::HostError&)
        {
            return detail::error(TrampolineError::HostError);
        }
        catch (...)
        {
            return detail::error(TrampolineError::UnrecoverableSystemError);
        }
    }
}
