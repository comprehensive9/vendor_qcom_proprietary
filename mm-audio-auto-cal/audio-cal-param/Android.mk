LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    src/audcalparam_api.c \
    src/audcalparam_serv_con.c

LOCAL_COPY_HEADERS := \
    inc/audcalparam_api.h \
    inc/audcalparam_util.h

libaudcalparam-def := -g -O3
libaudcalparam-def += -D_ANDROID_
libaudcalparam-def += -D AUDIO_USE_SYSTEM_HEAP_ID
libaudcalparam-def += -D USE_LIBACDBLOADER
#libaudcalparam-def += -D AUDCALPARAM_DBG
#libaudcalparam-def += -D AUDCALPARAM_BUF_DBG
LOCAL_CFLAGS       = $(libaudcalparam-def)

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libdl \
    libjson

LOCAL_C_INCLUDES        = vendor/qcom/proprietary/mm-audio-auto-cal/audio-cal-param/util
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/libjson/inc/
LOCAL_C_INCLUDES        += $(TARGET_OUT_HEADERS)/mm-audio/audio-acdb-util
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES        += $(TARGET_OUT_INTERMEDIATES)/vendor/qcom/opensource/audio-kernel/include

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= libaudcalparam
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

