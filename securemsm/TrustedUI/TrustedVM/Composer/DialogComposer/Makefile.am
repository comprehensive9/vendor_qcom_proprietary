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
COMMONSYS_INTF_PATH = $(WORKSPACE)/security/commonsys-intf/securemsm

AM_CFLAGS = -DENABLE_ON_LEVM

AM_CFLAGS += -I$(WORKSPACE)/system/core/libcutils \
             -I$(SECUREMSM_NOSHIP_PATH)/CommonLib/inc \
             -I$(SECUREMSM_NOSHIP_PATH)/smcinvoke/TZCom/inc \
             -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/DialogComposer/inc \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/TUIComposer/inc \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/inc \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/SecureTouch/inc \
             -I$(COMMONSYS_INTF_PATH)/smcinvoke/minksocket/sock/ \
             -I$(SECUREMSM_NOSHIP_PATH)/ssgtzd/include/ \
             -I$(SECUREMSM_NOSHIP_PATH)/ssgtzd/ \
             -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship \
             -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/commonsys-intf-securemsm \
             -I$(WORKSPACE)/security/commonsys-intf/securemsm/smcinvoke/minksocket/sock \
             -Dstrlcat=g_strlcat

C_SOURCES = src/TuiDialogComposer.c \
      src/pinDialog.c \
      src/loginDialog.c \
      src/msgDialog.c \
      src/customDialog.c \
      src/eidDialog.c \
      src/utils.c \
      src/eidTui.c

#Compilation for the lib is disabled so it won't be generated,
#the sources are compiled statically as part of tuiapp, eidapp
lib_LTLIBRARIES = libdialogcomposer.la

libdialogcomposer_la_SOURCES = $(C_SOURCES)
libdialogcomposer_la_CFLAGS = $(AM_CFLAGS)
libdialogcomposer_la_LDFLAGS = -lcutils -lcommonlib ../LayoutManager/liblayout.la

pkgconfigdir = $(libdir)/pkgconfig
