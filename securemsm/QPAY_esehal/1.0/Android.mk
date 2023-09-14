ifeq ($(strip $(TARGET_USES_QPAY_ESE)),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SMSM_PATH           := $(QC_PROP_ROOT)/securemsm

COMMON_INCLUDES     :=  $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                        $(TARGET_OUT_HEADERS)/common/inc \
                        $(SMSM_PATH)/QSEEComAPI \
                        $(SMSM_PATH)/QTEEConnector/include \
                        $(SMSM_PATH)/QPAY_esehal \

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.secure_element@1.0-service
LOCAL_INIT_RC := vendor.qti.secure_element@1.0-service.rc
LOCAL_C_INCLUDES := $(COMMON_INCLUDES)
LOCAL_SRC_FILES := \
    service.cpp \
    SecureElement.cpp \
    SecureElementHalCallback.cpp \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libhardware \

LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    android.hardware.secure_element@1.0 \
    android.hardware.secure_element@1.0-impl

include $(BUILD_EXECUTABLE)
endif
