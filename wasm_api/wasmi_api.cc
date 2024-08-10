#include "wasm_api/wasmi_api.h"

#include <utility>

#include "wasm_api/ffi_trampolines.h"

enum class WasmiInvokeError : uint32_t
{
    None = 0,
    WasmiError = 1,
    FuncNExist = 2,
    InputError = 3, // input validation fails
    ReturnTypeError = 4,
    CallError = 5,
    HostError = 6,
    UnrecoverableSystemError = 7
};

struct WasmiInvokeResult
{
    uint64_t result;
    uint32_t error;
    uint64_t gas_remaining;
};

extern "C"
{

    MemorySlice wasmi_get_memory(void* runtime_pointer);

    WasmiInvokeResult wasmi_invoke(void* runtime_pointer,
                        const uint8_t* method_bytes,
                        const uint32_t method_len,
                        uint64_t gas_limit);

    // return true if out of gas -- i.e. should shortcircuit rest of host function
    bool wasmi_consume_gas(void* runtime_pointer,
        uint64_t gas_to_consume);

    uint64_t wasmi_get_available_gas(const void* runtime_pointer);

    void wasmi_link_nargs(void* runtime_pointer,
                           const uint8_t* module_bytes,
                           const uint32_t module_bytes_len,
                           const uint8_t* method_bytes,
                           const uint32_t method_bytes_len,
                           void* fn_pointer,
                           uint8_t nargs);

    void* new_wasmi_context();
    void free_wasmi_context(void* wasmi_context);
    void* new_wasmi_runtime(const uint8_t* data,
                             uint32_t size,
                             void* wasmi_context,
                             void* userctx);
    void free_wasmi_runtime(void* wasmi_runtime);
}

namespace wasm_api
{

Wasmi_WasmContext::Wasmi_WasmContext()
    : context_pointer(new_wasmi_context())
{}

Wasmi_WasmContext::~Wasmi_WasmContext()
{
    free_wasmi_context(context_pointer);
}

Wasmi_WasmRuntime::Wasmi_WasmRuntime(Script const& data,
                                       void* context_pointer,
                                       HostCallContext* userctx)
    : runtime_pointer(
          new_wasmi_runtime(data.data, data.len, context_pointer, (void*)userctx))
{}

Wasmi_WasmRuntime::~Wasmi_WasmRuntime()
{
    free_wasmi_runtime(runtime_pointer);
}

std::unique_ptr<WasmRuntime>
Wasmi_WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    std::unique_ptr<WasmRuntime> out = std::make_unique<WasmRuntime>(ctxp);

    Wasmi_WasmRuntime* wasmi_runtime = new Wasmi_WasmRuntime(contract, context_pointer, out -> get_host_call_context());

    out -> initialize(wasmi_runtime);

    return out;
}

std::pair<uint8_t*, uint32_t>
Wasmi_WasmRuntime::get_memory()
{
    auto slice = ::wasmi_get_memory(runtime_pointer);
    return { slice.mem, slice.size };
}
std::pair<const uint8_t*, uint32_t>
Wasmi_WasmRuntime::get_memory() const
{
    auto slice = ::wasmi_get_memory(runtime_pointer);
    return { slice.mem, slice.size };
}

detail::MeteredReturn<uint64_t>
Wasmi_WasmRuntime::invoke(std::string const& method_name, uint64_t gas_limit)
{
    auto invoke_res
        = ::wasmi_invoke(runtime_pointer,
                   reinterpret_cast<const uint8_t*>(method_name.c_str()),
                   static_cast<uint32_t>(method_name.size()),
                   gas_limit);
    switch (WasmiInvokeError(invoke_res.error))
    {
        case WasmiInvokeError::None:
            return {invoke_res.result, gas_limit - invoke_res.gas_remaining};
        case WasmiInvokeError::WasmiError:
            throw UnrecoverableSystemError("internal wasmi error");
        case WasmiInvokeError::InputError:
            throw WasmError("invalid input fn name");
        case WasmiInvokeError::FuncNExist:
            throw WasmError("func nexist");
        case WasmiInvokeError::ReturnTypeError:
            throw WasmError("output type error");
        case WasmiInvokeError::CallError:
            throw WasmError("call error");
        case WasmiInvokeError::HostError:
            throw WasmError("propagating host error");
        case WasmiInvokeError::UnrecoverableSystemError:
            throw UnrecoverableSystemError("propagating unrecoverable error");
    }

    std::unreachable();
}

void
Wasmi_WasmRuntime::consume_gas(uint64_t gas)
{
    if (wasmi_consume_gas(runtime_pointer, gas))
    {
        throw WasmError("gas limit exceeded");
    }
}


uint64_t 
Wasmi_WasmRuntime::get_available_gas() const {
    return wasmi_get_available_gas(runtime_pointer);
}

void
Wasmi_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*))
{

    wasmi_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      0);
}

void
Wasmi_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*, uint64_t))
{

    wasmi_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      1);
}

void
Wasmi_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*, uint64_t, uint64_t))
{

    wasmi_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      2);
}

void
Wasmi_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t))
{

    wasmi_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      3);
}

void
Wasmi_WasmRuntime::link_fn(
    std::string const& module_name,
    std::string const& fn_name,
    uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t))
{

    wasmi_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      4);
}

void
Wasmi_WasmRuntime::link_fn(
    std::string const& module_name,
    std::string const& fn_name,
    uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t))
{

    wasmi_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      5);
}

} // namespace wasm_api