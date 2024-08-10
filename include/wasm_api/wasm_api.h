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
#include <cstring>
#include <cstdint>
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

constexpr static Script null_script = Script { .data = nullptr, .len = 0 };

class WasmRuntime;

struct HostCallContext {
	WasmRuntime* runtime = nullptr;
	void* user_ctx = nullptr;
};

namespace detail
{

template<typename T> struct MeteredReturn;

template<>
struct MeteredReturn<void> {
	uint64_t consumed_gas = 0;
};

template<typename T>
struct MeteredReturn {
	T out;
	uint64_t consumed_gas = 0;
};

class WasmRuntimeImpl;
class WasmContextImpl {
public:

	virtual std::unique_ptr<WasmRuntime>
	new_runtime_instance(Script const& contract, void* ctxp) = 0;

	virtual ~WasmContextImpl() {}
};

class WasmRuntimeImpl {
public:
	virtual std::pair<uint8_t*, uint32_t> get_memory() = 0;
	virtual std::pair<const uint8_t*, uint32_t> get_memory() const = 0;

	virtual void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(HostCallContext*)) = 0;
	virtual void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(HostCallContext*, uint64_t)) = 0;
	virtual void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(HostCallContext*, uint64_t, uint64_t)) = 0;
	virtual void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t)) = 0;
	virtual void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t)) = 0;
	virtual void link_fn(
		std::string const& module_name,
		std::string const& fn_name,
		uint64_t (*f)(HostCallContext*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) = 0;

	virtual detail::MeteredReturn<uint64_t>
	invoke(std::string const& method_name, uint64_t gas_limit) = 0;

	virtual void
	consume_gas(uint64_t gas) = 0;

	virtual uint64_t
	get_available_gas() const = 0;

	virtual ~WasmRuntimeImpl() {}
protected:
	WasmRuntimeImpl() = default;

private:
	WasmRuntimeImpl(WasmRuntimeImpl const&) = delete;
	WasmRuntimeImpl(WasmRuntimeImpl&&) = delete;
};

void check_bounds(uint32_t mlen, uint32_t offset, uint32_t len);

template<typename T>
concept VectorLike
= requires (const T object)
{
	object.data();
	object.size();
};

} /* detail */

enum class SupportedWasmEngine {
	WASM3 = 0,
	MAKEPAD_STITCH = 1,
	WASMI = 2,
};

class WasmContext {

	detail::WasmContextImpl* impl;

	WasmContext(const WasmContext&) = delete;
	WasmContext(WasmContext&&) = delete;
	WasmContext& operator=(const WasmContext&) = delete;
	WasmContext& operator=(WasmContext&&) = delete;

public:

	WasmContext(const uint32_t MAX_STACK_BYTES, SupportedWasmEngine engine = SupportedWasmEngine::WASM3);
 
	std::unique_ptr<WasmRuntime>
	new_runtime_instance(Script const& script, void* ctxp = nullptr);

	~WasmContext();
};

class WasmRuntime {

	detail::WasmRuntimeImpl* impl;

	std::pair<uint8_t*, uint32_t> get_memory();
	std::pair<const uint8_t*, uint32_t> get_memory() const;

	WasmRuntime(const WasmRuntime&) = delete;
	WasmRuntime(WasmRuntime&&) = delete;
	WasmRuntime& operator=(const WasmRuntime&) = delete;
	WasmRuntime& operator=(WasmRuntime&&) = delete;

	void _write_to_memory(const uint8_t* src_ptr, uint32_t offset, uint32_t len);

	HostCallContext host_call_context;

public:
	WasmRuntime(void* ctxp);

	//takes ownership of impl
	void initialize(detail::WasmRuntimeImpl* impl);

	HostCallContext* get_host_call_context() {
		return &host_call_context;
	}

	template<typename ret>
	detail::MeteredReturn<ret> 
	invoke(std::string const& method_name, uint64_t gas_limit = UINT64_MAX) {
		if constexpr (std::is_same<ret, void>::value) {
			return detail::MeteredReturn<void>{
				.consumed_gas = impl -> invoke(method_name, gas_limit).consumed_gas
			};
		} else {
			auto res = impl -> invoke(method_name, gas_limit);
			return detail::MeteredReturn<ret>{
				.out = static_cast<ret>(res.out),
				.consumed_gas = res.consumed_gas
			};
		}
	}

	// At the moment, this only supports uint64 input types.
	template<auto f>
	void link_fn(
		const char* module,
		const char* fn_name)
	{
		impl -> link_fn(module, fn_name, f);
	}

	template<auto f>
	void link_env(
		const char* fn_name)
	{
		impl -> link_fn("env", fn_name, f);
	}

	template<typename ArrayLike>
	ArrayLike load_from_memory(uint32_t offset, uint32_t len) const
	{
		auto const [mem, mlen] = get_memory();

		detail::check_bounds(mlen, offset, len);

		return ArrayLike(mem + offset, mem + offset + len);
	}

	template <typename ArrayLike>
	ArrayLike load_from_memory_to_const_size_buf(uint32_t offset) const
	{
		ArrayLike out;
		const size_t len = out.size();

		auto const [mem, mlen] = get_memory();

		detail::check_bounds(mlen, offset, len);

		std::memcpy(out.data(), mem + offset, len);
		return out;
	}

	template<detail::VectorLike V>
	void write_to_memory(V const& array, uint32_t offset, uint32_t max_len)
	{
		uint32_t write_len = std::min<uint32_t>(max_len, array.size());

		_write_to_memory(array.data(), offset, write_len);
	}

	template<detail::VectorLike V>
	void 
	write_slice_to_memory(V const& array, uint32_t offset, uint32_t slice_start, uint32_t slice_end)
	{
		if (slice_start > slice_end)
		{
			throw HostError("invalid slice params");
		}

		if (slice_end > array.size())
		{
			throw HostError("array too short");
		}

		_write_to_memory(array.data() + slice_start, offset, (slice_end - slice_start));
	}

	template<std::integral integer_type>
	void
	write_to_memory(integer_type const& value, uint32_t offset)
	{
		static_assert(std::endian::native == std::endian::little);

		_write_to_memory(reinterpret_cast<const uint8_t*>(&value), offset, sizeof(integer_type));
	}

	int32_t memcmp(uint32_t lhs, uint32_t rhs, uint32_t max_len) const;

	uint32_t memset(uint32_t dst, uint8_t val, uint32_t len);

	uint32_t safe_memcpy(uint32_t dst, uint32_t src, uint32_t len);

	uint32_t safe_strlen(uint32_t start, uint32_t max_len) const;

	void consume_gas(uint64_t gas);

	uint64_t get_available_gas() const;

	~WasmRuntime();
};

} /* wasm_api */
