ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8996 msm8992 msm8952 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710 qcs605 msm8909 msmnile sdmshrike kona $(MSMSTEPPE) lito atoll bengal monaco $(TRINKET) lahaina holi),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES :=
LOCAL_SHARED_LIBRARIES :=

ifeq ($(call is-board-platform-in-list,msmnile sdmshrike kona $(MSMSTEPPE) lito atoll bengal monaco $(TRINKET) lahaina sdm660 holi),true)
include $(LIBION_HEADER_PATH_WRAPPER)
LOCAL_C_INCLUDES += $(LIBION_HEADER_PATHS)
LOCAL_SHARED_LIBRARIES += libion
endif

ifeq ($(TARGET_KERNEL_VERSION),$(filter $(TARGET_KERNEL_VERSION),4.14 4.19))
    ifeq ($(call is-board-platform-in-list,msm8953 msm8937),true)
        include $(LIBION_HEADER_PATH_WRAPPER)
        LOCAL_C_INCLUDES += $(LIBION_HEADER_PATHS)
        LOCAL_SHARED_LIBRARIES += libion
    endif
endif

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../QSEEComAPI \
                    $(LOCAL_PATH)/../sampleclient \
                    $(TARGET_OUT_HEADERS)/common/inc \


LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES += \
        libc \
        libcutils \
        libutils \
        liblog \
        libQSEEComAPI \
        libdl

LOCAL_MODULE := qseecom_sample_client
LOCAL_SRC_FILES := qseecom_sample_client.c
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

endif # end filter
