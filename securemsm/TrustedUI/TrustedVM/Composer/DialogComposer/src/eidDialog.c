/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "ITuiComposer.h"
#include "utils.h"
#include "TUILog.h"
#include "TUIHeap.h"
#include "eid_tui.h"

#define MSG_TITLE " "
#define DESC_TITLE " "
#define MSG_RIGHT_BTN_LABEL "OK"
#define MSG_LEFT_BTN_LABEL "CANCEL"
#define MSG_MIDDLE_BTN_LABEL ""


int32_t startEidDialog(tuiComposerParam_t *param,
                       qsee_tui_dialog_ret_val_t *status)
{
    int32_t ret = TUI_SUCCESS;
    qsee_tui_dialog_params_t msg_params = {0};
    char layoutName[MAX_LAYOUT_NAME];
    uint8_t *app_info = NULL;
    uint8_t *prompt_info = NULL;
    int32_t app_info_len, prompt_info_len;
    uint8_t *rsa_exponent = NULL;
    uint8_t *rsa_modules = NULL;

    TUILOGD("%s::%d ", __func__, __LINE__);

    uint8_t *msg = (uint8_t *)tui_malloc(MAX_MESSAGE_LEN);
    if (msg == NULL) {
        TUILOGE("failed to allocate memory for eID message.");
        return TUI_GENERAL_ERROR;
    }

    app_info = msg;
    prompt_info = msg + MAX_APP_INFO;

    int32_t msg_len = loadSecureMessage(app_info, MAX_APP_INFO, prompt_info, MAX_PROMOTE_INFO, &app_info_len, &prompt_info_len);
    if (msg_len < 0) {
        TUILOGE("%s::%d failed to load secure message from eid TA.",
                __func__, __LINE__);
        ret = TUI_GENERAL_ERROR;
        goto ret_handle;
    }

    snprintf(layoutName, MAX_LAYOUT_NAME, "eid_%dx%d", param->width, param->height);
    LayoutPage_t *loginPage = get_layout_by_name_eid(layoutName);
    if (NULL == loginPage) {
        TUILOGE("%s::%d error get_layout_by_name_eid('%s') for login dialog layout",
            __func__, __LINE__, layoutName);
        ret = TUI_ILLEGAL_INPUT;
        goto ret_handle;
    }

    get_eid_rsa_config(&rsa_exponent, &rsa_modules);
    font_mgr_config_rsa(rsa_exponent, rsa_modules);

    TUILOGD("%s::%d get layout (%s) successfully.", __func__, __LINE__, layoutName);

    msg_params.layout_page = loginPage;

    msg_params.dialogType = TUI_DIALOG_MESSAGE;

    msg_params.dialogCommonParams.width = param->width;
    msg_params.dialogCommonParams.height = param->height;

    msg_params.dialogCommonParams.title.pu8Buffer = app_info;
    msg_params.dialogCommonParams.title.u32Len = app_info_len;

    msg_params.dialogCommonParams.description.pu8Buffer = (uint8_t *)DESC_TITLE;
    msg_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

    msg_params.dialogCommonParams.secureIndicatorParams
        .bDisplayGlobalSecureIndicator = false;
    msg_params.dialogCommonParams.secureIndicatorParams.secureIndicator
        .pu8Buffer = NULL;
    msg_params.dialogCommonParams.secureIndicatorParams.secureIndicator
        .u32Len = 0;

    msg_params.dialogCommonParams.logo.pu8Buffer = NULL;
    msg_params.dialogCommonParams.logo.u32Len = 0;

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

    msg_params.msgDialogParams.msg.pu8Buffer = prompt_info;
    msg_params.msgDialogParams.msg.u32Len = prompt_info_len;

    ret = qsee_tui_dialog(&msg_params, status);
ret_handle:
    tui_free(msg);
    TUILOGD("%s::%d ret:%d", __func__, __LINE__, ret);
    return ret;
}
