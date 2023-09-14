/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "ITuiComposer.h"
#include "utils.h"
#include "TUILog.h"

#define MSG_TITLE "SECURE MESSAGE"
#define DESC_TITLE "Test Trusted UI®"
#define MSG_RIGHT_BTN_LABEL "ok"
#define MSG_LEFT_BTN_LABEL "cancel"
#define MSG_MIDDLE_BTN_LABEL ""

int32_t startMsgDialog(tuiComposerParam_t *param,
                       qsee_tui_dialog_ret_val_t *status, uint8_t *msg,
                       uint32_t msg_len)
{
    qsee_tui_dialog_params_t msg_params = {0};
    uint8_t *logoBuff = NULL;
    uint32_t logoSize = 0;
    uint8_t *indicatorBuff = NULL;
    uint32_t indicatorSize = 0;

    CHECK_STATUS(*status);

    if (NULL == msg) {
        return TUI_ILLEGAL_INPUT;
    }

    // load dialog based on display resolution. get_layout_by_name() can be
    // used directly if the layout name is known at compile time
    LayoutPage_t *loginPage =
        getLayout("message_double", param->width, param->height);
    if (NULL == loginPage) {
        TUILOGE("%s::%d error getLayout('message_double') for login dialog layout",
            __func__, __LINE__);
        return TUI_ILLEGAL_INPUT;
    }

    // TODO: Error handling, load it once, check when these get freed
    logoSize = loadLogo(&logoBuff);

    msg_params.layout_page = loginPage;

    msg_params.dialogType = TUI_DIALOG_MESSAGE;

    msg_params.dialogCommonParams.width = param->width;
    msg_params.dialogCommonParams.height = param->height;

    msg_params.dialogCommonParams.title.pu8Buffer = (uint8_t *)MSG_TITLE;
    msg_params.dialogCommonParams.title.u32Len = strlen(MSG_TITLE);

    msg_params.dialogCommonParams.description.pu8Buffer = (uint8_t *)DESC_TITLE;
    msg_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

    if (param->useSecureIndicator != true) {
        // display local indicator
        indicatorSize = loadIndicator(&indicatorBuff);
        msg_params.dialogCommonParams.secureIndicatorParams
            .bDisplayGlobalSecureIndicator = false;
        msg_params.dialogCommonParams.secureIndicatorParams.secureIndicator
            .pu8Buffer = indicatorBuff;
        msg_params.dialogCommonParams.secureIndicatorParams.secureIndicator
            .u32Len = indicatorSize;
    } else {
        if (loadSecureIndicator() == 0) {
            // display global secure indicator
            msg_params.dialogCommonParams.secureIndicatorParams
                .bDisplayGlobalSecureIndicator = true;
        } else {
            TUILOGE("%s::%d Failed to load SecureIndicator", __func__, __LINE__);
            return TUI_SECURE_INDICATOR_ERROR;
        }
    }

    msg_params.dialogCommonParams.logo.pu8Buffer = logoBuff;
    msg_params.dialogCommonParams.logo.u32Len = logoSize;

    msg_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;

    msg_params.msgDialogParams.rightBtnParams.btnLabel.pu8Buffer =
        (uint8_t *)MSG_RIGHT_BTN_LABEL;
    msg_params.msgDialogParams.rightBtnParams.btnLabel.u32Len =
        strlen(MSG_RIGHT_BTN_LABEL);
    msg_params.msgDialogParams.rightBtnParams.bShowBtn = 1;
    msg_params.msgDialogParams.rightBtnParams.bDisableBtn = 0;

    msg_params.msgDialogParams.leftBtnParams.btnLabel.pu8Buffer =
        (uint8_t *)MSG_LEFT_BTN_LABEL;
    msg_params.msgDialogParams.leftBtnParams.btnLabel.u32Len =
        strlen(MSG_LEFT_BTN_LABEL);
    msg_params.msgDialogParams.leftBtnParams.bShowBtn = 1;
    msg_params.msgDialogParams.leftBtnParams.bDisableBtn = 0;

    msg_params.msgDialogParams.middleBtnParams.bShowBtn = 0;
    msg_params.msgDialogParams.middleBtnParams.btnLabel.pu8Buffer =
        (uint8_t *)MSG_MIDDLE_BTN_LABEL;
    msg_params.msgDialogParams.middleBtnParams.btnLabel.u32Len =
        strlen(MSG_MIDDLE_BTN_LABEL);
    msg_params.msgDialogParams.middleBtnParams.bDisableBtn = 0;

    msg_params.msgDialogParams.msg.pu8Buffer = msg;
    msg_params.msgDialogParams.msg.u32Len = msg_len;

    return qsee_tui_dialog(&msg_params, status);
}