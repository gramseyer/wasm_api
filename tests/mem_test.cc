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

class MemoryAccessTests : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {

 protected:
  void SetUp() override {
    auto c = load_wasm_from_file("tests/wat/test_set_memory.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    runtime = ctx->new_runtime_instance(s);

	buf = {0x01, 0x02, 0x03, 0x04};
	runtime->write_to_memory(buf, 10, 4);

  }

  std::vector<uint8_t> buf;
  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
}; 

TEST_P(MemoryAccessTests, good_memcpy)
{

		runtime -> safe_memcpy(0, 10, 4);
		std::vector<uint8_t> cmp;

		cmp = runtime -> template load_from_memory<decltype(cmp)>(0, 4);

		EXPECT_TRUE(cmp == buf);
	
}

TEST_P(MemoryAccessTests, identical_memcpy)
{
	EXPECT_ANY_THROW(runtime->safe_memcpy(10, 10, 4));
}

TEST_P(MemoryAccessTests, small_overlap_dst_before_src)
{
	EXPECT_ANY_THROW(runtime->safe_memcpy(9, 10, 4));
}

TEST_P(MemoryAccessTests, small_overlap_src_before_dst)
{
	EXPECT_ANY_THROW(runtime->safe_memcpy(10, 9, 4));
}

TEST_P(MemoryAccessTests, read_from_memory)
{
	std::vector<uint8_t> cmp = runtime -> template load_from_memory<std::vector<uint8_t>>(10, 4);
	EXPECT_EQ(cmp , buf);
}

TEST_P(MemoryAccessTests, read_fixed_size_buf)
{
	std::array<uint8_t, 4> arr;

	arr = runtime -> template load_from_memory_to_const_size_buf<decltype(arr)>(10);

	std::vector<uint8_t> cmp;
	cmp.insert(cmp.end(), arr.begin(), arr.end());

	EXPECT_EQ(cmp, buf);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, MemoryAccessTests,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                        	wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                        	wasm_api::SupportedWasmEngine::WASMI,
                        	wasm_api::SupportedWasmEngine::FIZZY));



} /* wasm_api */
