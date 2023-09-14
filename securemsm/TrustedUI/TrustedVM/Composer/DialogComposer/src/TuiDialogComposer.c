/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif

#include "ITuiComposer.h"

#include "TUILog.h"
#include "memscpy.h"
#include "TUIHeap.h"
#include "timesafe_strncmp.h"
#include "secure_memset.h"
#include "qsee_tui_dialogs.h"
#include "qsee_tui_layout.h"

#define PROFILE_ON

#ifdef PROFILE_ON
#define PROFILE \
      unsigned long long stop;         \
      unsigned long long start;        \
      struct timeval start_time;       \
      unsigned long long delay = 0; \
      struct timeval stop_time;

#define TIME_START                   \
      gettimeofday(&start_time, NULL); \
      start = start_time.tv_usec;

#define TIME_STOP                   \
    gettimeofday(&stop_time, NULL); \
    stop = stop_time.tv_usec;        \
    delay = ((stop_time.tv_sec - start_time.tv_sec) * 1000000) + (stop_time.tv_usec - start_time.tv_usec); \
    TUILOGD("%s::%d- (stop time: %llu micro seconds)", __func__, __LINE__, stop); \
    TUILOGE("%s::%d (time taken = %llu micro seconds)", __func__, __LINE__, delay);
#else
#define PROFILE \
    do {           \
    } while (0)

#define TIME_START \
    do {           \
    } while (0)
#define TIME_STOP \
    do {          \
    } while (0)

#endif


typedef struct screen_surface {
    uint32_t buffer_size;
    uint8_t *buffer;
    uint32_t width;
    uint32_t height;
    uint32_t pixelWidth;
} screen_surface_t;

typedef struct tuiDialogComposerCtx {
    uint32_t width;
    uint32_t stride;
    uint32_t height;
    uint32_t pixelWidth;
    uint32_t size;
    tuiComposerBufferHandle_t appBuffer;
    screen_surface_t renderSurface;
    screen_surface_t helperSurface;
    ITuiComposerCallback_t *callback;
    bool initialized;
} tuiDialogComposerCtx_t;

static qsee_tui_dialog_ret_val_t status = TUI_SUCCESS;

#define MAX_MSG_LEN 300

#define IS_PIN_DIALOG(id) \
    (timesafe_strncmp((id), "pin", (strlen("pin") + 1)) == 0)
#define IS_LOGIN_DIALOG(id) \
    (timesafe_strncmp((id), "login", (strlen("login") + 1)) == 0)
#define IS_CREDIT_CARD_DIALOG(id) \
    (timesafe_strncmp((id), "credit_card", (strlen("credit_card") + 1)) == 0)
#define IS_MSG_LOGIN_DIALOG(id) \
    (timesafe_strncmp((id), "msg_login", (strlen("msg_login") + 1)) == 0)
#define IS_MSG_PIN_DIALOG(id) \
    (timesafe_strncmp((id), "msg_pin", (strlen("msg_pin") + 1)) == 0)
#define IS_MSG_CREDIT_CARD_DIALOG(id)          \
    (timesafe_strncmp((id), "credit_card_msg", \
                      (strlen("credit_card_msg") + 1)) == 0)
#define IS_EID_DIALOG(id)          \
    (timesafe_strncmp((id), "eid", \
                      (strlen("eid") + 1)) == 0)

extern layout_mgr_err_t layout_mgr_render_layout(
    const screen_surface_t *surface);
extern layout_mgr_err_t layout_mgr_render_bg_image(
    const screen_surface_t *surface);

extern tuiComposerInputStatus_t manage_layout_event(
    tuiComposerInputCommand_t cmd, tuiComposerTouchInput_t *fingers,
    int32_t *timeout);

extern int32_t startPinDialog(tuiComposerParam_t *param,
                              qsee_tui_dialog_ret_val_t *status);
extern int32_t startLoginDialog(tuiComposerParam_t *param,
                                qsee_tui_dialog_ret_val_t *status);
extern int32_t startMsgDialog(tuiComposerParam_t *param,
                              qsee_tui_dialog_ret_val_t *status, uint8_t *msg,
                              uint32_t msg_len);
extern int32_t startCustomCreditCardDialog(tuiComposerParam_t *param,
                                           qsee_tui_dialog_ret_val_t *status);

extern int32_t startEidDialog(tuiComposerParam_t *param,
                              qsee_tui_dialog_ret_val_t *status);

extern uint32_t getPinResult(char *msg, uint32_t len);
extern uint32_t getLoginResult(char *msg, uint32_t len);
extern uint32_t getCustomResult(char *msg, uint32_t len);

/**
 * _stopDialog: start requested dialog.
 */
static int32_t _startDialog(tuiComposerParam_t *param)
{
    int32_t ret = 0;
    uint8_t msg[MAX_MSG_LEN];
    uint32_t len = MAX_MSG_LEN;
    qsee_tui_init();

    if (IS_PIN_DIALOG(param->layoutId)) {
        ret = startPinDialog(param, &status);
    } else if (IS_LOGIN_DIALOG(param->layoutId)) {
        ret = startLoginDialog(param, &status);
    } else if (IS_CREDIT_CARD_DIALOG(param->layoutId)) {
        ret = startCustomCreditCardDialog(param, &status);
    } else if (IS_MSG_LOGIN_DIALOG(param->layoutId)) {
        len = getLoginResult((char *)msg, len);
        if (len == 0) {
            ret = -1;
            goto EXIT;
        }
        ret = startMsgDialog(param, &status, msg, len);
    } else if (IS_MSG_PIN_DIALOG(param->layoutId)) {
        len = getPinResult((char *)msg, len);
        if (len == 0) {
            ret = -1;
            goto EXIT;
        }
        ret = startMsgDialog(param, &status, msg, len);
    } else if (IS_MSG_CREDIT_CARD_DIALOG(param->layoutId)) {
        len = getCustomResult((char *)msg, len);
        if (len == 0) {
            ret = -1;
            goto EXIT;
        }
        ret = startMsgDialog(param, &status, msg, len);
    } else if (IS_EID_DIALOG(param->layoutId)) {
        ret = startEidDialog(param, &status);
    } else {
        ret = -1;
        TUILOGE( "%s::%d unsupported dialog", __func__, __LINE__);
    }

EXIT:
    TUILOGD("%s::%d _startDialog, ret : %d", __func__, __LINE__, ret);
    if(status != TUI_SUCCESS) {
       TUILOGE( "%s::%d status = %d is being reset", __func__, __LINE__, status);
       status = TUI_SUCCESS;
    }
    return ret;
}

/**
 * _stopDialog: stop dialog.
 */
static void _stopDialog(void)
{
    qsee_tui_tear_down();
    qsee_tui_reset_secure_indicator();
    unLoadDialogResources();
}

/**
 * _copy_data: Copy data from one surface to another.
 */
static void _copy_data(screen_surface_t *dstSurf,
                       const screen_surface_t *srcSurf)
{
    uint32_t srcWidthSize = srcSurf->width * srcSurf->pixelWidth;
    TUILOGD("%s::%d (%u, %u, %u, %u)", __func__, __LINE__,
             srcSurf->width * srcSurf->pixelWidth, srcSurf->height, srcWidthSize);
    if (srcWidthSize / srcSurf->width != srcSurf->pixelWidth) {
        TUILOGE("%s: overflow detected src wxd (%d, %d)",
                 __func__, srcSurf->width, srcSurf->pixelWidth);
        return;
    }
    uint32_t dstWidthSize = dstSurf->width * dstSurf->pixelWidth;
    TUILOGD("%s::%d (%u, %u, %u)", __func__, __LINE__,
             dstSurf->width * dstSurf->pixelWidth, dstWidthSize);
    if (dstWidthSize / dstSurf->width != dstSurf->pixelWidth) {
        TUILOGE("%s: overflow detected dst wxd (%d, %d)",
                 __func__, dstSurf->width, dstSurf->pixelWidth);
        return;
    }

    for (uint32_t row = 0; row < srcSurf->height; row++) {
        memscpy((void *)(&(dstSurf->buffer[row * dstWidthSize])), dstWidthSize,
                (void *)(&(srcSurf->buffer[row * srcWidthSize])), srcWidthSize);
    }
}

/**
 * _getWidth: Get pixel width in bytes.
 */
static uint32_t _getWidth(tuiComposerFormat_t format)
{
    uint32_t width = 0;

    switch (format) {
        case TUI_COMPOSER_FORMAT_RGBA:
            width = 4;
            break;
        case TUI_COMPOSER_FORMAT_MAX:
        default:
            width = 0;
            break;
    }

    return width;
}

static int32_t _getEidConfirmation(qsee_tui_dialog_ret_val_t val)
{
    int32_t ret = SECUIEID_GENERAL_ERROR;

    TUILOGD("%s::%d ", __func__, __LINE__);
    switch(val) {
        case TUI_GENERAL_ERROR:
            TUILOGE("%s::%d Unexpected result: %d", __func__, __LINE__, TUI_GENERAL_ERROR);
            break;
        case TUI_SECURE_INDICATOR_ERROR:
            TUILOGE("%s::%d Unexpected result: %d", __func__, __LINE__, TUI_SECURE_INDICATOR_ERROR);
            break;
        case TUI_LAYOUT_ERROR:
            TUILOGE("%s::%d Unexpected result: %d", __func__, __LINE__, TUI_LAYOUT_ERROR);
            break;
        case TUI_ILLEGAL_INPUT:
            TUILOGE("%s::%d Unexpected result: %d", __func__, __LINE__, TUI_ILLEGAL_INPUT);
            break;
        case TUI_SUCCESS:
            TUILOGE("%s::%d Unexpected result: %d", __func__, __LINE__, TUI_SUCCESS);
            break;
        case TUI_MIDDLE_PRESSED:
            TUILOGE("%s::%d Unexpected result: %d", __func__, __LINE__, TUI_MIDDLE_PRESSED);
            break;
        case TUI_OK_PRESSED:
            TUILOGD("%s::%d ENTER button was pressed: TUI_OK_PRESSED", __func__, __LINE__);
            ret = SECUIEID_ENTER_PRESSED;
            break;
        case TUI_RIGHT_PRESSED:
            TUILOGD("%s::%d ENTER button was pressed: TUI_RIGHT_PRESSED", __func__, __LINE__);
            ret = SECUIEID_ENTER_PRESSED;
            break;
        case TUI_CANCEL_PRESSED:
            TUILOGD("%s::%d CANCEL button was pressed: TUI_CANCEL_PRESSED", __func__, __LINE__);
            ret = SECUIEID_CANCEL_PRESSED;
            break;
        case TUI_LEFT_PRESSED:
            TUILOGD("%s::%d CANCEL button was pressed: TUI_LEFT_PRESSED", __func__, __LINE__);
            ret = SECUIEID_CANCEL_PRESSED;
            break;
        default:
            ret = SECUIEID_GENERAL_ERROR;
            break;
    }

    return ret;
}

/**
 * dialogComposerInit:
 *
 * Initialise composer for requested layout.
 *
 * @param ctx Composer context.
 * @param param Composer init params {@link tuiComposerParam_t}
 *
 * @return status Return status of this operation:
 *     TUI_COMPOSER_STATUS_OK:
 *         Init completed
 *     TUI_COMPOSER_STATUS_BAD_PARAM:
 *         Invalid parameter
 *     TUI_COMPOSER_STATUS_FAILED:
 *         Init failed
 *
 */
static tuiComposerStatus_t dialogComposerInit(void *ctx,
                                              tuiComposerParam_t *param,
                                              ITuiComposerCallback_t *cb)
{
    tuiComposerStatus_t status = TUI_COMPOSER_STATUS_OK;
    tuiDialogComposerCtx_t *ComposerCtx = NULL;
    int32_t retVal = 0;
    uint32_t tmpSize = 0, screenSize = 0;
    uint32_t pixelWidth = 0;
    if ((ctx == NULL) || (param == NULL) || (cb == NULL)) {
        TUILOGE( "%s::%d invalid params passed\n", __func__, __LINE__);
        return TUI_COMPOSER_STATUS_BAD_PARAM;
    }
    ComposerCtx = (tuiDialogComposerCtx_t *)ctx;
    ComposerCtx->callback = cb;
    pixelWidth = _getWidth(param->format);
    tmpSize = param->width * param->height;
    if (tmpSize / param->width != param->height) {
        TUILOGE("%s::%d overflow detected wxh (%d, %d)",
                 __func__, __LINE__, param->width, param->height);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }
    ComposerCtx->appBuffer.size = tmpSize;
    tmpSize *= pixelWidth;
    if (tmpSize / pixelWidth != ComposerCtx->appBuffer.size) {
        TUILOGE("%s::%d overflow detected rxd (%d, %d)",
                 __func__, __LINE__, ComposerCtx->appBuffer.size, pixelWidth);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }
    screenSize = tmpSize;
    ComposerCtx->appBuffer.size = screenSize;

    tmpSize *= 2;
    if (tmpSize / 2 != ComposerCtx->appBuffer.size) {
        TUILOGE("%s: overflow detected size/2 (%d)",
                 __func__, ComposerCtx->appBuffer.size);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }
    ComposerCtx->appBuffer.size = tmpSize;

    if (cb->handleCmd) {
        font_mgr_register_callback(cb->handleCmd);
    }

    if (cb->allocHelperBuff) {
        TUILOGD("%s::%d allocate helper buffer of size:%d\n", __func__, __LINE__,
                ComposerCtx->appBuffer.size);
        status = cb->allocHelperBuff(cb->cbData, &ComposerCtx->appBuffer);
        if (status != TUI_COMPOSER_STATUS_OK) {
            TUILOGE("%s::%d failed to allocate helper buffer\n", __func__, __LINE__);
            ComposerCtx->callback = NULL;
            ComposerCtx->appBuffer.size = 0;
            goto EXIT;
        }
    }
    else{
        TUILOGE("%s::%d allocate helper buffer is nullptr\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    status = _startDialog(param);
    if (status != TUI_COMPOSER_STATUS_OK) {
        TUILOGE("%s::%d _startDialog failed\n", __func__, __LINE__);
        goto EXIT;
    }

    ComposerCtx->size = screenSize;
    ComposerCtx->width = param->width;
    ComposerCtx->height = param->height;
    ComposerCtx->pixelWidth = _getWidth(param->format);

    ComposerCtx->renderSurface.buffer_size = ComposerCtx->size;
    ComposerCtx->renderSurface.buffer = ComposerCtx->appBuffer.vAddr;
    ComposerCtx->renderSurface.width = param->width;
    ComposerCtx->renderSurface.height = param->height;
    ComposerCtx->renderSurface.pixelWidth = ComposerCtx->pixelWidth;

    if (ComposerCtx->appBuffer.size == 2 * ComposerCtx->size) {
        ComposerCtx->helperSurface.buffer_size = ComposerCtx->size;
        ComposerCtx->helperSurface.buffer =
            ComposerCtx->appBuffer.vAddr + ComposerCtx->size;
        ComposerCtx->helperSurface.width = param->width;
        ComposerCtx->helperSurface.height = param->height;
        ComposerCtx->helperSurface.pixelWidth = ComposerCtx->pixelWidth;
    } else {
        ComposerCtx->helperSurface.buffer = NULL;
        ComposerCtx->helperSurface.buffer_size = 0;
    }

    retVal = layout_mgr_render_layout(&ComposerCtx->renderSurface);
    if (retVal < 0) {
        TUILOGE("%s::%d layout_mgr_render_layout failed  %d",
                 __func__, __LINE__, retVal);
        status = TUI_COMPOSER_STATUS_INVALID_LAYOUT;
        goto EXIT;
    }

    if (ComposerCtx->helperSurface.buffer) {
        TUILOGD("%s::%d Render helper surface", __func__, __LINE__);
        retVal = layout_mgr_render_bg_image(&ComposerCtx->helperSurface);
        if (retVal < 0) {
            TUILOGE("%s::%d layout_mgr_render_bg_image returned %d", __func__, __LINE__,
                    retVal);
            status = TUI_COMPOSER_STATUS_FAILED;
            goto EXIT;
        }
    }

    ComposerCtx->initialized = true;

EXIT:
    if (status != TUI_COMPOSER_STATUS_OK) {
        (void)_stopDialog();
        if (cb && cb->freeHelperBuff && ComposerCtx->appBuffer.size) {
            (void)cb->freeHelperBuff(cb->cbData, &ComposerCtx->appBuffer);
        }
    }
    return status;
}

/**
 * dialogComposerCompose:
 *
 * Process user input and compose a graphics buffer in accordance with selected
 * layout to be presented on display.
 *
 * @param ctx Composer context
 * @param input User input {@link tuiComposerInput_t}
 * @param buffer Display buffer to render into {@link tuiComposerBufferHandle_t}
 *
 * @return status Return status of this operation:
 *     TUI_COMPOSER_STATUS_OK:
 *         Init completed
 *     TUI_COMPOSER_STATUS_BAD_PARAM:
 *         Invalid parameter
 *     TUI_COMPOSER_STATUS_FAILED:
 *         Init failed
 *
 */
static tuiComposerStatus_t dialogComposerCompose(
    void *ctx, tuiComposerInput_t *input, tuiComposerDisplayBuffer_t *buffer)
{
    tuiComposerStatus_t status = TUI_COMPOSER_STATUS_OK;
    tuiDialogComposerCtx_t *ComposerCtx = NULL;
    int32_t retVal = 0;
    screen_surface_t displaySurface;
    PROFILE;

    if ((ctx == NULL) || (input == NULL) || (buffer == NULL)) {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    if (input->type != TUI_COMPOSER_INPUT_TOUCH) {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    ComposerCtx = (tuiDialogComposerCtx_t *)ctx;

    if (!ComposerCtx->initialized) {
        TUILOGE("%s::%d init not done\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_NO_INIT;
        goto EXIT;
    }

    TIME_START;
    /* Prepare display surface */
    displaySurface.buffer_size = buffer->handle.size;
    displaySurface.buffer = buffer->handle.vAddr;
    displaySurface.width = buffer->config.stride;
    displaySurface.height = buffer->config.height;
    displaySurface.pixelWidth = _getWidth(buffer->config.format);

    /* Handle supported commands */
    if (input->cmd == TUI_COMPOSER_INPUT_CMD_START) {
        _copy_data(&displaySurface, &ComposerCtx->renderSurface);
        input->timeOut = -1;
        input->status = TUI_COMPOSER_INPUT_STATUS_CONTINUE;
        goto EXIT;
    } else if ((input->cmd == TUI_COMPOSER_INPUT_CMD_DATA) ||
               (input->cmd == TUI_COMPOSER_INPUT_CMD_TIMEOUT)) {
        /* Process secure input as per selected layout */
        input->status = manage_layout_event(input->cmd, &input->data.touch,
                                            &input->timeOut);
    } else {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    /* Check if input was valid, if so, render new content to be displayed */
    if (input->status == TUI_COMPOSER_INPUT_STATUS_CONTINUE) {
        layout_mgr_err_t layout_mgr_retval;

        retVal = layout_mgr_render_layout(&ComposerCtx->renderSurface);
        if (retVal == LAYOUT_MGR_UNCHANGED) {
            TUILOGE("%s::%d layout_mgr_render_layout unchanged", __func__, __LINE__);
            _copy_data(&displaySurface, &ComposerCtx->renderSurface);
            input->status = TUI_COMPOSER_INPUT_STATUS_CONTINUE;
            goto EXIT;
        }

        if (LAYOUT_MGR_TEXT_EXCEED_OBJECT == retVal) {
            TUILOGE("%s::%d layout_mgr_render_layout failed, "
                     "text exceeded the object, trying again...",
                     __func__, __LINE__);
            layout_mgr_retval = layout_mgr_delete_input_last_character();
            if (layout_mgr_retval < 0) {
                TUILOGE("%s::%d layout_mgr_delete_input_last_character"
                         " returned  %d",
                         __func__, __LINE__, layout_mgr_retval);
                input->status = TUI_COMPOSER_INPUT_STATUS_CANCEL;
                goto EXIT;
            }
            retVal = layout_mgr_render_layout(&ComposerCtx->renderSurface);
        }

        if (retVal < 0) {
            TUILOGE("%s::%d layout_mgr_render_layout failed  %d", __func__, __LINE__,
                     retVal);
            input->status = TUI_COMPOSER_INPUT_STATUS_CANCEL;
            goto EXIT;
        }

        _copy_data(&displaySurface, &ComposerCtx->renderSurface);
    }

EXIT:
    TIME_STOP;
    return status;
}

/**
 * dialogComposerSendCommand:
 *
 * Send custom command to composer.
 *
 * @param ctx Composer context.
 * @param cmd Command identifier.
 * @param ctx Command payload structure.
 *
 * @return result Session result.
 *
 */
tuiComposerStatus_t dialogComposerSendCommand(void *ctx, uint32_t cmd,
                                              tuiComposerCmdPayload_t *payload)
{
    tuiComposerStatus_t ret = TUI_COMPOSER_STATUS_OK;

    TUILOGD("%s::%d ", __func__, __LINE__);

    if (ctx == NULL || payload == NULL) {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        ret = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    if (payload->respPtr  == NULL) {
        TUILOGE("%s::%d respPtr is null.\n", __func__, __LINE__);
        ret = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    char *rsp = payload->respPtr;

    uint32_t cmdId = cmd & 0x0000FFFF;
    if (cmdId == TUI_CMD_GET_STATUS) {
        if (payload->respSize >= 4) {
            int32_t dialog_retval = _getEidConfirmation(status);
            rsp[0] = (char)(dialog_retval >> 24);
            rsp[1] = (char)(dialog_retval >> 16);
            rsp[2] = (char)(dialog_retval >> 8);
            rsp[3] = (char)dialog_retval;
        } else {
            ret = TUI_COMPOSER_STATUS_BAD_PARAM;
            TUILOGE("%s::%d response size(%d) is less than 4\n", __func__, __LINE__, payload->respSize);
        }
    } else {
        TUILOGE("%s::%d command(0x%x) is not supported.\n", __func__, __LINE__, cmd);
        ret = TUI_COMPOSER_STATUS_BAD_PARAM;
    }

    return ret;
EXIT:
        return ret;
}

/**
 * dialogComposerDeInit:
 *
 * De-initialise composer.
 *
 * @param ctx Composer context
 *
 * @return status Return status of this operation:
 *     TUI_COMPOSER_STATUS_OK:
 *         deInit completed
 *     TUI_COMPOSER_STATUS_BAD_PARAM:
 *         Invalid parameter
 *     TUI_COMPOSER_STATUS_FAILED:
 *         deInit failed
 *
 */
static tuiComposerStatus_t dialogComposerDeInit(void *ctx)
{
    tuiComposerStatus_t status = TUI_COMPOSER_STATUS_OK;
    tuiDialogComposerCtx_t *ComposerCtx = NULL;
    ITuiComposerCallback_t *callback = NULL;

    if (ctx == NULL) {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    ComposerCtx = (tuiDialogComposerCtx_t *)ctx;
    if (!ComposerCtx->initialized) {
        TUILOGE("%s::%d init not done\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_NO_INIT;
        goto EXIT;
    }

    callback = ComposerCtx->callback;
    _stopDialog();

    if (callback && callback->freeHelperBuff && ComposerCtx->appBuffer.size) {
        status =
            callback->freeHelperBuff(callback->cbData, &ComposerCtx->appBuffer);
        if (status != TUI_COMPOSER_STATUS_OK) {
            TUILOGE("%s::%d failed to free helper buffer\n",
                     __func__, __LINE__);
            goto EXIT;
        }
    }
    ComposerCtx->initialized = false;

EXIT:
    return status;
}

/**
 * setupDialogComposer:
 *
 * Setup ITuiComposer object for TUI_COMPOSER_TYPE_DIALOG.
 *
 * @param type Composer object to be initialised
 *
 * @return none
 *
 */
void setupDialogComposer(ITuiComposer_t *composer)
{
    tuiDialogComposerCtx_t *ctx = NULL;
    if ((composer == NULL) || (composer->type != TUI_COMPOSER_TYPE_DIALOG)) {
        TUILOGE("%s::%d invalid param\n", __func__, __LINE__);
        return;
    }

    ctx = tui_malloc(sizeof(tuiDialogComposerCtx_t));
    if (ctx == NULL) {
        TUILOGE("%s::%d failed to create composer context\n",
                 __func__, __LINE__);
        return;
    }

    secure_memset(ctx, 0, sizeof(tuiDialogComposerCtx_t));

    composer->init = dialogComposerInit;
    composer->compose = dialogComposerCompose;
    composer->sendCommand = dialogComposerSendCommand;
    composer->deInit = dialogComposerDeInit;

    composer->ctx = (void *)ctx;
}
