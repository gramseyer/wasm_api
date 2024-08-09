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

uint64_t
throw_runtime_error(HostCallContext* ctxp)
{
    throw std::runtime_error("bad fn\n");
}

uint64_t
throw_host_error(HostCallContext* ctxp)
{
    throw wasm_api::HostError("host error\n");
}

uint64_t
throw_bad_alloc(HostCallContext* ctxp)
{
    throw std::bad_alloc();
}

uint64_t
good_call(HostCallContext* ctxp)
{ 
    return 0;
}

static WasmRuntime* s_runtime;
static bool reentrance_hit = false;

uint64_t
reentrance(HostCallContext* ctxp)
{
    reentrance_hit = true;
    s_runtime->template invoke<void>("unreachable");
    return 0;
}

class ExternalCallTest : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {
 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_error_handling.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    runtime = ctx->new_runtime_instance(s);

    runtime->template link_fn<&good_call>("test", "good_call");
  }

  bool no_error_handling_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: error handling not supported in MAKEPAD_STITCH, aborting test\n");
        return true;
    }
    return false;
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
};

#define ERROR_GUARD if (no_error_handling_shame()) return;

TEST_P(ExternalCallTest, unlinked_fn)
{
    EXPECT_ANY_THROW(runtime -> template invoke<void>("call1"));
}

TEST_P(ExternalCallTest, runtime_error)
{
    runtime->template link_fn<&throw_runtime_error>("test",
                                                        "external_call");
    ERROR_GUARD
    EXPECT_THROW(runtime->template invoke<void>("call1"),
                          wasm_api::UnrecoverableSystemError);
}


TEST_P(ExternalCallTest, host_error)
{
    runtime->template link_fn<&throw_host_error>("test", "external_call");
    ERROR_GUARD
    EXPECT_THROW(runtime->template invoke<void>("call1"),
                      wasm_api::HostError);
}

TEST_P(ExternalCallTest, other_weird_error)
{
    runtime->template link_fn<&throw_bad_alloc>("test", "external_call");
    ERROR_GUARD
    EXPECT_THROW(runtime->template invoke<void>("call1"),
                      wasm_api::UnrecoverableSystemError);
}



TEST_P(ExternalCallTest, wasm_error)
    {
        // link all the fns, required for MAKEPAD_STITCH
        runtime->template link_fn<&good_call>("test", "external_call");

        EXPECT_THROW(runtime->template invoke<void>("unreachable"),
                          wasm_api::HostError);
    }

TEST_P(ExternalCallTest, follow_bad_with_good)
{
    runtime->template link_fn<&throw_host_error>("test", "external_call");

    ERROR_GUARD
    EXPECT_THROW(runtime->template invoke<void>("call1"),
                      wasm_api::HostError);

    runtime->template invoke<void>("call2");
}

TEST_P(ExternalCallTest, reentrance)
    {
        runtime->template link_fn<&reentrance>("test", "external_call");
        s_runtime = runtime.get();

        ERROR_GUARD
        EXPECT_THROW(runtime->template invoke<void>("call1"),
                          wasm_api::HostError);
        EXPECT_TRUE(reentrance_hit);
    }

TEST_P(ExternalCallTest, null_handling)
{
    EXPECT_TRUE(ctx->new_runtime_instance(null_script) == nullptr);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, ExternalCallTest,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI));


