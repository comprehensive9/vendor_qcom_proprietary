LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..
#
include $(CLEAR_VARS)

LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

ifeq ($(RIL_USE_ADB_PROP_FOR_APM_SIM_NOT_PWDN),true)
LOCAL_CFLAGS += -DRIL_USE_ADB_PROP_FOR_APM_SIM_NOT_PWDN
LOCAL_CPPFLAGS += -DRIL_USE_ADB_PROP_FOR_APM_SIM_NOT_PWDN
LOCAL_CXXFLAGS += -DRIL_USE_ADB_PROP_FOR_APM_SIM_NOT_PWDN
endif


ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)

LOCAL_MODULE               := qcrilNrNasModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     := libcutils_headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += qtiril-utils-headers
LOCAL_SHARED_LIBRARIES     += libsqlite
LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += qtiril-utils
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += qtiPeripheralMgr-headers
LOCAL_HEADER_LIBRARIES     += qcrilInterfaces-headers
LOCAL_C_INCLUDES           := $(QCRIL_NR_DIR)/qcril-common/interfaces/inc
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += libril-db-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers

include $(BUILD_STATIC_LIBRARY)
