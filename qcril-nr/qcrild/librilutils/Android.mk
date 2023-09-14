# Copyright 2013 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    librilutils.c \
    record_stream.c \

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qcril-common/interfaces/include

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

LOCAL_MODULE:= qcrild_libqcrilnrutils
sapapi_generated_dir := $(call generated-sources-dir-for,SHARED_LIBRARIES,$(LOCAL_MODULE),,)

LOCAL_VENDOR_MODULE:= true
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES += qcrilInterfaces-headers

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := qcrild_libqcrilnrutils-headers
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_CFLAGS += $(qcril_cflags)
LOCAL_CPPLAGS += $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)
ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

include $(BUILD_HEADER_LIBRARY)


# Create static library for those that want it
# =========================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    librilutils.c \
    record_stream.c \

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qcril-common/interfaces/include

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif

LOCAL_MODULE:= qcrild_libqcrilnrutils_static

sapapi_generated_dir := $(call generated-sources-dir-for,SHARED_LIBRARIES,$(LOCAL_MODULE),,)

LOCAL_VENDOR_MODULE:= true
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_HEADER_LIBRARIES += qcrilInterfaces-headers

include $(BUILD_STATIC_LIBRARY)


