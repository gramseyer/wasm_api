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

HostFnStatus<uint64_t> arity0(HostCallContext* ctx) {
  return 100;
}

HostFnStatus<uint64_t> arity1(HostCallContext* ctx, uint64_t arg0)
{
  if (arg0 != 1) {
    throw std::runtime_error("invalid arity check");
  }
  return 101;
}

HostFnStatus<uint64_t> arity2(HostCallContext* ctx, uint64_t arg0, uint64_t arg1)
{
  if (arg0 != 1) {
    throw std::runtime_error("invalid arity check");
  }
  return 102;
}

HostFnStatus<uint64_t> arity3(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3) {
    throw std::runtime_error("invalid arity check");
  }
  return 103;
}

HostFnStatus<uint64_t> arity4(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4) {
    throw std::runtime_error("invalid arity check");
  }
  return 104;
}


HostFnStatus<uint64_t> arity5(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5) {
    throw std::runtime_error("invalid arity check");
  }
  return 105;
}

HostFnStatus<uint64_t> arity6(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5 || arg5 != 6) {
    throw std::runtime_error("invalid arity check");
  }
  return 106;
}

HostFnStatus<uint64_t> arity7(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5 || arg5 != 6 || arg6 != 7) {
    throw std::runtime_error("invalid arity check");
  }
  return 107;
}
HostFnStatus<uint64_t> arity8(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5 || arg5 != 6 || arg6 != 7 || arg7 != 8) {
    throw std::runtime_error("invalid arity check");
  }
  return 108;
}

bool noret_called = false;

HostFnStatus<void> noret_arity0(HostCallContext* ctx) {
  noret_called = true;
  return {};
}

HostFnStatus<void> noret_arity1(HostCallContext* ctx, uint64_t arg0)
{
  if (arg0 != 1) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}

HostFnStatus<void> noret_arity2(HostCallContext* ctx, uint64_t arg0, uint64_t arg1)
{
  if (arg0 != 1) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}

HostFnStatus<void> noret_arity3(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}

HostFnStatus<void> noret_arity4(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}


HostFnStatus<void> noret_arity5(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}

HostFnStatus<void> noret_arity6(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5 || arg5 != 6) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}

HostFnStatus<void> noret_arity7(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5 || arg5 != 6 || arg6 != 7) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}

HostFnStatus<void> noret_arity8(HostCallContext* ctx, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7)
{
  if (arg0 != 1 || arg1 != 2 || arg2 != 3 || arg3 != 4 || arg4 != 5 || arg5 != 6 || arg6 != 7 || arg7 != 8) {
    throw std::runtime_error("invalid arity check");
  }
  return {};
}



using namespace test;

class InvokeArityTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_invoke_arity.wasm");
    uint32_t len = c->size();

    Script s {.data = c->data(), .len = len};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    ASSERT_TRUE(ctx->link_fn("test", "arg0", &arity0));
    ASSERT_TRUE(ctx->link_fn("test", "arg1", &arity1));
    ASSERT_TRUE(ctx->link_fn("test", "arg2", &arity2));
    ASSERT_TRUE(ctx->link_fn("test", "arg3", &arity3));
    ASSERT_TRUE(ctx->link_fn("test", "arg4", &arity4));
    ASSERT_TRUE(ctx->link_fn("test", "arg5", &arity5));
    ASSERT_TRUE(ctx->link_fn("test", "arg6", &arity6));
    ASSERT_TRUE(ctx->link_fn("test", "arg7", &arity7));
    ASSERT_TRUE(ctx->link_fn("test", "arg8", &arity8));

    ASSERT_TRUE(ctx->link_fn("test", "noret_arg0", &noret_arity0));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg1", &noret_arity1));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg2", &noret_arity2));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg3", &noret_arity3));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg4", &noret_arity4));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg5", &noret_arity5));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg6", &noret_arity6));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg7", &noret_arity7));
    ASSERT_TRUE(ctx->link_fn("test", "noret_arg8", &noret_arity8));

    runtime = ctx->new_runtime_instance(s, nullptr);

    ASSERT_TRUE(!!runtime);
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
};

TEST_P(InvokeArityTests, arity0)
{
  auto res = runtime->invoke("calltest0");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 100u);
}

TEST_P(InvokeArityTests, arity1)
{
  auto res = runtime->invoke("calltest1");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 101u);
}

TEST_P(InvokeArityTests, arity2)
{
  auto res = runtime->invoke("calltest2");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 102u);
}

TEST_P(InvokeArityTests, arity3)
{
  auto res = runtime->invoke("calltest3");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 103u);
}

TEST_P(InvokeArityTests, arity4)
{
  auto res = runtime->invoke("calltest4");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 104u);
}

TEST_P(InvokeArityTests, arity5)
{
  auto res = runtime->invoke("calltest5");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 105u);
}

TEST_P(InvokeArityTests, arity6)
{
  auto res = runtime->invoke("calltest6");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 106u);
}

TEST_P(InvokeArityTests, arity7)
{
  auto res = runtime->invoke("calltest7");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 107u);
}

TEST_P(InvokeArityTests, arity8)
{
  auto res = runtime->invoke("calltest8");
  ASSERT_TRUE(!!res.result);
  EXPECT_EQ(*res.result, 108u);
}

TEST_P(InvokeArityTests, noreturn)
{
  noret_called = false;
  auto res = runtime->invoke("callnoret0");
  EXPECT_TRUE(noret_called);
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg1)
{
  auto res = runtime->invoke("callnoret1");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg2)
{
  auto res = runtime->invoke("callnoret2");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg3)
{
  auto res = runtime->invoke("callnoret3");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg4)
{
  auto res = runtime->invoke("callnoret4");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg5)
{
  auto res = runtime->invoke("callnoret5");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg6)
{
  auto res = runtime->invoke("callnoret6");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg7)
{
  auto res = runtime->invoke("callnoret7");
  EXPECT_TRUE(!!res.result);
}

TEST_P(InvokeArityTests, noret_arg8)
{
  auto res = runtime->invoke("callnoret8");
  EXPECT_TRUE(!!res.result);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, InvokeArityTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY,
                            wasm_api::SupportedWasmEngine::WASMTIME_CRANELIFT,
                            wasm_api::SupportedWasmEngine::WASMTIME_WINCH));
} /* wasm_api */
