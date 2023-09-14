LOCAL_PATH := $(call my-dir)

common_src_files :=
common_src_files += Logger.cpp
common_src_files += AndroidLogger.cpp

diag_src_files := DiagLogger.cpp

target_src_files :=
target_daemon_src_files :=

## Build header library
include $(CLEAR_VARS)
LOCAL_MODULE               := libril-qc-logger-headers
LOCAL_VENDOR_MODULE        := true
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)
LOCAL_HEADER_LIBRARIES     := libdiag_headers
include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE               := libril-qc-logger-headers
LOCAL_VENDOR_MODULE        := true
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)
LOCAL_IS_HOST_MODULE       := true
include $(BUILD_HEADER_LIBRARY)

##
## Build shared library
##
include $(CLEAR_VARS)

LOCAL_PROTOC_OPTIMIZE_TYPE := full
LOCAL_CFLAGS               += -Wall -Werror -Wno-error=unused-parameter -DTAG=\"libril-qc-logger\"
LOCAL_CXXFLAGS             += -std=c++17
LOCAL_SRC_FILES            += $(common_src_files) $(diag_src_files)

LOCAL_MODULE               := libril-qc-logger
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += $(LOCAL_PATH)

LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += libc++
LOCAL_SHARED_LIBRARIES     += libdiag
LOCAL_HEADER_LIBRARIES     += libril-qc-logger-headers
LOCAL_HEADER_LIBRARIES     += vendor_common_inc
include $(BUILD_SHARED_LIBRARY)


##
## Build host shared library
##
include $(CLEAR_VARS)

LOCAL_PROTOC_OPTIMIZE_TYPE := full
LOCAL_CFLAGS               += -Wall -Werror -Wno-error=unused-parameter -DTAG=\"libril-qc-logger\" -DDEBUG_LOGGER_STDERR
LOCAL_CXXFLAGS             += -std=c++17
LOCAL_SRC_FILES            += $(common_src_files)

LOCAL_MODULE               := libril-qc-logger
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_C_INCLUDES           += $(LOCAL_PATH)

LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += libc++

include $(BUILD_HOST_SHARED_LIBRARY)
