LOCAL_PATH := $(call my-dir)

###############################
# Build AFL static library    #
###############################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    ril_afl.cpp

LOCAL_CFLAGS := $(qcril_cflags)
LOCAL_CFLAGS += $(qcril_cflags)
LOCAL_CPPLAGS += $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

# TODO: Use settingsd headers from the header library
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../qcril_client \
    $(LOCAL_PATH)/../../qcril-common/interfaces/include \
    $(LOCAL_PATH)/../../qcril-common/marshalling/inc \
    $(LOCAL_PATH)/../../qcril-common/marshalling/platform/linux/inc \
    $(LOCAL_PATH)/../../include \
    $(LOCAL_PATH)/../../qtimutex/include \

LOCAL_HEADER_LIBRARIES := \
    qcrilInterfaces-headers \
    libqcrilNrFramework-headers \
    libqcrilNrLogger-headers \

LOCAL_SHARED_LIBRARIES := \
    qcril_client \
    libqcrilNrLogger \
    libqcrilNrFramework \

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE:= qcril_afl

ifeq ($(ENABLE_AFL), true)
LOCAL_CPPFLAGS += -DQCRIL_ENABLE_AFL
LOCAL_CXXFLAGS += -DQCRIL_ENABLE_AFL
include $(BUILD_STATIC_LIBRARY)
endif

###############################
# Build QC RIL socket writer  #
###############################

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    ril_socket_writer.cpp

LOCAL_HEADER_LIBRARIES := \
    qcrilInterfaces-headers \

LOCAL_SHARED_LIBRARIES := \
    qcril_client \
    libqcrilNrLogger \
    libqcrilNrFramework

LOCAL_WHOLE_STATIC_LIBRARIES := \
    qcril_afl

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../qcril_client \
    $(LOCAL_PATH)/../../qcril-common/interfaces/include \
    $(LOCAL_PATH)/../../qcril-common/marshalling/inc \
    $(LOCAL_PATH)/../../qcril-common/marshalling/platform/linux/inc \
    $(LOCAL_PATH)/../../include \
    $(LOCAL_PATH)/../../qtimutex/include \

LOCAL_CFLAGS += $(qcril_cflags)
LOCAL_CPPLAGS += $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE:= qcril_socket_writer

ifeq ($(ENABLE_AFL), true)
LOCAL_CPPFLAGS += -DQCRIL_ENABLE_AFL
LOCAL_CXXFLAGS += -DQCRIL_ENABLE_AFL
include $(BUILD_EXECUTABLE)
endif