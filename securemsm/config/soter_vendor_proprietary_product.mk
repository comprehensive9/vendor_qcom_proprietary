ifeq ($(strip $(TARGET_USES_SOTER)),true)

#SOTER Build Rules
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0
PRODUCT_PACKAGES += vendor.qti.hardware.soter-V1.0-java
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0_vendor
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0-client
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0-provision
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0-impl
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0-service
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0-service.rc
PRODUCT_PACKAGES += vendor.qti.hardware.soter@1.0.vendor

endif #TARGET_USES_SOTER
