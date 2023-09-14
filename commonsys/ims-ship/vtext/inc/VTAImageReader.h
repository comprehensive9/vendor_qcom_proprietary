/*****************************************************************************

 ============================
Copyright (c)  2016-2020  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VTAImageReader.h
 Description: Implementation of VideoAImageReader

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 25-Mar-2020   Narasimha      First Version
 *****************************************************************************/

#include <qplog.h>
#include <utils/Log.h>
#include <media/NdkImage.h>
#include <media/NdkImageReader.h>
#include <jni.h>
#include<android/native_window.h>
#include <media/NdkImage.h>
#include <media/NdkImageReader.h>

class VideoAImageReader
{
  AImageReader* m_pImageReader;
  AImage *m_pAImg;
  ANativeWindow *m_pImageReaderNativeWindow;
  FILE *m_pImageYuVFile;
  int m_iRotation;

public:

  virtual ~VideoAImageReader();
  VideoAImageReader();
  ANativeWindow*  getRecordingSurface();

   bool Init(int iWidth, int iHeight, int rotation);
   bool ProcessAImageBuffers(uint8_t *pFramebuffer, int *iDataLength);
   int GetSleepTime(uint64_t iCurrentTime, int32_t iFrameRate);

};