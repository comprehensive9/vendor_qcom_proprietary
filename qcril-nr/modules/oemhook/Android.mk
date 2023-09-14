LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               +=  $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)


ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_MODULE               := qcrilNrOemHookModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers \
                              libqcrilNrQtiMutex-headers \
                              qtiril-utils-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_SHARED_LIBRARIES     := libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += qtiril-utils

LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qcrilhook@1.0

ifeq ($(FEATURE_QCRIL_LTE_DIRECT_ENABLED),true)
LOCAL_CFLAGS               += -DFEATURE_QCRIL_LTE_DIRECT
LOCAL_SHARED_LIBRARIES     += libril-qc-ltedirectdisc
endif

include $(BUILD_STATIC_LIBRARY)
