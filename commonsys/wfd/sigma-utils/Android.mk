LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libsigmautils

LOCAL_SRC_FILES := sigmautils.cpp
LOCAL_C_INCLUDES += $(TOP)/vendor/qcom/proprietary/common/inc
LOCAL_C_INCLUDES += $(TOP)/vendor/qcom/proprietary/commonsys/wfd/wfd_headers/
LOCAL_C_INCLUDES += $(TOP)/vendor/qcom/proprietary/commonsys/wfd-framework/wfdSvc/

LOCAL_SHARED_LIBRARIES := libgui libui libutils liblog libwfdclient
LOCAL_HEADER_LIBRARIES := libwfd_headers
LOCAL_HEADER_LIBRARIES += libmmosal_headers
LOCAL_MODULE_TAGS := optional
LOCAL_SYSTEM_EXT_MODULE := true
include $(BUILD_SHARED_LIBRARY)
