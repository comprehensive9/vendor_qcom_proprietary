# Makefile.am - Automake script for USTANative

AM_CPPFLAGS = -Werror                         \
              -Wno-unused-parameter           \
              -fexceptions                    \
              -DSNS_TARGET_LE_PLATFORM        \
              -I./                            \
              -I../../sensors-log/            \
              -I../../ssc/                    \
              -I../../sensors-diag-log/inc/

if ENABLE_SNS_DIRECT_CHANNEL
  AM_CPPFLAGS += -I../../direct_channel_stub/inc/
endif

if TARGET_PROTO_PATH_SPECIFIED
  AM_CPPFLAGS += -DSNS_TARGET_PROTO_PATH=\""$(TARGET_PROTO_PATH)\""
endif

cpp_sources = ./sensor_cxt.cpp              \
              ./sensor_suid.cpp             \
              ./sensor.cpp                  \
              ./sensor_descriptor_pool.cpp  \
              ./sensor_message_parser.cpp   \
              ./sensor_attributes.cpp       \
              ./logging_util.cpp

if ENABLE_SNS_DIRECT_CHANNEL
  cpp_sources += ./sensor_direct_channel.cpp
endif

requiredlibs = ../../ssc/libsnsapi.la                   \
               ../../ssc/libssc.la                      \
               ../../sensors-diag-log/libsnsdiaglog.la  \
               -llog                                    \
               -lutils                                  \
               $(PROTOBUF_LIBS)

if ENABLE_SNS_DIRECT_CHANNEL
  requiredlibs += ../../direct_channel_stub/libsns_direct_channel_stub.la
endif

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libUSTANative.la
libUSTANative_la_CC = @CC@
libUSTANative_la_SOURCES = $(cpp_sources)
libUSTANative_la_CPPFLAGS = $(AM_CPPFLAGS) @PROTOBUF_CFLAGS@
libUSTANative_la_LDFLAGS = -shared -version-info 1:0:0
libUSTANative_la_LIBADD = $(requiredlibs)
