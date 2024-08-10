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
    CallError = 6,
    UnrecoverableSystemError = 7
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

    void stitch_link_nargs(void* runtime_pointer,
                           const uint8_t* module_bytes,
                           const uint32_t module_bytes_len,
                           const uint8_t* method_bytes,
                           const uint32_t method_bytes_len,
                           void* fn_pointer,
                           uint8_t nargs);

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

    out->initialize(stitch_runtime);

    return out;
}

std::pair<uint8_t*, uint32_t>
Stitch_WasmRuntime::get_memory()
{
    auto slice = ::stitch_get_memory(runtime_pointer);
    return { slice.mem, slice.size };
}
std::pair<const uint8_t*, uint32_t>
Stitch_WasmRuntime::get_memory() const
{
    auto slice = ::stitch_get_memory(runtime_pointer);
    return { slice.mem, slice.size };
}

detail::MeteredReturn<uint64_t>
Stitch_WasmRuntime::invoke(std::string const& method_name, uint64_t gas_limit)
{
    available_gas = gas_limit;
    auto invoke_res
        = ::stitch_invoke(runtime_pointer,
                          reinterpret_cast<const uint8_t*>(method_name.c_str()),
                          static_cast<uint32_t>(method_name.size()));
    switch (StitchInvokeError(invoke_res.error))
    {
        case StitchInvokeError::None:
            return { invoke_res.result, gas_limit - available_gas };
        case StitchInvokeError::StitchError:
            throw UnrecoverableSystemError("internal stitch error");
        case StitchInvokeError::InputError:
            throw WasmError("invalid input fn name");
        case StitchInvokeError::FuncNExist:
            throw WasmError("func nexist");
        case StitchInvokeError::ReturnTypeError:
            throw WasmError("output type error");
        case StitchInvokeError::WasmError:
            throw WasmError("propagating wasm error");
        case StitchInvokeError::CallError:
            throw WasmError("error from call");
        case StitchInvokeError::UnrecoverableSystemError:
            throw UnrecoverableSystemError("propagating unrecoverable error");
    }

    std::unreachable();
}

void
Stitch_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      0);
}

void
Stitch_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*, uint64_t))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      1);
}

void
Stitch_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*, uint64_t, uint64_t))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      2);
}

void
Stitch_WasmRuntime::link_fn(
    std::string const& module_name,
    std::string const& fn_name,
    uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      3);
}

void
Stitch_WasmRuntime::link_fn(
    std::string const& module_name,
    std::string const& fn_name,
    uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      4);
}

void
Stitch_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(HostCallContext*,
                                          uint64_t,
                                          uint64_t,
                                          uint64_t,
                                          uint64_t,
                                          uint64_t))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      5);
}

void
Stitch_WasmRuntime::consume_gas(uint64_t gas)
{
    if (gas > available_gas)
    {
        available_gas = 0;
        throw WasmError("gas limit exceeded");
    }
    available_gas -= gas;
}

uint64_t
Stitch_WasmRuntime::get_available_gas() const
{
    return available_gas;
}

} // namespace wasm_api