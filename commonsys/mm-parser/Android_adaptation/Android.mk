BUILD_PARSER :=

ifeq ($(TARGET_FWK_SUPPORTS_AV_VALUEADDS),true)
BUILD_PARSER := true
endif

ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
BUILD_PARSER := true
endif

ifeq ($(BUILD_PARSER),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_FLAGS := -Wall -Werror -D_ANDROID_

LOCAL_SRC_FILES:=                 \
    src/MMParserExtractor.cpp     \
    src/QComExtractorFactory.cpp  \
    src/SourcePort.cpp            \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc

LOCAL_HEADER_LIBRARIES :=       \
    libmmosal_headers           \
    libmmparser_headers         \
    libstagefright_headers      \
    vendor_common_inc

LOCAL_SHARED_LIBRARIES +=       \
    libcutils                   \
    liblog                      \
    libmediandk                 \
    libmmparser_lite            \
    libstagefright_foundation   \
    libbinder_ndk               \
    libutils

ifeq ($(PLATFORM_VERSION), 12)
LOCAL_STATIC_LIBRARIES += libstagefright_foundation_colorutils_ndk
endif

LOCAL_SANITIZE := cfi signed-integer-overflow unsigned-integer-overflow

LOCAL_MODULE:= libmmparserextractor

LOCAL_MODULE_RELATIVE_PATH := extractors

LOCAL_SYSTEM_EXT_MODULE := true

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
endif
