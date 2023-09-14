ifeq ($(TARGET_USES_QMAA),true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_KMGK),true)
ifeq ($(KMGK_USE_QTI_STUBS),)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
KMGK_USE_QTI_STUBS := true
ENABLE_STRONGBOX_KM := false
endif #KMGK_USE_QTI_STUBS
endif #TARGET_USES_QMAA_OVERRIDE_KMGK
endif #TARGET_USES_QMAA

# Enable TDES support when KM4 is enabled
ifeq ($(ENABLE_KM_4_0), true)
PRODUCT_PROPERTY_OVERRIDES += ro.hardware.keystore_desede=true
endif

#disable strongbox support for auto targets
ifeq ($(TARGET_BOARD_AUTO), true)
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.gatekeeper.disable_spu = true
endif

ifeq ($(ADOPTABLE_STORAGE_SUPPORTED), true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.crypto.volume.filenames_mode = "aes-256-cts" \
    ro.crypto.set_dun=true
endif

ifeq ($(METADATA_ENC_FORMAT_VERSION_2), true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.crypto.dm_default_key.options_format.version = 2 \
    ro.crypto.volume.metadata.method=dm-default-key
endif

#PRODUCT_PACKAGES
ifeq ($(KMGK_USE_QTI_SERVICE),true)
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-impl-qti
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-service-qti
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-service-qti.rc
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-impl-qti

ifeq ($(ENABLE_KM_4_0), true)
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-service-qti.rc

else ifeq ($(ENABLE_KM_4_1), true)
PRODUCT_PACKAGES += android.hardware.keymaster@4.1-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@4.1-service-qti.rc
ifdef SHIPPING_API_LEVEL
# Copy “android.hardware.hardware_keystore” feature if the shipping API level is at least 31. 
ifeq ($(call math_gt_or_eq, $(SHIPPING_API_LEVEL),31), true)
PRODUCT_COPY_FILES += vendor/qcom/proprietary/securemsm/keymaster_utils/android.hardware.hardware_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.hardware_keystore.xml
endif #ifdef SHIPPING_API_LEVEL
endif

else
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-service-qti.rc
endif #ENABLE_KM_4_0

ifeq ($(ENABLE_STRONGBOX_KM), true)
PRODUCT_PACKAGES += keymasterd
PRODUCT_PACKAGES += keymasterd.rc
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-strongbox-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-strongbox-service-qti.rc
ifeq ($(TARGET_BOARD_PLATFORM),lahaina)
PRODUCT_SKU_LAHAINA := sku_lahaina
PRODUCT_COPY_FILES += vendor/qcom/proprietary/securemsm/keymaster_utils/android.hardware.strongbox_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(PRODUCT_SKU_LAHAINA)/android.hardware.strongbox_keystore.xml
else
PRODUCT_COPY_FILES += vendor/qcom/proprietary/securemsm/keymaster_utils/android.hardware.strongbox_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.strongbox_keystore.xml
endif
endif

else
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-impl
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-service
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-impl
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-service
endif # KMGK_USE_QTI_SERVICE

ifeq ($(ENABLE_AUTH_SECRET), true)
PRODUCT_PACKAGES += android.hardware.authsecret@1.0-service-qti
PRODUCT_PACKAGES += android.hardware.authsecret@1.0-service-qti.rc
endif

PRODUCT_PACKAGES += gatekeeper.$(TARGET_BOARD_PLATFORM)
PRODUCT_PACKAGES += init.qti.keymaster.sh
PRODUCT_PACKAGES += keystore.$(TARGET_BOARD_PLATFORM)
PRODUCT_PACKAGES += KmInstallKeybox
PRODUCT_PACKAGES += libkeymasterdeviceutils
PRODUCT_PACKAGES += libkeymasterprovision
PRODUCT_PACKAGES += libkeymasterutils
PRODUCT_PACKAGES += libqtikeymaster4
PRODUCT_PACKAGES += qti_gatekeeper_tests
PRODUCT_PACKAGES += qti_keymaster_tests
PRODUCT_PACKAGES += android.hardware.keymaster@3.0.vendor
PRODUCT_PACKAGES += android.hardware.keymaster@4.0.vendor
PRODUCT_PACKAGES += android.hardware.keymaster@4.1.vendor
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0.vendor
PRODUCT_PACKAGES += android.hardware.authsecret@1.0.vendor
