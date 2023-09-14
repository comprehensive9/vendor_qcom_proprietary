AM_MAKEFLAGS = \
		'CXX=$(CXX)'

AM_CFLAGS = \
	-I./ \
	-MMD \
	-O2 \
	-Wall \
	-g

AM_CPPFLAGS = \
	-std=c++14

ACLOCAL_AMFLAGS = -I m4

# libdpp_manager
lib_LTLIBRARIES = libdpp_manager.la
libdpp_manager_la_CXX= @CXX@
libdpp_manager_la_SOURCES = \
	./dppManager.cpp \
	./dppCommonUtils.cpp \
	./dppEnrollee.cpp \
	./dppWpaCtrlIface.cpp \
	./xmlUtil.cpp \
	./wpa_ctrl_utils.cpp

libdpp_manager_la_CXXFLAGS = $(AM_CFLAGS)
libdpp_manager_la_CXXFLAGS += -shared
libdpp_manager_la_CXXFLAGS += -fPIC

# dppdaemon binary
dppdaemon_SOURCES = \
	./dppdaemon.cpp

dppdaemon_LDADD = \
	libdpp_manager.la
dppdaemon_LDFLAGS = -lstdc++ -ldl

if USE_GLIB
dppdaemon_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
dppdaemon_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
dppdaemon_CFLAGS = $(AM_CFLAGS)
dppdaemon_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

#Create and Install libraries
bin_PROGRAMS = dppdaemon



