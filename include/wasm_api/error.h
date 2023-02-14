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

struct UnrecoverableSystemError : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

} /* wasm_api */
