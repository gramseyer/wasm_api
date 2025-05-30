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

#include <gtest/gtest.h>

#include "wasm_api/wasm_api.h"

#include "tests/load_wasm.h"

using namespace wasm_api;
using namespace test;

HostFnStatus<uint64_t>
throw_runtime_error(HostCallContext* ctxp)
{
    throw std::runtime_error("bad fn\n");
}

HostFnStatus<uint64_t>
throw_host_error(HostCallContext* ctxp)
{
    return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::OUT_OF_GAS};
}

HostFnStatus<uint64_t>
throw_bad_alloc(HostCallContext* ctxp)
{
    throw std::bad_alloc();
}

HostFnStatus<uint64_t>
good_call(HostCallContext* ctxp)
{ 
    return 0;
}

HostFnStatus<uint64_t>
nonzero_return(HostCallContext* ctxp)
{
    return 100;
}

static WasmRuntime* s_runtime;
static bool reentrance_hit = false;

HostFnStatus<uint64_t>
reentrance(HostCallContext* ctxp)
{
    reentrance_hit = true;
    auto invoke_res = s_runtime->invoke("unreachable");

    if (invoke_res.result.has_value()) {
        return *invoke_res.result;
    }
    switch(invoke_res.result.error()) {
    case InvokeError::NONE:
        throw std::runtime_error("impossible");
    case InvokeError::DETERMINISTIC_ERROR:
        return 1;
    case InvokeError::OUT_OF_GAS_ERROR:
        return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::OUT_OF_GAS};
    case InvokeError::RETURN:
        return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::RETURN_SUCCESS};
    default:
        return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::UNRECOVERABLE};
    }
}

class ExternalCallTest : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {
 protected:
  void SetUp() override {
    contract = load_wasm_from_file("tests/wat/test_error_handling.wasm");
    uint32_t len = contract->size();

    script =  Script {.data = contract->data(), .len = len};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    ASSERT_TRUE(ctx->link_fn("test", "good_call", &good_call));
  }

  bool no_error_handling_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: error handling not supported in MAKEPAD_STITCH, aborting test\n");
        return true;
    }
    return false;
  }

  bool terminate_on_syserror_shame() {
    return false;
  }

  std::unique_ptr<std::vector<uint8_t>> contract;
  Script script;

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
};

#define ERROR_GUARD if (no_error_handling_shame()) return;
#define UNRECOVERABLEGUARD if (terminate_on_syserror_shame()) return;

TEST_P(ExternalCallTest, unlinked_fn)
{
    runtime = ctx -> new_runtime_instance(script, nullptr);
    
    // either runtime == nullptr, or error later is allowed
    if (runtime) {
        auto res = runtime ->invoke("call1");
        ASSERT_FALSE(!!res.result);
        EXPECT_EQ(res.result.error(), InvokeError::DETERMINISTIC_ERROR);
    }
}

TEST_P(ExternalCallTest, runtime_error)
{
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &throw_runtime_error));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);

    ERROR_GUARD
    UNRECOVERABLEGUARD
    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::UNRECOVERABLE);
}


TEST_P(ExternalCallTest, host_error)
{
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &throw_host_error));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);
    
    ERROR_GUARD

    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::OUT_OF_GAS_ERROR);
}

TEST_P(ExternalCallTest, other_weird_error)
{
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &throw_bad_alloc));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);

    ERROR_GUARD
    UNRECOVERABLEGUARD
    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::UNRECOVERABLE);
}


TEST_P(ExternalCallTest, wasm_error)
{
    // link all the fns, required for MAKEPAD_STITCH
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &good_call));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);

    auto res = runtime -> invoke("unreachable");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::DETERMINISTIC_ERROR);
}

TEST_P(ExternalCallTest, follow_bad_with_good)
{
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &throw_host_error));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);

    ERROR_GUARD
    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::OUT_OF_GAS_ERROR);

    res = runtime->invoke("call2");
    EXPECT_TRUE(!!res.result);
}

TEST_P(ExternalCallTest, reentrance)
{
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &reentrance));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);

    s_runtime = runtime.get();

    ERROR_GUARD

    auto res = runtime->invoke("call1");
    ASSERT_TRUE(!!res.result);
    // gracefully handle error in subcall, here meaning return 1
    EXPECT_EQ(*res.result, 1u);
    EXPECT_TRUE(reentrance_hit);
}

TEST_P(ExternalCallTest, nonzero_return)
{
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &nonzero_return));
    runtime = ctx -> new_runtime_instance(script, nullptr);
    ASSERT_TRUE(!!runtime);

    ERROR_GUARD

    auto res = runtime->invoke("call1");
    ASSERT_TRUE(!!res.result);
    // gracefully handle error in subcall, here meaning return 1
    EXPECT_EQ(*res.result, 100u);
}

TEST_P(ExternalCallTest, ModuleCacheSanityCheck) {
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &nonzero_return));
    Hash h{}; // doesn't matter what the hash actually is
    h[10] = 20;
    h[20] = 30;
    runtime = ctx -> new_runtime_instance(script, nullptr, &h);
    ASSERT_TRUE(runtime);
    ERROR_GUARD

    // invoke once
    auto res = runtime->invoke("call1");
    ASSERT_TRUE(!!res.result);
    EXPECT_EQ(*res.result, 100u);

    runtime = ctx -> new_runtime_instance(script, nullptr, &h);
    ASSERT_TRUE(runtime);
    res = runtime->invoke("call1");
    ASSERT_TRUE(!!res.result);
    EXPECT_EQ(*res.result, 100u);
}


TEST_P(ExternalCallTest, null_handling)
{
    EXPECT_TRUE(ctx->new_runtime_instance(null_script, nullptr) == nullptr);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, ExternalCallTest,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY,
                            wasm_api::SupportedWasmEngine::WASMTIME_CRANELIFT,
                            wasm_api::SupportedWasmEngine::WASMTIME_WINCH));


