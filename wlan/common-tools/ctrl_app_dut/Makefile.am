AM_CFLAGS = -Wall \
            $(DIAG_CFLAGS) \
            $(LIBNL_CFLAGS)

if DEBUG
AM_CFLAGS += -g
else
AM_CFLAGS += -O2
endif

requiredlibs = -lnl-3 -lnl-genl-3 -lrt

if USE_GLIB
AM_CFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
requiredlibs += $(GLIB_LIBS)
endif

if CLD80211
AM_CFLAGS += -DCONFIG_CLD80211_LIB
requiredlibs += -lcld80211
endif

AM_CFLAGS += -D_DUT_
AM_CFLAGS += -DMDM
AM_CFLAGS += -DCONFIG_CTRL_IFACE_CLIENT_DIR=\"/var/run/wpa_supplicant\"

ctrl_app_dut_SOURCES = main.c eloop.c indigo_api.c indigo_packet.c utils.c wpa_ctrl.c indigo_api_callback_dut.c vendor_specific_dut.c

ctrl_app_dut_LDADD = $(DIAG_LIBS) $(requiredlibs)

bin_PROGRAMS = ctrl_app_dut

