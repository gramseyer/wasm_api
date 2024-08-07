#pragma once

#include "wasm_api/wasm_api.h"

namespace wasm_api
{

class Wasmi_WasmRuntime;

class Wasmi_WasmContext : public detail::WasmContextImpl
{

    Wasmi_WasmContext(Wasmi_WasmContext&) = delete;
    Wasmi_WasmContext(Wasmi_WasmContext&&) = delete;

public:
    Wasmi_WasmContext();

    ~Wasmi_WasmContext();

    std::unique_ptr<WasmRuntime> new_runtime_instance(Script const& contract,
                                                      void* ctxp);

private:
    void* context_pointer;
};

class Wasmi_WasmRuntime : public detail::WasmRuntimeImpl
{
    Wasmi_WasmRuntime(Wasmi_WasmRuntime&) = delete;
    Wasmi_WasmRuntime(Wasmi_WasmRuntime&&) = delete;

public:
    Wasmi_WasmRuntime(Script const& data,
                       void* context_pointer,
                       HostCallContext* host_call_context);

    ~Wasmi_WasmRuntime();

    std::pair<uint8_t*, uint32_t> get_memory() override final;
    std::pair<const uint8_t*, uint32_t> get_memory() const override final;

    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t, uint64_t));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t));
    void link_fn(
        std::string const& module_name,
        std::string const& fn_name,
        uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t));

    detail::MeteredReturn<uint64_t>
    invoke(std::string const& method_name, uint64_t gas_limit) override final;

    void consume_gas(uint64_t gas) override;

private:
    void* runtime_pointer;
};

} // namespace wasm_api
