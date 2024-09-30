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

//#include "wasm_api/stitch_api.h"
#include "wasm_api/wasm3_api.h"
//#include "wasm_api/wasmi_api.h"
#include "wasm_api/fizzy_api.h"

#include <string.h>

#include <cinttypes>

namespace wasm_api
{

WasmContext::WasmContext(const uint32_t MAX_STACK_BYTES,
                         SupportedWasmEngine engine)
    : impl([&]() -> detail::WasmContextImpl* {
        switch (engine)
        {
            case SupportedWasmEngine::WASM3:
                return new Wasm3_WasmContext(MAX_STACK_BYTES);
          //  case SupportedWasmEngine::MAKEPAD_STITCH:
          //      return new Stitch_WasmContext();
          //  case SupportedWasmEngine::WASMI:
          //      return new Wasmi_WasmContext(MAX_STACK_BYTES);
            case SupportedWasmEngine::FIZZY:
                return new Fizzy_WasmContext(MAX_STACK_BYTES);
            default:
                return nullptr;
        }
    }())
{}

std::unique_ptr<WasmRuntime>
WasmContext::new_runtime_instance(Script const& contract, void* ctxp)
{
    if (contract.data == nullptr)
    {
        return nullptr;
    }
    if (impl) {
        return impl->new_runtime_instance(contract, ctxp);
    }
    std::printf("failed to set impl\n");
    return nullptr;
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

detail::MeteredReturn 
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
        return detail::MeteredReturn {
            .result = InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::UNRECOVERABLE},
            .gas_consumed = 0
        };
    }
    impl -> set_available_gas(gas_backup);

    return {
        res,
        gas_limit - gas_remaining
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
