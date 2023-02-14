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

#include "wasm_api/wasm_api.h"

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

namespace test
{

class PhonyScriptDB : public wasm_api::ScriptDB
{
	std::map<wasm_api::Hash, std::unique_ptr<std::vector<uint8_t>>> scripts;

public:

	const std::vector<uint8_t>* get_script(const wasm_api::Hash& h,[[maybe_unused]] const wasm_api::script_context_t& context) const override final
	{
		return scripts.at(h).get();
	}

	void add_script(const wasm_api::Hash& h, std::unique_ptr<std::vector<uint8_t>>& script)
	{
		scripts[h] = std::make_unique<std::vector<uint8_t>>(*script);
	}
};

wasm_api::Hash 
static make_hash(uint8_t idx)
{
	wasm_api::Hash h;
	h[0] = idx;
	return h;
}

} /* test */
