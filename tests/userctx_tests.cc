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

namespace wasm_api
{

using namespace test;

void* ctx_check = (void*) (0xAABBCCDD'EEFF0011);

HostFnStatus<uint64_t>
foo(HostCallContext* ctx, uint64_t value)
{
	EXPECT_TRUE(ctx->user_ctx == ctx_check);
	EXPECT_TRUE(value == 12);
	return 15;
}

class UserCtxTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_invoke.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());
    
    runtime = ctx->new_runtime_instance(s, ctx_check);
    ASSERT_TRUE(!!runtime);

    ASSERT_TRUE(runtime->link_fn("test", "redir_call", &foo));
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
}; 

uint32_t expect = 0;

TEST_P(UserCtxTests, check_userctx_correct)
{
  auto res = runtime->invoke("calltest");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 15u);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, UserCtxTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                          wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                          wasm_api::SupportedWasmEngine::WASMI,
                          wasm_api::SupportedWasmEngine::FIZZY));

} /* wasm_api */
