LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO   := qmi/inc

LOCAL_COPY_HEADERS      := ../legacy_client/qmi_client.h
LOCAL_COPY_HEADERS      += ../inc/qmi_csvt_srvc.h

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
include $(BUILD_COPY_HEADERS)
