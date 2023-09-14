TOP_LOCAL_PATH:= $(call my-dir)
include $(call all-subdir-makefiles)

include $(CLEAR_VARS)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

LOCAL_PATH:= $(TOP_LOCAL_PATH)

SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm

LOCAL_MODULE := libTrustedInput

LOCAL_SRC_FILES := TrustedInputFactory.cpp
LOCAL_C_INCLUDES = $(SECUREMSM_PATH)/TrustedUI/TrustedInput/TrustedInputTZ/include \
                   $(SECUREMSM_PATH)/TrustedUI/TrustedInput/TouchInputVM/inc \
                   $(SECUREMSM_PATH)/TrustedUI/1.0/service/ \


libsecui-def := -g -O3 -Wno-unused-parameter -fno-operator-names
libsecui-def += -D_VENDOR_QTI_
libsecui-def += -D__LINUX__


LOCAL_SHARED_LIBRARIES := liblog \
                          libhidlbase \
                          libbinder \
                          libutils \
                          libcutils \
                          vendor.qti.hardware.trustedui@1.0 \
                          vendor.qti.hardware.trustedui@1.1 \
                          vendor.qti.hardware.trustedui@1.2 \
                          vendor.qti.hardware.systemhelper@1.0 \
                          libTrustedInputTZ \
                          libTouchInputVM

LOCAL_CFLAGS := $(libsecui-def)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_SHARED_LIBRARY)
