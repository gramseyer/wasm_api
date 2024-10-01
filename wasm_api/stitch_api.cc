#include "wasm_api/stitch_api.h"

#include <utility>

#include "wasm_api/ffi_trampolines.h"

enum class StitchInvokeError : uint32_t
{
    None = 0,
    StitchError = 1,
    FuncNExist = 2,
    InputError = 3, // input validation fails
    ReturnTypeError = 4,
    WasmError = 5,
    UnrecoverableSystemError = 6
};

struct StitchInvokeResult
{
    uint64_t result;
    uint32_t error;
};

extern "C"
{

    MemorySlice stitch_get_memory(void* runtime_pointer);

    StitchInvokeResult stitch_invoke(void* runtime_pointer,
                                     const uint8_t* method_bytes,
                                     const uint32_t method_len);

    bool stitch_link_nargs(void* runtime_pointer,
                           const uint8_t* module_bytes,
                           const uint32_t module_bytes_len,
                           const uint8_t* method_bytes,
                           const uint32_t method_bytes_len,
                           void* fn_pointer,
                           uint8_t nargs,
                           uint8_t ret_type);

    void* new_stitch_context();
    void free_stitch_context(void* stitch_context);
    void* new_stitch_runtime(const uint8_t* data,
                             uint32_t size,
                             void* stitch_context,
                             void* userctx);
    void free_stitch_runtime(void* stitch_runtime);
}

namespace wasm_api
{

Stitch_WasmContext::Stitch_WasmContext()
    : context_pointer(new_stitch_context())
{}

Stitch_WasmContext::~Stitch_WasmContext()
{
    free_stitch_context(context_pointer);
}

Stitch_WasmRuntime::Stitch_WasmRuntime(Script const& data,
                                       void* context_pointer,
                                       void* userctx)
    : runtime_pointer(
          new_stitch_runtime(data.data, data.len, context_pointer, userctx))
{}

Stitch_WasmRuntime::~Stitch_WasmRuntime()
{
    free_stitch_runtime(runtime_pointer);
}

std::unique_ptr<WasmRuntime>
Stitch_WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    std::unique_ptr<WasmRuntime> out = std::make_unique<WasmRuntime>(ctxp);

    auto* stitch_runtime = new Stitch_WasmRuntime(
        contract, context_pointer, out->get_host_call_context());

    if (!stitch_runtime -> has_valid_runtime_pointer()) {
        return nullptr;
    }

    out->initialize(stitch_runtime);

    return out;
}

std::span<std::byte>
Stitch_WasmRuntime::get_memory()
{
    auto slice = ::stitch_get_memory(runtime_pointer);
    return { reinterpret_cast<std::byte*>(slice.mem), slice.size};
}
std::span<const std::byte>
Stitch_WasmRuntime::get_memory() const
{
    auto slice = ::stitch_get_memory(runtime_pointer);
    return { reinterpret_cast<const std::byte*>(slice.mem), slice.size};
}

InvokeStatus<uint64_t>
Stitch_WasmRuntime::invoke(std::string const& method_name)
{
    auto invoke_res
        = ::stitch_invoke(runtime_pointer,
                          reinterpret_cast<const uint8_t*>(method_name.c_str()),
                          static_cast<uint32_t>(method_name.size()));
    switch (StitchInvokeError(invoke_res.error))
    {
        case StitchInvokeError::None:
            return invoke_res.result;
        case StitchInvokeError::StitchError:
            /**
             * Occurs when the module fails to instantiate.
             * As far as I can tell skimming the code, this is deterministic
             * (i.e. lots of wasm validation checks), with the possibility of a panic
             * occuring for nondeterministic errors (i.e. module.rs: line 159,
             * unreachable() occurs if a wasm section has an unknown number,
             * instead of returning an error).
             */
        case StitchInvokeError::InputError:
        case StitchInvokeError::FuncNExist:
        case StitchInvokeError::ReturnTypeError:
        case StitchInvokeError::WasmError:
            return InvokeStatus<uint64_t>(std::unexpect_t{}, InvokeError::DETERMINISTIC_ERROR);
        case StitchInvokeError::UnrecoverableSystemError:
            return InvokeStatus<uint64_t>(std::unexpect_t{}, InvokeError::UNRECOVERABLE);
    }

    throw std::runtime_error("impossible");
}

bool 
Stitch_WasmRuntime::link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs, 
    WasmValueType ret_type) 
{
    if (ret_type != WasmValueType::U64) {
        return false;
        // unimpl;
    }
    return stitch_link_nargs(
        runtime_pointer,
        (const uint8_t*)module_name.c_str(),
        module_name.size(),
        (const uint8_t*)fn_name.c_str(),
        fn_name.size(),
        (void*)fn,
        nargs,
        static_cast<uint8_t>(ret_type));
}

bool
__attribute__((warn_unused_result))
Stitch_WasmRuntime::consume_gas(uint64_t gas)
{
    if (gas > available_gas)
    {
        available_gas = 0;
        return false;
    }
    available_gas -= gas;
    return true;
}

uint64_t
Stitch_WasmRuntime::get_available_gas() const
{
    return available_gas;
}

void
Stitch_WasmRuntime::set_available_gas(uint64_t gas)
{
    available_gas = gas;
}

} // namespace wasm_api