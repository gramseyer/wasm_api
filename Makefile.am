#
# Copyright 2023 Geoffrey Ramseyer
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include Makefile.am.fragment

.wat.wasm:
	wat2wasm -o $@ $<

lib_LIBRARIES = libwasmapi.a

pkginclude_HEADERS = \
	include/wasm_api/error.h \
	include/wasm_api/wasm_api.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wasm_api.pc

libwasmapi_a_SOURCES = $(wasm_api_SRCS)

test_SOURCES = $(wasm_api_SRCS) $(wasm_api_TEST_SRCS) test.cc

AM_CPPFLAGS = \
	$(gtest_CFLAGS) \
	$(wasm_api_CFLAGS)

LDADD = \
	$(gtest_LIBS) \
	$(wasm_api_LIBS)

test.o : CXXFLAGS += $(Catch2_CFLAGS)
test.o: $(wasm_api_TEST_WASMS)

check_PROGRAMS = test
TESTS = test
