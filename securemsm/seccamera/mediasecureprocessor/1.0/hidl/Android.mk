LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.hardware.secureprocessor@1.0
LOCAL_INIT_RC := vendor.qti.hardware.secureprocessor@1.0.rc
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := service.cpp \
    utils.cpp \
    SecureProcessorFactory.cpp \
    tee-processor/SecureProcessorTEE.cpp

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc \
                    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/QSEEComAPI \
                    $(LOCAL_PATH)/tee-processor \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libhardware \
    libhidlbase \
    libbinder \
    libQSEEComAPI \
    vendor.qti.hardware.secureprocessor.common@1.0 \
    vendor.qti.hardware.secureprocessor.config@1.0 \
    vendor.qti.hardware.secureprocessor.device@1.0 \
    vendor.qti.hardware.secureprocessor.common@1.0-helper

LOCAL_HEADER_LIBRARIES := \
    display_intf_headers

include $(BUILD_EXECUTABLE)
