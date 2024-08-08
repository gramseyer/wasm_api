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

#include "wasm_api/wasm3.h"

#include "tests/load_wasm.h"

#include <cstring>

using namespace wasm_api;

TEST(wasm3, call)
{
	wasm3::environment e;

	auto c = ::test::load_wasm_from_file("tests/wat/test_call_simple.wasm");

	auto m = e.parse_module(c->data(), c->size());

	auto r = e.new_runtime(65536, nullptr);

	r->load(*m);

	auto f = r->find_function("add");

	int32_t res = f.template call<int32_t>(15, 20);
	EXPECT_EQ(res, 35);
}


TEST(wasm3, set_memory)
{
	wasm3::environment e;
	auto c = ::test::load_wasm_from_file("tests/wat/test_set_memory.wasm");

	auto m = e.parse_module(c->data(), c->size());

	auto r = e.new_runtime(65536, nullptr);

	r->load(*m);

	auto sz = r->find_function("size");

	EXPECT_TRUE(sz.template call<int32_t>() == 1);

	auto store = r->find_function("store");

	store.call(0, 0x12345678);

	store.call(4, 0xABCDEF90);

	auto load = r->find_function("load");

	uint32_t mem0 = load.template call<int32_t>(0);

	EXPECT_TRUE(mem0 == 0x12345678);

	auto load8 = r->find_function("load8");

	EXPECT_TRUE(load8.template call<int32_t>(0) == 0x78);
	EXPECT_TRUE(load8.template call<int32_t>(1) == 0x56);

	auto load16 = r->find_function("load16");

	EXPECT_TRUE(load16.template call<int32_t>(3) == 0x9012);

	auto [mem_ptr, mlen] = r->get_memory();

	EXPECT_TRUE(mlen == 65536);

	uint32_t buf[2];
	buf[0] = 0x12345678;
	buf[1] = 0xABCDEF90;

	EXPECT_TRUE(0 == memcmp(mem_ptr, (uint8_t*)buf, 8));
}
