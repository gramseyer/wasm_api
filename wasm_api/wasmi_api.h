#pragma once

#include "wasm_api/wasm_api.h"

namespace wasm_api
{

class Wasmi_WasmRuntime;

class Wasmi_WasmContext : public detail::WasmContextImpl
{
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
public:
    Wasmi_WasmRuntime(Script const& data,
                      void* context_pointer,
                      HostCallContext* host_call_context);

    ~Wasmi_WasmRuntime();

    std::pair<uint8_t*, uint32_t> get_memory() override;
    std::pair<const uint8_t*, uint32_t> get_memory() const override;

    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*)) override;
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t)) override;
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t, uint64_t)) override;
    void link_fn(
        std::string const& module_name,
        std::string const& fn_name,
        uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t)) override;
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*,
                               uint64_t,
                               uint64_t,
                               uint64_t,
                               uint64_t)) override;
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*,
                               uint64_t,
                               uint64_t,
                               uint64_t,
                               uint64_t,
                               uint64_t)) override;

    detail::MeteredReturn<uint64_t> invoke(std::string const& method_name,
                                           uint64_t gas_limit) override final;

    void consume_gas(uint64_t gas) override;
    uint64_t get_available_gas() const override;

private:
    void* runtime_pointer;
};

} // namespace wasm_api
