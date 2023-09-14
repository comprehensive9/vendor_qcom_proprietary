# ---------------------------------------------------------------------------------
#                       Make the dummy library
# ---------------------------------------------------------------------------------

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE        := libTestCase_14
LOCAL_MODULE_TAGS   := optional
LOCAL_CFLAGS        := -Werror -Wno-undefined-bool-conversion -Wno-format
LOCAL_CLANG         := true

LOCAL_SRC_FILES     += test_case.cpp main.cpp

LOCAL_EXPORT_C_INCLUDES := test_case.h
LOCAL_ABI_CHECKER := true

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
