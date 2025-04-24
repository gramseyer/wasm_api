#include "wasm_api/wasmtime_api.h"

#include <utility>

#include "wasm_api/bindings.h"

#include <cinttypes>
#include <cassert>

namespace wasm_api
{

Wasmtime_WasmContext::Wasmtime_WasmContext(uint32_t max_stack_bytes, bool is_cranelift)
    : context_pointer([=] () {
        if (is_cranelift) {
            return new_wasmtime_context_cranelift();
        }
        return new_wasmtime_context_winch();
    }())
{}

Wasmtime_WasmContext::~Wasmtime_WasmContext()
{
    free_wasmtime_context(context_pointer);
}

Wasmtime_WasmRuntime::Wasmtime_WasmRuntime(void* wasmtime_runtime_ptr)
    : runtime_pointer(wasmtime_runtime_ptr)
{
    assert(!!runtime_pointer);
}

Wasmtime_WasmRuntime::~Wasmtime_WasmRuntime()
{
    free_wasmtime_runtime(runtime_pointer);
}

std::unique_ptr<WasmRuntime>
Wasmtime_WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    std::unique_ptr<WasmRuntime> out = std::make_unique<WasmRuntime>(ctxp);

    void* runtime_pointer = new_wasmtime_runtime(contract.data,
        contract.len, out -> get_host_call_context(), context_pointer);

    if (runtime_pointer == nullptr) {
        return nullptr;
    }

    Wasmtime_WasmRuntime* wasmtime_runtime = new Wasmtime_WasmRuntime(
        runtime_pointer);

    out->initialize(wasmtime_runtime);

    return out;
}

std::span<std::byte>
Wasmtime_WasmRuntime::get_memory()
{
    auto slice = ::wasmtime_get_memory(runtime_pointer);
    return std::span(reinterpret_cast<std::byte*>(slice.mem), slice.sz);
}
std::span<const std::byte>
Wasmtime_WasmRuntime::get_memory() const
{
    auto slice = ::wasmtime_get_memory(runtime_pointer);
    return std::span(reinterpret_cast<const std::byte*>(slice.mem), slice.sz);
}

bool 
Wasmtime_WasmContext::link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs, 
    WasmValueType ret_type)
{
    std::lock_guard lock(link_entry_mutex);
    return wasmtime_link_nargs(context_pointer,
                     (const uint8_t*)module_name.c_str(),
                     module_name.size(),
                     (const uint8_t*)fn_name.c_str(),
                     fn_name.size(),
                     (void*)fn,
                     nargs,
                     static_cast<uint8_t>(ret_type));
}

InvokeStatus<uint64_t> 
Wasmtime_WasmRuntime::invoke(std::string const &method_name)
{
    auto invoke_res
        = ::wasmtime_invoke(runtime_pointer,
                          reinterpret_cast<const uint8_t*>(method_name.c_str()),
                          static_cast<uint32_t>(method_name.size()));


    InvokeError err = static_cast<InvokeError>(invoke_res.error);
    if (err == InvokeError::NONE) {
        return invoke_res.result;
    }

    return InvokeStatus<uint64_t>(std::unexpect_t{}, err);
}

bool
__attribute__((warn_unused_result))
Wasmtime_WasmRuntime::consume_gas(uint64_t gas)
{
    return wasmtime_consume_gas(runtime_pointer, gas);
}

uint64_t
Wasmtime_WasmRuntime::get_available_gas() const
{
    return wasmtime_get_available_gas(runtime_pointer);
}

void
Wasmtime_WasmRuntime::set_available_gas(uint64_t gas)
{
    wasmtime_set_available_gas(runtime_pointer, gas);
}


} // namespace wasm_api