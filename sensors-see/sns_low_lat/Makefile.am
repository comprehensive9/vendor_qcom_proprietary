# Makefile.am - Automake script for sns_low_lat_stream_stub.la

AM_CPPFLAGS = -Werror                       \
              -Wall                         \
              -Wno-unused-parameter         \
              -fexceptions                  \
              -fno-strict-aliasing          \
              -I./inc/                      \
              -I./../sns_fastRPC_utils/inc  \
              -I../reverserpc/inc

c_sources = src/sns_low_lat_stream_stub.c

requiredlibs = ./prebuilt/lib/q6/libsns_low_lat_stream_skel.so  \
               -ldl                                             \
               -llog                                            \
               -lc                                              \
               -lutils                                          \
               -lcutils

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libsns_low_lat_stream_stub.la
libsns_low_lat_stream_stub_la_CC = @CC@
libsns_low_lat_stream_stub_la_SOURCES = $(c_sources)
libsns_low_lat_stream_stub_la_CPPFLAGS = $(AM_CPPFLAGS)
libsns_low_lat_stream_stub_la_LDFLAGS = -shared -version-info 1:0:0
libsns_low_lat_stream_stub_la_LIBADD = $(requiredlibs)
