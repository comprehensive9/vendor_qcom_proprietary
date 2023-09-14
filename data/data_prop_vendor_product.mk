#DATA_PROP
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
DATA_PROP := ATFWD-daemon
endif
DATA_PROP += dsi_config.xml
DATA_PROP += libconfigdb
DATA_PROP += libdsutils
DATA_PROP += libnetmgr
DATA_PROP += libnetmgr_common
DATA_PROP += libdsi_netctrl
DATA_PROP += libqdi
DATA_PROP += libqdp
DATA_PROP += netmgr_config.xml
DATA_PROP += netmgrd
DATA_PROP += netmgrd.rc
DATA_PROP += port-bridge
DATA_PROP += port-bridge.rc

#IPACM-diag
ifeq ($(TARGET_HAS_LOW_RAM),true)
BOARD_IPA_LOW_RAM_EXCP_LIST := bengal
BOARD_IPA_LOW_RAM_EXCP_LIST += monaco
ifeq ($(call is-board-platform-in-list,$(BOARD_IPA_LOW_RAM_EXCP_LIST)),true)
DATA_PROP += ipacm-diag
DATA_PROP += ipacm-diag.rc
endif
else
BOARD_PLATFORM_LIST := msm8916
BOARD_PLATFORM_LIST += msm8909
BOARD_PLATFORM_LIST += msm8917
ifneq ($(call is-board-platform-in-list,$(BOARD_PLATFORM_LIST)),true)
ifneq (,$(filter $(QCOM_BOARD_PLATFORMS),$(TARGET_BOARD_PLATFORM)))
ifneq (, $(filter arm64 arm, $(TARGET_ARCH)))
DATA_PROP += ipacm-diag
DATA_PROP += ipacm-diag.rc
endif
endif
endif
endif
DATA_PROP += libqcmaputils
DATA_PROP += qti
DATA_PROP += adpl

#IPA_FWS
IPA_FWS := ipa_fws
IPA_FWS += ipa_fws.elf
IPA_FWS += ipa_fws.rc
IPA_FWS += ipa_uc.elf
IPA_FWS += ipa_uc

#DATA_PROP_HY11_HY22_diff
DATA_PROP_HY11_HY22_diff := libqdi
DATA_PROP_HY11_HY22_diff += libqdp

PRODUCT_PACKAGES += $(IPA_FWS)
PRODUCT_PACKAGES += $(DATA_PROP)
PRODUCT_PACKAGES += $(DATA_PROP_HY11_HY22_diff)

# Ipsec_tunnels feature
BOARD_IPSEC_FEATURE_EXCP_LIST := msm8998
BOARD_IPSEC_FEATURE_EXCP_LIST += sdm660
ifneq ($(call is-board-platform-in-list,$(BOARD_IPSEC_FEATURE_EXCP_LIST)),true)
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.ipsec_tunnels.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.ipsec_tunnels.xml
endif

