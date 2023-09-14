LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO   := qrtr/inc
LOCAL_COPY_HEADERS      := lib/libqrtr.h
LOCAL_COPY_HEADERS      += src/ns.h

include $(BUILD_COPY_HEADERS)
