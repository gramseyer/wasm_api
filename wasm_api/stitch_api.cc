#include "wasm_api/stitch_api.h"

#include <utility>

struct MemorySlice {
    uint8_t* mem;
    uint32_t size;
};

enum class InvokeError : uint32_t {
	None = 0,
    StitchError = 1,
    FuncNExist = 2,
    InputError = 3, // input validation fails
    ReturnTypeError = 4,
    WasmError = 5,
    CallError = 6,
    UnrecoverableSystemError = 7
};

enum class TrampolineError : uint8_t {
	None = 0,
	HostError = 1,
	UnrecoverableSystemError = 2
};

struct InvokeResult {
    uint64_t result;
    uint32_t error;
};

struct TrampolineResult {
	uint64_t result;
	uint8_t panic;
};

namespace detail {
TrampolineResult success(uint64_t res) {
	return TrampolineResult {res, static_cast<uint8_t>(TrampolineError::None)};
}

TrampolineResult error(TrampolineError error) {
	return TrampolineResult {0, static_cast<uint8_t>(error)};
}

} // namespace detail

extern "C" {

MemorySlice get_memory(void* runtime_pointer);

InvokeResult invoke(void* runtime_pointer, const uint8_t* method_bytes, const uint32_t method_len);

void stitch_link_nargs(void* runtime_pointer,
	const uint8_t* module_bytes, const uint32_t module_bytes_len,
	const uint8_t* method_bytes, const uint32_t method_bytes_len,
	void* fn_pointer,
	uint8_t nargs);

// trampolines

TrampolineResult c_call_0args(void* function_pointer, void* user_ctx) noexcept
{
	try {
		auto* ptr = (uint64_t (*) (void*))(function_pointer);
		return detail::success((*ptr)(user_ctx));
	} catch(wasm_api::HostError&) {
		return detail::error(TrampolineError::HostError);
	} catch(...) {
		return detail::error(TrampolineError::UnrecoverableSystemError);
	}
}

TrampolineResult c_call_1args(void* function_pointer, void* user_ctx, uint64_t arg1) noexcept
{ 
	try {
		auto* ptr = (uint64_t (*) (void*, uint64_t))(function_pointer);
		return detail::success((*ptr)(user_ctx, arg1));
	} catch(wasm_api::HostError&) {
		return detail::error(TrampolineError::HostError);
	} catch(...) {
		return detail::error(TrampolineError::UnrecoverableSystemError);
	}
}

TrampolineResult c_call_2args(void* function_pointer, void* user_ctx, uint64_t arg1, uint64_t arg2) noexcept
{
	try {
		auto* ptr = (uint64_t (*) (void*, uint64_t, uint64_t))(function_pointer);
		return detail::success((*ptr)(user_ctx, arg1, arg2));
	} catch(wasm_api::HostError&) {
		return detail::error(TrampolineError::HostError);
	} catch(...) {
		return detail::error(TrampolineError::UnrecoverableSystemError);
	}
}

TrampolineResult c_call_3args(void* function_pointer, void* user_ctx, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
	try {
		auto* ptr = (uint64_t (*) (void*, uint64_t, uint64_t, uint64_t))(function_pointer);
		return detail::success((*ptr)(user_ctx, arg1, arg2, arg3));
	} catch(wasm_api::HostError&) {
		return detail::error(TrampolineError::HostError);
	} catch(...) {
		return detail::error(TrampolineError::UnrecoverableSystemError);
	}
}

TrampolineResult c_call_4args(void* function_pointer, void* user_ctx, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
	try {
		auto* ptr = (uint64_t (*) (void*, uint64_t, uint64_t, uint64_t, uint64_t))(function_pointer);
		return detail::success((*ptr)(user_ctx, arg1, arg2, arg3, arg4));
	} catch(wasm_api::HostError&) {
		return detail::error(TrampolineError::HostError);
	} catch(...) {
		return detail::error(TrampolineError::UnrecoverableSystemError);
	}
}

TrampolineResult c_call_5args(void* function_pointer, void* user_ctx, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
	try {
		auto* ptr = (uint64_t (*) (void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t))(function_pointer);
		return detail::success((*ptr)(user_ctx, arg1, arg2, arg3, arg4, arg5));
	} catch(wasm_api::HostError&) {
		return detail::error(TrampolineError::HostError);
	} catch(...) {
		return detail::error(TrampolineError::UnrecoverableSystemError);
	}
}

}


namespace wasm_api
{

std::unique_ptr<WasmRuntime>
Stitch_WasmContext::new_runtime_instance(Script const& contract, void* ctxp) {
		return std::make_unique<WasmRuntime>(new Stitch_WasmRuntime(contract, context_pointer, ctxp));
}

std::pair<uint8_t*, uint32_t> 
Stitch_WasmRuntime::get_memory() {
	auto slice = ::get_memory(runtime_pointer);
	return {slice.mem, slice.size};

}
std::pair<const uint8_t*, uint32_t> 
Stitch_WasmRuntime::get_memory() const {
	auto slice = ::get_memory(runtime_pointer);
	return {slice.mem, slice.size};
}

uint64_t
Stitch_WasmRuntime::invoke(std::string const& method_name) {
	auto invoke_res = ::invoke(runtime_pointer, reinterpret_cast<const uint8_t*>(method_name.c_str()), static_cast<uint32_t>(method_name.size()));
	switch (InvokeError(invoke_res.error)) {
	case InvokeError::None:
		return invoke_res.result;
	case InvokeError::StitchError:
		throw UnrecoverableSystemError("internal stitch error");
	case InvokeError::InputError:
		throw WasmError("invalid input fn name");
	case InvokeError::FuncNExist:
		throw WasmError("func nexist");
	case InvokeError::ReturnTypeError:
		throw WasmError("output type error");
	case InvokeError::WasmError:
		throw WasmError("propagating wasm error");
	case InvokeError::CallError:
		throw WasmError("error from call");
	case InvokeError::UnrecoverableSystemError:
		throw UnrecoverableSystemError("propagating unrecoverable error");
	}

	std::unreachable();
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*)) {

	stitch_link_nargs(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f, 0);
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*, uint64_t)) {

	stitch_link_nargs(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f, 1);
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*, uint64_t, uint64_t)) {

	stitch_link_nargs(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f, 2);
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t)) {

	stitch_link_nargs(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f, 3);
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t)) {

	stitch_link_nargs(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f, 4);
}

void 
Stitch_WasmRuntime::link_fn(
	std::string const& module_name,
	std::string const& fn_name,
	uint64_t (*f)(void*, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) {

	stitch_link_nargs(runtime_pointer, (const uint8_t*) module_name.c_str(), module_name.size(),
		(const uint8_t*) fn_name.c_str(), fn_name.size(),
		(void*)f, 5);
}


}