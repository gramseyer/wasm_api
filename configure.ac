AC_INIT([wasm_api],[0])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC
AC_LANG(C++)

AC_PROG_RANLIB

AX_APPEND_COMPILE_FLAGS([-pthread])
AX_APPEND_COMPILE_FLAGS([-std=gnu++2a])
AS_IF([test -z "${WFLAGS+set}"], AX_APPEND_COMPILE_FLAGS([-Wall], WFLAGS))

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([Catch2], [Catch2])

AC_C_BIGENDIAN
AC_C_BIGENDIAN(IS_BIG_ENDIAN=1, IS_BIG_ENDIAN=0)
AC_SUBST(IS_BIG_ENDIAN)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile include/wasm_api/build_endian.h wasm_api.pc wasm_api-uninstalled.pc])
AC_OUTPUT
