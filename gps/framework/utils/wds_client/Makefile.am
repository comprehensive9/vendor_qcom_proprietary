AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(QMIF_CFLAGS) \
     $(QMI_CFLAGS) \
     $(DATA_CFLAGS) \
     -I./ \
     -D__STDC_FORMAT_MACROS \
     -Dstrlcpy=g_strlcpy \
     -Dstrlcat=g_strlcat \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

libloc_api_wds_h_sources = \
    LocQmiWds.h

libloc_api_wds_c_sources = \
    LocQmiWds.cpp

libloc_api_wds_la_SOURCES = \
    $(libloc_api_wds_h_sources) \
    $(libloc_api_wds_c_sources)

if USE_DSDA
    AM_CFLAGS += -DFEATURE_DSDA_ENABLED
endif

if USE_GLIB
libloc_api_wds_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@ -include glib.h
libloc_api_wds_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_api_wds_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@ -include glib.h
else
libloc_api_wds_la_CFLAGS = $(AM_CFLAGS)
libloc_api_wds_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_api_wds_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_api_wds_la_LIBADD = -ldl -lstdc++ $(GPSUTILS_LIBS) $(QMIF_LIBS) -lqmiservices $(DATA_LIBS)
library_include_HEADERS = \
    LocQmiWds.h

#Create and Install libraries
lib_LTLIBRARIES = libloc_api_wds.la
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(libloc_api_wds_h_sources)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-api-wds.pc
EXTRA_DIST = $(pkgconfig_DATA)

