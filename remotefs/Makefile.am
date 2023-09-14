root_sbindir = "/sbin"
root_sbin_PROGRAMS = rmt_storage

root_etcdir = "/etc"
root_libdir = "/lib"

AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
	-Wno-trigraphs

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
	-DRMTS_LE_BUILD \
	-D_LARGEFILE64_SOURCE \
	-DLOG_NIDEBUG=0 \
	@QMIF_CFLAGS@ \
	@LIBCUTILS_CFLAGS@


bin_PROGRAMS = rmt_storage
rmt_storage_CC = @CC@

rmt_storage_SOURCES = \
	qmi_rmt_storage/remote_storage_v01.c \
	qmi_rmt_storage/rmt_storage_svc.c

rmt_storage_LDFLAGS = -lpthread @QMIF_LIBS@ @LIBCUTILS_LIBS@
rmt_storage_LDADD = @QMIF_LIBS@ @LIBCUTILS_LIBS@

if USE_GLIB
 AM_CPPFLAGS  += -DUSE_GLIB @GLIB_CFLAGS@
 rmt_storage_LDFLAGS += @GLIB_LIBS@
endif
if USE_SYSTEMD
rmt_storage_systemddir = $(root_libdir)/systemd/system
rmt_storage_systemd_DATA =
rmt_storage_systemd_DATA += rmt_storage.service
rmt_storage_etcdir = $(root_etcdir)/udev/rules.d
rmt_storage_etc_SCRIPTS =
rmt_storage_etc_SCRIPTS += rmtstorage.rules
else
rmt_storage_etcdir = $(root_etcdir)/init.d
rmt_storage_etc_SCRIPTS =
rmt_storage_etc_SCRIPTS += rmt_storage.sh
endif
