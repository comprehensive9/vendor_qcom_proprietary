ifneq ($(TARGET_BOARD_AUTO),true)
ifneq ($(ENABLE_HYP), true)

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
IMS_SHIP_RCS += uceShimService
IMS_SHIP_RCS += whitelist_com.qualcomm.qti.uceShimService.xml
endif

IMS_SHIP_VT += lib-imscamera
IMS_SHIP_VT += lib-imsvideocodec

PRODUCT_PACKAGES += $(IMS_SHIP_RCS)
PRODUCT_PACKAGES += $(IMS_SHIP_VT)

endif
endif
