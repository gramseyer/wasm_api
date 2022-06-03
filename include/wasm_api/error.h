#pragma once

#include <stdexcept>

namespace wasm_api
{

// calls to builtin fns from the wasm environment should
// throw HostError
struct HostError : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

// Thrown by Wasm API if there's some issue
// arising within a wasm call,
// including when there's an error in a nested call
struct WasmError : public HostError
{
	using HostError::HostError;
};

} /* wasm_api */
