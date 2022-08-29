#pragma once

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
