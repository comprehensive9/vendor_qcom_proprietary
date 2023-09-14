
ifneq ($(TARGET_NO_TELEPHONY), true)
PRODUCT_PACKAGES += \
    embms \
    embmslibrary \
    embms.xml
ifneq ($(TARGET_HAS_LOW_RAM),true)
PRODUCT_PACKAGES += \
    uimlpaservice \
    remotesimlockservice \
    uimservicelibrary \
    UimService.xml \
    uimgbaservice
PRODUCT_PACKAGES_DEBUG += \
    EmbmsTestApp \
    uimlpatest
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS), true)
PRODUCT_PACKAGES_DEBUG += \
    MultiplePdpTest
endif
endif
endif


PRODUCT_PACKAGES_DEBUG += \
    TestAppForSAM \
    QtiMmsTestApp \
