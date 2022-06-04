#include <catch2/catch_test_macros.hpp>

#include "wasm_api/wasm_api.h"

#include "tests/phony_script_db.h"
#include "tests/load_wasm.h"


namespace wasm_api
{

using namespace test;

TEST_CASE("memcpy", "[env]")
{
	PhonyScriptDB scripts;

	auto h = make_hash(0);
	auto c = load_wasm_from_file("tests/wat/test_set_memory.wasm");
	scripts.add_script(h, c);

	WasmContext ctx(scripts, 65536);


	auto runtime = ctx.new_runtime_instance(h);

	std::vector<uint8_t> buf = {0x01, 0x02, 0x03, 0x04};

	runtime->write_to_memory(buf, 10, 4);

	SECTION("good memcpy")
	{
		runtime -> safe_memcpy(0, 10, 4);
		std::vector<uint8_t> cmp;

		cmp = runtime -> template load_from_memory<decltype(cmp)>(0, 4);

		REQUIRE(cmp == buf);
	}

	SECTION("identical location memcpy")
	{
		REQUIRE_THROWS(runtime -> safe_memcpy(10, 10, 4));
	}

	SECTION("small overlap memcpy dst < src")
	{
		REQUIRE_THROWS(runtime -> safe_memcpy(9, 10, 4));
	}

	SECTION("small overlap memcpy src < dst")
	{
		REQUIRE_THROWS(runtime -> safe_memcpy(11, 10, 4));
	}

	SECTION("read from memory")
	{
		std::vector<uint8_t> cmp = runtime -> template load_from_memory<std::vector<uint8_t>>(10, 4);
		REQUIRE(cmp == buf);
	}
	SECTION("read fixed size buf")
	{
		std::array<uint8_t, 4> arr;

		arr = runtime -> template load_from_memory_to_const_size_buf<decltype(arr)>(10);

		std::vector<uint8_t> cmp;
		cmp.insert(cmp.end(), arr.begin(), arr.end());

		REQUIRE(cmp == buf);
	}
}


} /* wasm_api */
