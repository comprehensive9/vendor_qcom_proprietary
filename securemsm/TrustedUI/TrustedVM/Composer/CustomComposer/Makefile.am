#===========================================================================
# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
# ===========================================================================*/

# =============================================================================
#
# Module: TUI Composer
#
# =============================================================================

ACLOCAL_AMFLAGS = -I m4

SECUREMSM_NOSHIP_PATH = $(WORKSPACE)/security/securemsm-noship
TRUSTEDVM_NOSHIP_PATH = $(SECUREMSM_NOSHIP_PATH)/TrustedUI/TrustedVM
SECUREMSM_SHIP_PATH = $(WORKSPACE)/security/securemsm
TRUSTEDVM_SHIP_PATH = $(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM

AM_CFLAGS = -DENABLE_ON_LEVM

AM_CFLAGS += -I$(WORKSPACE)/system/core/libcutils \
             -I$(SECUREMSM_NOSHIP_PATH)/CommonLib/inc \
             -I$(SECUREMSM_NOSHIP_PATH)/smcinvoke/TZCom/inc \
             -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/TUIComposer/inc \
             -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship

C_SOURCES = src/TuiCustomComposer.c

#Compilation for the lib is disabled so it won't be generated,
#the sources are compiled statically as part of tuiapp, eidapp
lib_LTLIBRARIES = libcustomcomposer.la

libcustomcomposer_la_SOURCES = $(C_SOURCES)
libcustomcomposer_la_CFLAGS = $(AM_CFLAGS)
libcustomcomposer_la_LDFLAGS = -lcutils

pkgconfigdir = $(libdir)/pkgconfig
