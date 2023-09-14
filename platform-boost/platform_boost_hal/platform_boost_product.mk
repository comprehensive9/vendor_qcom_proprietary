ifeq ($(TARGET_ENABLE_PLATFORM_BOOST),true)
  EXTRA_VENDOR_LIBRARIES += vendor.qti.platform_boost@1.0

  PLATFORM_BOOST = vendor.qti.platform_boost@1.0
  PLATFORM_BOOST += vendor.qti.platform_boost-V1.0-java
  PLATFORM_BOOST += vendor.qti.platform_boost@1.0-service
  PLATFORM_BOOST += boost_mode.xml
  PLATFORM_BOOST += boost_cap.xml

  PRODUCT_PACKAGES += $(PLATFORM_BOOST)
  PRODUCT_PACKAGES += TestPlatformBoostHIDL
endif