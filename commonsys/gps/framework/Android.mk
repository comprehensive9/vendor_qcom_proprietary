LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#this tag means this package will be available for all eng builds
#note if the OEM decides to include this permission and group assignment in the final build, the
#tag should be changed to 'user'
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_JAVA_LIBRARIES := telephony-common \
                        services \
                        com.android.location.provider \
                        izat.xt.srv

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v4
LOCAL_STATIC_JAVA_LIBRARIES += izat.lib.glue
LOCAL_STATIC_JAVA_LIBRARIES += izat.lib.hidlclient

LOCAL_PACKAGE_NAME := com.qualcomm.location
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_SYSTEM_EXT_MODULE := true

LOCAL_CERTIFICATE := platform
LOCAL_PRIVATE_PLATFORM_APIS := true

include $(BUILD_PACKAGE)
