#include <gtest/gtest.h>

#include "wasm_api/wasm_api.h"
#include "wasm_api/error.h"

#include "tests/load_wasm.h"

using namespace wasm_api;
using namespace test;

HostFnStatus<uint64_t>
consume_gas_call(HostCallContext* ctxp)
{
    if (!ctxp -> runtime->consume_gas(100))
    {
        return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::OUT_OF_GAS};
    }
    return 0;
}

HostFnStatus<uint64_t>
consume_gas_call2(HostCallContext* ctxp)
{
    if (!ctxp -> runtime->consume_gas(5000))
    {
        return HostFnStatus<uint64_t>{std::unexpect_t{}, HostFnError::OUT_OF_GAS};
    }
    return 0;
}

class GasApiTest : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {
 protected:
  void SetUp() override {
    // this wasm just invokes an external call.  I'm sort of abusing it here.
    contract = load_wasm_from_file("tests/wat/test_error_handling.wasm");

    script = Script{.data = contract->data(), .len = contract->size()};

    ctx = std::make_unique<WasmContext>(65536, GetParam());

    ASSERT_TRUE(ctx->link_fn("test", "good_call", &consume_gas_call2)); // called by "call2"
    ASSERT_TRUE(ctx->link_fn("test", "external_call", &consume_gas_call)); // called by "call1"

    runtime = ctx -> new_runtime_instance(script);
    ASSERT_TRUE(!!runtime);
  }

  bool no_error_handling_shame() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: error handling not supported in MAKEPAD_STITCH, aborting test\n");
        return true;
    }
    return false;
  }

  std::unique_ptr<std::vector<uint8_t>> contract;
  Script script;

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

    auto res = runtime -> invoke("call1", 300);

    ASSERT_TRUE(!!res.result);

    EXPECT_GE(res.gas_consumed, 100);

    EXPECT_EQ(runtime->get_available_gas(), 5000);

    EXPECT_TRUE(runtime -> consume_gas(res.gas_consumed));

    ERROR_GUARD

    res = runtime -> invoke("call1", 80);
    ASSERT_FALSE(!!res.result);
    EXPECT_EQ(res.result.error(), InvokeError::OUT_OF_GAS_ERROR);
    EXPECT_EQ(res.gas_consumed, 80);
}

INSTANTIATE_TEST_SUITE_P(AllEngines, GasApiTest,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY));


