/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "ITuiComposer.h"
#include "utils.h"
#include "TUILog.h"
#include "secure_memset.h"


#define PIN_TITLE "ENTER PIN"
#define DESC_TITLE "Test Trusted UI®"
#define PIN_LABEL ""

static uint8_t pinData[MAX_INPUT_LEN_BYTES];
static uint32_t pinDataLen = 0;

uint32_t getPinResult(char *msg, uint32_t len)
{
    const char *pinMsg = "Pin is: ";
    uint32_t length = 0;

    if ((msg == NULL) || (len == 0)) {
        TUILOGE("%s: invalid param", __func__);
        return 0;
    }

    if (pinDataLen >= MAX_INPUT_LEN_BYTES) {
        TUILOGE("%s: invalid pin length", __func__);
        return 0;
    }

    pinData[pinDataLen] = '\0';

    secure_memset(msg, 0, len);

    length = strlcpy(msg, pinMsg, len);
    if (length >= len) {
        TUILOGE("%s: truncation happened", __func__);
        return 0;
    }
    length = strlcat(msg, (char *)pinData, len);
    if (length >= len) {
        TUILOGE("%s: truncation happened", __func__);
        return 0;
    }

    return length;
}

int32_t startPinDialog(tuiComposerParam_t *param,
                       qsee_tui_dialog_ret_val_t *status)
{
    qsee_tui_dialog_params_t pin_params = {0};
    uint8_t *logoBuff = NULL;
    uint32_t logoSize = 0;
    uint8_t *indicatorBuff = NULL;
    uint32_t indicatorSize = 0;

    CHECK_STATUS(*status);

    LayoutPage_t *pinPage = getLayout("pin", param->width, param->height);
    if (NULL == pinPage) {
        TUILOGE("%s::%d error getLayout('pin') for pin dialog layout", __func__, __LINE__);
        return TUI_ILLEGAL_INPUT;
    }
    //Clear data from last session
    secure_memset(pinData, 0, sizeof(pinData));
    pinDataLen = 0;

    // TODO: Error handling, load it once, check when these get freed
    logoSize = loadLogo(&logoBuff);

    pin_params.layout_page = pinPage;
    pin_params.dialogType = TUI_DIALOG_PIN;

    pin_params.dialogCommonParams.width = param->width;
    pin_params.dialogCommonParams.height = param->height;

    pin_params.dialogCommonParams.title.pu8Buffer = (uint8_t *)PIN_TITLE;
    pin_params.dialogCommonParams.title.u32Len = strlen(PIN_TITLE);

    pin_params.dialogCommonParams.description.pu8Buffer = (uint8_t *)DESC_TITLE;
    pin_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

    if (param->useSecureIndicator != true) {
        // Display local indicator
        indicatorSize = loadIndicator(&indicatorBuff);
        pin_params.dialogCommonParams.secureIndicatorParams
            .bDisplayGlobalSecureIndicator = false;
        pin_params.dialogCommonParams.secureIndicatorParams.secureIndicator
            .pu8Buffer = indicatorBuff;
        pin_params.dialogCommonParams.secureIndicatorParams.secureIndicator
            .u32Len = indicatorSize;
    } else {
        if (loadSecureIndicator() == 0) {
            // Display global secure indicator
            pin_params.dialogCommonParams.secureIndicatorParams
                .bDisplayGlobalSecureIndicator = true;
        } else {
            TUILOGE("%s::%d Failed to load SecureIndicator", __func__, __LINE__);
            return TUI_SECURE_INDICATOR_ERROR;
        }
    }

    pin_params.dialogCommonParams.logo.pu8Buffer = logoBuff;
    pin_params.dialogCommonParams.logo.u32Len = logoSize;

    pin_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;

    pin_params.pinDialogParams.inpMode = DISP_MODE_VIS_THEN_HID;

    pin_params.pinDialogParams.pin.u32MaxLen = MAX_INPUT_LEN;
    pin_params.pinDialogParams.pin.u32MinLen = MIN_INPUT_LEN;
    pin_params.pinDialogParams.pin.inpValue.pu8Buffer = pinData;
    pin_params.pinDialogParams.pin.inpValue.u32Len = MAX_INPUT_LEN_BYTES;
    pin_params.pinDialogParams.pin.pu32InpValueLen = &pinDataLen;
    pin_params.pinDialogParams.pin.defaultValue.pu8Buffer = pinData;
    pin_params.pinDialogParams.pin.defaultValue.u32Len = pinDataLen;

    pin_params.pinDialogParams.pin.label.pu8Buffer = (uint8_t *)PIN_LABEL;
    pin_params.pinDialogParams.pin.label.u32Len = strlen(PIN_LABEL);

    pin_params.pinDialogParams.keyPadMode = TUI_KEYPAD_RANDOMIZED;
    pin_params.pinDialogParams.bHideInputBox = 0;

    return qsee_tui_dialog(&pin_params, status);
}
