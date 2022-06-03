#pragma once

#include "wasm_api/wasm_api.h"

#include "wasm_api/wasm3.h"

namespace wasm_api
{

namespace detail
{

class Wasm3_WasmRuntime;

class Wasm3_WasmContext {

	wasm3::environment env;

	ScriptDB const& script_db;

	const uint32_t MAX_STACK_BYTES;

public:
	using runtime_t = Wasm3_WasmRuntime;

	Wasm3_WasmContext(ScriptDB const& db, uint32_t MAX_STACK_BYTES)
		: env()
		, script_db(db)
		, MAX_STACK_BYTES(MAX_STACK_BYTES)
		{}

	std::unique_ptr<WasmRuntime> 
	new_runtime_instance(Hash const& script_addr);
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
