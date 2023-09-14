ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

PRODUCT_PACKAGES += vendor.qti.gnss@1.0
PRODUCT_PACKAGES += vendor.qti.gnss@1.1
PRODUCT_PACKAGES += vendor.qti.gnss@1.2
PRODUCT_PACKAGES += vendor.qti.gnss@2.0
PRODUCT_PACKAGES += vendor.qti.gnss@2.1
PRODUCT_PACKAGES += vendor.qti.gnss@3.0
PRODUCT_PACKAGES += vendor.qti.gnss@4.0

# List of supported board platform targets
LOC_BOARD_PLATFORM_LIST += msm8937
LOC_BOARD_PLATFORM_LIST += msm8953
LOC_BOARD_PLATFORM_LIST += msm8998
LOC_BOARD_PLATFORM_LIST += apq8098_latv
LOC_BOARD_PLATFORM_LIST += sdm660
LOC_BOARD_PLATFORM_LIST += sdm845
LOC_BOARD_PLATFORM_LIST += msmpeafowl
LOC_BOARD_PLATFORM_LIST += sdm710
LOC_BOARD_PLATFORM_LIST += qcs605
LOC_BOARD_PLATFORM_LIST += msmnile
LOC_BOARD_PLATFORM_LIST += sdmshrike
LOC_BOARD_PLATFORM_LIST += $(MSMSTEPPE)
LOC_BOARD_PLATFORM_LIST += $(TRINKET)
LOC_BOARD_PLATFORM_LIST += kona
LOC_BOARD_PLATFORM_LIST += atoll
LOC_BOARD_PLATFORM_LIST += lito
LOC_BOARD_PLATFORM_LIST += bengal
LOC_BOARD_PLATFORM_LIST += lahaina
LOC_BOARD_PLATFORM_LIST += holi
LOC_BOARD_PLATFORM_LIST += monaco

ifneq (,$(filter $(LOC_BOARD_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))

ifeq ($(TARGET_DEVICE),apq8026_lw)
LW_FEATURE_SET := true
endif

ifeq ($(TARGET_SUPPORTS_WEAR_OS),true)
LW_FEATURE_SET := true
endif

ifneq ($(LW_FEATURE_SET),true)

PRODUCT_PACKAGES += com.qualcomm.location
PRODUCT_PACKAGES += com.qualcomm.location.xml

PRODUCT_PACKAGES += privapp-permissions-com.qualcomm.location.xml
PRODUCT_PACKAGES += xtra_t_app
PRODUCT_PACKAGES += xtra_t_app_setup
PRODUCT_PACKAGES += izat.xt.srv
PRODUCT_PACKAGES += izat.xt.srv.xml
PRODUCT_PACKAGES += com.qti.location.sdk
PRODUCT_PACKAGES += com.qti.location.sdk.xml
PRODUCT_PACKAGES += liblocationservice_jni
PRODUCT_PACKAGES += liblocsdk_diag_jni
PRODUCT_PACKAGES += libxt_native

# GPS_DBG
ifneq ($(TARGET_HAS_LOW_RAM),true)
PRODUCT_PACKAGES_DEBUG += com.qualcomm.qti.qlogcat
endif

endif # ifneq ($(LW_FEATURE_SET),true)

PRODUCT_PACKAGES_DEBUG += com.qualcomm.qmapbridge.xml
PRODUCT_PACKAGES_DEBUG += com.qualcomm.qti.izattools.xml
PRODUCT_PACKAGES_DEBUG += ODLT
PRODUCT_PACKAGES_DEBUG += qmapbridge
PRODUCT_PACKAGES_DEBUG += libdiagbridge
PRODUCT_PACKAGES_DEBUG += libloc2jnibridge
PRODUCT_PACKAGES_DEBUG += SampleLocationAttribution

endif # ifneq (,$(filter $(LOC_BOARD_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
