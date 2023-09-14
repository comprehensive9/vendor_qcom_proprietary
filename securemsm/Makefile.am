# Makefile.am - Automake script for securemsm

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = sampleclient daemon

SUBDIRS += qapi/build

if TRUSTED_UI
SUBDIRS =  TrustedUI/TrustedVM/TrustedInputVM \
           TrustedUI/TrustedVM/Composer/LayoutManager \
           TrustedUI/TrustedVM/libTrustedUI \
           TrustedUI/TrustedVM/TUIApp \
           TrustedUI/TrustedVM/EIDTUIApp \
           TrustedUI/TrustedVM/TVMAppLoader
endif

if LV_BUILD
SUBDIRS += GPTEE_Sample_client tlsauth km_lite/tests
else
if UC_BUILD
SUBDIRS += km_lite/tests
CFLAGS += -DENABLE_UC_SECURITY
endif
endif

if USE_LEGACY_KM
SUBDIRS += km_lite/tests
endif

export LOAD_CMNLIB

if ENABLE_HYP
CFLAGS += -DQTI_VIRTUAL_KM
endif
export ENABLE_HYP

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = securemsm.pc
EXTRA_DIST = $(pkgconfig_DATA)

