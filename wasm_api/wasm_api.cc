#include "wasm_api/wasm_api.h"

#include "wasm_api/wasm3_api.h"

namespace wasm_api
{

//template<typename... ImplArgs>
//WasmRuntime::WasmRuntime(ImplArgs&&... args)
//	: impl(std::make_unique<detail::Wasm3_WasmRuntime>(std::move(args)...)
//	{}

WasmContext::WasmContext(const ScriptDB& script_db, const uint32_t MAX_STACK_BYTES)
	: impl(std::make_unique<detail::Wasm3_WasmContext>(script_db, MAX_STACK_BYTES))
	{}

std::unique_ptr<WasmRuntime>
WasmContext::new_runtime_instance(Hash const& script_addr)
{
	return impl->new_runtime_instance(script_addr);
}

std::pair<uint8_t*, uint32_t> 
WasmRuntime::get_memory()
{
	return impl->get_memory();
}

std::pair<const uint8_t*, uint32_t> 
WasmRuntime::get_memory() const
{
	return impl->get_memory();
}

int32_t 
WasmRuntime::invoke(const char* method_name)
{
	return impl->invoke(method_name);
}

template<typename... Args>
void 
WasmRuntime::_link_fn(
	const char* module,
	const char* fn_name,
	auto (*f)(Args...))
{
	impl->link_fn(module, fn_name, f);
}

template<>
void WasmRuntime::_link_fn<uint32_t, uint32_t>(
	const char* module,
	const char* fn_name,
	uint32_t (*f)(uint32_t, uint32_t));

int32_t 
WasmRuntime::memcmp(uint32_t lhs, uint32_t rhs, uint32_t max_len) const
{
	auto [mem, mlen] = get_memory();
	if (lhs + max_len > mlen || rhs + max_len > mlen)
	{
		throw HostError("OOB memcmp");
	}

	return std::memcmp(mem + lhs, mem + rhs, max_len);
}

uint32_t
WasmRuntime::memset(uint32_t dst, uint8_t val, uint32_t len)
{
	auto [mem, mlen] = get_memory();
	if (dst + len > mlen)
	{
		throw HostError("OOB memset");
	}
	std::memset(mem + dst, val, len);
	return dst;
}

// throws if memory regions overlap
uint32_t
WasmRuntime::safe_memcpy(uint32_t dst, uint32_t src, uint32_t len)
{
	auto [mem, mlen] = get_memory();
	if (dst <= src && dst + len >= src) {
		throw HostError("overlapping memcpy");
	}
	if (src <= dst && src + len >= dst) {
		throw HostError("overlapping memcpy");
	}

	if (src + len >= mlen || dst + len >= mlen) {
		throw HostError("OOB memcpy");
	}

	std::memcpy(mem + dst, mem + src, len);
	return dst;
}

} /* wasm_api */
