BASE_PATH := $(call my-dir)

ifneq ($(BUILD_QEMU_IMAGES), true)
include $(BASE_PATH)/common/Android.mk
-include $(BASE_PATH)/$(TARGET_BOARD_PLATFORM)/Android.mk
endif
