ifeq ($(call is-board-platform-in-list,kona),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES := libqmi_common_headers
LOCAL_HEADER_LIBRARIES += libwigigsensing_headers
LOCAL_C_INCLUDES :=
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH) -type d -name 'inc' -print )

LOCAL_CFLAGS := -Wno-unused-parameter

LOCAL_SRC_FILES := \
    src/main.cpp \
    src/CommandListener.cpp \
    src/SensingDaemon.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libutils \
    libsysutils \
    libbase \
    libhidlbase \
    libwigigsensing

WPA_SUPPL_DIR := $(TOP)/external/wpa_supplicant_8

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../api
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utils/inc
LOCAL_C_INCLUDES += $(WPA_SUPPL_DIR)/src/utils \
                    $(WPA_SUPPL_DIR)/src/common

LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := sensingdaemon

include $(BUILD_EXECUTABLE)

endif