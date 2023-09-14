/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif

#include "ITuiComposer.h"
#include "utils.h"
#include "TUILog.h"
#include "secure_memset.h"

#define LOGIN_TITLE "SECURE LOGIN"
#define DESC_TITLE "Test Secure UI®"

#define PASSWORD_LABEL "PASSWORD"
#define USERNAME_LABEL "USERNAME"
#define DEFAULT_USER "sample user"

static uint8_t received_username[MAX_INPUT_LEN_BYTES];
static uint32_t received_username_len = 0;
static uint8_t received_password[MAX_INPUT_LEN_BYTES];
static uint32_t received_password_len = 0;

uint32_t getLoginResult(char *msg, uint32_t len)
{
    const char *usrMsg = "Username is: ";
    const char *passMsg = " Password is: ";
    uint32_t length = 0;

    if ((msg == NULL) || (len == 0)) {
        TUILOGE("%s: invalid param", __func__);
        return 0;
    }

    if ((received_username_len >= MAX_INPUT_LEN_BYTES) ||
        (received_password_len >= MAX_INPUT_LEN_BYTES)) {
        TUILOGE("%s: invalid login/passwd length", __func__);
        return 0;
    }

    received_username[received_username_len] = '\0';
    received_password[received_password_len] = '\0';

    secure_memset(msg, 0, len);

    length = strlcpy(msg, usrMsg, len);
    if (length >= len) {
        TUILOGE("%s: truncation happened", __func__);
        return 0;
    }
    length = strlcat(msg, (char *)received_username, len);
    if (length >= len) {
        TUILOGE("%s: truncation happened", __func__);
        return 0;
    }
    length = strlcat(msg, passMsg, len);
    if (length >= len) {
        TUILOGE("%s: truncation happened", __func__);
        return 0;
    }
    length = strlcat(msg, (char *)received_password, len);
    if (length >= len) {
        TUILOGE("%s: truncation happened", __func__);
        return 0;
    }

    return length;
}

int32_t startLoginDialog(tuiComposerParam_t *param,
                         qsee_tui_dialog_ret_val_t *status)
{
    qsee_tui_dialog_params_t login_params = {0};
    uint8_t *logoBuff = NULL;
    uint32_t logoSize = 0;
    uint8_t *indicatorBuff = NULL;
    uint32_t indicatorSize = 0;

    CHECK_STATUS(*status);

    // load dialog based on display resolution. get_layout_by_name() can be
    // used directly if the layout name is known at compile time
    LayoutPage_t *loginPage =
        getLayout("login_double", param->width, param->height);
    if (NULL == loginPage) {
        TUILOGE("%s::%d error getLayout('login_double') for login dialog layout",
            __func__, __LINE__);
        return TUI_ILLEGAL_INPUT;
    }
    //Clear Data from last session
    secure_memset(received_password, 0, sizeof(received_password));
    received_password_len = 0;

    // TODO: Error handling, load it once; check when these get freed
    logoSize = loadLogo(&logoBuff);

    login_params.layout_page = loginPage;

    login_params.dialogType = TUI_DIALOG_PASSWORD;

    login_params.dialogCommonParams.width = param->width;
    login_params.dialogCommonParams.height = param->height;

    login_params.dialogCommonParams.title.pu8Buffer = (uint8_t *)LOGIN_TITLE;
    login_params.dialogCommonParams.title.u32Len = strlen(LOGIN_TITLE);

    login_params.dialogCommonParams.description.pu8Buffer =
        (uint8_t *)DESC_TITLE;
    login_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

    if (param->useSecureIndicator != true) {
        // display local indicator
        indicatorSize = loadIndicator(&indicatorBuff);
        login_params.dialogCommonParams.secureIndicatorParams
            .bDisplayGlobalSecureIndicator = false;
        login_params.dialogCommonParams.secureIndicatorParams.secureIndicator
            .pu8Buffer = indicatorBuff;
        login_params.dialogCommonParams.secureIndicatorParams.secureIndicator
            .u32Len = indicatorSize;
    } else {
        if (loadSecureIndicator() == 0) {
            // display global secure indicator
            login_params.dialogCommonParams.secureIndicatorParams
                .bDisplayGlobalSecureIndicator = true;
        } else {
            TUILOGE("%s::%d Failed to load SecureIndicator", __func__, __LINE__);
            return TUI_SECURE_INDICATOR_ERROR;
        }
    }

    login_params.dialogCommonParams.logo.pu8Buffer = logoBuff;
    login_params.dialogCommonParams.logo.u32Len = logoSize;

    login_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;

    login_params.pswdDialogParams.inpMode = DISP_MODE_VIS_THEN_HID;
    login_params.pswdDialogParams.bUserNameInpShow = 1;
    login_params.pswdDialogParams.bPasswordInpShow = 1;

    login_params.pswdDialogParams.username.u32MaxLen = MAX_INPUT_LEN;
    login_params.pswdDialogParams.username.u32MinLen = MIN_INPUT_LEN;
    login_params.pswdDialogParams.username.inpValue.pu8Buffer =
        received_username;
    login_params.pswdDialogParams.username.inpValue.u32Len =
        MAX_INPUT_LEN_BYTES;
    login_params.pswdDialogParams.username.pu32InpValueLen =
        &received_username_len;
    login_params.pswdDialogParams.username.defaultValue.pu8Buffer =
        (uint8_t *)DEFAULT_USER;
    login_params.pswdDialogParams.username.defaultValue.u32Len =
        strlen(DEFAULT_USER);
    login_params.pswdDialogParams.username.label.pu8Buffer =
        (uint8_t *)USERNAME_LABEL;
    login_params.pswdDialogParams.username.label.u32Len =
        strlen(USERNAME_LABEL);

    login_params.pswdDialogParams.password.u32MaxLen = MAX_INPUT_LEN;
    login_params.pswdDialogParams.password.u32MinLen = MIN_INPUT_LEN;
    login_params.pswdDialogParams.password.inpValue.pu8Buffer =
        received_password;
    login_params.pswdDialogParams.password.inpValue.u32Len =
        MAX_INPUT_LEN_BYTES;
    login_params.pswdDialogParams.password.pu32InpValueLen =
        &received_password_len;
    login_params.pswdDialogParams.password.defaultValue.pu8Buffer =
        received_password;
    login_params.pswdDialogParams.password.defaultValue.u32Len =
        received_password_len;
    login_params.pswdDialogParams.password.label.pu8Buffer =
        (uint8_t *)PASSWORD_LABEL;
    login_params.pswdDialogParams.password.label.u32Len =
        strlen(PASSWORD_LABEL);

    return qsee_tui_dialog(&login_params, status);
}
