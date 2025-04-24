#include <gtest/gtest.h>

#include "wasm_api/wasm_api.h"
#include "wasm_api/error.h"

#include "tests/load_wasm.h"

using namespace wasm_api;
using namespace test;


class NoStartTest : public ::testing::TestWithParam<wasm_api::SupportedWasmEngine> {
 protected:
  void SetUp() override {
    // this wasm just invokes an external call.  I'm sort of abusing it here.
    contract = load_wasm_from_file("tests/wat/test_no_start.wasm");
    uint32_t len = contract->size();

    script = Script{.data = contract->data(), .len = len};

    ctx = std::make_unique<WasmContext>(65536, GetParam());
  }

  std::unique_ptr<std::vector<uint8_t>> contract;
  Script script;

  std::unique_ptr<WasmContext> ctx;
  std::unique_ptr<WasmRuntime> runtime;

  bool no_start_check() {
    if (GetParam() == wasm_api::SupportedWasmEngine::MAKEPAD_STITCH) {
        std::printf("SHAME: aborting test because MAKEPAD_STITCH has no start check or option flag\n");
        return true;
    }
    return false;
  }
};

#define START_GUARD if (no_start_check()) return;

TEST_P(NoStartTest, set_get_noinvoke)
{
    START_GUARD

    runtime = ctx -> new_runtime_instance(script, nullptr);
    // expected behavior:
    // Either instantiation fails (no start allowed) or start() is not run

    if (runtime) {
        auto mem = runtime -> get_memory();

        ASSERT_TRUE(mem.size() > 8);

        uint64_t first_bytes = *reinterpret_cast<uint64_t*>(mem.data());
        EXPECT_EQ(first_bytes, 0u);
    }
}

INSTANTIATE_TEST_SUITE_P(AllEngines, NoStartTest,
                        ::testing::Values(wasm_api::SupportedWasmEngine::WASM3, 
                            wasm_api::SupportedWasmEngine::MAKEPAD_STITCH,
                            wasm_api::SupportedWasmEngine::WASMI,
                            wasm_api::SupportedWasmEngine::FIZZY,
                            wasm_api::SupportedWasmEngine::WASMTIME_CRANELIFT,
                            wasm_api::SupportedWasmEngine::WASMTIME_WINCH));

