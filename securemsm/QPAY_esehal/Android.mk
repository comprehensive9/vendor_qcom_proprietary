ifeq ($(strip $(TARGET_USES_QPAY_ESE)),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SMSM_PATH           := $(QC_PROP_ROOT)/securemsm

COMMON_INCLUDES     :=  $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                        $(TARGET_OUT_HEADERS)/common/inc \
                        $(SMSM_PATH)/esepowermanager/1.0 \
                        $(SMSM_PATH)/esepowermanager/1.1 \
                        $(SMSM_PATH)/QSEEComAPI \
                        $(SMSM_PATH)/QTEEConnector/include \

LOCAL_MODULE := android.hardware.secure_element@1.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := \
    QPayAdapter.cpp \
    QPayAdapter-utils.cpp \
    esepm.cpp \

LOCAL_C_INCLUDES := $(COMMON_INCLUDES)

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libutils \
    liblog \
    libhardware \
    libbase \
    libcutils \
    libQSEEComAPI \
    libQTEEConnector_vendor \
    vendor.qti.hardware.qteeconnector@1.0 \
    android.hardware.secure_element@1.0 \
    vendor.qti.esepowermanager@1.0 \
    vendor.qti.esepowermanager@1.1 \

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
