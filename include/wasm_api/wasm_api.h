#pragma once

#include "wasm_api/error.h"

#include "wasm_api/build_endian.h"

#include <array>
#include <cstring>
#include <cstdint>
#include <memory>
#include <vector>

namespace wasm_api
{

namespace detail
{
	class Wasm3_WasmContext;
	class Wasm3_WasmRuntime;
} /* detail */

typedef std::array<uint8_t, 32> Hash;
typedef const void* script_context_t;

struct ScriptDB {
	virtual const std::vector<uint8_t>* get_script(const Hash& h, const script_context_t& context) const = 0;
};

class WasmRuntime;

class WasmContext {

private:
	detail::Wasm3_WasmContext* impl;

public:

	WasmContext(const ScriptDB& script_db, const uint32_t MAX_STACK_BYTES);

	WasmContext(const WasmContext&) = delete;
	WasmContext(WasmContext&&) = delete;
	WasmContext& operator=(const WasmContext&) = delete;
 
	std::unique_ptr<WasmRuntime>
	new_runtime_instance(Hash const& script_addr, const script_context_t& context);

	~WasmContext();
};

class WasmRuntime {

	detail::Wasm3_WasmRuntime* impl;

	std::pair<uint8_t*, uint32_t> get_memory();

	std::pair<const uint8_t*, uint32_t> get_memory() const;

	friend class detail::Wasm3_WasmContext;

	template<typename ret, typename... Args>
	void _link_fn(
		const char* module,
		const char* fn_name,
		ret (*f)(Args...));

	template<typename... Args>
	void _link_fn(
		const char* module,
		const char* fn_name,
		void (*f)(Args...));

public:

	template<typename... ImplArgs>
	WasmRuntime(ImplArgs&&... args)
		: impl(new detail::Wasm3_WasmRuntime(std::move(args)...))
		{}

	WasmRuntime(const WasmRuntime&) = delete;
	WasmRuntime(WasmRuntime&&) = delete;
	WasmRuntime& operator=(const WasmRuntime&) = delete;
	 
	template<typename ret>
	ret invoke(const char* method_name);

	template<auto f>
	void link_fn(
		const char* module,
		const char* fn_name)
	{
		_link_fn(module, fn_name, f);
	}

	template<auto f>
	void link_env(
		const char* fn_name)
	{
		_link_fn("env", fn_name, f);
	}

	template<typename ArrayLike>
	ArrayLike load_from_memory(uint32_t offset, uint32_t len)
	{
		auto [mem, mlen] = get_memory();

		check_bounds(mlen, offset, len);


		return ArrayLike(mem + offset, mem + offset + len);
	}

	void
	check_bounds(size_t mlen, size_t offset, size_t len)
	{
		if ((mlen < offset + len) 
			|| (offset + len < offset)) //overflow
		{
			throw HostError(
				"OOB Mem Access: mlen = " 
				+ std::to_string(mlen) 
				+ " offset = " 
				+ std::to_string(offset) 
				+ " len = " 
				+ std::to_string(len));
		}
	}

	template <typename ArrayLike>
	ArrayLike load_from_memory_to_const_size_buf(uint32_t offset)
	{
		ArrayLike out;
		const size_t len = out.size();

		auto [mem, mlen] = get_memory();

		check_bounds(mlen, offset, len);

		std::memcpy(out.data(), mem + offset, len);
		return out;
	}

	template<typename ArrayLike>
	void write_to_memory(ArrayLike const& array, uint32_t offset, uint32_t max_len)
	{

		if (array.size() > max_len)
		{
			throw HostError("Array is too big to write");
		}

		auto [mem, mlen] = get_memory();

		check_bounds(mlen, offset, max_len);

		std::memcpy(mem + offset, array.data(), array.size());
	}

	template<std::integral integer_type>
	void
	write_to_memory(integer_type const& value, uint32_t offset)
	{
		auto [mem, mlen] = get_memory();

		check_bounds(mlen, offset, sizeof(integer_type));

		static_assert(WASM_API_ENDIAN == 0, "invalid endianness");

		std::memcpy(mem + offset, reinterpret_cast<const uint8_t*>(&value), sizeof(integer_type));
	}

	int32_t memcmp(uint32_t lhs, uint32_t rhs, uint32_t max_len) const;

	uint32_t memset(uint32_t dst, uint8_t val, uint32_t len);

	uint32_t safe_memcpy(uint32_t dst, uint32_t src, uint32_t len);

	~WasmRuntime();
};

} /* wasm_api */
