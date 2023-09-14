LOCAL_PATH:= $(call my-dir)

# WifiResTarget for lahaina, which surppots DBS features

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_MANIFEST_FILE := AndroidManifest.xml

LOCAL_PACKAGE_NAME := WifiResTarget
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResTarget-spf for common SPF features. ex: lahaina, kodiak and cedros
# This is primarily for non-DBS targets.

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/spf/res

LOCAL_MANIFEST_FILE := AndroidManifest-spf.xml

LOCAL_PACKAGE_NAME := WifiResTarget_spf
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResTarget-yupik for kodiak + HSP target

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/yupik-iot/res

LOCAL_MANIFEST_FILE := AndroidManifest-yupik-iot.xml

LOCAL_PACKAGE_NAME := WifiResTarget_yupik_iot
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)
