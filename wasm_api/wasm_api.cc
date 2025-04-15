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

#include "wasm_api/stitch_api.h"
#include "wasm_api/wasm3_api.h"
#include "wasm_api/wasmi_api.h"
#include "wasm_api/fizzy_api.h"
#include "wasm_api/wasmtime_api.h"

#include <string.h>

#include <cinttypes>
#include <cstring>

namespace wasm_api
{

std::string engine_to_string(SupportedWasmEngine engine)
{
	switch(engine) {
		case SupportedWasmEngine::WASM3:
			return "wasm3";
		case SupportedWasmEngine::MAKEPAD_STITCH:
			return "makepad_stitch";
		case SupportedWasmEngine::WASMI:
			return "wasmi";
		case SupportedWasmEngine::FIZZY:
			return "fizzy";
		case SupportedWasmEngine::WASMTIME_CRANELIFT:
			return "wasmtime cranelift";
        case SupportedWasmEngine::WASMTIME_WINCH:
            return "wasmtime winch";
	}
	throw std::runtime_error("unknown engine");
}

WasmContext::WasmContext(const uint32_t MAX_STACK_BYTES,
                         SupportedWasmEngine engine)
    : impl([&]() -> detail::WasmContextImpl* {
        switch (engine)
        {
            case SupportedWasmEngine::WASM3:
                return new Wasm3_WasmContext(MAX_STACK_BYTES);
            case SupportedWasmEngine::MAKEPAD_STITCH:
                return new Stitch_WasmContext();
            case SupportedWasmEngine::WASMI:
                return new Wasmi_WasmContext(MAX_STACK_BYTES);
            case SupportedWasmEngine::FIZZY:
                return new Fizzy_WasmContext(MAX_STACK_BYTES);
            case SupportedWasmEngine::WASMTIME_CRANELIFT:
                return new Wasmtime_WasmContext(MAX_STACK_BYTES, true);
            case SupportedWasmEngine::WASMTIME_WINCH:
                return new Wasmtime_WasmContext(MAX_STACK_BYTES, false);
            default:
                return nullptr;
        }
    }())
{
    if (impl) {
        if (!impl -> init_success()) {
            impl = nullptr;
        }
    }
}

std::unique_ptr<WasmRuntime>
WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    if (contract.data == nullptr)
    {
        return nullptr;
    }
    std::lock_guard lock(mtx);
    if (!impl) {
        return nullptr;
    }
    auto pre_link = impl->new_runtime_instance(contract, ctxp);
    if (!impl -> finish_link(pre_link)) {
        return nullptr;
    }
    return pre_link;
}

namespace detail {
bool 
WasmContextImpl::finish_link(std::unique_ptr<WasmRuntime>& pre_link)
{
    if (!pre_link) {
        return false;
    }
    for (auto const& entry : link_entries)
    {
        if (!pre_link -> link_fn(entry))
        {
            return false;
        }
    }
    return true;
}
}

WasmRuntime::WasmRuntime(void* ctxp)
    : impl(nullptr)
    , host_call_context(this, ctxp)
{}

void
WasmRuntime::initialize(detail::WasmRuntimeImpl* i)
{
    if (impl != nullptr)
    {
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

std::span<std::byte>
WasmRuntime::get_memory()
{
    if (impl) {
        return impl->get_memory();
    }
    return std::span<std::byte>();
}

std::span<const std::byte>
WasmRuntime::get_memory() const
{
    if (impl) {
        return impl->get_memory();
    }
    return std::span<const std::byte>();
}

MeteredReturn 
WasmRuntime::invoke(std::string const& method_name,
                                uint64_t gas_limit)
{
    if (!impl) {
        return { .result = InvokeStatus<uint64_t>(std::unexpect_t{}, InvokeError::UNRECOVERABLE), .gas_consumed = 0 };
    }
    uint64_t gas_backup = impl -> get_available_gas();

    impl -> set_available_gas(gas_limit);
    auto res = impl->invoke(method_name);
    uint64_t gas_remaining = impl -> get_available_gas();

    if (gas_limit < gas_remaining) {
        return MeteredReturn {
            .result = InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::UNRECOVERABLE},
            .gas_consumed = 0
        };
    }
    impl -> set_available_gas(gas_backup);

    return MeteredReturn {
        .result = res,
        .gas_consumed = gas_limit - gas_remaining
    };
}

bool
__attribute__((warn_unused_result))
WasmRuntime::consume_gas(uint64_t gas)
{
    if (impl) {
        return impl->consume_gas(gas);
    }
    return false;
}

uint64_t
WasmRuntime::get_available_gas() const
{
    if (impl) {
        return impl->get_available_gas();
    }
    return 0;
}

} // namespace wasm_api
