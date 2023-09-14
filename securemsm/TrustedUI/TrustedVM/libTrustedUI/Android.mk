LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
include $(LIBION_HEADER_PATH_WRAPPER)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm-noship
SECUREMSM_SHIP_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm
COMPOSER_PATH := $(SECUREMSM_PATH)/TrustedUI/TrustedVM

commonIncludes := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include


commonIncludes += $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/inc \
                    $(SECUREMSM_SHIP_PATH)/mink/inc/interface/\
                    $(SECUREMSM_PATH)/smcinvoke/TZCom/inc/ \
                    $(SECUREMSM_PATH)/smcinvoke/inc \
                    $(SECUREMSM_PATH)/drm/common/cp_ion/ \
                    $(SECUREMSM_SHIP_PATH)/ssg/mink/include \
                    $(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/TrustedInputVM/inc \
                    $(SECUREMSM_PATH)/CommonLib/inc

LOCAL_C_INCLUDES := $(LIBION_HEADER_PATHS)
LOCAL_C_INCLUDES += $(commonIncludes)

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        liblog \
        libcpion


LOCAL_CFLAGS := -O3

LOCAL_SHARED_LIBRARIES += libminksocket libcommonlib

LOCAL_MODULE := libTrustedUILib

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := src/TrustedUILib.cpp \
                   src/TUIInputCallback.cpp \
                   src/MINKHelper.cpp

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
include $(BUILD_SHARED_LIBRARY)
