# Makefile.am - Automake script for sns_client_test_cpp_cmdline_args

AM_CPPFLAGS = -Werror                           \
              -Wall                             \
              -Wno-unused-parameter             \
              -Wno-unused-variable              \
              -fexceptions                      \
              -DSNS_TARGET_LE_PLATFORM          \
              -I./inc                           \
              -I../../ssc                       \
              -I../../ssc/client_api            \
              -I../../sensors-log               \
              -I../../sensors-diag-log/inc

requiredlibs = -llog                               \
               -lcutils                            \
               @SENSORSEE_LIBS@                    \
               @LIBUTILS_LIBS@                     \
               @LIBCUTILS_LIBS@

cpp_sources = ./src/sns_client_example.cpp        \
              ./src/ssc_connection_reference.cpp  \
              ./src/ssc_suid_util.cpp

bin_PROGRAMS =  sns_client_test_cpp_cmdline_args
sns_client_test_cpp_cmdline_args_SOURCES = $(cpp_sources)
sns_client_test_cpp_cmdline_args_CC = @CC@
sns_client_test_cpp_cmdline_args_CPPFLAGS = $(AM_CPPFLAGS) @QMIFRAMEWORK_CFLAGS@
sns_client_test_cpp_cmdline_args_LDFLAGS = @QMIFRAMEWORK_LIBS@ @PROTOBUF_LIBS@
if USE_GLIB
 sns_client_test_cpp_cmdline_args_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 sns_client_test_cpp_cmdline_args_LDFLAGS += @GLIB_LIBS@
endif
sns_client_test_cpp_cmdline_args_LDADD = $(requiredlibs)
