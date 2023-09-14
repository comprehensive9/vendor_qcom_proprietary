LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libwigig_ftm_flows
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

# in Android lib-genl is part of libnl
LOCAL_SHARED_LIBRARIES := libnl

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	external/libnl/include \

LOCAL_CFLAGS += -Wno-unused-parameter

LOCAL_SRC_FILES := $(shell find $(LOCAL_PATH) -type f \( -name "*.c" \) | sed s:^$(LOCAL_PATH)::g)

include $(LOCAL_PATH)/BuildInfoAndroid.mk

include $(BUILD_SHARED_LIBRARY)

####### header library #######
include $(CLEAR_VARS)
LOCAL_MODULE := libwigig_ftm_flows_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
include $(BUILD_HEADER_LIBRARY)
