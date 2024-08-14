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

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace wasm_api
{

typedef std::array<uint8_t, 32> Hash;

/* Does not own the underlying memory */
struct Script
{
    uint8_t const* data;
    uint32_t len;
};

constexpr static Script null_script = Script{ .data = nullptr, .len = 0 };

class WasmRuntime;

struct HostCallContext
{
    WasmRuntime* runtime = nullptr;
    void* user_ctx = nullptr;
};

namespace detail
{

template<typename T>
struct MeteredReturn;

template<>
struct MeteredReturn<void>
{
    uint64_t consumed_gas = 0;
    ErrorType panic;
};

template<typename T>
struct MeteredReturn
{
    std::optional<T> out;
    uint64_t consumed_gas = 0;
    ErrorType panic;
};

class WasmRuntimeImpl;
class WasmContextImpl
{
public:
    virtual std::unique_ptr<WasmRuntime>
    new_runtime_instance(Script const& contract, void* ctxp) = 0;

    virtual ~WasmContextImpl() {}

protected:
    WasmContextImpl() = default;

private:
    WasmContextImpl(WasmContextImpl const&) = delete;
    WasmContextImpl(WasmContextImpl&&) = delete;
};

class WasmRuntimeImpl
{
public:
    virtual std::pair<uint8_t*, uint32_t> get_memory() = 0;
    virtual std::pair<const uint8_t*, uint32_t> get_memory() const = 0;

    virtual void link_fn(std::string const& module_name,
                         std::string const& fn_name,
                         uint64_t (*f)(HostCallContext*))
        = 0;
    virtual void link_fn(std::string const& module_name,
                         std::string const& fn_name,
                         uint64_t (*f)(HostCallContext*, uint64_t))
        = 0;
    virtual void link_fn(std::string const& module_name,
                         std::string const& fn_name,
                         uint64_t (*f)(HostCallContext*, uint64_t, uint64_t))
        = 0;
    virtual void link_fn(
        std::string const& module_name,
        std::string const& fn_name,
        uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t))
        = 0;
    virtual void link_fn(
        std::string const& module_name,
        std::string const& fn_name,
        uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t))
        = 0;
    virtual void link_fn(std::string const& module_name,
                         std::string const& fn_name,
                         uint64_t (*f)(HostCallContext*,
                                       uint64_t,
                                       uint64_t,
                                       uint64_t,
                                       uint64_t,
                                       uint64_t))
        = 0;

    virtual detail::MeteredReturn<uint64_t>
    invoke(std::string const& method_name, const uint64_t gas_limit) = 0;

    virtual bool
    __attribute__((warn_unused_result))
    consume_gas(uint64_t gas) = 0;

    virtual uint64_t get_available_gas() const = 0;
    virtual void set_available_gas(uint64_t gas) = 0;

    virtual ~WasmRuntimeImpl() {}

protected:
    WasmRuntimeImpl() = default;

private:
    WasmRuntimeImpl(WasmRuntimeImpl const&) = delete;
    WasmRuntimeImpl(WasmRuntimeImpl&&) = delete;
};

void
check_bounds(uint32_t mlen, uint32_t offset, uint32_t len);

bool
__attribute__((warn_unused_result))
check_bounds_noexcept(uint32_t mlen, uint32_t offset, uint32_t len) noexcept;


template<typename T>
concept VectorLike = requires(const T object) {
    object.data();
    object.size();
};

} // namespace detail

enum class SupportedWasmEngine
{
    WASM3 = 0,
    MAKEPAD_STITCH = 1,
    WASMI = 2,
};

class WasmContext
{
public:
    WasmContext(const uint32_t MAX_STACK_BYTES,
                SupportedWasmEngine engine = SupportedWasmEngine::WASM3);

    std::unique_ptr<WasmRuntime> new_runtime_instance(Script const& script,
                                                      void* ctxp = nullptr);

    ~WasmContext();

private:
    detail::WasmContextImpl* impl;

    WasmContext(const WasmContext&) = delete;
    WasmContext(WasmContext&&) = delete;
    WasmContext& operator=(const WasmContext&) = delete;
    WasmContext& operator=(WasmContext&&) = delete;
};

class WasmRuntime
{
public:
    WasmRuntime(void* ctxp);

    // takes ownership of impl
    void initialize(detail::WasmRuntimeImpl* impl);

    HostCallContext* get_host_call_context() { return &host_call_context; }

    /**
     * Invoke pushes the current available gas limit onto the stack,
     * and then runs using the gas specified by gas_limit.
     * It restores the amount of gas after the invocation.
     * In most use-cases, caller should then deduct (return_value).gas_consumed
     * from some other gas limit.
     **/
    template<typename ret>
    detail::MeteredReturn<ret> invoke(std::string const& method_name,
                                      uint64_t gas_limit = UINT64_MAX)
    {
        if constexpr (std::is_same<ret, void>::value)
        {
        	if (!impl) {
        		return {0, ErrorType::UnrecoverableSystemError};
        	}
        	uint64_t gas_backup = impl -> get_available_gas();
        	auto res = impl -> invoke(method_name, gas_limit);
        	impl -> set_available_gas(gas_backup);
            return detail::MeteredReturn<void>{
                .consumed_gas
                = res.consumed_gas,
                .panic = res.panic
            };
        } else
        {
        	if (!impl) {
        		return {std::nullopt, 0, ErrorType::UnrecoverableSystemError};
        	}
        	uint64_t gas_backup = impl -> get_available_gas();
            auto res = impl->invoke(method_name, gas_limit);
            impl -> set_available_gas(gas_backup);
            std::optional<ret> cast_out = std::nullopt;
            if (res.out.has_value()) {
            	cast_out = {static_cast<ret>(*res.out)};
            }
            return detail::MeteredReturn<ret>{ .out = cast_out,
                                               .consumed_gas
                                               = res.consumed_gas,
                                               .panic = res.panic };
        }
    }

    // At the moment, this only supports uint64 input types.
    template<auto f>
    void link_fn(const char* module, const char* fn_name)
    {
    	if (!impl) {
    		return;	
    	}
        impl->link_fn(module, fn_name, f);
    }

    template<auto f>
    void link_env(const char* fn_name)
    {
    	if (!impl) {
    		return;
    	}
        impl->link_fn("env", fn_name, f);
    }

    template<detail::VectorLike V>
    bool __attribute__((warn_unused_result))
    load_from_memory_noexcept(V& out, uint32_t offset, uint32_t read_len) const noexcept
    {
    	// must preallocate, or else we might error inside noexcept
    	if (out.size() != read_len) {
    		return false;
    	}

    	auto const [mem, mlen] = get_memory();

    	if (!detail::check_bounds_noexcept(mlen, offset, read_len)) {
    		return false;
    	}

    	std::memcpy(out.data(), mem + offset, read_len);
    	return true;
    }



    // can't be _noexcept_ if the allocations might fail
    template<typename ArrayLike>
    ArrayLike load_from_memory(uint32_t offset, uint32_t len) const
    {
    	ArrayLike out;
    	out.resize(len);
        if (!load_from_memory_noexcept(out, offset, len)) {
        	throw HostError("failed to load from memory");
        }
        return out;
    }

    template<typename ArrayLike>
    ArrayLike load_from_memory_to_const_size_buf(uint32_t offset) const
    {
        ArrayLike out;
        const size_t len = out.size();

        if (!load_from_memory_noexcept(out, offset, len))
        {
        	throw HostError("failed to load from memory");
        }

        return out;
    }

    template<typename T, typename... Args>
    void
    write_to_memory(T const& value, Args const& ... args) {
    	if (!write_to_memory_noexcept(value, args...)) {
    		throw HostError("write to memory failed");
    	}
    }


    template<detail::VectorLike V>
    bool
    __attribute__((warn_unused_result))
    write_to_memory_noexcept(V const& array, uint32_t offset, uint32_t max_len) noexcept
    {
        uint32_t write_len = std::min<uint32_t>(max_len, array.size());

        return _write_to_memory_noexcept(array.data(), offset, write_len);
    }

    template<std::integral integer_type>
    bool
    __attribute__((warn_unused_result))
    write_to_memory_noexcept(integer_type const& value, uint32_t offset) noexcept
    {
        static_assert(std::endian::native == std::endian::little);

        return _write_to_memory_noexcept(reinterpret_cast<const uint8_t*>(&value),
                         offset,
                         sizeof(integer_type));
    }

    template<detail::VectorLike V>
    void write_slice_to_memory(V const& array,
                               uint32_t offset,
                               uint32_t slice_start,
                               uint32_t slice_end)
    {
    	if (!write_slice_to_memory_noexcept(array, offset, slice_start, slice_end)) {
    		throw HostError("write slice failed");
    	}
    }

    template<detail::VectorLike V>
    bool 
    __attribute__((warn_unused_result))
    write_slice_to_memory_noexcept(V const& array,
                               uint32_t offset,
                               uint32_t slice_start,
                               uint32_t slice_end) noexcept
    {
        if ((slice_start > slice_end) || (slice_end > array.size()))
        {
            return false;
        }

        return _write_to_memory_noexcept(
            array.data() + slice_start, offset, (slice_end - slice_start));
    }

    int32_t memcmp(uint32_t lhs, uint32_t rhs, uint32_t max_len) const;
    uint32_t memset(uint32_t dst, uint8_t val, uint32_t len);

    uint32_t safe_memcpy(uint32_t dst, uint32_t src, uint32_t len);
    std::optional<uint32_t>
    __attribute__((warn_unused_result))
    safe_memcpy_noexcept(uint32_t dst, uint32_t src, uint32_t len);

    uint32_t safe_strlen(uint32_t start, uint32_t max_len) const;

    /**
     * If consume_gas returns true, then gas was consumed successfully.
     * If not, as much gas as possible is still consumed.
     * (i.e. consumed is max(gas, get_available_gas()))
     */
    bool
    __attribute__((warn_unused_result))
    consume_gas(uint64_t gas);
    uint64_t get_available_gas() const;
    void set_available_gas(uint64_t gas) {
    	if (impl) {
    		impl -> set_available_gas(gas);
    	}
    }

    ~WasmRuntime();

private:
    detail::WasmRuntimeImpl* impl;
    HostCallContext host_call_context;

    std::pair<uint8_t*, uint32_t> get_memory();
    std::pair<const uint8_t*, uint32_t> get_memory() const;

    void _write_to_memory(const uint8_t* src_ptr,
                          uint32_t offset,
                          uint32_t len);

    // true on success
    bool 
    __attribute__((warn_unused_result))
    _write_to_memory_noexcept(const uint8_t* src_ptr,
    	uint32_t offset,
    	uint32_t len) noexcept;

    WasmRuntime(const WasmRuntime&) = delete;
    WasmRuntime(WasmRuntime&&) = delete;
    WasmRuntime& operator=(const WasmRuntime&) = delete;
    WasmRuntime& operator=(WasmRuntime&&) = delete;
};

} // namespace wasm_api
