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

class MissingImportedFnTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_invoke.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    runtime = ctx->new_runtime_instance(s, nullptr);
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
}; 

TEST_P(MissingImportedFnTests, missing_import_fail)
{
  // runtime is either nullptr or gives error on initial call
  if (runtime) {
    auto res = runtime->invoke("calltest");
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::DETERMINISTIC_ERROR);
  }
}

INSTANTIATE_TEST_SUITE_P(AllEngines, MissingImportedFnTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY,
                            wasm_api::SupportedWasmEngine::WASMTIME_CRANELIFT,
                            wasm_api::SupportedWasmEngine::WASMTIME_WINCH));

HostFnStatus<uint64_t> import_fn(HostCallContext*, uint64_t arg) {
  return 2*arg;
}

class UnnecessaryImportedFnTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_invoke.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    ASSERT_TRUE(ctx -> link_fn("test", "redir_call", &import_fn));

    ASSERT_TRUE(ctx -> link_fn("test", "nexist", &import_fn));

    runtime = ctx->new_runtime_instance(s, nullptr);

    ASSERT_TRUE(!!runtime);
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
}; 

TEST_P(UnnecessaryImportedFnTests, unnecessary_import_success)
{
  auto res = runtime->invoke("calltest");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 24);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, UnnecessaryImportedFnTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY,
                            wasm_api::SupportedWasmEngine::WASMTIME_CRANELIFT,
                            wasm_api::SupportedWasmEngine::WASMTIME_WINCH));

} /* wasm_api */
