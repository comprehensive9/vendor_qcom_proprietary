LOCAL_PATH := $(call my-dir)

## Build header libraries
include $(CLEAR_VARS)
LOCAL_MODULE               := qtiwakelock-headers
LOCAL_VENDOR_MODULE        := true
LOCAL_EXPORT_C_INCLUDE_DIRS:= $(LOCAL_PATH)/inc
include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE               := qtiwakelock-headers
LOCAL_VENDOR_MODULE        := true
LOCAL_EXPORT_C_INCLUDE_DIRS:= $(LOCAL_PATH)/inc
LOCAL_IS_HOST_MODULE       := true
include $(BUILD_HEADER_LIBRARY)

## Build shared libraries
include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

LOCAL_MODULE               := qtiwakelock
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/common)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src/android)
LOCAL_C_INCLUDES           := $(LOCAL_PATH)/inc
LOCAL_HEADER_LIBRARIES     := qtiwakelock-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_SHARED_LIBRARIES     := libcutils
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += libhardware_legacy

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qtiwakelock-headers
include $(BUILD_SHARED_LIBRARY)

## Build shared libraries for HOST
include $(CLEAR_VARS)

LOCAL_MODULE               := qtiwakelock
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/common)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src/android)
LOCAL_C_INCLUDES           := $(LOCAL_PATH)/inc
LOCAL_HEADER_LIBRARIES     := qtiwakelock-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_SHARED_LIBRARIES     := libcutils
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_CFLAGS               := -DQMI_RIL_UTF

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qtiwakelock-headers
include $(BUILD_HOST_SHARED_LIBRARY)
