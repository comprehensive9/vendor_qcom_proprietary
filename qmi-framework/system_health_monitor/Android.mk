LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO   := system_health_monitor/inc
LOCAL_COPY_HEADERS      += sys_health_mon.h

include $(BUILD_COPY_HEADERS)