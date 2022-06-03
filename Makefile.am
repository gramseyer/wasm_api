
WASM3_SRCS = \
	wasm3/source/m3_api_libc.c \
	wasm3/source/m3_api_meta_wasi.c \
	wasm3/source/m3_api_tracer.c \
	wasm3/source/m3_api_uvwasi.c \
	wasm3/source/m3_api_wasi.c \
	wasm3/source/m3_bind.c \
	wasm3/source/m3_code.c \
	wasm3/source/m3_compile.c \
	wasm3/source/m3_core.c \
	wasm3/source/m3_env.c \
	wasm3/source/m3_exec.c \
	wasm3/source/m3_function.c \
	wasm3/source/m3_info.c \
	wasm3/source/m3_module.c \
	wasm3/source/m3_parse.c

WASM_API_SRCS = \
	wasm_api/wasm_api.cc \
	wasm_api/wasm3_api.cc

TEST_SRCS = \
	tests/wasm3_test.cc \
	tests/mem_test.cc

TEST_WATS = \
	$(wildcard tests/wat/*.wat)

TEST_WASMS = ${TEST_WATS:.wat=.wasm}

.wat.wasm:
	wat2wasm -o $@ $<

lib_LIBRARIES = libwasmapi.a

libwasmapi_a_SOURCES = $(WASM3_SRCS) $(WASM_API_SRCS)

$(WASM_API_SRCS:.cc=.o): CXXFLAGS += -I./include

test_SOURCES = $(WASM3_SRCS) $(WASM_API_SRCS) $(TEST_SRCS) test.cc

$(TEST_SRCS:.cc=.o): CXXFLAGS += $(Catch2_CFLAGS) -I./include
test.o : CXXFLAGS += $(Catch2_CFLAGS)
test.o : $(TEST_WASMS)

check_PROGRAMS = test
TESTS = test

test_LDFLAGS = $(Catch2_LIBS)



