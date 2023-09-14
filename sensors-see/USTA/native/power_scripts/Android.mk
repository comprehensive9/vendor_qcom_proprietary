LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#===============================================================
$(shell mkdir -p $(TARGET_OUT_VENDOR_ETC)/sensors/proto)

$(shell cp -f $(LOCAL_PATH)/../../../ssc/google/protobuf/descriptor.proto $(TARGET_OUT_VENDOR_ETC)/sensors)
$(shell cp -f $(LOCAL_PATH)/../../../ssc/*.proto $(TARGET_OUT_VENDOR_ETC)/sensors)
$(shell mv -f $(TARGET_OUT_VENDOR_ETC)/sensors/*.proto $(TARGET_OUT_VENDOR_ETC)/sensors/proto)
#===============================================================
# List of power scripts
sns_power_scripts_files := $(wildcard $(LOCAL_PATH)/*.txt)
$(shell mkdir -p $(TARGET_OUT_VENDOR_ETC)/sensors/scripts)
$(info "Adding power scripts files  = $(sns_power_scripts_files)")
define _add_power_files
 include $$(CLEAR_VARS)
 LOCAL_MODULE := $(notdir $(1))
 LOCAL_MODULE_STEM := $(notdir $(1))
 sns_power_scripts_modules += $$(LOCAL_MODULE)
 LOCAL_SRC_FILES := $(1:$(LOCAL_PATH)/%=%)
 LOCAL_MODULE_TAGS := optional
 LOCAL_MODULE_CLASS := ETC
 LOCAL_MODULE_PATH := $$(TARGET_OUT_VENDOR_ETC)/sensors/scripts
include $$(BUILD_PREBUILT)
endef

sns_power_scripts_modules :=
sns_config :=
$(foreach _conf-file, $(sns_power_scripts_files), $(eval $(call _add_power_files,$(_conf-file))))
