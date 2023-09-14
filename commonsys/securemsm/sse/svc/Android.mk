LOCAL_PATH:= $(call my-dir)

# ---------------------------------------------------------------------------------
#                 Socket communication library
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

libsvcsock-def := -g -O3
libsvcsock-def += -D_ANDROID_
libsvcsock-def += -D__LINUX__
libsvcsock-def += -DVERBOSE
libsvcsock-def += -D_DEBUG

commonIncludes := $(LOCAL_PATH)/ \
                  $(TARGET_OUT_HEADERS)/common/inc

commonSharedLibraries := \
	libutils \
	libcutils \
	liblog \
	libhidlbase \
	libutils \
	vendor.qti.hardware.tui_comm@1.0

svc_sourceFiles := \
	src/svc_sock.cpp \

LOCAL_CFLAGS := $(libsvcsock-def)
LOCAL_SANITIZE := cfi integer_overflow
LOCAL_SRC_FILES :=  $(svc_sourceFiles)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_SYSTEM_EXT_MODULE := true
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libsecureui_svcsock_system
LOCAL_SHARED_LIBRARIES := $(commonSharedLibraries)

LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)
