ifeq ($(call is-board-platform-in-list, sdm845 sdm710 msmnile $(MSMSTEPPE) $(TRINKET) kona lito bengal atoll lahaina holi monaco),true)
TARGET_USES_NQ_NFC := true
# Disable NFC support on targets matching the TARGET_PRODUCT value in the list
ifeq ($(filter $(TARGET_PRODUCT), bengal_32 bengal_32go),$(TARGET_PRODUCT))
TARGET_USES_NQ_NFC := false
endif
endif

ifneq ($(QC_PROP_ROOT), )
QC_PROP_ROOT := vendor/qcom/proprietary
endif

NQ_VENDOR_NFC_PROP := nqnfcinfo

#SN100 NFC firmware images + config files
ifeq ($(TARGET_ARCH), arm64)
PRODUCT_COPY_FILES += \
    $(QC_PROP_ROOT)/nqnfc-firmware/sn100x/64-bit/libsn100u_fw.so:$(TARGET_COPY_OUT_VENDOR)/lib/libsn100u_fw.so
else
PRODUCT_COPY_FILES += \
    $(QC_PROP_ROOT)/nqnfc-firmware/sn100x/32-bit/libsn100u_fw.so:$(TARGET_COPY_OUT_VENDOR)/lib/libsn100u_fw.so
endif

PRODUCT_COPY_FILES += \
    $(QC_PROP_ROOT)/nqnfc-firmware/libnfc-mtp-SN100.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-mtp-SN100.conf \
    $(QC_PROP_ROOT)/nqnfc-firmware/libnfc-qrd-SN100.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-qrd-SN100.conf \
    $(QC_PROP_ROOT)/nqnfc-firmware/libnfc-mtp-SN100_38_4MHZ.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-mtp-SN100_38_4MHZ.conf \
    $(QC_PROP_ROOT)/nqnfc-firmware/libnfc-qrd-SN100_38_4MHZ.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-qrd-SN100_38_4MHZ.conf

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
PRODUCT_PACKAGES += $(NQ_VENDOR_NFC_PROP)
endif
