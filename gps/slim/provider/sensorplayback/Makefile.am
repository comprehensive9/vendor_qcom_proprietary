
# Makefile.am - Automake script for libsensorplayback

#if FEATURE_LOCTECH_SLIM_SENSOR_PLAYBACK
AM_CFLAGS = \
            -DFEATURE_COARSE_POS_TIMESTAMP_AGE \
            -DFEATURE_LOC_MW_EFS_ACCESS \
            -DFEATURE_RPC_CHAR_ARRAY \
            -DFEATURE_GNSS_BIT_API \
            -DFEATURE_LOCTECH_SLIM_NOHANDLES \
            -DSLIM_API="" \
            -DLOG_TAG=\"slim\" \
            -D_GNU_SOURCE \
            -I../../apss/daemon \
            -I../../provider/common \
            -I../../provider/sensorhal \
            -I../../provider/sensorplayback \
            $(GPSUTILS_CFLAGS) \
            $(SLIMUTILS_CFLAGS) \
            $(SLIMCOMMON_CFLAGS) \
            $(SLIMCLIENT_CFLAGS) \
            $(QMIF_CFLAGS) \
            $(DIAG_CFLAGS) \
            $(LOCPLA_CFLAGS) \
            -D__func__=__PRETTY_FUNCTION__ \
            -DHAVE_LIBLOG \
            -Dstrlcat=g_strlcat \
            -Dstrlcpy=g_strlcpy \
            -DFEATURE_LOCTECH_SLIM_LOG_LOGCAT
AM_CPPFLAGS = \
            -D__STDC_FORMAT_MACROS \
            -D__STDINT_LIMITS \
            -D__STDINT_MACROS \
            -DFEATURE_LOCTECH_QMISLIM_PROVIDER_TEST \
            -DFEATURE_LOCTECH_SLIM_DEBUG \
            -DFEATURE_LOCTECH_SLIM_LOG_LOGCAT

h_sources = \
            SlimSensorPlayBackProvider.h

c_sources = \
            SlimSensorPlayBackProviderFileStream.cpp \
            ../../apss/daemon/SlimDaemonManager.cpp \
            ../../apss/listener/ClientListener.cpp \
            ../../provider/common/ProviderBase.cpp \
            ../../provider/common/MultiplexingProvider.cpp \
            ../../provider/common/SlimProviderConf.cpp \
            ../../provider/common/KalmanFilter.cpp

requiredlibs = -lrt $(SLIMUTILS_LIBS) $(SLIMCOMMON_LIBS) $(SLIMCLIENT_LIBS)

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

libsensorplayback_la_SOURCES = $(c_sources) $(h_sources)

if USE_GLIB
libsensorplayback_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libsensorplayback_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libsensorplayback_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libsensorplayback_la_CFLAGS = $(AM_CFLAGS)
libsensorplayback_la_LDFLAGS = -shared -Wl,-z,defs -version-info 1:0:0
endif
libsensorplayback_la_LIBADD = -ldl $(requiredlibs) $(GPSUTILS_LIBS) $(DIAG_LIBS)

lib_LTLIBRARIES = libsensorplayback.la
#endif
