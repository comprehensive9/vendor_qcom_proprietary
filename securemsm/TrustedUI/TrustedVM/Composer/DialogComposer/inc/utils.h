/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#ifndef __TUI_COMPOSER_UTILS_H__
#define __TUI_COMPOSER_UTILS_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif
#include <stdbool.h>

#include "layout_manager.h"

#include "TUILog.h"
#include "qsee_tui_dialogs.h"
#include "qsee_tui_layout.h"

#define MAX_LAYOUT_NAME 128
#define MAX_APP_INFO     256
#define MAX_PROMOTE_INFO 256
#define MAX_MESSAGE_LEN (MAX_APP_INFO + MAX_PROMOTE_INFO)

#define NO_TIMEOUT (-1)
#define DEFAULT_TIMEOUT NO_TIMEOUT

#define MAX_INPUT_LEN_BYTES 500
#define MAX_INPUT_LEN 200
#define MIN_INPUT_LEN 0
#define MAX_MSG_LEN 300

#define CHECK_STATUS(status)                                                  \
    if (((status) < 0) || (TUI_CANCEL_PRESSED == (status)) ||                 \
        (TUI_MIDDLE_PRESSED == (status)) || (TUI_LEFT_PRESSED == (status))) { \
        TUILOGE("point 0, status %d", (status));         \
        return (status);                                                      \
    }

extern LayoutPage_t* getLayout(const char* id, uint32_t width, uint32_t height);
extern uint32_t loadLogo(uint8_t** logoBuff);
extern uint32_t loadIndicator(uint8_t** indicatorBuff);
extern int32_t loadSecureIndicator(void);
extern void unLoadDialogResources(void);

#endif //__TUI_COMPOSER_UTILS_H__
