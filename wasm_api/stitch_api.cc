#include "wasm_api/stitch_api.h"

struct MemorySlice {
    uint8_t* mem;
    uint32_t size;
};

struct InvokeResult {
    uint64_t result;
    bool error;
};

extern "C" {

MemorySlice get_memory(void* runtime_pointer);

InvokeResult invoke(void* runtime_pointer, const uint8_t* method_bytes, const uint32_t method_len);

void stitch_link_0args(void* runtime_pointer,
	const uint8_t* module_bytes, const uint32_t module_bytes_len,
	const uint8_t* method_bytes, const uint32_t method_bytes_len,
	void* fn_pointer);

// trampolines

uint64_t c_call_0args(void* function_pointer, void* user_ctx)
{
	auto* ptr = (uint64_t (*) (void*))(function_pointer);
	return (*ptr)(user_ctx);
}

}


namespace wasm_api
{

std::unique_ptr<WasmRuntime>
Stitch_WasmContext::new_runtime_instance(Script const& contract, void* ctxp) {
		return std::make_unique<WasmRuntime>(new Stitch_WasmRuntime(contract, context_pointer, ctxp));
}

std::pair<uint8_t*, uint32_t> 
Stitch_WasmRuntime::get_memory() {
	auto slice = ::get_memory(runtime_pointer);
	return {slice.mem, slice.size};

}
std::pair<const uint8_t*, uint32_t> 
Stitch_WasmRuntime::get_memory() const {
	auto slice = ::get_memory(runtime_pointer);
	return {slice.mem, slice.size};
}

uint64_t
Stitch_WasmRuntime::invoke(std::string const& method_name) {
	auto invoke_res = ::invoke(runtime_pointer, reinterpret_cast<const uint8_t*>(method_name.c_str()), static_cast<uint32_t>(method_name.size()));
	if (invoke_res.error) {
		throw UnrecoverableSystemError("internal stitch error");
	}
	return invoke_res.result;
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*)) {

	stitch_link_0args(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f);
}


}