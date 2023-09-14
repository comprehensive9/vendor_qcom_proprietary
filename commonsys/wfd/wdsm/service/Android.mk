LOCAL_PATH := $(call my-dir)

#=============================================
#  Wfd Service APK
#=============================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PRIVATE_PLATFORM_APIS := true

LOCAL_SRC_FILES := $(call all-java-files-under, src/com/qualcomm/wfd) \
                src/com/qualcomm/compat/VersionedInputManager.java \
                src/com/qualcomm/wfd/service/ISessionManagerService.aidl \
                src/com/qualcomm/wfd/service/IWfdActionListener.aidl \
                src/com/qualcomm/wfd/service/IHIDEventListener.aidl \
				src/com/qualcomm/wfd/service/IWfdSession.aidl

ifeq ($(call is-android-codename,ICECREAM_SANDWICH),true)
LOCAL_SRC_FILES += src/com/qualcomm/compat/IcsInputManager.java
else
LOCAL_SRC_FILES += src/com/qualcomm/compat/JbInputManager.java
endif

LOCAL_AIDL_INCLUDES += frameworks/native/aidl/gui

LOCAL_PACKAGE_NAME := WfdService

LOCAL_CERTIFICATE := platform

# Usage ex: mmm wdsm/service/ WFD_DISABLE_JACK=1
$(info JACK DISABLED in WFD Service = $(WFD_DISABLE_JACK))
ifeq ($(WFD_DISABLE_JACK),1)
LOCAL_JACK_ENABLED := disabled
endif

LOCAL_PRIVILEGED_MODULE := true

LOCAL_JNI_SHARED_LIBRARIES := libwfdnative
LOCAL_SYSTEM_EXT_MODULE := true
include $(BUILD_PACKAGE)

#==================================================
# Install priv-app permisison file
#==================================================
include $(CLEAR_VARS)

LOCAL_MODULE := wfd-system-ext-privapp-permissions-qti.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SYSTEM_EXT_MODULE := true

LOCAL_MODULE_PATH := $(TARGET_OUT_SYSTEM_EXT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))
