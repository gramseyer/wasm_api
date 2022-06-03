#include <catch2/catch_test_macros.hpp>

#include "wasm_api/wasm3.h"

#include "tests/load_wasm.h"

using namespace wasm_api;
using namespace test;

TEST_CASE("call simple", "[wasm3]")
{
	wasm3::environment e;
	
	auto c = load_wasm_from_file("tests/wat/test_call_simple.wasm");

	auto m = e.parse_module(c->data(), c->size());

	auto r = e.new_runtime(65536);

	r->load(*m);

	auto f = r->find_function("add");

	int32_t res = f.template call<int32_t>(15, 20);
	REQUIRE(res == 35);
}


TEST_CASE("set memory", "[wasm3]")
{
	wasm3::environment e;
	auto c = load_wasm_from_file("tests/wat/test_set_memory.wasm");

	auto m = e.parse_module(c->data(), c->size());

	auto r = e.new_runtime(65536);

	r->load(*m);

	auto sz = r->find_function("size");

	REQUIRE(sz.template call<int32_t>() == 1);

	auto store = r->find_function("store");

	store.call(0, 0x12345678);

	store.call(4, 0xABCDEF90);

	auto load = r->find_function("load");

	uint32_t mem0 = load.template call<int32_t>(0);

	REQUIRE(mem0 == 0x12345678);

	auto load8 = r->find_function("load8");

	REQUIRE(load8.template call<int32_t>(0) == 0x78);
	REQUIRE(load8.template call<int32_t>(1) == 0x56);

	auto load16 = r->find_function("load16");

	REQUIRE(load16.template call<int32_t>(3) == 0x9012);

	auto [mem_ptr, mlen] = r->get_memory();

	REQUIRE(mlen == 65536);

	uint32_t buf[2];
	buf[0] = 0x12345678;
	buf[1] = 0xABCDEF90;

	REQUIRE(0 == memcmp(mem_ptr, (uint8_t*)buf, 8));
}
