#DPM

ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
ifeq ($(BOARD_USES_DPM),true)
ifneq ($(BUILD_TINY_ANDROID),true)
ifeq ($(call is-vendor-board-platform,QCOM),true)

DPM := com.qti.dpmframework
DPM += com.qti.dpmframework.xml
DPM += dpm.conf
DPM += dpmapi
DPM += dpmapi.xml
DPM += dpmd
DPM += dpmd.rc
DPM += dpmserviceapp
DPM += libdpmctmgr
DPM += libdpmfdmgr
DPM += libdpmframework
DPM += libdpmtcm
DPM += tcmclient
DPM += libmwqemiptablemgr

PRODUCT_PACKAGES += $(DPM)

endif
endif
endif
endif

ifeq ($(call is-board-platform-in-list, lahaina holi), true)
  PRODUCT_SYSTEM_EXT_PROPERTIES += persist.vendor.dpm.idletimer.mode=default
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.idletimer.mode=default
endif

ifeq ($(TARGET_HAS_LOW_RAM),true)
  PRODUCT_SYSTEM_EXT_PROPERTIES += persist.vendor.dpm.feature=0
else
  PRODUCT_SYSTEM_EXT_PROPERTIES += persist.vendor.dpm.feature=11
endif
