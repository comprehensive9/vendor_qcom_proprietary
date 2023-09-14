LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.esepowermanager@1.0-impl
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    EsePowerManager.cpp

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc


LOCAL_SHARED_LIBRARIES := \
    libbase \
    libcutils \
    libhidlbase \
    liblog \
    libutils \
    vendor.qti.esepowermanager@1.0

LOCAL_SANITIZE := cfi integer_overflow
#Enable below line during debug
#LOCAL_SANITIZE_DIAG := cfi integer_overflow

include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_VENDOR_MODULE := true

LOCAL_MODULE := vendor.qti.esepowermanager@1.0-service
LOCAL_INIT_RC := vendor.qti.esepowermanager@1.0-service.rc
LOCAL_SRC_FILES := \
        service.cpp \

LOCAL_HEADER_LIBRARIES := QSEEComAPI_header

LOCAL_SHARED_LIBRARIES := \
       libcutils \
       libdl \
       libbase \
       libutils \
       libhardware_legacy \
       libhardware \
       libQSEEComAPI \
       liblog \
       libhidlbase \
       vendor.qti.esepowermanager@1.0

LOCAL_SANITIZE := cfi integer_overflow
#Enable below line during debug
#LOCAL_SANITIZE_DIAG := cfi integer_overflow

include $(BUILD_EXECUTABLE)
