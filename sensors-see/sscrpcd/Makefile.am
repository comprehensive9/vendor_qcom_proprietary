# Makefile.am - Automake script for sscrpcd


ACLOCAL_AMFLAGS = -I m4

root_libdir  = "/lib"

AM_CPPFLAGS = -Werror                   \
              -Wall                     \
              -Wno-unused-parameter     \
              -fexceptions              \
              -DSNS_TARGET_LE_PLATFORM  \
              -I./                      \
              -I../sensors-log/         \
              -I../reverserpc/inc

cpp_sources = ./src/sscrpcd.cpp

requiredlibs = ../sensors-log/libsensorslog.la  \
               -llog                            \
               -ldl

bin_PROGRAMS = sscrpcd
sscrpcd_SOURCES = $(cpp_sources)
sscrpcd_CC = @CC@
sscrpcd_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS) @DIAG_CFLAGS@ @PROTOBUF_CFLAGS@
if USE_GLIB
 sscrpcd_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 sscrpcd_LDFLAGS = @GLIB_LIBS@
endif
sscrpcd_LDADD = $(requiredlibs)

if USE_SYSTEMD
sscrpcd_systemddir = $(root_libdir)/systemd/system
sscrpcd_systemd_DATA = sscrpcd.service
sscrpcd_systemd_DATA += init_sscrpcd
endif
