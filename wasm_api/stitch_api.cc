#include "wasm_api/stitch_api.h"

#include <utility>

struct MemorySlice
{
    uint8_t* mem;
    uint32_t size;
};

enum class InvokeError : uint32_t
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

struct InvokeResult
{
    uint64_t result;
    uint32_t error;
};

extern "C"
{

    MemorySlice get_memory(void* runtime_pointer);

    InvokeResult invoke(void* runtime_pointer,
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
    return std::make_unique<WasmRuntime>(
        new Stitch_WasmRuntime(contract, context_pointer, ctxp));
}

std::pair<uint8_t*, uint32_t>
Stitch_WasmRuntime::get_memory()
{
    auto slice = ::get_memory(runtime_pointer);
    return { slice.mem, slice.size };
}
std::pair<const uint8_t*, uint32_t>
Stitch_WasmRuntime::get_memory() const
{
    auto slice = ::get_memory(runtime_pointer);
    return { slice.mem, slice.size };
}

uint64_t
Stitch_WasmRuntime::invoke(std::string const& method_name)
{
    auto invoke_res
        = ::invoke(runtime_pointer,
                   reinterpret_cast<const uint8_t*>(method_name.c_str()),
                   static_cast<uint32_t>(method_name.size()));
    switch (InvokeError(invoke_res.error))
    {
        case InvokeError::None:
            return invoke_res.result;
        case InvokeError::StitchError:
            throw UnrecoverableSystemError("internal stitch error");
        case InvokeError::InputError:
            throw WasmError("invalid input fn name");
        case InvokeError::FuncNExist:
            throw WasmError("func nexist");
        case InvokeError::ReturnTypeError:
            throw WasmError("output type error");
        case InvokeError::WasmError:
            throw WasmError("propagating wasm error");
        case InvokeError::CallError:
            throw WasmError("error from call");
        case InvokeError::UnrecoverableSystemError:
            throw UnrecoverableSystemError("propagating unrecoverable error");
    }

    std::unreachable();
}

void
Stitch_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(void*))
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
                            uint64_t (*f)(void*, uint64_t))
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
                            uint64_t (*f)(void*, uint64_t, uint64_t))
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
Stitch_WasmRuntime::link_fn(std::string const& module_name,
                            std::string const& fn_name,
                            uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t))
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
    uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t))
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
Stitch_WasmRuntime::link_fn(
    std::string const& module_name,
    std::string const& fn_name,
    uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t))
{

    stitch_link_nargs(runtime_pointer,
                      (const uint8_t*)module_name.c_str(),
                      module_name.size(),
                      (const uint8_t*)fn_name.c_str(),
                      fn_name.size(),
                      (void*)f,
                      5);
}

} // namespace wasm_api