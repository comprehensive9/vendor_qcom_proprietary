# Makefile.am - Automake script for libsns_api

AM_CFLAGS = -Werror                           \
            -Wall                             \
            -DSNS_TARGET_LE_PLATFORM          \
            -DPB_FIELD_16BIT                  \
            -I./inc                           \
            -I../../nanopb/inc                \
            -I../client_api

requiredlibs = ../../nanopb/libnanopb.la

c_sources = src/descriptor.pb.c         \
            src/nanopb.pb.c             \
            src/sns_client.pb.c         \
            ../client_api/sns_client_api_v01.c    \
            src/sns_client_qsocket.pb.c \
            src/sns_registry.pb.c       \
            src/sns_std.pb.c            \
            src/sns_std_sensor.pb.c     \
            src/sns_std_type.pb.c       \
            src/sns_suid.pb.c

lib_LTLIBRARIES = libsns_api.la
libsns_api_la_SOURCES = $(c_sources)
libsns_api_la_CFLAGS = $(AM_CFLAGS) @QMIFRAMEWORK_CFLAGS@
libsns_api_la_LDFLAGS = -shared -version-info 1:0:0  @LDFLAGS@ @QMIFRAMEWORK_LIBS@
if USE_GLIB
 libsns_api_la_CFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 libsns_api_la_LDFLAGS += @GLIB_LIBS@
endif
libsns_api_la_LIBADD = $(requiredlibs)
