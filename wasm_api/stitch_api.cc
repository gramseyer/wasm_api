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
            return { invoke_res.result, gas_limit - available_gas , ErrorType::None};
        case StitchInvokeError::StitchError:
            /**
             * Occurs when the module fails to instantiate.
             * As far as I can tell skimming the code, this is deterministic
             * (i.e. lots of wasm validation checks), with the possibility of a panic
             * occuring for nondeterministic errors (i.e. module.rs: line 159,
             * unreachable() occurs if a wasm section has an unknown number,
             * instead of returning an error).
             */
            return { std::nullopt, gas_limit - available_gas , ErrorType::HostError};
        case StitchInvokeError::InputError:
            return { std::nullopt, gas_limit - available_gas , ErrorType::HostError};
            //throw WasmError("invalid input fn name");
        case StitchInvokeError::FuncNExist:
            return { std::nullopt, gas_limit - available_gas , ErrorType::HostError};

            //throw WasmError("func nexist");
        case StitchInvokeError::ReturnTypeError:
            return { std::nullopt, gas_limit - available_gas , ErrorType::HostError};
            //throw WasmError("output type error");
        case StitchInvokeError::WasmError:
            return { std::nullopt, gas_limit - available_gas , ErrorType::HostError};
            //throw WasmError("propagating wasm error");
        case StitchInvokeError::CallError:
            // Was intended to handle the case of an error within a host function
            // propagating back out of stitch via e.g. catching a panic,
            // but that just doesn't work in stitch.  So this case cannot happen.
            //return { std::nullopt, gas_limit - available_gas , ErrorType::HostError};
            std::unreachable();
        case StitchInvokeError::UnrecoverableSystemError:
            // Likewise never actually occurs.
            throw UnrecoverableSystemError("stitch propagating unrecoverable system error");
            //return { std::nullopt, gas_limit - available_gas , ErrorType::UnrecoverableSystemError};
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