ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
WFD_DISABLE_PLATFORM_LIST := msm8610 mpq8092 msm_bronze msm8909 msm8952

ifeq ($(call is-product-in-list, qssi qssi_32),true)
ifneq ($(call is-board-platform-in-list,$(WFD_DISABLE_PLATFORM_LIST)),true)
ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libwfd_headers
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/./utils/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/./wfd_headers
LOCAL_SYSTEM_EXT_MODULE := true
include $(BUILD_HEADER_LIBRARY)

include $(call all-makefiles-under, $(LOCAL_PATH))
endif
endif
endif # TARGET_FWK_SUPPORTS_FULL_VALUEADDS, AUTO TARGETS
endif # TARGET_USES_WFD
endif # compile only incase of qssi build
