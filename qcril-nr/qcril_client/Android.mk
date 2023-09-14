LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
    RilApiSession.cpp \
    RequestManager.cpp \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../qcril-common/marshalling/inc \
    $(LOCAL_PATH)/../qcrild/include/telephony \

LOCAL_HEADER_LIBRARIES := \
    qcrilInterfaces-headers \
    libqcrilNrFramework-headers \
    libqcrilNrLogger-headers \
    libqcrilNrQtiMutex-headers \

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := \
    qcrilMarshal-headers \
    qcrilInterfaces-headers \
    libqcrilNrLogger-headers \

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

LOCAL_CFLAGS +=  $(qcril_cflags)
LOCAL_CPPFLAGS += -std=c++17 $(qcril_cppflags)
LOCAL_CXXFLAGS += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

LOCAL_SHARED_LIBRARIES := \
    qcrilMarshal \
    libqcrilNrFramework \
    qcrild_libqcrilnrutils \
    libqcrilNrLogger \

LOCAL_MODULE := qcril_client
LOCAL_CLANG := true
LOCAL_SANITIZE += integer

include $(BUILD_SHARED_LIBRARY)

###############################
# Build sample app executable #
###############################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    sample.cpp

LOCAL_CFLAGS := $(qcril_cflags)
LOCAL_CFLAGS += $(qcril_cflags)
LOCAL_CPPLAGS += $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

# TODO: Use settingsd headers from the header library
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../qcril-common/interfaces/include \
    $(LOCAL_PATH)/../qcril-common/marshalling/inc \
    $(LOCAL_PATH)/../qcril-common/marshalling/platform/linux/inc \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../qtimutex/include \
#LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/../qcrild/include

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := \
    libqcrilNrLogger-headers \

LOCAL_HEADER_LIBRARIES := \
    qcrilInterfaces-headers \
    libqcrilNrFramework-headers \
	libqcrilNrLogger-headers \

LOCAL_SHARED_LIBRARIES := \
    qcril_client \
    libqcrilNrLogger \
    libqcrilNrFramework \

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE:= qcril_client_sample

include $(BUILD_EXECUTABLE)
