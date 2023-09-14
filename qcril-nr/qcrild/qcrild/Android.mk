# Copyright 2006 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)

# seccomp filter for qcril-nr
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), arm64))
include $(CLEAR_VARS)
LOCAL_MODULE := qcrilnr@2.0.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/seccomp_policy
LOCAL_SRC_FILES := seccomp_policy/qcrilnr@2.0-$(TARGET_ARCH).policy
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        rild.c \
        SetupMinijail.cpp

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libdl \
        liblog \
        libqcrilNr \
        qtiwakelock \
        libutils \
        libavservices_minijail

LOCAL_HEADER_LIBRARIES := qtiwakelock-headers

LOCAL_CFLAGS := -DRIL_SHLIB $(qcril_cflags)
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

ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_CFLAGS               += -DFEATURE_QCRIL_DEVICE_INFO_SERVICE
endif

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/

ifeq ($(ENABLE_AFL), true)
# TODO: Use header library
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../test/afl
LOCAL_CFLAGS += -DQCRIL_ENABLE_AFL
LOCAL_SHARED_LIBRARIES += \
        libqcrilNrFramework \
        qcril_client
LOCAL_WHOLE_STATIC_LIBRARIES += qcril_afl
endif

ifeq ($(SIM_COUNT), 2)
    LOCAL_CFLAGS += -DANDROID_MULTI_SIM
    LOCAL_CFLAGS += -DANDROID_SIM_COUNT_2
endif

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE:= qcrilNrd
LOCAL_INIT_RC := qcrilNrd.rc

LOCAL_REQUIRED_MODULES_arm64 := qcrilnr@2.0.policy
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), arm64))
LOCAL_CFLAGS += -DFEATURE_QCRIL_MINIJAIL
endif

include $(BUILD_EXECUTABLE)

