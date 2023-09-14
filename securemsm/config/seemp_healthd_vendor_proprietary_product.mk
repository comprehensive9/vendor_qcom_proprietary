#seemp_healthd module related packages

ifeq ($(call is-board-platform-in-list, sdm710 sdm845 sdm660),true)

PRODUCT_PACKAGES += seemp_healthd
PRODUCT_PACKAGES += seemp_healthd.rc

endif
