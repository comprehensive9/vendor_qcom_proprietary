# Makefile.am - Automake script for SEESalt libraries

AM_CPPFLAGS = -Werror                       \
              -Wno-unused-parameter         \
              -fexceptions                  \
              -DSNS_TARGET_LE_PLATFORM      \
              -I./                          \
              -I${WORKSPACE}/vendor/qcom/proprietary/sensors-see/USTA/native  \
              -I../../ssc                   \
              -I../../sensors-diag-log/inc

cpp_sources = ./salt_usta.cpp           \
              ./see_salt.cpp            \
              ./por.cpp                 \
              ./parse.cpp               \
              ./see_salt_payloads.cpp   \
              ./see_salt_sensor.cpp     \
              ./see_salt_sleep.cpp

requiredlibs = -llog                              \
               -lutils                             \
               @SENSORSEE_LIBS@

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libSEESalt.la
libSEESalt_la_CC = @CC@
libSEESalt_la_SOURCES = $(cpp_sources)
libSEESalt_la_CPPFLAGS = $(AM_CPPFLAGS) @PROTOBUF_CFLAGS@ @SENSORSEE_CFLAGS@
libSEESalt_la_LDFLAGS = -shared -version-info 1:0:0 @SENSORSEE_CFLAGS@
libSEESalt_la_LIBADD = $(requiredlibs)

