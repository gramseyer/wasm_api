#pragma once

#include "wasm_api/error.h"

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

struct ScriptDB {
	virtual const std::vector<uint8_t>* get_script(const Hash& h) const = 0;
};

class WasmRuntime;

class WasmContext {

private:
	detail::Wasm3_WasmContext* impl;

public:

	WasmContext(const ScriptDB& script_db, const uint32_t MAX_STACK_BYTES);
 
	std::unique_ptr<WasmRuntime>
	new_runtime_instance(Hash const& script_addr);

	~WasmContext();
};

class WasmRuntime {

	detail::Wasm3_WasmRuntime* impl;

	std::pair<uint8_t*, uint32_t> get_memory();

	std::pair<const uint8_t*, uint32_t> get_memory() const;

	friend class detail::Wasm3_WasmContext;

	template<typename... Args>
	void _link_fn(
		const char* module,
		const char* fn_name,
		auto (*f)(Args...));

public:

	template<typename... ImplArgs>
	WasmRuntime(ImplArgs&&... args)
		: impl(new detail::Wasm3_WasmRuntime(std::move(args)...))
		{}
	 
	int32_t invoke(const char* method_name);

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
	ArrayLike load_from_memory(int32_t offset, int32_t len)
	{
		auto [mem, mlen] = get_memory();

		if (offset < 0 || len < 0) {
			throw HostError("Invalid Mem Parameters");
		}

		if (mlen < static_cast<uint32_t>(offset) + static_cast<uint32_t>(len)) {
			// did you forget (memory 1 1) in the .wat file
			throw HostError(
				std::string("OOB Mem Access: mlen = ")
				+ std::to_string(mlen) 
				+ " offset = " 
				+ std::to_string(offset) 
				+ " len = " 
				+ std::to_string(len));
		}

		return ArrayLike(mem + offset, mem + offset + len);
	}

	template <typename ArrayLike>
	ArrayLike load_from_memory_to_const_size_buf(int32_t offset)
	{
		ArrayLike out;
		const size_t len = out.size();

		auto [mem, mlen] = get_memory();
		
		if (offset < 0 || len < 0) {
			throw HostError("Invalid Mem Parameters");
		}

		if (mlen < static_cast<uint32_t>(offset) + static_cast<uint32_t>(len)) {
			throw HostError("OOB Mem Access");
		}

		memcpy(out.data(), mem, len);
		return out;
	}

	template<typename ArrayLike>
	void write_to_memory(ArrayLike const& array, uint32_t offset, uint32_t max_len)
	{

		if (array.size() > max_len)
		{
			throw HostError("Array is too big to write");
		}

		if (offset < 0 || max_len < 0) {
			throw HostError("Invalid Mem Parameters");
		}

		auto [mem, mlen] = get_memory();

		if (mlen < offset + array.size()) {
			throw HostError(
				"OOB Mem Write: mlen = " 
				+ std::to_string(mlen) 
				+ " offset = " 
				+ std::to_string(offset) 
				+ " max_len = " 
				+ std::to_string(max_len));
		}

		memcpy(mem + offset, array.data(), array.size());
	}

	int32_t memcmp(uint32_t lhs, uint32_t rhs, uint32_t max_len) const;

	uint32_t memset(uint32_t dst, uint8_t val, uint32_t len);

	uint32_t safe_memcpy(uint32_t dst, uint32_t src, uint32_t len);

	~WasmRuntime();
};

} /* wasm_api */
