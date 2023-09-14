LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO   := qsocket/inc
LOCAL_COPY_HEADERS      := inc/msm_ipc.h
LOCAL_COPY_HEADERS      += inc/qsocket.h
LOCAL_COPY_HEADERS      += inc/qsocket_ipcr.h

include $(BUILD_COPY_HEADERS)