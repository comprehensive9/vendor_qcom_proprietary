LOCAL_PATH := $(call my-dir)

# ---------------------------------------------------------------------------------
# wfdhdcphalservice
# ---------------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := wfdhdcphalservice.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := wfdhdcphalservice-arm.policy
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/seccomp_policy
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

# ---------------------------------------------------------------------------------
# wifidisplayhalservice
# ---------------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := wifidisplayhalservice.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := wifidisplayhalservice-arm.policy
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/seccomp_policy
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

# ---------------------------------------------------------------------------------
# wfdvndservice
# ---------------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := wfdvndservice.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := wfdvndservice-arm.policy
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/seccomp_policy
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
