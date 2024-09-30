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

#include <expected>
#include <cstdint>

namespace wasm_api
{

enum class HostFnError : uint8_t {
    NONE_OR_RECOVERABLE = 0,
    RETURN_SUCCESS = 1, // technically "success", but terminates the caller wasm instance
    OUT_OF_GAS = 2,
    UNRECOVERABLE = 3
};

template<typename T>
using HostFnStatus = std::expected<T, HostFnError>;

enum class InvokeError : uint8_t {
  NONE = 0,
  DETERMINISTIC_ERROR = 1,
  OUT_OF_GAS_ERROR = 2,
  UNRECOVERABLE = 3,
  RETURN = 4,
};

template<typename T>
using InvokeStatus = std::expected<T, InvokeError>;

} // namespace wasm_api
