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

namespace detail
{

class Wasm3_WasmRuntime;

class Wasm3_WasmContext {

	wasm3::environment env;

	const uint32_t MAX_STACK_BYTES;

public:
	using runtime_t = Wasm3_WasmRuntime;

	Wasm3_WasmContext(uint32_t MAX_STACK_BYTES)
		: env()
		, MAX_STACK_BYTES(MAX_STACK_BYTES)
		{
		}

	std::unique_ptr<WasmRuntime> 
	new_runtime_instance(Script const& contract);
};

class Wasm3_WasmRuntime {

	std::unique_ptr<wasm3::runtime> runtime;
	std::unique_ptr<wasm3::module> module;

public:

	std::pair<uint8_t*, uint32_t> get_memory()
	{
		return runtime->get_memory();
	}

	std::pair<const uint8_t*, uint32_t> 
	get_memory() const
	{
		return runtime->get_memory();
	}

	Wasm3_WasmRuntime(std::unique_ptr<wasm3::runtime> r, std::unique_ptr<wasm3::module> m)
		: runtime(std::move(r))
		, module(std::move(m))
		{}

	template<typename... Args>
	void link_fn(
		const char* module_name,
		const char* fn_name,
		auto (*f)(Args...))
	{
		module->link_optional(module_name, fn_name, f);
	}

	template<typename ret>
	ret 
	invoke(const char* method_name);
};

} /* detail */

} /* wasm_api */
