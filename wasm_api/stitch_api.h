#pragma once

#include "wasm_api/wasm_api.h"

extern "C"
{
void* new_stitch_context();
void free_stitch_context(void* stitch_context);
void* new_stitch_runtime(const uint8_t* data, uint32_t size, void* stitch_context);
void free_stitch_runtime(void* stitch_runtime);
}


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

	Stitch_WasmRuntime(Script const& data, void* context_pointer)
		: runtime_pointer(new_stitch_runtime(data.data, data.len, context_pointer))
		{}

	~Stitch_WasmRuntime() {
		free_stitch_runtime(runtime_pointer);
	}

	std::pair<uint8_t*, uint32_t> get_memory() override final;
	std::pair<const uint8_t*, uint32_t> get_memory() const override final;

	 void link_fn(
		const char* module_name,
		const char* fn_name,
		uint64_t (*f)(void*)) {}
	 void link_fn(
		const char* module_name,
		const char* fn_name,
		uint64_t (*f)(void*, uint64_t)) {}
	 void link_fn(
		const char* module_name,
		const char* fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t)) {}
	 void link_fn(
		const char* module_name,
		const char* fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t)) {}
	 void link_fn(
		const char* module_name,
		const char* fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t)) {}
	 void link_fn(
		const char* module_name,
		const char* fn_name,
		uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) {}

	uint64_t
	invoke(std::string const& method_name) override final;

private:
	void* runtime_pointer;
};

}

