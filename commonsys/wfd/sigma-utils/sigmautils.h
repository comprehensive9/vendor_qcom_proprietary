/*
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __SIGMAUTILS__
#define __SIGMAUTILS__

#include "WiFiDisplayClient.h"
int createVDS(void* pInfo);
void releaseVDS();
void deleteSurfaceUtil();
void setWFDSurface(WiFiDisplayClient *gWFDClient);
typedef struct mmSessionInfo
{
    int height;
    int width;
    int hdcp;
    long producer;
}mmSessionInfo;

#endif
