ifeq ($(call is-board-platform-in-list, lahaina),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(TARGET_OUT_HEADERS)/diag/include \
                    $(LOCAL_PATH)/../QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libdiag \
        libQSEEComAPI \

LOCAL_MODULE := diagcommd

LOCAL_SRC_FILES := diagcommd.c
LOCAL_SRC_FILES += diag_dispatch.c
LOCAL_INIT_RC := diagcommd.rc
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(DIAG_CFLAGS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

endif # end filter

