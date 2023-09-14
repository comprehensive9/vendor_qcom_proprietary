/******************************************************************************
  @file    ioctl.cpp
  @brief   ioctl call made to display driver

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <cutils/log.h>
#include "ioctl.h"
#include <xf86drm.h>
#include <display/drm/sde_drm.h>
#include <dlfcn.h>

#include <string>

static int g_disp_fd = -1;
typedef int (*WaitForComposerFnPtr)();

int WaitForComposer() {
    int composer_up = -1;
    std::string lib_name = "libqdutils.so";
    void *qdutils = dlopen(lib_name.c_str(), RTLD_GLOBAL | RTLD_NOW);
    if (!qdutils) {
        QLOGE("drmIOCTLLib failed to open:%s", lib_name.c_str());
        return composer_up;
    }

    std::string func_name = "waitForComposerInitPerf";
    WaitForComposerFnPtr wait_func =  (int (*)()) dlsym(qdutils, func_name.c_str());

    if (wait_func){
        QLOGD("drmIOCTLLib Waiting for composer service init");
        composer_up = wait_func();
    } else {
        QLOGW("wait_func is NULL");
    }

  dlclose(qdutils);
  return composer_up;
}

int open_display_driver() {
    int composer_up = WaitForComposer();
    QLOGD("drmIOCTLLib done WaitForComposer");
    if(!composer_up) {
        g_disp_fd = drmOpen("msm_drm", NULL);
        if (g_disp_fd < 0) {
            QLOGE("drmIOCTLLib open failed with error=%s", strerror(errno));
            goto exit;
        }
        drmDropMaster(g_disp_fd);
    }
    else {
        QLOGD("drmIOCTLLib composer is not up");
    }
exit:
    return g_disp_fd;
}

void close_display_driver() {
    if (g_disp_fd < 0) {
        QLOGI("drmIOCTLLib display FD is not valid %d", g_disp_fd);
    }
    else {
        int ret = drmClose(g_disp_fd);
        g_disp_fd = -1; // nothing to do close fails
        QLOGI("drmIOCTLLib close return %d", ret);
    }
}

int early_wakeup_ioctl(int connectorId) {
    int FAILED = -1;
    QLOGI("drmIOCTLLib connectorId: %d", connectorId);
    if ((g_disp_fd < 0) && (open_display_driver() < 0)) {
        return FAILED;
    }

    struct drm_msm_display_hint display_hint= {};
    struct drm_msm_early_wakeup early_wakeup = {};
    display_hint.hint_flags= DRM_MSM_DISPLAY_EARLY_WAKEUP_HINT;
    display_hint.data = (uint64_t) &early_wakeup;
    early_wakeup.wakeup_hint = 1;
    early_wakeup.connector_id = connectorId;
    int ret = drmIoctl(g_disp_fd, DRM_IOCTL_MSM_DISPLAY_HINT, &display_hint);
    QLOGI("drmIOCTLLib ret: %d", ret);
    if(ret) {
        QLOGE("drmIOCTLLib failed ret=%d error=%s", ret, strerror(errno));
        close_display_driver();
        return FAILED;
    }

    close_display_driver();
    return 0;
}