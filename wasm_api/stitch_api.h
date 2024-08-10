#pragma once

#include "wasm_api/wasm_api.h"

/**
 * Editorial:
 *
 * The makepad/stitch library appears well-engineered (?)
 * but has two problems that make it not suited for me.
 * This has become an exercise in futility, but at least
 * I learned something about rust and its FFI mechanics.
 *
 * 1) the wasm "stack" is a threadlocal block.  Appropriate
 * locks are acquired to ensure that each wasm module
 * executes correctly, but there's no way to ensure
 * deterministic stack overflows.
 *
 * 2) the library doesn't support error handling within
 * host functions.
 */

namespace wasm_api
{

class Stitch_WasmRuntime;

class Stitch_WasmContext : public detail::WasmContextImpl
{
public:
    Stitch_WasmContext();

    ~Stitch_WasmContext();

    std::unique_ptr<WasmRuntime> new_runtime_instance(Script const& contract,
                                                      void* ctxp);

private:
    void* context_pointer;
};

class Stitch_WasmRuntime : public detail::WasmRuntimeImpl
{
public:
    Stitch_WasmRuntime(Script const& data,
                       void* context_pointer,
                       void* userctx);

    ~Stitch_WasmRuntime();

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
                                           uint64_t gas_limit) override;

    void consume_gas(uint64_t gas) override;
    uint64_t get_available_gas() const override;

private:
    void* runtime_pointer;
    uint64_t available_gas = 0;
};

} // namespace wasm_api
