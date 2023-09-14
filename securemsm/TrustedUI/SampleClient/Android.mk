ifeq ($(ENABLE_TRUSTED_UI_2_0), true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

SECUREMSM_SHIP_PATH   := $(TOP)/$(QC_PROP_ROOT)/securemsm

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_C_INCLUDES := $(TARGET_OUT_HEADERS)/common/inc \
                    $(TOP)/hardware/qcom/display/libqdutils \
                    $(TOP)/hardware/libhardware/include \
                    $(SECUREMSM_SHIP_PATH)/sse/SecureIndicator/inc

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        liblog \
        libdisplayconfig.qti \
        libqdutils

LOCAL_CFLAGS := -O3

ifeq ($(ENABLE_TRUSTED_UI_VM_3_0), true)
LOCAL_CFLAGS += -DENABLE_TRUSTED_UI_VM_3_0
endif


LOCAL_SHARED_LIBRARIES += libhidlbase libhidlmemory libsi \
        libbase vendor.display.config@1.3 \
        vendor.qti.hardware.trustedui@1.0 \
        vendor.qti.hardware.trustedui@1.1 \
        vendor.qti.hardware.trustedui@1.2 \
        android.hidl.allocator@1.0

LOCAL_HEADER_LIBRARIES := display_intf_headers
LOCAL_STATIC_LIBRARIES += libgtest

LOCAL_MODULE := TrustedUISampleTest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := TrustedUISampleClient.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_SANITIZE := cfi integer_overflow
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)
endif
