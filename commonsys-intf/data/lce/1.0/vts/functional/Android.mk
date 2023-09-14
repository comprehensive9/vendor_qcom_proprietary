LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE        := VtsHalDataLceV1_0_IFactoryV2_3TargetTest
LOCAL_MODULE_TAGS   := optional
LOCAL_CFLAGS        := -O0 -g

LOCAL_SRC_FILES     += VtsHalDataLceV1_0_IFactoryV2_3TargetTest.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libhidlbase \
                          libutils \
                          vendor.qti.data.factory@2.3 \
                          vendor.qti.hardware.data.lce@1.0

LOCAL_STATIC_LIBRARIES := VtsHalHidlTargetTestBase

LOCAL_MODULE_OWNER := qti

include $(BUILD_NATIVE_TEST)
