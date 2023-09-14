ifneq ($(TARGET_BUILD_VARIANT), user)
LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH = $(LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE := wifimyftm

LOCAL_SRC_FILES:= \
    wifimyftm.cpp \
    service.cpp

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += \
      libbase \
      libhidlbase \
      liblog \
      libutils \
      libnl \
      libc \
      libhidlmemory \
      android.hidl.memory@1.0 \
      android.hidl.allocator@1.0 \
      vendor.qti.hardware.wifi.wifimyftm@1.0

LOCAL_MODULE_OWNER := qti
LOCAL_SANITIZE := integer_overflow
LOCAL_INIT_RC := vendor.qti.hardware.wifi.wifimyftm@1.0-service.rc
LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.wifi.wifimyftm@1.0-service.xml

include $(BUILD_EXECUTABLE)

endif
