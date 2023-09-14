LOCAL_PATH := $(call my-dir)
ifeq ($(call is-board-platform-in-list,msmnile),true)
include $(CLEAR_VARS)
LOCAL_MODULE := sensors_list
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .txt
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_CLASS = ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/persist/sensors/
LOCAL_SRC_FILES := sensors_list.txt
include $(BUILD_PREBUILT)
endif
