LOCAL_PATH:= $(call my-dir)
#Cannot convert this as header libs cannot be used for Host
include $(CLEAR_VARS)
# Logging Features. Turn any one ON at any time

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../core/lib/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../platform

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_SRC_FILES += qmi_idl_lib.c
LOCAL_SRC_FILES += qmi_idl_accessor.c

LOCAL_MODULE := libidl
LOCAL_SANITIZE=integer_overflow

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_HOST_SHARED_LIBRARY)
