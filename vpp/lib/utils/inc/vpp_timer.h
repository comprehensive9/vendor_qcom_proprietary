/*!
 * @file vpp_timer.h
 *
 * @cr
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_TIMER_H_
#define _VPP_TIMER_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Defines
 ***********************************************************************/

/************************************************************************
 * Structures / Enumerations
 ***********************************************************************/

typedef struct {
    uint32_t u32Seconds;
    uint32_t u32NanoSeconds;
} t_StVppTimerDuration;

typedef struct {
    void *pvData;
    void (*pfOnTimerExpiry)(void *pvData);
} t_StVppTimerCallback;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

void *pvVppTimer_Init(t_StVppTimerCallback stCallback,
                      t_StVppTimerDuration stDuration);
void vVppTimer_Term(void *pvTimer);
uint32_t u32VppTimer_Restart(void *pvTimer);
uint32_t u32VppTimer_Stop(void *pvTimer);

#ifdef __cplusplus
}
#endif

#endif /* _VPP_TIMER_H_ */
