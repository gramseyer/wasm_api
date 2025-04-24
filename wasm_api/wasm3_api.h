#pragma once

/**
 * Copyright 2023 Geoffrey Ramseyer
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wasm_api/wasm_api.h"

#include "wasm_api/wasm3.h"

namespace wasm_api
{

class Wasm3_WasmRuntime;

class Wasm3_WasmContext : public detail::WasmContextImpl
{
public:
    using runtime_t = Wasm3_WasmRuntime;

    Wasm3_WasmContext(uint32_t MAX_STACK_BYTES)
        : env()
        , MAX_STACK_BYTES(MAX_STACK_BYTES)
    {}

    std::unique_ptr<WasmRuntime> new_runtime_instance(Script const& contract,
                                                      void* ctxp) override;

private:
    std::mutex mtx;
    wasm3::environment env;
    const uint32_t MAX_STACK_BYTES;
};

class Wasm3_WasmRuntime : public detail::WasmRuntimeImpl
{
public:
    Wasm3_WasmRuntime(std::unique_ptr<wasm3::runtime> r,
                      std::unique_ptr<wasm3::module> m);

    std::span<std::byte> get_memory() override
    {
        return runtime->get_memory();
    }

    std::span<const std::byte> get_memory() const override
    {
        return runtime->get_memory();
    }

    bool link_fn_nargs(
        std::string const& module_name,
        std::string const& fn_name,
        void* fn,
        uint8_t nargs,
        WasmValueType ret_type) override
    {
        return module -> link_nargs(module_name.c_str(), fn_name.c_str(), fn, nargs, ret_type);
    }

    InvokeStatus<uint64_t> invoke(std::string const& method_name) override;

    // This version of WasmRuntime requires the wasm to be instrumented
    // with calls to a "consume gas" function.  Host functions
    // should also call into this, as necessary.
    bool
    __attribute__((warn_unused_result))
    consume_gas(uint64_t gas) override;

    void set_available_gas(uint64_t gas) override;
    uint64_t get_available_gas() const override;

private:
    std::unique_ptr<wasm3::runtime> runtime;
    std::unique_ptr<wasm3::module> module;

    uint64_t available_gas_;
};

} // namespace wasm_api
