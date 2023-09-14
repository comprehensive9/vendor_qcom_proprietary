/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI_UTILS_H__
#define __TUI_UTILS_H__

#include <map>

#define C_LENGTHOF(array) \
    ((void *)&(array) == (void *)(array) ? sizeof(array) / sizeof *(array) : 0)

#define TUI_COND(error_code)                                   \
    ret = (error_code);                                         \
    TUILOGE("%s:: Line no.:%d err=%x", __func__, __LINE__, error_code);\
    goto end;

#define TUI_CHECK_COND(cond, error_code) \
    if (!(cond)) {                      \
        TUI_COND(error_code)           \
    }


#define TUI_ERROR(error_code)                                   \
    ret = (error_code);                                         \
    TUILOGE("%s:: Line no.:%d err=%x (%s)", __func__, __LINE__, error_code, \
          strerror(error_code));                                \
    goto end;

#define TUI_CHECK_ERR(cond, error_code) \
    if (!(cond)) {                      \
        TUI_ERROR(error_code)           \
    }

#define TUI_CHECK(cond)                      \
    if (!(cond)) {                           \
        TUILOGE("%s::Line:%d", __func__, __LINE__); \
        goto end;                            \
    }

#endif
