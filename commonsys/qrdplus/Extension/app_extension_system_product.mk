ifneq ($(TARGET_HAS_LOW_RAM),true)
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
PRODUCT_PACKAGES += \
    PowerOffAlarm \
    SimContact \
    PerformanceMode \
    QColor
endif
endif
