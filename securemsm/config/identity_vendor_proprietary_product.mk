#PRODUCT_PACKAGES
ifeq ($(IDENTITY_USE_QTI_SERVICE),true)
PRODUCT_PACKAGES += android.hardware.identity-service-qti
PRODUCT_PACKAGES += android.hardware.identity-service-qti.rc
PRODUCT_PACKAGES += libqtiidentitycredential
endif # IDENTITY_USE_QTI_SERVICE
