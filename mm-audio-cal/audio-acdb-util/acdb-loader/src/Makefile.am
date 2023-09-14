AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic

AM_CPPFLAGS = -I$(top_srcdir)/inc \
              -I$(top_srcdir)/inc/8974 \
              $(ACDBRTAC_CFLAGS) \
              $(AUDCAL_CFLAGS) \
              $(ADIERTAC_CFLAGS) \
              -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include/audio-kernel \
              -I$(top_srcdir)/../acdb-mcs/inc \
              -I$(top_srcdir)/../acdb-fts/inc \
              -I.

h_sources = $(top_srcdir)/inc/8974/acdb-loader.h \
            $(top_srcdir)/inc/acdb-anc-taiko.h \
            $(top_srcdir)/inc/acdb-anc-general.h \
            $(top_srcdir)/inc/anc_map_api.h \
            $(top_srcdir)/inc/vbat_map_api.h \
            $(top_srcdir)/inc/anc_bitmasks.h \
            $(top_srcdir)/inc/wcd9335_anc_registers.h \
            $(top_srcdir)/inc/acdb-anc-tabla.h \
            $(top_srcdir)/../acdb-mcs/inc/acdb-mcs.h \
            $(top_srcdir)/../acdb-mcs/inc/mcs-route-ctrl.h \
            $(top_srcdir)/../acdb-mcs/inc/mcs-tinyalsa-wrapper.h \
            $(top_srcdir)/../acdb-fts/inc/acdb-fts.h

c_sources := acdb-loader.c anc_map.c vbat_map.c
if ENABLE_MCS_FTS
c_sources += ../../acdb-mcs/src/acdb-mcs.c ../../acdb-mcs/src/mcs-route-ctrl.c ../../acdb-fts/src/acdb-fts.c
endif

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

lib_LTLIBRARIES = libacdbloader.la
libacdbloader_la_CC = @CC@
libacdbloader_la_SOURCES = $(c_sources) $(h_sources)
libacdbloader_la_CFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@ -D WCD9320_ANC_BIN_PATH=\"/data/misc/audio/wcd9320_anc.bin\" -D MBHC_BIN_PATH=\"/data/misc/audio/mbhc.bin\" -D MAD_BIN_PATH=\"/data/misc/audio/wcd9320_mad_audio.bin\" -D ETC_ROOT_PATH=\"/etc/\" -D ACDB_BIN_PATH=\"/etc/acdbdata/\" -D DEFAULT_BOARD=\"MTP\"
libacdbloader_la_CFLAGS += -D__unused=__attribute__\(\(__unused__\)\) -D TAVIL_MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer_tavil.txt\" -D MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer.txt\" -D TASHA_MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer_tasha.txt\" -D WSA_MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer_wsa.txt\" -D CSRA1_MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer_csra1.txt\" -D CSRA6_MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer_csra6.txt\" -D CSRA8_MIXER_CONTROL_FILE_PATH=\"/etc/audio_tuning_mixer_csra8.txt\"
libacdbloader_la_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libacdbloader_la_LDFLAGS = -lm -llog $(ACDBRTAC_LIBS) $(AUDCAL_LIBS) $(ADIERTAC_LIBS) @GLIB_LIBS@ -shared -avoid-version
if WITH_TINYALSA
libacdbloader_la_CFLAGS += -D _TINY_ALSA_LIB_
libacdbloader_la_LDFLAGS += -ltinyalsa
endif
if USE_LIBION_LIBRARY
libacdbloader_la_CPPFLAGS += -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include/ion_headers
libacdbloader_la_LDFLAGS += -lion
endif
