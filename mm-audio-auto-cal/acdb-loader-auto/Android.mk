ifneq ($(BUILD_TINY_ANDROID),true)

ifeq ($(strip $(AUDIO_FEATURE_ENABLED_DAEMON_SUPPORT)),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                 Common definitons
# ---------------------------------------------------------------------------------

libacdbloaderclient-def := -g -O3
libacdbloaderclient-def += -D_ANDROID_
libacdbloaderclient-def += -D_ENABLE_QC_MSG_LOG_
libacdbloaderclient-def += -D DEFAULT_BOARD=\"MTP\"
libacdbloaderclient-def += -D_TINY_ALSA_LIB_

ifeq ($(call is-board-platform-in-list,msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm670 qcs605 sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
libacdbloaderclient-def += -D AUDIO_USE_SYSTEM_HEAP_ID
endif
# ---------------------------------------------------------------------------------
#             Make the Shared library (libacdbloaderclient)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

libacdbloaderclient-inc     := $(LOCAL_PATH)/inc
libacdbloaderclient-inc     += $(LOCAL_PATH)/src

LOCAL_MODULE            := libacdbloaderclient
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libacdbloaderclient-def)
LOCAL_C_INCLUDES        := $(libacdbloaderclient-inc)
ifeq ($(call is-board-platform-in-list,sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_C_INCLUDES        += system/core/libion/include
LOCAL_C_INCLUDES        += system/core/libion/kernel-headers
endif
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audcal
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audio-acdb-util
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES 	    += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include

LOCAL_C_INCLUDES        += external/tinyalsa/include

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifeq ($(strip $(AUDIO_FEATURE_ENABLED_DLKM)),true)
  LOCAL_HEADER_LIBRARIES := audio_kernel_headers
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
endif

LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libcutils libutils liblog libaudcal libtinyalsa
LOCAL_COPY_HEADERS_TO   := mm-audio/audio-acdb-util

ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm670 qcs605 sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_SHARED_LIBRARIES  += libacdbrtac
LOCAL_SHARED_LIBRARIES  += libadiertac
LOCAL_SHARED_LIBRARIES  += libacdb-fts
ifeq ($(call is-board-platform-in-list,sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_SHARED_LIBRARIES  += libion
LOCAL_SHARED_LIBRARIES  += libbinder
endif
endif

ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm670 qcs605 sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_SRC_FILES         := src/acdb-loader-wrapper.cpp \
				src/acdb-loader-proxy.cpp
endif

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(call is-board-platform-in-list,kona),true)
LOCAL_SANITIZE := integer_overflow
endif
include $(BUILD_SHARED_LIBRARY)

# ---------------------------------------------------------------------------------
#             Make the acdb_loader_service binary
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

acdb_loader_service-def := -g -O3
acdb_loader_service-def += -D_ANDROID_
acdb_loader_service-def += -D_ENABLE_QC_MSG_LOG_
acdb_loader_service-def += -D DEFAULT_BOARD=\"MTP\"
acdb_loader_service-def += -D_TINY_ALSA_LIB_

acdb_loader_service-inc     := $(LOCAL_PATH)/inc
acdb_loader_service-inc     += $(LOCAL_PATH)/src

LOCAL_MODULE            := acdb_loader_service
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(acdb_loader_service-def)
LOCAL_C_INCLUDES        := $(acdb_loader_service-inc)
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audcal
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audio-acdb-util

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifeq ($(strip $(AUDIO_FEATURE_ENABLED_DLKM)),true)
  LOCAL_HEADER_LIBRARIES := audio_kernel_headers
  LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include
endif

ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm670 qcs605 sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_SRC_FILES         := src/acdb-loader-service.cpp \
				src/acdb-loader-proxy.cpp \
				src/acdb-loader-server.cpp
endif

LOCAL_SHARED_LIBRARIES  := libcutils libutils liblog libaudcal libtinyalsa
ifeq ($(call is-board-platform-in-list,msm8909 msm8996 msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm670 qcs605 sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_SHARED_LIBRARIES  += libacdbrtac
LOCAL_SHARED_LIBRARIES  += libadiertac
LOCAL_SHARED_LIBRARIES  += libacdb-fts
ifeq ($(call is-board-platform-in-list,sdmshrike msmnile $(MSMSTEPPE) $(TRINKET) kona lito atoll bengal),true)
LOCAL_SHARED_LIBRARIES  += libion
LOCAL_SHARED_LIBRARIES  += libbinder
endif
endif

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/bin

ifeq ($(call is-board-platform-in-list,kona),true)
LOCAL_SANITIZE := integer_overflow
endif

include $(BUILD_EXECUTABLE)

endif

endif


# ---------------------------------------------------------------------------------
#                     END
# ---------------------------------------------------------------------------------
