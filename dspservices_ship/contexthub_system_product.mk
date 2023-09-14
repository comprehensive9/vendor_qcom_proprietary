#Context hub HAL
ifneq ($(TARGET_BOARD_AUTO),true)
ifeq ($(call is-board-platform-in-list,sdm845 sdm670 sdm710 msmnile sdmshrike $(MSMSTEPPE)),true)
PRODUCT_PACKAGES += \
    android.hardware.contexthub@1.0-impl.generic \
    android.hardware.contexthub@1.0-service
endif
endif
