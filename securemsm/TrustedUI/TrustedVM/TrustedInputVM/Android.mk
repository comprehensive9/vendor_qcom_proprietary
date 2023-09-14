# =============================================================================
#
# Module: TrustedInput VM Interface
#
# =============================================================================

LOCAL_PATH          := $(call my-dir)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm-noship
SECUREMSM_SHIP_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm
include $(CLEAR_VARS)

LOCAL_MODULE        := TrustedInputService

LOCAL_MODULE_TAGS := optional

LOCAL_CPPFLAGS   := -DTEST_ON_ANDROID

LOCAL_C_INCLUDES   := $(LOCAL_PATH)/inc \
                      $(SECUREMSM_SHIP_PATH)/mink/inc/interface \
                      $(SECUREMSM_PATH)/CommonLib/inc \
                      $(SECUREMSM_PATH)/TrustedUI/TrustedVM/CoreService/inc


LOCAL_SRC_FILES     := src/TrustedInputMain.cpp \
                       src/TouchInput.cpp \
                       src/TouchDevice.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libutils \
                          libminksocket \
                          libcommonlib

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
