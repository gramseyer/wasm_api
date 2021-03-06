#pragma once

#include <cstdio>

namespace test
{

[[maybe_unused]]
static std::unique_ptr<std::vector<uint8_t>> 
load_wasm_from_file(const char* filename)
{
	FILE* f = std::fopen(filename, "r");

	if (f == nullptr) {
		throw std::runtime_error("failed to load wasm file");
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

