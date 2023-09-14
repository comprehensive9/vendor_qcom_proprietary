/*========================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#ifndef __TUIDEFS_H__
#define __TUIDEFS_H__

#include <stdint.h>
/** Commands indication from QSEE App to Android Lib layer */
/*
typedef enum {
    SEC_TOUCH_CMD_CONTINUE = 0,
    SEC_TOUCH_CMD_CANCELLED = 1,
    SEC_TOUCH_CMD_COMPLETED = 2,
    SEC_TOUCH_CMD_MAX = SEC_TOUCH_CMD_COMPLETED,
    SEC_TOUCH_CMD_INVALID = (int32_t)-1
} sec_touch_cmd_t;

*/
typedef enum {
    SEC_TOUCH_EVENT_STARTED = 0, /** Secure touch started */
    SEC_TOUCH_EVENT_STOPPED = 1, /** Secure touch stopped */
    SEC_TOUCH_EVENT_DATA = 2,    /** Interrupt fired */
    SEC_TOUCH_EVENT_TIMEOUT = 3, /** Timeout */
    SEC_TOUCH_EVENT_POWER = 4,   /** Abort request to service a power event */
    SEC_TOUCH_EVENT_ABORT = 5,   /** Abort request  from VM*/
    SEC_TOUCH_EVENT_MAX = SEC_TOUCH_EVENT_ABORT,
    SEC_TOUCH_EVENT_INVALID = (int32_t)-1
} sec_touch_event_t;

#endif
