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

    Stitch_WasmContext(Stitch_WasmContext&) = delete;
    Stitch_WasmContext(Stitch_WasmContext&&) = delete;

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
    Stitch_WasmRuntime(Stitch_WasmRuntime&) = delete;
    Stitch_WasmRuntime(Stitch_WasmRuntime&&) = delete;

public:
    Stitch_WasmRuntime(Script const& data,
                       void* context_pointer,
                       void* userctx);

    ~Stitch_WasmRuntime();

    std::pair<uint8_t*, uint32_t> get_memory() override final;
    std::pair<const uint8_t*, uint32_t> get_memory() const override final;

    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(void*));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(void*, uint64_t));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(void*, uint64_t, uint64_t));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t));
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t));
    void link_fn(
        std::string const& module_name,
        std::string const& fn_name,
        uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t));

    uint64_t invoke(std::string const& method_name) override final;

private:
    void* runtime_pointer;
};

} // namespace wasm_api
