ifeq ($(TARGET_SUPPORTS_DS),true)
LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)
ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_SRC_FILES            := src/DeepSleepModule.cpp
LOCAL_SRC_FILES            += src/android/AndroidPowerStateHandler.cpp
LOCAL_SRC_FILES            += src/common/PowerStateHandler.cpp

LOCAL_C_INCLUDES           := $(LOCAL_PATH)/src/common

LOCAL_MODULE               := qcrilNrDeepSleepModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     := libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += libutils_headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_SHARED_LIBRARIES     := libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.powerstateservice@1.0
LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libutils

include $(BUILD_STATIC_LIBRARY)
endif
