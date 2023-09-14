ACLOCAL_AMFLAGS = -I m4

SECUREMSM_SHIP_PATH := $(WORKSPACE)/security/securemsm
SECUREMSM_PATH := $(WORKSPACE)/security/securemsm-noship
TRUSTEDVM_SHIP_PATH := $(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM
TRUSTEDVM_NOSHIP_PATH := $(SECUREMSM_PATH)/TrustedUI/TrustedVM
COMMONSYS_INTF_PATH = $(WORKSPACE)/security/commonsys-intf/securemsm

AM_CPPFLAGS = -I$(TRUSTEDVM_SHIP_PATH)/TUIApp/inc \
              -I$(TRUSTEDVM_SHIP_PATH)/libTrustedUI/inc \
              -I$(TRUSTEDVM_SHIP_PATH)/Composer/TUIComposer/inc \
              -I$(SECUREMSM_SHIP_PATH)/smcinvoke/inc \
              -I$(TRUSTEDVM_NOSHIP_PATH)/CoreService/inc \
              -I$(SECUREMSM_PATH)/CommonLib/inc \
              -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
              -I$(TRUSTEDVM_NOSHIP_PATH)/Composer/LayoutManagerUtils/libgd/include \
              -I$(TRUSTEDVM_NOSHIP_PATH)/Composer/LayoutManagerUtils/libtuipng/include \
              -I$(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/SecureTouch/inc \
              -I$(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/inc \
              -I$(WORKSPACE)/system/core/include/ \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/commonsys-intf-securemsm \
              -D strlcpy=g_strlcpy -include glib.h -DUSE_GLIB $(GLIB_CFLAGS) \
              -D LE_FLAVOR \
              -DENABLE_LOG_DEBUG

AM_CPPFLAGS += -I$(WORKSPACE)/system/core/libcutils \
               -I$(SECUREMSM_NOSHIP_PATH)/smcinvoke/TZCom/inc \
               -I$(TRUSTEDVM_SHIP_PATH)/Composer/TUIComposer/inc \
               -I$(SECUREMSM_NOSHIP_PATH)/CommonLib/inc \
               -I$(TRUSTEDVM_SHIP_PATH)/Composer/DialogComposer/inc \
               -I$(COMMONSYS_INTF_PATH)/smcinvoke/minksocket/sock/ \
               -I$(SECUREMSM_NOSHIP_PATH)/ssgtzd/include/ \
               -I$(SECUREMSM_NOSHIP_PATH)/ssgtzd/ \
               -I$(WORKSPACE)/security/commonsys-intf/securemsm/smcinvoke/minksocket/sock \
               -Dstrlcat=g_strlcat

C_SOURCES = src/CTUISampleApp.cpp \
            ../LayoutSamples/layout_1080x2340.c \
            ../LayoutSamples/layout_1440x2560.c \
            ../LayoutSamples/layout_1080x1960_2160x3840_eid.c \
            ../Composer/CustomComposer/src/TuiCustomComposer.c \
            ../Composer/TUIComposer/src/TuiComposer.c \
            ../Composer/DialogComposer/src/TuiDialogComposer.c \
            ../Composer/DialogComposer/src/pinDialog.c \
            ../Composer/DialogComposer/src/loginDialog.c \
            ../Composer/DialogComposer/src/msgDialog.c \
            ../Composer/DialogComposer/src/customDialog.c \
            ../Composer/DialogComposer/src/eidDialog.c \
            ../Composer/DialogComposer/src/utils.c \
            ../Composer/DialogComposer/src/eidTui.c


lib_LTLIBRARIES = libTUIApp.la
libTUIApp_la_CC = @CC@

libTUIApp_la_SOURCES  = $(C_SOURCES)
libTUIApp_la_CPPFLAGS = $(AM_CPPFLAGS)
libTUIApp_la_LDFLAGS  = -lpthread @GLIB_LIBS@
libTUIApp_la_LIBADD   = -lcutils -lminksocket -lcpion -lcommonlib
libTUIApp_la_LIBADD   += ../libTrustedUI/libTrustedUILib.la ../Composer/LayoutManager/liblayout.la
pkgconfigdir = $(libdir)/pkgconfig
