# Makefile.am - Automake script for audiod

ACLOCAL_AMFLAGS = -I m4
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audiod.pc
EXTRA_DIST = $(pkgconfig_DATA)

AM_CFLAGS = -Wundef \
            -Wstrict-prototypes \
            -Wno-trigraphs \
            -g -O0 \
            -fno-inline \
            -fno-short-enums \
            -fpic

AM_LDFLAGS = -lutils \
             -lcutils \
             -llog \
             -lbinder \
             -lasound \
             -lpthread

h_sources = AutoAudioDaemon.h \
            auto_audio_ext.h \
            AutoPower.h

c_sources = auto_audiod_main.cpp \
            AutoAudioDaemon.cpp \
            auto_audio_ext.cpp

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libaudiod.la
libaudiod_la_CC = @CC@
libaudiod_la_SOURCES = $(c_sources) $(h_sources)
libaudiod_la_CFLAGS = @GLIB_CFLAGS@ $(AM_CFLAGS) -DUSE_GLIB
libaudiod_la_CPPFLAGS = @GLIB_CFLAGS@ -DUSE_GLIB -DLINUX_ENABLED -D@TARGET_BOARD_PLATFORM@
libaudiod_la_LDFLAGS = @GLIB_LIBS@ -shared -version-info 1:0:0

bin_PROGRAMS = audiod
audiod_CC = @CC@
audiod_SOURCES = $(c_sources) $(h_sources)
audiod_CFLAGS = @GLIB_CFLAGS@ $(AM_CFLAGS) -DUSE_GLIB
audiod_CPPFLAGS = @GLIB_CFLAGS@ -DUSE_GLIB -DLINUX_ENABLED -D@TARGET_BOARD_PLATFORM@
audiod_LDFLAGS = @GLIB_CFLAGS@ $(AM_LDFLAGS)
