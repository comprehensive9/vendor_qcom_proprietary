LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ftm_flows_test
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

LOCAL_CFLAGS := -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES := libwigig_ftm_flows

LOCAL_HEADER_LIBRARIES := libwigig_ftm_flows_headers

LOCAL_SRC_FILES := $(shell find $(LOCAL_PATH) -type f \( -name "*.c" \) | sed s:^$(LOCAL_PATH)::g)

include $(BUILD_EXECUTABLE)
