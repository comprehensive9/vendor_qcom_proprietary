RCS_DIR := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
include $(RCS_DIR)/java/PresenceApp/Android.mk
include $(RCS_DIR)/java/ConnectionManagerTestApp/Android.mk
endif
include $(RCS_DIR)/java/uceShimService/Android.mk

#include $(call first-makefiles-under, $(RCS_DIR))
