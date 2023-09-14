//*************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#include "robustness_test.h"

namespace haltests
{

    RobustnessTest::RobustnessTest(): afModes(nullptr), aeModes(nullptr), supportedStreams(nullptr)
    {
    }

    RobustnessTest::~RobustnessTest()
    {
    }

    void RobustnessTest::Setup()
    {
        Camera3Stream::Setup();
    }

    /**************************************************************************************************************************
    * RobustnessTest::GetAfStateFromMeta()
    *
    * @brief
    *   Helper to get the autofocus state from metadata
    * @return
    *   uint8_t
    **************************************************************************************************************************/
    uint8_t RobustnessTest::GetAfStateFromMeta(const camera_metadata_t* metaFromFile)
    {

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            ANDROID_CONTROL_AF_STATE, &entry);

        if (ret == 0 && entry.count > 0)
        {
            return entry.data.u8[0];
        }
        NT_LOG_ERROR("FindCameraMetadataEntryByTag ANDROID_CONTROL_AF_STATE failed");
        return UINT8_MAX;
    }

    /**************************************************************************************************************************
    * RobustnessTest::GetAeStateFromMeta()
    *
    * @brief
    *   Helper to get the auto-exposure state from metadata
    * @return
    *   uint8_t
    **************************************************************************************************************************/
    uint8_t RobustnessTest::GetAeStateFromMeta(const camera_metadata_t* metaFromFile)
    {

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
                                             ANDROID_CONTROL_AE_STATE, &entry);

        if (ret == 0 && entry.count > 0)
        {
            return entry.data.u8[0];
        }
        NT_LOG_ERROR("FindCameraMetadataEntryByTag ANDROID_CONTROL_AE_STATE failed");
        return UINT8_MAX;
    }

    /**************************************************************************************************************************
    * RobustnessTest::TestBadSurfaceDimension()
    *
    * @brief
    *   Verifies device has unsupported resolutions and stream configurations
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::TestBadSurfaceDimension()
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // open camera
            Camera3Device::OpenCamera(cameraId);

            camera_info info;
            if (0 != GetCamModule()->get_camera_info(cameraId, &info))
            {
                NT_ASSERT(1 == 0, "Can't get camera info for camera id = %d \n", cameraId);
            }

            camera_metadata_entry configs;
            int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                                                  ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
                                                  &configs);

            NT_ASSERT(val == 0, "Failed to obtain stream configuartion for camera id = %d \n",cameraId);

            std::map<uint32_t, uint32_t> supportedResolutions;
            int count = 0;


            // Add all the supported yuv_420 resolutions
            for (uint32_t j = 0; j < configs.count; j += 4) {
                if (configs.data.i32[j + 3] == ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT && configs.data.i32[j] == HAL_PIXEL_FORMAT_YCbCr_420_888)
                {
                    supportedResolutions[configs.data.i32[j + 1]] = configs.data.i32[j + 2];
                }
            }

            Size wrongSize = Size(643, 577);

            // checking if more than 100 wrong sizes are supported
            while (supportedResolutions.find(wrongSize.width) != supportedResolutions.end())
            {
                if (supportedResolutions.find(wrongSize.width)->second == wrongSize.height)
                {
                    count++;
                    wrongSize = Size(wrongSize.width + 1, wrongSize.height + 1);
                    NT_ASSERT(count < 100, "Too many exotic YUV_420_888 resolutions supported. \n");
                }

            }

            // Create one output stream with wrong size
            camera3_stream_t *newStream = new camera3_stream_t();

            newStream->stream_type = CAMERA3_STREAM_OUTPUT;
            newStream->width = wrongSize.width;
            newStream->height = wrongSize.height;
            newStream->format = HAL_PIXEL_FORMAT_YCbCr_420_888;
            newStream->data_space = static_cast<android_dataspace>(0x23);
            newStream->rotation = CAMERA3_STREAM_ROTATION_0;
            newStream->usage = 0;
            newStream->max_buffers = 0;
            newStream->priv = nullptr;

            camera3_stream_configuration config = {};
            config.operation_mode = CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE;
            config.num_streams = 1;
            camera3_stream_t* mixedStreams[1];
            mixedStreams[0] = newStream;
            config.streams = reinterpret_cast<camera3_stream_t **>(mixedStreams);
            camera3_device_t* pCamDev = GetCamDevice(cameraId);
            NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
            int res = pCamDev->ops->configure_streams(pCamDev, &config);
            NT_ASSERT(res != 0, "Stream configuration is expected to fail with invalid resolution!");

            delete(newStream);

            // close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::GetAvailableAfandAeModes()
    *
    * @brief
    *   Helper to get all the supported autofocus and autoexposure modes
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::GetAvailableAfandAeModes(camera_info info)
    {
        camera_metadata_entry configs;
        int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                                              ANDROID_CONTROL_AF_AVAILABLE_MODES,
                                              &configs);

        NT_ASSERT(val == 0, "Failed to obtain available AF modes \n");

        afCount = configs.count;
        afModes = SAFE_NEW() uint8_t[afCount];

        for (uint32_t j = 0; j < afCount; j++) {
            afModes[j] = configs.data.u8[j];
        }

        val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                                          ANDROID_CONTROL_AE_AVAILABLE_MODES,
                                          &configs);

        NT_ASSERT(val == 0, "Failed to obtain available AE modes \n");

        aeCount = configs.count;
        aeModes = SAFE_NEW() uint8_t[aeCount];

        for (uint32_t j = 0; j < aeCount; j++) {
            aeModes[j] = configs.data.u8[j];
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::GetSupportedResolutions()
    *
    * @brief
    *   Helper to get all the stream configurations (format + resolution + direction) that a device supports
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::GetSupportedResolutions(
        camera_info info,
        int requestedDirection,
        int& numSupportedStreams)
    {
        camera_metadata_entry_t entry;
        int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
            &entry);
        NT_ASSERT(val == 0, "Can't find the metadata entry for ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS.");

        const int STREAM_CONFIGURATION_SIZE = 4;
        const int STREAM_WIDTH_OFFSET = 1;
        const int STREAM_HEIGHT_OFFSET = 2;
        const int STREAM_DIRECTION_OFFSET = 3;

        numSupportedStreams = 0;
        supportedStreams = new RobustnessItem[entry.count];

        for (size_t i = 0; i < entry.count; i += STREAM_CONFIGURATION_SIZE)
        {
            int32_t format1;
            int32_t format2;
            Size res1;
            Size res2;
            int32_t streamDir1;
            int32_t streamDir2;

            format1 = entry.data.i32[i];
            res1 = Size(static_cast<uint32_t>(entry.data.i32[i + STREAM_WIDTH_OFFSET]),
                        static_cast<uint32_t>(entry.data.i32[i + STREAM_HEIGHT_OFFSET]));
            streamDir1 = entry.data.i32[i + STREAM_DIRECTION_OFFSET];

            // Look for resolutions that support both OUTPUT and INPUT
            // This assumes that INPUT will the next consecutive entry after OUTPUT of the same resolution (if it is supported)
            if (requestedDirection == CAMERA3_STREAM_BIDIRECTIONAL)
            {
                format2 = entry.data.i32[i + STREAM_CONFIGURATION_SIZE];
                res2 = Size(static_cast<uint32_t>(entry.data.i32[i + STREAM_WIDTH_OFFSET + STREAM_CONFIGURATION_SIZE]),
                            static_cast<uint32_t>(entry.data.i32[i + STREAM_HEIGHT_OFFSET + STREAM_CONFIGURATION_SIZE]));
                streamDir2 = entry.data.i32[i + STREAM_DIRECTION_OFFSET + STREAM_CONFIGURATION_SIZE];

                if ((streamDir1 == CAMERA3_STREAM_OUTPUT && streamDir2 == CAMERA3_STREAM_INPUT) &&
                    res1.width == res2.width && res1.height == res2.height && format1 == format2)
                {
                    supportedStreams[numSupportedStreams].format = format1;
                    supportedStreams[numSupportedStreams++].resolution = res1;
                    supportedStreams[numSupportedStreams].format = format2;
                    supportedStreams[numSupportedStreams++].resolution = res2;
                    i += STREAM_CONFIGURATION_SIZE; // Skip over INPUT config
                }
            }
            else
            {
                if (streamDir1 == requestedDirection)
                {
                    supportedStreams[numSupportedStreams].format = format1;
                    supportedStreams[numSupportedStreams++].resolution = res1;
                }
            }
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::TestBasicTriggerSequence()
    *
    * @brief
    *   Wrapper for testing basic trigger sequence. This allows proper cleanup if the underlying function fails an ASSERT
    *   statement.
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::TestBasicTriggerSequence()
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // open camera
            OpenCamera(cameraId);

            camera_info info;
            if (0 != GetCamModule()->get_camera_info(cameraId, &info))
            {
                NT_LOG_ERROR("Can't get camera info for camera id: %d", cameraId);
                // close camera
                CloseCamera(cameraId);
                continue;
            }

            GetAvailableAfandAeModes(info);

            if (PrepareTriggerTestSession(cameraId))
            {
                BasicTriggerSequenceHelper(cameraId);
            }

            delete[] afModes;
            delete[] aeModes;

            // clear streams
            ClearOutStreams();

            // close camera
            CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::TestSimultaneousTriggers()
    *
    * @brief
    *   Wrapper for testing simultaneous triggers. This allows proper cleanup if the underlying function fails an ASSERT
    *   statement.
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::TestSimultaneousTriggers()
    {
        std::vector<int> camList = GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // open camera
            OpenCamera(cameraId);

            camera_info info;
            if (0 != GetCamModule()->get_camera_info(cameraId, &info))
            {
                NT_LOG_ERROR("Can't get camera info for camera id: %d", cameraId);
                // close camera
                CloseCamera(cameraId);
                continue;
            }

            GetAvailableAfandAeModes(info);

            if (PrepareTriggerTestSession(cameraId))
            {
                SimultaneousTriggersHelper(cameraId);
            }

            delete[] afModes;
            delete[] aeModes;

            // clear streams
            ClearOutStreams();

            // close camera
            CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::TestResolutionSweep()
    *
    * @brief
    *   Wrapper for testing resolution sweep. This allows proper cleanup if the underlying function fails an ASSERT
    *   statement.
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::TestResolutionSweep()
    {
        std::vector<int> camList = GetCameraList();

        NT_ASSERT(0 < static_cast<int>(camList.size()), "No cameras found");

        int captureCount = 1;

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // open camera
            OpenCamera(cameraId);

            camera_info info;
            if (0 != GetCamModule()->get_camera_info(cameraId, &info))
            {
                NT_LOG_ERROR("Can't get camera info for camera id: %d", cameraId);
                // close camera
                CloseCamera(cameraId);
                continue;
            }

            int numSupportedStreams = 0;
            GetSupportedResolutions(info, CAMERA3_STREAM_OUTPUT, numSupportedStreams);

            ResolutionSweepHelper(cameraId, numSupportedStreams, captureCount);

            delete[] supportedStreams;

            // close camera
            CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::ResolutionSweepHelper()
    *
    * @brief
    *   Helper to avoid memory leaks in TestResolutionSweep when ASSERT statements are used
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::ResolutionSweepHelper(int cameraId, int numSupportedStreams, int captureCount)
    {
        bool firstRun = true;
        int frameNumber = 0;

        for (int streamIndex = 0; streamIndex < numSupportedStreams; streamIndex++)
        {
            int format = supportedStreams[streamIndex].format;
            Size resolution = supportedStreams[streamIndex].resolution;

            // Create one output stream
            CreateStream(CAMERA3_STREAM_OUTPUT, resolution, 0, format, CAMERA3_STREAM_ROTATION_0);

            int configResult = ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId);
            NT_EXPECT(configResult == 0, "Configure streams failed for format: %d, resolution : %s", format,
                ConvertResToString(resolution).c_str());
            if (configResult != 0)
            {
                NT_LOG_WARN("Skipping to next format/resolution...");
                continue;
            }

#ifdef ENABLE3A
            if (firstRun)
            {
                frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(cameraId, frameNumber,
                    CAMERA3_TEMPLATE_PREVIEW, 0, // output stream index
                    false, "");
                NT_ASSERT(frameNumber != -1, "3A cannot converge!");
                firstRun = false;
            }
#endif

            for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
            {

                std::string captureImageName = ConstructOutputFileName(cameraId, format, ++frameNumber, ConvertResToString(resolution).c_str());

                NT_ASSERT(Camera3Stream::ProcessCaptureRequest(cameraId, frameNumber, CAMERA3_TEMPLATE_STILL_CAPTURE, 0,
                    true, captureImageName.c_str(), true), "There are no more buffers to send next request, so fail the test");
            }
            FlushStream(cameraId);
            // clear streams
            ClearOutStreams();
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::BasicTriggerSequenceHelper()
    *
    * @brief
    *   Helper to avoid memory leaks in TestBasicTriggerSequence when ASSERT statements are used
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::BasicTriggerSequenceHelper(int cameraId)
    {
        int frameNumber = 1;

        for (int j = 0; j < static_cast<int>(afCount); j++)
        {
            if (afModes[j] == ANDROID_CONTROL_AF_MODE_OFF ||
                afModes[j] == ANDROID_CONTROL_AF_MODE_EDOF)
            {
                // Only test AF modes that have meaningful trigger behavior
                continue;
            }
            for (int k = 0; k < static_cast<int>(aeCount); k++)
            {
                if (aeModes[k] == ANDROID_CONTROL_AE_MODE_OFF)
                {
                    // Only test AE modes that have meaningful trigger behavior
                    continue;
                }
                NT_LOG_INFO("Testing AF mode: %d and AE mode: %d", unsigned(afModes[j]), unsigned(aeModes[k]));

                // Create hal request
                InitHalRequest(cameraId, frameNumber, nullptr, nullptr, 1, CAMERA3_TEMPLATE_PREVIEW);
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");

                SetKey(cameraId, ANDROID_CONTROL_AF_MODE, &afModes[j], 1);
                SetKey(cameraId, ANDROID_CONTROL_AE_MODE, &aeModes[k], 1);

                CancelTriggersAndWait(cameraId, afModes[j], frameNumber);//non fatal

                TriggerAfandVerify(cameraId, afModes[j], frameNumber);//non fatal

                // Standard sequence - Part 2 AE trigger
                TriggerAeandVerify(cameraId, frameNumber);//non fatal

                uint8_t aeState;
                for (int l = 1; l <= MAX_RESULT_STATE_POSTCHANGE_WAIT_FRAMES; l++)
                {

                    GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                    NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
                    NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
                        "Start Streaming Failed ! ");
                    const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
                    NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");
                    aeState = GetAeStateFromMeta(metaFromFile);

                    if (metaFromFile != nullptr)
                    {
                        free(const_cast<camera_metadata_t*>(metaFromFile));
                    }

                    NT_ASSERT(aeState != ANDROID_CONTROL_AE_STATE_PRECAPTURE, "Late transition to PRECAPTURE state seen \n");
                }
                DeleteHalRequest(cameraId);
            }
        }
    }

    /**************************************************************************************************************************
    * RobustnessTest::SimultaneousTriggersHelper()
    *
    * @brief
    *   Helper to avoid memory leaks in TestSimultaneousTriggers when ASSERT statements are used
    * @return
    *   void
    **************************************************************************************************************************/
    void RobustnessTest::SimultaneousTriggersHelper(int cameraId)
    {
        int frameNumber = 1;

        for (int j = 0; j < static_cast<int>(afCount); j++)
        {
            if (afModes[j] == ANDROID_CONTROL_AF_MODE_OFF ||
                afModes[j] == ANDROID_CONTROL_AF_MODE_EDOF)
            {
                // Only test AF modes that have meaningful trigger behavior
                continue;
            }
            for (int k = 0; k < static_cast<int>(aeCount); k++)
            {
                if (aeModes[k] == ANDROID_CONTROL_AE_MODE_OFF)
                {
                    // Only test AE modes that have meaningful trigger behavior
                    continue;
                }
                NT_LOG_INFO("Testing AF mode: %d and AE mode: %d", unsigned(afModes[j]), unsigned(aeModes[k]));

                // Create hal request
                InitHalRequest(cameraId, frameNumber, nullptr, nullptr, 1, CAMERA3_TEMPLATE_PREVIEW);
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");

                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_MODE, &afModes[j], 1);
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_MODE, &aeModes[k], 1);

                CancelTriggersAndWait(cameraId, afModes[j], frameNumber);//non fatal

                TriggerAfAeandVerify(cameraId, afModes[j], frameNumber);//non fatal

                DeleteHalRequest(cameraId);
            }
        }
    }

    void RobustnessTest::TriggerAfAeandVerify(int cameraId, uint8_t afMode, int& frameNumber)
    {
        uint8_t afTrigger = ANDROID_CONTROL_AF_TRIGGER_START;
        uint8_t aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START;
        uint8_t afState, aeState;

        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
        NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
            "Start Streaming Failed ! ");
        // Return both triggers to IDLE (only send TRIGGER_START request once)
        afTrigger = ANDROID_CONTROL_AF_TRIGGER_IDLE;
        aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE;

        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
        NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null");

        afState = GetAfStateFromMeta(metaFromFile);
        aeState = GetAeStateFromMeta(metaFromFile);

        if (metaFromFile != nullptr)
        {
            free(const_cast<camera_metadata_t*>(metaFromFile));
        }

        bool precaptureComplete = false;
        bool focusComplete = false;

        for (int i = 1; i <= MAX_TRIGGER_SEQUENCE_FRAMES && !(focusComplete && precaptureComplete); i++)
        {
            VerifyAfSequence(afMode, afState, focusComplete);
            VerifyAeSequence(aeState, precaptureComplete);

            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
                "Start Streaming Failed ! ");
            metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null");

            afState = GetAfStateFromMeta(metaFromFile);
            aeState = GetAeStateFromMeta(metaFromFile);

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
        }

        NT_ASSERT(precaptureComplete, "Precapture sequence never completed!\n ");
        NT_ASSERT(focusComplete, "Focus sequence never completed! \n");
    }

    void RobustnessTest::TriggerAfandVerify(int cameraId, uint8_t afMode, int& frameNumber)
    {
        uint8_t afState;
        bool focusComplete = false;
        uint8_t afTrigger = ANDROID_CONTROL_AF_TRIGGER_START;
        uint8_t aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE;

        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
        NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
            "Start Streaming Failed ! ");
        // Return AF_TRIGGER to IDLE (only send TRIGGER_START request once)
        afTrigger = ANDROID_CONTROL_AF_TRIGGER_IDLE;
        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);

        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
        NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null");
        afState = GetAfStateFromMeta(metaFromFile);

        if (metaFromFile != nullptr)
        {
            free(const_cast<camera_metadata_t*>(metaFromFile));
        }

        for (int i = 1; i <= MAX_TRIGGER_SEQUENCE_FRAMES && !focusComplete; i++)
        {
            VerifyAfSequence(afMode, afState, focusComplete);

            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
                "Start Streaming Failed ! ");
            metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");
            afState = GetAfStateFromMeta(metaFromFile);

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
        }

        NT_ASSERT(focusComplete, "Focussing never completed! \n");
    }

    void RobustnessTest::VerifyAfSequence(uint8_t afMode, uint8_t afState, bool& focusComplete)
    {
        if (focusComplete)
        {
            NT_ASSERT(afState == ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED ||
                afState == ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED,
                "AF Mode: %d  Focus lock lost after convergence: AF state:%d ", afMode, afState);
            return;
        }
        switch (afMode)
        {
        case ANDROID_CONTROL_AF_MODE_AUTO:
        case ANDROID_CONTROL_AF_MODE_MACRO:
            focusComplete = (afState != ANDROID_CONTROL_AF_STATE_ACTIVE_SCAN);
            break;
        case ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE:
            focusComplete = (afState != ANDROID_CONTROL_AF_STATE_PASSIVE_SCAN);
            break;
        case ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO:
            focusComplete = true;
            break;
        default:
            NT_ASSERT(1 == 0, "Unexpected AF Mode: ", afMode);
        }
    }

    void RobustnessTest::TriggerAeandVerify(int cameraId, int& frameNumber)
    {

        uint8_t aeState;
        bool precaptureComplete = false;
        uint8_t afTrigger = ANDROID_CONTROL_AF_TRIGGER_IDLE;
        uint8_t aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START;

        SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
        SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
        NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
            "Start Streaming Failed ! ");
        // Return AE_PRECAPTURE_TRIGGER to IDLE (only send TRIGGER_START request once)
        aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE;
        SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
        NT_ASSERT(metaFromFile != nullptr,"Meta read from file is null \n");
        aeState = GetAeStateFromMeta(metaFromFile);

        if (metaFromFile != nullptr)
        {
            free(const_cast<camera_metadata_t*>(metaFromFile));
        }

        for (int i = 1; i <= MAX_TRIGGER_SEQUENCE_FRAMES && !precaptureComplete; i++)
        {
            VerifyAeSequence(aeState, precaptureComplete);

            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
                "Start Streaming Failed ! ");
            metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");
            aeState = GetAeStateFromMeta(metaFromFile);

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
        }

        NT_ASSERT(precaptureComplete, "Precapture sequence never completed.");
    }

    void RobustnessTest::VerifyAeSequence(uint8_t aeState, bool& precaptureComplete)
    {
        if (precaptureComplete)
        {
            NT_ASSERT(aeState != ANDROID_CONTROL_AE_STATE_PRECAPTURE, "Precapture seen after convergence \n");
            return;
        }
        switch (aeState)
        {
        case ANDROID_CONTROL_AE_STATE_PRECAPTURE:
            //scan still continuing
            break;
        case ANDROID_CONTROL_AE_STATE_CONVERGED:
        case ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED:
            //completed
            precaptureComplete = true;
            break;
        default:
            NT_ASSERT(1 == 0, "Precapture sequence transistioned to AE state: %d  incorrectly! \n", static_cast<int>(aeState));
            break;
        }
    }

    bool RobustnessTest::PrepareTriggerTestSession(
        int cameraId)
    {
        // Create one output stream
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            HD,
            0,
            HAL_PIXEL_FORMAT_YCbCr_420_888,  //0x23 from graphics.h
            CAMERA3_STREAM_ROTATION_0);

        if (Camera3Stream::ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) != 0)
        {
            NT_EXPECT(1 == 0, "configure streams failed");
            return false;
        }

        return true;
    }


    void RobustnessTest::CancelTriggersAndWait(int cameraId, uint8_t afMode, int& frameNumber)
    {
        uint8_t afTrigger = ANDROID_CONTROL_AF_TRIGGER_CANCEL;
        uint8_t aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_CANCEL;

        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        uint8_t afState = 0;
        uint8_t aeState = 0;

        GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
        NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
            "Start Streaming Failed ! ");
        // Return triggers to IDLE (only send TRIGGER_CANCEL request once)
        afTrigger = ANDROID_CONTROL_AF_TRIGGER_IDLE;
        aeTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE;

        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
        Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER, &aeTrigger, 1);

        for (int i = 0; i < PREVIEW_WARMUP_FRAMES; i++)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr),
                "Start Streaming Failed ! ");
            const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");

            afState = GetAfStateFromMeta(metaFromFile);
            aeState = GetAeStateFromMeta(metaFromFile);

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
        }

        // Verify starting states
        switch (afMode)
        {
        case ANDROID_CONTROL_AF_MODE_AUTO:
        case ANDROID_CONTROL_AF_MODE_MACRO:
            NT_ASSERT(afState == ANDROID_CONTROL_AF_STATE_INACTIVE, "AF state not INACTIVE, is: %d",static_cast<int>(afState));
            break;
        case ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE:
        case ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO:
            // After several frames, AF must no longer be in INACTIVE state
            NT_ASSERT(afState == ANDROID_CONTROL_AF_STATE_PASSIVE_SCAN ||
                afState == ANDROID_CONTROL_AF_STATE_PASSIVE_FOCUSED ||
                afState == ANDROID_CONTROL_AF_STATE_PASSIVE_UNFOCUSED,
                "In AF mode: %d , AF state not PASSIVE_SCAN, PASSIVE_FOCUSED, or PASSIVE_UNFOCUSED, is: %d",static_cast<int>(afMode), static_cast<int>(afState));
            break;
        default:
            NT_ASSERT(1 == 0, "Unexpected AF mode");
            break;
        }

        // After several frames, AE must no longer be in INACTIVE state
        NT_ASSERT(aeState == ANDROID_CONTROL_AE_STATE_SEARCHING ||
            aeState == ANDROID_CONTROL_AE_STATE_CONVERGED ||
            aeState == ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED,
            "AE state must be SEARCHING, CONVERGED, or FLASH_REQUIRED, but it is: %d" ,static_cast<int>(aeState));
    }
}
