/*=============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef _TUI_INPUT_TYPES_H_
#define _TUI_INPUT_TYPES_H_

typedef enum {
  TUI_Input_Touch = 1,
  TUI_Input_Max
} TUIInputType;

typedef struct {
  uint32_t event; /* 0: None, 1: Down, 2: Move, 4: Up */
  uint32_t xCoOrdinate;
  uint32_t yCoOrdinate;
  uint64_t timestamp; /* Unused */
  uint32_t dirty;
} TUITouchData;

#define TUI_MAX_FINGER_NUM 10

typedef struct {
  TUITouchData fingers[TUI_MAX_FINGER_NUM];
} TUITouchInput;

typedef struct {
  TUIInputType type;
  int32_t timeOut;
  union {
    TUITouchInput touch;
  } data;
} TUIInputData;

#endif /*_TUI_INPUT_TYPES_H_*/