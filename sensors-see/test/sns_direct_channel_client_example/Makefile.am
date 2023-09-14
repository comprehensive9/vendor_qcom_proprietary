# Makefile.am - Automake script for sns_direct_channel_client_example

AM_CPPFLAGS = -Werror                           \
              -Wall                             \
              -Wno-unused-parameter             \
              -Wno-unused-variable              \
              -fexceptions                      \
              -DSNS_TARGET_LE_PLATFORM          \
              -I${WORKSPACE}/vendor/qcom/proprietary/sensors-see/direct_channel_stub/inc   \
              -I../../ssc

requiredlibs = -llog                                                    \
               -lcutils                                                 \
               -ladsprpc                                                \
               -lutils                                                  \
               -lsensorslog                                             \
               @SENSORSEE_LIBS@                                         \
               @LIBCUTILS_LIBS@

cpp_sources = ./sns_direct_channel_client.cpp

bin_PROGRAMS =  sns_direct_channel_client_example
sns_direct_channel_client_example_SOURCES = $(cpp_sources)
sns_direct_channel_client_example_CC = @CC@
sns_direct_channel_client_example_CPPFLAGS = $(AM_CPPFLAGS) @SENSORSEE_LIBS@
sns_direct_channel_client_example_LDFLAGS = -lprotobuf
if USE_GLIB
 sns_direct_channel_client_example_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 sns_direct_channel_client_example_LDFLAGS += @GLIB_LIBS@
endif
sns_direct_channel_client_example_LDADD = $(requiredlibs)
