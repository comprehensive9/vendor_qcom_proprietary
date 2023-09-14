# Makefile.am - Automake script for libsns_direct_channel_stub.la

AM_CPPFLAGS = -Werror               \
              -Wall                 \
              -fno-strict-aliasing  \
              -I./inc/

libsns_direct_channel_stub_c_sources = src/sns_direct_channel_stub.c

libsns_direct_channel_stub_requiredlibs = -llog     \
                                          -lc       \
                                          -lutils   \
                                          -lcutils  \
                                          -ladsprpc

library_includedir = $(pkgincludedir)

# install library interface header into ${includedir}
include_HEADERS = inc/sns_direct_channel.h

lib_LTLIBRARIES = libsns_direct_channel_stub.la
libsns_direct_channel_stub_la_CC = @CC@
libsns_direct_channel_stub_la_SOURCES = $(libsns_direct_channel_stub_c_sources)
libsns_direct_channel_stub_la_CPPFLAGS = $(AM_CPPFLAGS)
libsns_direct_channel_stub_la_LDFLAGS = -shared -version-info 1:0:0
libsns_direct_channel_stub_la_LIBADD = $(libsns_direct_channel_stub_requiredlibs)
