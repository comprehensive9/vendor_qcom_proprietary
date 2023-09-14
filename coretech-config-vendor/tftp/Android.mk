LOCAL_PATH:= $(call my-dir)

#########################################################################
# Create Folder Structure
#########################################################################
TARGET_TFTP_FIRMWARE="/vendor/firmware_mnt"
TARGET_TFTP_PERSIST="/mnt/vendor/persist"

#To be enabled when prepopulation support is needed for the read_write folder
# $(shell rm -rf  $(TARGET_OUT_DATA)/rfs/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/msm/mpss/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/msm/adsp/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/msm/slpi/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/msm/cdsp/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/msm/wpss/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/mdm/mpss/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/mdm/adsp/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/mdm/slpi/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/mdm/cdsp/)
# $(shell mkdir -p $(TARGET_OUT_DATA)/rfs/mdm/wpss/)

define symlink-rfs
$(call symlink-file,,$(1),$(2)) \
$(eval RFS_SYMLINKS_DEPS += $(2))
endef

#########################################################################
# MSM Folders
#########################################################################
$(call symlink-rfs,/data/vendor/tombstones/rfs/modem,$(TARGET_OUT_VENDOR)/rfs/msm/mpss/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/msm/mpss,$(TARGET_OUT_VENDOR)/rfs/msm/mpss/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/mpss/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/mpss/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/msm/mpss/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/msm/mpss/readonly/vendor/firmware)
$(call symlink-rfs,/data/vendor/firmware/wlanmdsp.mbn,$(TARGET_OUT_VENDOR)/firmware/wlanmdsp.otaupdate)

$(call symlink-rfs,/data/vendor/tombstones/rfs/lpass,$(TARGET_OUT_VENDOR)/rfs/msm/adsp/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/msm/adsp,$(TARGET_OUT_VENDOR)/rfs/msm/adsp/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/adsp/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/adsp/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/msm/adsp/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/msm/adsp/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/slpi,$(TARGET_OUT_VENDOR)/rfs/msm/slpi/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/msm/slpi,$(TARGET_OUT_VENDOR)/rfs/msm/slpi/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/slpi/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/slpi/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/msm/slpi/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/msm/slpi/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/cdsp,$(TARGET_OUT_VENDOR)/rfs/msm/cdsp/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/msm/cdsp,$(TARGET_OUT_VENDOR)/rfs/msm/cdsp/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/cdsp/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/cdsp/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/msm/cdsp/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/msm/cdsp/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/wpss,$(TARGET_OUT_VENDOR)/rfs/msm/wpss/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/msm/wpss,$(TARGET_OUT_VENDOR)/rfs/msm/wpss/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/wpss/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/msm/wpss/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/msm/wpss/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/msm/wpss/readonly/vendor/firmware)
#########################################################################
# MDM Folders
#########################################################################
$(call symlink-rfs,/data/vendor/tombstones/rfs/modem,$(TARGET_OUT_VENDOR)/rfs/mdm/mpss/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/mdm/mpss,$(TARGET_OUT_VENDOR)/rfs/mdm/mpss/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/mpss/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/mpss/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE) $(TARGET_OUT_VENDOR)/rfs/mdm/mpss/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/mdm/mpss/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/lpass,$(TARGET_OUT_VENDOR)/rfs/mdm/adsp/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/mdm/adsp,$(TARGET_OUT_VENDOR)/rfs/mdm/adsp/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/adsp/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/adsp/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/mdm/adsp/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/mdm/adsp/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/slpi,$(TARGET_OUT_VENDOR)/rfs/mdm/slpi/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/mdm/slpi,$(TARGET_OUT_VENDOR)/rfs/mdm/slpi/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/slpi/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/slpi/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/mdm/slpi/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/mdm/slpi/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/cdsp,$(TARGET_OUT_VENDOR)/rfs/mdm/cdsp/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/mdm/cdsp,$(TARGET_OUT_VENDOR)/rfs/mdm/cdsp/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/cdsp/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/cdsp/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/mdm/cdsp/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/mdm/cdsp/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/tn,$(TARGET_OUT_VENDOR)/rfs/mdm/tn/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/mdm/tn,$(TARGET_OUT_VENDOR)/rfs/mdm/tn/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/tn/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/tn/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/mdm/tn/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/mdm/tn/readonly/vendor/firmware)

$(call symlink-rfs,/data/vendor/tombstones/rfs/wpss,$(TARGET_OUT_VENDOR)/rfs/mdm/wpss/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/mdm/wpss,$(TARGET_OUT_VENDOR)/rfs/mdm/wpss/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/wpss/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/mdm/wpss/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/mdm/wpss/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/mdm/wpss/readonly/vendor/firmware)
#########################################################################
# APQ Folders
#########################################################################
$(call symlink-rfs,/data/vendor/tombstones/rfs/modem,$(TARGET_OUT_VENDOR)/rfs/apq/gnss/ramdumps)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/apq/gnss,$(TARGET_OUT_VENDOR)/rfs/apq/gnss/readwrite)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/rfs/shared,$(TARGET_OUT_VENDOR)/rfs/apq/gnss/shared)
$(call symlink-rfs,$(TARGET_TFTP_PERSIST)/hlos_rfs/shared,$(TARGET_OUT_VENDOR)/rfs/apq/gnss/hlos)
$(call symlink-rfs,$(TARGET_TFTP_FIRMWARE),$(TARGET_OUT_VENDOR)/rfs/apq/gnss/readonly/firmware)
$(call symlink-rfs,/vendor/firmware,$(TARGET_OUT_VENDOR)/rfs/apq/gnss/readonly/vendor/firmware)

include $(CLEAR_VARS)
LOCAL_MODULE := rfs_symlinks
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/rfs
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := .tmp
LOCAL_ADDITIONAL_DEPENDENCIES = $(RFS_SYMLINKS_DEPS)
include $(BUILD_PREBUILT)
