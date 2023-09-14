LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE			   := $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_CFLAGS               += -DRIL_SHLIB $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)
LOCAL_SRC_FILES            += $(call all-cc-files-under, src)
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/include
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/common/inc/

LOCAL_MODULE               := libqcrilNrFramework
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libcutils_headers
LOCAL_HEADER_LIBRARIES     += libutils_headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilNrQtiMutex-headers

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES += libqcrilNrLogger

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS               += -DRIL_SHLIB $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)
LOCAL_SRC_FILES            += $(call all-cc-files-under, src)
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/common/inc/

ifneq ($(qcril_sanitize_diag),)
#LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
#LOCAL_SANITIZE			   := $(qcril_sanitize)
endif

LOCAL_MODULE               := libqcrilNrFramework
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libcutils_headers
LOCAL_HEADER_LIBRARIES     += libutils_headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/include
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/

LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES += libqcrilNrLogger

include $(BUILD_HOST_SHARED_LIBRARY)
