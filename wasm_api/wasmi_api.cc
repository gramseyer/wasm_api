#include "wasm_api/wasmi_api.h"

#include <utility>

#include "wasm_api/ffi_trampolines.h"

#include <cinttypes>
#include <cassert>

extern "C"
{

    MemorySlice wasmi_get_memory(void* runtime_pointer);

    FFIInvokeResult wasmi_invoke(void* runtime_pointer,
                                   const uint8_t* method_bytes,
                                   const uint32_t method_len);

    // return true if out of gas -- i.e. should shortcircuit rest of host
    // function
    bool wasmi_consume_gas(void* runtime_pointer, uint64_t gas_to_consume);

    uint64_t wasmi_get_available_gas(const void* runtime_pointer);
    void wasmi_set_available_gas(void* runtime_pointer, uint64_t gas);

    bool wasmi_link_nargs(wasm_api::WasmiContextPtr,
                          const uint8_t* module_bytes,
                          const uint32_t module_bytes_len,
                          const uint8_t* method_bytes,
                          const uint32_t method_bytes_len,
                          void* fn_pointer,
                          uint8_t nargs,
                          uint8_t ret_type);

    wasm_api::WasmiContextPtr new_wasmi_context(uint32_t max_stack_bytes);
    void free_wasmi_context(wasm_api::WasmiContextPtr wasmi_context);

    void* new_wasmi_runtime(const uint8_t* data,
                            uint32_t size,
                            void* userctx,
                            wasm_api::WasmiContextPtr wasmi_context);

    void free_wasmi_runtime(void* wasmi_runtime);
}

namespace wasm_api
{

Wasmi_WasmContext::Wasmi_WasmContext(uint32_t max_stack_bytes)
    : context_pointer(new_wasmi_context(max_stack_bytes))
{}

Wasmi_WasmContext::~Wasmi_WasmContext()
{
    free_wasmi_context(context_pointer);
}

Wasmi_WasmRuntime::Wasmi_WasmRuntime(void* wasmi_runtime_ptr)
    : runtime_pointer(wasmi_runtime_ptr)
{
    assert(!!runtime_pointer);
}

Wasmi_WasmRuntime::~Wasmi_WasmRuntime()
{
    free_wasmi_runtime(runtime_pointer);
}

std::unique_ptr<WasmRuntime>
Wasmi_WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    std::unique_ptr<WasmRuntime> out = std::make_unique<WasmRuntime>(ctxp);

    void* wasmi_runtime_ptr = new_wasmi_runtime(contract.data,
        contract.len, out -> get_host_call_context(), context_pointer);

    if (wasmi_runtime_ptr == nullptr) {
        return nullptr;
    }

    Wasmi_WasmRuntime* wasmi_runtime = new Wasmi_WasmRuntime(
        wasmi_runtime_ptr);

    out->initialize(wasmi_runtime);

    return out;
}

std::span<std::byte>
Wasmi_WasmRuntime::get_memory()
{
    auto slice = ::wasmi_get_memory(runtime_pointer);
    return std::span(reinterpret_cast<std::byte*>(slice.mem), slice.size);
}
std::span<const std::byte>
Wasmi_WasmRuntime::get_memory() const
{
    auto slice = ::wasmi_get_memory(runtime_pointer);
    return std::span(reinterpret_cast<const std::byte*>(slice.mem), slice.size);
}

bool 
Wasmi_WasmContext::link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs,
    WasmValueType ret_type)
{
    return wasmi_link_nargs(context_pointer,
                     (const uint8_t*)module_name.c_str(),
                     module_name.size(),
                     (const uint8_t*)fn_name.c_str(),
                     fn_name.size(),
                     (void*)fn,
                     nargs,
                     static_cast<uint8_t>(ret_type));
}

InvokeStatus<uint64_t> 
Wasmi_WasmRuntime::invoke(std::string const &method_name)
{
    auto invoke_res
        = ::wasmi_invoke(runtime_pointer,
                          reinterpret_cast<const uint8_t*>(method_name.c_str()),
                          static_cast<uint32_t>(method_name.size()));


    InvokeError err = static_cast<InvokeError>(invoke_res.invoke_panic);
    if (err == InvokeError::NONE) {
        return invoke_res.result;
    }

    return InvokeStatus<uint64_t>(std::unexpect_t{}, err);
}

bool
__attribute__((warn_unused_result))
Wasmi_WasmRuntime::consume_gas(uint64_t gas)
{
    return wasmi_consume_gas(runtime_pointer, gas);
}

uint64_t
Wasmi_WasmRuntime::get_available_gas() const
{
    return wasmi_get_available_gas(runtime_pointer);
}

void
Wasmi_WasmRuntime::set_available_gas(uint64_t gas)
{
    wasmi_set_available_gas(runtime_pointer, gas);
}


} // namespace wasm_api