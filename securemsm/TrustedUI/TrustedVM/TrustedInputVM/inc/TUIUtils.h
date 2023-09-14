/*
 * Copyright (c) 2019, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI_UTILS_H__
#define __TUI_UTILS_H__

#include "TUILog.h"

#define TUI_ERROR(error_code)                                           \
    ret = (error_code);                                                 \
    TUILOGE("%s::%d err=%x (%s)", __func__, __LINE__,                   \
           error_code, strerror(error_code));                           \
    goto errorExit;

#define TUI_CHECK_ERR(cond, error_code) \
    if (!(cond)) {                      \
        TUI_ERROR(error_code)           \
    }

#define TUI_CHECK(cond)                                          \
    if (!(cond)) {                                               \
        TUILOGE("%s::%d", __func__, __LINE__);                   \
        goto errorExit;                                          \
    }

#define TUI_COND(error_code)                                       \
    ret = (error_code);                                            \
    TUILOGE("%s::%d err=%d", __func__, __LINE__,                   \
           error_code);                                            \
    goto errorExit;

#define TUI_CHECK_COND(cond, error_code) \
    if (!(cond)) {                       \
        TUI_COND(error_code)             \
    }

#endif /* __TUI_UTILS_H__ */
