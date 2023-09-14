LOCAL_PATH := $(call my-dir)

common_src_files :=
common_src_files += Logger.cpp
common_src_files += AndroidLogger.cpp

diag_src_files := DiagLogger.cpp

target_src_files :=
target_daemon_src_files :=

##
## Build shared library
##
include $(CLEAR_VARS)

LOCAL_PROTOC_OPTIMIZE_TYPE := full
LOCAL_CFLAGS               += -DTAG=\"libqcrilNrLogger\"
LOCAL_CXXFLAGS             += -std=c++17

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_SRC_FILES            += $(common_src_files) $(diag_src_files)

LOCAL_MODULE               := libqcrilNrLogger
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += $(LOCAL_PATH)
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS) $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES           += $(TARGET_OUT_HEADERS)/common/inc

LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += libc++
LOCAL_SHARED_LIBRARIES     += libdiag
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libqcrilNrLogger-headers
include $(BUILD_SHARED_LIBRARY)


##
## Build host shared library
##
include $(CLEAR_VARS)

LOCAL_PROTOC_OPTIMIZE_TYPE := full
LOCAL_CFLAGS               += -DTAG=\"libqcrilNrLogger\" -DDEBUG_LOGGER_STDERR
LOCAL_CXXFLAGS             += -std=c++17
LOCAL_SRC_FILES            += $(common_src_files)

LOCAL_MODULE               := libqcrilNrLogger
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += $(LOCAL_PATH)

LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += libc++

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libqcrilNrLogger-headers
include $(BUILD_HOST_SHARED_LIBRARY)
