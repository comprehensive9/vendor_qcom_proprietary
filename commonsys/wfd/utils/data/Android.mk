LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#            COPY THE WFDCONFIGSINK.XML FILE TO /system/etc
# ---------------------------------------------------------------------------------

ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
LOCAL_MODULE:= wfdconfigsink.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SYSTEM_EXT_ETC)
LOCAL_SYSTEM_EXT_MODULE := true
include $(BUILD_PREBUILT)
endif
