ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list, msmnile sdmshrike),true)
    SECUREMSM += init.spdaemon.rc
    PRODUCT_PACKAGES += $(SECUREMSM)
else ifeq ($(call is-board-platform-in-list, kona),true)
    SECUREMSM += init.spdaemon_wait_spunvm.rc
    PRODUCT_PACKAGES += $(SECUREMSM)
endif
endif
