LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               +=  $(qcril_cflags)
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

LOCAL_MODULE               := qcrilNrVoiceModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers \
							  qtiwakelock-headers \
                              libqcrilNrQtiMutex-headers \
                              qtiril-utils-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_SHARED_LIBRARIES     := libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += libsqlite
LOCAL_SHARED_LIBRARIES     += qtiwakelock
LOCAL_SHARED_LIBRARIES     += qtiril-utils
LOCAL_HEADER_LIBRARIES     += libril-db-headers

include $(BUILD_STATIC_LIBRARY)
