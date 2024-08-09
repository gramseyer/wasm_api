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

namespace wasm_api
{

 
Wasm3_WasmRuntime::Wasm3_WasmRuntime(std::unique_ptr<wasm3::runtime> r, std::unique_ptr<wasm3::module> m)
	: runtime(std::move(r))
	, module(std::move(m))
	{}

std::unique_ptr<WasmRuntime> 
Wasm3_WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
	if (contract.data == nullptr)
	{
		throw UnrecoverableSystemError("invalid nullptr passed to wasm3_wasmcontext");
	}

	WasmRuntime* out = new WasmRuntime(ctxp);

	auto module = env.parse_module(contract.data, contract.len);

	auto runtime = env.new_runtime(MAX_STACK_BYTES, out -> get_host_call_context());

	runtime->load(*module);

	Wasm3_WasmRuntime* new_runtime = new Wasm3_WasmRuntime(std::move(runtime), std::move(module));

	out -> initialize(new_runtime);

	//WasmRuntime* out = new WasmRuntime(new Wasm3_WasmRuntime(std::move(runtime), std::move(module)));

	return std::unique_ptr<WasmRuntime>(out);//std::unique_ptr<WasmRuntime>(out);
}

detail::MeteredReturn<uint64_t>
Wasm3_WasmRuntime::invoke(std::string const& method_name, uint64_t gas_limit)
{
	auto fn = runtime->find_function(method_name.c_str());
	available_gas = gas_limit;

	auto res = fn.template call<uint64_t>();

	return {res, gas_limit - available_gas};
}

void
Wasm3_WasmRuntime::consume_gas(uint64_t gas)
{
	if (gas > available_gas) {
		available_gas = 0;
		throw WasmError("gas limit exceeded");
	}
	available_gas -= gas;
}


} /* wasm_api */
