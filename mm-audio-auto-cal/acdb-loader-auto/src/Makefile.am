AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic

AM_CPPFLAGS = -I$(top_srcdir)/inc \
              $(ACDBRTAC_CFLAGS) \
              $(AUDCAL_CFLAGS) \
              $(ACDBLOADER_CFLAGS) \
              $(ADIERTAC_CFLAGS) \
              -I.

cpp_sources = acdb-loader-wrapper.cpp \
              acdb-loader-proxy.cpp

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libacdbloaderclient.la

# Build libacdbloaderclient
h_sources  = $(top_srcdir)/inc/acdb-loader-server-client.h \
             $(top_srcdir)/inc/acdb-loader-server.h \
             $(top_srcdir)/inc/acdb-loader-server-interface.h


libacdbloaderclient_la_CC = @CC@
libacdbloaderclient_la_SOURCES = $(cpp_sources) $(h_sources)
libacdbloaderclient_la_CFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libacdbloaderclient_la_CFLAGS += -D__unused=__attribute__\(\(__unused__\)\)
libacdbloaderclient_la_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libacdbloaderclient_la_CPPFLAGS += -DDEBUG_REFS_CALLSTACK_ENABLED=0 -DHAVE_PTHREADS -DHAVE_ANDROID_OS
libacdbloaderclient_la_CXXFLAGS := -std=c++11
libacdbloaderclient_la_LDFLAGS = -lm -llog $(ACDBRTAC_LIBS) $(AUDCAL_LIBS) $(ADIERTAC_LIBS) @GLIB_LIBS@ -shared -avoid-version $(LIBLOG_LIBS) @GLIB_LIBS@
libacdbloaderclient_la_LDFLAGS  += -lcutils -llog -ldl -lbinder -lpthread -lutils
if USE_LIBION_LIBRARY
libacdbloaderclient_la_CPPFLAGS += -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include/ion_headers
libacdbloaderclient_la_LDFLAGS += -lion
endif


# Build acdb loader service deamon
bin_PROGRAMS = acdb_loader_service

acdb_loader_service_SOURCES  := acdb-loader-service.cpp \
                                acdb-loader-server.cpp \
                                acdb-loader-proxy.cpp \
                                $(h_sources)
acdb_loader_service_CPPFLAGS := $(libacdbloaderclient_la_CPPFLAGS) $(libacdbloaderclient_la_CXXFLAGS)
acdb_loader_service_LDFLAGS  := -lutils -lcutils -llog -lpthread -lbinder
acdb_loader_service_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
acdb_loader_service_CPPFLAGS += -DDEBUG_REFS_CALLSTACK_ENABLED=0 -DHAVE_PTHREADS -DHAVE_ANDROID_OS
acdb_loader_service_CXXFLAGS := -std=c++11
acdb_loader_service_LDFLAGS = -lm -llog $(ACDBRTAC_LIBS) $(AUDCAL_LIBS) $(ADIERTAC_LIBS) @GLIB_LIBS@ -shared -avoid-version $(LIBLOG_LIBS) @GLIB_LIBS@
acdb_loader_service_LDFLAGS  += -lcutils -llog -ldl -lbinder -lpthread -lutils

