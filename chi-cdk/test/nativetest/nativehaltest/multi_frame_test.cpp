//*************************************************************************************************
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

/*
*@file multi_frame_test.cpp
*@brief Source file for multi frame test
*******************************************************************
*/

/*
Desc:
Performs simultaneous capture on multiple cameras
*/


#include "multi_frame_test.h"
#include<unordered_set>
#include<random>


namespace haltests
{

    MultiFrameTest::MultiFrameTest()
    {}

    MultiFrameTest::~MultiFrameTest()
    {}

    void MultiFrameTest::Setup()
    {
        HalTestCase::Setup();
        NT_ASSERT(Camera3SuperModule::VerifySupportedPlatform(HalCmdLineParser::g_platform), "Unsupported platform: [%d ]. Please use --h to lookup supported platforms.", HalCmdLineParser::g_platform);
    }

    void MultiFrameTest::OpenCamera(int cameraId)
    {
        HalTestCase::OpenCamera(cameraId);
    }

    int MultiFrameTest::ConfigStream(
        int cameraId,
        int format,
        Size resolution,
        bool setResourceCheck
    )
    {
        HalTestCase::CreateStream(
            cameraId,
            CAMERA3_STREAM_OUTPUT,
            resolution,
            0,
            format,
            CAMERA3_STREAM_ROTATION_0);

        return HalTestCase::ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, setResourceCheck);
    }

    void MultiFrameTest::TestOpenMultipleCameras(std::vector<int> camIDList, int format, Size resolution, int num_iterations)
    {
        std::vector<std::thread> threadGroup;
        for (int iteration = 0; iteration < num_iterations; iteration++)
        {
            bool setResourceCheck = true;
            NT_LOG_INFO("Open/Close cycle iteration: %d", iteration + 1);
            for (size_t camIndex = 0; camIndex < camIDList.size(); camIndex++)
            {
                OpenCamera(camIDList[camIndex]);
                camera3_device_t* camDevice = HalTestCase::GetCamDevice(camIDList[camIndex]);
                // Close camera and delete object instance from map if camDevice is null
                if (camDevice == nullptr)
                {
                    // Close all cameras opened before and including the current one
                    for (size_t index = 0; index <= camIndex; index++)
                    {
                        HalTestCase::CloseCamera(camIDList[index]);
                    }
                }
                NT_ASSERT(nullptr!= camDevice,"Cannot get camera device with id : %d", camIDList[camIndex]);
                int res = ConfigStream(camIDList[camIndex], format, resolution, setResourceCheck);
                NT_ASSERT(res == 0,"Config stream failed on cam %d",camIDList[camIndex]);
                // Override resource cost validation for just one time
                setResourceCheck = false;
            }
            for (size_t camIndex = 0; camIndex < camIDList.size(); camIndex++)
            {
                std::stringstream fileName;
                fileName << "Test" << camIDList[camIndex] << "YUVCapture";

                std::vector<Cam3Device::streamRequestType> streamReqType;
                streamReqType.push_back(
                    //| requestType                 | format | skipFrame | sendFrame | intervalType | numStreams | fileName        | enableZSL | dumpBuffer |
                    { CAMERA3_TEMPLATE_STILL_CAPTURE, {format},  -1,       -1,         NOINTERVAL,      1,          { fileName.str() }, false,      { true }  }
                );
                threadGroup.push_back(std::thread(&MultiFrameTest::TestSingleCameraCaptureAsync, this, camIDList[camIndex], streamReqType));
            }
            for (auto& thread : threadGroup)
            {
                if (thread.joinable())
                {
                    thread.join();
                }
            }
            for (size_t camIndex = 0; camIndex < camIDList.size(); camIndex++)
            {
                CloseCamera(camIDList[camIndex]);
            }
        }
    }

    void MultiFrameTest::CloseCamera(int cameraId)
    {
        HalTestCase::FlushStream(cameraId);
        HalTestCase::CloseCamera(cameraId);
    }

    void MultiFrameTest::TestOpenAllCameras(std::vector<int> camIDList, int format, Size resolution)
    {
        for (size_t camIndex = 0; camIndex < camIDList.size(); camIndex++)
        {
            OpenCamera(camIDList[camIndex]);
            int res = ConfigStream(camIDList[camIndex], format, resolution);
            NT_ASSERT(res == 0, "Config stream failed on cam %d", camIDList[camIndex]);
            std::stringstream fileName;
            fileName << "Test" << camIDList[camIndex] << "YUVCapture";

            std::vector<Cam3Device::streamRequestType> streamReqType;
            streamReqType.push_back(
                //| requestType                   | format | skipFrame | sendFrame | intervalType | numStreams | fileName         | enableZSL | dumpBuffer |
                { CAMERA3_TEMPLATE_STILL_CAPTURE, { format },  -1,         -1,       NOINTERVAL,      1,          { fileName.str() }, false,      { true } }
            );
            TestSingleCameraCaptureAsync(camIDList[camIndex], streamReqType);
            CloseCamera(camIDList[camIndex]);
        }
    }

    std::vector<int> MultiFrameTest::FilterCamListOnIFE(std::vector<int> camList, int nIFEs, std::map<int, CameraMapConfiguration> map)
    {
        std::vector<int> filteredList;
        for (size_t index = 0; index < camList.size(); index++)
        {
            int camId = camList[index];
            if (map[camId].logicalCameraType)
            {
                if (map[camId].camList.size() < static_cast<unsigned int>(nIFEs))
                {
                    filteredList.push_back(camId);
                }
            }
            else
            {
                filteredList.push_back(camId);
            }
        }
        return filteredList;
    }

    void MultiFrameTest::TestOpenRandomMultipleCameras(int format, Size resolution, int num_iterations)
    {
        std::vector<int> camList = Camera3SuperModule::GetCameraList();
        std::map<int, CameraMapConfiguration> map = Camera3SuperModule::GetCameraMap(HalCmdLineParser::g_platform);
        int nIFEs = Camera3SuperModule::GetNumOfIFEs(HalCmdLineParser::g_platform);

        std::vector<int> randomCamList;
        std::unordered_set<int> cannotOpenCamList;
        NT_LOG_INFO("Random seed value set to: %d", HalCmdLineParser::g_randomseed);

        // Filter valid camList based on the number of IFEs
        std::vector<int> validCamList = FilterCamListOnIFE(camList, nIFEs, map);
        // Initialize random engine with the random seed provided
        std::default_random_engine eng(HalCmdLineParser::g_platform);
        for (int iteration = 0; iteration < num_iterations; iteration++)
        {
            NT_LOG_INFO( "Random iteration: %d", iteration + 1);
            // Randomly shuffle camera ID list based on seed provided
            shuffle(validCamList.begin(), validCamList.end(), eng);
            int count = 0;
            for (size_t camIndex = 0; camIndex < validCamList.size(); camIndex++)
            {
                bool pushCam = true;
                std::vector<int> mapValueCamList = map[validCamList[camIndex]].camList;
                for (size_t index = 0; index < mapValueCamList.size(); index++)
                {
                    if (cannotOpenCamList.count(mapValueCamList[index]) != 0)
                    {
                        pushCam = false;
                        break;
                    }
                }
                if (pushCam)
                {
                    for (size_t index = 0; index < mapValueCamList.size(); index++)
                    {
                        cannotOpenCamList.insert(mapValueCamList[index]);
                    }
                    if (count < nIFEs)
                    {
                        randomCamList.push_back(validCamList[camIndex]);
                        count++;
                    }
                }
            }
            TestOpenMultipleCameras(randomCamList, format, resolution, 1);
            randomCamList.clear();
            cannotOpenCamList.clear();
        }
    }

    /**************************************************************************************************************************
    * MultiFrameTest::TestSingleCameraCaptureAsync
    *
    * @brief
    *   This is main entry function which Opens a camera, sets up stream on each camera and start capture on
    *   separate threads
    * @return
    *   void
    **************************************************************************************************************************/
    void MultiFrameTest::TestSingleCameraCaptureAsync(
        uint32_t cameraId,
        std::vector<Cam3Device::streamRequestType> streamReqType
    )
    {
        HalTestCase::SendContinuousPCR(
            cameraId,
            streamReqType, // requestType,
            0); // output stream index,
    }
}