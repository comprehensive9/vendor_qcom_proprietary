/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif

#include "TUIHeap.h"
#include "TUILog.h"
#include "object.h"
#include "secure_memset.h"
#include "minkipc.h"
#include "IClientEnv.h"
#include "IAppClient.h"
#include "CAppClient.h"
#include "IOpener.h"
#include "ITunnelInvokeService.h"
#include "CEidTui.h"
#include "IEidTui.h"
#include "malloc.h"
#include "secure_memset.h"
#include "eid_tui.h"


static void *eid_malloc(size_t size)
{
    void *p = tui_malloc(size);
    if (p != NULL) {
        secure_memset(p, 0, size);
    }
    return p;
}

static int convertUtf16be2Utf8(uint8_t *in_buf, size_t in_len, uint8_t *out_buf, size_t out_len)
{
    int i,j,w_l, total_size = 0;
    uint16_t wchar;

    if (in_len&1) {
        TUILOGE("%s::%d buffer length must be even!\n", __func__, __LINE__);
        return -1;
    }

    for (i = 0; i < in_len; i+=2) {
        wchar = (in_buf[i] << 8) | in_buf[i+1];

        if (wchar == 0x0a) {
            wchar = ' ';
        }

        if (wchar <= 0x7F) {
            w_l = 1;
        } else if (wchar <= 0x7FF) {
            w_l = 2;
        } else
            w_l = 3;

        if (total_size + w_l > out_len) {
            TUILOGE("%s::%d out buffer is not enough, size of buffer:%d, it needs :%d\n", __func__, __LINE__, out_len, j+w_l);
            total_size = -1;
            break;
        }

        switch (w_l) {
            case 1:
                out_buf[total_size++] = (char)wchar;
                break;
            case 2:
                out_buf[total_size++] = (char)((wchar >> 8) | 0xC0);
                out_buf[total_size++] = (char)((wchar & 0x3F) | 0x80);
                break;
            case 3:
                out_buf[total_size++] = (char)((wchar >> 12) | 0xE0);
                out_buf[total_size++] = (char)(((wchar & 0xFC0) >> 6) | 0x80);
                out_buf[total_size++] = (char)((wchar & 0x3F) | 0x80);
                break;
            default:
                break;
        }

    }

    return total_size;
}

static MinkIPC* getEidTuiObj(Object *pTunnelInvokeOpener,
                             Object *pClientEnvTunnel,
                             Object *pAppClient,
                             Object *pAppObj,
                             Object *pEIDObj)
{
    int32_t ret = Object_OK;
    const char *pTunnelSocketPath = "/dev/socket/le_tisock";
    const char *pEIDStr = "eid";

    MinkIPC* pTunnelMinkIPC = MinkIPC_connect(pTunnelSocketPath, pTunnelInvokeOpener);
    if (!pTunnelMinkIPC || Object_isNull(*pTunnelInvokeOpener)) {
        TUILOGE("%s::%d Error: Failed to get Tunnel Invoke object", __func__,
                __LINE__);
        ret = Object_ERROR;
        goto exit;
    }


    ret = ITunnelInvokeService_getClientEnv(*pTunnelInvokeOpener,
                                            pClientEnvTunnel);
    if (Object_isERROR(ret) || Object_isNull(*pClientEnvTunnel)) {
        TUILOGE("%s::%d Failed to get Client Env from Tunnel Invoke, ret - %d",
                __func__, __LINE__, ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = IClientEnv_open(*pClientEnvTunnel, CAppClient_UID, pAppClient);
    if (Object_isERROR(ret) || Object_isNull(*pAppClient)) {
        TUILOGE("%s::%d Failed to get App Client, ret - %d", __func__, __LINE__,
                ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = IAppClient_getAppObject(*pAppClient, pEIDStr, strlen(pEIDStr), pAppObj);
    if (Object_isERROR(ret) || Object_isNull(*pAppObj)) {
        TUILOGE("%s::%d Failed to get App Object, ret - %d", __func__, __LINE__,
                ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = IOpener_open(*pAppObj, CEidTui_UID, pEIDObj);
    if (Object_isERROR(ret) || Object_isNull(*pEIDObj)) {
        TUILOGE("%s::%d Failed to get eid object, ret - %d",
                __func__, __LINE__, ret);
        ret = Object_ERROR;
        goto exit;
    }

exit:
    if (ret) {
        if (pTunnelMinkIPC) {
            MinkIPC_release(pTunnelMinkIPC);
            pTunnelMinkIPC = NULL;
        }
        Object_ASSIGN_NULL(*pEIDObj);
        Object_ASSIGN_NULL(*pAppObj);
        Object_ASSIGN_NULL(*pAppClient);
        Object_ASSIGN_NULL(*pClientEnvTunnel);
        Object_ASSIGN_NULL(*pTunnelInvokeOpener);
    }
    return pTunnelMinkIPC;
}

int32_t loadSecureMessage(uint8_t *app_info, uint32_t app_info_size,uint8_t *prompt_info, uint32_t prompt_info_size,
         int32_t *app_info_size_out, int32_t *prompt_info_size_out)
{
    int32_t size = 0;
    int32_t result = Object_OK;
    uint8_t* msgBuffer = NULL;
    size_t msgLenOut = 0;
    MinkIPC *pTunnelMinkIPC = NULL;
    Object tunnelInvokeOpener = Object_NULL;
    Object clientEnvTunnel = Object_NULL;
    Object appClient = Object_NULL;
    Object appObj = Object_NULL;
    Object EIDObj = Object_NULL;
    eid_sign_msg_t *msg_header = NULL;
    uint8_t* p_data = NULL;

    TUILOGD("%s:Load secure message from eID TA.", __func__);

    if (app_info == NULL || app_info_size == 0 ||
        prompt_info == NULL || prompt_info_size == 0) {
        TUILOGE("%s::%d Invalid input.", __func__, __LINE__);
        return -1;
    }

    pTunnelMinkIPC = getEidTuiObj(
        &tunnelInvokeOpener, &clientEnvTunnel, &appClient, &appObj, &EIDObj);

    if (pTunnelMinkIPC == NULL || Object_isNull(tunnelInvokeOpener) ||
        Object_isNull(clientEnvTunnel) || Object_isNull(appClient) ||
        Object_isNull(appObj) || Object_isNull(EIDObj)) {
        TUILOGE("%s::%d Error: Failed to get EidTui object, result - %d",
                __func__, __LINE__, result);
        result = -1;
        goto exit;
    }

    msgBuffer = (uint8_t*)eid_malloc(MAX_SECURE_MESSAGE + sizeof(eid_sign_msg_t));
    result = IEidTui_GetTUIMessage(EIDObj,
                         (void *)msgBuffer,
                         MAX_SECURE_MESSAGE + sizeof(eid_sign_msg_t), &msgLenOut);
    if (Object_isERROR(result)) {
        TUILOGE("%s::%d Error: Failed to call GetTUIMessage, result - %d",
            __func__, __LINE__, result);
        result = -1;
        goto exit;
    }

    if (msgLenOut < sizeof(eid_sign_msg_t)) {
        TUILOGE("%s::%d packate lenth is too short %d",
                __func__, __LINE__, msgLenOut);
        result = -1;
        goto exit;
    }

    msg_header = (eid_sign_msg_t *)msgBuffer;
    p_data = msgBuffer + sizeof(eid_sign_msg_t);
    TUILOGD("%s:%d packate len:%d, app info:%d, promote info:%d",
        __func__, __LINE__, msg_header->app_info_len, msg_header->prompt_info_len);

     size = convertUtf16be2Utf8(p_data, msg_header->app_info_len, app_info, app_info_size);
     if (size < 0) {
        TUILOGE("%s::%d Failed to convert app info to UTF-8 code.",
                __func__, __LINE__);
        result = -1;
        goto exit;
     }
     *app_info_size_out = size;

     size = convertUtf16be2Utf8(p_data + msg_header->app_info_len, msg_header->prompt_info_len, prompt_info, prompt_info_size);
     if (size < 0) {
        TUILOGE("%s::%d Failed to convert prompt info to UTF-8 code.",
                __func__, __LINE__);
        result = -1;
        goto exit;
     }
     *prompt_info_size_out = size;

    result = *app_info_size_out + *prompt_info_size_out;
exit:
    if (pTunnelMinkIPC) {
        MinkIPC_release(pTunnelMinkIPC);
        pTunnelMinkIPC = NULL;
    }

    if (msgBuffer)
        tui_free(msgBuffer);
    Object_ASSIGN_NULL(EIDObj);
    Object_ASSIGN_NULL(appObj);
    Object_ASSIGN_NULL(appClient);
    Object_ASSIGN_NULL(clientEnvTunnel);
    Object_ASSIGN_NULL(tunnelInvokeOpener);
    TUILOGD("%s::%d result = %d", __func__, __LINE__, result);
    return result;
}
