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
    wasm3::environment env;
    const uint32_t MAX_STACK_BYTES;
};

class Wasm3_WasmRuntime : public detail::WasmRuntimeImpl
{
public:
    Wasm3_WasmRuntime(std::unique_ptr<wasm3::runtime> r,
                      std::unique_ptr<wasm3::module> m);

    std::pair<uint8_t*, uint32_t> get_memory() override
    {
        return runtime->get_memory();
    }

    std::pair<const uint8_t*, uint32_t> get_memory() const override
    {
        return runtime->get_memory();
    }

    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*)) override
    {
        _link_fn(module_name, fn_name, f);
    }
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t)) override
    {
        _link_fn(module_name, fn_name, f);
    }
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*, uint64_t, uint64_t)) override
    {
        _link_fn(module_name, fn_name, f);
    }
    void link_fn(
        std::string const& module_name,
        std::string const& fn_name,
        uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t)) override
    {
        _link_fn(module_name, fn_name, f);
    }
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*,
                               uint64_t,
                               uint64_t,
                               uint64_t,
                               uint64_t)) override
    {
        _link_fn(module_name, fn_name, f);
    }
    void link_fn(std::string const& module_name,
                 std::string const& fn_name,
                 uint64_t (*f)(HostCallContext*,
                               uint64_t,
                               uint64_t,
                               uint64_t,
                               uint64_t,
                               uint64_t)) override
    {
        _link_fn(module_name, fn_name, f);
    };

    detail::MeteredReturn<uint64_t> invoke(std::string const& method_name,
                                           uint64_t gas_limit) override;

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

    uint64_t available_gas;

    template<typename... Args>
    void _link_fn(std::string const& module_name,
                  std::string const& fn_name,
                  auto (*f)(Args...))
    {
        module->link_optional(module_name.c_str(), fn_name.c_str(), f);
    }
};

} // namespace wasm_api
