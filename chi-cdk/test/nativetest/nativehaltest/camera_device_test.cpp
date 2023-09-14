//*******************************************************************************
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera_device_test.h"
#include "capture_request_test.h"

namespace haltests
{

    CameraDeviceTest::CameraDeviceTest()
    {}

    CameraDeviceTest::~CameraDeviceTest()
    {}

    void CameraDeviceTest::Setup()
    {
        HalTestCase::Setup();
    }

    void CameraDeviceTest::Teardown()
    {
        HalTestCase::Teardown();
    }

    /**
    * Private function to check the expected default setting values for a given template
    */
    void CameraDeviceTest::CheckRequestForTemplate(
        int cameraId,
        const camera_metadata_t *templateRequest,
        int reqTemplate,
        camera_info info)
    {
        int val = HalTestCase::FindCameraMetadataFromTemplate(cameraId, const_cast<camera_metadata_t*>(templateRequest),
            ANDROID_CONTROL_MODE);
        NT_ASSERT(0 == val, "Can't find ANDROID_CONTROL_MODE for this template.");

        if (reqTemplate != CAMERA3_TEMPLATE_MANUAL)
        {
            camera_metadata_entry_t entry;
            int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_MODE, &entry);
            NT_EXPECT(0 == val,"Can't find the metadata entry for ANDROID_CONTROL_MODE.");
            uint8_t mode = entry.data.u8[0];
            NT_EXPECT(mode == ANDROID_CONTROL_MODE_AUTO, "Control mode is expected to be AUTO for non-manual template!");
        }

        //Check 3A settings--AE/AWB/AF
        camera_metadata_entry_t entry3A;
        NT_ASSERT(0 == find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(
                info.static_camera_characteristics),
            ANDROID_CONTROL_MAX_REGIONS, &entry3A), "ANDROID_CONTROL_MAX_REGIONS.");

        int maxRegionsAe = entry3A.data.u8[0] != 0 ? entry3A.data.u8[0] : 0;
        int maxRegionsAwb = entry3A.data.u8[1] != 0 ? entry3A.data.u8[1] : 0;
        int maxRegionsAf = entry3A.data.u8[2] != 0 ? entry3A.data.u8[2] : 0;

        CheckFPSRange(cameraId, templateRequest, reqTemplate, info);
        CheckAFMode(cameraId, templateRequest, reqTemplate, info);
        CheckAntiBandingMode(cameraId, templateRequest, reqTemplate, info);

        if (reqTemplate == CAMERA3_TEMPLATE_MANUAL)
        {
            camera_metadata_entry_t entry;
            int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_MODE, &entry);
            NT_EXPECT(0 == val,"Can't find the metadata entry for ANDROID_CONTROL_MODE.");
            NT_EXPECT(entry.data.u8[0] == ANDROID_CONTROL_MODE_OFF,"For Manual template the default control mode should be off.");

            val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AE_MODE, &entry);
            NT_EXPECT(0 == val,"Can't find the metadata entry for ANDROID_AE_MODE.");
            NT_EXPECT(entry.data.u8[0] == ANDROID_CONTROL_AE_MODE_OFF, "For Manual template the default control AE mode should be off.");

            val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AWB_MODE, &entry);
            NT_EXPECT(0 == val,"Can't find the metadata entry for ANDROID_AWB_MODE.");
            NT_EXPECT(entry.data.u8[0] == ANDROID_CONTROL_AWB_MODE_OFF, "For Manual template the default control AWBmode should be off.");
        }
        else
        {
            camera_metadata_entry_t entryRegion;
            if (maxRegionsAe > 0)
            {
                int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AE_REGIONS, &entryRegion);
                NT_EXPECT(0 == val, "Can't find the metadata entry for ANDROID_CONTROL_AE_REGIONS.");
                int32_t x_min = entryRegion.data.i32[0];
                int32_t y_min = entryRegion.data.i32[1];
                int32_t x_max = entryRegion.data.i32[2];
                int32_t y_max = entryRegion.data.i32[3];

                NT_EXPECT(x_max > x_min && y_max > y_min,"Invalid AE region");
            }
            if (maxRegionsAwb > 0)
            {
                int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AWB_REGIONS, &entryRegion);
                NT_EXPECT(0 == val, "Can't find the metadata entry for ANDROID_CONTROL_AWB_REGIONS.");
            }
            if (maxRegionsAf > 0)
            {
                int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AF_REGIONS, &entryRegion);
                NT_EXPECT(0 == val, "Can't find the metadata entry for ANDROID_CONTROL_AF_REGIONS.");
            }

        }

    }

    void CameraDeviceTest::CheckFPSRange(
        int cameraId,
        const camera_metadata_t *templateRequest,
        int reqTemplate,
        camera_info info)
    {
        camera_metadata_entry_t entry;
        int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AE_TARGET_FPS_RANGE, &entry);
        NT_ASSERT(0 == val,"Can't find the metadata entry for CONTROL_AE_TARGET_FPS_RANGE.");

        //int minFPS = entry.data.i32[0];
        int maxFPS = entry.data.i32[1];

        val = HalTestCase::GetCameraMetadataEntryByTag(
            cameraId,
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES,
            &entry);
        NT_EXPECT(0 == val,"Can't find the metadata entry for CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES.");

        NT_ASSERT(entry.count > 0, "Does not find positive FPS ranges =%d", entry.count);

        if (reqTemplate != CAMERA3_TEMPLATE_MANUAL &&
            reqTemplate != CAMERA3_TEMPLATE_STILL_CAPTURE)
        {
            NT_EXPECT(maxFPS >= MIN_FPS_REQUIRED_FOR_STREAMING, "Max fps should be at least %d",MIN_FPS_REQUIRED_FOR_STREAMING);

        }

    }

    void CameraDeviceTest::CheckAFMode(
        int cameraId,
        const camera_metadata_t *templateRequest,
        int reqTemplate,
        camera_info info)
    {
        camera_metadata_entry_t entry;

        if (!CaptureRequestTest::hasFocuser(&info))
        {
            NT_LOG_WARN("Camera is fixed focus, skipping AF mode check...");
            return;
        }

        int val = HalTestCase::GetCameraMetadataEntryByTag(
            cameraId,
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE,
            &entry);
        NT_ASSERT(0 == val,"Can't find the metadata entry for LENS_INFO_MINIMUM_FOCUS_DISTANCE.");

        float distance = entry.data.f[0];

        NT_EXPECT(distance > 0.0f," The value of LENS_INFO_MINIMUM_FOCUS_DISTANCE must be positive!");

        val = HalTestCase::GetCameraMetadataEntryByTag(
            cameraId,
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_CONTROL_AF_AVAILABLE_MODES,
            &entry);
        NT_ASSERT(0 == val, "Can't find the metadata entry for CONTROL_AF_AVAILABLE_MODES.");

        if (entry.count > 0)
        {
            int availableTargets = 0;
            for (size_t i = 0; i < entry.count; i++)
            {
                if (entry.data.u8[i] == ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE ||
                    entry.data.u8[i] == ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO ||
                    entry.data.u8[i] == ANDROID_CONTROL_AF_MODE_OFF ||
                    entry.data.u8[i] == ANDROID_CONTROL_AF_MODE_AUTO)
                {
                    availableTargets++;
                }
            }

            NT_ASSERT(availableTargets == 4,"Cannot find the expected AF modes");

        }

        camera_metadata_entry_t targetEntry;
        val = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(templateRequest), ANDROID_CONTROL_AF_MODE, &targetEntry);
        NT_ASSERT(0 == val, "Can't find the metadata entry for ANDROID_CONTROL_AF_MODE in the capture request.");

        uint8_t targetAFMode = targetEntry.data.u8[0];

        if (reqTemplate == CAMERA3_TEMPLATE_PREVIEW ||
            reqTemplate == CAMERA3_TEMPLATE_STILL_CAPTURE ||
            reqTemplate == CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG)
        {
            NT_EXPECT(targetAFMode == ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE, " Default AF Control Mode should be ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE.");
        }
        else if (reqTemplate == CAMERA3_TEMPLATE_VIDEO_RECORD ||
            reqTemplate == CAMERA3_TEMPLATE_VIDEO_SNAPSHOT)
        {
            NT_EXPECT(targetAFMode == ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO," For video the default AF Control Mode should be CONTROL_AF_MODE_CONTINUOUS_VIDEO.");
        }
        else if (reqTemplate == CAMERA3_TEMPLATE_MANUAL)
        {
            NT_EXPECT(targetAFMode == ANDROID_CONTROL_AF_MODE_OFF," For Manual template the default AF Control Mode should be CONTROL_AF_MODE_OFF.");
        }

    }

    void CameraDeviceTest::CheckAntiBandingMode(
        int cameraId,
        const camera_metadata_t *templateRequest,
        int reqTemplate,
        camera_info info)
    {
        if (reqTemplate == CAMERA3_TEMPLATE_MANUAL)
        {
            return;
        }
        camera_metadata_entry_t entry;
        int val = HalTestCase::GetCameraMetadataEntryByTag(cameraId,
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES,
            &entry);
        NT_ASSERT(0 == val, "Can't find the metadata entry for ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES.");

        bool foundAutoMode = false;
        if (entry.count > 0)
        {
            camera_metadata_entry_t entryMode;
            val = HalTestCase::GetCameraMetadataEntryByTag(
                cameraId,
                const_cast<camera_metadata_t*>(templateRequest),
                ANDROID_CONTROL_AE_ANTIBANDING_MODE,
                &entryMode);
            NT_ASSERT(0 == val,"Can't find the metadata entry for CONTROL_AE_ANTIBANDING_MODE in the capture request.");

            for (size_t i = 0; i < entry.count; i++)
            {

                if (entry.data.u8[i] == ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO)
                {
                    foundAutoMode = true;
                    break;
                }
            }

            if (foundAutoMode)
            {
                NT_EXPECT(entryMode.data.u8[0] == ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO,"Default mode should be ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO");
            }
            else
            {
                NT_EXPECT(entryMode.data.u8[0] == ANDROID_CONTROL_AE_ANTIBANDING_MODE_50HZ ||
                    entryMode.data.u8[0] == ANDROID_CONTROL_AE_ANTIBANDING_MODE_60HZ,
                    "Default mode should be either CONTROL_AE_ANTIBANDING_MODE_50HZ or CONTROL_AE_ANTIBANDING_MODE_60HZ");
            }
        }
    }



    /**
    * A CTS test to verify Capture Template based on camera ID
    */
    void CameraDeviceTest::TestCaptureTemplateByCamera()
    {

        std::vector<int> camList = HalTestCase::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != HalTestCase::GetCamModule()->get_camera_info, "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0 == HalTestCase::GetCamModule()->get_camera_info(cameraId, &info), "Can't get camera info for camera id = %u", cameraId);

            // open camera
            HalTestCase::OpenCamera(cameraId);

            for (int j = CAMERA3_TEMPLATE_PREVIEW; j < CAMERA3_TEMPLATE_COUNT; j++)  // j=1 , j<7
            {
                const camera_metadata_t *templateRequest;
                camera3_device_t* pCamDev = GetCamDevice(cameraId);
                NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
                templateRequest = pCamDev->ops->construct_default_request_settings(pCamDev, j);

                NT_ASSERT(templateRequest != nullptr, "Not Supported: the template = %d", j);

                CheckRequestForTemplate(cameraId, templateRequest, j, info);

            }

            // close camera
            HalTestCase::CloseCamera(cameraId);
        }
    }


}
