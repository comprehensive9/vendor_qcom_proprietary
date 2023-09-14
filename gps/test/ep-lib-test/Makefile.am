AM_CFLAGS = \
     -I./ \
     -DMULTIBAND_SUPPORTED \
     -D__STDC_FORMAT_MACROS \
     -Dstrlcpy=g_strlcpy \
     -Dstrlcat=g_strlcat \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

######################
# Build ep_lib_test
######################
ep_lib_test_SOURCES = EpLibMain.cpp

if USE_GLIB
ep_lib_test_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@ -include glib.h
ep_lib_test_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
ep_lib_test_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@ -include glib.h
else
ep_lib_test_CFLAGS = $(AM_CFLAGS)
ep_lib_test_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
ep_lib_test_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

ep_lib_test_LDADD = -lstdc++ -ldl $(EP_LIBS)

bin_PROGRAMS = ep-lib-test

