/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
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
#include "TUIHeap.h"
#include "secure_memset.h"

extern void setupDialogComposer(ITuiComposer_t *composer);
extern void setupCustomComposer(ITuiComposer_t *composer);

/**
 * createTUIComposer:
 *
 * Create a new composer object (of requested type) for secure graphics content
 * rendering/compostion.
 *
 * @param type Composer type {@link tuiComposerType_t}
 *
 * @return object Newly created composer object
 *
 */
ITuiComposer_t *createTUIComposer(tuiComposerType_t type)
{
    bool init = false;
    ITuiComposer_t *composer = (ITuiComposer_t *)tui_malloc(sizeof(*composer));
    if (composer == NULL) {
       TUILOGE("%s: failed to allocate memory\n", __func__);
        return NULL;
    }

    secure_memset(composer, 0, sizeof(ITuiComposer_t));
    composer->type = type;

    switch (type) {
        case TUI_COMPOSER_TYPE_DIALOG:
            setupDialogComposer(composer);
            init = true;
            break;
        case TUI_COMPOSER_TYPE_CUSTOM:
            setupCustomComposer(composer);
            init = true;
            break;
        default:
            TUILOGE("%s: invalid type: %d\n", __func__, type);
    }

    if (!init) {
        destroyTUIComposer(composer);
        composer = NULL;
    }

    return composer;
}

/**
 * destroyTUIComposer:
 *
 * Destroy an already created composer object.
 *
 * @param composer Composer object
 *
 */
void destroyTUIComposer(ITuiComposer_t *composer)
{
    if (composer == NULL) {
        return;
    }

    if (composer->ctx) {
        (void)composer->deInit(composer->ctx);
        tui_free(composer->ctx);
    }

    tui_free(composer);
}
