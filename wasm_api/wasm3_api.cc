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

#include "wasm_api/wasm3_api.h"

#include <utility>

namespace wasm_api
{

Wasm3_WasmRuntime::Wasm3_WasmRuntime(std::unique_ptr<wasm3::runtime> r,
                                     std::unique_ptr<wasm3::module> m)
    : runtime(std::move(r))
    , module(std::move(m))
    , available_gas(0)
{}

std::unique_ptr<WasmRuntime>
Wasm3_WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    if (contract.data == nullptr)
    {
    	return nullptr;
       /* throw UnrecoverableSystemError(
            "invalid nullptr passed to wasm3_wasmcontext"); */
    }

    std::unique_ptr<WasmRuntime> out = std::make_unique<WasmRuntime>(ctxp);

    auto module = env.parse_module(contract.data, contract.len);

    auto runtime
        = env.new_runtime(MAX_STACK_BYTES, out->get_host_call_context());

    if (!runtime->load(*module))
    {
    	return nullptr;
    }

    Wasm3_WasmRuntime* new_runtime
        = new Wasm3_WasmRuntime(std::move(runtime), std::move(module));

    out->initialize(new_runtime);

    return std::unique_ptr<WasmRuntime>(out.release());
}

detail::MeteredReturn<uint64_t>
Wasm3_WasmRuntime::invoke(std::string const& method_name, uint64_t gas_limit)
{
    auto fn = runtime->find_function(method_name.c_str());

    if (!fn){
    	return {std::nullopt, 0, ErrorType::HostError};
    }

    available_gas = gas_limit;

    auto [ret_val, res_err]  = fn->template call<uint64_t>();

    uint64_t consumed_gas = gas_limit - available_gas;

    if (res_err == m3Err_none)
    {
    	if (!ret_val.has_value())
    	{
    		return {std::nullopt, consumed_gas, ErrorType::UnrecoverableSystemError};
    	}
    	return {*ret_val, consumed_gas, ErrorType::None};
    }    

    if (res_err != m3Err_unrecoverableSystemError)
    {
    	return {std::nullopt, consumed_gas, ErrorType::HostError};
    }

    std::unreachable(); // m3Err_unrecoverableSystemError should have thrown from within the wasm3 api
}

bool
__attribute__((warn_unused_result))
Wasm3_WasmRuntime::consume_gas(uint64_t gas)
{
    if (gas > available_gas)
    {
        available_gas = 0;
        return false;
    }
    available_gas -= gas;
    return true;
}

void
Wasm3_WasmRuntime::set_available_gas(uint64_t gas)
{
    available_gas = gas;
}

uint64_t
Wasm3_WasmRuntime::get_available_gas() const
{
    return available_gas;
}

} // namespace wasm_api
