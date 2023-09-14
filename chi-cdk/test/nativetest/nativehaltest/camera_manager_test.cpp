//*******************************************************************************
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera_manager_test.h"

namespace haltests
{

    CameraManagerTest::CameraManagerTest()
    {}

    CameraManagerTest::~CameraManagerTest()
    {}

    void CameraManagerTest::Setup()
    {
        HalTestCase::Setup();

        int camNum = HalTestCase::GetNumCams();
        NT_ASSERT(0< camNum, "No cameras found");
    }

    void CameraManagerTest::Teardown()
    {
        HalTestCase::Teardown();
    }

    /**
    * Negative test to expect opening camera with invalid ID
    */
    void CameraManagerTest::TestOpenInvalidCamera()
    {
        NT_ASSERT(nullptr != HalTestCase::GetCamModule()->common.methods->open,"Camera open() is unimplemented");
        int invalidId = -1;

        camera_info info;
        NT_ASSERT(0 != HalTestCase::GetCamModule()->get_camera_info(invalidId, &info),"Shouldn't get camera info for camera id -1");

        hw_device_t* mDevice = nullptr;
        NT_ASSERT(0 != HalTestCase::GetCamModule()->common.methods->open(
            (const hw_module_t*)HalTestCase::GetCamModule(), std::to_string(invalidId).c_str(), &mDevice),"Can still open camera device with invalid id -1 ");
    }

    /**
    * Tests that each camera device can be opened one at a time, several times.
    */
    void CameraManagerTest::TestOpenCamerasSerially()
    {
        std::vector<int> camList = HalTestCase::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            for (int j = 0; j < NUM_CAMERA_REOPENS; j++)
            {
                HalTestCase::OpenCamera(cameraId);
                HalTestCase::CloseCamera(cameraId);
            }

        }
    }

    /**
    * Tests one or more camera devices can be open at the same time, or the right error state
    * is set if this can't be done.
    */
    void CameraManagerTest::TestOpenAllCamerasFrontFirst()
    {
        // first open front sensor
        camera_info info;
        NT_ASSERT(0== GetCamModule()->get_camera_info(1, &info),"Can't get camera info for %d", 1);
        HalTestCase::OpenCamera(1);
        //next open back sensor
        NT_ASSERT(0== GetCamModule()->get_camera_info(0, &info),"Can't get camera info for %d", 0);
        HalTestCase::OpenCamera(0);

        HalTestCase::CloseCamera(1);
        HalTestCase::CloseCamera(0);
    }

    void CameraManagerTest::TestOpenAllCamerasBackFirst()
    {
        // first open back sensor
        camera_info info;
        NT_ASSERT(0==GetCamModule()->get_camera_info(0, &info),"Can't get camera info for %d" , 0);
        HalTestCase::OpenCamera(0);
        //next open front sensor
        NT_ASSERT(0==GetCamModule()->get_camera_info(1, &info),"Can't get camera info for %d" , 1);
        HalTestCase::OpenCamera(1);

        HalTestCase::CloseCamera(0);
        HalTestCase::CloseCamera(1);
    }



}