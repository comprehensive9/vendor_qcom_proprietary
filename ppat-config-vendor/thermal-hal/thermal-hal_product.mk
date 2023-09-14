THERMAL_HAL := android.hardware.thermal@2.0-impl
THERMAL_HAL += android.hardware.thermal@2.0-service.qti
THERMAL_HAL += android.hardware.thermal@2.0-service.qti.rc
THERMAL_HAL += android.hardware.thermal@2.0-service.qti.xml

PRODUCT_PACKAGES += $(THERMAL_HAL)

LIMITS_HAL += vendor.qti.hardware.limits@1.0-service
LIMITS_HAL += vendor.qti.hardware.limits@1.0-service.rc
LIMITS_HAL += vendor.qti.hardware.limits@1.0-service.xml

PRODUCT_PACKAGES += $(LIMITS_HAL)

PRODUCT_PACKAGES_DEBUG += thermal_hal_test
