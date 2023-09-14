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

LOCAL_PATH          := $(call my-dir)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

SECUREMSM_NOSHIP_PATH := $(QC_PROP_ROOT)/securemsm-noship
SECUREMSM_SHIP_PATH := $(QC_PROP_ROOT)/securemsm
TRUSTEDVM_NOSHIP_PATH := $(SECUREMSM_NOSHIP_PATH)/TrustedUI/TrustedVM
TRUSTEDVM_SHIP_PATH := $(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM
COMMONSYS_INTF_PATH = $(QC_PROP_ROOT)/commonsys-intf/securemsm
include $(CLEAR_VARS)

LOCAL_MODULE        := libdialogcomposer

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS    := -DTEST_ON_ANDROID

LOCAL_NOSANITIZE := signed-integer-overflow unsigned-integer-overflow

LOCAL_C_INCLUDES   := $(LOCAL_PATH)/inc \
                      $(SECUREMSM_NOSHIP_PATH)/CommonLib/inc \
                      $(SECUREMSM_NOSHIP_PATH)/smcinvoke/TZCom/inc \
                      $(SECUREMSM_SHIP_PATH)/mink/inc/interface \
                      $(TRUSTEDVM_SHIP_PATH)/Composer/TUIComposer/inc \
                      $(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/inc \
                      $(TRUSTEDVM_SHIP_PATH)/Composer/LayoutManager/SecureTouch/inc \
                      $(COMMONSYS_INTF_PATH)/smcinvoke/minksocket/sock/ \
                      $(SECUREMSM_NOSHIP_PATH)/ssgtzd/include/ \
                      $(SECUREMSM_NOSHIP_PATH)/ssgtzd/

LOCAL_SRC_FILES  :=  src/TuiDialogComposer.c \
          src/pinDialog.c \
          src/loginDialog.c \
          src/msgDialog.c \
          src/customDialog.c \
          src/utils.c

LOCAL_SHARED_LIBRARIES := liblog \
                          libutils \
                          libcommonlib \
                          liblayout

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
