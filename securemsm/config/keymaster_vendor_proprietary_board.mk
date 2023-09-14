# QMAA mode configuration
ifeq ($(TARGET_USES_QMAA),true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_KMGK),true)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
KMGK_USE_QTI_STUBS := true
ENABLE_STRONGBOX_KM := false
endif #TARGET_USES_QMAA_OVERRIDE_KMGK
else # Non-QMAA mode configuration
# lahaina/sm8350 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),lahaina)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_STRONGBOX_KM := true
METADATA_ENC_FORMAT_VERSION_2 := true
ENABLE_AUTH_SECRET := true
endif

ifeq ($(TARGET_BOARD_PLATFORM),holi)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
METADATA_ENC_FORMAT_VERSION_2 := true
endif

# kona/sm8250 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),kona)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_STRONGBOX_KM := true
ADOPTABLE_STORAGE_SUPPORTED := true
METADATA_ENC_FORMAT_VERSION_2 := true
ENABLE_AUTH_SECRET := true
endif

# lito specific defines
ifeq ($(TARGET_BOARD_PLATFORM),lito)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
METADATA_ENC_FORMAT_VERSION_2 := true
endif

# atoll specific defines
ifeq ($(TARGET_BOARD_PLATFORM),atoll)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
endif

# Bengal specific defines
ifeq ($(TARGET_BOARD_PLATFORM),bengal)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
METADATA_ENC_FORMAT_VERSION_2 := true
endif

# Monaco specific defines
ifeq ($(TARGET_BOARD_PLATFORM),monaco)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
METADATA_ENC_FORMAT_VERSION_2 := true
ENABLE_QB_HANDLER := true
ADOPTABLE_STORAGE_SUPPORTED := true
endif

# msmnile/sm8150 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msmnile)
KMGK_USE_QTI_SERVICE := true
ifneq ($(ENABLE_HYP), true)
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
else
ENABLE_KM_4_0 := true
endif #ENABLE_HYP
ifneq ($(TARGET_BOARD_AUTO), true)
ENABLE_STRONGBOX_KM := true
endif #TARGET_BOARD_AUTO
endif

# sdmshrike specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdmshrike)
KMGK_USE_QTI_SERVICE := true
ifneq ($(ENABLE_HYP), true)
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
else
ENABLE_KM_4_0 := true
endif #ENABLE_HYP
ifneq ($(TARGET_BOARD_AUTO), true)
ENABLE_STRONGBOX_KM := true
endif #TARGET_BOARD_AUTO
endif

# sdm845 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdm845)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
endif

# trinket specific defines
ifeq ($(TARGET_BOARD_PLATFORM),$(TRINKET))
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
endif

# talos specific defines
ifeq ($(TARGET_BOARD_PLATFORM),$(MSMSTEPPE))
KMGK_USE_QTI_SERVICE := true
ifneq ($(ENABLE_HYP), true)
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
else
ENABLE_KM_4_0 := true
endif #ENABLE_HYP
endif

# sdm710 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdm710)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
ADOPTABLE_STORAGE_SUPPORTED := true
endif

# sdm660 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdm660)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
ADOPTABLE_STORAGE_SUPPORTED := true
METADATA_ENC_FORMAT_VERSION_2 := true
endif
ENABLE_KM_4_0 := true
endif

# msm8998 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8998)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ENABLE_KM_4_0 := true
endif

# msm8996 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8996)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ifeq ($(strip $(TARGET_KERNEL_VERSION)), 4.4)
ENABLE_KM_4_0 := true
endif
endif

# msm8937 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8937)
ADOPTABLE_STORAGE_SUPPORTED := true
METADATA_ENC_FORMAT_VERSION_2 := true
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ifeq ($(TARGET_KERNEL_VERSION),$(filter $(TARGET_KERNEL_VERSION),4.14 4.9))
ENABLE_KM_4_0 := true
else ifeq ($(TARGET_KERNEL_VERSION),$(filter $(TARGET_KERNEL_VERSION),4.19))
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
endif
endif

# msm8953 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8953)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ifeq ($(strip $(TARGET_KERNEL_VERSION)),4.19 4.9)
ENABLE_KM_4_0 := true
endif
endif

# msm8909 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8909)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
endif

endif #TARGET_USES_QMAA
