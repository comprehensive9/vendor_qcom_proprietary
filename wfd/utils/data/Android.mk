LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#            COPY THE WFDCONFIG.XML FILE TO /data
# ---------------------------------------------------------------------------------

LOCAL_MODULE:= wfdconfig.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
