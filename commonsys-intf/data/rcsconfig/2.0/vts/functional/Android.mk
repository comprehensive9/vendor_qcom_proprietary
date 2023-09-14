LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE        := VtsImsRcsConfigTest_IImsFactoryV1_0
LOCAL_MODULE_TAGS   := optional
LOCAL_CFLAGS        := -O0 -g

LOCAL_SRC_FILES     += VtsImsRcsConfigTest_IImsFactoryV1_0.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libhidlbase \
                          libutils \
                          libz \
                          vendor.qti.ims.factory@1.0 \
                          vendor.qti.ims.rcsconfig@2.0 \

LOCAL_STATIC_LIBRARIES := VtsHalHidlTargetTestBase

LOCAL_MODULE_OWNER := qti

include $(BUILD_NATIVE_TEST)
