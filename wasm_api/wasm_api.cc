#include "wasm_api/wasm_api.h"

#include "wasm_api/wasm3_api.h"

namespace wasm_api
{

WasmContext::WasmContext(const ScriptDB& script_db,
                         const uint32_t MAX_STACK_BYTES)
    : impl(new detail::Wasm3_WasmContext(script_db, MAX_STACK_BYTES))
{}

std::unique_ptr<WasmRuntime>
WasmContext::new_runtime_instance(Hash const& script_addr,
                                  const script_context_t& context)
{
    return impl->new_runtime_instance(script_addr, context);
}

WasmContext::~WasmContext()
{
    if (impl)
    {
        delete impl;
    }
    impl = nullptr;
}

WasmRuntime::WasmRuntime(detail::Wasm3_WasmRuntime* impl)
		: impl(impl)
		{}


WasmRuntime::~WasmRuntime()
{
    if (impl)
    {
        delete impl;
    }
    impl = nullptr;
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

template<typename ret>
ret
WasmRuntime::invoke(const char* method_name)
{
    return impl->template invoke<ret>(method_name);
}

template void
WasmRuntime::invoke<void>(const char* methodname);

template int32_t
WasmRuntime::invoke<int32_t>(const char* methodname);

template<typename ret, typename... Args>
void
WasmRuntime::_link_fn(const char* module,
                      const char* fn_name,
                      ret (*f)(Args...))
{
    impl->link_fn(module, fn_name, f);
}

template<typename... Args>
void
WasmRuntime::_link_fn(const char* module,
                      const char* fn_name,
                      void (*f)(Args...))
{
    impl->link_fn(module, fn_name, f);
}

#define LINK_DECL(...)                                                         \
    template void WasmRuntime::_link_fn<uint32_t, __VA_ARGS__>(                \
        const char* module, const char* fn_name, uint32_t (*f)(__VA_ARGS__));  \
    template void WasmRuntime::_link_fn<int32_t, __VA_ARGS__>(                 \
        const char* module, const char* fn_name, int32_t (*f)(__VA_ARGS__));   \
    template void WasmRuntime::_link_fn<__VA_ARGS__>(                          \
        const char* module, const char* fn_name, void (*f)(__VA_ARGS__));

LINK_DECL(uint32_t)
LINK_DECL(uint32_t, uint32_t)
LINK_DECL(uint32_t, uint32_t, uint32_t)
LINK_DECL(uint32_t, uint32_t, uint32_t, uint32_t)
LINK_DECL(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
LINK_DECL(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)

LINK_DECL(uint32_t, int64_t)
LINK_DECL(uint32_t, int64_t, int64_t)

LINK_DECL(uint32_t, uint64_t)
LINK_DECL(uint32_t, uint32_t, uint64_t)
LINK_DECL(uint32_t, uint64_t, uint32_t)

LINK_DECL(uint64_t)
LINK_DECL(uint64_t, uint32_t)
LINK_DECL(uint64_t, uint32_t, uint32_t)


template void WasmRuntime::_link_fn<uint32_t>(                
    const char* module, const char* fn_name, uint32_t (*f)(void));  
template void WasmRuntime::_link_fn<int32_t>(                 
    const char* module, const char* fn_name, int32_t (*f)(void)); 
template void WasmRuntime::_link_fn<uint64_t>(                
    const char* module, const char* fn_name, uint64_t (*f)(void));  
template void WasmRuntime::_link_fn<int64_t>(                 
    const char* module, const char* fn_name, int64_t (*f)(void));     
template void WasmRuntime::_link_fn<>(                          
    const char* module, const char* fn_name, void (*f)(void));


int32_t
WasmRuntime::memcmp(uint32_t lhs, uint32_t rhs, uint32_t max_len) const
{
    auto [mem, mlen] = get_memory();
    detail::check_bounds(mlen, std::max(lhs, rhs), max_len);
    return std::memcmp(mem + lhs, mem + rhs, max_len);
}

uint32_t
WasmRuntime::memset(uint32_t dst, uint8_t val, uint32_t len)
{
    auto [mem, mlen] = get_memory();
    detail::check_bounds(mlen, dst, len);
    std::memset(mem + dst, val, len);
    return dst;
}

// throws if memory regions overlap
uint32_t
WasmRuntime::safe_memcpy(uint32_t dst, uint32_t src, uint32_t len)
{
    auto [mem, mlen] = get_memory();

    // implicity checks overflows for src+len and dst+len
    detail::check_bounds(mlen, std::max(src, dst), len);

    if (dst <= src && dst + len >= src)
    {
        throw HostError("overlapping memcpy");
    }
    if (src <= dst && src + len >= dst)
    {
        throw HostError("overlapping memcpy");
    }

    if (src + len >= mlen || dst + len >= mlen)
    {
        throw HostError("OOB memcpy");
    }

    std::memcpy(mem + dst, mem + src, len);
    return dst;
}

void 
WasmRuntime::_write_to_memory(const uint8_t* src_ptr, uint32_t offset, uint32_t len)
{
	auto [mem, mlen] = get_memory();

	detail::check_bounds(mlen, offset, len);

	std::memcpy(mem + offset, src_ptr, len);
}

namespace detail
{

void
check_bounds(uint32_t mlen, uint32_t offset, uint32_t len)
{
    if (__builtin_add_overflow_p(
            offset, len, static_cast<uint32_t>(0)) // overflow
        || (mlen < offset + len))
    {
        throw HostError("OOB Mem Access: mlen = " + std::to_string(mlen)
                        + " offset = " + std::to_string(offset)
                        + " len = " + std::to_string(len));
    }
}

} // namespace detail

} // namespace wasm_api
