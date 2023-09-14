ifneq ($(TARGET_BOARD_AUTO),true)
ifeq ($(call is-board-platform-in-list,sdm845 sdm670 sdm710 msmnile sdmshrike $(MSMSTEPPE)),true)
PRODUCT_PACKAGES += chre
endif
endif
