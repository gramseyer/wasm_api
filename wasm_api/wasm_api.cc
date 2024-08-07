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

#include "wasm_api/wasm3_api.h"
#include "wasm_api/stitch_api.h"
#include "wasm_api/wasmi_api.h"

#include <string.h>

namespace wasm_api
{

WasmContext::WasmContext(
                         const uint32_t MAX_STACK_BYTES, SupportedWasmEngine engine)
    : impl([&] () -> detail::WasmContextImpl* {
        switch (engine) {
        case SupportedWasmEngine::WASM3:
            return new Wasm3_WasmContext(MAX_STACK_BYTES);
        case SupportedWasmEngine::MAKEPAD_STITCH:
            return new Stitch_WasmContext();
        case SupportedWasmEngine::WASMI:
            return new Wasmi_WasmContext();
        default:
            return nullptr;
        }
    } ())
{}

std::unique_ptr<WasmRuntime>
WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    if (contract.data == nullptr) {
        return nullptr;
    }
    return impl->new_runtime_instance(contract, ctxp);
}

WasmContext::~WasmContext()
{
    if (impl)
    {
        delete impl;
    }
    impl = nullptr;
}

WasmRuntime::WasmRuntime(void* ctxp)
    : impl(nullptr)
    , host_call_context(this, ctxp)
    {}

void
WasmRuntime::initialize(detail::WasmRuntimeImpl* i)
{
    if (impl != nullptr) {
        throw std::runtime_error("double initialize");
    }
    impl = i;
}


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

    if (dst <= src && dst + len > src)
    {
        throw HostError("overlapping memcpy");
    }
    if (src <= dst && src + len > dst)
    {
        throw HostError("overlapping memcpy");
    }

    if (src + len > mlen || dst + len > mlen)
    {
        throw HostError("OOB memcpy");
    }

    std::memcpy(mem + dst, mem + src, len);
    return dst;
}

uint32_t
WasmRuntime::safe_strlen(uint32_t start, uint32_t max_len) const
{
    auto [mem, mlen] = get_memory();

    if (start > mlen) {
        return 0;
    }

    return strnlen(reinterpret_cast<const char*>(mem + start), std::min(max_len, mlen - start));
}


void 
WasmRuntime::_write_to_memory(const uint8_t* src_ptr, uint32_t offset, uint32_t len)
{
	auto [mem, mlen] = get_memory();

	detail::check_bounds(mlen, offset, len);

	std::memcpy(mem + offset, src_ptr, len);
}

void
WasmRuntime::consume_gas(uint64_t gas) {
    impl -> consume_gas(gas);
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
