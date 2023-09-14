# Makefile.am - Automake script for ssc_drva_test

AM_CPPFLAGS = -Werror                       \
              -Wall                         \
              -Wno-unused-parameter         \
              -Wno-unused-variable          \
              -fexceptions                  \
              -DSNS_TARGET_LE_PLATFORM      \
              -I./                          \
              -I${WORKSPACE}/vendor/qcom/proprietary/sensors-see/ssc      \
              -I${WORKSPACE}/vendor/qcom/proprietary/sensors-see/sensors-log  \
              -I${WORKSPACE}/vendor/qcom/proprietary/sensors-see/sensors-log/inc

cpp_sources = ./ssc_drva_test.cpp

requiredlibs = -ldl                                     \
               -llog                                    \
               -lcutils                                 \
               $(PROTOBUF_LIBS)                          \
               @SENSORSEE_LIBS@

bin_PROGRAMS =  ssc_drva_test
ssc_drva_test_SOURCES = $(cpp_sources)
ssc_drva_test_CC = @CC@
ssc_drva_test_CPPFLAGS = $(AM_CPPFLAGS)
ssc_drva_test_LDFLAGS = -shared -version-info 1:0:0
if USE_GLIB
 ssc_drva_test_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 ssc_drva_test_LDFLAGS += @GLIB_LIBS@
endif
ssc_drva_test_LDADD = $(requiredlibs)
