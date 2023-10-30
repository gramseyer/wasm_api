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

#include <cstdio>

#include "wasm_api/wasm_api.h"

#include "config.h"

#ifndef WASMAPI_TESTS_RELDIR
#define WASMAPI_TESTS_RELDIR "" 
#endif

namespace test
{

[[maybe_unused]]
static std::unique_ptr<std::vector<uint8_t>> 
load_wasm_from_file(const char* filename)
{
	std::string base(WASMAPI_TESTS_RELDIR);
	base += filename;
	FILE* f = std::fopen(base.c_str(), "r");

	if (f == nullptr) {
		std::printf("failed to load file %s\n", base.c_str());
		std::fflush(stdout);
		throw std::runtime_error("failed to load wasm file: " + base);
	}
	
	std::unique_ptr<std::vector<uint8_t>> contents = std::make_unique<std::vector<uint8_t>>();

	const int BUF_SIZE = 65536;
	char buf[BUF_SIZE];

	int count = -1;
	while (count != 0) {
		count = std::fread(buf, sizeof(char), BUF_SIZE, f);
		if (count > 0) {
			contents->insert(contents->end(), buf, buf+count);
		}
	}
	std::fclose(f);

	return contents;
}


} /* test */

