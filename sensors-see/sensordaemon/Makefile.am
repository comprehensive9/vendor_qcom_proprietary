# Makefile.am - Automake script for Sensordaemon

ACLOCAL_AMFLAGS = -I m4

root_etcdir  = "/etc"
root_libdir  = "/lib"

AM_CPPFLAGS = -Werror                   \
              -Wall                     \
              -Wno-unused-parameter     \
              -Wno-unused-result        \
              -fexceptions              \
              -fno-strict-aliasing      \
              -DSNS_TARGET_LE_PLATFORM  \
              -I./src/                  \
              -I../sscrpcd/inc/         \
              -I../sensors-log          \
              -I../ssc

cpp_sources = ./src/aont.cpp          \
              ./src/sensordaemon.cpp

requiredlibs = ../ssc/libsnsapi.la              \
               ../sensors-log/libsensorslog.la  \
               ../ssc/libssc.la                 \
               -llog                            \
               -lcutils                         \
               @DIAG_LIBS@                     \
               @PROTOBUF_LIBS@

bin_PROGRAMS = sensors.qti
sensors_qti_SOURCES = $(cpp_sources)
sensors_qti_CC = @CC@
sensors_qti_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS) @DIAG_CFLAGS@ @PROTOBUF_CFLAGS@
if USE_GLIB
 sensors_qti_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 sensors_qti_LDFLAGS = @GLIB_LIBS@
endif
sensors_qti_LDADD = $(requiredlibs)

if USE_SYSTEMD
sensors_qti_systemddir = $(root_libdir)/systemd/system
sensors_qti_systemd_DATA = sensors-see-daemon.service
sensors_qti_systemd_DATA += init_qti_sensors
if USE_QTI_SLPI
sensors_qti_systemd_DATA += init_slpi.service
endif
endif
