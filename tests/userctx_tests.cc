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

void* ctx_check = (void*) (0xAABBCCDD'EEFF0011);

uint64_t foo(void* ctx, uint64_t value)
{
	EXPECT_TRUE(ctx == ctx_check);
	EXPECT_TRUE(value == 12);
	return 15;
}

uint32_t expect = 0;

TEST(wasm3, userctx)
{
	auto c = load_wasm_from_file("tests/wat/test_invoke.wasm");

	WasmContext ctx (65536);

	Script s {.data = c -> data(), .len = c -> size() };

	auto runtime = ctx.new_runtime_instance(s, ctx_check);

	runtime->template link_fn<&foo>("test", "redir_call");

	EXPECT_EQ(runtime->template invoke<uint32_t>("calltest"), 15u);
}


} /* wasm_api */
