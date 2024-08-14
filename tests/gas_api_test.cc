#include <gtest/gtest.h>

#include "wasm_api/wasm_api.h"

#include "tests/load_wasm.h"

using namespace wasm_api;
using namespace test;

uint64_t
consume_gas_call(HostCallContext* ctxp)
{
    if (!ctxp -> runtime->consume_gas(100))
    {
        throw wasm_api::HostError("out of gas");
    }
    return 0;
}

uint64_t
consume_gas_call2(HostCallContext* ctxp)
{
    if (!ctxp -> runtime->consume_gas(5000))
    {
        throw wasm_api::HostError("out of gas");
    }
    return 0;
}

class GasApiTest : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {
 protected:
  void SetUp() override {
    // this wasm just invokes an external call.  I'm sort of abusing it here.
    auto c = load_wasm_from_file("tests/wat/test_error_handling.wasm");

    Script s {.data = c->data(), .len = c->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    runtime = ctx->new_runtime_instance(s);

    runtime->template link_fn<&consume_gas_call2>("test", "good_call"); // called by "call2"
    runtime->template link_fn<&consume_gas_call>("test", "external_call"); // called by "call1"
  }

  bool no_error_handling_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: error handling not supported in MAKEPAD_STITCH, aborting test\n");
        return true;
    }
    return false;
  }

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;
};

#define ERROR_GUARD if (no_error_handling_shame()) return;

TEST_P(GasApiTest, set_get_noinvoke)
{
    EXPECT_EQ(runtime -> get_available_gas(), 0);

    runtime -> set_available_gas(100);

    EXPECT_TRUE(runtime -> consume_gas(60));

    EXPECT_EQ(runtime -> get_available_gas(), 40);

    EXPECT_FALSE(runtime -> consume_gas(50));

    EXPECT_EQ(runtime -> get_available_gas(), 0);
}

TEST_P(GasApiTest, invoke_resets_gas)
{
    runtime -> set_available_gas(5000);

    auto res = runtime -> template invoke<uint64_t>("call1", 300);

    EXPECT_EQ(res.panic, wasm_api::ErrorType::None);
    EXPECT_GE(res.consumed_gas, 100);

    EXPECT_EQ(runtime->get_available_gas(), 5000);

    EXPECT_TRUE(runtime -> consume_gas(res.consumed_gas));

    ERROR_GUARD

    res = runtime -> template invoke<uint64_t>("call1", 80);
    EXPECT_EQ(res.panic, wasm_api::ErrorType::HostError);
    EXPECT_EQ(res.consumed_gas, 80);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, GasApiTest,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI));


