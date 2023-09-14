# =============================================================================
#
# Module: Secure Touch Driver Interface
#
# =============================================================================

LOCAL_PATH          := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE        := libTrustedInputTZ

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES   := $(LOCAL_PATH)/include \
                      $(TARGET_OUT_HEADERS)/common/inc \

LOCAL_HEADER_LIBRARIES := libutils_headers

LOCAL_CPPFLAGS        := $(ST_TARGET_CFLAGS) -fdiagnostics-show-option
LOCAL_SANITIZE := cfi integer_overflow

LOCAL_SRC_FILES     := src/TrustedInput.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libhidlbase \
                          libbinder \
                          libutils \
                          libcutils \
                          libbase \
                          vendor.qti.hardware.trustedui@1.0 \
                          vendor.qti.hardware.trustedui@1.1 \
                          vendor.qti.hardware.trustedui@1.2

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
