AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(SECUREMSM-NOSHIP_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG \
    -I$(WORKSPACE)/security/securemsm/QSEEComAPI \
    -DOE \
    $(SECUREMSMNOSHIP_CFLAGS)

#init_ddir = ${sysconfdir}/init.d
#init_d_SCRIPTS = start_qseecomd

bin_PROGRAMS = qseecomd
qseecomd_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
qseecomd_SOURCES = qseecomd.c
qseecomd_CPPFLAGS = $(AM_CPPFLAGS)
qseecomd_CFLAGS = $(AM_CFLAGS)

if NORPMB_SUPPORT
qseecomd_CFLAGS += -DRPMB_LISTENER_DISABLED
endif
if NOSSD_SUPPORT
qseecomd_CFLAGS += -DSSD_LISTENER_DISABLED
endif

if ENABLE_HYP
qseecomd_CFLAGS += -DLISTENERS_VIRTUALIZED
endif
if LOAD_CMNLIB
qseecomd_CFLAGS += -DENABLE_CMNLIB_LOAD
endif

qseecomd_LDFLAGS = -lpthread -ldl -lcutils -lQseeComApi -lrpmb -lssd -ldrmfs -lion
qseecomd_LDADD = -lQseeComApi -lrpmb -lssd -ldrmfs -lion

if LV_BUILD
qseecomd_CFLAGS += -DENABLE_LV_SECURITY
qseecomd_LDFLAGS += -lGPreqcancel
qseecomd_LDADD +=  -lGPreqcancel
endif

if LE_QSEECOM_ENABLED
qseecomd_CFLAGS += -DLE_QSEECOM_ENABLED
endif

if UC_BUILD
qseecomd_CFLAGS += -DENABLE_UC_SECURITY
endif

qseecomd_LDFLAGS = -lpthread -ldl -lcutils -lQseeComApi -lrpmb -lssd -ldrmfs -lion
qseecomd_LDADD = -lQseeComApi -lrpmb -lssd -ldrmfs -lion
