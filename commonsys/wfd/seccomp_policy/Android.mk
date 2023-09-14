LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
LOCAL_MODULE:= wfdservice.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := wfdservice-arm.policy
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SYSTEM_EXT_ETC)/seccomp_policy
LOCAL_SYSTEM_EXT_MODULE := true
include $(BUILD_PREBUILT)
endif
