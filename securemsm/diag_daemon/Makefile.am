AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(DIAG_CFLAGS) \
        $(SECUREMSM-NOSHIP_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

requiredlibs = -lrt $(DIAG_LIBS)

bin_PROGRAMS = diagcommd
diagcommd_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
diagcommd_SOURCES = diagcommd.c
diagcommd_CPPFLAGS = $(AM_CPPFLAGS)
diagcommd_CFLAGS = $(AM_CFLAGS)
diagcommd_LDFLAGS = -lpthread -ldl -lcutils -lQseeComApi -lion
diagcommd_LDADD = -lQseeComApi  -lion $(DIAG_LIBS)

