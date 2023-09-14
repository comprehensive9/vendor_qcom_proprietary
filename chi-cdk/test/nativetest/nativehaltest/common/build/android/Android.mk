ifeq ($(CAMX_TDEV_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../../../chi-cdk)
else
LOCAL_PATH := $(CAMX_TDEV_PATH)/nativetest/nativehaltest
endif

NATIVEHALTEST_PATH    := $(CAMX_CHICDK_PATH)/test/nativetest
JSONPARSER_PATH       := $(NATIVEHALTEST_PATH)/jsonparser

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/core/build/android/common.mk


LOCAL_SRC_FILES:=                                \
    cam3device.cpp                               \
    camera_device_test.cpp                       \
    camera_manager_test.cpp                      \
    camera_module_test.cpp                       \
    camera3buffer.cpp                            \
    camera3common.cpp                            \
    camera3device.cpp                            \
    camera3metadata.cpp                          \
    camera3module.cpp                            \
    camera3stream.cpp                            \
    camera3supermodule.cpp                       \
    camera3tests.cpp                             \
    capture_request_test.cpp                     \
    commonutilslinux.cpp                         \
    commonutilswin32.cpp                         \
    concurrent_capture_test.cpp                  \
    extended_camera_characteristics_test.cpp     \
    flush_test.cpp                               \
    halcmdlineparser.cpp                         \
    haljsonparser.cpp                            \
    haltestcase.cpp                              \
    ife_unit_test.cpp                            \
    multi_frame_test.cpp                         \
    nativehalx.cpp                               \
    nativetest.cpp                               \
    nativetestlog.cpp                            \
    partial_metadata_test.cpp                    \
    preview_capture_test.cpp                     \
    recording_test.cpp                           \
    robustness_test.cpp                          \
    staticmetadata.cpp                           \
    still_capture_test.cpp                       \
    vendor_tags_test.cpp                         \

LOCAL_INC_FILES :=                                                  \
    $(TARGET_OUT_HEADERS)/                                          \
    $(TARGET_OUT_HEADERS)                                           \
    cam3device.h                                                    \
    camera_device_test.h                                            \
    camera_manager_test.h                                           \
    camera_module_test.h                                            \
    camera3buffer.h                                                 \
    camera3common.h                                                 \
    camera3device.h                                                 \
    camera3metadata.h                                               \
    camera3module.h                                                 \
    camera3stream.h                                                 \
    camera3supermodule.h                                            \
    capture_request_test.h                                          \
    commonutils.h                                                   \
    concurrent_capture_test.h                                       \
    extended_camera_characteristics_test.h                          \
    flush_test.h                                                    \
    halcmdlineparser.h                                              \
    haljsonparser.h                                                 \
    haltestcase.h                                                   \
    ife_unit_test.h                                                 \
    multi_frame_test.h                                              \
    nativehalx.h                                                    \
    nativetest.h                                                    \
    nativetestlog.h                                                 \
    partial_metadata_test.h                                         \
    preview_capture_test.h                                          \
    recording_test.h                                                \
    robustness_test.h                                               \
    staticmetadata.h                                                \
    still_capture_test.h                                            \
    vendor_tags_test.h                                              \
    $(NATIVEHALTEST_PATH)/nativehaltest/                            \


# Put here any libraries that should be linked by CAMX projects
LOCAL_C_LIBS := $(CAMX_C_LIBS)


# Paths to included headers
LOCAL_C_INCLUDES :=                                                 \
    hardware/libhardware/include                                    \
    system/media/camera/include/system/                             \
    $(TARGET_OUT_HEADERS)/                                          \
    $(CAMX_C_INCLUDES)                                              \
    $(CAMX_CHICDK_PATH)/../ext                                      \
    $(CAMX_CHICDK_PATH)/../ext/system                               \
    $(CAMX_CHICDK_PATH)/../ext/hardware                             \
    $(JSONPARSER_PATH)                                              \
    $(JSONPARSER_PATH)/detail                                       \
    $(JSONPARSER_PATH)/detail/conversions                           \
    $(JSONPARSER_PATH)/detail/input                                 \
    $(JSONPARSER_PATH)/detail/iterators                             \
    $(JSONPARSER_PATH)/detail/meta                                  \
    $(JSONPARSER_PATH)/detail/output                                \
    $(JSONPARSER_PATH)/thirdparty/hedley                            \
    $(NATIVEHALTEST_PATH)/nativehaltest/                            \
    $(CAMX_CHICDK_PATH)/test/nativetest/nativetestutils/            \

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CFLAGS += -fexceptions               \
                -fno-stack-protector       \
                -g                         \
                -Wno-unused-variable

LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Libraries to link
LOCAL_SHARED_LIBRARIES +=           \
    libui                           \
    libcamera_metadata              \
    libcutils                       \
    libhardware                     \
    libhidlbase                     \
    liblog                          \
    libqdMetaData                   \
    libsync                         \
    libutils                        \

LOCAL_LDLIBS :=       \
    -llog             \
    -lz               \
    -ldl              \

LOCAL_LDFLAGS :=

# Binary name
LOCAL_MODULE := nativehaltest

# Deployment path under bin
LOCAL_MODULE_RELATIVE_PATH := ../bin
LOCAL_VENDOR_MODULE        := true
LOCAL_PROPRIETARY_MODULE   := true

include $(BUILD_EXECUTABLE)
