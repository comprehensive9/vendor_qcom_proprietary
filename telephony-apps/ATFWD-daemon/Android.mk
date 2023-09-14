ifneq ($(TARGET_NO_TELEPHONY), true)
ifneq ($(TARGET_HAS_LOW_RAM),true)

LOCAL_PATH:= $(call my-dir)

# seccomp filter for atfwd daemon
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), arm64))
include $(CLEAR_VARS)
LOCAL_MODULE := atfwd@2.0.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/seccomp_policy
LOCAL_SRC_FILES := seccomp_policy/atfwd@2.0-$(TARGET_ARCH).policy
include $(BUILD_PREBUILT)
endif

ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), arm))
include $(CLEAR_VARS)
LOCAL_MODULE := atfwd@2.0.policy
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/seccomp_policy
LOCAL_SRC_FILES := seccomp_policy/atfwd@2.0-$(TARGET_ARCH).policy
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wall -Werror
ifeq ($(call is-platform-sdk-version-at-least,18),true)
 # JB MR2 or later
 LOCAL_CFLAGS += -DANDROID_JB_MR2=1
endif

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)

LOCAL_SRC_FILES := \
        atfwd_daemon.c \
        sendcmd.cpp \
        SetupMinijail.cpp

LOCAL_SHARED_LIBRARIES += libcutils libutils libc libqmi_cci libqmi libmdmdetect libqmi_client_qmux libqmi_client_helper libqmiservices liblog libavservices_minijail libqmi_common_so
LOCAL_SHARED_LIBRARIES += libbase libhidlbase vendor.qti.hardware.radio.atcmdfwd@1.0
LOCAL_HEADER_LIBRARIES += libqmi_cci_headers libril-qc-qmi-services-headers libqmi_legacy_headers vendor_common_inc

LOCAL_CPPFLAGS += -std=c++17

LOCAL_MODULE:= ATFWD-daemon

LOCAL_CLANG:=true

LOCAL_MODULE_TAGS := optional

LDLIBS += -lpthread

LOCAL_MODULE_OWNER := qcom

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SANITIZE := integer_overflow

ifeq ($(ATEL_ENABLE_LLVM_SA),true)
    LLVM_SA_OUTPUT_DIR := $(PRODUCT_OUT)/atel-llvm-sa-results/$(LOCAL_MODULE)
    LLVM_SA_FLAG := --compile-and-analyze $(LLVM_SA_OUTPUT_DIR)
    LOCAL_CFLAGS += $(LLVM_SA_FLAG)
    LOCAL_CPPFLAGS += $(LLVM_SA_FLAG)
endif

LOCAL_REQUIRED_MODULES_arm64 := atfwd@2.0.policy
LOCAL_REQUIRED_MODULES_arm := atfwd@2.0.policy

include $(BUILD_EXECUTABLE)

endif
endif #TARGET_NO_TELEPHONY
