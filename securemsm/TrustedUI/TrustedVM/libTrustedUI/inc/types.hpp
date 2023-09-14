/*=============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#pragma once
// AUTOGENERATED FILE: DO NOT EDIT

#include <cstdint>
#include "object.h"

/** @file  types.idl */

struct TUIConfig {
    uint8_t useSecureIndicator;
    uint8_t layoutName[256];
    uint8_t enableSessionAuth;
};

static const int32_t TUI_SUCCESS = INT32_C(0);
static const int32_t TUI_FAILURE = INT32_C(-1);
static const int32_t TUI_LISTENER_ERROR = INT32_C(-2);
static const int32_t TUI_ALREADY_RUNNING = INT32_C(-3);
static const int32_t TUI_IS_NOT_RUNNING = INT32_C(-4);

static const uint32_t TUI_PRIMARY_DISPLAY = UINT32_C(1);
static const uint32_t TUI_SECONDARY_DISPLAY = UINT32_C(2);

static const uint32_t TUI_TOUCH_INPUT = UINT32_C(1);
static const uint32_t TUI_MAX_INPUT = UINT32_C(1);


