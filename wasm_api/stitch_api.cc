#include "wasm_api/stitch_api.h"

#include <utility>

#include "wasm_api/bindings.h"

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
    return { reinterpret_cast<std::byte*>(slice.mem), slice.sz};
}
std::span<const std::byte>
Stitch_WasmRuntime::get_memory() const
{
    auto slice = ::stitch_get_memory(runtime_pointer);
    return { reinterpret_cast<const std::byte*>(slice.mem), slice.sz};
}

InvokeStatus<uint64_t>
Stitch_WasmRuntime::invoke(std::string const& method_name)
{
    auto invoke_res
        = ::stitch_invoke(runtime_pointer,
                          reinterpret_cast<const uint8_t*>(method_name.c_str()),
                          static_cast<uint32_t>(method_name.size()));

    InvokeError err = static_cast<InvokeError>(invoke_res.error);
    if (err == InvokeError::NONE) {
        return invoke_res.result;
    }

    return InvokeStatus<uint64_t>(std::unexpect_t{}, err);
}

bool 
Stitch_WasmRuntime::link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs, 
    WasmValueType ret_type) 
{
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