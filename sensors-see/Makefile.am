# Makefile.am - Automake script for sensors-see

ACLOCAL_AMFLAGS = -I m4

dirs = nanopb                      \
        sensors-log                 \
        ssc/nanopb_api              \
        ssc                         \
        sensors-diag-log            \
        sensordaemon                \
        sscrpcd                     \
        sns_fastRPC_utils           \
        reverserpc                  \
        registry

if ENABLE_SNS_DIRECT_CHANNEL
  dirs += direct_channel_stub
endif

dirs += USTA/native

SUBDIRS = $(dirs)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sensors-see.pc

EXTRA_DIST = $(pkgconfig_DATA)
