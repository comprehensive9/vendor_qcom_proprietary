LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO   := smem_log/inc
LOCAL_COPY_HEADERS      := smem_log.h

include $(BUILD_COPY_HEADERS)