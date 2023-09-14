ACLOCAL_AMFLAGS = -I m4

SECUREMSM_SHIP_PATH := $(WORKSPACE)/security/securemsm
SECUREMSM_PATH := $(WORKSPACE)/security/securemsm-noship

AM_CPPFLAGS = -I${WORKSPACE}/system/core/libcutils \
              -I$(WORKSPACE)/security/commonsys-intf/securemsm/smcinvoke/minksocket/sock \
              -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
              -I$(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/TrustedInputVM/inc \
              -I$(SECUREMSM_PATH)/TrustedUI/TrustedVM/CoreService/inc \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/commonsys-intf-securemsm

AM_CPPFLAGS += -DTEST_ON_LEVM

CPP_SOURCES = src/TouchDevice.cpp \
      src/TouchInput.cpp \
      src/TrustedInputMain.cpp

bin_PROGRAMS = TrustedInputService
TrustedInputService_CC = @CC@

TrustedInputService_SOURCES = $(CPP_SOURCES)
TrustedInputService_CPPFLAGS = $(AM_CPPFLAGS)
TrustedInputService_LDADD = -lcutils -lpthread -lminksocket -lcommonlib

pkgconfigdir = $(libdir)/pkgconfig

all-local: $(bin_PROGRAMS)


