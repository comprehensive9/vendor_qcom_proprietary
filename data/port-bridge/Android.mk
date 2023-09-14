LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true

LOCAL_COPY_HEADERS_TO   := data/inc
LOCAL_COPY_HEADERS      := inc/portbridge_sockif.h

include $(BUILD_COPY_HEADERS)
