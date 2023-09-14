LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..
#
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
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := qcrilNrSecureElementModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_SHARED_LIBRARIES     += android.hardware.secure_element@1.0 \
                              android.hardware.secure_element@1.1 \
                              android.hardware.secure_element@1.2 \
                              libqcrilNrQtiMutex \
                              qtiril-utils

LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers \
                              libqcrilNrQtiMutex-headers \
                              qtiril-utils-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers

include $(BUILD_STATIC_LIBRARY)
