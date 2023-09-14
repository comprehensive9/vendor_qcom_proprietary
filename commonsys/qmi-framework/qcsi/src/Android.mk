LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO   := qmi-framework/inc
LOCAL_COPY_HEADERS      := ../inc/qmi_csi_common.h

include $(BUILD_COPY_HEADERS)