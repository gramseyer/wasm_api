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
		cmp.resize(4);

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
}


} /* wasm_api */
