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

using namespace wasm_api;
using namespace test;

void
throw_runtime_error(void* ctxp)
{
    throw std::runtime_error("bad fn");
}

void
throw_host_error(void* ctxp)
{
    throw wasm_api::HostError("host error");
}

void
throw_bad_alloc(void* ctxp)
{
    throw std::bad_alloc();
}

void
good_call(void* ctxp)
{}

static WasmRuntime* s_runtime;

void
reentrance(void* ctxp)
{
    s_runtime->template invoke<void>("unreachable");
}

TEST_CASE("call external", "[wasm_api]")
{
    auto c = load_wasm_from_file("tests/wat/test_error_handling.wasm");

    Script s {.data = c->data(), .len = c->size()};

    WasmContext ctx(65536);

    auto runtime = ctx.new_runtime_instance(s);

    runtime->template link_fn<&good_call>("test", "good_call");

    SECTION("unlinked fn")
    {
        REQUIRE_THROWS(runtime->template invoke<void>("call1"));
    }

    SECTION("runtime error")
    {
        runtime->template link_fn<&throw_runtime_error>("test",
                                                        "external_call");
        REQUIRE_THROWS_AS(runtime->template invoke<void>("call1"),
                          wasm_api::UnrecoverableSystemError);
    }
    SECTION("host error")
    {
        runtime->template link_fn<&throw_host_error>("test", "external_call");
        REQUIRE_THROWS_AS(runtime->template invoke<void>("call1"),
                          wasm_api::HostError);
    }
    SECTION("other weird error")
    {
        runtime->template link_fn<&throw_bad_alloc>("test", "external_call");
        REQUIRE_THROWS_AS(runtime->template invoke<void>("call1"),
                          wasm_api::UnrecoverableSystemError);
    }

    SECTION("wasm error")
    {
        REQUIRE_THROWS_AS(runtime->template invoke<void>("unreachable"),
                          wasm_api::HostError);
    }

    SECTION("follow bad call with good")
    {
        runtime->template link_fn<&throw_host_error>("test", "external_call");
        REQUIRE_THROWS_AS(runtime->template invoke<void>("call1"),
                          wasm_api::HostError);

        runtime->template invoke<void>("call2");
    }

    SECTION("reentrance")
    {
        runtime->template link_fn<&reentrance>("test", "external_call");
        s_runtime = runtime.get();

        REQUIRE_THROWS_AS(runtime->template invoke<void>("call1"),
                          wasm_api::HostError);
    }
}

TEST_CASE("null handling", "[wasm_api]")
{
    WasmContext ctx(65535);

    REQUIRE(ctx.new_runtime_instance(null_script) == nullptr);
}

