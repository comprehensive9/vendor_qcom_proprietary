#DPM
DPM += dpmQmiMgr
DPM += libdpmqmihal
DPM += dpmQmiMgr.rc

PRODUCT_PACKAGES += $(DPM)

ifeq ($(call is-board-platform-in-list, lahaina holi), true)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.idletimer.mode=default
endif

ifeq ($(TARGET_HAS_LOW_RAM),true)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpmhalservice.enable=0
else
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpmhalservice.enable=1
endif


