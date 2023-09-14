LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libdpp_manager
LOCAL_MODULE_SUFFIX:= .so
LOCAL_MODULE_TAGS := optional
LOCAL_CPP_EXTENSION := .cpp

LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_CLANG := true
LOCAL_CFLAGS += -MMD -O2 -Wall -g
LOCAL_CFLAGS += -Werror
LOCAL_CXXFLAGS += -std=c++14 -O2 -g

LOCAL_SRC_FILES :=
LOCAL_SRC_FILES += dppCommonUtils.cpp
LOCAL_SRC_FILES += dppManager.cpp
LOCAL_SRC_FILES += dppEnrollee.cpp
LOCAL_SRC_FILES += dppWpaCtrlIface.cpp
LOCAL_SRC_FILES += xmlUtil.cpp
LOCAL_SRC_FILES += wpa_ctrl_utils.cpp
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SANITIZE := integer_overflow
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
# DPP_DAEMON_GIT_VER := $(shell cd $(LOCAL_PATH)/ && git describe --always)
# LOCAL_CFLAGS += -DDPP_DAEMON_VER=\"$(DPP_DAEMON_GIT_VER)\"
LOCAL_CFLAGS += -MMD -O2 -Wall -g
LOCAL_CFLAGS += -Werror
LOCAL_CXXFLAGS += -std=c++14 -O2 -g

LOCAL_MODULE := dppdaemon
LOCAL_SHARED_LIBRARIES := libdpp_manager
LOCAL_SRC_FILES += dppdaemon.cpp
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SANITIZE := integer_overflow
include $(BUILD_EXECUTABLE)
