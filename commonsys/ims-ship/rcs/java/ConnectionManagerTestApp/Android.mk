#/******************************************************************************
#*@file Android.mk
#*brief Rules for copiling the source files
#*******************************************************************************/
ifneq ($(TARGET_HAS_LOW_RAM), true)
ifneq ($(TARGET_BUILD_VARIANT),user)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_SRC_FILES := $(call all-subdir-java-files)
#LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data
LOCAL_JAVA_LIBRARIES := com.qualcomm.qti.imscmservice-V2.0-java
LOCAL_JAVA_LIBRARIES += com.qualcomm.qti.imscmservice-V2.1-java
LOCAL_JAVA_LIBRARIES += com.qualcomm.qti.imscmservice-V2.2-java
LOCAL_JAVA_LIBRARIES += android.hidl.base-V1.0-java

LOCAL_STATIC_JAVA_LIBRARIES += androidx.appcompat_appcompat
LOCAL_STATIC_JAVA_LIBRARIES += androidx-constraintlayout_constraintlayout
LOCAL_STATIC_JAVA_LIBRARIES += com.google.android.material_material
LOCAL_STATIC_JAVA_LIBRARIES += androidx.lifecycle_lifecycle-extensions

LOCAL_PACKAGE_NAME := ConnectionManagerTestApp
LOCAL_CERTIFICATE := platform
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_MODULE_OWNER := qti
LOCAL_SYSTEM_EXT_MODULE := true
#LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PACKAGE)
endif
endif
