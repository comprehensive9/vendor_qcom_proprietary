LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE        := VtsCallCapabilityV1_0Test
LOCAL_MODULE_TAGS   := optional
LOCAL_CFLAGS        := -O0 -g

LOCAL_SRC_FILES     += VtsCallCapabilityV1_0Test.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libbase \
                          libhidlbase \
                          libutils \
                          libz \
                          vendor.qti.ims.factory@1.0 \
                          vendor.qti.ims.callcapability@1.0 \

LOCAL_STATIC_LIBRARIES := VtsHalHidlTargetTestBase
LOCAL_MODULE_OWNER := qti
include $(BUILD_NATIVE_TEST)
