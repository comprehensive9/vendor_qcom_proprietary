#ANT
BOARD_ANT_WIRELESS_DEVICE := "qualcomm-hidl"
ifneq ($(filter msm8937 msm8953 msm8909, $(TARGET_BOARD_PLATFORM)),)
BOARD_ANT_WIRELESS_DEVICE := "vfs-prerelease"
endif

#BT
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_QCOM := true
TARGET_USE_QTI_BT_CONFIGSTORE := true
TARGET_USE_QTI_BT_SAR := true
TARGET_USE_QTI_VND_FWK_DETECT := true

#FM
ifneq ($(TARGET_BOARD_TYPE), auto)
BOARD_HAVE_QCOM_FM := true
endif

ifeq ($(TARGET_USES_QMAA), true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_BLUETOOTH), true)
BOARD_HAVE_BLUETOOTH_QCOM := false
DISABLE_BT_FTM := true
BOARD_ANT_WIRELESS_DEVICE :=
endif
ifneq ($(TARGET_USES_QMAA_OVERRIDE_FM), true)
BOARD_HAVE_QCOM_FM := false
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM), sdm845) # SDM845 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), msmnile) # msmnile specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
TARGET_HAS_DUN_HIDL_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), sm6150)  # Talos/sm6150 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), sdm710)  # sdm710/Warlock specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), sdm660)  # sdm660 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), kona)  # kona/sm8250 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
TARGET_HAS_DUN_HIDL_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "richwave"
endif

ifeq ($(TARGET_BOARD_PLATFORM), trinket) # trinket specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), lito) # lito specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), bengal) # bengal specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
ifeq ($(TARGET_BOARD_SUFFIX), _32)
BOARD_HAVE_QTI_BT_LAZY_SERVICE := true
endif
ifeq ($(TARGET_BOARD_SUFFIX), _32go)
BOARD_HAVE_QTI_BT_LAZY_SERVICE := true
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM), atoll) # atoll specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), lahaina) # lahaina specific defines
BOARD_HAS_QCA_FM_SOC := "richwave"
TARGET_HAS_BT_QCV_FOR_SPF := true
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8998) # MSM8998 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8996) # MSM8996 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8937) # msm8937 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := false
ifeq ($(TARGET_BOARD_SUFFIX), _32go)
BOARD_HAVE_QTI_BT_LAZY_SERVICE := true
TARGET_USE_QTI_BT_SAR := false
endif
ifeq ($(TARGET_BOARD_SUFFIX), _lily)
BOARD_HAVE_QTI_BT_LAZY_SERVICE := true
TARGET_USE_QTI_BT_SAR := false
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8953) # MSM8953 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8909) # MSM8909 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8952) # MSM8952 specific defines
TARGET_HAS_SPLIT_A2DP_FEATURE := false
endif

ifeq ($(TARGET_BOARD_PLATFORM), holi) # holi specific defines
TARGET_HAS_BT_QCV_FOR_SPF := true
endif
