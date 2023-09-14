#tloc module related packages

ifneq ($(TARGET_BOARD_AUTO),true)
PRODUCT_PACKAGES += tloc_daemon
PRODUCT_PACKAGES += tloc_daemon.rc
endif
