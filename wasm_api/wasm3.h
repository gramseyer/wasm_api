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

#include <tuple>
#include <algorithm>
#include <type_traits>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <iterator>
#include <cassert>

#include "wasm_api/error.h"

#include "wasm3/source/wasm3.h"

namespace wasm_api
{

class HostCallContext;

}

namespace wasm3 {
    /** @cond */
    namespace detail {
        typedef uint64_t *stack_type;
        typedef void *mem_type;
        template<typename T, typename...> struct first_type { typedef T type; };

        typedef const void *(*m3_api_raw_fn)(IM3Runtime, uint64_t *, void *);

        template<typename T>
        void arg_from_stack(T &dest, stack_type &_sp, mem_type mem) {
            m3ApiGetArg(T, tmp);
            dest = tmp;
        }

        template<typename T>
        void arg_from_stack(T* &dest, stack_type &_sp, mem_type _mem) {
            m3ApiGetArgMem(T*, tmp);
            dest = tmp;
        };

        template<typename T>
        void arg_from_stack(const T* &dest, stack_type &_sp, mem_type _mem) {
            m3ApiGetArgMem(const T*, tmp);
            dest = tmp;
        };

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
        };

        template <typename ...Args>
        static void get_args_from_stack(stack_type &sp, mem_type mem, std::tuple<Args...> &tuple) {
            std::apply([&](auto &... item) {
                (arg_from_stack(item, sp, mem), ...);
            }, tuple);
        }

        template<typename Func>
        struct wrap_helper;

        template <typename Ret, typename ...Args>
        struct wrap_helper<Ret(wasm_api::HostCallContext*, Args...)> {
            using Func = Ret(wasm_api::HostCallContext*, Args...);
            static const void *wrap_fn(IM3Runtime rt, IM3ImportContext _ctx, stack_type _sp, mem_type mem) {
                 
                using base_tuple_t = std::tuple<Args...>;
                using aug_tuple_t = std::tuple<wasm_api::HostCallContext*, Args...>;

                base_tuple_t args;

                // The order here matters: m3ApiReturnType should go before calling get_args_from_stack,
                // since both modify `_sp`, and the return value on the stack is reserved before the arguments.
                m3ApiReturnType(Ret);
                get_args_from_stack(_sp, mem, args);
                Func* function = reinterpret_cast<Func*>(_ctx->userdata);
                try {
                    aug_tuple_t aug_args = std::tuple_cat(std::tuple<wasm_api::HostCallContext*>(reinterpret_cast<wasm_api::HostCallContext*>(m3_GetUserData(rt))), args);
                    Ret r = std::apply(function, aug_args);
                    m3ApiReturn(r);
                } 
                catch (wasm_api::HostError& e)
                {
                    m3ApiTrap(m3Err_trapHostEnvError);
                } catch(std::runtime_error& e)
                {
                    // We needed this, rather than just throwing all the way back to the top invocation,
                    // because wasm3 had a bug in the unwinding/dtor code.  This has since been fixed
                    // (in dec 2023)
                    std::printf("cannot recover from other errors safely: what %s\n", e.what());
                    m3ApiTrap(m3Err_unrecoverableSystemError);
                } catch(...)
                {
                    std::printf("unknown error type, cannot recover\n");
                    m3ApiTrap(m3Err_unrecoverableSystemError);
                }
                // wasm3 version of this file does not have an m3ApiSuccess here, so presumably not needed */
            }
        };

        template <typename ...Args>
        struct wrap_helper<void(wasm_api::HostCallContext*, Args...)> {
            using Func = void(wasm_api::HostCallContext*, Args...);
            static const void *wrap_fn(IM3Runtime rt, IM3ImportContext _ctx, stack_type sp, mem_type mem) {
                
                using base_tuple_t = std::tuple<Args...>;
                using aug_tuple_t = std::tuple<wasm_api::HostCallContext*, Args...>;

                base_tuple_t args;
                get_args_from_stack(sp, mem, args);


                try {
                    Func* function = reinterpret_cast<Func*>(_ctx->userdata);
                    aug_tuple_t aug_args = std::tuple_cat(std::tuple<wasm_api::HostCallContext*>(reinterpret_cast<wasm_api::HostCallContext*>(m3_GetUserData(rt))), args);
                    std::apply(function, aug_args);
                    m3ApiSuccess();
                } catch (wasm_api::HostError& e)
                {
                    m3ApiTrap(m3Err_trapHostEnvError);
                } catch(std::runtime_error& e)
                {
                    std::printf("cannot recover from other errors safely: what %s\n", e.what());
                    m3ApiTrap(m3Err_unrecoverableSystemError);
                } catch(...) {
                    std::printf("unknown error type, cannot recover\n");
                    m3ApiTrap(m3Err_unrecoverableSystemError);
                }
            }
        };

        template<typename Func>
        class m3_wrapper;

        template<typename Ret, typename ... Args>
        class m3_wrapper<Ret(wasm_api::HostCallContext*, Args...)> {
        public:
            static M3Result link(IM3Module io_module,
                                 const char *const i_moduleName,
                                 const char *const i_functionName,
                                 Ret (*function)(wasm_api::HostCallContext*, Args...)) {

                return m3_LinkRawFunctionEx(io_module, i_moduleName, i_functionName,
                                            m3_signature<Ret, Args...>::value,
                                            &wrap_helper<Ret(wasm_api::HostCallContext*, Args...)>::wrap_fn,
                                            reinterpret_cast<void*>(function));
            }
        };
        template<typename ... Args>
        class m3_wrapper<void(wasm_api::HostCallContext*, Args...)> {
        public:
            static M3Result link(IM3Module io_module,
                                 const char *const i_moduleName,
                                 const char *const i_functionName,
                                 void (*function)(wasm_api::HostCallContext*, Args...)) {

                return m3_LinkRawFunctionEx(io_module, i_moduleName, i_functionName,
                                            m3_signature<void, Args...>::value,
                                            &wrap_helper<void(wasm_api::HostCallContext*, Args...)>::wrap_fn,
                                            reinterpret_cast<void*>(function));
            }
        };

        template<typename ret_t>
        struct opt_t;

        struct empty {};

        template<> struct opt_t<void> {
            using type = std::optional<empty>;
        };

        template<typename ret_t>
        struct opt_t {
            using type = std::optional<ret_t>;
        };
    } // namespace detail
    /** @endcond */

    /** @cond */
    namespace detail {

        static inline void throw_nondeterministic_errors(M3Result err)
        {
            if (err == m3Err_mallocFailed) {
                throw wasm_api::UnrecoverableSystemError(err);
            }
            if (err == m3Err_mallocFailedCodePage) {
                throw wasm_api::UnrecoverableSystemError(err);
            }
            if (err == m3Err_unrecoverableSystemError) {
                // rethrow, perhaps
                throw wasm_api::UnrecoverableSystemError(err);
            }
        }
    } // namespace detail
    /** @endcond */

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
        environment() {
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
        std::unique_ptr<runtime> new_runtime(size_t stack_size_bytes, void* ctxp);

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
        bool
        __attribute__((warn_unused_result))
        load(module &mod);

        /**
         * Get a function handle by name
         * 
         * If the function is not found, returns nullopt
         * @param name  name of a function, c-string
         * @return function object
         */
        std::optional<function> find_function(const char *name);

        std::pair<uint8_t*, size_t> get_memory();

        friend class environment;

        runtime(const std::shared_ptr<M3Environment> &env, size_t stack_size_bytes, void* ctxp)
                : m_env(env) {
            m_runtime.reset(m3_NewRuntime(env.get(), stack_size_bytes, ctxp), &m3_FreeRuntime);
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
     * It can be constructed by parsing a WASM binary using environment::parse_module.
     * Functions can be linked to the loaded module.
     * Once constructed, modules can be loaded into the runtime.
     */
    class module {
    public:
        /**
         * Link an external function.
         *
         * Returns false if the module doesn't reference a function with the given name.
         *
         * @tparam Func Function type (signature)
         * @param module  Name of the module to link the function to, or "*" to link to any module
         * @param function_name  Name of the function (as referenced by the module)
         * @param function  Function to link (a function pointer)
         */
        template<typename ret, typename...Args>
        bool link(const char *module, const char *function_name, ret (*function)(wasm_api::HostCallContext*, Args...));
        template<typename...Args>
        bool link(const char *module, const char *function_name, void (*function)(wasm_api::HostCallContext*, Args...));

        /**
         * Same as module::link, but doesn't return false if the function is not referenced.
         */
        template<typename ret, typename...Args>
        bool link_optional(const char *module, const char *function_name, ret (*function)(wasm_api::HostCallContext*, Args...));
        template<typename...Args>
        bool link_optional(const char *module, const char *function_name, void (*function)(wasm_api::HostCallContext*, Args...));

        ~module()
        {
            if ((!m_loaded) && (m_module != nullptr))
            {
                m3_FreeModule(m_module);
            }
        }

        friend class environment;
        friend class runtime;

        module(std::istream &in_wasm) {
            in_wasm.unsetf(std::ios::skipws);
            std::copy(std::istream_iterator<uint8_t>(in_wasm),
                      std::istream_iterator<uint8_t>(),
                      std::back_inserter(m_moduleRawData));
        }

        module(const uint8_t *data, size_t size) {
            m_moduleRawData = std::vector<uint8_t>{data, data + size};
        }

        bool 
        __attribute__((warn_unused_result))
        init(const std::shared_ptr<M3Environment> &env) {
            // exists only to extend lifetime of env
            m_env = env;
            return parse(env.get(), m_moduleRawData.data(), m_moduleRawData.size());
        }

    protected:

        module(module&&) = delete;
        module(module&) = delete;
        module& operator=(const module&) = delete;

        bool
        __attribute__((warn_unused_result))
        parse(IM3Environment env, const uint8_t *data, size_t size) {
            IM3Module p;
            M3Result err = m3_ParseModule(env, &p, data, size);
            detail::throw_nondeterministic_errors(err);
            if (err != m3Err_none) {
                return false;
            }

            if (m_loaded)
            {
                m3_FreeModule(m_module);
            }
            m_module = p;
            return true;
        }

        bool
        __attribute__((warn_unused_result))
        load_into(IM3Runtime runtime) {
            M3Result err = m3_LoadModule(runtime, m_module);
            detail::throw_nondeterministic_errors(err);
            if (err != m3Err_none)
            {
                return false;
            }
            m_loaded = true;
            return true;
        }

        // m_env exists only to extend lifetime of environment
        std::shared_ptr<M3Environment> m_env;
        IM3Module m_module;

        bool m_loaded = false;
        std::vector<uint8_t> m_moduleRawData {};
    };


    /**
     * Handle of a function. Can be obtained from runtime::find_function method by name.
     */
    class function {
    public:

        /**
         * Call the function with the provided arguments (int/float types).
         *
         * Note that the type of the return value must be explicitly specified as a template argument.
         *
         */
        template<typename Ret = void, typename ... Args>
        std::pair<typename detail::opt_t<Ret>::type, M3Result>
        call(Args... args) {
            const void *arg_ptrs[] = { reinterpret_cast<const void*>(&args)... };
            M3Result res = m3_Call(m_func, sizeof...(args), arg_ptrs);
            detail::throw_nondeterministic_errors(res);
            if (res != m3Err_none)
            {
                return {std::nullopt, res};
            }

            if constexpr (!std::is_void<Ret>::value) {
                Ret ret;
                const void* ret_ptrs[] = { &ret };
                res = m3_GetResults(m_func, 1, ret_ptrs);
                detail::throw_nondeterministic_errors(res);
                if (res != m3Err_none)
                {
                    return {std::nullopt, res};
                }
                return {{ret}, res};
            } else {
                return {detail::empty{}, res};
            }
        }

        friend class runtime;

        function(const std::shared_ptr<M3Runtime> &runtime) : m_runtime(runtime) {
        }

        bool
        __attribute__((warn_unused_result))
        initialize(const char* name) {
            M3Result err = m3_FindFunction(&m_func, m_runtime.get(), name);
            detail::throw_nondeterministic_errors(err);
            return (m_func != nullptr);
        }


    protected:

        std::shared_ptr<M3Runtime> m_runtime;
        M3Function *m_func = nullptr;
    };

    inline std::unique_ptr<runtime> environment::new_runtime(size_t stack_size_bytes, void* ctxp) {
        return std::make_unique<runtime>(m_env, stack_size_bytes, ctxp);
    }

    inline std::unique_ptr<module> environment::parse_module(std::istream &in) {
        auto out = std::make_unique<module>(in);
        if (out -> init(m_env)) {
            return out;
        }
        return nullptr;
    }

    inline std::unique_ptr<module> environment::parse_module(const uint8_t *data, size_t size) {
        auto out = std::make_unique<module>(data, size);
        if (out -> init(m_env)) {
            return out;
        }
        return nullptr;
    }

    inline bool
    __attribute__((warn_unused_result))
    runtime::load(module &mod) {
        return mod.load_into(m_runtime.get());
    }

    inline std::optional<function> runtime::find_function(const char *name) {
        auto f = function(m_runtime);
        if (!f.initialize(name)) {
            return std::nullopt;
        }
        return f;
    }

    inline std::pair<uint8_t*, size_t> runtime::get_memory()
    {
        uint32_t len = 0;
        uint8_t* mem = m3_GetMemory(m_runtime.get(), &len, 0);
        return std::make_pair(mem, len);
    }

    template<typename R, typename... Args>
    bool module::link(const char *module, const char *function_name, R (*function)(wasm_api::HostCallContext*, Args...)) {
        using Func = R(wasm_api::HostCallContext*, Args...);
        M3Result ret = detail::m3_wrapper<Func>::link(m_module, module, function_name, function);
        detail::throw_nondeterministic_errors(ret);
        return (ret == m3Err_none);
    }

    template<typename... Args>
    bool module::link(const char *module, const char *function_name, void (*function)(wasm_api::HostCallContext*, Args...)) {
        using Func = void(wasm_api::HostCallContext*, Args...);
        M3Result ret = detail::m3_wrapper<Func>::link(m_module, module, function_name, function);
        detail::throw_nondeterministic_errors(ret);
        return (ret == m3Err_none);
    }

    template<typename R, typename... Args>
    bool module::link_optional(const char *module, const char *function_name, R(*function)(wasm_api::HostCallContext*, Args...)) {
        using Func = R(wasm_api::HostCallContext*, Args...);
        M3Result ret = detail::m3_wrapper<Func>::link(m_module, module, function_name, function);
        detail::throw_nondeterministic_errors(ret);
        return ((ret == m3Err_none) || (ret == m3Err_functionLookupFailed));
    }

    template<typename... Args>
    bool module::link_optional(const char *module, const char *function_name, void(*function)(wasm_api::HostCallContext*, Args...)) {
        using Func = void(wasm_api::HostCallContext*, Args...);
        M3Result ret = detail::m3_wrapper<Func>::link(m_module, module, function_name, function);
        detail::throw_nondeterministic_errors(ret);
        return ((ret == m3Err_none) || (ret == m3Err_functionLookupFailed));
    }

} // namespace wasm3
