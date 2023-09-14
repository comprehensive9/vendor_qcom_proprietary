//******************************************************************************
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera_module_test.h"

namespace chitests
{
/**************************************************************************************************
*   CameraModuleTest::Setup
*
*   @brief
*       Entry point to the test
**************************************************************************************************/
VOID CameraModuleTest::Setup()
{
    m_pChiModule = ChiModule::GetInstance();
}

/**************************************************************************************************
*   CameraModuleTest::Teardown
*
*   @brief
*       Teardown point to the test
**************************************************************************************************/
VOID CameraModuleTest::Teardown()
{
    if (m_pChiModule != nullptr)
    {
        m_pChiModule->DestroyInstance();
        m_pChiModule = nullptr;
    }
}

/**************************************************************************************************
*   CameraModuleTest::TestNumberOfCamera
*
*   @brief
*       Test Module reports at least one camera
**************************************************************************************************/
void CameraModuleTest::TestNumberOfCamera()
{
    NT_ASSERT((0 != m_pChiModule->GetNumberCameras()),"No cameras found");
}

    /**************************************************************************************************
    *   CameraModuleTest::TestGetCameraInfo
    *
    *   @brief
    *       Test module returns camera info for requested cameraId
    **************************************************************************************************/
    void CameraModuleTest::TestGetCameraInfo()
    {
        int numOfCameras = m_pChiModule->GetNumberCameras();
        NT_ASSERT(0 < numOfCameras, "No cameras found");

        for (int i = 0; i < numOfCameras; i++)
        {
            const CHICAMERAINFO *info = m_pChiModule->GetCameraInfo(i);
            NT_ASSERT(info != nullptr, "Failed to obtain camera info for cameraId: %d", i);
        }
    }

    /**************************************************************************************************
    *   CameraModuleTest::TestStaticCameraInfoAndMetadata
    *
    *   @brief
    *       Verify static metadata info returned by module
    **************************************************************************************************/
    void CameraModuleTest::TestStaticCameraInfoAndMetadata()
    {
        for (int i = 0; i < m_pChiModule->GetNumberCameras(); ++i)
        {
            const CHICAMERAINFO* chiInfo;
            chiInfo = m_pChiModule->GetCameraInfo(i);
            NT_ASSERT(nullptr != chiInfo, "Can't get camera info for %d", i);
            camera_info* info = reinterpret_cast<camera_info*>(chiInfo->pLegacy);
            int orientation = info->orientation;
            NT_ASSERT(orientation == 0 || orientation == 90 || orientation == 180 || orientation == 270,
                "camera orientation is not correct!");
            int facing = info->facing;
            NT_ASSERT(facing == CAMERA_FACING_BACK || facing == CAMERA_FACING_FRONT ||
                facing == CAMERA_FACING_EXTERNAL, "camera facing is not correct!");


            camera_metadata_entry_t  entry;
            NT_ASSERT(0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info->static_camera_characteristics),
                ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE, &entry),
                 "Can't find the sensor pixel array size.");

            int pixel_array_w = entry.data.i32[0];
            int pixel_array_h = entry.data.i32[1];

            NT_ASSERT(0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info->static_camera_characteristics),
                ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE, &entry),
                 "Can't find the sensor active array size.");

            int active_array_w = entry.data.i32[0];
            int active_array_h = entry.data.i32[1];

            NT_EXPECT(active_array_h <= pixel_array_h, "Condition failed : active_array_h <= pixel_array_h");
            NT_EXPECT(active_array_w <= pixel_array_w, "Condition failed : active_array_w <= pixel_array_w");

            NT_ASSERT(0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info->static_camera_characteristics),
                ANDROID_SENSOR_ORIENTATION, &entry),
                 "Can't find the sensor orientation.");


        }
    }
}