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

#include <catch2/catch_test_macros.hpp>

#include "wasm_api/wasm_api.h"

#include "tests/load_wasm.h"

namespace wasm_api
{

using namespace test;

TEST_CASE("memcpy", "[env][wasm_api]")
{
	auto c = load_wasm_from_file("tests/wat/test_set_memory.wasm");

	WasmContext ctx (65536);

	Script s {.data = c -> data(), .len = c -> size() };

	auto runtime = ctx.new_runtime_instance(s);

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
