DPM_VNDK := com.qualcomm.qti.dpm.api@1.0.vendor

SLM_VNDK := vendor.qti.hardware.slmadapter@1.0.vendor

MWQEM_VNDK := vendor.qti.hardware.mwqemadapter@1.0.vendor

PRODUCT_PACKAGES += $(DPM_VNDK)
PRODUCT_PACKAGES += $(SLM_VNDK)
PRODUCT_PACKAGES += $(MWQEM_VNDK)

