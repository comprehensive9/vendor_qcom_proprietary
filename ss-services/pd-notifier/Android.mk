PD_NOTIFIER_ROOT := $(call my-dir)
LOCAL_PATH := $(PD_NOTIFIER_ROOT)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := libpdnotifier
LOCAL_C_INCLUDES += $(LOCAL_PATH) \
		    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
		    $(PD_NOTIFIER_ROOT)/pd-notifier-idl
LOCAL_SRC_FILES := libpdnotifier/libpdnotifier.c \
	pd-notifier-idl/service_registry_notifier_v01.c
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif
LOCAL_HEADER_LIBRARIES := vendor_common_inc\
                          libqmi_common_headers
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libpdnotifier \
			$(LOCAL_PATH)/pd-notifier-idl
LOCAL_COPY_HEADERS_TO := libpdnotifier/inc
LOCAL_COPY_HEADERS := libpdnotifier/libpdnotifier.h
LOCAL_COPY_HEADERS += pd-notifier-idl/service_registry_notifier_v01.h
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
ifeq ($(TARGET_PD_SERVICE_ENABLED), true)
	LOCAL_CFLAGS += -DLIBPDNOTIFIER_ENABLED
endif
include $(BUILD_SHARED_LIBRARY)
