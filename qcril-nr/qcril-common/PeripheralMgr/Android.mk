LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

## Build header libraries
include $(CLEAR_VARS)
LOCAL_MODULE               := qtiPeripheralMgr-headers
LOCAL_VENDOR_MODULE        := true
LOCAL_EXPORT_C_INCLUDE_DIRS:= $(LOCAL_PATH)/inc
include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/common)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src/android)

LOCAL_MODULE               := qtiPeripheralMgr
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libmdmdetect
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += libcutils_headers
LOCAL_HEADER_LIBRARIES     += qtiPeripheralMgr-headers

LOCAL_EXPORT_HEADER_LIBRARIES    += qtiPeripheralMgr-headers

include $(BUILD_STATIC_LIBRARY)
