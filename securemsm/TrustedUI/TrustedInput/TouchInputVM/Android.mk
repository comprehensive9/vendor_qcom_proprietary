# =============================================================================
#
# Module: Secure TouchInputVM Android Driver Interface
#
# =============================================================================

LOCAL_PATH          := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := libTouchInputVM

LOCAL_MODULE_TAGS   := optional

LOCAL_C_INCLUDES   := $(LOCAL_PATH)/inc \
                      $(TARGET_OUT_HEADERS)/common/inc \

LOCAL_CPPFLAGS      := $(ST_TARGET_CFLAGS) -fdiagnostics-show-option

LOCAL_SRC_FILES     := src/TouchInputVM.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libhidlbase \
                          libutils \
                          vendor.qti.hardware.trustedui@1.0 \
                          vendor.qti.hardware.trustedui@1.1 \
                          vendor.qti.hardware.trustedui@1.2

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
