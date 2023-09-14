
ifneq ($(TARGET_NO_TELEPHONY), true)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.radio.sib16_support=1 \
    persist.vendor.radio.custom_ecc=1 \
    persist.vendor.radio.procedure_bytes=SKIP

ifneq ($(call is-board-platform-in-list, monaco),true)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.radio.apm_sim_not_pwdn=1
endif

ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
    PRODUCT_PROPERTY_OVERRIDES += persist.radio.multisim.config=dsds
endif

PRODUCT_COPY_FILES += \
frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml \
frameworks/native/data/etc/android.hardware.telephony.cdma.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.cdma.xml \
frameworks/native/data/etc/android.hardware.telephony.ims.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.ims.xml
endif #TARGET_NO_TELEPHONY

ifneq ($(call is-board-platform-in-list, msm8909 qcs605 taro),true)
ifneq ($(TARGET_PRODUCT),sdm660_32)
PRODUCT_COPY_FILES += \
frameworks/native/data/etc/android.hardware.se.omapi.uicc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.se.omapi.uicc.xml
endif
endif

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
PRODUCT_PACKAGES += ATFWD-daemon
endif

# QCRIL
QCRIL := liblqe
QCRIL += librilqmiservices

ifneq ($(TARGET_NO_TELEPHONY), true)
ifeq ($(call is-board-platform-in-list, lahaina holi monaco),true)
# QCRIL-NR
QCRIL += qcrilNr.db
QCRIL += libqcrilNr
QCRIL += qcrild_libqcrilnr
QCRIL += qcrild_libqcrilnrutils
QCRIL += libwmsts
QCRIL += libqcrilNrLogger
QCRIL += qcrilNrd
QCRIL += qcrilNrd.rc
QCRIL += libqcrilNrQtiMutex
QCRIL += qtiwakelock
QCRIL += libqcrilNrFramework
QCRIL += deviceInfoServiceModuleNr
QCRIL += ccid_daemon_nr
QCRIL += qcril_client

QCRIL_DEBUG += qcril_cli

else

# QCRIL-HAL
QCRIL += qcril.db
QCRIL += libril-qc-hal-qmi
QCRIL += libril-qcril-hook-oem
QCRIL += qcrild_libril
QCRIL += qcrild_librilutils
QCRIL += libsettings
QCRIL += qcrild
QCRIL += qcrild.rc
QCRIL += qtimutex
QCRIL += libqcrilFramework
QCRIL += deviceInfoServiceModule
endif

# qtisetprop, qtigetprop required for all targets
QCRIL += qtisetprop
QCRIL += qtigetprop
QCRIL += libril-qc-radioconfig
QCRIL += libril-qc-ltedirectdisc
QCRIL += libqmiextservices

ifeq ($(filter $(TARGET_PRODUCT), msm8937_lily),$(TARGET_PRODUCT))
TELEPHONY_APPS += ImsOverlayVendor
endif #msm8937_lily

endif #TARGET_NO_TELEPHONY

PRODUCT_PACKAGES += $(QCRIL)
PRODUCT_PACKAGES += $(TELEPHONY_APPS)

PRODUCT_PACKAGES_DEBUG += $(QCRIL_DEBUG)

ENABLE_VENDOR_RIL_SERVICE := true

