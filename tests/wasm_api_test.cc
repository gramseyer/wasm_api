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

static WasmRuntime* s_runtime;
static bool reentrance_hit = false;

HostFnStatus<uint64_t>
reentrance(HostCallContext* ctxp)
{
    reentrance_hit = true;
    auto invoke_res = s_runtime->invoke("unreachable");

    std::printf("got here\n");


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
    auto c = load_wasm_from_file("tests/wat/test_error_handling.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    runtime = ctx->new_runtime_instance(s);

    ASSERT_TRUE(!!runtime);

    ASSERT_TRUE(runtime->template link_fn("test", "good_call", &good_call));
  }

  bool no_error_handling_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: error handling not supported in MAKEPAD_STITCH, aborting test\n");
        return true;
    }
    return false;
  }

  bool terminate_on_syserror_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::FIZZY) {
        std::printf("SHAME: I haven't bothered forking Fizzy to support different kinds of error signaling\n");
        return true;
    }
    return false;
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
};

#define ERROR_GUARD if (no_error_handling_shame()) return;
#define UNRECOVERABLEGUARD if (terminate_on_syserror_shame()) return;

TEST_P(ExternalCallTest, unlinked_fn)
{
    auto res = runtime ->invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::DETERMINISTIC_ERROR);
}

TEST_P(ExternalCallTest, runtime_error)
{
    runtime->link_fn("test",
                                                        "external_call",
                                                        &throw_runtime_error);
    ERROR_GUARD
    UNRECOVERABLEGUARD
    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::UNRECOVERABLE);
}


TEST_P(ExternalCallTest, host_error)
{
    runtime->link_fn("test", "external_call", &throw_host_error);
    ERROR_GUARD

    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::OUT_OF_GAS_ERROR);
}

TEST_P(ExternalCallTest, other_weird_error)
{
    runtime->link_fn("test", "external_call", &throw_bad_alloc);
    ERROR_GUARD
    UNRECOVERABLEGUARD
    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::UNRECOVERABLE);
}


TEST_P(ExternalCallTest, wasm_error)
{
    // link all the fns, required for MAKEPAD_STITCH
    runtime->link_fn("test", "external_call", &good_call);

    auto res = runtime -> invoke("unreachable");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::DETERMINISTIC_ERROR);
}

TEST_P(ExternalCallTest, follow_bad_with_good)
{
    runtime->link_fn("test", "external_call", &throw_host_error);

    ERROR_GUARD
    auto res = runtime -> invoke("call1");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::OUT_OF_GAS_ERROR);

    res = runtime->invoke("call2");
    EXPECT_TRUE(!!res.result);
}

TEST_P(ExternalCallTest, reentrance)
{
    runtime->link_fn("test", "external_call", &reentrance);
    s_runtime = runtime.get();

    ERROR_GUARD

    auto res = runtime->invoke("call1");
    ASSERT_TRUE(!!res.result);
    // gracefully handle error in subcall, here meaning return 1
    EXPECT_EQ(*res.result, 1);
    EXPECT_TRUE(reentrance_hit);
}

TEST_P(ExternalCallTest, null_handling)
{
    EXPECT_TRUE(ctx->new_runtime_instance(null_script) == nullptr);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, ExternalCallTest,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY));


