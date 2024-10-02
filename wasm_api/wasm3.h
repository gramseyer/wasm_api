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

/*

Based on wasm3/platforms/cpp/wasm3_cpp.h

Original Copyright Notice:

MIT License

Copyright (c) 2019 Steven Massey, Volodymyr Shymanskyy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <span>

#include "wasm_api/error.h"
#include "wasm_api/value_type.h"

#include "wasm3/source/wasm3.h"

#include "wasm_api/ffi_trampolines.h"

namespace wasm_api {

class HostCallContext;

}

namespace wasm3 {
/** @cond */
namespace detail {

typedef uint64_t *stack_type;
typedef void *mem_type;
template<typename T, typename...> struct first_type {
  typedef T type;
};

typedef const void *(*m3_api_raw_fn)(IM3Runtime, uint64_t *, void *);

template<typename T>
void
arg_from_stack(T &dest, stack_type &_sp, mem_type mem)
{
  m3ApiGetArg(T, tmp);
  dest = tmp;
}

template<typename T>
void
arg_from_stack(T *&dest, stack_type &_sp, mem_type _mem)
{
  m3ApiGetArgMem(T *, tmp);
  dest = tmp;
};

template<typename T>
void
arg_from_stack(const T *&dest, stack_type &_sp, mem_type _mem)
{
  m3ApiGetArgMem(const T *, tmp);
  dest = tmp;
};

/*
        template<char c>
        struct m3_sig {
            static const char value = c;
        };
        template<typename T> struct m3_type_to_sig;
        template<> struct m3_type_to_sig<int32_t> : m3_sig<'i'> {};
        template<> struct m3_type_to_sig<int64_t> : m3_sig<'I'> {};
        template<> struct m3_type_to_sig<uint32_t> : m3_sig<'i'> {};
        template<> struct m3_type_to_sig<uint64_t> : m3_sig<'I'> {};
        template<> struct m3_type_to_sig<float>   : m3_sig<'f'> {};
        template<> struct m3_type_to_sig<double>  : m3_sig<'F'> {};
        template<> struct m3_type_to_sig<void>    : m3_sig<'v'> {};
        template<> struct m3_type_to_sig<void *>  : m3_sig<'*'> {};
        template<> struct m3_type_to_sig<const void *> : m3_sig<'*'> {};


        template<typename Ret, typename ... Args>
        struct m3_signature {
            constexpr static size_t n_args = sizeof...(Args);
            constexpr static const char value[n_args + 4] = {
                    m3_type_to_sig<Ret>::value,
                    '(',
                    m3_type_to_sig<Args>::value...,
                    ')',
                    0
            };
        }; */

template<typename... Args>
static void
get_args_from_stack(stack_type &sp, mem_type mem, std::tuple<Args...> &tuple)
{
  std::apply([&](auto &...item) { (arg_from_stack(item, sp, mem), ...); },
             tuple);
}

template<typename ret_type, std::same_as<uint64_t>... Args>
const void*
wrap_fn_return(IM3Runtime rt, IM3ImportContext _ctx, stack_type _sp, mem_type mem)
{
    static_assert(!std::is_void_v<ret_type>);
    m3ApiReturnType(ret_type);
    
    using args_tuple_t = std::tuple<Args...>;

    args_tuple_t args;

    get_args_from_stack(_sp, mem, args);

    void* fn_pointer = _ctx -> userdata;

    constexpr size_t nargs = std::tuple_size_v<args_tuple_t>;

    TrampolineResult result;

    if constexpr (nargs == 0) {
        result = c_call_0args(fn_pointer, m3_GetUserData(rt));
    } else if constexpr (nargs == 1) {
        result = c_call_1args(fn_pointer, m3_GetUserData(rt), std::get<0>(args));
    } else if constexpr (nargs == 2) {
        result = c_call_2args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args));
    } else if constexpr (nargs == 3) {
        result = c_call_3args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args));
    } else if constexpr (nargs == 4) {
        result = c_call_4args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
    } else if constexpr (nargs == 5) {
        result = c_call_5args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args));
    } else if constexpr (nargs == 6) {
        result = c_call_6args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args));
    } else if constexpr (nargs == 7) {
        result = c_call_7args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args), std::get<6>(args));
    } else if constexpr (nargs == 8) {
        result = c_call_8args(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args), std::get<6>(args), std::get<7>(args));
    } else {
        std::terminate();
    }

    switch (result.panic) {
      case static_cast<uint8_t>(wasm_api::HostFnError::NONE_OR_RECOVERABLE):
        m3ApiReturn(result.result);
        std::terminate();  // asserting that m3ApiReturn macro actually returns
      case static_cast<uint8_t>(wasm_api::HostFnError::UNRECOVERABLE):
        m3ApiTrap(m3Err_unrecoverableSystemError);
      case static_cast<uint8_t>(wasm_api::HostFnError::OUT_OF_GAS):
        m3ApiTrap(m3Err_outOfGasError);
      case static_cast<uint8_t>(wasm_api::HostFnError::RETURN_SUCCESS):
        m3ApiTrap(m3Err_returnSuccessError);
      default:
        throw std::runtime_error("impossible");
    }
}

template<std::same_as<uint64_t>... Args>
const void*
wrap_fn_noreturn(IM3Runtime rt, IM3ImportContext _ctx, stack_type _sp, mem_type mem)
{    
    using args_tuple_t = std::tuple<Args...>;

    args_tuple_t args;

    get_args_from_stack(_sp, mem, args);

    void* fn_pointer = _ctx -> userdata;

    constexpr size_t nargs = std::tuple_size_v<args_tuple_t>;

    TrampolineResult result;

    if constexpr (nargs == 0) {
        result = c_call_0args_noret(fn_pointer, m3_GetUserData(rt));
    } else if constexpr (nargs == 1) {
        result = c_call_1args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args));
    } else if constexpr (nargs == 2) {
        result = c_call_2args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args));
    } else if constexpr (nargs == 3) {
        result = c_call_3args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args));
    } else if constexpr (nargs == 4) {
        result = c_call_4args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args));
    } else if constexpr (nargs == 5) {
        result = c_call_5args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args));
    } else if constexpr (nargs == 6) {
        result = c_call_6args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args));
    } else if constexpr (nargs == 7) {
        result = c_call_7args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args), std::get<6>(args));
    } else if constexpr (nargs == 8) {
        result = c_call_8args_noret(fn_pointer, m3_GetUserData(rt), std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args), std::get<5>(args), std::get<6>(args), std::get<7>(args));
    } else {
        std::terminate();
    }

    switch (result.panic) {
      case static_cast<uint8_t>(wasm_api::HostFnError::NONE_OR_RECOVERABLE):
        m3ApiSuccess();
        std::terminate();  // asserting that m3ApiSuccess macro actually returns
      case static_cast<uint8_t>(wasm_api::HostFnError::UNRECOVERABLE):
        m3ApiTrap(m3Err_unrecoverableSystemError);
      case static_cast<uint8_t>(wasm_api::HostFnError::OUT_OF_GAS):
        m3ApiTrap(m3Err_outOfGasError);
      case static_cast<uint8_t>(wasm_api::HostFnError::RETURN_SUCCESS):
        m3ApiTrap(m3Err_returnSuccessError);
      default:
        throw std::runtime_error("impossible");
    }
}


static void
throw_nondeterministic_errors(M3Result result) {
    if (result == m3Err_mallocFailed) {
        throw std::runtime_error("malloc failed in wasm3");
    }
    if (result == m3Err_unrecoverableSystemError) {
        throw std::runtime_error("unrecoverable system error");
    }
}

} // namespace detail
/** @endcond */

static bool
static_link_nargs(IM3Module io_module, const char *const i_moduleName,
           const char *const i_functionName,
           void *function, // expects signature of
                           // HostFnStatus<uint64_t>(HostCallContext*, nargs
                           // repeated uint64)
           uint8_t nargs,
           wasm_api::WasmValueType ret_type)
{
  using enum wasm_api::WasmValueType;
  auto arg_sig = [&nargs, &ret_type] () -> std::string {
    std::string out;
    switch(ret_type) {
    case VOID:
        out += "v";
        break;
    case U64:
        out += "I";
        break;
    }
    out += "(";
    for (auto i = 0u; i < nargs; i++) {
      out += "I";
    }
    out += ")";
    return out;
  };

  decltype(&detail::wrap_fn_noreturn<>) wrapped_fn_pointer = nullptr;

  // gross but it works
  switch(ret_type) {
  case U64:
      switch (nargs) {
      case 0:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t>;
        break;
      case 1:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t>;
        break;
      case 2:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t>;
        break;
      case 3:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 4:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 5:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 6:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 7:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 8:
        wrapped_fn_pointer = &detail::wrap_fn_return<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      default:
        return false;
      }
    break;
  case VOID:
    switch (nargs) {
      case 0:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<>;
        break;
      case 1:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t>;
        break;
      case 2:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t>;
        break;
      case 3:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t, uint64_t>;
        break;
      case 4:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 5:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 6:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 7:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      case 8:
        wrapped_fn_pointer = &detail::wrap_fn_noreturn<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
        break;
      default:
        std::printf("invalid void nargs %u\n", nargs);
        return false;
      }
    break;
  default:
    std::terminate();
  }

  auto cur_sig = arg_sig();

  M3Result result =
      m3_LinkRawFunctionEx(io_module, i_moduleName, i_functionName,
                           cur_sig.c_str(), wrapped_fn_pointer, function);
  return (result == m3Err_none || result == m3Err_functionLookupFailed);
}

class module;
class runtime;
class function;

/**
 * Wrapper for WASM3 environment.
 *
 * Runtimes, modules are owned by an environment.
 */
class environment {
public:
  environment()
  {
    m_env.reset(m3_NewEnvironment(), m3_FreeEnvironment);
    if (m_env == nullptr) {
      throw std::bad_alloc();
    }
  }

  /**
   * Create new runtime
   *
   * @param stack_size_bytes  size of the WASM stack for this runtime
   * @return runtime object
   */
  std::unique_ptr<runtime> new_runtime(size_t stack_size_bytes, void *ctxp);

  /**
   * Parse a WASM module from file
   *
   * The parsed module is not loaded into any runtime. Use runtime::load to
   * load the module after parsing it.
   *
   * @param in  file (WASM binary)
   * @return module object
   */
  std::unique_ptr<module> parse_module(std::istream &in);

  /**
   * Parse a WASM module from binary data
   *
   * @param data  pointer to the start of the binary
   * @param size  size of the binary
   * @return module object
   */
  std::unique_ptr<module> parse_module(const uint8_t *data, size_t size);

protected:
  std::shared_ptr<struct M3Environment> m_env;
};

/**
 * Wrapper for the runtime, where modules are loaded and executed.
 */
class runtime {
public:
  /**
   * Load the module into runtime
   * @param mod  module parsed by environment::parse_module
   */
  bool __attribute__((warn_unused_result)) load(module &mod);

  /**
   * Get a function handle by name
   *
   * If the function is not found, returns nullopt
   * @param name  name of a function, c-string
   * @return function object
   */
  std::optional<function> find_function(const char *name);

  std::span<std::byte> get_memory();

  friend class environment;

  runtime(const std::shared_ptr<M3Environment> &env, size_t stack_size_bytes,
          void *ctxp)
    : m_env(env)
  {
    m_runtime.reset(m3_NewRuntime(env.get(), stack_size_bytes, ctxp),
                    &m3_FreeRuntime);
    if (m_runtime == nullptr) {
      throw std::bad_alloc();
    }
  }

protected:
  /* runtime extends the lifetime of the environment */
  std::shared_ptr<M3Environment> m_env;
  std::shared_ptr<M3Runtime> m_runtime;
};

/**
 * Module object holds a webassembly module
 *
 * It can be constructed by parsing a WASM binary using
 * environment::parse_module. Functions can be linked to the loaded module. Once
 * constructed, modules can be loaded into the runtime.
 */
class module {
public:

  // expected signature: HostFnStatus<ret_type>(HostCallContext*, uint64t repeated nargs)
  bool
  link_nargs(const char* module, const char* function_name,
      void* function_pointer, uint8_t nargs, wasm_api::WasmValueType ret_type);

  ~module()
  {
    if ((!m_loaded) && (m_module != nullptr)) {
      m3_FreeModule(m_module);
    }
  }

  friend class environment;
  friend class runtime;

  module(std::istream &in_wasm)
  {
    in_wasm.unsetf(std::ios::skipws);
    std::copy(std::istream_iterator<uint8_t>(in_wasm),
              std::istream_iterator<uint8_t>(),
              std::back_inserter(m_moduleRawData));
  }

  module(const uint8_t *data, size_t size)
  {
    m_moduleRawData = std::vector<uint8_t>{data, data + size};
  }

  bool __attribute__((warn_unused_result))
  init(const std::shared_ptr<M3Environment> &env)
  {
    // exists only to extend lifetime of env
    m_env = env;
    return parse(env.get(), m_moduleRawData.data(), m_moduleRawData.size());
  }

protected:
  module(module &&) = delete;
  module(module &) = delete;
  module &operator=(const module &) = delete;

  bool __attribute__((warn_unused_result))
  parse(IM3Environment env, const uint8_t *data, size_t size)
  {
    IM3Module p;
    M3Result err = m3_ParseModule(env, &p, data, size);
    detail::throw_nondeterministic_errors(err);
    if (err != m3Err_none) {
      return false;
    }

    if (m_loaded) {
      m3_FreeModule(m_module);
    }
    m_module = p;
    return true;
  }

  bool __attribute__((warn_unused_result)) load_into(IM3Runtime runtime)
  {
    M3Result err = m3_LoadModule(runtime, m_module);
    detail::throw_nondeterministic_errors(err);
    if (err != m3Err_none) {
      return false;
    }
    m_loaded = true;
    return true;
  }

  // m_env exists only to extend lifetime of environment
  std::shared_ptr<M3Environment> m_env;
  IM3Module m_module;

  bool m_loaded = false;
  std::vector<uint8_t> m_moduleRawData{};
};

/**
 * Handle of a function. Can be obtained from runtime::find_function method by
 * name.
 */
class function {
public:
  /**
   * Call the function with the provided arguments (int/float types).
   *
   * Note that the type of the return value must be explicitly specified as a
   * template argument.
   *
   */
  template<std::same_as<uint64_t>... Args>
  wasm_api::InvokeStatus<uint64_t> call(Args... args)
  {
    const void *arg_ptrs[] = {reinterpret_cast<const void *>(&args)...};
    M3Result res = m3_Call(m_func, sizeof...(args), arg_ptrs);

    if (res == m3Err_mallocFailed || res == m3Err_unrecoverableSystemError)
    {
        return wasm_api::InvokeStatus<uint64_t>{std::unexpect_t{}, wasm_api::InvokeError::UNRECOVERABLE};
    }

    if (res == m3Err_outOfGasError) {
        return wasm_api::InvokeStatus<uint64_t>{std::unexpect_t{}, wasm_api::InvokeError::OUT_OF_GAS_ERROR};
    }

    if (res == m3Err_returnSuccessError) {
        return wasm_api::InvokeStatus<uint64_t>{std::unexpect_t{}, wasm_api::InvokeError::RETURN};
    }

    if (res != m3Err_none) {
        return wasm_api::InvokeStatus<uint64_t>{std::unexpect_t{}, wasm_api::InvokeError::DETERMINISTIC_ERROR};
    }

    uint64_t ret;
    const void *ret_ptrs[] = {&ret};
    res = m3_GetResults(m_func, 1, ret_ptrs);

    if (res == m3Err_argumentCountMismatch) {
        return wasm_api::InvokeStatus<uint64_t>{std::unexpect_t{}, wasm_api::InvokeError::DETERMINISTIC_ERROR};
    }
    if (res == m3Err_none) {
      return ret;
    }
    return wasm_api::InvokeStatus<uint64_t>{std::unexpect_t{}, wasm_api::InvokeError::UNRECOVERABLE};
  }

  friend class runtime;

  function(const std::shared_ptr<M3Runtime> &runtime) : m_runtime(runtime) {}

  bool __attribute__((warn_unused_result)) initialize(const char *name)
  {
    M3Result err = m3_FindFunction(&m_func, m_runtime.get(), name);
    detail::throw_nondeterministic_errors(err);
    return (m_func != nullptr);
  }

protected:
  std::shared_ptr<M3Runtime> m_runtime;
  M3Function *m_func = nullptr;
};

inline std::unique_ptr<runtime>
environment::new_runtime(size_t stack_size_bytes, void *ctxp)
{
  return std::make_unique<runtime>(m_env, stack_size_bytes, ctxp);
}

inline std::unique_ptr<module>
environment::parse_module(std::istream &in)
{
  auto out = std::make_unique<module>(in);
  if (out->init(m_env)) {
    return out;
  }
  return nullptr;
}

inline std::unique_ptr<module>
environment::parse_module(const uint8_t *data, size_t size)
{
  auto out = std::make_unique<module>(data, size);
  if (out->init(m_env)) {
    return out;
  }
  return nullptr;
}

inline bool __attribute__((warn_unused_result))
runtime::load(module &mod)
{
  return mod.load_into(m_runtime.get());
}

inline std::optional<function>
runtime::find_function(const char *name)
{
  auto f = function(m_runtime);
  if (!f.initialize(name)) {
    return std::nullopt;
  }
  return f;
}

inline std::span<std::byte>
runtime::get_memory()
{
  uint32_t len = 0;
  uint8_t *mem = m3_GetMemory(m_runtime.get(), &len, 0);
  return std::span<std::byte>{reinterpret_cast<std::byte*>(mem), len};
}

// expected signature: HostFnStatus<uint64_t>(HostCallContext*, uint64t repeated nargs)
inline bool
module::link_nargs(const char* module, const char* function_name,
    void* function_pointer, uint8_t nargs, wasm_api::WasmValueType ret_type)
{
    return static_link_nargs(m_module, module, function_name, function_pointer, nargs, ret_type);
}

} // namespace wasm3
