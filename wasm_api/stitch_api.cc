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

}

namespace wasm_api
{

std::unique_ptr<WasmRuntime>
Stitch_WasmContext::new_runtime_instance(Script const& contract, void* ctxp) {
		// TODO ctx pointer
		return std::make_unique<WasmRuntime>(new Stitch_WasmRuntime(contract, context_pointer));
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


}