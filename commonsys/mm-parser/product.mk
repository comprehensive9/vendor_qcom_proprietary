ifneq ($(filter true, $(TARGET_FWK_SUPPORTS_FULL_VALUEADDS) $(TARGET_FWK_SUPPORTS_AV_VALUEADDS)),)
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.mm.enable.qcom_parser=16777215
PRODUCT_PACKAGES += libmmparserextractor
PRODUCT_PACKAGES += libmmparser_lite
PRODUCT_BOUNDS_EXCLUDE_PATHS += vendor/qcom/proprietary/commonsys/mm-parser-noship
endif
