#include "wasm_api/fizzy_api.h"

#include "wasm_api/error.h"
#include "wasm_api/ffi_trampolines.h"

#include <fizzy/fizzy.h>

#include <utility>

namespace wasm_api {

void
throw_unrecoverable_errors(FizzyError error)
{
  if (error.code == FizzyErrorCode::FizzyErrorOther) {
    throw std::runtime_error("unknown fizzy error");
  }
  if (error.code == FizzyErrorCode::FizzyErrorMemoryAllocationFailed) {
    throw std::runtime_error("fizzy malloc failed");
  }
}

template<FizzyValueType ret_type>
FizzyExecutionResult
handle_trampoline_result(TrampolineResult result, HostFnError* ctx_errno)
{
  // Fizzy doesn't have a good way of passing back the error type internally,
  // so we set it in FizzyTrampolineHostContext
  HostFnError err = static_cast<HostFnError>(result.panic);
  *ctx_errno = err;

  if (err == HostFnError::NONE_OR_RECOVERABLE) {
    if constexpr (ret_type == FizzyValueTypeVoid) {
      return FizzyExecutionResult {
        .trapped = false,
        .has_value = false,
        .value = 0
      };
    } else if constexpr (ret_type == FizzyValueTypeI64) {
      FizzyValue out;
      out.i64 = result.result;
      return FizzyExecutionResult {
        .trapped = false,
        .has_value = true,
        .value = out
      };
    } else {
      std::terminate();
    }
  } else {
    return FizzyExecutionResult {
      .trapped = true,
      .has_value = false,
      .value = 0
    };
  }
}

// again, for convenience, we assume all args are uint64
template<FizzyValueType ret_type>
FizzyExecutionResult
fizzy_trampoline_0args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);


  TrampolineResult host_fn_result;
  if constexpr(ret_type == FizzyValueTypeVoid) {
    host_fn_result 
        = c_call_0args_noret(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context);
  } else {
    host_fn_result 
        = c_call_0args(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context);
  }
  
  return handle_trampoline_result<ret_type>(host_fn_result,
    fizzy_host_ctx -> errno_);
}

template<FizzyValueType ret_type>
FizzyExecutionResult
fizzy_trampoline_1args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);

  TrampolineResult host_fn_result;
  if constexpr(ret_type == FizzyValueTypeVoid) {
    host_fn_result 
        = c_call_1args_noret(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context, args[0].i64);
  } else {
    host_fn_result 
        = c_call_1args(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context, args[0].i64);
  }
  

  return handle_trampoline_result<ret_type>(host_fn_result,
    fizzy_host_ctx -> errno_);
}

template<FizzyValueType ret_type>
FizzyExecutionResult
fizzy_trampoline_2args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);


  TrampolineResult host_fn_result;
  if constexpr(ret_type == FizzyValueTypeVoid) {
    host_fn_result 
        = c_call_2args_noret(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context, args[0].i64, args[1].i64);
  } else {
    host_fn_result 
      = c_call_2args(fizzy_host_ctx -> fn_pointer,
      fizzy_host_ctx -> real_context, args[0].i64, args[1].i64);
  }
  
  return handle_trampoline_result<ret_type>(host_fn_result,
    fizzy_host_ctx -> errno_);
}

template<FizzyValueType ret_type>
FizzyExecutionResult
fizzy_trampoline_3args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);


  TrampolineResult host_fn_result;
  if constexpr(ret_type == FizzyValueTypeVoid) {
    host_fn_result 
        = c_call_3args_noret(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context, args[0].i64, args[1].i64, args[2].i64);
  } else {
    host_fn_result 
      = c_call_3args(fizzy_host_ctx -> fn_pointer,
      fizzy_host_ctx -> real_context, args[0].i64, args[1].i64, args[2].i64);
  }

  return handle_trampoline_result<ret_type>(host_fn_result,
    fizzy_host_ctx -> errno_);
}

template<FizzyValueType ret_type>
FizzyExecutionResult
fizzy_trampoline_4args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);


  TrampolineResult host_fn_result;
  if constexpr(ret_type == FizzyValueTypeVoid) {
    host_fn_result 
        = c_call_4args_noret(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context, args[0].i64, args[1].i64, args[2].i64, args[3].i64);
  } else {
    host_fn_result 
      = c_call_4args(fizzy_host_ctx -> fn_pointer,
      fizzy_host_ctx -> real_context, args[0].i64, args[1].i64, args[2].i64, args[3].i64);
  }

  return handle_trampoline_result<ret_type>(host_fn_result,
    fizzy_host_ctx -> errno_);
}

template<FizzyValueType ret_type>
FizzyExecutionResult
fizzy_trampoline_5args(void *host_ctx, FizzyInstance *instance,
                       const FizzyValue *args,
                       FizzyExecutionContext *ctx) noexcept
{

  FizzyTrampolineHostContext *fizzy_host_ctx =
      reinterpret_cast<FizzyTrampolineHostContext *>(host_ctx);


  TrampolineResult host_fn_result;
  if constexpr(ret_type == FizzyValueTypeVoid) {
    host_fn_result 
        = c_call_5args_noret(fizzy_host_ctx -> fn_pointer,
        fizzy_host_ctx -> real_context, args[0].i64, args[1].i64, args[2].i64, args[3].i64, args[4].i64);
  } else {
    host_fn_result 
      = c_call_5args(fizzy_host_ctx -> fn_pointer,
      fizzy_host_ctx -> real_context, args[0].i64, args[1].i64, args[2].i64, args[3].i64, args[4].i64);
  }

  return handle_trampoline_result<ret_type>(host_fn_result,
    fizzy_host_ctx -> errno_);
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

  if (fizzy_module_has_start_function(m_module)) {
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

InvokeStatus<uint64_t>
Fizzy_WasmRuntime::invoke(std::string const &method_name)
{
  if (!lazy_link()) {
    return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::DETERMINISTIC_ERROR};
  }

  uint32_t fn_index;
  if (!fizzy_find_exported_function_index(m_module, method_name.c_str(),
                                          &fn_index)) {
    return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::DETERMINISTIC_ERROR};
  }

  const FizzyExecutionResult result =
      fizzy_execute(m_instance, fn_index, NULL, exec_ctx);

  if (result.trapped) {
    switch(errno_last_call_) {
    case HostFnError::NONE_OR_RECOVERABLE:
      // trap must be from some source within wasm
      return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::DETERMINISTIC_ERROR};
    case HostFnError::OUT_OF_GAS:
      return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::OUT_OF_GAS_ERROR};
    case HostFnError::UNRECOVERABLE:
      return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::UNRECOVERABLE};
    case HostFnError::RETURN_SUCCESS:
      return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::RETURN};
    case HostFnError::DETERMINISTIC_ERROR:
      return InvokeStatus<uint64_t>{std::unexpect_t{}, InvokeError::DETERMINISTIC_ERROR};
    }
    throw std::runtime_error("unreachable");
  }

  if (!result.has_value) {
    throw std::runtime_error("invalid return from invoke");
  }

  return result.value.i64;
}

bool 
Fizzy_WasmRuntime::link_fn_nargs(std::string const& module_name,
    std::string const& fn_name,
    void* fn,
    uint8_t nargs,
    WasmValueType ret_type)
{
  if (nargs > 5) {
    //unimplemented
    return false;
  }

  FizzyValueType output;

  switch(ret_type) {
  case WasmValueType::U64:
    output = FizzyValueTypeI64;
    break;
  case WasmValueType::VOID:
    output = FizzyValueTypeVoid;
    break;
  default:
    return false;
  }

  FizzyLazyFunctionImport import{
    .module_name = module_name,
    .method_name = fn_name,
    .inputs = std::vector<FizzyValueType>(nargs, FizzyValueTypeI64),
    .output = output,
    .trampoline_ctx = 
      std::make_unique<FizzyTrampolineHostContext>(fn, host_call_context, &errno_last_call_)
  };
  imported_functions.emplace_back(std::move(import));

  return true;
}

FizzyExternalFn
get_trampoline_fn(size_t args, FizzyValueType output)
{
  switch (output) {
  case FizzyValueTypeVoid:
    switch (args) {
    case 0:
      return fizzy_trampoline_0args<FizzyValueTypeVoid>;
    case 1:
      return fizzy_trampoline_1args<FizzyValueTypeVoid>;
    case 2:
      return fizzy_trampoline_2args<FizzyValueTypeVoid>;
    case 3:
      return fizzy_trampoline_3args<FizzyValueTypeVoid>;
    case 4:
      return fizzy_trampoline_4args<FizzyValueTypeVoid>;
    case 5:
      return fizzy_trampoline_5args<FizzyValueTypeVoid>;
    default:
      std::terminate();
    }
  case FizzyValueTypeI64:
    switch (args) {
    case 0:
      return fizzy_trampoline_0args<FizzyValueTypeI64>;
    case 1:
      return fizzy_trampoline_1args<FizzyValueTypeI64>;
    case 2:
      return fizzy_trampoline_2args<FizzyValueTypeI64>;
    case 3:
      return fizzy_trampoline_3args<FizzyValueTypeI64>;
    case 4:
      return fizzy_trampoline_4args<FizzyValueTypeI64>;
    case 5:
      return fizzy_trampoline_5args<FizzyValueTypeI64>;
    default:
      std::terminate();
    }
  default:
    std::terminate();
  }
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
          .function = get_trampoline_fn(inputs.size(), output),
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

std::span<std::byte>
Fizzy_WasmRuntime::get_memory()
{
  if (!lazy_link()) {
    return {};
  }

  if (!m_instance) {
    return {};
  }

  return std::span<std::byte>(reinterpret_cast<std::byte*>(fizzy_get_instance_memory_data(m_instance)),
          fizzy_get_instance_memory_size(m_instance));
}

std::span<const std::byte>
Fizzy_WasmRuntime::get_memory() const
{
  if (!m_instance) {
    return {};
  }

  return std::span<const std::byte>(
    reinterpret_cast<const std::byte*>(fizzy_get_instance_memory_data(m_instance)),
          fizzy_get_instance_memory_size(m_instance));
}

} // namespace wasm_api
