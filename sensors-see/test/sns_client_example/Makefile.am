# Makefile.am - Automake script for sns_client_example

AM_CFLAGS = -Werror                           \
            -Wall                             \
            -DSNS_TARGET_LE_PLATFORM          \
            -I./inc                           \
            -I../../nanopb/inc                \
            -I../../ssc/client_api            \
            -I${WORKSPACE}/vendor/qcom/proprietary/sensors-see/ssc/nanopb_api/inc

requiredlibs = -llog                              \
               -lcutils                            \
               @SENSORSEE_LIBS@

c_sources = src/sns_client_example.c

bin_PROGRAMS =  sns_client_example
sns_client_example_SOURCES = $(c_sources)
sns_client_example_CFLAGS = $(AM_CFLAGS) @QMIFRAMEWORK_CFLAGS@
sns_client_example_LDFLAGS = @QMIFRAMEWORK_LIBS@
if USE_GLIB
 sns_client_example_CFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 sns_client_example_LDFLAGS += @GLIB_LIBS@
endif
sns_client_example_LDADD = $(requiredlibs)
