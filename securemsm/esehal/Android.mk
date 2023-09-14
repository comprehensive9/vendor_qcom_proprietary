ifneq ($(ENABLE_HYP),true)
ifeq ($(strip $(TARGET_USES_GPQESE)),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

SMSM_PATH           := $(QC_PROP_ROOT)/securemsm

COMMON_INCLUDES     :=  $(SMSM_PATH)/GPTEE/inc \
                        $(TARGET_OUT_HEADERS)/common/inc \
                        $(SMSM_PATH)/esepowermanager/1.1 \
                        $(LOCAL_PATH)/ese-clients/inc \

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_HEADER_LIBRARIES += libese_client_headers
endif

LOCAL_MODULE := android.hardware.secure_element@1.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := \
    gpqese/gpqese-be.cpp \
    gpqese/gpqese-apdu.cpp \
    gpqese/gpqese-utils.cpp \

LOCAL_C_INCLUDES := $(COMMON_INCLUDES)

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libutils \
    liblog \
    libhardware \
    libbase \
    libcutils \
    android.hardware.secure_element@1.0 \
    libGPTEE_vendor \
    vendor.qti.esepowermanager@1.0 \
    vendor.qti.esepowermanager@1.1 \

LOCAL_SANITIZE := cfi integer_overflow
#Enable below line during debug
#LOCAL_SANITIZE_DIAG := cfi integer_overflow

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
endif
