/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif

#include "ITuiComposer.h"
#include "TUILog.h"
#include "memscpy.h"
#include "TUIHeap.h"
#include "secure_memset.h"


typedef struct screen_surface {
    uint32_t buffer_size;
    uint8_t *buffer;
    uint32_t width;
    uint32_t height;
    uint32_t pixelWidth;
} screen_surface_t;

typedef struct touch_event {
    uint32_t x;
    uint32_t y;
    uint8_t *dot;
} touch_event_t;

typedef struct tuiCustomComposerCtx {
    uint32_t width;
    uint32_t stride;
    uint32_t height;
    uint32_t pixelWidth;
    uint32_t size;
    tuiComposerBufferHandle_t appBuffer;
    screen_surface_t renderSurface;
    ITuiComposerCallback_t *callback;
    bool initialized;
} tuiCustomComposerCtx_t;

#define TOP_EXIT_BAR 100
#define BYTES_PER_PIXEL 4
#define DOT_SIZE 10
#define RED_PIXEL 0xff0000ff
#define GREEN_PIXEL 0xff00ff00

static uint8_t redDot[DOT_SIZE * BYTES_PER_PIXEL];
static uint8_t greenDot[DOT_SIZE * BYTES_PER_PIXEL];

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
        TUILOGE("%s::%d overflow detected src wxd (%d, %d)",
                 __func__, __LINE__, srcSurf->width, srcSurf->pixelWidth);
        return;
    }
    uint32_t dstWidthSize = dstSurf->width * dstSurf->pixelWidth;
    TUILOGD("%s::%d (%u, %u, %u)", __func__, __LINE__,
             dstSurf->width * dstSurf->pixelWidth, dstWidthSize);
    if (dstWidthSize / dstSurf->width != dstSurf->pixelWidth) {
        TUILOGE("%s::%d overflow detected dst wxd (%d, %d)",
                 __func__, __LINE__, dstSurf->width, dstSurf->pixelWidth);
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

static void _copyDot(screen_surface_t *surf, touch_event_t *event)
{
    size_t y = 0;
    size_t dotWidth = 0;

    uint32_t xSize = surf->width * surf->pixelWidth;
    TUILOGD("%s::%d (%u, %u, %u)", __func__, __LINE__,
             surf->width, surf->pixelWidth, xSize);
    if (xSize / surf->width != surf->pixelWidth) {
        TUILOGE("%s::%d overflow detected src wxd (%d, %d)",
                 __func__,__LINE__, surf->width, surf->pixelWidth);
        return;
    }

    if (surf->width - event->x > DOT_SIZE) {
        dotWidth = DOT_SIZE;
    } else {
        dotWidth = surf->width - event->x;
    }

    TUILOGD("%s::%d (%u,%u) onto buffer 0x%x", __func__, __LINE__,
             event->x, event->y, surf->buffer);
    TUILOGD("%s::%d (%u,%u)", __func__, __LINE__,
             surf->pixelWidth, surf->width);

    for (y = event->y; (y < event->y + DOT_SIZE) && (y < surf->height); y++) {
        memscpy(
            surf->buffer + (y * surf->width + event->x) * surf->pixelWidth,
            surf->buffer_size - (y * surf->width + event->x) * surf->pixelWidth,
            event->dot, dotWidth * BYTES_PER_PIXEL);
    }
}

static tuiComposerInputStatus_t _drawDot(tuiComposerTouchInput_t *input,
                                         screen_surface_t *surf)
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    size_t current = 0;

    for (j = 0; j < MAX_FINGER_NUM; j++) {
        if (input->finger[j].event != TLAPI_TOUCH_EVENT_NONE) {
            touch_event_t event = {0};
            TUILOGD("%s::%d Position: (%u, %u)", __func__, __LINE__,
                     input->finger[j].xCoOrdinate,
                     input->finger[j].yCoOrdinate);
            if (input->finger[j].yCoOrdinate < TOP_EXIT_BAR) {
                return TUI_COMPOSER_INPUT_STATUS_COMPLETE;
            }
            event.x = input->finger[j].xCoOrdinate;
            event.y = input->finger[j].yCoOrdinate;

            if (input->finger[j].event & TLAPI_TOUCH_EVENT_UP) {
                event.dot = greenDot;
            } else {
                event.dot = redDot;
            }
            _copyDot(surf, &event);
        }
    }

    return TUI_COMPOSER_INPUT_STATUS_CONTINUE;
}

/**
 * customComposerInit:
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
static tuiComposerStatus_t customComposerInit(void *ctx,
                                              tuiComposerParam_t *param,
                                              ITuiComposerCallback_t *cb)
{
    tuiComposerStatus_t status = TUI_COMPOSER_STATUS_OK;
    tuiCustomComposerCtx_t *ComposerCtx = NULL;
    int32_t retVal = 0;
    uint32_t tmpSize = 0;
    uint32_t pixelWidth = 0;

    if ((ctx == NULL) || (param == NULL) || (cb == NULL)) {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        return TUI_COMPOSER_STATUS_BAD_PARAM;
    }

    ComposerCtx = (tuiCustomComposerCtx_t *)ctx;
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
    ComposerCtx->appBuffer.size = tmpSize;

    if (cb->allocHelperBuff) {
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

    ComposerCtx->size = ComposerCtx->appBuffer.size;
    ComposerCtx->width = param->width;
    ComposerCtx->height = param->height;
    ComposerCtx->pixelWidth = _getWidth(param->format);

    ComposerCtx->renderSurface.buffer_size = ComposerCtx->size;
    ComposerCtx->renderSurface.buffer = ComposerCtx->appBuffer.vAddr;
    ComposerCtx->renderSurface.width = param->width;
    ComposerCtx->renderSurface.height = param->height;
    ComposerCtx->renderSurface.pixelWidth = ComposerCtx->pixelWidth;

    secure_memset(ComposerCtx->appBuffer.vAddr, 0, ComposerCtx->appBuffer.size);

    for (int32_t i = 0; i < DOT_SIZE; i++) {
        ((uint32_t *)redDot)[i] = RED_PIXEL;
        ((uint32_t *)greenDot)[i] = GREEN_PIXEL;
    }

    ComposerCtx->initialized = true;

EXIT:
    if (status != TUI_COMPOSER_STATUS_OK) {
        if (cb && cb->freeHelperBuff && ComposerCtx->appBuffer.size) {
            (void)cb->freeHelperBuff(cb->cbData, &ComposerCtx->appBuffer);
        }
    }
    return status;
}

/**
 * customComposerCompose:
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
static tuiComposerStatus_t customComposerCompose(
    void *ctx, tuiComposerInput_t *input, tuiComposerDisplayBuffer_t *buffer)
{
    tuiComposerStatus_t status = TUI_COMPOSER_STATUS_OK;
    tuiCustomComposerCtx_t *ComposerCtx = NULL;
    int32_t retVal = 0;
    screen_surface_t displaySurface;

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

    ComposerCtx = (tuiCustomComposerCtx_t *)ctx;

    if (!ComposerCtx->initialized) {
        TUILOGE("%s::%d init not done\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_NO_INIT;
        goto EXIT;
    }

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
    } else if ((input->cmd == TUI_COMPOSER_INPUT_CMD_DATA) ||
               (input->cmd == TUI_COMPOSER_INPUT_CMD_TIMEOUT)) {
        /* Process secure input as per selected layout */
        input->status =
            _drawDot(&input->data.touch, &ComposerCtx->renderSurface);
        if (input->status == TUI_COMPOSER_INPUT_STATUS_CONTINUE) {
            _copy_data(&displaySurface, &ComposerCtx->renderSurface);
            input->timeOut = -1;
        }
    } else {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
    }

EXIT:
    return status;
}

/**
 * customComposerSendCommand:
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
tuiComposerStatus_t customComposerSendCommand(void *ctx, uint32_t cmd,
                                              tuiComposerCmdPayload_t *payload)
{
    return TUI_COMPOSER_STATUS_OK;
}

/**
 * customComposerDeInit:
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
static tuiComposerStatus_t customComposerDeInit(void *ctx)
{
    tuiComposerStatus_t status = TUI_COMPOSER_STATUS_OK;
    tuiCustomComposerCtx_t *ComposerCtx = NULL;
    ITuiComposerCallback_t *callback = NULL;

    if (ctx == NULL) {
        TUILOGE("%s::%d invalid params passed\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_BAD_PARAM;
        goto EXIT;
    }

    ComposerCtx = (tuiCustomComposerCtx_t *)ctx;
    if (!ComposerCtx->initialized) {
        TUILOGE("%s::%d init not done\n", __func__, __LINE__);
        status = TUI_COMPOSER_STATUS_NO_INIT;
        goto EXIT;
    }

    callback = ComposerCtx->callback;

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
 * setupCustomComposer:
 *
 * Setup ITuiComposer object for TUI_COMPOSER_TYPE_CUSTOM.
 *
 * @param type Composer object to be initialised
 *
 * @return none
 *
 */
void setupCustomComposer(ITuiComposer_t *composer)
{
    tuiCustomComposerCtx_t *ctx = NULL;

    if ((composer == NULL) || (composer->type != TUI_COMPOSER_TYPE_CUSTOM)) {
        TUILOGE("%s::%d invalid param\n", __func__, __LINE__);
        return;
    }

    ctx = tui_malloc(sizeof(tuiCustomComposerCtx_t));
    if (ctx == NULL) {
        TUILOGE("%s::%d failed to create composer context\n",
                 __func__, __LINE__);
        return;
    }

    secure_memset(ctx, 0, sizeof(tuiCustomComposerCtx_t));

    composer->init = customComposerInit;
    composer->compose = customComposerCompose;
    composer->sendCommand = customComposerSendCommand;
    composer->deInit = customComposerDeInit;

    composer->ctx = (void *)ctx;
}
