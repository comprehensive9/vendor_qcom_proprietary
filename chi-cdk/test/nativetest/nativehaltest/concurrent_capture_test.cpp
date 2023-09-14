//*************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

/*
*@file concurrent_capture_test.cpp
*@brief Source file for concurrent capture test
*******************************************************************
*/

/*
Desc:
  Performs simultaneous capture on multiple cameras
*/


#include "concurrent_capture_test.h"

namespace haltests
{

    ConcurrentCaptureTest::ConcurrentCaptureTest()
    {}

    ConcurrentCaptureTest::~ConcurrentCaptureTest()
    {}

    void ConcurrentCaptureTest::Setup()
    {
        Camera3Stream::Setup();
    }
    void ConcurrentCaptureTest::Teardown() {}


    /**************************************************************************************************************************
    * ConcurrentCaptureTest::TestCapture
    *
    * @brief
    *   This is function is executed for every thread spawned based on number of cameras. This function performs snapshot
    *   request for requested number of frames
    * @return
    *   void
    **************************************************************************************************************************/
    void ConcurrentCaptureTest::TestCapture(
        int numCameras,         //[in] number of cameras
        int cameraId,           //[in] camera Id
        int format,             //[in] stream format
        const char* filename,   //[in] filename to be used
        int captureCount)       //[in] capture count
    {
        int startFrame = cameraId + 1;
        int cameraFrame = 1;

        for (uint32_t frameNumber = startFrame; cameraFrame <= captureCount; frameNumber += numCameras, ++cameraFrame)
        {
            std::string outputName = ConstructOutputFileName(cameraId, format, cameraFrame, filename);

            NT_ASSERT(Camera3Stream::ProcessCaptureRequest(
                cameraId,
                frameNumber, // frameNumber
                CAMERA3_TEMPLATE_PREVIEW, // requestType,
                cameraId, // output stream index,
                true,
                outputName.c_str(), // output filename,
                true),
                "There are no more buffers to send next request, so fail the test");
        }

        Camera3Stream::FlushStream(cameraId);
    }

    /**************************************************************************************************************************
    * ConcurrentCaptureTest::TestConcurrentCapture
    *
    * @brief
    *   This is main entry function which Opens given number of cameras, sets up stream on each camera and start capture on
    *   separate threads
    * @return
    *   void
    **************************************************************************************************************************/
    void ConcurrentCaptureTest:: TestConcurrentCapture(
        int numRequiredCameras,  //[in] expected number of cameras
        int format,              //[in] stream format to be used
        Size resolution,         //[in] resolution of the stream
        int numFrames,           //[in] number of frames to be captured
        const char* testName)    //[in] filename to be used
    {
        int numOfCameras = Camera3Module::GetNumCams();
        bool setResourceCostCheck = true;
        if (numRequiredCameras > numOfCameras)
        {
            NT_LOG_UNSUPP("Number of cameras reported (%d) is less than required (%d)", numOfCameras, numRequiredCameras);
            return;
        }

        // TODO: number of IFEs are 3 as of now, more dynamic design is in progress
        if (numRequiredCameras > 3) {
            NT_LOG_UNSUPP("Number of cameras requested (%d) are greater than num of IFEs (%d)", numRequiredCameras, 3);
            return;
        }

        for (int cameraId = 0; cameraId < numRequiredCameras; cameraId++)
        {
            Camera3Device::OpenCamera(cameraId);
            //Adding check to return failure if multiple cameras cannot be opened
            NT_ASSERT(nullptr!= Camera3Device::GetCamDevice(cameraId),"Cannot get camera device with id: %d ", cameraId);
            int streamIndex = cameraId;

            Camera3Stream::CreateStream(
                CAMERA3_STREAM_OUTPUT,
                resolution,
                0,
                format,
                CAMERA3_STREAM_ROTATION_0);

            int res;

            // Once we configure stream for last cameraId we can generate the buffers
            if (cameraId == numRequiredCameras - 1)
            {
                res = Camera3Stream::ConfigStreamSingle(false, streamIndex, cameraId, true, setResourceCostCheck);
            }
            else
            {
                res = Camera3Stream::ConfigStreamSingle(false, streamIndex, cameraId, false, setResourceCostCheck);
            }

            // Override resource cost validation for just one time
            setResourceCostCheck = false;
            NT_ASSERT(res == 0,"Config stream failed on cam %d " ,cameraId);
        }

        // Create one output stream for each camera and start thread
        std::vector<std::thread> cameraThreads(numRequiredCameras);

        //Start threads on each camera and wait here for all threads to finish execution
        for (int cameraId = 0; cameraId < numRequiredCameras; cameraId++)
        {
            cameraThreads.at(cameraId) = std::thread(
                &ConcurrentCaptureTest::TestCapture, this, numRequiredCameras, cameraId, format, testName, numFrames);
        }
        for (auto & t : cameraThreads)
        {
            t.join();
        }

        // close streams
        Camera3Stream::ClearOutStreams();
        // close camera
        for (int cameraId = 0; cameraId < numRequiredCameras; cameraId++)
        {
            Camera3Device::CloseCamera(cameraId);
        }
    }
}
