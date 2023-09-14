ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list, kona),true)

LOCAL_PATH:= $(call my-dir)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

SECUREMSM_SHIP_PATH := $(QC_PROP_ROOT)/securemsm
SECUREMSM_SHIP_COMMONSYS_PATH := $(QC_PROP_ROOT)/commonsys/securemsm
include $(CLEAR_VARS)

include $(LIBION_HEADER_PATH_WRAPPER)
LOCAL_C_INCLUDES := $(LIBION_HEADER_PATHS)

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(SECUREMSM_SHIP_COMMONSYS_PATH)/QSEEComAPI \
                    $(SECUREMSM_SHIP_PATH)/sp_license_sample \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(SECUREMSM_SHIP_PATH)/sse/SecureUILib \


LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := \
        libc \
        libcutils \
        libutils \
        liblog \
        libQSEEComAPI_system \
        libdl \
        libion

LOCAL_MODULE := sp_license_sample
LOCAL_SRC_FILES := sp_license_sample.c
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

endif
endif
