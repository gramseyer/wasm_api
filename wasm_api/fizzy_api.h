#pragma once

#include "wasm_api/wasm_api.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <fizzy/fizzy.h>

namespace wasm_api {

struct FizzyTrampolineHostContext {
  void *fn_pointer;
  HostCallContext *real_context;
};

// recreate FizzyFunctionImport,
// but with control over data lifetimes
struct FizzyLazyFunctionImport {
  std::string module_name;
  std::string method_name;

  // make up FizzyFunctionType
  std::vector<FizzyValueType> inputs;
  FizzyValueType output;

  std::unique_ptr<FizzyTrampolineHostContext> trampoline_ctx;

  FizzyImportedFunction to_imported_function() const;
};

class Fizzy_WasmRuntime;

class Fizzy_WasmContext : public detail::WasmContextImpl {
public:
  Fizzy_WasmContext(uint32_t max_stack_bytes);

  ~Fizzy_WasmContext();

  std::unique_ptr<WasmRuntime> new_runtime_instance(Script const &contract,
                                                    void *ctxp);

private:
};

class Fizzy_WasmRuntime : public detail::WasmRuntimeImpl {
public:
  Fizzy_WasmRuntime(HostCallContext *host_call_context);

  ~Fizzy_WasmRuntime();

  std::pair<uint8_t *, uint32_t> get_memory() override;
  std::pair<const uint8_t *, uint32_t> get_memory() const override;

  void link_fn(std::string const &module_name, std::string const &fn_name,
               uint64_t (*f)(HostCallContext *)) override;
  void link_fn(std::string const &module_name, std::string const &fn_name,
               uint64_t (*f)(HostCallContext *, uint64_t)) override;
  void link_fn(std::string const &module_name, std::string const &fn_name,
               uint64_t (*f)(HostCallContext *, uint64_t, uint64_t)) override;
  void link_fn(std::string const &module_name, std::string const &fn_name,
               uint64_t (*f)(HostCallContext *, uint64_t, uint64_t,
                             uint64_t)) override;
  void link_fn(std::string const &module_name, std::string const &fn_name,
               uint64_t (*f)(HostCallContext *, uint64_t, uint64_t, uint64_t,
                             uint64_t)) override;
  void link_fn(std::string const &module_name, std::string const &fn_name,
               uint64_t (*f)(HostCallContext *, uint64_t, uint64_t, uint64_t,
                             uint64_t, uint64_t)) override;

  detail::MeteredReturn<uint64_t>
  invoke(std::string const &method_name,
         const uint64_t gas_limit) override final;

  bool __attribute__((warn_unused_result)) consume_gas(uint64_t gas) override;
  uint64_t get_available_gas() const override;
  void set_available_gas(uint64_t gas) override;

  bool __attribute__((warn_unused_result)) initialize(Script const &data);

private:
  // particular to fizzy -- lazy_link will fail always if it fails once.
  bool __attribute__((warn_unused_result)) lazy_link();

  bool link_tried = false;
  const FizzyModule *m_module;
  FizzyInstance *m_instance;
  HostCallContext *host_call_context;

  FizzyExecutionContext *exec_ctx;

  std::vector<FizzyLazyFunctionImport> imported_functions;
};

} // namespace wasm_api