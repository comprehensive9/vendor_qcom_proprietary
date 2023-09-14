ifeq ($(TARGET_BOARD_PLATFORM),holi)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.debug.ssc_qmi_debug=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.hal_trigger_ssr=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.rt_task=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.support_direct_channel=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.bypass_worker=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.sync_request=true
endif
ifeq ($(TARGET_BOARD_PLATFORM),bengal_32)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.debug.ssc_qmi_debug=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.hal_trigger_ssr=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.rt_task=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.support_direct_channel=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.bypass_worker=true
endif

ifeq ($(TARGET_BOARD_PLATFORM),bengal)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.debug.ssc_qmi_debug=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.hal_trigger_ssr=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.rt_task=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.support_direct_channel=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.bypass_worker=true
endif

ifeq ($(TARGET_BOARD_PLATFORM),lahaina)
# Sensor conf files
SNS_PRODUCT_SKU_LAHAINA := sku_lahaina
SNS_PRODUCT_SKU_SHIMA   := sku_shima
SNS_PRODUCT_SKU_YUPIK   := sku_yupik
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_LAHAINA)/android.hardware.sensor.hifi_sensors.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_SHIMA)/android.hardware.sensor.hifi_sensors.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(SNS_PRODUCT_SKU_YUPIK)/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.sync_request=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.allow_non_default_discovery=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.on_change_sample_period=true
endif

ifeq ($(TARGET_BOARD_PLATFORM),$(TRINKET))
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.support_direct_channel=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.rt_task=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.bypass_worker=true
endif

ifeq ($(TARGET_BOARD_PLATFORM),atoll)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.allow_non_default_discovery=true
endif

ifeq ($(TARGET_BOARD_PLATFORM),lito)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.debug.ssc_qmi_debug=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.allow_non_default_discovery=true


endif

ifeq ($(TARGET_BOARD_PLATFORM),kona)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.sync_request=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.allow_non_default_discovery=true
endif

ifeq ($(TARGET_BOARD_PLATFORM),msmnile)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml
endif

ifeq ($(TARGET_BOARD_PLATFORM),sdm845)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.firmware_property_read_support=false
endif

ifeq ($(TARGET_BOARD_PLATFORM),sdm710)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.firmware_property_read_support=false
endif

ifeq ($(TARGET_BOARD_PLATFORM),$(MSMSTEPPE))
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8937)
# Sensor conf files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.support_direct_channel=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.rt_task=false
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.enable.bypass_worker=true
ifeq ($(TARGET_SUPPORTS_WEAR_OS),true)
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.sensors.hal_trigger_ssr=true
endif
endif

#SNS_VENDOR_PRODUCTS
ifeq ($(TARGET_USES_QMAA), true)
ifeq ($(TARGET_USES_QMAA_OVERRIDE_SENSORS),true)
SNS_VENDOR_PRODUCTS := hals.conf
endif
else
SNS_VENDOR_PRODUCTS := hals.conf
endif
SNS_VENDOR_PRODUCTS += sensors_settings
SNS_VENDOR_PRODUCTS += sns_reg_config
SNS_VENDOR_PRODUCTS += sns_reg_version
SNS_VENDOR_PRODUCTS += sensors_registry
SNS_VENDOR_PRODUCTS += sensors.qti
SNS_VENDOR_PRODUCTS += vendor.sensors.qti.rc
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifneq ($(call is-board-platform-in-list,$(TRINKET) sdm710 $(MSMSTEPPE) qcs605 msm8937), true)
ifneq ($(ENABLE_HYP), true)
SNS_VENDOR_PRODUCTS += sscrpcd
SNS_VENDOR_PRODUCTS += vendor.sensors.sscrpcd.rc
endif
endif
endif
SNS_VENDOR_PRODUCTS += sensors.ssc
SNS_VENDOR_PRODUCTS += init.vendor.sensors.rc
SNS_VENDOR_PRODUCTS += android.hardware.sensors@2.0-service.multihal
SNS_VENDOR_PRODUCTS += android.hardware.sensors@2.0-ScopedWakelock
SNS_VENDOR_PRODUCTS += sensors_config_module
SNS_VENDOR_PRODUCTS += sensors_list
ifeq ($(call is-vendor-board-platform,QCOM),true)
SNS_VENDOR_PRODUCTS += libsns_registry_skel
endif
SNS_VENDOR_PRODUCTS += libsnsdiaglog
SNS_VENDOR_PRODUCTS += libssc
SNS_VENDOR_PRODUCTS += libsnsapi
SNS_VENDOR_PRODUCTS += libsensorslog
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
SNS_VENDOR_PRODUCTS += libsns_low_lat_stream_stub
SNS_VENDOR_PRODUCTS += libsns_low_lat_stream_skel
endif
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
SNS_VENDOR_PRODUCTS += libsns_device_mode_stub
SNS_VENDOR_PRODUCTS += libsns_device_mode_skel
SNS_VENDOR_PRODUCTS += libsns_fastRPC_util
endif
SNS_VENODR_PRODUCTS += libprotobuf-cpp-full-vendor
#enable for new direct channel support
ifeq ($(SNS_SUPPORT_DIRECT_CHANNEL_CLIENT_API) , true)
SNS_VENDOR_PRODUCTS += libsns_direct_channel_stub
endif
ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
ifeq ($(TARGET_SUPPORTS_WEAR_OS),true)
SNS_VENDOR_PRODUCTS += sns_wear_wo_reg_group_list
SNS_VENDOR_PRODUCTS += sns_wrist_ssr
SNS_VENDOR_PRODUCTS += vendor.sensors.wrist-ssr.rc
endif
endif

#SNS_CAL_VENDOR_PRODUCTS
SNS_CAL_VENDOR_PRODUCTS := vendor.qti.hardware.sensorscalibrate@1.0
SNS_CAL_VENDOR_PRODUCTS += vendor.qti.hardware.sensorscalibrate-V1.0-java
SNS_CAL_VENDOR_PRODUCTS += vendor.qti.hardware.sensorscalibrate@1.0-service
ifeq ($(TARGET_PRODUCT),bengal_32)
SNS_CAL_VENDOR_PRODUCTS += vendor.qti.hardware.sensorscalibrate@1.0-service.disable.rc
else
ifeq ($(TARGET_HAS_LOW_RAM), true)
SNS_CAL_VENDOR_PRODUCTS += vendor.qti.hardware.sensorscalibrate@1.0-service.disable.rc
else
SNS_CAL_VENDOR_PRODUCTS += vendor.qti.hardware.sensorscalibrate@1.0-service.rc
endif
endif
SNS_CAL_VENDOR_PRODUCTS += vendor.qti.hardware.sensorscalibrate@1.0-impl
SNS_CAL_VENDOR_PRODUCTS += vendor-qti-hardware-sensorscalibrate.xml
SNS_CAL_VENDOR_PRODUCTS += libsensorcal

#SNS_VENDOR_PRODUCTS_DEBUG
ifeq ($(TARGET_HAS_LOW_RAM), true)
ifeq ($(TARGET_SUPPORTS_WEARABLES), true)
SNS_VENDOR_PRODUCTS_DBG := QSensorTest
endif
else
SNS_VENDOR_PRODUCTS_DBG := QSensorTest
endif
SNS_VENDOR_PRODUCTS_DBG += init.qcom.sensors.sh
SNS_VENDOR_PRODUCTS_DBG += see_resampler
SNS_VENDOR_PRODUCTS_DBG += see_selftest
SNS_VENDOR_PRODUCTS_DBG += see_workhorse
SNS_VENDOR_PRODUCTS_DBG += ssc_drva_test
SNS_VENDOR_PRODUCTS_DBG += ssc_sensor_info
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
SNS_VENDOR_PRODUCTS_DBG += sns_ext_svc_test
endif
SNS_VENDOR_PRODUCTS_DBG += libSEESalt
SNS_VENDOR_PRODUCTS_DBG += UnifiedSensorTestApp
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
SNS_VENDOR_PRODUCTS_DBG += libsensor_low_lat
endif
SNS_VENDOR_PRODUCTS_DBG += libUSTANative
SNS_VENDOR_PRODUCTS_DBG += libSensorCalLibNative

SNS_VENDOR_PRODUCTS_DBG += SNS4A_accel_9a418d19f8d7ba44_1fbb6afc01727ea6.txt
SNS_VENDOR_PRODUCTS_DBG += SNS6A_accel_9a418d19f8d7ba44_1fbb6afc01727ea6.txt
SNS_VENDOR_PRODUCTS_DBG += SNS9A_pedometer_57f249b272e1d985_f67eb04f0c5bdaf7.txt
SNS_VENDOR_PRODUCTS_DBG += SNS9A_sig_motion_b13b11c377fc7bba_3b4fb01aa61bbbf4.txt
SNS_VENDOR_PRODUCTS_DBG += SNS9A_tilt_1602c5c25b1aaf88_36431f840d547294.txt
SNS_VENDOR_PRODUCTS_DBG += SNS10A_pedometer_57f249b272e1d985_f67eb04f0c5bdaf7.txt
SNS_VENDOR_PRODUCTS_DBG += SNS10A_sig_motion_b13b11c377fc7bba_3b4fb01aa61bbbf4.txt
SNS_VENDOR_PRODUCTS_DBG += SNS10A_tilt_1602c5c25b1aaf88_36431f840d547294.txt
SNS_VENDOR_PRODUCTS_DBG += SNS6A_accel_354639543f5014b0_a993598532f4db85.txt
SNS_VENDOR_PRODUCTS_DBG += SNS11A_amd_87744fe9eb6add92_ae4f94a8b60df456.txt


PRODUCT_PACKAGES += $(SNS_VENDOR_PRODUCTS)
PRODUCT_PACKAGES += $(SNS_CAL_VENDOR_PRODUCTS)
PRODUCT_PACKAGES_DEBUG += $(SNS_VENDOR_PRODUCTS_DBG)
