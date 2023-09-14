ifneq ($(TARGET_BOARD_AUTO),true)
ifneq ($(TARGET_HAS_LOW_RAM),true)
#QCC-TR-UI
PRODUCT_PACKAGES += QCC-TR-UI
endif
endif
