DATA_DIR := $(call my-dir)

ifneq ($(TARGET_BOARD_SUFFIX),_gvmq)
 include $(call all-subdir-makefiles)
else
 include $(DATA_DIR)/configdb/Android.mk
 include $(DATA_DIR)/dsi_netctrl/Android.mk
 include $(DATA_DIR)/dsucsd/Android.mk
 include $(DATA_DIR)/dsutils/Android.mk
 include $(DATA_DIR)/ipa_fws/Android.mk
 include $(DATA_DIR)/netmgr/Android.mk
 include $(DATA_DIR)/lqe/Android.mk
 include $(DATA_DIR)/port-bridge/Android.mk
 include $(DATA_DIR)/qdi/Android.mk
 include $(DATA_DIR)/tests/Android.mk
endif
