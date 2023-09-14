ifeq ($(strip $(TARGET_USES_GPQESE)),true)
#SECURE_ELEMENT Build Rules
SECUREMSM_VENDOR += android.hardware.secure_element@1.0-impl
SECUREMSM_VENDOR += vendor.qti.secure_element@1.2-service
SECUREMSM_VENDOR += vendor.qti.secure_element@1.2-service.rc
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.se.omapi.ese.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.se.omapi.ese.xml
endif

ifneq ($(TARGET_BOARD_AUTO),true)
#eSEPowerManager Build Rules
ifneq ($(TARGET_PRODUCT),bengal_32go)
SECUREMSM_VENDOR += vendor.qti.esepowermanager@1.1-impl
SECUREMSM_VENDOR += vendor.qti.esepowermanager@1.1-service
SECUREMSM_VENDOR += vendor.qti.esepowermanager@1.1-service.rc
endif
endif

PRODUCT_PACKAGES += $(SECUREMSM_VENDOR)
