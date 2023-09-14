#include common overlays
PRODUCT_PACKAGES += \
    FrameworksResCommon \
    CarrierConfigResCommon \
    CellBroadcastReceiverResCommon \
    SystemUIResCommon \
    TelecommResCommon \
    TelephonyResCommon \
    FrameworksResCommonQva \
    WifiResCommon

#include target specific overlays
OVERLAY_PATH_SYS_INTF := vendor/qcom/proprietary/commonsys-intf/resource-overlay
-include $(OVERLAY_PATH_SYS_INTF)/$(TARGET_BOARD_PLATFORM)/OverlayBoardConfig.mk
