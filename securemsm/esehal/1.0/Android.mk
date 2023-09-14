LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.secure_element@1.0-service
LOCAL_INIT_RC := vendor.qti.secure_element@1.0-service.rc
LOCAL_C_INCLUDES := $(SMSM_PATH)/GPTEE/inc \
                    $(SMSM_PATH)/esepowermanager/1.1 \
                    $(SMSM_PATH)/esehal \
                    $(SMSM_PATH)/esehal/ese-clients/inc \

LOCAL_SRC_FILES := \
    service.cpp \
    interface.cpp \
    SecureElement.cpp \
    SecureElementHalCallback.cpp

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_HEADER_LIBRARIES += libese_client_headers
LOCAL_CFLAGS += -DESE_CLIENT_LIB_PRESENT=true
LOCAL_SRC_FILES += \
    ../ese-clients/src/eSEClient.cpp
else
LOCAL_CFLAGS += -DESE_CLIENT_LIB_PRESENT=false
endif

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libhardware \

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_SHARED_LIBRARIES += \
    se_nq_extn_client \
    ls_nq_client \
    jcos_nq_client
endif
LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    android.hardware.secure_element@1.0 \
    vendor.qti.esepowermanager@1.0 \
    vendor.qti.esepowermanager@1.1 \
    android.hardware.secure_element@1.0-impl

LOCAL_SANITIZE := cfi integer_overflow
#Enable below line during debug
#LOCAL_SANITIZE_DIAG := cfi integer_overflow

include $(BUILD_EXECUTABLE)
