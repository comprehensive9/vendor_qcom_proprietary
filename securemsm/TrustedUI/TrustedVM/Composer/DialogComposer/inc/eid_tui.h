/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __EID_TUI_H__
#define __EID_TUI_H__

#include "minkipc.h"

#define IEidTui_OP_GetTUIMessage 0
#define MAX_SECURE_MESSAGE      256

typedef struct _eid_sign_msg {
    uint32_t app_info_len;
    uint32_t prompt_info_len;
}__attribute__ ((packed)) eid_sign_msg_t;


int32_t loadSecureMessage(uint8_t *app_info, uint32_t app_info_size,uint8_t *prompt_info, uint32_t prompt_info_size,
         int32_t *app_info_size_out, int32_t *prompt_info_size_out);


#endif
