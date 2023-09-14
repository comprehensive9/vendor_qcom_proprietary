ifeq ($(call is-board-platform-in-list, kona), true)
PRODUCT_PACKAGES += vendor.qti.spu@1.0
PRODUCT_PACKAGES += vendor.qti.spu@1.0.vendor
PRODUCT_PACKAGES += vendor.qti.spu@1.0-service
PRODUCT_PACKAGES += init.vendor.qti.spu@1.0-service.rc
endif
