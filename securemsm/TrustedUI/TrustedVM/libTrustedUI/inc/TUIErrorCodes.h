/*=============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef _TUI_ERROR_CODES_H_
#define _TUI_ERROR_CODES_H_

typedef enum {
  TUI_ERR_NOT_ENOUGH_BUFFERS = -11, /* TUI Session out of buffers */
  TUI_ERR_OUT_OF_MEMORY = -10, /* TUI EE out of memory */
  TUI_ERR_CORE_SVC_NOT_READY = -9, /* TUI Core Service not running or in bad state */
  TUI_ERR_ALREADY_RUNNING = -8, /* TUI Session already running */
  TUI_ERR_INVALID_INPUT = -7,   /* Invalid input data */
  TUI_ERR_INVALID_BUFFER = -6,  /* Invalid buffer */
  TUI_ERR_HLOS_CALLBACK = -5,   /* HLOS callback error */
  TUI_ERR_INVALID_STATE = -4,   /* Invalid session state */
  TUI_ERR_NOT_SUPPORTED = -3,   /* Not supported */
  TUI_ERR_INVALID_PARAM = -2,   /* Invalid parameter */
  TUI_ERR_GENERIC_FAILURE = -1,             /* general failure */
  TUI_OK = 0,                   /* success */
}TUIResult;
#endif /*_TUI_ERROR_CODES_H_*/
