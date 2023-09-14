# Makefile.am - Automake script for see_selftest

AM_CPPFLAGS = -Werror                   \
              -Wall                     \
              -Wno-unused-parameter     \
              -Wno-unused-variable      \
              -fexceptions              \
              -DSNS_TARGET_LE_PLATFORM  \
              -I./                      \
              -I../see_salt

requiredlibs = ../see_salt/libSEESalt.la  \
               -llog                      \
               -lcutils

cpp_sources = ./see_selftest.cpp

bin_PROGRAMS =  see_selftest
see_selftest_SOURCES = $(cpp_sources)
see_selftest_CC = @CC@
see_selftest_CPPFLAGS = $(AM_CPPFLAGS)
if USE_GLIB
 see_selftest_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 see_selftest_LDFLAGS = @GLIB_LIBS@
endif
see_selftest_LDADD = $(requiredlibs)
