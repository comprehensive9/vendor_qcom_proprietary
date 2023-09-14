/******************************************************************************
  @file    ioctl_fallthrough.cpp
  @brief   Dummy implementation of ioctl call made to display driver

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

int early_wakeup_ioctl(int connectorId) {
    QLOGI("drmIOCTLLib fallthrough connectorId: %d", connectorId);
    return 0;
}