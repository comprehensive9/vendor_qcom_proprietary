/*****************************************************************************

============================
Copyright (c)  2016-2018  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VTAImageReader.cpp
 Description: Implementation of VideoAImageReader

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 25-Mar-2020   Narasimha      First Version
 *****************************************************************************/
#include <VTAImageReader.h>
#include <qplog.h>
#include <string.h>


/******************************************************************************
 FUNCTION     : VideoAImageReader
 ARGUMENTS    : None
 RETURN VALUE : VideoAImageReader Object
 PURPOSE      : This function creates object of VideoAImageReader object
******************************************************************************/
VideoAImageReader::VideoAImageReader()
{
  m_pImageReaderNativeWindow = NULL;
  m_pImageReader = NULL;
  m_pAImg = NULL;
  m_pImageYuVFile = NULL;
  /* test code to record frame into file
   * if(m_pImageYuVFile == NULL)
  {
   m_pImageYuVFile = fopen("/storage/Movies/AImageYuVFile.yuv", "wb");
  }
  else
  {
    CRITICAL1("ProcessAImageBuffers:m_pAImg is open failed");
  }*/
  CRITICAL1("H263 VideoAImageReader Instance created");
}

/******************************************************************************
 FUNCTION     : ~VideoAImageReader
 ARGUMENTS    : None
 RETURN VALUE : None
 PURPOSE      : This function deletes object of VideoAImageReader object
******************************************************************************/
VideoAImageReader::~VideoAImageReader()
{
  if (m_pImageYuVFile != NULL) {
    fflush(m_pImageYuVFile);
    fclose(m_pImageYuVFile);
    m_pImageYuVFile = NULL;
  }

  if (m_pAImg != NULL) {
    AImage_delete(m_pAImg);
  }

  if (m_pImageReader != NULL) {
    AImageReader_delete(m_pImageReader);
  }
}

/******************************************************************************
 FUNCTION     : Init
 ARGUMENTS    : Frame width and Height
 RETURN VALUE : None
 PURPOSE      : This function initializes the AImageReader framework object and
                creates recording Native surface window to capture camera frames
******************************************************************************/
bool VideoAImageReader::Init(int iWidth, int iHeight, int iRotation)
{
  int iReturnValue;
  uint64_t iUsage = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN;
  int32_t iFormat = AIMAGE_FORMAT_YUV_420_888;
  iReturnValue = AImageReader_newWithUsage(iWidth, iHeight, iFormat, iUsage, 2,
                 &m_pImageReader);
  CRITICAL4("Init:m_iRotation width %d height %d rotation %d", iWidth, iHeight,
            iRotation);

  if (iReturnValue != AMEDIA_OK || m_pImageReader == NULL) {
    ALOGE("Failed to create Image reader Object, ret=%d", iReturnValue);
    return false;
  }

  iReturnValue = AImageReader_getWindow(m_pImageReader,
                                        &m_pImageReaderNativeWindow);

  if (iReturnValue != AMEDIA_OK || m_pImageReaderNativeWindow == NULL) {
    ALOGE("Failed to get ANativeWindow from AImageReader, ret=%d, "
          "Native Window=%p.", iReturnValue, m_pImageReaderNativeWindow);
    return false;
  }

  m_iRotation = iRotation;
  return true;
}

/******************************************************************************
 FUNCTION     : getRecordingSurface
 ARGUMENTS    : JNI envirnoment variable
 RETURN VALUE : A Native window
 PURPOSE      : Purpose of this function is to return recording Native window
                surface to camera to flush the preview frames
******************************************************************************/
ANativeWindow*  VideoAImageReader::getRecordingSurface()
{
  CRITICAL2(" H263 getRecordingSurface NativeWindow %p",
            m_pImageReaderNativeWindow);
  return m_pImageReaderNativeWindow;
}

/******************************************************************************
 FUNCTION     : ProcessAImageBuffers
 ARGUMENTS    : Frame buffer data pointer, Frame data length
 RETURN VALUE : integer value of function status
 PURPOSE      : This function acquires frames from camera via native window, reads invidual data planes.
                calculates frame data length based on the configured fram width and height.
                rotates invidula data planes. first rotate Y plane and chooses equivalent
                U, V plannes based on the planer.
******************************************************************************/
bool VideoAImageReader::ProcessAImageBuffers(uint8_t *pFramebuffer,
    int *iDataLength)
{
  int retValue = -1;
  int yPlaneIndex = 0;
  int uPlaneIndex = 1;
  int vPlaneIndex = 2;
  IMAGE_UV_DATA stYUVData;
  QP_UNUSED(iDataLength);

  if (m_pAImg != NULL) {
    ALOGE("Something wrong prev buffer not cleared %d", retValue);
    AImage_delete(m_pAImg);
    m_pAImg = NULL;
    return false;
  }

  retValue = AImageReader_acquireNextImage(m_pImageReader, &m_pAImg);

  if (retValue != AMEDIA_OK || m_pAImg == NULL) {
    ALOGE("%s: Acquire image from reader %p failed! ret: %d, img %p", __FUNCTION__,
          m_pImageReader, retValue, m_pAImg);
    return false;
  }

  int32_t iFormat = -1;
  retValue = AImage_getFormat(m_pAImg, &iFormat);

  if (retValue != AMEDIA_OK || iFormat == -1) {
    CRITICAL3("H263 Get format for failed! ret: %d, format %d",  retValue, iFormat);
    return false;
  }

  if (iFormat != AIMAGE_FORMAT_YUV_420_888) {
    ALOGE("Format %d not supported", iFormat);
    AImage_delete(m_pAImg);
    m_pAImg = NULL;
    return false;
  }

  int yPlaneLength, cbLength, crLength;
  int Imageheight = 0;
  int Imagewidth = 0;
  //Fetch Y_data plane information.
  AImage_getPlanePixelStride(m_pAImg, yPlaneIndex, &stYUVData.yPlanePixelStride);
  AImage_getPlaneRowStride(m_pAImg, yPlaneIndex, &stYUVData.yPlaneRowStride);
  AImage_getPlaneData(m_pAImg, yPlaneIndex, &stYUVData.yPlanedataPtr,
                      &yPlaneLength);
  AImage_getWidth(m_pAImg, &Imagewidth);
  AImage_getHeight(m_pAImg, &Imageheight);
  stYUVData.iWidth = Imagewidth;
  stYUVData.iHeight = Imageheight;
  // uncomment to debug the issue
  //CRITICAL3("H263 ProcessAImageBuffers:yPixelStride %d yPlaneRowStride %d", stYUVData.yPlanePixelStride, stYUVData.yPlaneRowStride);
  // CRITICAL5("H263 ProcessAImageBuffers: memory buf %p height %d  width %d m_iRotation %d", pFramebuffer, stYUVData.Imageheight, stYUVData.Imagewidth, m_iRotation);
  AImage_getPlanePixelStride(m_pAImg, uPlaneIndex, &stYUVData.cbPlanePixelStride);
  AImage_getPlaneRowStride(m_pAImg, uPlaneIndex, &stYUVData.cbPlaneRowStride);
  AImage_getPlaneData(m_pAImg, uPlaneIndex, &stYUVData.cbDataPtr, &cbLength);
  AImage_getPlanePixelStride(m_pAImg, vPlaneIndex, &stYUVData.crPlanePixelStride);
  AImage_getPlaneRowStride(m_pAImg, vPlaneIndex, &stYUVData.crPlaneRowStride);
  AImage_getPlaneData(m_pAImg, vPlaneIndex, &stYUVData.crDataPtr, &crLength);
  // uncomment to debug the issue
  //CRITICAL5("H263 RoatateUVPlane: cbRowStride %d  cbPixelStride %d crRowStride %d  crPixelStride %d", stYUVData.cbPlaneRowStride, stYUVData.cbPlanePixelStride, stYUVData.crPlaneRowStride, stYUVData.crPlanePixelStride);
  RotateYUVPlanesByMountAngle(pFramebuffer, &stYUVData, m_iRotation);

  //CRITICAL4("H263 AImage reader ret: %d, len %d pFramebuffer %p", retValue, *iDataLength, pFramebuffer);

  if (m_pAImg) {
    AImage_delete(m_pAImg);
    m_pAImg = NULL;
  }

  return true;
}


int VideoAImageReader::GetSleepTime(uint64_t iCurrentTime, int32_t iFrameRate)
{
  uint64_t iAfterTime, iDiffTime;
  iAfterTime = getCurrentSystemTime(NULL);
  iDiffTime = iAfterTime - iCurrentTime;
  int64_t iSleepInMs = 0;

  /* Calculating the sleep time of the thread using the fps */
  if (iFrameRate) {
    iSleepInMs =  (int64_t)(1000 / iFrameRate) - (int64_t)iDiffTime;
  }

  /* If sleep time is coming as zero or negative, */
  if (iSleepInMs <= 0) {
    iSleepInMs = 33;
  }

  return (int)iSleepInMs;
}