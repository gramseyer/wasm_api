#include "wasm_api/wasm3_api.h"

namespace wasm_api
{

namespace detail
{


std::unique_ptr<WasmRuntime> 
Wasm3_WasmContext::new_runtime_instance(Hash const& script_addr, const script_context_t& context)
{
	auto const* contract = script_db.get_script(script_addr, context);

	if (contract == nullptr)
	{
		return nullptr;
	}

	auto module = env.parse_module(contract->data(), contract->size());

	auto runtime = env.new_runtime(MAX_STACK_BYTES);

	runtime->load(*module);

	return std::make_unique<WasmRuntime>(std::move(runtime), std::move(module));
}

template<typename ret>
ret  
Wasm3_WasmRuntime::invoke(const char* method_name)
{
	auto fn = runtime->find_function(method_name);

	return fn.template call<ret>();
}

template
void
Wasm3_WasmRuntime::invoke<void>(const char* methodname);

template
int32_t
Wasm3_WasmRuntime::invoke<int32_t>(const char* methodname);

} /* detail */

} /* wasm_api */
