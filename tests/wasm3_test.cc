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
#include <cstdint>

using namespace wasm_api;

void expect_res_eq(InvokeStatus<uint64_t> res, uint64_t const& val) {
	ASSERT_TRUE(!!res);
	EXPECT_EQ(*res, val);
}

// lol does this work
void expect_res(auto const& res) {
	EXPECT_TRUE(res.second == m3Err_none);
	EXPECT_TRUE(res.first.has_value());
}

TEST(wasm3, call)
{
	wasm3::environment e;

	auto c = ::test::load_wasm_from_file("tests/wat/test_call_simple.wasm");

	auto m = e.parse_module(c->data(), c->size());

	auto r = e.new_runtime(65536, nullptr);

	EXPECT_TRUE(r->load(*m));

	auto f = r->find_function("add");

	ASSERT_TRUE(f.has_value());

	EXPECT_EQ(f->call(UINT64_C(15), UINT64_C(20)), 35);
}



TEST(wasm3, set_memory)
{
	wasm3::environment e;
	auto c = ::test::load_wasm_from_file("tests/wat/test_set_memory.wasm");

	auto m = e.parse_module(c->data(), c->size());
	auto r = e.new_runtime(65536, nullptr);

	ASSERT_TRUE(!!m);
	ASSERT_TRUE(!!r);

	ASSERT_TRUE(r->load(*m));

	auto sz = r->find_function("size");

	ASSERT_TRUE(!!sz);

	auto sz_res = sz->call();
	ASSERT_TRUE(!!sz_res);
	EXPECT_EQ(*sz_res, 1u);

	auto store = r->find_function("store");

	EXPECT_TRUE(!!store->call(UINT64_C(0), UINT64_C(0x12345678'12345678)));

	EXPECT_TRUE(!!store->call(UINT64_C(8), UINT64_C(0xABCDEF90'ABCDEF90)));

	auto load = r->find_function("load");

	ASSERT_TRUE(load.has_value());

	auto mem0 = load->call(UINT64_C(0));

	expect_res_eq(mem0, 0x12345678'12345678);

	auto load8 = r->find_function("load8");

	ASSERT_TRUE(load8.has_value());

	expect_res_eq(load8->call(UINT64_C(0)), 0x78);
	expect_res_eq(load8->call(UINT64_C(1)), 0x56);

	auto load16 = r->find_function("load16");

	ASSERT_TRUE(load16.has_value());

	expect_res_eq(load16->call(UINT64_C(7)), 0x9012);

	auto mem = r->get_memory();

	EXPECT_TRUE(mem.size() == 65536u);

	uint64_t buf[2];
	buf[0] = 0x12345678'12345678;
	buf[1] = 0xABCDEF90'ABCDEF90;

	EXPECT_EQ(0, memcmp(reinterpret_cast<uint8_t*>(mem.data()), (uint8_t*)buf, 16));
} 
