#pragma once

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

#include "wasm_api/error.h"
#include "wasm_api/value_type.h"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include <span>

namespace wasm_api {

typedef std::array<uint8_t, 32> Hash;

/* Does not own the underlying memory */
struct Script {
  uint8_t const *data;
  uint32_t len;
};

constexpr static Script null_script = Script{.data = nullptr, .len = 0};

class WasmRuntime;

struct HostCallContext {
  WasmRuntime *runtime = nullptr;
  void *user_ctx = nullptr;
};

struct MeteredReturn {
  InvokeStatus<uint64_t> result;
  uint64_t gas_consumed;
};

namespace detail {

template<typename T>
struct WasmValueTypeLookup;

template<>
struct WasmValueTypeLookup<void> {
    constexpr static WasmValueType VAL = WasmValueType::VOID;
};

template<>
struct WasmValueTypeLookup<uint64_t> {
    constexpr static WasmValueType VAL = WasmValueType::U64;
};

struct DefaultLinkEntry {
    std::string module_name;
    std::string fn_name;
    void* fn;
    uint8_t nargs;
    WasmValueType ret_type;
};

class WasmRuntimeImpl;
class WasmContextImpl {
public:

  virtual std::unique_ptr<WasmRuntime>
  new_runtime_instance(Script const &contract, void *ctxp, const Hash* script_identifier) = 0;

  virtual ~WasmContextImpl() {}

  // Expected function signature: HostFnStatus<uint64_t>(HostCallContext*, nargs repeated uint64)
  virtual bool link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs,
    WasmValueType ret_type) {
      std::lock_guard lock(link_entry_mutex);
        link_entries.emplace_back(
            module_name,
            fn_name,
            fn,
            nargs,
            ret_type);
    return true;
  }

  virtual bool init_success() { return true; }

  virtual bool finish_link(std::unique_ptr<WasmRuntime>& pre_link);

protected:
  WasmContextImpl() = default;

  std::mutex link_entry_mutex;

private:
  std::vector<DefaultLinkEntry> link_entries;

  WasmContextImpl(WasmContextImpl const &) = delete;
  WasmContextImpl(WasmContextImpl &&) = delete;
};

class WasmRuntimeImpl {
public:
  virtual std::span<std::byte> get_memory() = 0;
  virtual std::span<const std::byte> get_memory() const = 0;

  virtual InvokeStatus<uint64_t> invoke(std::string const &method_name) = 0;

  virtual bool __attribute__((warn_unused_result))
  consume_gas(uint64_t gas) = 0;

  virtual uint64_t get_available_gas() const = 0;
  virtual void set_available_gas(uint64_t gas) = 0;

  virtual bool link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs,
    WasmValueType ret_type) = 0;

  virtual ~WasmRuntimeImpl() {}

protected:
  WasmRuntimeImpl() = default;

private:
  WasmRuntimeImpl(WasmRuntimeImpl const &) = delete;
  WasmRuntimeImpl(WasmRuntimeImpl &&) = delete;
};

template<typename T>
concept VectorLike = requires(const T object) {
  object.data();
  object.size();
};

} // namespace detail

enum class SupportedWasmEngine {
  WASM3 = 0,
  MAKEPAD_STITCH = 1,
  WASMI = 2,
  FIZZY = 3,
  WASMTIME_CRANELIFT = 4,
  WASMTIME_WINCH = 5,
};

class WasmContext;

std::string engine_to_string(SupportedWasmEngine engine);
std::string engine_to_string(std::variant<SupportedWasmEngine, WasmContext> engine);

// Any context that can be safely shared between multiple runtimes.
// This is a wrapper around a shared_ptr, so can be copied/moved/etc.
// 
// Note to self: If you care about determinism (i.e. if used in groundhog),
// then nothing within a WasmContext implementation can affect a WasmRuntime's operation.
class WasmContext {
public:
  WasmContext(const uint32_t MAX_STACK_BYTES,
              SupportedWasmEngine engine = SupportedWasmEngine::WASM3);

  std::unique_ptr<WasmRuntime> new_runtime_instance(Script const &script,
                                                    void *ctxp,
                                                    const Hash* script_identifier = nullptr);

  template<typename ret_type, std::same_as<uint64_t>... Args>
  bool link_fn(std::string const& module_name, std::string const& fn_name,
               HostFnStatus<ret_type> (*f)(HostCallContext *, Args...))
  {
    if (!impl) {
        return false;
    }
    return impl -> link_fn_nargs(module_name, fn_name, reinterpret_cast<void *>(f),
                         (kArgCount<Args>() + ... + 0),
                         detail::WasmValueTypeLookup<ret_type>::VAL);
  }

  std::string engine() const {
    return engine_to_string(engine_type);
  }

  ~WasmContext() = default;

private:
  std::shared_ptr<detail::WasmContextImpl> impl;

  const SupportedWasmEngine engine_type;

  template<typename T> constexpr static auto kArgCount = [] { return 1; };
};

class WasmRuntime {
public:
  WasmRuntime(void *ctxp);

  // takes ownership of impl
  void initialize(detail::WasmRuntimeImpl *impl);

  HostCallContext *get_host_call_context() { return &host_call_context; }

  /**
   * Invoke pushes the current available gas limit onto the stack,
   * and then runs using the gas specified by gas_limit.
   * It restores the amount of gas after the invocation.
   * In most use-cases, caller should then deduct (return_value).gas_consumed
   * from some other gas limit.
   **/
  MeteredReturn invoke(std::string const &method_name,
                               uint64_t gas_limit = UINT64_MAX);

  bool link_fn(detail::DefaultLinkEntry const& entry)
  {
    if (!impl) {
        return false;
    }
    return impl -> link_fn_nargs(entry.module_name, entry.fn_name, reinterpret_cast<void *>(entry.fn),
        entry.nargs, entry.ret_type);
  }

  std::span<std::byte> get_memory();
  std::span<const std::byte> get_memory() const;

  /**
   * If consume_gas returns true, then gas was consumed successfully.
   * If not, as much gas as possible is still consumed.
   * (i.e. consumed is max(gas, get_available_gas()))
   */
  bool __attribute__((warn_unused_result)) consume_gas(uint64_t gas);
  uint64_t get_available_gas() const;
  void set_available_gas(uint64_t gas)
  {
    if (impl) {
      impl->set_available_gas(gas);
    }
  }

  ~WasmRuntime();

private:
  detail::WasmRuntimeImpl *impl;
  HostCallContext host_call_context;

  WasmRuntime(const WasmRuntime &) = delete;
  WasmRuntime(WasmRuntime &&) = delete;
  WasmRuntime &operator=(const WasmRuntime &) = delete;
  WasmRuntime &operator=(WasmRuntime &&) = delete;

  template<typename T> constexpr static auto kArgCount = [] { return 1; };
};

} // namespace wasm_api
