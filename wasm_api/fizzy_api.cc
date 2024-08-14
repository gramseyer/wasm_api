#include "wasm_api/fizzy_api.h"

#include "wasm_api/error.h"

#include <fizzy/fizzy.h>

#include <utility>

namespace wasm_api {

void
throw_unrecoverable_errors(FizzyError error)
{
  if (error.code == FizzyErrorCode::FizzyErrorOther) {
    throw UnrecoverableSystemError("unknown fizzy error");
  }
  if (error.code == FizzyErrorCode::FizzyErrorMemoryAllocationFailed) {
    throw UnrecoverableSystemError("fizzy malloc failed");
  }
}

// again, for convenience, we assume all args are uint64
FizzyExecutionResult
fizzy_trampoline_0args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  auto *ptr = (uint64_t(*)(HostCallContext *))(fizzy_host_ctx->fn_pointer);
  FizzyValue out;
  out.i64 = 0;
  try {
    out.i64 = (*ptr)(fizzy_host_ctx->real_context);
    return FizzyExecutionResult{.trapped = false,
                                .has_value = true,
                                .value = out};
  } catch (HostError &) {
    return FizzyExecutionResult{.trapped = true,
                                .has_value = false,
                                .value = out};
  }
  // other errors result in std::terminate
  std::unreachable();
}

FizzyExecutionResult
fizzy_trampoline_1args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  auto *ptr =
      (uint64_t(*)(HostCallContext *, uint64_t))(fizzy_host_ctx->fn_pointer);
  FizzyValue out;
  out.i64 = 0;
  try {
    out.i64 = (*ptr)(fizzy_host_ctx->real_context, args[0].i64);
    return FizzyExecutionResult{.trapped = false,
                                .has_value = true,
                                .value = out};
  } catch (HostError &) {
    return FizzyExecutionResult{.trapped = true,
                                .has_value = false,
                                .value = out};
  }
  // other errors result in std::terminate
  std::unreachable();
}

FizzyExecutionResult
fizzy_trampoline_2args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  auto *ptr = (uint64_t(*)(HostCallContext *, uint64_t, uint64_t))(
      fizzy_host_ctx->fn_pointer);
  FizzyValue out;
  out.i64 = 0;
  try {
    out.i64 = (*ptr)(fizzy_host_ctx->real_context, args[0].i64, args[1].i64);
    return FizzyExecutionResult{.trapped = false,
                                .has_value = true,
                                .value = out};
  } catch (HostError &) {
    return FizzyExecutionResult{.trapped = true,
                                .has_value = false,
                                .value = out};
  }
  // other errors result in std::terminate
  std::unreachable();
}

FizzyExecutionResult
fizzy_trampoline_3args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  auto *ptr = (uint64_t(*)(HostCallContext *, uint64_t, uint64_t, uint64_t))(
      fizzy_host_ctx->fn_pointer);
  FizzyValue out;
  out.i64 = 0;
  try {
    out.i64 = (*ptr)(fizzy_host_ctx->real_context, args[0].i64, args[1].i64,
                     args[2].i64);
    return FizzyExecutionResult{.trapped = false,
                                .has_value = true,
                                .value = out};
  } catch (HostError &) {
    return FizzyExecutionResult{.trapped = true,
                                .has_value = false,
                                .value = out};
  }
  // other errors result in std::terminate
  std::unreachable();
}

FizzyExecutionResult
fizzy_trampoline_4args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  auto *ptr = (uint64_t(*)(HostCallContext *, uint64_t, uint64_t, uint64_t,
                           uint64_t))(fizzy_host_ctx->fn_pointer);
  FizzyValue out;
  out.i64 = 0;
  try {
    out.i64 = (*ptr)(fizzy_host_ctx->real_context, args[0].i64, args[1].i64,
                     args[2].i64, args[3].i64);
    return FizzyExecutionResult{.trapped = false,
                                .has_value = true,
                                .value = out};
  } catch (HostError &) {
    return FizzyExecutionResult{.trapped = true,
                                .has_value = false,
                                .value = out};
  }
  // other errors result in std::terminate
  std::unreachable();
}

FizzyExecutionResult
fizzy_trampoline_5args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  auto *ptr = (uint64_t(*)(HostCallContext *, uint64_t, uint64_t, uint64_t,
                           uint64_t, uint64_t))(fizzy_host_ctx->fn_pointer);
  FizzyValue out;
  out.i64 = 0;
  try {
    out.i64 = (*ptr)(fizzy_host_ctx->real_context, args[0].i64, args[1].i64,
                     args[2].i64, args[3].i64, args[4].i64);
    return FizzyExecutionResult{.trapped = false,
                                .has_value = true,
                                .value = out};
  } catch (HostError &) {
    return FizzyExecutionResult{.trapped = true,
                                .has_value = false,
                                .value = out};
  }
  // other errors result in std::terminate
  std::unreachable();
}

Fizzy_WasmContext::Fizzy_WasmContext(uint32_t max_stack_bytes)
{}
Fizzy_WasmContext::~Fizzy_WasmContext()
{}

std::unique_ptr<WasmRuntime>
Fizzy_WasmContext::new_runtime_instance(Script const &contract, void *ctxp)
{
  std::unique_ptr<WasmRuntime> out = std::make_unique<WasmRuntime>(ctxp);

  auto fizzy_runtime =
      std::make_unique<Fizzy_WasmRuntime>(out->get_host_call_context());

  if (!fizzy_runtime->initialize(contract)) {
    return nullptr;
  }

  out->initialize(fizzy_runtime.get());
  fizzy_runtime.release();

  return out;
}

Fizzy_WasmRuntime::Fizzy_WasmRuntime(HostCallContext *host_call_context)
  : m_module(nullptr), m_instance(nullptr),
    host_call_context(host_call_context), exec_ctx(nullptr)
{}

Fizzy_WasmRuntime::~Fizzy_WasmRuntime()
{
  if (!link_tried) {
    // no effect if input is nullptr
    // module should not be freed
    // if instance already owns it
    fizzy_free_module(m_module);
  }
  // no effect if input is nullptr
  fizzy_free_instance(m_instance);

  fizzy_free_execution_context(exec_ctx);
}

bool __attribute__((warn_unused_result))
Fizzy_WasmRuntime::initialize(Script const &data)
{
  FizzyError error;
  m_module = fizzy_parse(data.data, data.len, &error);

  throw_unrecoverable_errors(error);

  if (m_module == nullptr) {
    return false;
  }

  exec_ctx = fizzy_create_metered_execution_context(0, 0);

  if (exec_ctx == nullptr) {
    return false;
  }

  return true;
}

bool __attribute__((warn_unused_result))
Fizzy_WasmRuntime::lazy_link()
{

  if (m_instance != nullptr) {
    return true;
  }

  if (link_tried) {
    return false;
  }
  link_tried = true;

  constexpr uint32_t mem_page_limit =
      100; // 100 * pagesize=64k, arbitrary (should be config param, i'm lazy)

  std::vector<FizzyImportedFunction> functions;

  for (auto const &f : imported_functions) {
    functions.push_back(f.to_imported_function());
  }

  FizzyError error;

  m_instance = fizzy_resolve_instantiate(
      m_module,
      functions.data(), // only needs to live until end of instantiate call
      functions.size(),
      nullptr, // imported table
      nullptr, // imported memory
      nullptr, // imported globals,
      0,       // imported globals size
      mem_page_limit, &error);

  return (m_instance != nullptr);
}

detail::MeteredReturn<uint64_t>
Fizzy_WasmRuntime::invoke(std::string const &method_name,
                          const uint64_t gas_limit)
{
  set_available_gas(gas_limit);

  if (!lazy_link()) {
    return {std::nullopt, 0, ErrorType::HostError};
  }

  uint32_t fn_index;
  if (!fizzy_find_exported_function_index(m_module, method_name.c_str(),
                                          &fn_index)) {
    return {std::nullopt, 0, ErrorType::HostError};
  }

  const FizzyExecutionResult result =
      fizzy_execute(m_instance, fn_index, NULL, exec_ctx);

  uint64_t remaining_gas = get_available_gas();
  if (result.trapped) {
    return {std::nullopt, gas_limit - remaining_gas, ErrorType::HostError};
  }

  if (!result.has_value) {
    throw UnrecoverableSystemError("invalid return from invoke");
  }

  return {result.value.i64, gas_limit - remaining_gas, ErrorType::None};
}

void
Fizzy_WasmRuntime::link_fn(std::string const &module_name,
                           std::string const &fn_name,
                           uint64_t (*f)(HostCallContext *))
{
  FizzyLazyFunctionImport import{
      .module_name = module_name,
      .method_name = fn_name,
      .inputs = {},
      .output = FizzyValueTypeI64,
      .trampoline_ctx =
          std::make_unique<FizzyTrampolineHostContext>((void *)f,
                                                       host_call_context)};

  imported_functions.emplace_back(std::move(import));
}

void
Fizzy_WasmRuntime::link_fn(std::string const &module_name,
                           std::string const &fn_name,
                           uint64_t (*f)(HostCallContext *, uint64_t))
{
  FizzyLazyFunctionImport import{
      .module_name = module_name,
      .method_name = fn_name,
      .inputs = {FizzyValueTypeI64},
      .output = FizzyValueTypeI64,
      .trampoline_ctx =
          std::make_unique<FizzyTrampolineHostContext>((void *)f,
                                                       host_call_context)};

  imported_functions.emplace_back(std::move(import));
}

void
Fizzy_WasmRuntime::link_fn(std::string const &module_name,
                           std::string const &fn_name,
                           uint64_t (*f)(HostCallContext *, uint64_t, uint64_t))
{
  FizzyLazyFunctionImport import{
      .module_name = module_name,
      .method_name = fn_name,
      .inputs = {FizzyValueTypeI64, FizzyValueTypeI64},
      .output = FizzyValueTypeI64,
      .trampoline_ctx =
          std::make_unique<FizzyTrampolineHostContext>((void *)f,
                                                       host_call_context)};

  imported_functions.emplace_back(std::move(import));
}

void
Fizzy_WasmRuntime::link_fn(std::string const &module_name,
                           std::string const &fn_name,
                           uint64_t (*f)(HostCallContext *, uint64_t, uint64_t,
                                         uint64_t))
{
  FizzyLazyFunctionImport import{
      .module_name = module_name,
      .method_name = fn_name,
      .inputs = {FizzyValueTypeI64, FizzyValueTypeI64, FizzyValueTypeI64},
      .output = FizzyValueTypeI64,
      .trampoline_ctx =
          std::make_unique<FizzyTrampolineHostContext>((void *)f,
                                                       host_call_context)};

  imported_functions.emplace_back(std::move(import));
}

void
Fizzy_WasmRuntime::link_fn(std::string const &module_name,
                           std::string const &fn_name,
                           uint64_t (*f)(HostCallContext *, uint64_t, uint64_t,
                                         uint64_t, uint64_t))
{
  FizzyLazyFunctionImport import{
      .module_name = module_name,
      .method_name = fn_name,
      .inputs = {FizzyValueTypeI64, FizzyValueTypeI64, FizzyValueTypeI64,
                 FizzyValueTypeI64},
      .output = FizzyValueTypeI64,
      .trampoline_ctx =
          std::make_unique<FizzyTrampolineHostContext>((void *)f,
                                                       host_call_context)};

  imported_functions.emplace_back(std::move(import));
}

void
Fizzy_WasmRuntime::link_fn(std::string const &module_name,
                           std::string const &fn_name,
                           uint64_t (*f)(HostCallContext *, uint64_t, uint64_t,
                                         uint64_t, uint64_t, uint64_t))
{
  FizzyLazyFunctionImport import{
      .module_name = module_name,
      .method_name = fn_name,
      .inputs = {FizzyValueTypeI64, FizzyValueTypeI64, FizzyValueTypeI64,
                 FizzyValueTypeI64, FizzyValueTypeI64},
      .output = FizzyValueTypeI64,
      .trampoline_ctx =
          std::make_unique<FizzyTrampolineHostContext>((void *)f,
                                                       host_call_context)};

  imported_functions.emplace_back(std::move(import));
}

FizzyExternalFn
get_trampoline_fn(size_t args)
{
  switch (args) {
  case 0:
    return fizzy_trampoline_0args;
  case 1:
    return fizzy_trampoline_1args;
  case 2:
    return fizzy_trampoline_2args;
  case 3:
    return fizzy_trampoline_3args;
  case 4:
    return fizzy_trampoline_4args;
  case 5:
    return fizzy_trampoline_5args;
  default:
    std::terminate();
  }
  std::unreachable();
}

FizzyImportedFunction
FizzyLazyFunctionImport::to_imported_function() const
{
  return FizzyImportedFunction{
      .module = module_name.c_str(),
      .name = method_name.c_str(),
      .external_function = FizzyExternalFunction{
          .type = FizzyFunctionType{.output = output,
                                    .inputs = inputs.data(),
                                    .inputs_size = inputs.size()},
          .function = get_trampoline_fn(inputs.size()),
          .context = static_cast<void *>(trampoline_ctx.get())}};
}

bool __attribute__((warn_unused_result))
Fizzy_WasmRuntime::consume_gas(uint64_t gas)
{
  int64_t *cur_gas = fizzy_get_execution_context_ticks(exec_ctx);

  if (gas > INT64_MAX) {
    *cur_gas = 0;
    return false;
  }

  if ((*cur_gas) < static_cast<int64_t>(gas)) {
    *cur_gas = 0;
    return false;
  }
  *cur_gas -= gas;
  return true;
}
uint64_t
Fizzy_WasmRuntime::get_available_gas() const
{
  int64_t res = *fizzy_get_execution_context_ticks(exec_ctx);

  if (res < 0) {
    return 0;
  }
  return res;
}
void
Fizzy_WasmRuntime::set_available_gas(uint64_t gas)
{
  int64_t actual_set = INT64_MAX;
  if (gas < INT64_MAX) {
    actual_set = gas;
  }

  *fizzy_get_execution_context_ticks(exec_ctx) = actual_set;
}

std::pair<uint8_t *, uint32_t>
Fizzy_WasmRuntime::get_memory()
{
  if (!lazy_link()) {
    return {nullptr, 0};
  }

  if (!m_instance) {
    return {nullptr, 0};
  }

  return {fizzy_get_instance_memory_data(m_instance),
          fizzy_get_instance_memory_size(m_instance)};
}

std::pair<const uint8_t *, uint32_t>
Fizzy_WasmRuntime::get_memory() const
{
  if (!m_instance) {
    return {nullptr, 0};
  }

  return {fizzy_get_instance_memory_data(m_instance),
          fizzy_get_instance_memory_size(m_instance)};
}

} // namespace wasm_api
