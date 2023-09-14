ACLOCAL_AMFLAGS = -I m4

SECUREMSM_SHIP_PATH := $(WORKSPACE)/security/securemsm
SECUREMSM_PATH := $(WORKSPACE)/security/securemsm-noship

AM_CPPFLAGS = -I$(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/libTrustedUI/inc \
              -I$(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/TrustedInputVM/inc \
              -I$(WORKSPACE)/system/core/include \
              -I$(WORKSPACE)/system/core/libion/include \
              -I$(SECUREMSM_PATH)/CommonLib/inc \
              -I$(SECUREMSM_PATH)/drm/common/cp_ion \
              -I$(SECUREMSM_PATH)/TrustedUI/TrustedVM/CoreService/inc \
              -I$(WORKSPACE)/security/commonsys-intf/securemsm/smcinvoke/minksocket/sock \
              -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/commonsys-intf-securemsm

C_SOURCES = src/TrustedUILib.cpp \
            src/TUIInputCallback.cpp \
            src/MINKHelper.cpp

lib_LTLIBRARIES := libTrustedUILib.la

libTrustedUILib_la_SOURCES = $(C_SOURCES)
libTrustedUILib_la_CFLAGS = $(AM_CFLAGS)
libTrustedUILib_la_CPPFLAGS = $(AM_CPPFLAGS)
libTrustedUILib_la_LIBADD = -lion -llog -ldl -lcpion -lminksocket -lcommonlib
pkgconfigdir = $(libdir)/pkgconfig
