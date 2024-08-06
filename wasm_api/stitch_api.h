#pragma once

#include "wasm_api/wasm_api.h"

extern "C"
{
void* new_stitch_context();
void free_stitch_context(void* stitch_context);
void* new_stitch_runtime(const uint8_t* data, uint32_t size, void* stitch_context, void* userctx);
void free_stitch_runtime(void* stitch_runtime);
}

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

namespace wasm_api {

class Stitch_WasmRuntime;

class Stitch_WasmContext : public detail::WasmContextImpl {

	Stitch_WasmContext(Stitch_WasmContext&) = delete;
	Stitch_WasmContext(Stitch_WasmContext&&) = delete;

public:

	Stitch_WasmContext()
		: context_pointer(new_stitch_context())
		{}

	~Stitch_WasmContext() {
		free_stitch_context(context_pointer);
	}

	std::unique_ptr<WasmRuntime>
	new_runtime_instance(Script const& contract, void* ctxp);

private:
	void* context_pointer;
};

class Stitch_WasmRuntime : public detail::WasmRuntimeImpl {
	Stitch_WasmRuntime(Stitch_WasmRuntime&) = delete;
	Stitch_WasmRuntime(Stitch_WasmRuntime&&) = delete;

public:

	Stitch_WasmRuntime(Script const& data, void* context_pointer, void* userctx)
		: runtime_pointer(new_stitch_runtime(data.data, data.len, context_pointer, userctx))
		{}

	~Stitch_WasmRuntime() {
		free_stitch_runtime(runtime_pointer);
	}

	std::pair<uint8_t*, uint32_t> get_memory() override final;
	std::pair<const uint8_t*, uint32_t> get_memory() const override final;

	 void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(void*));
	 void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(void*, uint64_t));
	 void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t));
	 void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t));
	 void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t));
	 void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t));

	uint64_t
	invoke(std::string const& method_name) override final;

private:
	void* runtime_pointer;
};

}

