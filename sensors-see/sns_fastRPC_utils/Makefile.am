# Makefile.am - Automake script for sns_fastRPC_util

AM_CPPFLAGS = -Werror               \
              -Wall                 \
              -Wno-unused-parameter \
              -fexceptions          \
              -I./inc/              \
              -I../reverserpc/inc

cpp_sources = ./src/sns_fastRPC_utils.cpp

requiredlibs = -ldl     \
               -llog    \
               -lc      \
               -lutils  \
               -lcutils

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libsns_fastRPC_util.la
libsns_fastRPC_util_la_CC = @CC@
libsns_fastRPC_util_la_SOURCES = $(cpp_sources)
libsns_fastRPC_util_la_CPPFLAGS = $(AM_CPPFLAGS) @DIAG_CFLAGS@ @QMIFRAMEWORK_CFLAGS@ @PROTOBUF_CFLAGS@
libsns_fastRPC_util_la_LDFLAGS = -shared -version-info 1:0:0 @LDFLAGS@
if USE_GLIB
 libsns_fastRPC_util_la_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 libsns_fastRPC_util_la_LDFLAGS += @GLIB_LIBS@
endif
libsns_fastRPC_util_la_LIBADD = $(requiredlibs)