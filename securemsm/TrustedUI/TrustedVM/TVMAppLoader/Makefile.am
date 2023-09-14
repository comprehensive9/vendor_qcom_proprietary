ACLOCAL_AMFLAGS = -I m4

SECUREMSM_SHIP_PATH := $(WORKSPACE)/security/securemsm
SECUREMSM_PATH := $(WORKSPACE)/security/securemsm-noship

AM_CPPFLAGS = -I$(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/TVMAppLoader/inc \
              -I$(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/TUIApp/inc \
              -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
              -I$(SECUREMSM_PATH)/CommonLib/inc \
              -I$(SECUREMSM_PATH)/ssgtzd \
              -I$(WORKSPACE)/security/commonsys-intf/securemsm/smcinvoke/minksocket/sock \
              -I$(WORKSPACE)/security/commonsys-intf/securemsm/smcinvoke/minksocket/include \
              -I$(WORKSPACE)/system/core/include/ \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/commonsys-intf-securemsm \
              -D strlcpy=g_strlcpy -include glib.h -DUSE_GLIB $(GLIB_CFLAGS) \
              -D LE_FLAVOR \
              -DENABLE_LOG_DEBUG

C_SOURCES = src/main.cpp \
            src/CTVMAppLoader.cpp

bin_PROGRAMS = tvmapploader

tvmapploader_SOURCES = $(C_SOURCES)
tvmapploader_CPPFLAGS = $(AM_CPPFLAGS)
tvmapploader_LDFLAGS = -lpthread @GLIB_LIBS@
tvmapploader_LDADD = -lcutils -lminksocket -lcommonlib -ldl -lcrypto
pkgconfigdir = $(libdir)/pkgconfig

all-local: $(bin_PROGRAMS)
