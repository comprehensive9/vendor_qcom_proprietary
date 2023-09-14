//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera_module_test.h"

namespace haltests
{

    CameraModuleTest::CameraModuleTest()
    {}

    CameraModuleTest::~CameraModuleTest()
    {}

    void CameraModuleTest::Setup()
    {
        HalTestCase::Setup();
    }
    void CameraModuleTest::Teardown()
    {
        HalTestCase::Teardown();
    }

    void CameraModuleTest::TestNumberOfCamera()
    {
        NT_ASSERT(0<HalTestCase::GetNumCams(), "No cameras found");
    }

    void CameraModuleTest::TestGetCameraInfo()
    {
        int numOfCameras = HalTestCase::GetNumCams();
        NT_ASSERT(0<numOfCameras, "No cameras found");

        for (int i = 0; i < numOfCameras; i++)
        {
            camera_info *info = HalTestCase::GetCameraInfoWithId(i);
            NT_ASSERT(info != nullptr, "Failed to obtain camera info for cameraId: %d\n", i);
        }
    }

    void CameraModuleTest::TestStaticCameraInfoAndMetadata()
    {
        for (int i = 0; i < GetNumCams(); ++i)
        {
            NT_ASSERT(nullptr != HalTestCase::GetCamModule()->get_camera_info, "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0==GetCamModule()->get_camera_info(i, &info), "Can't get camera info for %d", i);

            int orientation = info.orientation;
                NT_ASSERT(orientation == 0 || orientation == 90 || orientation == 180 || orientation == 270,"camera orientation is not correct!");

            NT_ASSERT(info.facing == CAMERA_FACING_FRONT || info.facing == CAMERA_FACING_BACK ||
                info.facing == CAMERA_FACING_EXTERNAL, "camera facing is not unknown!");

            camera_metadata_entry_t  entry;
            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info.static_camera_characteristics),
                ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE, &entry),"Can't find the sensor pixel array size.");

            int pixel_array_w = entry.data.i32[0];
            int pixel_array_h = entry.data.i32[1];

            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info.static_camera_characteristics),
                ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE, &entry), "Can't find the sensor active array size.");

            int active_array_w = entry.data.i32[0];
            int active_array_h = entry.data.i32[1];

            NT_EXPECT(active_array_h <= pixel_array_h, "Active array height not set");
            NT_EXPECT(active_array_w <= pixel_array_w, "Active array width not set");

            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info.static_camera_characteristics),
                ANDROID_SENSOR_ORIENTATION, &entry), "Can't find the sensor orientation.");
        }
    }




}