#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module - common
#
# GENERAL DESCRIPTION
#   Common location service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := nmea_test_app
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    nmea_test_app.cpp

LOCAL_SHARED_LIBRARIES := \
    libgps.utils \
    libc++ \
    libloc_api_v02

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(TOP)/external/libcxx/src \
    $(TARGET_OUT_HEADERS)/qmi-framework/inc

LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    libloc_api_v02_headers

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_EXECUTABLE)
