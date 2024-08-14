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

uint64_t arity0(HostCallContext* ctx) {
  return 100;
}

uint64_t arity1(HostCallContext* ctx, uint64_t arg0)
{
  if (arg0 != 1) {
    throw HostError("invalid arity check");
  }
  return 101;
}

uint64_t arity2(HostCallContext* ctx, uint64_t arg0, uint64_t arg1)
{
  if (arg0 != 1) {
    throw HostError("invalid arity check");
  }
  return 102;
}

uint64_t arity3(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3) {
    throw HostError("invalid arity check");
  }
  return 103;
}

uint64_t arity4(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4) {
    throw HostError("invalid arity check");
  }
  return 104;
}


uint64_t arity5(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5) {
    throw HostError("invalid arity check");
  }
  return 105;
}



using namespace test;

class InvokeArityTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_invoke_arity.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    runtime = ctx->new_runtime_instance(s, nullptr);

    runtime->template link_fn<&arity0>("test", "arg0");
    runtime->template link_fn<&arity1>("test", "arg1");
    runtime->template link_fn<&arity2>("test", "arg2");
    runtime->template link_fn<&arity3>("test", "arg3");
    runtime->template link_fn<&arity4>("test", "arg4");
    runtime->template link_fn<&arity5>("test", "arg5");

  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
}; 

TEST_P(InvokeArityTests, arity0)
{
  auto res = runtime->template invoke<uint64_t>("calltest0");
  EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
  ASSERT_TRUE(res.out.has_value());

  EXPECT_EQ(*res.out, 100);
}

TEST_P(InvokeArityTests, arity1)
{
  auto res = runtime->template invoke<uint64_t>("calltest1");
  EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
  ASSERT_TRUE(res.out.has_value());

  EXPECT_EQ(*res.out, 101);
}

TEST_P(InvokeArityTests, arity2)
{
  auto res = runtime->template invoke<uint64_t>("calltest2");
  EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
  ASSERT_TRUE(res.out.has_value());

  EXPECT_EQ(*res.out, 102);
}

TEST_P(InvokeArityTests, arity3)
{
  auto res = runtime->template invoke<uint64_t>("calltest3");
  EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
  ASSERT_TRUE(res.out.has_value());

  EXPECT_EQ(*res.out, 103);
}

TEST_P(InvokeArityTests, arity4)
{
  auto res = runtime->template invoke<uint64_t>("calltest4");
  EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
  ASSERT_TRUE(res.out.has_value());

  EXPECT_EQ(*res.out, 104);
}


TEST_P(InvokeArityTests, arity5)
{
  auto res = runtime->template invoke<uint64_t>("calltest5");
  EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
  ASSERT_TRUE(res.out.has_value());

  EXPECT_EQ(*res.out, 105);
}



INSTANTIATE_TEST_SUITE_P(AllEngines, InvokeArityTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                          wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                          wasm_api::SupportedWasmEngine::WASMI,
                          wasm_api::SupportedWasmEngine::FIZZY));

} /* wasm_api */
