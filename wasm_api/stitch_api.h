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
 * 
 * 3) The library implicitly checks for a start function
 * and then executes that on module instantiation.
 * This would have to be disabled or guarded against
 * in order to use stitch in prod.
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

    std::span<std::byte> get_memory() override;
    std::span<const std::byte> get_memory() const override;

    // expected signature: HostFnStatus<uint64_t>(HostCallContext*, uint64_t repeated nargs)
    bool link_fn_nargs(std::string const& module_name,
        std::string const& fn_name,
        void* fn,
        uint8_t nargs) override;

    InvokeStatus<uint64_t> invoke(std::string const &method_name);

    bool
    __attribute__((warn_unused_result))
    consume_gas(uint64_t gas) override;
    uint64_t get_available_gas() const override;
    void set_available_gas(uint64_t gas) override;

    bool has_valid_runtime_pointer() const {
        return runtime_pointer != nullptr;
    }

private:
    void* runtime_pointer;
    uint64_t available_gas = 0;
};

} // namespace wasm_api
