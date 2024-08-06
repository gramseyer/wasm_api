#include "wasm_api/stitch_api.h"

struct MemorySlice {
    uint8_t* mem;
    uint32_t size;
};

extern "C" {

MemorySlice get_memory(void* runtime_pointer);

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


}