ifeq ($(CAMX_TDEV_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../../../chi-cdk)
else
LOCAL_PATH := $(CAMX_TDEV_PATH)/nativetest/nativechitest
endif

NATIVETEST_PATH := $(CAMX_TDEV_PATH)/nativetest

JSONPARSER_PATH := $(NATIVETEST_PATH)/jsonparser

include $(CLEAR_VARS)

MIMAS_PLATFORMS := bengal

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/core/build/android/common.mk

LOCAL_SRC_FILES :=                          \
    binary_comp_test.cpp                    \
    camera3metadata.cpp                     \
    camera3stream.cpp                       \
    camera_manager_test.cpp                 \
    camera_module_test.cpp                  \
    chibuffermanager.cpp                    \
    chimetadatautil.cpp                     \
    chimetadata_test.cpp                    \
    chimodule.cpp                           \
    chipipeline.cpp                         \
    chisession.cpp                          \
    chitestcase.cpp                         \
    cmdlineparser.cpp                       \
    commonchipipelineutils.cpp              \
    commonutilslinux.cpp                    \
    commonutilswin32.cpp                    \
    custom_node_test.cpp                    \
    e2e_suite.cpp                           \
    jsonparser.cpp                          \
    multi_camera_test.cpp                   \
    nativetestlog.cpp                       \
    nativetest.cpp                          \
    offline_pipeline_test.cpp               \
    pip_camera_test.cpp                     \
    recipe_test.cpp                         \
    realtime_pipeline_test.cpp              \

LOCAL_INC_FILES :=                          \
    binary_comp_test.h                      \
    camera3common.h                         \
    camera3metadata.h                       \
    camera3stream.h                         \
    camera_manager_test.h                   \
    camera_module_test.h                    \
    chibuffermanager.h                      \
    chimetadatautil.h                       \
    chimetadata_test.h                      \
    chimodule.h                             \
    chipipeline.h                           \
    chisession.h                            \
    chitestcase.h                           \
    cmdlineparser.h                         \
    commonchipipelineutils.h                \
    commonutils.h                           \
    custom_node_test.h                      \
    e2e_suite.h                             \
    jsonparser.h                            \
    multi_camera_test.h                     \
    nativetestlog.h                         \
    nativetest.h                            \
    nchicommon.h                            \
    offline_pipeline_test.h                 \
    pip_camera_test.h                       \
    recipe_test.h                           \
    realtime_pipeline_test.h                \


# Put here any libraries that should be linked by CAMX projects
LOCAL_C_LIBS := $(CAMX_C_LIBS)

# Paths to included headers
LOCAL_C_INCLUDES :=                                     \
    $(CAMX_C_INCLUDES)                                  \
    $(CAMX_CHICDK_PATH)/../camx/src/core/hal/           \
    $(CAMX_CHICDK_PATH)/api/common/                     \
    $(CAMX_CHICDK_PATH)/api/utils/                      \
    $(CAMX_CHICDK_PATH)/api/stats/                      \
    hardware/libhardware/include                        \
    $(JSONPARSER_PATH)/                                 \
    $(JSONPARSER_PATH)/detail                           \
    $(JSONPARSER_PATH)/detail/conversions               \
    $(JSONPARSER_PATH)/detail/input                     \
    $(JSONPARSER_PATH)/detail/iterators                 \
    $(JSONPARSER_PATH)/detail/meta                      \
    $(JSONPARSER_PATH)/detail/output                    \
    $(JSONPARSER_PATH)/thirdparty/hedley                \
    $(NATIVETEST_PATH)/nativechitest/                   \
    system/media/camera/include/system/                 \
    system/media/camera/include/system/                 \
    $(TARGET_OUT_INTERMEDIATES)/include/common/inc      \
    $(TARGET_OUT_INTERMEDIATES)/include/xmllib/inc      \
    $(TARGET_OUT_HEADERS)/camx                          \

ifneq ($(filter $(TARGET_BOARD_PLATFORM),$(MIMAS_PLATFORMS)),)
LOCAL_SRC_FILES += mimas/chipipelineutils.cpp
LOCAL_SRC_FILES += mimas/chitests.cpp
LOCAL_INC_FILES += $(NATIVETEST_PATH)/nativechitest/mimas/
LOCAL_C_INCLUDES += $(NATIVETEST_PATH)/nativechitest/mimas/
else
$(info titan was picked to generate native chi header)
LOCAL_SRC_FILES += titan/chipipelineutils.cpp
LOCAL_SRC_FILES += titan/chitests.cpp
LOCAL_INC_FILES += $(NATIVETEST_PATH)/nativechitest/titan/
LOCAL_C_INCLUDES += $(NATIVETEST_PATH)/nativechitest/titan/
endif

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CFLAGS += -fexceptions            \
                -g                      \
                -Wno-unused-variable

LOCAL_CFLAGS += -DFEATURE_XMLLIB

LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Libraries to statically link
LOCAL_STATIC_LIBRARIES :=               \
    libchiutils

# Libraries to statically link
LOCAL_WHOLE_STATIC_LIBRARIES :=

# Libraries to link
LOCAL_SHARED_LIBRARIES +=           \
    $(NATIVETEST_SHARED_LIBRARIES)  \
    libui                           \
    libcamera_metadata              \
    libcutils                       \
    libhardware                     \
    libhidlbase                     \
    liblog                          \
    libqdMetaData                   \
    libsync                         \
    libutils                        \

LOCAL_HEADER_LIBRARIES +=           \
    display_headers

LOCAL_LDLIBS :=                 \
    -llog                       \
    -lz                         \
    -ldl

LOCAL_LDFLAGS :=

# Binary name
LOCAL_MODULE := nativechitest

# Deployment path under bin
LOCAL_MODULE_RELATIVE_PATH := ../bin
LOCAL_PROPRIETARY_MODULE   := true
include $(BUILD_EXECUTABLE)
