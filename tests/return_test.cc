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

HostFnStatus<uint64_t>
return_function(HostCallContext*) {
  return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::RETURN_SUCCESS};
}

class ReturnFnTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    contract = load_wasm_from_file("tests/wat/test_return.wasm");

    script = Script{.data = contract->data(), .len = contract->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    ASSERT_TRUE(ctx->link_fn("test", "return_fn", &return_function));
    runtime = ctx -> new_runtime_instance(script);

    ASSERT_TRUE(!!runtime);
  }

  std::unique_ptr<std::vector<uint8_t>> contract;
  Script script;

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;

  bool no_error_handling_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: error handling not supported in MAKEPAD_STITCH, aborting test\n");
        return true;
    }
    return false;
  }
}; 

#define ERROR_GUARD if (no_error_handling_shame()) return;

TEST_P(ReturnFnTests, check_userctx_correct)
{
  ERROR_GUARD
  auto res = runtime->invoke("returntest");
  ASSERT_TRUE(!res.result);
  EXPECT_EQ(res.result.error(), InvokeError::RETURN);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, ReturnFnTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                          wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                          wasm_api::SupportedWasmEngine::WASMI,
                          wasm_api::SupportedWasmEngine::FIZZY));

} /* wasm_api */
