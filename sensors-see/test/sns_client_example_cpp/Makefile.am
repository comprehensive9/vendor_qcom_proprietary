# Makefile.am - Automake script for sns_client_example_cpp

AM_CPPFLAGS = -Werror                           \
              -Wall                             \
              -Wno-unused-parameter             \
              -Wno-unused-variable              \
              -fexceptions                      \
              -DSNS_TARGET_LE_PLATFORM          \
              -I./inc                           

requiredlibs = -llog                              \
               -lcutils                           \
               @SENSORSEE_LIBS@                   \
               @LIBUTILS_LIBS@                     \
               @LIBCUTILS_LIBS@

cpp_sources = ./src/sns_client_example.cpp        \
              ./src/ssc_connection_reference.cpp  \
              ./src/ssc_suid_util.cpp

bin_PROGRAMS =  sns_client_example_cpp
sns_client_example_cpp_SOURCES = $(cpp_sources)
sns_client_example_cpp_CC = @CC@
sns_client_example_cpp_CPPFLAGS = $(AM_CPPFLAGS) @SENSORSEE_CFLAGS@ @LIBUTILS_CFLAGS@ @LIBCUTILS_CFLAGS@ @QMIFRAMEWORK_CFLAGS@
sns_client_example_cpp_LDFLAGS = @PROTOBUF_LIBS@ @SENSORSEE_LIBS@ @QMIFRAMEWORK_LIBS@
if USE_GLIB
 sns_client_example_cpp_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 sns_client_example_cpp_LDFLAGS += @GLIB_LIBS@
endif
sns_client_example_cpp_LDADD = $(requiredlibs)
