USE_SENSOR_MULTI_HAL := true

ifeq ($(TARGET_BOARD_PLATFORM),kona)
QMAA_ENABLED_HAL_MODULES += sensors
endif

SNS_SUPPORT_DIRECT_CHANNEL_CLIENT_API :=  false

$(call add_soong_config_namespace,snsconfig)
ifeq ($(GENERIC_ODM_IMAGE)$(TARGET_SUPPORTS_WEARABLES),true)
$(call add_soong_config_var_value,snsconfig,imagetype,generic_odm_image)
else
$(call add_soong_config_var_value,snsconfig,imagetype,odm_image)
endif
ifeq ($(call is-board-platform-in-list,sdm845 sdm710), true)
$(call add_soong_config_var_value,snsconfig,isDefaultHeap,true)
endif

ifeq ($(SNS_SUPPORT_DIRECT_CHANNEL_CLIENT_API) , true)
$(call add_soong_config_var_value,snsconfig,isDirectChannel,enabled)
else
$(call add_soong_config_var_value,snsconfig,isDirectChannel,disabled)
endif

ifeq ($(call is-board-platform-in-list,msm8937), true)
$(call add_soong_config_var_value,snsconfig,isWearableTarget,true)
endif

ifeq ($(TARGET_SUPPORTS_WEARABLES), true)
ifeq ($(TARGET_SUPPORTS_WEAR_OS), true)
$(call add_soong_config_var_value,snsconfig,isWearableTargetBG,true)
endif
endif