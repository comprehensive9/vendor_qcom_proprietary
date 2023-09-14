ifneq ($(TARGET_BOARD_AUTO),true)
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
ifeq ($(TARGET_BUILD_JAVA_SUPPORT_LEVEL),platform)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

res_dir := res $(LOCAL_PATH)/res

LOCAL_MODULE_TAGS := optional
#LOCAL_SDK_VERSION := current

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dir))
LOCAL_USE_AAPT2 := true
LOCAL_JAVA_LIBRARIES := com.qti.media.secureprocessor \
                        com.google.android.material_material \

LOCAL_STATIC_ANDROID_LIBRARIES := \
    androidx.core_core \
    androidx.appcompat_appcompat \
    androidx.fragment_fragment

LOCAL_DEX_PREOPT := false
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
#LOCAL_PRODUCT_MODULE := true
LOCAL_PACKAGE_NAME := Cam2test
LOCAL_PRIVATE_PLATFORM_APIS := true

include $(BUILD_PACKAGE)

endif
endif
endif
