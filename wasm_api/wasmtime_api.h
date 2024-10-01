#pragma once

#include "wasm_api/wasm_api.h"

namespace wasm_api
{

typedef void* WasmtimeContextPtr;

class Wasmtime_WasmRuntime;

class Wasmtime_WasmContext : public detail::WasmContextImpl
{
public:
    Wasmtime_WasmContext(uint32_t max_stack_bytes);

    ~Wasmtime_WasmContext();

    std::unique_ptr<WasmRuntime> new_runtime_instance(Script const& contract,
                                                      void* ctxp);

    // expected signature: HostFnStatus<uint64_t>(HostCallContext*, uint64_t repeated nargs)
    bool link_fn_nargs(std::string const& module_name,
        std::string const& fn_name,
        void* fn,
        uint8_t nargs) override;

    bool finish_link(std::unique_ptr<WasmRuntime>& pre_link) {return true;}

    bool init_success() override { return context_pointer != nullptr; }

private:
    WasmtimeContextPtr context_pointer;
};

class Wasmtime_WasmRuntime : public detail::WasmRuntimeImpl
{
public:
    Wasmtime_WasmRuntime(void* wasmtime_runtime_ptr);

    ~Wasmtime_WasmRuntime();

    std::span<std::byte> get_memory() override;
    std::span<const std::byte> get_memory() const override;

    bool link_fn_nargs(std::string const& module_name,
        std::string const& fn_name,
        void* fn,
        uint8_t nargs) override {
        return false;
    }

    InvokeStatus<uint64_t> invoke(std::string const &method_name);

    bool 
    __attribute__((warn_unused_result))
    consume_gas(uint64_t gas) override;
    uint64_t get_available_gas() const override;
    void set_available_gas(uint64_t gas) override;

private:
    void* runtime_pointer;
};

} // namespace wasm_api
