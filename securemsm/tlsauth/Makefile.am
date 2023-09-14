AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -Wno-deprecated-declarations \
        -Werror

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -I$(WORKSPACE)/security/securemsm/GPTEE/inc

requiredlibs = -lcrypto -lGPTEE_vendor -lGPQTEEC_vendor

library_includedir =
library_include_HEADERS =

c_sources = qautheng.c

lib_LTLIBRARIES := libqautheng.la
libqautheng_la_CC = @CC@
libqautheng_la_SOURCES = $(c_sources) $(h_sources)
libqautheng_la_CFLAGS = $(AM_CFLAGS) -fPIC
libqautheng_la_CPPFLAGS = $(AM_CPPFLAGS) $(DATA_INCLUDES)
libqautheng_la_LDFLAGS = -shared -version-info 1:0:0
libqautheng_la_LIBADD = $(requiredlibs)
