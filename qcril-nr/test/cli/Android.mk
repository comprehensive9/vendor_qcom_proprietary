LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG        := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             := $(qcril_sanitize)
endif
LOCAL_SANITIZE             += cfi address
LOCAL_SANITIZE_DIAG        += cfi address
LOCAL_MODULE               := qcril_cli
LOCAL_VENDOR_MODULE        := true
LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CPPFLAGS             += $(qcril_cppflags) -fexceptions
LOCAL_LDFLAGS              += $(qcril_ldflags)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../qcril_client \
    $(LOCAL_PATH)/../../qcril-common/marshalling/inc \
    $(LOCAL_PATH)/../../qcril-common/marshalling/platform/linux/inc \

LOCAL_HEADER_LIBRARIES := \
    vendor_common_inc \

LOCAL_SHARED_LIBRARIES := \
    qcril_client \

LOCAL_SRC_FILES            += $(call all-cpp-files-under,Call)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,console_app_framework)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Data)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,ECall)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Phone)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Ims)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,SimCardServices)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Sms)
LOCAL_SRC_FILES            += TelSdkConsoleApp.cpp
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG        := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             := $(qcril_sanitize)
endif
LOCAL_MODULE               := qcril_cli
LOCAL_VENDOR_MODULE        := true
LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CPPFLAGS             += $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Call)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,console_app_framework)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Data)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,ECall)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Phone)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,SimCardServices)
LOCAL_SRC_FILES            += $(call all-cpp-files-under,Sms)
LOCAL_SRC_FILES            += TelSdkConsoleApp.cpp
#include $(BUILD_HOST_EXECUTABLE)
