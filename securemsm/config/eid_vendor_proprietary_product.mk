ifeq ($(strip $(TARGET_USES_EID)),true)

#eID Build Rules
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0
PRODUCT_PACKAGES += vendor.qti.hardware.eid-V1.0-java
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0_vendor
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0-client
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0-impl
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0-service
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0-service.rc
PRODUCT_PACKAGES += vendor.qti.hardware.eid@1.0.vendor

endif #TARGET_USES_EID
