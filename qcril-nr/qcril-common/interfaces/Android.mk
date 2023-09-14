LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

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
LOCAL_SRC_FILES            += $(call all-cpp-files-under, .)

LOCAL_MODULE               := qcrilInterfaces
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     := libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QCRIL_NR_DIR)/include

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilInterfaces-headers

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
#LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
#LOCAL_SANITIZE             += $(qcril_sanitize)
endif
LOCAL_SRC_FILES            += $(call all-cpp-files-under, .)

LOCAL_MODULE               := qcrilInterfaces
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     := libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_C_INCLUDES           := $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_C_INCLUDES           += $(QCRIL_NR_DIR)/include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QCRIL_NR_DIR)/qcrild/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QCRIL_NR_DIR)/include

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilInterfaces-headers

include $(BUILD_HOST_STATIC_LIBRARY)
