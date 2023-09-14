/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif

#include "utils.h"
#include "TUILog.h"
#include "memscpy.h"
#include "TUIHeap.h"
#include "secure_memset.h"
#include "object.h"
#include "minkipc.h"
#include "IClientEnv.h"
#include "IAppClient.h"
#include "CAppClient.h"
#include "IOpener.h"
#include "ITunnelInvokeService.h"
#include "CSecureIndicator.h"
#include "ISecureIndicator.h"

#define PROFILE_ON

#ifdef PROFILE_ON
#define PROFILE \
      unsigned long long stop;         \
      unsigned long long start;        \
      struct timeval start_time;       \
      unsigned long long delay = 0; \
      struct timeval stop_time;

#define TIME_START                   \
      gettimeofday(&start_time, NULL); \
      start = start_time.tv_usec;

#define TIME_STOP                   \
    gettimeofday(&stop_time, NULL); \
    stop = stop_time.tv_usec;        \
    delay = ((stop_time.tv_sec - start_time.tv_sec) * 1000000) + (stop_time.tv_usec - start_time.tv_usec); \
    TUILOGD("%s::%d- (stop time: %llu micro seconds)", __func__, __LINE__, stop); \
    TUILOGE("%s::%d (time taken = %llu micro seconds)", __func__, __LINE__, delay);
#else
#define PROFILE \
    do {           \
    } while (0)

#define TIME_START \
    do {           \
    } while (0)
#define TIME_STOP \
    do {          \
    } while (0)

#endif


extern qsee_tui_dialog_ret_val_t qsee_tui_get_secure_indicator_buffer(
    qsee_tui_secure_indicator_t **indicator);

static uint32_t lwidth;
static uint32_t lheight;
static uint8_t *dialogLogo = NULL;
static uint8_t *dialogIndicator = NULL;

uint32_t loadLogo(uint8_t **logoBuff)
{
    int fd = 0, size = 0, status = 0;
    struct stat fsstat;
    uint32_t logoSize = 0;
    char *logoPath = NULL;
    char logoPath_default[] = "/usr/data/tui/logo.png";
    char logoPath_1080x2340[] = "/usr/data/tui/1080x2340/logo.png";

    if (logoBuff == NULL) {
        return 0;
    }

    if ((lwidth == 1080 && lheight == 2340) ||
        (lwidth == 1080 && lheight == 2408))
       logoPath = logoPath_1080x2340;
    else
       logoPath = logoPath_default;

    TUILOGD("read logo file, from = %s", logoPath);
    status = lstat(logoPath, &fsstat);
    if (status < 0) {
        TUILOGE("failed lstat for the logo");
        return 0;
    }

    logoSize = fsstat.st_size;

    *logoBuff = (uint8_t *)tui_malloc(logoSize);
    if (*logoBuff == NULL) {
        TUILOGE("failed to allocate memory for logo");
        return 0;
    }

    fd = open(logoPath, O_RDONLY);
    if (fd < 0) {
        TUILOGE("failed to open logo file");
        tui_free(*logoBuff);
        *logoBuff = NULL;
        return 0;
    }

    size = read(fd, *logoBuff, logoSize);
    if (size != logoSize) {
        TUILOGE("failed to read logo file");
        tui_free(*logoBuff);
        *logoBuff = NULL;
        return 0;
    }

    dialogLogo = *logoBuff;

    TUILOGD("read logo file, size = %d", size);
    close(fd);

    return size;
}

uint32_t loadIndicator(uint8_t **indicatorBuff)
{
    int fd = 0, size = 0, status = 0;
    uint32_t indicatorSize = 0;
    char *indicatorPath = NULL;
    char indicatorPath_default[] = "/usr/data/tui/sec_ind.png";
    char indicatorPath_1080x2340[] = "/usr/data/tui/1080x2340/sec_ind.png";

    if (indicatorBuff == NULL) {
        return 0;
    }

    if ((lwidth == 1080 && lheight == 2340) ||
        (lwidth == 1080 && lheight == 2408))
        indicatorPath = indicatorPath_1080x2340;
    else
        indicatorPath = indicatorPath_default;

    TUILOGD("read indicator file, from = %s", indicatorPath);

    struct stat fsstat;
    status = lstat(indicatorPath, &fsstat);
    if (status < 0) {
        TUILOGE("failed lstat for the indicator");
        return 0;
    }

    indicatorSize = fsstat.st_size;

    *indicatorBuff = (uint8_t *)tui_malloc(indicatorSize);
    if (*indicatorBuff == NULL) {
        TUILOGD("failed to allocate buffer for indicator");
        return 0;
    }

    fd = open(indicatorPath, O_RDONLY);
    if (fd < 0) {
        TUILOGD("failed to open indicator file");
        tui_free(*indicatorBuff);
        *indicatorBuff = NULL;
        return 0;
    }

    size = read(fd, *indicatorBuff, indicatorSize);
    if (size != indicatorSize) {
        TUILOGE("failed to read indicator file");
        tui_free(*indicatorBuff);
        *indicatorBuff = NULL;
        return 0;
    }

    dialogIndicator = *indicatorBuff;

    TUILOGD("read indicator file, size = %d", size);
    close(fd);
    return size;
}

static MinkIPC *getSecureIndicatorObj(Object *pTunnelInvokeOpener, Object *pSI2Obj)
{
    int32_t ret = Object_OK;
    Object clientEnvTunnel = Object_NULL;
    Object appClient = Object_NULL;
    Object appObj = Object_NULL;
    const char *pTunnelSocketPath = "/dev/socket/le_tisock";
    const char *pSI2Str = "secureindicator2";
    PROFILE;
    TIME_START;

    MinkIPC* pTunnelMinkIPC = MinkIPC_connect(pTunnelSocketPath, pTunnelInvokeOpener);
    if (!pTunnelMinkIPC || Object_isNull(*pTunnelInvokeOpener)) {
        TUILOGE("%s::%d Error: Failed to get Tunnel Invoke object", __func__,
                __LINE__);
        ret = Object_ERROR;
        goto exit;
    }

    ret = ITunnelInvokeService_getClientEnv(*pTunnelInvokeOpener, &clientEnvTunnel);
    if (Object_isERROR(ret) || Object_isNull(clientEnvTunnel)) {
        TUILOGE("%s::%d Failed to get Client Env from Tunnel Invoke, ret - %d",
                __func__, __LINE__, ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = IClientEnv_open(clientEnvTunnel, CAppClient_UID, &appClient);
    if (Object_isERROR(ret) || Object_isNull(appClient)) {
        TUILOGE("%s::%d Failed to get App Client, ret - %d", __func__, __LINE__,
                ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = IAppClient_getAppObject(appClient, pSI2Str, strlen(pSI2Str), &appObj);
    if (Object_isERROR(ret) || Object_isNull(appObj)) {
        TUILOGE("%s::%d Failed to get App Object, ret - %d", __func__, __LINE__,
                ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = IOpener_open(appObj, CSecureIndicator_UID, pSI2Obj);
    if (Object_isERROR(ret) || Object_isNull(*pSI2Obj)) {
        TUILOGE("%s::%d Failed to get secureindicator2 object, ret - %d",
                __func__, __LINE__, ret);
        ret = Object_ERROR;
        goto exit;
    }

exit:
    Object_RELEASE_IF(appObj);
    Object_RELEASE_IF(appClient);
    Object_RELEASE_IF(clientEnvTunnel);
    if (ret) {
        Object_RELEASE_IF(*pSI2Obj);
        Object_RELEASE_IF(*pTunnelInvokeOpener);
        if (pTunnelMinkIPC) {
            MinkIPC_release(pTunnelMinkIPC);
            pTunnelMinkIPC = NULL;
        }
    }
    TIME_STOP;
    return pTunnelMinkIPC;
}

int32_t loadSecureIndicator(void)
{
    int32_t ret = Object_OK;
    uint32_t isProvisioned = 0;
    size_t indicatorLenout = 0;
    MinkIPC *pTunnelMinkIPC = NULL;
    Object tunnelInvokeOpener = Object_NULL;
    Object SI2Obj = Object_NULL;
    qsee_tui_secure_indicator_t *pSecureIndicator = NULL;
    PROFILE;

    if (qsee_tui_get_secure_indicator_buffer(&pSecureIndicator) != TUI_SUCCESS) {
        TUILOGE("%s::%d Error retrieving the secure indicator buffer", __func__,
                __LINE__);
        ret = Object_ERROR;
        goto exit;
    }
    pTunnelMinkIPC = getSecureIndicatorObj(&tunnelInvokeOpener, &SI2Obj);

    if (pTunnelMinkIPC == NULL || Object_isNull(tunnelInvokeOpener) || Object_isNull(SI2Obj)) {
        TUILOGE("%s::%d Error: Failed to get secureindicator2 object, ret - %d",
                __func__, __LINE__, ret);
        ret = Object_ERROR;
        goto exit;
    }

    ret = ISecureIndicator_isIndicatorProvisioned(SI2Obj, &isProvisioned);
    if (Object_isERROR(ret)) {
        TUILOGE("%s::%d Error: Failed to check if secureindicator2 is provisioned, ret - %d",
            __func__, __LINE__, ret);
        goto exit;
    }
    if (!isProvisioned) {
        TUILOGE("%s::%d Error: Secure Indicator not provisioned", __func__,
                __LINE__);
        ret = Object_ERROR;
        goto exit;
    }
    TUILOGD("%s::%d Allocated indicator size - %zu ", __func__, __LINE__,
            pSecureIndicator->u32IndicatorSize);
    TIME_START;
    ret = ISecureIndicator_getIndicator(SI2Obj,
                         (void *)pSecureIndicator->pu8SecureIndicator,
                         pSecureIndicator->u32IndicatorSize, &indicatorLenout);
    TIME_STOP;
    if (Object_isERROR(ret)) {
        TUILOGE("%s::%d Error: Failed to call getIndicator, ret - %d", __func__,
                __LINE__, ret);
        goto exit;
    }
    if (indicatorLenout == 0) {
        TUILOGE("%s::%d Error: Secure Indicator Invalid length", __func__,
                __LINE__);
        ret = Object_ERROR;
        goto exit;
    }
    pSecureIndicator->bIsValid = 1;
    TUILOGE("%s::%d Secure Indicator size - %zu ", __func__, __LINE__,
            indicatorLenout);

exit:
    Object_RELEASE_IF(SI2Obj);
    Object_RELEASE_IF(tunnelInvokeOpener);
    if (pTunnelMinkIPC) {
        MinkIPC_release(pTunnelMinkIPC);
        pTunnelMinkIPC = NULL;
    }
    return ret;
}

LayoutPage_t *getLayout(const char *id, uint32_t width, uint32_t height)
{
    const char *suffixes[] = {"_large", "", "_small"};
    const uint32_t MAX_DIALOG_NAME_LENGTH = 64;
    uint32_t layout_width = 0, layout_height = 0;
    char buffer[MAX_DIALOG_NAME_LENGTH];
    secure_memset(buffer, 0, MAX_DIALOG_NAME_LENGTH);
    layout_mgr_err_t status = LAYOUT_MGR_SUCCESS;
    LayoutPage_t *layoutPage = NULL;
    TUILOGE("%s : %d, id - %s, width - %d, height - %d", __func__, __LINE__, id,
            width, height);
    if ((id == NULL) || (width == 0) || (height == 0)) {
        TUILOGE("invalid input param received");
        return NULL;
    }

    lwidth = width;
    lheight = height;

    /* Iteration is done from large to small layout sizes */
    for (uint32_t i = 0; i < (sizeof(suffixes) / sizeof(*suffixes)); i++) {
        size_t len1 = strlcpy(buffer, id, sizeof(buffer));
        size_t len2 = strlcat(buffer, suffixes[i], sizeof(buffer));
        if (len1 >= sizeof(buffer) || len2 >= sizeof(buffer)) {
            TUILOGE("Truncation error while processing dialog name: %s",
                    buffer);
        }
        TUILOGE("%s : %d, buffer - %s", __func__, __LINE__, buffer);
        layoutPage = get_layout_by_name_ex(width, height, buffer);
        if (layoutPage == NULL) {
            TUILOGE("%s : %d, DEBUG: ERROR - get_layout_by_name layoutPage is null",
                __func__, __LINE__);
            continue;
        }

        status = layout_mgr_load_layout(layoutPage);
        if (status < 0) {
            TUILOGE("%s : %d,DEBUG: ERROR - layout_mgr_load_layout, status - %d",
                __func__, __LINE__, status);
            continue;
        }
        TUILOGE("%s : %d,DEBUG: layout_mgr_load_layout, status - %d", __func__,
                __LINE__, status);

        status = layout_mgr_get_layout_size(&layout_height, &layout_width);
        if (status < 0) {
            TUILOGE("%s : %d,DEBUG: ERROR - layout_mgr_get_layout_size, status - "
                "%d", __func__, __LINE__, status);
            continue;
        }
        TUILOGE("%s : %d,DEBUG: layout_mgr_get_layout_size, status - %d",
                __func__, __LINE__, status);
        TUILOGE("%s : %d, DEBUG: Initial height - %d, layout_height - %d, width - "
            "%d, layout_width - %d",
            __func__, __LINE__, height, layout_height, width, layout_width);

        if ((height >= layout_height && width >= layout_width)) {
            TUILOGD("Suitable layout found (%s)", buffer);
            TUILOGD("id: %s, width: %u, height: %u, layout_width: %u, "
                "layout_height: %u",
                id, width, height, layout_width, layout_height);
            TUILOGE("%s : %d,DEBUG: height - %d, layout_height - %d, width - %d, "
                "layout_width - %d",
                __func__, __LINE__, height, layout_height, width, layout_width);
            return layoutPage;
        }
    }

    return NULL;
}

void unLoadDialogResources(void)
{
    if (dialogLogo) {
        tui_free(dialogLogo);
        dialogLogo = NULL;
    }

    if (dialogIndicator) {
        tui_free(dialogIndicator);
        dialogIndicator = NULL;
    }
}
