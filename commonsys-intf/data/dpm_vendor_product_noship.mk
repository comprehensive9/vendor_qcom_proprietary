#DPM
DPM += dpmQmiMgr
DPM += libdpmqmihal
DPM += dpmQmiMgr.rc

PRODUCT_PACKAGES += $(DPM)

ifeq ($(TARGET_HAS_LOW_RAM),true)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpmhalservice.enable=0
else
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpmhalservice.enable=1
endif
