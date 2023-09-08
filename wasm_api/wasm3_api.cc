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

namespace detail
{


std::unique_ptr<WasmRuntime> 
Wasm3_WasmContext::new_runtime_instance(Script const& contract)
{
	if (contract.data == nullptr)
	{
		throw UnrecoverableSystemError("invalid nullptr passed to wasm3_wasmcontext");
	}

	auto module = env.parse_module(contract.data, contract.len);

	auto runtime = env.new_runtime(MAX_STACK_BYTES);

	runtime->load(*module);

	WasmRuntime* out = new WasmRuntime(new Wasm3_WasmRuntime(std::move(runtime), std::move(module)));

	return std::unique_ptr<WasmRuntime>(out);
}

template<typename ret>
ret  
Wasm3_WasmRuntime::invoke(const char* method_name)
{
	auto fn = runtime->find_function(method_name);

	return fn.template call<ret>();
}

template
void
Wasm3_WasmRuntime::invoke<void>(const char* methodname);

template
int32_t
Wasm3_WasmRuntime::invoke<int32_t>(const char* methodname);

} /* detail */

} /* wasm_api */
