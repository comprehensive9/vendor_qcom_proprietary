#===========================================================================
# Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
# ===========================================================================*/

# =============================================================================
#
# Module: LayoutManager
#
# =============================================================================

ACLOCAL_AMFLAGS = -I m4

SECUREMSM_NOSHIP_PATH = $(WORKSPACE)/security/securemsm-noship
TRUSTEDVM_NOSHIP_PATH = $(SECUREMSM_NOSHIP_PATH)/TrustedUI/TrustedVM
SECUREMSM_SHIP_PATH = $(WORKSPACE)/security/securemsm
TRUSTEDVM_SHIP_PATH = $(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM

AM_CFLAGS = -DLEVM -DENABLE_ON_LEVM -DENABLE_MULTIPLE_LAYOUT_SUPPORT

AM_CFLAGS += -I$(WORKSPACE)/system/core/libcutils \
             -I$(SECUREMSM_NOSHIP_PATH)/CommonLib/inc \
             -I$(SECUREMSM_NOSHIP_PATH)/smcinvoke/TZCom/inc \
             -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
             -I$(TRUSTEDVM_NOSHIP_PATH)/Composer/LayoutManagerUtils/libgd/include \
             -I$(TRUSTEDVM_NOSHIP_PATH)/Composer/LayoutManagerUtils/libtuipng/include \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/SecureTouch/inc \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/inc \
             -I$(TRUSTEDVM_SHIP_PATH)/Composer/TUIComposer/inc \
             -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship


C_SOURCES = src/font_manager.c \
            src/layout_manager.c \
            src/secure_display_renderer.c \
            src/qsee_tui_dialogs.c

lib_LTLIBRARIES = liblayout.la

liblayout_la_SOURCES = $(C_SOURCES)
liblayout_la_CFLAGS = $(AM_CFLAGS)
liblayout_la_LDFLAGS = -lcutils -lcommonlib -lgd -ltuipng -lsklayout -lcpion -lcrypto

pkgconfigdir = $(libdir)/pkgconfig


