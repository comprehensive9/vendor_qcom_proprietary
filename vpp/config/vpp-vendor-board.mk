TARGET_DISABLE_QTI_VPP := false

ifeq ($(call is-board-platform,$(TRINKET)),true)
    TARGET_DISABLE_QTI_VPP := true
endif

ifeq ($(call is-board-platform,atoll),true)
    TARGET_DISABLE_QTI_VPP := true
endif

ifeq ($(ENABLE_HYP), true)
    TARGET_DISABLE_QTI_VPP := true
endif
