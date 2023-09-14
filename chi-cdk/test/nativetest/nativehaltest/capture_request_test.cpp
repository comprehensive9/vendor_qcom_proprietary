//*****************************************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*****************************************************************************************************************************

#include "capture_request_test.h"
#include <cinttypes>
#include <cmath>
#include "vendor_tags_test.h"
#include <set>

namespace haltests
{

#define PROCESS_CAPTURE_TIMEOUT 10

    CaptureRequestTest::CaptureRequestTest()
    {}

    CaptureRequestTest::~CaptureRequestTest()
    {}

    void CaptureRequestTest::Setup()
    {
        Camera3Stream::Setup();
    }

    void CaptureRequestTest::Teardown() {}


    /**************************************************************************************************************************
    * CaptureRequestTest::SetupPreview()
    *
    * @brief
    *   Helper function to setup a stream to converge 3A, and prepares
    *   hal request for capturerequest key testing
    * @return
    *   None
    **************************************************************************************************************************/
    void CaptureRequestTest::SetupPreview(
        int cameraId,
        int format,
        Size resolution,
        int& frameNumber)
    {

        // Create one output stream
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            resolution,
            0,
            format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) == 0, "Config stream failed");

#ifdef ENABLE3A
        frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
            cameraId,
            frameNumber,
            CAMERA3_TEMPLATE_PREVIEW, // requestType,
            0, // output stream index
            false,
            "");
        NT_ASSERT(frameNumber != -1,"3A did not converge!");
#endif
        CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_STILL_CAPTURE, 1,
                frameNumber);//non fatal
    }


    /**************************************************************************************************************************
    * CaptureRequestTest::isCapabilitySupported()
    *
    * @brief
    *   Checks if a specified capability is supported on device
    * @return
    *   True if supported, false if not
    **************************************************************************************************************************/
    bool CaptureRequestTest::isCapabilitySupported(camera_info *info, uint8_t capability)
    {

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_REQUEST_AVAILABLE_CAPABILITIES failed")
            return false;
        }

        for (size_t i = 0; i < entry.count; i++)
        {
            if (entry.data.u8[i] == capability)
            {
                return true;
            }
        }

        return false;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestExecute()
    *
    * @brief
    *   This is main entry function for CaptureRequestTest, it performs common operations such as
    *   open camera, setting up preview, clearning buffers and closing camera. The function pointer
    *   passed as an argument determines which CaptureRequest test to execute
    * @return
    *   void
    **************************************************************************************************************************/

    void CaptureRequestTest::TestExecute(
        int format,                 // [in] format of the stream
        Size resolution,            // [in] resolution of the stream
        bool bDumpOutputBuffer,     // [in] output image file to be dumped or not
        void(CaptureRequestTest::*TestCaptureRequest) (int, int, int, camera_info*, bool))
        // [in] function pointer to determine which CaptureRequest test to execute
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            if (!CheckSupportedResolution(cameraId, format, resolution))
            {
                NT_LOG_UNSUPP( "Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    format, ConvertResToString(resolution).c_str(), cameraId);
                continue;
            }

            camera_info *info = Camera3Module::GetCameraInfoWithId(cameraId);

            NT_ASSERT(info != nullptr, "Camera Info not set");

            Camera3Device::OpenCamera(cameraId);

            if (!isCapabilitySupported(info, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE))
            {
                NT_LOG_WARN("Device doesn't support color output, skipping..");
                // close camera
                Camera3Device::CloseCamera(cameraId);
                continue;
            }

            int frameNumber = 0;

             SetupPreview(cameraId, format, resolution, frameNumber);//non fatal

            if (frameNumber == -1)
            {
                Camera3Stream::DeleteHalRequest(cameraId);
                Camera3Stream::ClearOutStreams();
                CloseCamera(cameraId);
                continue;
            }

            (this->*(TestCaptureRequest))(cameraId, format, frameNumber, info, bDumpOutputBuffer);


            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::DeleteHalRequest(cameraId);
            Camera3Stream::ClearOutStreams();
            // close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }


    /**************************************************************************************************************************
    * CaptureRequestTest::TestAfModes()
    *
    * @brief
    *   Tests all available AF modes on device with lock and unlock
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAfModes(int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AF_AVAILABLE_MODES,
            &entry);
        NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_CONTROL_AF_AVAILABLE_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            uint8_t afMode = entry.data.u8[j];
            NT_LOG_DEBUG("start testing AF mode value: %d", static_cast<int>(afMode));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AF_MODE, &afMode);
            NT_ASSERT(frameNumber != -1, "Frame numberr not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestAfModes_AfMode" << static_cast<int>(afMode);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestAeModeAndLock()
    *
    * @brief
    *   Tests all available AE modes on device with lock and unlock
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAeModeAndLock(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AE_AVAILABLE_MODES,
            &entry);
        NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_CONTROL_AE_AVAILABLE_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            uint8_t aeMode = entry.data.u8[j];
            NT_LOG_DEBUG("start testing AE mode value: %d", static_cast<int>(aeMode));

            if (aeMode == ANDROID_CONTROL_AE_MODE_OFF)
            {
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_MODE, &aeMode);
                NT_ASSERT(frameNumber != -1 , "Frame number not set");
                TestAeManualControl(cameraId, frameNumber, info, bDumpOutputBuffer, format);
            }
            else
            {
                uint8_t aeLock = ANDROID_CONTROL_AE_LOCK_OFF;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_LOCK, &aeLock);
                NT_ASSERT(frameNumber != -1 , "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_MODE, &aeMode);
                NT_ASSERT(frameNumber != -1 , "Frame number not set");
                aeLock = ANDROID_CONTROL_AE_LOCK_ON;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_LOCK, &aeLock);//non fatal
                NT_ASSERT(frameNumber != -1 , "Frame number not set");
                if (bDumpOutputBuffer)
                {
                    std::ostringstream outputName;
                    outputName << "TestAeModeAndLock_AeMode" << static_cast<int>(aeMode);
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
                }
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestAeManualControl()
    *
    * @brief
    *   Tests manual AE control when AE mode is set to OFF
    *   Tests combinations of sensor exposure time and sensor sensitivity settings
    * @return
    *   void
    **************************************************************************************************************************/

    void CaptureRequestTest::TestAeManualControl(
        int cameraId,
        int frameNumber,
        camera_info * info,
        bool bDumpOutputBuffer,
        int format)
    {

        // Check if manual sensor is supported
        if (!isCapabilitySupported(info, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR))
        {
            NT_LOG_WARN( "Device doesn't support manual sensor, stop test");
            return;
        }

        // obtain list of exposure times to test
        std::vector<int64_t> expTimeTestValues(DEFAULT_NUM_EXPOSURE_TIME_STEPS + 1, 0);

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE failed");

        int64_t minExpTime = entry.data.i64[0];
        int64_t maxExpTime = entry.data.i64[1];
        NT_ASSERT(maxExpTime> minExpTime, "ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE is invalid [%d, %d]", minExpTime, maxExpTime );

        minExpTime = (minExpTime > SENSOR_INFO_EXPOSURE_TIME_RANGE_MIN_AT_MOST)
            ? SENSOR_INFO_EXPOSURE_TIME_RANGE_MIN_AT_MOST : minExpTime;
        maxExpTime = (maxExpTime < SENSOR_INFO_EXPOSURE_TIME_RANGE_MAX_AT_LEAST)
            ? SENSOR_INFO_EXPOSURE_TIME_RANGE_MAX_AT_LEAST : maxExpTime;

        int64_t expTimeRange = maxExpTime - minExpTime;
        double expTimeStepSize = expTimeRange / static_cast<double>(DEFAULT_NUM_EXPOSURE_TIME_STEPS);

        for (int i = 0; i <= DEFAULT_NUM_EXPOSURE_TIME_STEPS; i++)
        {
            expTimeTestValues[i] = maxExpTime - static_cast<int64_t>(expTimeStepSize * i);
            expTimeTestValues[i] = std::max(minExpTime, std::min(maxExpTime, expTimeTestValues[i]));
        }

        // obtain list of sensitivity values to test
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_SENSITIVITY_RANGE,
            &entry);
        NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_SENSITIVITY_RANGE failed");

        int minSensitivity = entry.data.i32[0];
        int maxSensitivity = entry.data.i32[1];
        NT_ASSERT(maxSensitivity > minSensitivity,"ANDROID_SENSOR_INFO_SENSITIVITY_RANGE is invalid [ %d, %d ]", minSensitivity,maxSensitivity);

        minSensitivity = (minSensitivity > SENSOR_INFO_SENSITIVITY_RANGE_MIN_AT_MOST)
            ? SENSOR_INFO_SENSITIVITY_RANGE_MIN_AT_MOST : minSensitivity;
        maxSensitivity = (maxSensitivity < SENSOR_INFO_SENSITIVITY_RANGE_MAX_AT_LEAST)
            ? SENSOR_INFO_SENSITIVITY_RANGE_MAX_AT_LEAST : maxSensitivity;

        int sensitivityRange = maxSensitivity - minSensitivity;
        int sensitivityStepSize = DEFAULT_SENSITIVITY_STEP_SIZE;
        int sensitivityNumSteps = sensitivityRange / sensitivityStepSize;

        // Bound the test steps to avoid supper long test.
        if (sensitivityNumSteps > DEFAULT_NUM_SENSITIVITY_STEPS)
        {
            sensitivityNumSteps = DEFAULT_NUM_SENSITIVITY_STEPS;
            sensitivityStepSize = sensitivityRange / sensitivityNumSteps;
        }

        int * senstivityTestValues = static_cast<int*>(malloc(sizeof(int) * (sensitivityNumSteps + 1)));
        NT_ASSERT(senstivityTestValues != nullptr,"Malloc failed");

        for (int i = 0; i <= sensitivityNumSteps; i++)
        {
            senstivityTestValues[i] = maxSensitivity - sensitivityStepSize * i;
            senstivityTestValues[i] = std::max(minSensitivity, std::min(maxSensitivity, senstivityTestValues[i]));
        }

        // Submit single request at a time, then verify the result.
        for (int i = 0; i <= DEFAULT_NUM_EXPOSURE_TIME_STEPS; i++)
        {
            for (int j = 0; j <= sensitivityNumSteps; j++)
            {
                NT_LOG_DEBUG("start testing manual AE with exposure time: %" PRId64"ns, sensitivity: %d"
                    , expTimeTestValues[i], senstivityTestValues[j]);

                frameNumber = ApplyAndVerifyKeySetting(cameraId,
                    frameNumber,
                    ANDROID_SENSOR_EXPOSURE_TIME,
                    &expTimeTestValues[i],
                    1,
                    EXPOSURE_TIME_ERROR_MARGIN_RATE);

                NT_ASSERT(frameNumber != -1  , "Frame Number not set");

                frameNumber = ApplyAndVerifyKeySetting(cameraId,
                    frameNumber,
                    ANDROID_SENSOR_SENSITIVITY,
                    &senstivityTestValues[j],
                    1,
                    SENSITIVITY_ERROR_MARGIN_RATE);

                NT_ASSERT(frameNumber != -1, "Frame Number not set");

                if (bDumpOutputBuffer)
                {
                    std::ostringstream outputName;
                    outputName << "TestAeManualControl_ExpTime" << expTimeTestValues[i] << "_Sensitivity" <<
                        senstivityTestValues[j];
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(i, format, -1,
                        outputName.str().c_str()).c_str());
                }
            }
        }

        free(senstivityTestValues);
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestAwbModeAndLock()
    *
    * @brief
    *   Tests all available AWB modes on device with lock and unlock
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAwbModeAndLock(int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AWB_AVAILABLE_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AWB_AVAILABLE_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            uint8_t awbMode = entry.data.u8[j];
            NT_LOG_DEBUG("start testing AWB mode value: %d", static_cast<int>(awbMode));

            if (awbMode == ANDROID_CONTROL_AWB_MODE_AUTO)
            {
                uint8_t awbLock = ANDROID_CONTROL_AWB_LOCK_OFF;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AWB_LOCK, &awbLock);
                NT_ASSERT(frameNumber != -1, "Frame not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AWB_MODE, &awbMode);
                NT_ASSERT(frameNumber != -1, "Frame not set");
                awbLock = ANDROID_CONTROL_AWB_LOCK_ON;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AWB_LOCK, &awbLock);
                NT_ASSERT(frameNumber != -1, "Frame not set");
            }
            else
            {
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AWB_MODE, &awbMode);
                NT_ASSERT(frameNumber != -1, "Frame not set");
            }

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestAwbModeAndLock_AwbMode" << static_cast<int>(awbMode);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestLensShadingMap()
    *
    * @brief
    *   Tests lens shading mode and lens shading map
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestLensShadingMap(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer)
    {

        if (!IsCaptureRequestKeySupported(info, ANDROID_STATISTICS_LENS_SHADING_MAP_MODE))
        {
            NT_LOG_WARN("Device doesn't support ANDROID_STATISTICS_LENS_SHADING_MAP_MODE, skip");
            return;
        }

        camera_metadata_entry_t lensShadingMapEntry;

        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES,
            &lensShadingMapEntry);
        NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES failed");


        for (size_t j = 0; j < lensShadingMapEntry.count; j++)
        {
            uint8_t lensShadingMapMode = lensShadingMapEntry.data.u8[j];
            NT_LOG_DEBUG( "start testing Lens Shading Map mode value: %d", static_cast<int>(lensShadingMapMode));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_STATISTICS_LENS_SHADING_MAP_MODE,
                &lensShadingMapMode);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            // test shading mode together
            if (lensShadingMapMode == ANDROID_STATISTICS_LENS_SHADING_MAP_MODE_ON)
            {
                camera_metadata_entry_t shadingModeEntry;
                ret = GetCameraMetadataEntryByTag(
                    const_cast<camera_metadata_t*>(info->static_camera_characteristics),
                    ANDROID_SHADING_AVAILABLE_MODES,
                    &shadingModeEntry);
                NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SHADING_AVAILABLE_MODES failed");

                for (size_t k = 0; k < shadingModeEntry.count; k++)
                {
                    uint8_t shadingMode = shadingModeEntry.data.u8[k];
                    NT_LOG_DEBUG( "start testing Shading mode value: %d", static_cast<int>(shadingMode));
                    frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_SHADING_MODE, &shadingMode);
                    NT_ASSERT(frameNumber != -1, "Frame number not set");
                    const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
                    NT_ASSERT(metaFromFile != nullptr,"Meta read from file is null");

                    camera_metadata_entry_t shadingMapEntry;
                    ret = GetCameraMetadataEntryByTag(
                        const_cast<camera_metadata_t*>(metaFromFile),
                        ANDROID_STATISTICS_LENS_SHADING_MAP,
                        &shadingMapEntry);
                    NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_STATISTICS_LENS_SHADING_MAP failed");

                    camera_metadata_entry_t shadingMapSizeEntry;
                    ret = GetCameraMetadataEntryByTag(
                        const_cast<camera_metadata_t*>(info->static_camera_characteristics),
                        ANDROID_LENS_INFO_SHADING_MAP_SIZE,
                        &shadingMapSizeEntry);
                    NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_LENS_INFO_SHADING_MAP_SIZE failed");

                    int width = shadingMapSizeEntry.data.i32[0];
                    int height = shadingMapSizeEntry.data.i32[1];
                    int numElementsInMap = width*height;

                    for (int m = 0; m < numElementsInMap; m++)
                    {
                        if (shadingMode == ANDROID_SHADING_MODE_FAST ||
                            shadingMode == ANDROID_SHADING_MODE_HIGH_QUALITY)
                        {
                            NT_ASSERT(1.0f<= shadingMapEntry.data.f[m], "Verifying Lens Shading Map: Encountered bad value in map");
                        }
                        else if (shadingMode == ANDROID_SHADING_MODE_OFF)
                        {
                            NT_ASSERT(1.0f<= shadingMapEntry.data.f[m], "Verifying Lens Shading Map: Encountered bad value in map");
                        }
                        else
                        {
                            NT_LOG_ERROR("Verifying Lens Shading Map: Invalid shading mode");
                        }
                    }

                    if (bDumpOutputBuffer)
                    {
                        std::ostringstream outputName;
                        outputName << "TestLensShadingMap_ShadingMode" << static_cast<int>(shadingMode);
                        DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1,
                            outputName.str().c_str()).c_str());
                    }

                    if (metaFromFile != nullptr)
                    {
                        free_camera_metadata(const_cast<camera_metadata_t*>(metaFromFile));
                    }
                }
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestEffectModes()
    *
    * @brief
    *   Tests available effect modes on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestEffectModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer)
    {

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_EFFECTS,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_EFFECTS failed");


        for (size_t j = 0; j < entry.count; j++)
        {
            uint8_t effectMode = entry.data.u8[j];
            NT_LOG_DEBUG("start testing effect mode value: %d", static_cast<int>(effectMode));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_EFFECT_MODE, &effectMode);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestEffectModes_EffectMode" << static_cast<int>(effectMode);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestSceneModes()
    *
    * @brief
    *   Tests available scene modes on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestSceneModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer)
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_SCENE_MODES,
            &entry);
       NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_SCENE_MODES failed");

       NT_LOG_DEBUG("start testing control mode: %s", stringify(ANDROID_CONTROL_MODE_USE_SCENE_MODE));
        uint8_t controlMode = ANDROID_CONTROL_MODE_USE_SCENE_MODE;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_MODE, &controlMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG( "start testing scene mode value: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_SCENE_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestSceneModes_SceneMode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }


    /**************************************************************************************************************************
    * CaptureRequestTest::TestAntiBandingModes()
    *
    * @brief
    *   Tests available anti-banding modes on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAntiBandingModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer)
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES failed");


        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG( "start testing antibanding mode value: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_ANTIBANDING_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestAntiBandingModes_ABMode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }

    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestFlashControl()
    *
    * @brief
    *   Tests available flash control on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestFlashControl(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        camera_metadata_entry_t entry;

        // check if unit has flash support
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_FLASH_INFO_AVAILABLE,
            &entry);
        NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_FLASH_INFO_AVAILABLE failed");

        bool bHasFlashSupport = false;
        if (entry.data.u8[0] == ANDROID_FLASH_INFO_AVAILABLE_TRUE)
        {
            bHasFlashSupport = true;
            NT_LOG_INFO("Flash is supported");
        }
        else
        {
            NT_LOG_UNSUPP("Flash is NOT supported");
        }


        // Flash control can only be used when the AE mode is ON or OFF, test both scenarios
        for (int j = ANDROID_CONTROL_AE_MODE_OFF; j <= ANDROID_CONTROL_AE_MODE_ON; j++)
        {
            if (j == ANDROID_CONTROL_AE_MODE_OFF)
            {
                NT_LOG_DEBUG("start testing ae mode: %s", stringify(ANDROID_CONTROL_AE_MODE_OFF))
                uint8_t aeMode = ANDROID_CONTROL_AE_MODE_OFF;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_MODE, &aeMode);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId,
                    frameNumber,
                    ANDROID_SENSOR_EXPOSURE_TIME,
                    &DEFAULT_EXP_TIME_NS,
                    1,
                    EXPOSURE_TIME_ERROR_MARGIN_RATE);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId,
                    frameNumber,
                    ANDROID_SENSOR_SENSITIVITY,
                    &DEFAULT_SENSITIVITY,
                    1,
                    SENSITIVITY_ERROR_MARGIN_RATE);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
            }
            else if (j == ANDROID_CONTROL_AE_MODE_ON)
            {
                NT_LOG_DEBUG("start testing ae mode: " stringify(ANDROID_CONTROL_AE_MODE_ON))
                uint8_t aeMode = ANDROID_CONTROL_AE_MODE_ON;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_MODE, &aeMode);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
            }
            else
            {
                continue;
            }

            if (!bHasFlashSupport)
            {
                // check flash state is unavailable if flash is not supported
                uint32_t frameNo = (uint32_t)frameNumber;
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for frame number: %d",
                    frameNumber);
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNo, false, 0, nullptr),
                    "Start Streaming Failed");
                uint8_t flashState = ANDROID_FLASH_STATE_UNAVAILABLE;

                if (Camera3Stream::CheckKeySetInResult(ANDROID_FLASH_STATE, &flashState, GetHalRequest(cameraId)->frame_number))
                {
                    NT_LOG_INFO("Key in capture result is set to the requested value");
                }
                else
                {
                    NT_ASSERT(ANDROID_FLASH_STATE_UNAVAILABLE == flashState, "Flash state should be unavailable if flash is not supported");
                }
            }
            else
            {
                // test flash SINGLE mode control
                // wait for flash state to be READY first
                bool stateReached = false;
                uint8_t flashMode = ANDROID_FLASH_MODE_SINGLE;

                for (int k = 0; k < PROCESS_CAPTURE_TIMEOUT; k++)
                {
                    GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                    NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for frame number: %d",
                        frameNumber);
                    NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, ++frameNumber, false, 0, nullptr),
                        "Start Streaming Failed");
                    uint8_t flashState = ANDROID_FLASH_STATE_READY;

                    if (Camera3Stream::CheckKeySetInResult(ANDROID_FLASH_STATE, &flashState, GetHalRequest(cameraId)->frame_number))
                    {
                        stateReached = true;
                        break;
                    }
                }

                NT_ASSERT(stateReached, "Flash failed to reach READY state");

                NT_LOG_DEBUG("start testing flash mode: ANDROID_FLASH_MODE_SINGLE %d", ANDROID_FLASH_MODE_SINGLE);
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_FLASH_MODE, &flashMode);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                if (bDumpOutputBuffer)
                {
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, "TestFlashControl_FlashSingle").c_str());
                }

                stateReached = false;
                uint8_t flashState = ANDROID_FLASH_STATE_FIRED;

                if (Camera3Stream::CheckKeySetInResult(ANDROID_FLASH_STATE, &flashState, GetHalRequest(cameraId)->frame_number))
                {
                    stateReached = true;
                }
                NT_ASSERT(stateReached, "Flash failed to reach FIRED state");

                // test flash TORCH mode control
                NT_LOG_DEBUG("start testing flash mode: ANDROID_FLASH_MODE_TORCH %d", ANDROID_FLASH_MODE_TORCH);
                flashMode = ANDROID_FLASH_MODE_TORCH;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_FLASH_MODE, &flashMode);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                if (bDumpOutputBuffer)
                {
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, "TestFlashControl_FlashTorch").c_str());
                }

                stateReached = false;
                if (Camera3Stream::CheckKeySetInResult(ANDROID_FLASH_STATE, &flashState, GetHalRequest(cameraId)->frame_number))
                {
                    stateReached = true;
                }
                NT_ASSERT(stateReached, "Flash failed to reach FIRED state");

                // test flash OFF mode control
                NT_LOG_DEBUG("start testing flash mode: ANDROID_FLASH_MODE_OFF %d", ANDROID_FLASH_MODE_OFF);
                flashMode = ANDROID_FLASH_MODE_OFF;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_FLASH_MODE, &flashMode);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                if (bDumpOutputBuffer)
                {
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, "TestFlashControl_FlashOff").c_str());
                }

            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestToneMapControl()
    *
    * @brief
    *   Tests available tone map modes and control on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestToneMapControl(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        if (!IsCaptureRequestKeySupported(info, ANDROID_TONEMAP_MODE))
        {
            NT_LOG_UNSUPP("Device doesn't support tone map controls, skip");
            return;
        }

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES failed");

        //Camera device must always support FAST mode
        bool bIsFastSupported = false;
        bool bIsHighQualitySupported = false;
        for (size_t j = 0; j < entry.count; j++)
        {
            if (entry.data.u8[j] == ANDROID_TONEMAP_MODE_FAST)
            {
                bIsFastSupported = true;
            }
            if (entry.data.u8[j] == ANDROID_TONEMAP_MODE_HIGH_QUALITY)
            {
                bIsHighQualitySupported = true;
            }
            // make sure value is in range
            NT_ASSERT(entry.data.u8[j] >= ANDROID_TONEMAP_MODE_CONTRAST_CURVE && entry.data.u8[j] <= ANDROID_TONEMAP_MODE_PRESET_CURVE,
                "Out of range tone map value %d" ,static_cast<int>(entry.data.u8[j]));
        }
        NT_ASSERT(bIsFastSupported, "Missing FAST from available tone map modes, expects support on all devices");

        camera_metadata_entry_t actualHwLevelEntry;
        NT_ASSERT(0== GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL,
            &actualHwLevelEntry), "GetCameraMetadataEntryByTag ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL failed");
        int actualHwLevel = actualHwLevelEntry.data.u8[0];

        if (actualHwLevel != ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY)
        {
            // FAST and HIGH_QUALITY mode must be both present
            NT_ASSERT(bIsHighQualitySupported, "Missing HIGH QUALITY from available tone map modes, expected support on all devices");
        }

        for (size_t j = 0; j < entry.count; j++)
        {
            uint8_t toneMapMode = entry.data.u8[j];
            NT_LOG_DEBUG("start testing tone map mode: %d", static_cast<int>(toneMapMode));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_MODE, &toneMapMode);

            switch (toneMapMode)
            {
            case ANDROID_TONEMAP_MODE_CONTRAST_CURVE:
            {
                NT_LOG_DEBUG("start testing tone map mode: CONTRAST_CURVE with linear data");
                // Linear tone mapping curve example.
                const float TONEMAP_CURVE_LINEAR[] = { 0.0f, 0.0f, 1.0f, 1.0f };
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_CURVE_BLUE, &TONEMAP_CURVE_LINEAR[0], 4, true);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_CURVE_GREEN, &TONEMAP_CURVE_LINEAR[0], 4, true);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_CURVE_RED, &TONEMAP_CURVE_LINEAR[0], 4, true);
                NT_ASSERT(frameNumber != -1, "Frame number not set");

                NT_LOG_DEBUG("start testing tone map mode: CONTRAST_CURVE with sRGB data");
                // Standard sRGB tone mapping, per IEC 61966-2-1:1999, with 16 control points.
                const float TONEMAP_CURVE_SRGB[] = {
                    0.0000f, 0.0000f, 0.0667f, 0.2864f, 0.1333f, 0.4007f, 0.2000f, 0.4845f,
                    0.2667f, 0.5532f, 0.3333f, 0.6125f, 0.4000f, 0.6652f, 0.4667f, 0.7130f,
                    0.5333f, 0.7569f, 0.6000f, 0.7977f, 0.6667f, 0.8360f, 0.7333f, 0.8721f,
                    0.8000f, 0.9063f, 0.8667f, 0.9389f, 0.9333f, 0.9701f, 1.0000f, 1.0000f
                };
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_CURVE_BLUE, &TONEMAP_CURVE_SRGB[0], 32, true);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_CURVE_GREEN, &TONEMAP_CURVE_SRGB[0], 32, true);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_CURVE_RED, &TONEMAP_CURVE_SRGB[0], 32, true);
                NT_ASSERT(frameNumber != -1, "Frame number not set");

                break;
            }
            case ANDROID_TONEMAP_MODE_GAMMA_VALUE:
            {
                NT_LOG_DEBUG("start testing tone map mode: GAMMA VALUE with linear data");
                float gammaValue = 1.0f;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_GAMMA, &gammaValue, 1);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                gammaValue = 2.2f;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_GAMMA, &gammaValue, 1);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                gammaValue = 5.0f;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_GAMMA, &gammaValue, 1);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                break;
            }
            case ANDROID_TONEMAP_MODE_PRESET_CURVE:
            {
                NT_LOG_DEBUG("start testing tone map mode: PRESET CURVE with linear data");
                uint8_t presetCurve = ANDROID_TONEMAP_PRESET_CURVE_SRGB;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_PRESET_CURVE, &presetCurve);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                presetCurve = ANDROID_TONEMAP_PRESET_CURVE_REC709;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_TONEMAP_PRESET_CURVE, &presetCurve);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                break;
            }

            default:
                NT_LOG_INFO("No further verification required for tonemapmode: %d", static_cast<int>(toneMapMode));
                break;
            }

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestToneMapControl_ToneMapMode" << static_cast<int>(toneMapMode);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }


    /**************************************************************************************************************************
    * CaptureRequestTest::TestColorCorrectionControl()
    *
    * @brief
    *   Tests color correction mode and control on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestColorCorrectionControl(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        if (!IsCaptureRequestKeySupported(info, ANDROID_COLOR_CORRECTION_MODE))
        {
            NT_LOG_UNSUPP("Device doesn't support color correction controls, skip");
            return;
        }

        // AWB must be disabled
        uint8_t controlMode = ANDROID_CONTROL_MODE_AUTO;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_MODE, &controlMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        uint8_t awbMode = ANDROID_CONTROL_AWB_MODE_OFF;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AWB_MODE, &awbMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");

        // test TRANSFORM_MATRIX mode
        NT_LOG_DEBUG("start testing color correction mode: " stringify(ANDROID_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX));
        uint8_t colorCorrectionMode = ANDROID_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_COLOR_CORRECTION_MODE, &colorCorrectionMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        NT_LOG_DEBUG("start testing " stringify(ANDROID_COLOR_CORRECTION_GAINS) ": unit gain");
        float UNIT_GAIN[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_COLOR_CORRECTION_GAINS, &UNIT_GAIN[0], 4, false);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        camera_metadata_rational_t ZERO_R;
        ZERO_R.numerator = 0;
        ZERO_R.denominator = 1;
        camera_metadata_rational_t ONE_R;
        ONE_R.numerator = 1;
        ONE_R.denominator = 1;

        camera_metadata_rational_t colorTransform[] = {
            ONE_R, ZERO_R, ZERO_R,
            ZERO_R, ONE_R, ZERO_R,
            ZERO_R, ZERO_R, ONE_R };

        NT_LOG_DEBUG("start testing " stringify(ANDROID_COLOR_CORRECTION_TRANSFORM) ": identity matrix");
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_COLOR_CORRECTION_TRANSFORM, &colorTransform[0], 9, false);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "TestColorCorrectionControl_TransformMatrix";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }

        // test FAST mode
        NT_LOG_DEBUG("start testing color correction mode: " stringify(ANDROID_COLOR_CORRECTION_MODE_FAST));
        colorCorrectionMode = ANDROID_COLOR_CORRECTION_MODE_FAST;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_COLOR_CORRECTION_MODE, &colorCorrectionMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "TestColorCorrectionControl_Fast";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }

        // test HQ mode
        NT_LOG_DEBUG("start testing color correction mode: " stringify(ANDROID_COLOR_CORRECTION_MODE_HIGH_QUALITY));
        colorCorrectionMode = ANDROID_COLOR_CORRECTION_MODE_HIGH_QUALITY;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_COLOR_CORRECTION_MODE, &colorCorrectionMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "TestColorCorrectionControl_HQ";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestEdgeModes()
    *
    * @brief
    *   Tests available edge modes on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestEdgeModes(int cameraId, int format, int frameNumber, camera_info *info,
        bool bDumpOutputBuffer)
    {
        if (!IsCaptureRequestKeySupported(info, ANDROID_EDGE_MODE))
        {
            NT_LOG_UNSUPP("Device doesn't support edge mode controls, skip");
            return;
        }

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_EDGE_AVAILABLE_EDGE_MODES, &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_EDGE_AVAILABLE_EDGE_MODES failed");

        bool bIsFastSupported = false;
        bool bIsHighQualitySupported = false;
        bool bIsOffSupported = false;
        for (size_t j = 0; j < entry.count; j++)
        {
            if (entry.data.u8[j] == ANDROID_EDGE_MODE_FAST)
            {
                bIsFastSupported = true;
            }
            if (entry.data.u8[j] == ANDROID_EDGE_MODE_HIGH_QUALITY)
            {
                bIsHighQualitySupported = true;
            }
            if (entry.data.u8[j] == ANDROID_EDGE_MODE_OFF)
            {
                bIsOffSupported = true;
            }
        }

        camera_metadata_entry_t actualHwLevelEntry;
        NT_ASSERT(0== GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, &actualHwLevelEntry),
            "GetCameraMetadataEntryByTag ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL failed");

        int actualHwLevel = actualHwLevelEntry.data.u8[0];

        if (actualHwLevel != ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY &&
            actualHwLevel >= ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL)
        {
            // Full device must contain OFF and FAST edge modes
            NT_ASSERT(bIsOffSupported, "Missing OFF from available edge map modes, expected support on > "
                "ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL [%d] devices", ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL);
            NT_ASSERT(bIsFastSupported, "Missing FAST from available edge map modes, expected support on > "
                "ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL [%d] devices", ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL);
        }

        if (actualHwLevel != ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY &&
            actualHwLevel >= ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED)
        {
            // FAST and HIGH_QUALITY mode must be both present or both not present
            NT_ASSERT(bIsFastSupported == bIsHighQualitySupported,
               "Expects FAST and HIGH_QUALITY mode must be both present or both not present support on >"
                "ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED [%d] devices", ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED);
        }

        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG("start testing edge mode value: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_EDGE_MODE, &entry.data.u8[j]);
            NT_LOG_DEBUG("back from Apply and Verify");
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestEdgeModes_EdgeMode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }


    /**************************************************************************************************************************
    * CaptureRequestTest::hasFocuser()
    *
    * @brief
    *   Checks if device supports manual focus control
    * @return
    *   True if supports, false if not
    **************************************************************************************************************************/
    bool CaptureRequestTest::hasFocuser(camera_info *info)
    {
        bool hasFocuser = false;
        camera_metadata_entry_t actualHwLevelEntry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL,
            &actualHwLevelEntry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL failed")
            return false;
        }
        int actualHwLevel = actualHwLevelEntry.data.u8[0];

        if (actualHwLevel == ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY)
        {
            // Check available AF Modes
            camera_metadata_entry_t entry;
            ret = GetCameraMetadataEntryByTag(
                const_cast<camera_metadata_t*>(info->static_camera_characteristics),
                ANDROID_CONTROL_AF_AVAILABLE_MODES,
                &entry);
            if (ret != 0)
            {
                NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_CONTROL_AF_AVAILABLE_MODES failed")
                return false;
            }
            //if there is a mode which doesn't ignore AF trigger, then there is focuser
            for (size_t i = 0; i < entry.count; i++)
            {
                if (entry.data.u8[0] == ANDROID_CONTROL_AF_MODE_AUTO ||
                    entry.data.u8[0] == ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO ||
                    entry.data.u8[0] == ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE ||
                    entry.data.u8[0] == ANDROID_CONTROL_AF_MODE_MACRO)
                {
                    hasFocuser = true;
                    break;
                }
            }
        }
        else
        {
            // check lens.info.minimumFocusDistance
            camera_metadata_entry_t entry;
            ret = GetCameraMetadataEntryByTag(
                const_cast<camera_metadata_t*>(info->static_camera_characteristics),
                ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE,
                &entry);
            if (ret != 0)
            {
                NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE failed")
                return false;
            }

            hasFocuser = (entry.data.f[0] > 0);
            NT_LOG_DEBUG("Minimum focus distance is %f", entry.data.f[0]);
        }

        return hasFocuser;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestFocusDistanceControl()
    *
    * @brief
    *   Tests combinations of focal length setting
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestFocusDistanceControl(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer)
    {
        // check if device has focuser
        if (!hasFocuser(info))
        {
            NT_LOG_UNSUPP("Device doesn't have focuser, skipping");
            return;
        }

        // Check if manual sensor is supported
        if (!isCapabilitySupported(info, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR))
        {
            NT_LOG_UNSUPP("Device doesn't support manual sensor, skipping");
            return;
        }

        // AF must be disabled
        NT_LOG_DEBUG("start testing AF mode: " stringify(ANDROID_CONTROL_AF_MODE_OFF));
        uint8_t controlMode = ANDROID_CONTROL_MODE_AUTO;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_MODE, &controlMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        uint8_t afMode = ANDROID_CONTROL_AF_MODE_OFF;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AF_MODE, &afMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");

        //Check calibration to get proper error margin for result verification
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION failed");

        float errorMargin = FOCUS_DISTANCE_ERROR_PERCENT_UNCALIBRATED;
        if (entry.data.u8[0] == ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_APPROXIMATE)
        {
            errorMargin = FOCUS_DISTANCE_ERROR_PERCENT_APPROXIMATE;
        }
        else if (entry.data.u8[0] == ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_CALIBRATED)
        {
            errorMargin = FOCUS_DISTANCE_ERROR_PERCENT_CALIBRATED;
        }

        // Test changing focus distance
        const int NUM_TEST_FOCUS_DISTANCES = 10;
        const int testValuesLength = NUM_TEST_FOCUS_DISTANCES + 1;
        float testDistances[testValuesLength];
        float minValue = 0;

        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE failed");
        float maxValue = entry.data.f[0];
        float range = maxValue - minValue;
        float stepSize = range / NUM_TEST_FOCUS_DISTANCES;

        for (int j = 0; j <= NUM_TEST_FOCUS_DISTANCES; j++)
        {
            testDistances[j] = minValue + stepSize * j;
        }

        float resultDistances[testValuesLength];

        // apply focal length and wait for lens state to be stationary
        for (int j = 0; j < testValuesLength; j++)
        {
            NT_LOG_DEBUG("start testing " stringify(ANDROID_LENS_FOCUS_DISTANCE) ": %f", testDistances[j]);
            Camera3Stream::SetKey(cameraId, ANDROID_LENS_FOCUS_DISTANCE, &testDistances[j], 1);

            bool stateAchieved = false;

            for (int retries = 0; retries < PROCESS_CAPTURE_TIMEOUT; retries++)
            {
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Get HAL request failure");
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                    "Start Streaming Failed");
                // wait for lens to stabilize per each focus distance setting
                uint8_t lensState = ANDROID_LENS_STATE_STATIONARY;
                if (CheckKeySetInResult(ANDROID_LENS_STATE, &lensState, GetHalRequest(cameraId)->frame_number))
                {
                    stateAchieved = true;
                    const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
                    NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null");

                    ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
                        ANDROID_LENS_FOCUS_DISTANCE, &entry);
                    NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_LENS_FOCUS_DISTANCE failed");
                    resultDistances[j] = entry.data.f[0];
                    free_camera_metadata(const_cast<camera_metadata_t*>(metaFromFile));


                    break;
                }
            }

            NT_ASSERT(stateAchieved, "Lens state failed to achieve stationary state for focus distance %f", testDistances[j]);

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestFocusDistanceControl_Distance" << testDistances[j];
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }

        //Verify result distances are valid
        float hyperfocalDistance;
        ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_LENS_INFO_HYPERFOCAL_DISTANCE, &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_LENS_INFO_HYPERFOCAL_DISTANCE failed");
        hyperfocalDistance = entry.data.f[0];

        for (int k = 0; k < testValuesLength - 1; k++)
        {
            // HAL may choose to use hyperfocal distance for all distances between [0, hyperfocal].
            float marginMax = std::max(testDistances[k], hyperfocalDistance) * (1.0f + errorMargin);
            NT_ASSERT(resultDistances[k] <= marginMax,
                "Focus distance in result %f  shouldn't move past request focus distance %f",resultDistances[k],marginMax);

            float compareDistance = resultDistances[k + 1] - resultDistances[k];
            NT_ASSERT(compareDistance > 0, "Adjacent result monotonicity is broken %f , %f" ,resultDistances[k] ,resultDistances[k + 1]);
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestZoom()
    *
    * @brief
    *   Tests a set of zoom values over the supported range
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestZoom(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        // Determine zoom test values
        int ZOOM_STEPS = 15;
        PointF *testZoomValues;
        int testValueCount = -1;
        float errorMargin = 0.0f;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SCALER_CROPPING_TYPE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SCALER_CROPPING_TYPE failed");
        uint8_t croppingType = entry.data.u8[0];

        if (croppingType == ANDROID_SCALER_CROPPING_TYPE_FREEFORM)
        {
            NT_LOG_DEBUG("Testing zoom with: " stringify(ANDROID_SCALER_CROPPING_TYPE_FREEFORM));
            testZoomValues = static_cast<PointF *>(malloc(sizeof(PointF) * 5));
            NT_ASSERT(testZoomValues, "PointF malloc failed");
            testValueCount = 5;
            testZoomValues[0] = { 0.5f, 0.5f }; // Center point
            testZoomValues[1] = { 0.25f, 0.25f }; // top left corner zoom, minimal zoom: 2x
            testZoomValues[2] = { 0.75f, 0.25f }; // top right corner zoom, minimal zoom: 2x
            testZoomValues[3] = { 0.25f, 0.75f }; // bottom left corner zoom, minimal zoom: 2x
            testZoomValues[4] = { 0.75f, 0.75f }; // bottom right corner zoom, minimal zoom: 2x
            errorMargin = CROP_REGION_ERROR_PERCENT_DELTA;
        }
        else
        {
            NT_LOG_DEBUG("Testing zoom with: " stringify(ANDROID_SCALER_CROPPING_TYPE_CENTER_ONLY));
            testZoomValues = static_cast<PointF *>(malloc(sizeof(PointF)));
            NT_ASSERT(testZoomValues, "PointF malloc failed");
            testValueCount = 1;
            testZoomValues[0] = { 0.5f, 0.5f }; // Center point
            errorMargin = CROP_REGION_ERROR_PERCENT_CENTERED;
        }

        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM failed");
        float maxZoom = entry.data.f[0];
        NT_ASSERT(maxZoom >= 1.0, "Invalid maxZoom");

        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE failed");
        Rect activeArraySize = { entry.data.i32[0], entry.data.i32[1], entry.data.i32[2], entry.data.i32[3] };

        std::vector<Rect> cropRegions;
        for (int j = 0; j < testValueCount; j++)
        {
            for (int k = 0; k < ZOOM_STEPS; k++)
            {
                float zoomFactor = static_cast<float>(1.0f + (maxZoom - 1.0) * k / ZOOM_STEPS);
                NT_ASSERT(zoomFactor >= 1.0, "Invalid zoom factor");
                NT_ASSERT(testZoomValues[j].x <= 1.0 && testZoomValues[j].x >= 0, "Invalid center.x, should be in range of [0, 1.0]");
                NT_ASSERT(testZoomValues[j].y <= 1.0 && testZoomValues[j].y >= 0, "Invalid center.y, should be in range of [0, 1.0]");
                cropRegions.push_back(getCropRegionForZoom(zoomFactor, testZoomValues[j], maxZoom, activeArraySize));

                int32_t scalerCropRegion[4];
                scalerCropRegion[0] = cropRegions[k].left;
                scalerCropRegion[1] = cropRegions[k].top;
                scalerCropRegion[2] = cropRegions[k].right - cropRegions[k].left;
                scalerCropRegion[3] = cropRegions[k].bottom - cropRegions[k].top;
                NT_ASSERT(errorMargin != 0.0f, "Error Margin not set");
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_SCALER_CROP_REGION, &scalerCropRegion[0], 4, errorMargin);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                if (bDumpOutputBuffer)
                {
                    std::ostringstream outputName;
                    outputName << "TestZoom_Region" << scalerCropRegion[0] << "_" << scalerCropRegion[1]
                        << "_" << scalerCropRegion[2] << "_" << scalerCropRegion[3];
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
                }
            }
        }

        if (testZoomValues)
            free(testZoomValues);
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::getCropRegionForZoom()
    *
    * @brief
    *   Get a crop region for a given zoom factor and center position.
    * @return
    *   Rect struct of the crop region
    **************************************************************************************************************************/
    CaptureRequestTest::Rect CaptureRequestTest::getCropRegionForZoom(
        float zoomFactor,
        PointF center,
        float maxZoom,
        Rect activeArray) const
    {
        float minCenterLength = std::min(std::min(center.x, 1.0f - center.x), std::min(center.y, 1.0f - center.y));
        float minEffectiveZoom = 0.5f / minCenterLength;

        if (minEffectiveZoom > maxZoom)
        {
            NT_LOG_ERROR("Requested center has minimal zoomable factor which exceeds max zoom factor");
        }

        if (zoomFactor < minEffectiveZoom) {
            zoomFactor = minEffectiveZoom;
        }

        int cropCenterX = static_cast<int>((activeArray.right - activeArray.left) * center.x);
        int cropCenterY = static_cast<int>((activeArray.bottom - activeArray.top) * center.y);
        int cropWidth = static_cast<int>((activeArray.right - activeArray.left) / zoomFactor);
        int cropHeight = static_cast<int>((activeArray.bottom - activeArray.top) / zoomFactor);

        return{/*left*/cropCenterX - cropWidth / 2,
            /*top*/cropCenterY - cropHeight / 2,
            /*right*/ cropCenterX + cropWidth / 2 - 1,
            /*bottom*/cropCenterY + cropHeight / 2 - 1 };
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestNoiseReductionModeControl()
    *
    * @brief
    *   Tests available noise reduction modes on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestNoiseReductionModeControl(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        if (!IsCaptureRequestKeySupported(info, ANDROID_NOISE_REDUCTION_MODE))
        {
            NT_LOG_UNSUPP("Device doesn't support noise reduction mode controls, skip");
            return;
        }

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG("start testing noise reduction mode: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_NOISE_REDUCTION_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestNoiseReductionModeControl_NRMode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestCameraStabilization()
    *
    * @brief
    *   Tests available video and optical stabilization modes on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestCameraStabilization(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        // Test available video stabilization modes
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG("start testing video stabilization mode: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestCameraStabilization_VideoMode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }

        // Test available lens optical stabilization modes
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG("start testing lens optical stabilization mode: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_LENS_OPTICAL_STABILIZATION_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestCameraStabilization_OpticalMode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::Test3ARegions()
    *
    * @brief
    *   Tests available 3A region settings on device
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::Test3ARegions(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        // Get the region coordinate system.
        // The coordinate system is based on the active pixel array, with (0,0) being the
        // top-left pixel in the active pixel array, and (android.sensor.info.activeArraySize.width - 1,
        // android.sensor.info.activeArraySize.height - 1) being the bottom-right pixel in the active pixel array.

        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE failed");
        int32_t activeArraySize[5] = { 0,
                                       0,
                                       entry.data.i32[2],
                                       entry.data.i32[3],
                                       1 };

        // get max regions
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_MAX_REGIONS,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_MAX_REGIONS failed");
        int AEregion = entry.data.i32[0];
        int AWBregion = entry.data.i32[1];
        int AFregion = entry.data.i32[2];

        if (AEregion > 0)
        {
            NT_LOG_DEBUG("start testing ANDROID_CONTROL_AE_REGIONS");
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_REGIONS, &activeArraySize[0], 5, 0);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
        }

        if (AWBregion > 0)
        {
            NT_LOG_DEBUG("start testing ANDROID_CONTROL_AWB_REGIONS");
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AWB_REGIONS, &activeArraySize[0], 5, 0);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
        }

        if (AFregion > 0)
        {
            NT_LOG_DEBUG("start testing ANDROID_CONTROL_AF_REGIONS");
            ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AF_REGIONS, &activeArraySize[0], 5, 0);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
        }

        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "Test3ARegions";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestFaceDetection()
    *
    * @brief
    *   Tests available face detection modes. This test is no longer a port from CTS, as the Hal layer does not an equivalent
    *   API as the Java layer (Face objects are not available from metadata in Hal). In general, metadata for face detection
    *   is organized differently between Hal and CTS. The tests performed here are similar to CTS, but not exactly the same.
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestFaceDetection(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        camera_metadata_entry_t faceDetectModeEntry;
         // Setup and resolve vendor tag
        bool vendorTags = false;
        camera_metadata_tag_t isFDSupp;
        char tagName[] = "isFDRenderingInCameraUISupported";
        char sectionName[] = "org.quic.camera.FDRendering";
        int ret = -1; //VendorTag return values

        // Initialize vendor tags if needed
        if (!vendorTags)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0,"Vendor tags could not be initialized!");
            vendorTags = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("FDRenderingInCameraUISupported vendor tag not available!");
            return;
        }
        isFDSupp = static_cast<camera_metadata_tag_t>(tagNum);

        //FDCameraSuported Check
        camera_info* pCamInfoLv = GetCameraInfoWithId(cameraId);
        NT_ASSERT(nullptr != pCamInfoLv, "Unable to retrieve camera info for camera Id: %d", cameraId);

        camera_metadata_entry_t tagEntry;
        ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoLv->static_camera_characteristics),
            isFDSupp, &tagEntry);
        NT_ASSERT(ret == 0,"find_camera_metadata_entry FD failed");
        if (static_cast<bool>(tagEntry.data.i32[0]) == false)
        {
            NT_LOG_UNSUPP("FaceDetection in Camera is not Supported");
            return;
        }

        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES,
            &faceDetectModeEntry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES failed");

        for (size_t i = 0; i < faceDetectModeEntry.count; i++)
        {
            uint8_t faceDetectMode = faceDetectModeEntry.data.u8[i];
            NT_LOG_DEBUG("Start testing face detection mode: %d", static_cast<int>(faceDetectMode));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_STATISTICS_FACE_DETECT_MODE, &faceDetectMode);
            NT_ASSERT(frameNumber != -1, "Frame number not set");

            camera_metadata_entry_t faceDetectRectanglesEntry;
            camera_metadata_entry_t faceDetectScoresEntry;
            camera_metadata_entry_t faceDetectIdsEntry;
            const camera_metadata_t* resultMetadata = GetCameraMetadataByFrame(frameNumber);

            // Get entry for face Ids
            int ret_id = GetCameraMetadataEntryByTag(
                const_cast<camera_metadata_t*>(resultMetadata),
                ANDROID_STATISTICS_FACE_IDS,
                &faceDetectIdsEntry);

            // Face detect mode not OFF
            if (faceDetectMode != ANDROID_STATISTICS_FACE_DETECT_MODE_OFF)
            {
                // COMMON MODE VALIDATIONS
                // Get entry for face rectangles
                ret = GetCameraMetadataEntryByTag(
                    const_cast<camera_metadata_t*>(resultMetadata),
                    ANDROID_STATISTICS_FACE_RECTANGLES,
                    &faceDetectRectanglesEntry);
                // If there are any faces (Face Ids are present), then face rectangles must be present
                NT_ASSERT(ret_id !=0 || ret == 0, "STATISTICS_FACE_RECTANGLES should not be null for mode %d", static_cast<int>(faceDetectMode));

                // Get entry for face scores
                ret = GetCameraMetadataEntryByTag(
                    const_cast<camera_metadata_t*>(resultMetadata),
                    ANDROID_STATISTICS_FACE_SCORES,
                    &faceDetectScoresEntry);
                // If there are any faces (Face Ids are present), then face scores must be present
                NT_ASSERT(ret_id != 0 || ret == 0, "STATISTICS_FACE_SCORES should not be null for mode %d", static_cast<int>(faceDetectMode));

                if (ret == 0)
                {
                    // Validate that face scores are within valid range (if present)
                    for (size_t k = 0; k < faceDetectScoresEntry.count; k++)
                    {
                        NT_EXPECT(FACE_SCORE_MIN <= faceDetectScoresEntry.data.u8[k] && faceDetectScoresEntry.data.u8[k] <= FACE_SCORE_MAX
                            ,"Face score ( %d ) is not within valid range [%d, %d ] ", static_cast<int>(faceDetectScoresEntry.data.u8[k])
                            , FACE_SCORE_MIN , FACE_SCORE_MAX);
                    }
                }

                // NON-COMMON MODE VALIDATIONS
                switch (faceDetectMode)
                {
                    // SIMPLE mode
                    case ANDROID_STATISTICS_FACE_DETECT_MODE_SIMPLE:
                    {
                        if (ret_id == 0)
                        {
                            // For SIMPLE mode, validate face Ids are UNSUPPORTED (if present)
                            for (size_t j = 0; j < faceDetectIdsEntry.count; j++)
                            {
                                NT_EXPECT(faceDetectIdsEntry.data.i32[j] == FACE_ID_UNSUPPORTED,"Face Id ( %d ) expected to be  %d for SIMPLE mode" ,
                                      faceDetectIdsEntry.data.i32[j], FACE_ID_UNSUPPORTED);
                            }
                        }
                        break;
                    }
                    // FULL mode
                    case ANDROID_STATISTICS_FACE_DETECT_MODE_FULL:
                    {
                        if (ret_id == 0)
                        {
                            // For FULL mode, validate face Ids are NOT UNSUPPORTED (if present)
                            std::vector<int32_t> faceIds;
                            for (size_t j = 0; j < faceDetectIdsEntry.count; j++)
                            {
                                faceIds.push_back(faceDetectIdsEntry.data.i32[j]);
                                NT_EXPECT(faceDetectIdsEntry.data.i32[j] == FACE_ID_UNSUPPORTED, "Face Id ( %d ) expected to be  %d for FULL mode",
                                    faceDetectIdsEntry.data.i32[j], FACE_ID_UNSUPPORTED);
                            }

                            // Check for face Id uniqueness
                            std::set<int32_t> faceIdsSet(faceIds.begin(), faceIds.end());
                            NT_EXPECT(faceIds.size() == faceIdsSet.size(), "Face Ids should be unique in FULL mode");
                        }
                        break;
                    }
                    default:
                    {
                        NT_LOG_ERROR("Invalid Face Detection Mode: %d", faceDetectMode);
                        break;
                    }
                }
            }

            if (resultMetadata != nullptr)
            {
                free_camera_metadata(const_cast<camera_metadata_t*>(resultMetadata));
            }

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestFaceDetection_FDMode" << static_cast<int>(faceDetectMode);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestDynamicBlackWhiteLevel()
    *
    * @brief
    *   Tests that the dynamic black and white levels shouldn't deviate
    *   from the global value too much for different sensitivities
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestDynamicBlackWhiteLevel(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {

        if (!IsCaptureResultKeySupported(info, ANDROID_SENSOR_DYNAMIC_BLACK_LEVEL))
        {
            NT_LOG_UNSUPP("Device doesn't support dynamic black level, skip");
            return;
        }

        // obtain list of sensitivity values to test
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_SENSITIVITY_RANGE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_SENSITIVITY_RANGE failed");

        int minSensitivity = entry.data.i32[0];
        int maxSensitivity = entry.data.i32[1];
        NT_ASSERT(maxSensitivity > minSensitivity, "ANDROID_SENSOR_INFO_SENSITIVITY_RANGE is invalid [ %d , %d ]", minSensitivity, maxSensitivity);

        minSensitivity = (minSensitivity > SENSOR_INFO_SENSITIVITY_RANGE_MIN_AT_MOST)
            ? SENSOR_INFO_SENSITIVITY_RANGE_MIN_AT_MOST : minSensitivity;
        maxSensitivity = (maxSensitivity < SENSOR_INFO_SENSITIVITY_RANGE_MAX_AT_LEAST)
            ? SENSOR_INFO_SENSITIVITY_RANGE_MAX_AT_LEAST : maxSensitivity;

        int sensitivityRange = maxSensitivity - minSensitivity;
        int sensitivityStepSize = DEFAULT_SENSITIVITY_STEP_SIZE;
        int sensitivityNumSteps = sensitivityRange / sensitivityStepSize;

        // Bound the test steps to avoid supper long test.
        if (sensitivityNumSteps > DEFAULT_NUM_SENSITIVITY_STEPS)
        {
            sensitivityNumSteps = DEFAULT_NUM_SENSITIVITY_STEPS;
            sensitivityStepSize = sensitivityRange / sensitivityNumSteps;
        }

        int * senstivityTestValues = static_cast<int*>(malloc(sizeof(int) * (sensitivityNumSteps + 1)));
        NT_ASSERT(senstivityTestValues != nullptr, "Malloc failed");

        for (int j = 0; j <= sensitivityNumSteps; j++)
        {
            senstivityTestValues[j] = maxSensitivity - sensitivityStepSize * j;
            senstivityTestValues[j] = std::max(minSensitivity, std::min(maxSensitivity, senstivityTestValues[j]));
        }

        std::vector<float *> dynamicBlackLevels(sensitivityNumSteps + 1, nullptr);
        std::vector<int> dynamicWhiteLevels(sensitivityNumSteps + 1, 0);

        uint8_t aeMode = ANDROID_CONTROL_AE_MODE_OFF;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_MODE, &aeMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");
        for (int j = 0; j < sensitivityNumSteps + 1; j++)
        {
            dynamicBlackLevels[j] = nullptr;
            dynamicWhiteLevels[j] = 0;
            frameNumber = ApplyAndVerifyKeySetting(cameraId,
                frameNumber,
                ANDROID_SENSOR_EXPOSURE_TIME,
                &DEFAULT_EXP_TIME_NS,
                1,
                EXPOSURE_TIME_ERROR_MARGIN_RATE);
            NT_ASSERT(frameNumber != -1, "Frame number not set");

            frameNumber = ApplyAndVerifyKeySetting(cameraId,
                frameNumber,
                ANDROID_SENSOR_SENSITIVITY,
                &senstivityTestValues[j],
                1,
                SENSITIVITY_ERROR_MARGIN_RATE);
            NT_ASSERT(frameNumber != -1, "Frame number not set");

            const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);

            ret = GetCameraMetadataEntryByTag(
                const_cast<camera_metadata_t*>(metaFromFile),
                ANDROID_SENSOR_DYNAMIC_BLACK_LEVEL,
                &entry);
            NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SENSOR_DYNAMIC_BLACK_LEVEL failed");

            dynamicBlackLevels[j] = static_cast<float *>(malloc(sizeof(float) * entry.count));
            for (size_t k = 0; k < entry.count; k++)
            {
                dynamicBlackLevels[j][k] = entry.data.f[k];
            }

            ret = GetCameraMetadataEntryByTag(
                const_cast<camera_metadata_t*>(metaFromFile),
                ANDROID_SENSOR_DYNAMIC_WHITE_LEVEL,
                &entry);
            NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_SENSOR_DYNAMIC_WHITE_LEVEL failed");

            dynamicWhiteLevels[j] = entry.data.i32[0];

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestDynamicBlackWhiteLevel_sensitivityStep" << j;
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }

            if (metaFromFile != nullptr)
            {
                free_camera_metadata(const_cast<camera_metadata_t*>(metaFromFile));
            }
        }

        for (int j = 0; j < sensitivityNumSteps + 1; j++)
        {
            if (dynamicBlackLevels[j] != nullptr)
                free(dynamicBlackLevels[j]);
        }
            free(senstivityTestValues);
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestAePrecaptureTrigger()
    *
    * @brief
    *   Tests possible values for precapture trigger
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAePrecaptureTrigger(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump image
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        camera_metadata_entry_t entry;
        int numStates = 3;
        uint8_t states[] = {
            ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE,
            ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START,
            ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_CANCEL
        };

        // Check for at least HARDWARE_LEVEL_LIMITED, to indicate support for precapture trigger
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL failed");

        if (entry.count != 1 || entry.data.i32[0] == ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY)
        {
            NT_LOG_UNSUPP("ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER not supported by this device (HARDWARE_LEVEL_LEGACY)");
            return;
        }
        NT_LOG_INFO("Precapture trigger supported: device hardware level = %d", entry.data.i32[0]);

        // Apply and verify each state
        for (int i = 0; i < numStates; i++)
        {
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &states[i]);
            NT_ASSERT(frameNumber != -1, "AE_PRECAPTURE_TRIGGER test failed for state %d ", static_cast<int>(states[i]));

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestAePrecaptureTriggerModes_State" << static_cast<int>(states[i]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestAfTrigger()
    *
    * @brief
    *   Tests possible values for autofocus trigger
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAfTrigger(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump image
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        NATIVETEST_UNUSED_PARAM(info);
        int numStates = 3;
        uint8_t states[] = {
            ANDROID_CONTROL_AF_TRIGGER_IDLE,
            ANDROID_CONTROL_AF_TRIGGER_START,
            ANDROID_CONTROL_AF_TRIGGER_CANCEL
        };

        // Apply and verify each state
        for (int i = 0; i < numStates; i++)
        {
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AF_TRIGGER, &states[i]);
            NT_ASSERT(frameNumber != -1, "ANDROID_CONTROL_AF_TRIGGER test failed for state %d ", static_cast<int>(states[i]));

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestAfTriggerModes_State" << static_cast<int>(states[i]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestEnableZSL()
    *
    * @brief
    *   Checks that ENABLE_ZSL is either null or false in each camera3 template, except for TEMPLATE_STILL_CAPTURE which should
    *   have ENABLE_ZSL as true (API 26 and above only). This function assumes we are running on Android O and newer, as this
    *   key was not public prior to API 26.
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestEnableZSL(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump image
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        NATIVETEST_UNUSED_PARAM(info);
        camera_metadata_entry_t entry;
        bool allTestsPassed = true;

        // Existing HAL request created by SetupPreview is not needed
        DeleteHalRequest(cameraId);

        for (int templ = CAMERA3_TEMPLATE_PREVIEW; templ < CAMERA3_TEMPLATE_COUNT; templ++)
        {
            frameNumber++;
            // Create HAL request with each template type
            CreateCaptureRequestWithMultBuffers(cameraId, 0, templ, 1, frameNumber);

            // Check HAL request settings for correct ZSL setting
            int ret = GetCameraMetadataEntryByTag(
                const_cast<camera_metadata_t*>(GetHalRequest(cameraId)->settings),
                ANDROID_CONTROL_ENABLE_ZSL,
                &entry);
            if (ret != 0)
            {
                NT_LOG_WARN("ENABLE_ZSL not found in template %d settings, skipping...", templ);
                DeleteHalRequest(cameraId);
                continue;
            }
            // If present in settings, ENABLE_ZSL should be false for all templates, except TEMPLATE_STILL_CAPTURE which MAY be true
            if (entry.count == 1 &&
               (entry.data.u8[0] == ANDROID_CONTROL_ENABLE_ZSL_FALSE ||
               (entry.data.u8[0] == ANDROID_CONTROL_ENABLE_ZSL_TRUE && templ == CAMERA3_TEMPLATE_STILL_CAPTURE)))
            {
                NT_LOG_INFO("Template %d settings correctly set ENABLE_ZSL to %d", templ, entry.data.u8[0]);
            }
            else
            {
                NT_LOG_ERROR("Template %d settings incorrectly set ENABLE_ZSL to %d", templ, entry.data.u8[0]);
                allTestsPassed = false;
            }

            // Take a snapshot
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                "Start Streaming Failed");

            // Check that metadata of snapshot also has the correct ZSL value
            int actualVal;
            uint8_t expectVal = ANDROID_CONTROL_ENABLE_ZSL_FALSE;
            if (templ == CAMERA3_TEMPLATE_STILL_CAPTURE || // STILL_CAPTURE template may be either true or false (no requirement)
                Camera3Stream::CheckKeySetInResult(ANDROID_CONTROL_ENABLE_ZSL, &expectVal, GetHalRequest(cameraId)->frame_number))
            {
                actualVal = expectVal;
                NT_LOG_INFO("Template %d capture result correctly sets ENABLE_ZSL to %d", templ, actualVal);
            }
            else
            {
                actualVal = (expectVal == ANDROID_CONTROL_ENABLE_ZSL_TRUE) ? ANDROID_CONTROL_ENABLE_ZSL_FALSE : ANDROID_CONTROL_ENABLE_ZSL_TRUE;
                NT_LOG_ERROR("Template %d capture result incorrectly sets ENABLE_ZSL to %d", templ, actualVal);
                allTestsPassed = false;
            }

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestEnableZSL_Template" << templ << "_ZSL" << actualVal;
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }

            DeleteHalRequest(cameraId);
        }
        NT_ASSERT(allTestsPassed, "One or more templates failed to set the correct ENABLE_ZSL value!");
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestCaptureIntent()
    *
    * @brief
    *   Sets and checks CONTROL_CAPTURE_INTENT modes in resulting snapshots (with 3A routines)
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestCaptureIntent(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump image
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        camera_metadata_entry_t entry;
        uint8_t setControlMode;
        bool manualSupport = false;
        bool zslSupport    = false;

        // Capture intent only effective if ANDROID_CONTROL_MODE is not OFF (set it to AUTO if necessary)
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(GetHalRequest(cameraId)->settings),
            ANDROID_CONTROL_MODE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_MODE failed");

        if (entry.count == 1 && entry.data.u8[0] != ANDROID_CONTROL_MODE_OFF)
        {
            NT_LOG_INFO("CONTROL_MODE is %d, CONTROL_CAPTURE_INTENT is effective", entry.data.u8[0]);
        }
        else
        {
            NT_LOG_WARN("CONTROL_MODE is OFF (%d), setting to AUTO so CONTROL_CAPTURE_INTENT is effective", entry.data.u8[0]);
            setControlMode = ANDROID_CONTROL_MODE_AUTO;
            Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_MODE, &setControlMode, 1);
        }

        // Determine support for ZERO_SHUTTER_LAG and MANUAL modes
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_REQUEST_AVAILABLE_CAPABILITIES failed");

        for (size_t i = 0; i < entry.count; i++)
        {
            if (!zslSupport &&
                (entry.data.u8[i] == ANDROID_REQUEST_AVAILABLE_CAPABILITIES_PRIVATE_REPROCESSING ||
                entry.data.u8[i] == ANDROID_REQUEST_AVAILABLE_CAPABILITIES_YUV_REPROCESSING))
            {
                zslSupport = true;
                NT_LOG_INFO("Found support for ZERO_SHUTTER_LAG mode");
            }
            else if (entry.data.u8[i] == ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR)
            {
                manualSupport = true;
                NT_LOG_INFO("Found support for MANUAL mode");
            }
        }

        // Sweep all possible modes, performing a 3A routine and checking the metadata of the final capture
        for (uint8_t setMode = ANDROID_CONTROL_CAPTURE_INTENT_CUSTOM; setMode <= ANDROID_CONTROL_CAPTURE_INTENT_MANUAL; setMode++)
        {
            // Skip modes that are not supported
            if ((setMode == ANDROID_CONTROL_CAPTURE_INTENT_MANUAL && !manualSupport) ||
                (setMode == ANDROID_CONTROL_CAPTURE_INTENT_ZERO_SHUTTER_LAG && !zslSupport))
            {
                NT_LOG_UNSUPP("CONTROL_CAPTURE_INTENT mode %d not supported on this device. Skipping test...", setMode);
                continue;
            }

            NT_LOG_INFO("Starting test for CONTROL_CAPTURE_INTENT mode %d", setMode);

            // Perform single capture validation for manual mode (skip 3A as it will not converge)
            if (setMode == ANDROID_CONTROL_CAPTURE_INTENT_MANUAL)
            {
                // Set ANDROID_CONTROL_MODE to OFF
                setControlMode = ANDROID_CONTROL_MODE_OFF;
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_MODE, &setControlMode, 1);

                // Set MANUAL mode key
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_CAPTURE_INTENT, &setMode, 1);
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for frame number: %d",
                    frameNumber);
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, ++frameNumber, false, 0, nullptr),
                    "Start Streaming Failed");

                if (Camera3Stream::CheckKeySetInResult(ANDROID_CONTROL_CAPTURE_INTENT, &setMode, frameNumber))
                {
                    NT_LOG_INFO("Key in capture result is set to the requested value");
                }
                else
                {
                    NT_EXPECT(1 == 0, "CONTROL_CAPTURE_INTENT test for mode %d  failed!", static_cast<int>(setMode));
                }

                // Set ANDROID_CONTROL_MODE back to AUTO
                setControlMode = ANDROID_CONTROL_MODE_AUTO;
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_MODE, &setControlMode, 1);
            }
            // All other modes, perform 3A check
            else
            {
#ifdef ENABLE3A
                frameNumber = ApplyAndVerifyKeySettingWith3AConverged(cameraId, frameNumber, ANDROID_CONTROL_CAPTURE_INTENT, &setMode);
                NT_ASSERT(frameNumber != -1, "CONTROL_CAPTURE_INTENT test for mode %d failed !", static_cast<int>(setMode));
#else
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_CAPTURE_INTENT, &setMode);
                NT_ASSERT(frameNumber != -1, "CONTROL_CAPTURE_INTENT test for mode %d failed !", static_cast<int>(setMode));
#endif
            }

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestCaptureIntentModes_Mode" << static_cast<int>(setMode);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestControlMode()
    *
    * @brief
    *   Sets and checks CONTROL_MODE modes in resulting snapshots
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestControlMode(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump images
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG("Start testing control mode value: %d", static_cast<int>(entry.data.u8[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestControlModes_Mode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestPostRawSensitivityBoost()
    *
    * @brief
    *   Sets and checks CONTROL_POST_RAW_SENSITIVITY_BOOST in resulting snapshots
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestPostRawSensitivityBoost(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump images
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        camera_metadata_entry_t entry;

        // Check that RAW format output is supported (a prerequesite for raw sensitivity boost)
        bool rawSupport = false;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_REQUEST_AVAILABLE_CAPABILITIES failed");
        for (size_t i = 0; i < entry.count; i++)
        {
            if (entry.data.u8[i] == ANDROID_REQUEST_AVAILABLE_CAPABILITIES_RAW)
            {
                rawSupport = true;
                NT_LOG_INFO("RAW format output is supported on this device.");
                break;
            }
        }

        // Query the boost value to ensure device supports this key (if null, then not supported)
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(GetHalRequest(cameraId)->settings),
            ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST,
            &entry);
        if (ret != 0 || entry.count != 1)
        {
            rawSupport = false;
            NT_LOG_WARN("Failed to find current post raw sensitivity boost.");
        }
        else
            NT_LOG_INFO("Current raw sensitivity boost is %d", entry.data.i32[0]);

        // If no raw support, then return without failure (pass the test)
        if (!rawSupport)
        {
            NT_LOG_UNSUPP("RAW format output not supported on this device. Cannot test Post Raw Sensitivity Boost.");
            return;
        }

        // Check that AE Mode or Control Mode is OFF (otherwise this will override the boost value)
        bool aeOff = false;
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AE_MODE,
            &entry);
        if (ret == 0 && entry.count == 1 && entry.data.u8[0] == ANDROID_CONTROL_AE_MODE_OFF)
            aeOff = true;

        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_MODE,
            &entry);
        if (ret == 0 && entry.count == 1 && entry.data.u8[0] == ANDROID_CONTROL_MODE_OFF)
            aeOff = true;

        if (!aeOff)
        {
            NT_LOG_WARN("AE should be OFF in order to test Post Raw Sensitivity Boost! Turning it off...");
            uint8_t setVal = ANDROID_CONTROL_AE_MODE_OFF;
            Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_MODE, &setVal, 1);
        }

        // Find raw sensitivity boost range
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST_RANGE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST_RANGE failed");
        NT_ASSERT(entry.count == 2, "POST_RAW_SENSITIVITY_BOOST_RANGE data not as expected!");
        NT_LOG_INFO("Found post raw sensitivity boost range [%d, %d]", entry.data.i32[0], entry.data.i32[1]);

        int32_t boostMin = entry.data.i32[0];
        int32_t boostMax = entry.data.i32[1];

        // Sweep over boost range
        for (int32_t boost = boostMin; boost <= boostMax; boost += POST_RAW_SENSITIVITY_BOOST_STEP)
        {
            NT_LOG_DEBUG("Start testing raw sensitivity boost value: %d", boost);
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST, &boost);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestPostRawSensitivityBoost_Level" << boost;
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestAeCompensation()
    *
    * @brief
    *   Tests available exposure compensation levels
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestAeCompensation(
        int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        // Existing HAL request created by SetupPreview is not needed
        DeleteHalRequest(cameraId);

        // Create HAL request with PREVIEW template
        CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_PREVIEW, 1, frameNumber);

        camera_metadata_entry_t entry;
        uint8_t val; // Used in SetKey operations

        // Get exp comp range data
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AE_COMPENSATION_RANGE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AE_COMPENSATION_RANGE failed");
        NT_ASSERT(entry.count == 2, "Exposure compensation range not as expected!");
        NT_ASSERT(entry.data.i32[0] != 0 && entry.data.i32[1] != 0, "Exposure compensation not supported, skipping test...");
        NT_LOG_INFO("Found exposure compensation range [%" PRId32 ", %" PRId32 "]", entry.data.i32[0], entry.data.i32[1]);

        // Copy exp comp range data
        int32_t ecMin = entry.data.i32[0];
        int32_t ecMax = entry.data.i32[1];

        // Get exposure compensation step data
        ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AE_COMPENSATION_STEP,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AE_COMPENSATION_STEP failed");
        NT_ASSERT(entry.count == 1, "Exposure compensation step not as expected!");
        double stepF = static_cast<double>(entry.data.r->numerator) / entry.data.r->denominator;
        int stepsPerEv = static_cast<int>(round(1.0 / stepF));
        int numSteps = (ecMax - ecMin) / stepsPerEv;

        NT_LOG_INFO("Found exposure compensation step size %" PRId32 "/%" PRId32 ", stepF = %f", entry.data.r->numerator, entry.data.r->denominator, stepF);

        // Check for AE Lock tag
        ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AE_LOCK_AVAILABLE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AE_LOCK_AVAILABLE failed");
        bool canSetAeLock = (entry.data.u8[0] == 1) ? true : false;

        // Check for read sensor settings capability
        bool canReadSensorSettings = isCapabilitySupported(info, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS);
        bool canReadExposureValueRange = IsCharacteristicKeySupported(info, ANDROID_SENSOR_INFO_SENSITIVITY_RANGE) &&
            IsCharacteristicKeySupported(info, ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE);
        bool canVerifyExposureValue = canReadSensorSettings && canReadExposureValueRange;
        int64_t minExposureValue = -1;
        int64_t maxExposureValueStill = -1;

        if (canReadExposureValueRange)
        {
            minExposureValue = GetSensitivityMinimumOrDefault(info) * GetExposureMinimumOrDefault(info) / 1000;
            int32_t maxSensitivity = GetSensitivityMaximumOrDefault(info);
            int64_t maxExposureTimeUs = GetExposureMaximumOrDefault(info) / 1000;

            // Get the max exposure value for still capture template (original hal request is initialized in SetupPreview)
            maxExposureValueStill = GetMaxExposureValue(GetHalRequest(cameraId), maxExposureTimeUs, maxSensitivity);
        }

        // Step through exposure compensation range, testing each value
        for (int32_t j = 0; j <= numSteps; j++)
        {
            int32_t exposureCompensation = j * stepsPerEv + ecMin;
            NT_LOG_INFO("Start testing exposure compensation level: %" PRId32, exposureCompensation);
            double expectedRatio = pow(2.0, exposureCompensation / stepsPerEv);

            // Take a normal capture (no exposure compensation set)
            int32_t defaultCompensation = 0;
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION, &defaultCompensation);
            NT_ASSERT(frameNumber != -1, "Failure in ApplyAndVerifyKeySetting!");
            WaitForAeStable(info, cameraId, frameNumber, NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY); //"Fatal failure occurred in WaitForAeStable!"
            const camera_metadata_t* normalResultMetadata = GetCameraMetadataByFrame(frameNumber);

            int64_t normalExposureValue = -1;
            if (canVerifyExposureValue)
            {
                // Check if normal exposure value is valid
                normalExposureValue = GetExposureValue(normalResultMetadata);
                if (normalExposureValue < minExposureValue || maxExposureValueStill < normalExposureValue)
                {
                    NT_LOG_WARN("Normal exposure value %" PRId64 " not in valid range [%" PRId64 ", %" PRId64 "]",
                        normalExposureValue, minExposureValue, maxExposureValueStill);
                }

                // Check if expected exposure value is within range, otherwise skip this exp comp setting
                int64_t expectedExposureValue = static_cast<int64_t>(normalExposureValue * expectedRatio);
                if (expectedExposureValue < minExposureValue ||  maxExposureValueStill < expectedExposureValue)
                {
                    NT_LOG_WARN(
                        "Expected exposure value %" PRId64 " not in valid range [%" PRId64 ", %" PRId64 "] for exposure compensation level %" PRId32 "!",
                        expectedExposureValue, minExposureValue, maxExposureValueStill, exposureCompensation);
                    continue;
                }
            }

            if (canSetAeLock)
            {
                val = ANDROID_CONTROL_AE_LOCK_ON;
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_LOCK, &val, 1);
            }

            // Apply exposure compensation setting and verify it in capture metadata
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION, &exposureCompensation);
            NT_ASSERT(frameNumber != -1, "Failure in ApplyAndVerifyKeySetting!");

            // Wait for compensation to converge (via AE Lock or the compensation value)
            if (canSetAeLock)
            {
                WaitForAeLocked(info, cameraId, frameNumber, NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY);
            }
            else
            {
                WaitForSettingsApplied(cameraId, frameNumber, NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY);
            }

            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestExposureCompensation_ECLevel" << exposureCompensation;
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }

            // Get metadata from exposure compensated result
            const camera_metadata_t* compensatedResultMetadata = GetCameraMetadataByFrame(frameNumber);

            if (canVerifyExposureValue)
            {
                // Verify the exposure value compensates as requested
                int64_t compensatedExposureValue = GetExposureValue(compensatedResultMetadata);
                NT_LOG_INFO("normalExposureValue = %" PRId64 ", compensatedExposureValue = %" PRId64 , normalExposureValue, compensatedExposureValue);
                if (compensatedExposureValue < minExposureValue || maxExposureValueStill < compensatedExposureValue)
                {
                    NT_LOG_WARN(
                        "Compensated exposure value %" PRId64 " not in valid range [%" PRId64 ", %" PRId64 "] for exposure compensation level %" PRId32 "!",
                        compensatedExposureValue, minExposureValue, maxExposureValueStill, exposureCompensation);
                }
                double observedRatio = static_cast<double>(compensatedExposureValue) / normalExposureValue;
                double error = observedRatio / expectedRatio;

                std::ostringstream infoString;
                infoString << "Exposure value ratios: expected(" << expectedRatio \
                    << ") observed(" << observedRatio << "). Normal exposure time " \
                    << (GetLongFromMetadata(normalResultMetadata, ANDROID_SENSOR_EXPOSURE_TIME) / 1000) \
                    << " us, sensitivity " << GetIntFromMetadata(normalResultMetadata, ANDROID_SENSOR_SENSITIVITY) \
                    << ". Compensated exposure time " << (GetLongFromMetadata(compensatedResultMetadata, ANDROID_SENSOR_EXPOSURE_TIME) / 1000) \
                    << " us, sensitivity " << GetIntFromMetadata(compensatedResultMetadata, ANDROID_SENSOR_SENSITIVITY);
                NT_LOG_INFO(const_cast<char*>(infoString.str().c_str()));
                NT_EXPECT( (1.0 - AE_COMPENSATION_ERROR_TOLERANCE) < error && error < (1.0 + AE_COMPENSATION_ERROR_TOLERANCE) ,
                               "Exposure compensation ratio ( %f ) exceeds error tolerance!", error);
            }

            NT_EXPECT(exposureCompensation == GetIntFromMetadata(compensatedResultMetadata, ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION),
                "Exposure compensation result should match requested value.");

            if (canSetAeLock)
            {
                NT_EXPECT(GetIntFromMetadata(compensatedResultMetadata, ANDROID_CONTROL_AE_LOCK) == ANDROID_CONTROL_AE_LOCK_ON,
                    "Exposure lock should be set.");
            }

            // Recover AE compensation and lock
            exposureCompensation = 0;
            Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION, &exposureCompensation, 1);
            if (canSetAeLock)
            {
                val = ANDROID_CONTROL_AE_LOCK_OFF;
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_LOCK, &val, 1);
            }
            WaitForSettingsApplied(cameraId, frameNumber, NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY);

            // Clean up metadata
            free_camera_metadata(const_cast<camera_metadata_t*>(compensatedResultMetadata));
            free_camera_metadata(const_cast<camera_metadata_t*>(normalResultMetadata));
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestTestPattern()
    *
    * @brief
    *   Sets and checks TEST_PATTERN modes in resulting snapshots
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestTestPattern(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump images
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_AVAILABLE_TEST_PATTERN_MODES,
            &entry);

        // This tag may be null on some devices
        if (ret != 0)
        {
            NT_LOG_WARN("GetCameraMetadataEntryByTag ANDROID_SENSOR_AVAILABLE_TEST_PATTERN_MODES failed");
            NT_LOG_UNSUPP("Camera %d does not support test patterns", cameraId);
        }
        else
        {
            for (size_t j = 0; j < entry.count; j++)
            {
                NT_LOG_DEBUG("Start testing test pattern value: %d", entry.data.i32[j]);
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_SENSOR_TEST_PATTERN_MODE, &entry.data.i32[j]);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                if (bDumpOutputBuffer)
                {
                    std::ostringstream outputName;
                    outputName << "TestTestPattern_Mode" << static_cast<int>(entry.data.i32[j]);
                    DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
                }
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestColorCorrectionAberration()
    *
    * @brief
    *   Sets and checks COLOR_CORRECTION_ABERRATION modes in resulting snapshots
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestColorCorrectionAberration(
        int cameraId,           //[in] camera Id
        int format,             //[in] format to dump images
        int frameNumber,        //[in] frame number
        camera_info * info,     //[in] camera info settings
        bool bDumpOutputBuffer) //[in] dump buffer
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES failed");

        for (size_t j = 0; j < entry.count; j++)
        {
            NT_LOG_DEBUG("Start testing color correction aberration value: %d", entry.data.u8[j]);
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_COLOR_CORRECTION_ABERRATION_MODE, &entry.data.u8[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestColorCorrectionAberration_Mode" << static_cast<int>(entry.data.u8[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::IsCaptureRequestKeySupported()
    *
    * @brief
    *   Queries whether a key is supported in capture request
    * @return
    *   True if supported, false if not
    **************************************************************************************************************************/
    bool CaptureRequestTest::IsCaptureRequestKeySupported(camera_info * info, int32_t tag)
    {
        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_REQUEST_KEYS, &entry);

        if (ret != 0)
        {
            NT_LOG_ERROR( "GetCameraMetadataEntryByTag ANDROID_REQUEST_AVAILABLE_REQUEST_KEYS failed");
            return false;
        }
        for (size_t i = 0; i < entry.count; i++)
        {
            if (tag == entry.data.i32[i])
            {
                return true;
            }
        }

        return false;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::IsCaptureResultKeySupported()
    *
    * @brief
    *   Queries whether a key is supported in capture result
    * @return
    *   True if supported, false if not
    **************************************************************************************************************************/
    bool CaptureRequestTest::IsCaptureResultKeySupported(camera_info * info, int32_t tag)
    {
        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_RESULT_KEYS,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_RESULT_AVAILABLE_RESULT_KEYS failed");
            return false;
        }
        for (size_t i = 0; i < entry.count; i++)
        {
            if (tag == entry.data.i32[i])
            {
                return true;
            }
        }

        return false;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::IsCharacteristicKeySupported()
    *
    * @brief
    *   Queries whether a characteristic key is supported
    * @return
    *   True if supported, false if not
    **************************************************************************************************************************/
    bool CaptureRequestTest::IsCharacteristicKeySupported(camera_info * info, int32_t tag)
    {
        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CHARACTERISTICS_KEYS,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_REQUEST_AVAILABLE_CHARACTERISTICS_KEYS failed");
            return false;
        }
        for (size_t i = 0; i < entry.count; i++)
        {
            if (tag == entry.data.i32[i])
            {
                return true;
            }
        }

        return false;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetMaxExposureValue()
    *
    * @brief
    *   Gets the maximum exposure value for the specified camera
    * @return
    *   int64_t exposure value, otherwise -1 if an error occurred
    **************************************************************************************************************************/
    int64_t CaptureRequestTest::GetMaxExposureValue(
        camera3_capture_request_t * request, //[in] capture request to look for setting in
        int64_t maxExposureTimeUs,           //[in] max exposure time in microseconds
        int64_t maxSensitivity)              //[in] max sensitivity
    {
        int32_t fpsRange[2];
        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(request->settings),
            ANDROID_CONTROL_AE_TARGET_FPS_RANGE,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_CONTROL_AE_TARGET_FPS_RANGE failed");
            return -1;
        }
        if (entry.count == 2)
        {
            fpsRange[0] = entry.data.i32[0];
            fpsRange[1] = entry.data.i32[1];
            NT_LOG_DEBUG("AE target fps range found to be [%" PRId32 ", %" PRId32 "]", fpsRange[0], fpsRange[1]);
            int64_t maxFrameDurationUs = round(1000000.0 / fpsRange[0]);
            int64_t currentMaxExposureTimeUs = (maxFrameDurationUs < maxExposureTimeUs) ? maxFrameDurationUs : maxExposureTimeUs;
            return currentMaxExposureTimeUs * maxSensitivity;
        }
        else
        {
            NT_LOG_ERROR("FPS range count not as expected: count = %d", entry.count);
            return -1;
        }

    }

    /**************************************************************************************************************************
    * CaptureRequestTest::ApplyAndVerifyKeySetting()
    *
    * @brief
    *   Apply and verify capture request key settings of type int32_t
    *   Applies specified key and value to capture request, start preview
    *   and wait for value to be reflected in capture result
    *
    *   @param
    *    [in]  int                  cameraId                cameraId associated with the pipeline
    *    [in]  int                  frameNumber             frameNumber of the last sent request
    *    [in]  uint32_t             tag                     Key tag of the feature to be tested
    *    [in]  const uint8_t*       value                   Value to be set for the key
    *    [in]  size_t               dataCount               Number of data points
    *    [in]  bool                 setKey                  setKey flag
    *
    * @return
    *   int frameNumber of the last capture request
    **************************************************************************************************************************/
    int CaptureRequestTest::ApplyAndVerifyKeySetting(int cameraId, int frameNumber, uint32_t tag, const uint8_t *value,
        size_t dataCount, bool setKey)
    {
        // apply requested value to the requested key
        if (setKey)
        {
            SetKey(cameraId, tag, value, dataCount);
        }

        int i = 0;

        while (i < PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            if(GetHalRequest(cameraId)->output_buffers == nullptr)
            {
                NT_LOG_ERROR("Failed to get output buffer for frame number: %d", frameNumber);
                return -1;
            }
            const auto frame_no = static_cast<uint32_t>(frameNumber);
            if (CDKResultSuccess != StreamStarter(cameraId, frame_no, false, 0, nullptr)) {
                NT_LOG_ERROR("Start Streaming Failed");
                return -1;
            }
            if (CheckKeySetInResult(tag, value, GetHalRequest(cameraId)->frame_number))
            {
                NT_LOG_INFO( "Key in capture result is set to the requested value");
                return frameNumber;
            }

            i++;
        }

        NT_EXPECT(1 == 0, "Capture Results does not contain the value set");
        return frameNumber;
    }


    /**************************************************************************************************************************
    * CaptureRequestTest::ApplyAndVerifyKeySetting()
    *
    * @brief
    *   Apply and verify capture request key settings of type int32_t
    *   Applies specified key and value to capture request, start preview
    *   and wait for value to be reflected in capture result (with error margin)
    * @return
    *   int frameNumber of the last capture request
    **************************************************************************************************************************/
    int CaptureRequestTest::ApplyAndVerifyKeySetting(
        int cameraId,
        int frameNumber, // frameNumber of the last sent request
        uint32_t tag, // Key tag of the feature to be tested
        const int32_t *value, // Value to be set for the key
        size_t dataCount, // Number of data points
        float errorMargin) // Error margin for verifying returned value in capture result
    {
        // apply requested value to the requested key
        Camera3Stream::SetKey(cameraId, tag, value, dataCount);

        int i = 0;

        while (i < PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            if (GetHalRequest(cameraId)->output_buffers == nullptr)
            {
                NT_LOG_ERROR("Failed to get output buffer for frame number: %d", frameNumber);
                return -1;
            }
            uint32_t frameNo = (uint32_t)frameNumber;
            if (CDKResultSuccess != StreamStarter(cameraId, frameNo, false, 0, nullptr)) {
                NT_LOG_ERROR("Start Streaming Failed");
                return -1;
            }

            if (Camera3Stream::CheckKeySetInResult(tag, value, errorMargin, GetHalRequest(cameraId)->frame_number))
            {
                NT_LOG_INFO( "Key in capture result is set to the requested value");
                return frameNumber;
            }

            i++;
        }

        NT_EXPECT(1 == 0, "Capture Results does not contain the value set \n");
        return frameNumber;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::ApplyAndVerifyKeySetting()
    *
    * @brief
    *   Apply and verify capture request key settings of type int64_t
    *   Applies specified key and value to capture request, start preview
    *   and wait for value to be reflected in capture result (with error margin)
    * @return
    *   int frameNumber of the last capture request
    **************************************************************************************************************************/
    int CaptureRequestTest::ApplyAndVerifyKeySetting(
        int cameraId,
        int frameNumber, // frameNumber of the last sent request
        uint32_t tag, // Key tag of the feature to be tested
        const int64_t *value, // Value to be set for the key
        size_t dataCount, // Number of data points
        float errorMargin) // Error margin for verifying returned value in capture result
    {
        // apply requested value to the requested key
        Camera3Stream::SetKey(cameraId, tag, value, dataCount);

        int i = 0;

        while (i < PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            if (GetHalRequest(cameraId)->output_buffers == nullptr)
            {
                NT_LOG_ERROR("Failed to get output buffer for frame number: %d", frameNumber);
                return -1;
            }
            uint32_t frameNo = (uint32_t)frameNumber;
            if (CDKResultSuccess != StreamStarter(cameraId, frameNo, false, 0, nullptr)) {
                NT_LOG_ERROR("Start Streaming Failed");
                return -1;
            }
            if (Camera3Stream::CheckKeySetInResult(tag, value, errorMargin, GetHalRequest(cameraId)->frame_number))
            {
                NT_LOG_INFO("Key in capture result is set to the requested value");
                return frameNumber;
            }

            i++;
        }

        NT_EXPECT(1 == 0, "Capture Results does not contain the value set \n");
        return frameNumber;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::ApplyAndVerifyKeySetting()
    *
    * @brief
    *   Apply and verify capture request key settings of type float
    *   Applies specified key and value to capture request, start preview
    *   and wait for value to be reflected in capture result
    * @return
    *   int frameNumber of the last capture request
    **************************************************************************************************************************/
    int CaptureRequestTest::ApplyAndVerifyKeySetting(
        int cameraId,
        int frameNumber, // frameNumber of the last sent request
        uint32_t tag, // Key tag of the feature to be tested
        const float * value, // Value to be set for the key
        size_t dataCount, // Number of data points
        bool skipVerification) // Flag to skip value verification
    {

        // apply requested value to the requested key
        Camera3Stream::SetKey(cameraId, tag, value, dataCount);

        int i = 0;

        while (i < PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            if (GetHalRequest(cameraId)->output_buffers == nullptr)
            {
                NT_LOG_ERROR("Failed to get output buffer for frame number: %d", frameNumber);
                return -1;
            }
            uint32_t frameNo = (uint32_t)frameNumber;
            if (CDKResultSuccess != StreamStarter(cameraId, frameNo, false, 0, nullptr)) {
                NT_LOG_ERROR("Start Streaming Failed");
                return -1;
            }

            if (!skipVerification)
            {
                if (Camera3Stream::CheckKeySetInResult(tag, value, GetHalRequest(cameraId)->frame_number))
                {
                    NT_LOG_INFO("Key in capture result is set to the requested value");
                    return frameNumber;
                }
            }
            else
            {
                // No need to wait for capture result value match
                return frameNumber;
            }

            i++;
        }

        NT_EXPECT(1 == 0, "Capture Results does not contain the value set \n");
        return frameNumber;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::ApplyAndVerifyKeySetting()
    *
    * @brief
    *   Apply and verify capture request key settings of type camera_metadata_rational_t
    *   Applies specified key and value to capture request, start preview
    *   and wait for value to be reflected in capture result
    * @return
    *   int frameNumber of the last capture request
    **************************************************************************************************************************/
    int CaptureRequestTest::ApplyAndVerifyKeySetting(
        int cameraId,
        int frameNumber, // frameNumber of the last sent request
        uint32_t tag, // Key tag of the feature to be tested
        const camera_metadata_rational_t * value, // Value to be set for the key
        size_t dataCount, // Number of data points
        bool skipVerification) // Flag to skip verification
    {

        // apply requested value to the requested key
        Camera3Stream::SetKey(cameraId, tag, value, dataCount);

        int i = 0;

        while (i < PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            if (GetHalRequest(cameraId)->output_buffers == nullptr)
            {
                NT_LOG_ERROR("Failed to get output buffer for frame number: %d", frameNumber);
                return -1;
            }

            if (CDKResultSuccess != StreamStarter(cameraId, frameNumber, false, 0, nullptr)) {
                NT_LOG_ERROR("Start Streaming Failed");
                return -1;
            }
            if (!skipVerification)
            {
                if (Camera3Stream::CheckKeySetInResult(tag, value, GetHalRequest(cameraId)->frame_number))
                {
                    NT_LOG_INFO("Key in capture result is set to the requested value");
                    return frameNumber;
                }
            }
            else
            {
                // No need to wait for capture result value match
                return frameNumber;
            }

            i++;
        }

        NT_EXPECT(1 == 0, "Capture Results does not contain the value set \n");
        return frameNumber;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::ApplyAndVerifyKeySettingWith3AConverged()
    *
    * @brief
    *   Apply and verify capture request key settings of type int32_t. Applies specified key and value to capture request,
    *   starts a process capture request with 3A routine, and checks the final capture result for the expected value.
    * @return
    *   int frameNumber of the last capture request
    **************************************************************************************************************************/
    int CaptureRequestTest::ApplyAndVerifyKeySettingWith3AConverged(
        int cameraId,         //[in] camera Id
        int frameNumber,      //[in] frameNumber of the last sent request
        uint32_t tag,         //[in] Key tag of the feature to be tested
        const uint8_t *value, //[in] Value to be set for the key
        size_t dataCount)     //[in] Number of data points
    {
        // Apply requested value to the requested key
        Camera3Stream::SetKey(cameraId, tag, value, dataCount);

        frameNumber = ProcessCaptureRequestWith3AConverged(cameraId, ++frameNumber, CAMERA3_TEMPLATE_STILL_CAPTURE, 0, false, "");
        if (frameNumber == -1)
        {
            NT_EXPECT(1 == 0, "3A did not converge!");
            return frameNumber;
        }

        if (Camera3Stream::CheckKeySetInResult(tag, value, frameNumber))
        {
            NT_LOG_INFO("Key in capture result is set to the requested value");
            return frameNumber;
        }

        NT_EXPECT(1 == 0, "Capture Results does not contain the value set \n");
        return frameNumber;
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetExposureValue()
    *
    * @brief
    *   Get exposure value from a result's metadata
    * @return
    *   int64_t exposure value, otherwise -1 if error
    **************************************************************************************************************************/
    int64_t CaptureRequestTest::GetExposureValue(
        const camera_metadata_t * metadata) //[in] metadata to retrieve exposure details from
    {
        // Get exposure time and sensitivity
        int64_t expTimeUs = GetLongFromMetadata(metadata, ANDROID_SENSOR_EXPOSURE_TIME) / 1000;
        int32_t sensitivity = GetIntFromMetadata(metadata, ANDROID_SENSOR_SENSITIVITY);

        // Get post raw sensitivity boost
        int32_t postRawSensitivity;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(metadata),
            ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST,
            &entry);
        if (ret != 0)
        {
            // Post raw sensitivity may be null (not found) if not supported by device
            postRawSensitivity = -1;
        }
        else if (entry.count != 1)
        {
            NT_LOG_ERROR("Post raw sensitivity boost data not as expected!");
            return -1;
        }
        postRawSensitivity = entry.data.i32[0];

        // Check for errors
        if (expTimeUs < 0 || sensitivity < 0)
        {
            NT_LOG_ERROR("Error during exposure value metadata details retrieval!");
            return -1;
        }

        // Calculate final exposure value
        if (postRawSensitivity != -1)
        {
            return static_cast<int64_t>(sensitivity) * (postRawSensitivity / 100) * expTimeUs;
        }
        else
        {
            return static_cast<int64_t>(sensitivity) * expTimeUs;
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetIntFromMetadata()
    *
    * @brief
    *   Get int32_t value from a result's metadata.
    * @return
    *   int32_t value, otherwise -1 if error
    **************************************************************************************************************************/
    int32_t CaptureRequestTest::GetIntFromMetadata(
        const camera_metadata_t * metadata, //[in] metadata to read from
        camera_metadata_tag_t tag)          //[in] tag to look for in metadata
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(metadata),
            tag,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag %d failed", tag);
            return -1;
        }
        if (entry.count != 1)
        {
            NT_LOG_ERROR("No data found for tag %d!", tag);
            return -1;
        }
        return entry.data.i32[0];
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetLongFromMetadata()
    *
    * @brief
    *   Get int64_t (long) value from a result's metadata.
    * @return
    *   int64_t value, otherwise -1 if error
    **************************************************************************************************************************/
    int64_t CaptureRequestTest::GetLongFromMetadata(
        const camera_metadata_t * metadata, //[in] metadata to read from
        camera_metadata_tag_t tag)          //[in] tag to look for in metadata
    {
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(metadata),
            tag,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag %d failed", tag);
            return -1;
        }
        if (entry.count != 1)
        {
            NT_LOG_ERROR("No data found for tag %d!", tag);
            return -1;
        }
        return entry.data.i64[0];
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::WaitForAeLocked()
    *
    * @brief
    *   Wait for AE_STATE to converge to AE_STATE_LOCKED
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::WaitForAeLocked(
        camera_info* info,  //[in] camera info for static metadata
        int cameraId,       //[in] camera Id
        int &frameNumber,   //[in/out] frame number of the current capture
        int numResultsWait) //[in] number of frames to wait
    {
        WaitForSettingsApplied(cameraId, frameNumber, numResultsWait);

        // Check for at least HARDWARE_LEVEL_LIMITED, to indicate support for AE Lock
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL failed");
        NT_ASSERT(entry.count == 1 && entry.data.i32[0] != ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY,
             "ANDROID_CONTROL_AE_STATE not supported by this device (HARDWARE_LEVEL_LEGACY)");

        uint8_t expectedAeStates[] = { ANDROID_CONTROL_AE_STATE_LOCKED };
        WaitForAnyStateValue(cameraId, frameNumber, ANDROID_CONTROL_AE_STATE, expectedAeStates, 1, NUM_RESULTS_WAIT_TIMEOUT);
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::WaitForAeStable()
    *
    * @brief
    *   Wait for AE_STATE to converge to CONVERGED or FLASH_REQUIRED
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::WaitForAeStable(
        camera_info* info,  //[in] camera info for static metadata
        int cameraId,       //[in] camera Id
        int &frameNumber,   //[in/out] frame number of the current capture
        int numResultsWait) //[in] number of frames to wait
    {
        WaitForSettingsApplied(cameraId, frameNumber, numResultsWait);

        // Check for at least HARDWARE_LEVEL_LIMITED
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL failed");
        NT_ASSERT(entry.count == 1 && entry.data.i32[0] != ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY,
              "ANDROID_CONTROL_AE_STATE not supported by this device (HARDWARE_LEVEL_LEGACY)");

        uint8_t expectedAeStates[] = { ANDROID_CONTROL_AE_STATE_CONVERGED, ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED };
        WaitForAnyStateValue(cameraId, frameNumber, ANDROID_CONTROL_AE_STATE, expectedAeStates, 2, NUM_RESULTS_WAIT_TIMEOUT);
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::WaitForAnyStateValue()
    *
    * @brief
    *   Wait for given state to converge to one of the expected values
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::WaitForAnyStateValue(
        int cameraId,               //[in] camera Id
        int &frameNumber,           //[in/out] frame number of the current capture
        int stateTag,               //[in] state metadata tag that is being tracked
        uint8_t expectedValues[],   //[in] expected values for the state tag
        int numExpectedValues,      //[in] number of expected values
        int numResultsWait)         //[in] number of frames to wait
    {
        NT_ASSERT(numResultsWait > 0 && numExpectedValues > 0, "Num Results less than 0");

        camera_metadata_entry_t entry;
        bool expectedFound = false;
        uint8_t lastState = UINT8_MAX;

        for (int i = 0; i < numResultsWait; i++)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for frame number: %d",frameNumber);
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                "Start Streaming Failed");
            const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNumber);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
                stateTag, &entry);
            lastState = entry.data.u8[0];
            int64_t exposure = GetExposureValue(metaFromFile);
            free_camera_metadata(const_cast<camera_metadata_t*>(metaFromFile));

            if (ret == 0 && entry.count > 0)
            {
                // Check if the state matches any of the expected values
                for (int j = 0; j < numExpectedValues; j++)
                {
                    NT_LOG_DEBUG("State for tag %d is %" PRIu8, stateTag, lastState);
                    NT_LOG_DEBUG("Current exposure = %" PRId64, exposure);
                    if (lastState == expectedValues[j])
                    {
                        expectedFound = true;
                    }
                }

                if (expectedFound)
                {
                    break;
                }
            }
            else
            {
                NT_LOG_WARN("Could not find tag %d metadata entry!", stateTag);
            }
        }

        NT_ASSERT(expectedFound, "State failed to reach expected values for tag %d , after waiting for %d results !", stateTag, numResultsWait);
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::WaitForSettingsApplied()
    *
    * @brief
    *   Wait for the given number of frames for settings to apply
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::WaitForSettingsApplied(
        int cameraId,       //[in] camera Id
        int &frameNumber,   //[in/out] frame number of the current capture
        int numResultsWait) //[in] number of frames to wait
    {
        for (int i = 0; i < numResultsWait; i++)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr,"Failed to get output buffer for frame number: %d" ,frameNumber);
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                "Start Streaming Failed");
            const camera_metadata_t* metadata = GetCameraMetadataByFrame(frameNumber);
            NT_ASSERT(metadata != nullptr, "Meta read from file is null \n");
            int64_t exposure = GetExposureValue(metadata);
            free_camera_metadata(const_cast<camera_metadata_t*>(metadata));
            NT_LOG_DEBUG("Current exposure = %" PRId64, exposure)
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetSensitivityMinimumOrDefault()
    *
    * @brief
    *   Get sensitivity minimum or default value
    * @return
    *   int32_t sensitivity ISO value
    **************************************************************************************************************************/
    int32_t CaptureRequestTest::GetSensitivityMinimumOrDefault(
        camera_info* info)  //[in] camera info for static metadata
    {
        int32_t defaultValue = SENSOR_INFO_SENSITIVITY_RANGE_MIN_AT_MOST;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_SENSITIVITY_RANGE,
            &entry);
        if (ret == 0 && entry.count == 2)
        {
            NT_LOG_INFO("Found sensitivity minimum value of %" PRId32, entry.data.i32[0]);
            return entry.data.i32[0];
        }
        else
        {
            NT_LOG_WARN("GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_SENSITIVITY_RANGE failed");
            NT_LOG_WARN("Using default value (%" PRId32 ") as sensitivity minimum", defaultValue);
            return defaultValue;
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetSensitivityMaximumOrDefault()
    *
    * @brief
    *   Get sensitivity maximum or default value
    * @return
    *   int32_t sensitivity ISO value
    **************************************************************************************************************************/
    int32_t CaptureRequestTest::GetSensitivityMaximumOrDefault(
        camera_info* info)  //[in] camera info for static metadata
    {
        int32_t defaultValue = SENSOR_INFO_SENSITIVITY_RANGE_MAX_AT_LEAST;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_SENSITIVITY_RANGE,
            &entry);
        if (ret == 0 && entry.count == 2)
        {
            NT_LOG_INFO("Found sensitivity maximum value of %" PRId32, entry.data.i32[1]);
            return entry.data.i32[1];
        }
        else
        {
            NT_LOG_WARN("GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_SENSITIVITY_RANGE failed");
            NT_LOG_WARN("Using default value (%" PRId32 ") as sensitivity maximum", defaultValue);
            return defaultValue;
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetExposureMinimumOrDefault()
    *
    * @brief
    *   Get exposure minimum or default value
    * @return
    *   int64_t exposure time
    **************************************************************************************************************************/
    int64_t CaptureRequestTest::GetExposureMinimumOrDefault(
        camera_info* info)  //[in] camera info for static metadata
    {
        int64_t defaultValue = SENSOR_INFO_EXPOSURE_TIME_RANGE_MIN_AT_MOST;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE,
            &entry);
        if (ret == 0 && entry.count == 2)
        {
            NT_LOG_INFO("Found exposure minimum value of %" PRId64, entry.data.i64[0]);
            return entry.data.i64[0];
        }
        else
        {
            NT_LOG_WARN("GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE failed");
            NT_LOG_WARN("Using default value (%" PRId64 ") as exposure minimum", defaultValue);
            return defaultValue;
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::GetExposureMaximumOrDefault()
    *
    * @brief
    *   Get exposure maximum or default value
    * @return
    *   int64_t exposure time
    **************************************************************************************************************************/
    int64_t CaptureRequestTest::GetExposureMaximumOrDefault(
        camera_info* info)  //[in] camera info for static metadata
    {
        int64_t defaultValue = SENSOR_INFO_EXPOSURE_TIME_RANGE_MAX_AT_LEAST;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE,
            &entry);
        if (ret == 0 && entry.count == 2)
        {
            NT_LOG_INFO("Found exposure maximum value of %" PRId64, entry.data.i64[1]);
            return entry.data.i64[1];
        }
        else
        {
            NT_LOG_WARN("GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE failed");
            NT_LOG_WARN("Using default value (%" PRId64 ") as exposure maximum", defaultValue);
            return defaultValue;
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestValidateEnableZSL()
    *
    * @brief
    *   Sets the ANDROID_CONTROL_ENABLE_ZSL tag to available values (true and false), and validates the same from metadata
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestValidateEnableZSL(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer)
    {
        NT_ASSERT(info != nullptr, "Camera Info is null");

        uint8_t tagTrue  = camera_metadata_enum_android_control_enable_zsl_t::ANDROID_CONTROL_ENABLE_ZSL_TRUE;
        uint8_t tagFalse = camera_metadata_enum_android_control_enable_zsl_t::ANDROID_CONTROL_ENABLE_ZSL_FALSE;

        std::vector <uint8_t> availableZSLModes = { tagTrue, tagFalse };

        for (size_t j = 0; j < availableZSLModes.size(); j++)
        {
            NT_LOG_DEBUG("start testing ZSL mode value: %d", static_cast<int>(availableZSLModes[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_ENABLE_ZSL, &availableZSLModes[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
            if (bDumpOutputBuffer)
            {
                std::ostringstream outputName;
                outputName << "TestValidateEnableZSL" << static_cast<int>(availableZSLModes[j]);
                DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
            }
        }

    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestHDR()
    *
    * @brief
    *   Tests HDR scene mode
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestHDR(int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        uint32_t tagID_SceneMode = ANDROID_CONTROL_SCENE_MODE;
        uint8_t  tagValue_HDR = ANDROID_CONTROL_SCENE_MODE_HDR;
        camera_metadata_entry_t entry;

        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_SCENE_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_SCENE_MODES failed");

        // scan through ANDROID_CONTROL_SCENE_MODE and check if HDR mode supported
        bool isHDRModeSupported = false;
        for (size_t j = 0; j < entry.count; j++)
        {
            // if HDR mode supported, set and validate
            if (entry.data.u8[j] == tagValue_HDR)
            {
                NT_LOG_INFO("Testing HDR mode");
                isHDRModeSupported = true;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, tagID_SceneMode, &tagValue_HDR);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                break;
            }
        }

        if (!isHDRModeSupported)
        {
            NT_LOG_UNSUPP("Device doesn't support HDR mode, skip");
            return;
        }

        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "TestHDR";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestMFNR()
    *
    * @brief
    *   Tests MFNR (Noise Reduction High Quality) mode
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestMFNR(int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        NATIVETEST_UNUSED_PARAM(info);
        bool isHQModeSupported = SetMFNRMode(cameraId);
        if (isHQModeSupported)
        {
            uint32_t tagID_NRMode = ANDROID_NOISE_REDUCTION_MODE;
            uint8_t  tagValue_NRHQ = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, tagID_NRMode, &tagValue_NRHQ, 1, false);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
        }
        else
        {
            NT_LOG_UNSUPP("Device doesn't support HIGH_QUALITY mode, skip");
            return;
        }

        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "TestMFNR";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestHDRWithMFNR()
    *
    * @brief
    *   Tests HDR and MFNR (Noise Reduction High Quality) modes, with priority given to HDR mode (by driver) if both modes set
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestHDRWithMFNR(int cameraId,
        int format,
        int frameNumber,
        camera_info *info,
        bool bDumpOutputBuffer)
    {
        uint32_t tagID_SceneMode = ANDROID_CONTROL_SCENE_MODE;
        uint8_t  tagValue_HDR    = ANDROID_CONTROL_SCENE_MODE_HDR;
        camera_metadata_entry_t entry;

        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_SCENE_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_SCENE_MODES failed");

        // scan through ANDROID_CONTROL_SCENE_MODE and check if HDR mode supported
        bool isHDRModeSupported = false;
        for (size_t j = 0; j < entry.count; j++)
        {
            // if HDR mode supported, set and validate
            if (entry.data.u8[j] == tagValue_HDR)
            {
                NT_LOG_INFO("Testing HDR mode");
                isHDRModeSupported = true;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, tagID_SceneMode, &tagValue_HDR);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                break;
            }
        }

        if (!isHDRModeSupported)
        {
            NT_LOG_UNSUPP("Device doesn't support HDR mode, skip");
            return;
        }

        bool isHQModeSupported = SetMFNRMode(cameraId);
        if (isHQModeSupported)
        {
            uint32_t tagID_NRMode = ANDROID_NOISE_REDUCTION_MODE;
            uint8_t  tagValue_NRHQ = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, tagID_NRMode, &tagValue_NRHQ, 1, false);
            NT_ASSERT(frameNumber != -1, "Frame number not set");
        }
        else
        {
            NT_LOG_UNSUPP("Device doesn't support HIGH_QUALITY mode, skip");
            return;
        }

        if (bDumpOutputBuffer)
        {
            std::ostringstream outputName;
            outputName << "TestHDRWithMFNR";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, outputName.str().c_str()).c_str());
        }
    }

    /**************************************************************************************************************************
    * CaptureRequestTest::TestHDRWithFlash()
    *
    * @brief
    *   Tests HDR with Flash modes enabled
    * @return
    *   void
    **************************************************************************************************************************/
    void CaptureRequestTest::TestHDRWithFlash(int cameraId, int format, int frameNumber, camera_info * info, bool bDumpOutputBuffer)
    {
        uint32_t tagID_SceneMode = ANDROID_CONTROL_SCENE_MODE;
        uint8_t  tagValue_HDR    = ANDROID_CONTROL_SCENE_MODE_HDR;
        camera_metadata_entry_t entry;

        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_CONTROL_AVAILABLE_SCENE_MODES,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_AVAILABLE_SCENE_MODES failed");

        // scan through ANDROID_CONTROL_SCENE_MODE and check if HDR mode supported
        bool isHDRModeSupported = false;
        for (size_t j = 0; j < entry.count; j++)
        {
            // if HDR mode supported, set and validate
            if (entry.data.u8[j] == tagValue_HDR)
            {
                NT_LOG_INFO("Testing HDR mode");
                isHDRModeSupported = true;
                frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, tagID_SceneMode, &tagValue_HDR);
                NT_ASSERT(frameNumber != -1, "Frame number not set");
                break;
            }
        }

        if (!isHDRModeSupported)
        {
            NT_LOG_UNSUPP("Device doesn't support HDR mode, skip");
            return;
        }

        // check if unit has flash support
        ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_FLASH_INFO_AVAILABLE,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_FLASH_INFO_AVAILABLE failed");

        if (!(entry.data.u8[0] == ANDROID_FLASH_INFO_AVAILABLE_TRUE))
        {
            NT_LOG_UNSUPP("Flash is NOT supported, skip");
            return;
        }

        // Flash control can only be used when the AE mode is ON or OFF, test for ON
        NT_LOG_DEBUG("start testing ae mode: " stringify(ANDROID_CONTROL_AE_MODE_ON));
        uint8_t aeMode = ANDROID_CONTROL_AE_MODE_ON;
        frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_CONTROL_AE_MODE, &aeMode);
        NT_ASSERT(frameNumber != -1, "Frame number not set");

        // Test all available Flash modes
        uint8_t flashOff = camera_metadata_enum_android_flash_mode_t::ANDROID_FLASH_MODE_OFF;
        uint8_t flashSingle = camera_metadata_enum_android_flash_mode_t::ANDROID_FLASH_MODE_SINGLE;
        uint8_t flashTorch = camera_metadata_enum_android_flash_mode_t::ANDROID_FLASH_MODE_TORCH;
        uint8_t flashStateFired = ANDROID_FLASH_STATE_FIRED;
        bool stateReached = false;

        std::vector <uint8_t> availableFlashModes = { flashSingle, flashOff, flashTorch };

        for (size_t j = 0; j < availableFlashModes.size(); j++)
        {

            NT_LOG_DEBUG("Testing flash mode value: %d", static_cast<int>(availableFlashModes[j]));
            frameNumber = ApplyAndVerifyKeySetting(cameraId, frameNumber, ANDROID_FLASH_MODE, &availableFlashModes[j]);
            NT_ASSERT(frameNumber != -1, "Frame number not set");

            // Except for OFF mode, check if flash reached fired state
            if (static_cast<int>(availableFlashModes[j]) != flashOff)
            {
                if (Camera3Stream::CheckKeySetInResult(ANDROID_FLASH_STATE, &flashStateFired, GetHalRequest(cameraId)->frame_number))
                {
                    stateReached = true;
                }
                NT_ASSERT(stateReached, "Flash failed to reach FIRED state");
                stateReached = false; // set the state back to original
            }
        }

        if (bDumpOutputBuffer)
        {
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, format, -1, "TestHDRWithFlash").c_str());
        }

    }

}
