LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

####### header library #######
LOCAL_MODULE := libaoa_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
include $(BUILD_HEADER_LIBRARY)
