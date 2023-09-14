//******************************************************************************
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "flush_test.h"
#include "recording_test.h"
#include "vendor_tags_test.h"


namespace haltests
{

    /**************************************************************************************************************************
    * FlushTest::TestFlushSnapshot()
    *
    * @brief
    *   Tests flush with ZSL preview doing a flush every 7 frames.
    *
    * @return
    *   void
    *************************************************************************************************************************/
    void FlushTest::TestFlushSnapshot(
        int captureFormat,      //[in] capture format
        Size captureResolution, //[in] capture resolution
        const char* testName,   //[in] test name
        int captureCount,       //[in] number of frames to capture
        SpecialSetting setting, //[in] special setting to apply
        CaptureType captureType)        //[in] type of capture to dump - snapshot/preview
    {
        std::vector<int> camList = Camera3Module::GetCameraList();
        int outputFormat[] = { captureFormat };
        int numStreams = 1;
        int numOutBufs = numStreams;
        int streamType[] = { CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlag[] = { 0 };

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // Check for format/resolution support
            Size resolution[] = { captureResolution };
            bool matched = true;
            if (!CheckSupportedResolution(cameraId, outputFormat[0], resolution[0]))
            {
                NT_LOG_UNSUPP("Given format: [%s] and resolution: [%s] not supported for cameraId : [%d]",
                    ConvertFormatToString(outputFormat[0]).c_str(), ConvertResToString(resolution[0]).c_str(), cameraId);
                matched = false;
                break;
            }

            // Skip this camera if format/resolution not supported
            if (!matched)
            {
                continue;
            }

            // Open camera and configure streams
            OpenCamera(cameraId);
            CreateMultiStreams(resolution, outputFormat, numStreams, usageFlag, streamType);
            NT_ASSERT(ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) == 0, "configure streams failed");

            // Handle 3A
            int frameNumber = 0;
#ifdef ENABLE3A
            frameNumber = ProcessCaptureRequestWith3AConverged(cameraId, frameNumber, CAMERA3_TEMPLATE_PREVIEW,
                0, false, "");
            NT_ASSERT(frameNumber != -1, "3A cannot converge!");
#endif

            // Create long-lived hal request
            CreateCaptureRequestWithMultBuffers(cameraId, 0, captureType, numOutBufs, ++frameNumber);//non fatal

            // Apply any special settings
            switch (setting)
            {
            case SettingZSL:
                NT_LOG_WARN("This test requires \"overrideDisableZSL=0\" in camxoverridesettings.txt");
                SetEnableZsl(cameraId);
                break;
            case SettingSWMF:
                NT_LOG_WARN("This test requires \"advanceFeatureMask=0x8\" in camxoverridesettings.txt");
                break;
            case SettingMNFR:
            {
                bool isHQModeSupported = SetMFNRMode(cameraId);
                if (!isHQModeSupported)
                {
                    NT_LOG_UNSUPP("High quality noise reduction mode not available on camera ID %d, skipping", cameraId);
                    continue;
                }
                break;
            }
            case SettingSAT:
                NT_LOG_WARN("This test requires \"multiCameraEnable=1\", \"multiCameraSATEnable=1\" and \"multiCamera3ASync=MultiCamera3ASyncQTI\" in camxoverridesettings.txt");
                break;
            case SettingBokeh:
                NT_LOG_WARN("This test requires \"multiCameraEnable=1\" in camxoverridesettings.txt");
                break;
            case SettingBurstshot:
                NT_LOG_WARN("This test requires \"multiCameraEnable=1\", \"multiCameraSATEnable=1\" and \"multiCamera3ASync=MultiCamera3ASyncQTI\" in camxoverridesettings.txt");
                break;
            default:
                NT_LOG_ERROR("Invalid SpecialSetting! (%d)", setting);
                break;
            }

            // Main capture loop
            for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
            {
                // Flush prep
                ClearErrorNotifyFlag(frameNumber);
                bool isFlushFrame = (frameIndex % FLUSH_FRAME_INTERVAL) == 0;   // Flush every 7 frames
                if (isFlushFrame)
                {
                    SetFlushTestFrame();
                }
                else
                {
                    ClearFlushTestFrame();
                }

                // Send hal request

                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                    "Start Streaming Failed ! ");

                // Construct file name and dump snapshot
                std::string captureImageName = ConstructOutputFileName(cameraId, captureFormat, frameNumber, testName);
                DumpBuffer(0, frameNumber, captureImageName.c_str());

                // Get new buffer for next frame
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for frame %d !"
                    ,frameNumber);
            }

            // Cleanup
            DeleteHalRequest(cameraId);
            FlushStream(cameraId);
            ClearOutStreams();
            CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************************************
    * FlushTest::SetEnableZsl()
    *
    * @brief
    *   Prepares the existing HAL request to test ENABLE_ZSL tag. CAPTURE_INTENT must be set to STILL_CAPTURE mode, and then
    *   the ENABLE_ZSL key should be set.
    *
    * @return
    *   void
    **************************************************************************************************************************/
    void FlushTest::SetEnableZsl(
        int cameraId) //[in] camera ID
    {
        camera_metadata_entry_t entry;
        uint8_t setVal;

        // Ensure that capture intent is set to STILL_CAPTURE
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(GetHalRequest(cameraId)->settings),
            ANDROID_CONTROL_CAPTURE_INTENT,
            &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_CAPTURE_INTENT failed");

        if (entry.count == 1 && entry.data.u8[0] == ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE)
        {
            NT_LOG_INFO("CAPTURE_INTENT is STILL_CAPTURE (%d), ENABLE_ZSL is effective", entry.data.u8[0]);
        }
        else
        {
            NT_LOG_WARN("CAPTURE_INTENT is %d, setting to STILL_CAPTURE to ensure that ENABLE_ZSL is effective", entry.data.u8[0]);
            setVal = ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE;
            SetKey(cameraId, ANDROID_CONTROL_CAPTURE_INTENT, &setVal, 1);
        }

        // Set ENABLE_ZSL to true
        setVal = ANDROID_CONTROL_ENABLE_ZSL_TRUE;
        SetKey(cameraId, ANDROID_CONTROL_ENABLE_ZSL, &setVal, 1);
    }

    /***************************************************************************************************
    *   FlushTest::StartHighSpeedStreaming
    *
    *   @brief
    *       Helper function to create output buffers and send HAL requests in batches and finally dumps
    *       the image buffers returned back by the camera device.
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::StartHighSpeedStreaming(
        int cameraId,           // [in] cameraId for the current open camera
        int batchSize,          // [in] batch size for simultaneous requests
        int& frameNumber,       // [in] frame number to start with
        std::string testName)   // [in] testcase name for saving image file
    {
        NT_ASSERT(GetHalRequest(cameraId) != nullptr, "No hal request object!");

        Camera3Device::isCaptureResultsReadyArray[cameraId] = false;
        camera3_stream_buffer_t outBuffers[2];

        Camera3Device::nOutputBuffersRemaining[cameraId] = batchSize + 1;
        Camera3Device::nInputBuffersRemaining[cameraId] = (nullptr != GetHalRequest(cameraId)->input_buffer) ? 1 : 0;
        Camera3Device::nPartialResultsRemaining[cameraId] = batchSize * GetPartialResultCount(cameraId);

        for (int frameIndex = 1; frameIndex <= batchSize; frameIndex++)
        {
            // Flush prep
            ClearErrorNotifyFlag(frameNumber);
            bool isFlushFrame = (frameIndex % FLUSH_FRAME_INTERVAL) == 0;   // Flush every 7 frames
            if (isFlushFrame)
            {
                SetFlushTestFrame();
            }
            else
            {
                ClearFlushTestFrame();
            }

            ++frameNumber;
            camera3_stream_buffer_t* buffer;
            // Only 1 request can have 2 buffers (preview + video)
            if (frameIndex == 1)
            {
                buffer = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(buffer != nullptr, "Buffer is Null");
                outBuffers[0] = *buffer;
                buffer = GetBufferForRequest(1, frameNumber);
                NT_ASSERT(buffer != nullptr, "Buffer is Null");
                outBuffers[1] = *buffer;
                GetHalRequest(cameraId)->output_buffers = outBuffers;
                GetHalRequest(cameraId)->num_output_buffers = 2;
            }
            // remaining requests with just video buffers
            else
            {
                buffer = GetBufferForRequest(1, frameNumber);
                NT_ASSERT(buffer != nullptr, "Buffer is Null");
                outBuffers[1] = *buffer;
                GetHalRequest(cameraId)->output_buffers = &outBuffers[1];
                GetHalRequest(cameraId)->num_output_buffers = 1;
            }

            GetHalRequest(cameraId)->frame_number = frameNumber;
            NT_LOG_DEBUG("Sending process capture request to cameraId %d , frameNumber %d ", cameraId, frameNumber);

            /* START critical section - hal request */
            std::unique_lock<std::mutex> halRequestLock(Camera3Device::halRequestMutex);
            camera3_device_t* pCamDev = GetCamDevice(cameraId);
            NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
            int res = pCamDev->ops->process_capture_request(pCamDev, GetHalRequest(cameraId));
            GetHalRequest(cameraId)->frame_number = frameNumber;
            NT_ASSERT(res == 0, "process_capture_request failed");

            halRequestLock.unlock();
            /* END critical section - hal request */

            if (IsFlushTestFrame())
            {
                int flushRes = TestFlush(cameraId);
                NT_ASSERT(flushRes == 0, "flush request failed");
            }
        }

        /* START critical section - result ready */
        std::unique_lock<std::mutex> resultReadyLock(Camera3Device::resultReadyMutexArray[cameraId]);

        int retries = 0;
        while (!isCaptureResultsReadyArray[cameraId] && retries < MAX_WAIT_RETRIES)
        {
            Camera3Device::resultReadyCondArray[cameraId].wait_for(resultReadyLock, std::chrono::seconds(HalCmdLineParser::g_timeout));
            NT_LOG_DEBUG("Thread woke up for camera: %d, is capture result ready?: %s", cameraId,
                isCaptureResultsReadyArray[cameraId] ? "YES" : "NO");
            retries++;
        }

        bool triggerFlush = false;
        // If no buffer received after MAX_WAIT_RETRIES, set flag to flush stream later to return buffers in progress
        if (!isCaptureResultsReadyArray[cameraId])
        {
            triggerFlush = true;
            NT_LOG_ERROR( "Failed to obtain output buffer(s)/metadata(s) after several retries for camera:"
                " %d\n", cameraId);
        }
        else
        {
            // If this was a flushed frame...
            if (IsFlushTestFrame())
            {
                // Verify that buffer status was bad
                NT_EXPECT(CAMERA3_BUFFER_STATUS_ERROR == mOutputBufferStatus[FRAME_REQUEST_MOD(frameNumber)],
                   "Buffer status of flushed frame should be CAMERA3_BUFFER_STATUS_ERROR (%d ), but got %d instead !", CAMERA3_BUFFER_STATUS_ERROR
                    , mOutputBufferStatus[FRAME_REQUEST_MOD(frameNumber)]);

                // Verify that buffer error or request error notify occurred
                NT_EXPECT(CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_REQUEST)
                    || CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_BUFFER), "Notify from flush for frame %d did not occur!"
                    ,frameNumber);
            }
        }

        NT_ASSERT(isCaptureResultsReadyArray[cameraId], "Failed to obtain output buffer(s)/metadata(s) after several retires");
        Camera3Device::isCaptureResultsReadyArray[cameraId] = false;
        resultReadyLock.unlock();

        if (triggerFlush)
        {
            FlushStream(cameraId);
            NT_FAIL("Capture results did not return before timeout! Flush was triggered.");
        }

        int startFrame = frameNumber - batchSize;

        std::string prvImage(testName);
        prvImage.append("_Prv_YUV420");
        std::string vidImage(testName);
        vidImage.append("_Video_YUV420");
        std::string finalFileName;

        for (int startIndex = 0; startIndex < batchSize; startIndex++)
        {

            startFrame++;

            if (startIndex == 0)
            {
                finalFileName = ConstructOutputFileName(cameraId, GetConfigedStreams().at(0)->format, startFrame, prvImage.c_str());
                Camera3Stream::DumpBuffer(0, startFrame, finalFileName.c_str());
                prvImage.clear();
                finalFileName.clear();
            }

            finalFileName = ConstructOutputFileName(cameraId, GetConfigedStreams().at(1)->format, startFrame, vidImage.c_str());
            Camera3Stream::DumpBuffer(1, startFrame, finalFileName.c_str());
            finalFileName.clear();

        }
    }

    /***************************************************************************************************
    *   FlushTest::VerifyFinalTargetFPS
    *
    *   @brief
    *       Helper function to read final metadata for target fps range
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::VerifyFinalTargetFPS(
        int32_t * fpsRange,  //[in] array containing fps range set
        int cameraId)        //[in] cameraId to verify
    {
        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
        NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null");

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            ANDROID_CONTROL_AE_TARGET_FPS_RANGE, &entry);

        if (ret == 0 && entry.count == 2)
        {
            NT_LOG_INFO("FPS MIN in capture result: %d FPS MIN value requested: %d",
                entry.data.i32[0], fpsRange[0]);
            NT_LOG_INFO("FPS MAX in capture result: %d FPS MAX value requested: %d",
                entry.data.i32[1], fpsRange[1]);
        }
        else
        {
            NT_EXPECT(1 == 0, "Failed to find ANDROID_CONTROL_AE_TARGET_FPS_RANGE in result metadata");
        }

        if (metaFromFile != nullptr)
        {
            free(const_cast<camera_metadata_t*>(metaFromFile));
        }
    }

    /***************************************************************************************************
    *   FlushTest::FindMatchingConfig
    *
    *   @brief
    *       Helper function to find matching availableHighSpeedVideoConfigurations for given parameters
    *
    *   @return
    *       index of matching HFR configuration
    ****************************************************************************************************/

    int FlushTest::FindMatchingConfig(
        Size res,                      //[in] resolution for the HFR stream
        int32_t fpsMin,                //[in] minimum fps
        int32_t fpsMax,                //[in] maximum fps
        camera_metadata_entry configs) //[in] metadata entry for HFR configurations
    {
        for (int32_t j = 0; (j + RecordingTest::AHSVC_CONFIG_SIZE - 1) < static_cast<int32_t>(configs.count); j += RecordingTest::AHSVC_CONFIG_SIZE)
        {
            if (res.width == static_cast<uint32_t>(configs.data.i32[j + RecordingTest::AHSVC_OFFSET_WIDTH]) &&
                res.height == static_cast<uint32_t>(configs.data.i32[j + RecordingTest::AHSVC_OFFSET_HEIGHT]) &&
                fpsMin == configs.data.i32[j + RecordingTest::AHSVC_OFFSET_FPS_MIN] &&
                fpsMax == configs.data.i32[j + RecordingTest::AHSVC_OFFSET_FPS_MAX])
            {
                Camera3Stream::SetPreviewFPSforHFR(
                    configs.data.i32[j + RecordingTest::AHSVC_OFFSET_FPS_MIN],
                    configs.data.i32[j + RecordingTest::AHSVC_OFFSET_FPS_MAX]);
                return j;
            }
        }

        return -1;
    }

    /***************************************************************************************************
    *   FlushTest::IsHighSpeedSupported
    *
    *   @brief
    *       Helper function perform standard high speed support checks
    *
    *   @return
    *       bool true if supported, false otherwise
    ****************************************************************************************************/

    bool FlushTest::IsHighSpeedSupported(int cameraId, camera_info info)
    {
        camera_metadata_entry configs;
        int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES,
            &configs);
        if (0 != val)
        {
            NT_LOG_ERROR("Failed to obtain stream configuartion for camera id = %d", cameraId);
            return false;
        }

        bool highSpeedSupported = false;
        for (uint32_t j = 0; j < configs.count; j++)
        {
            if (configs.data.u8[j] == ANDROID_REQUEST_AVAILABLE_CAPABILITIES_CONSTRAINED_HIGH_SPEED_VIDEO)
            {
                highSpeedSupported = true;
                break;
            }
        }

        if (!highSpeedSupported)
        {
            NT_LOG_UNSUPP("CONSTRAINED_HIGH_SPEED_VIDEO not supported in camera: %d", cameraId);
            Camera3Device::CloseCamera(cameraId);
            return false;
        }

        return true;
    }

    /***************************************************************************************************
    *   RecordingTest::TestFlushRecordingHFR
    *
    *   @brief
    *       Tests high speed recording with supported configurations on both bayer1 and bayer2, with flush operation every 7th frame
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::TestFlushRecordingHFR(
        Size resolution,    //[in] resolution of the video
        int32_t fpsMin,     //[in] min fps for recording
        int32_t fpsMax,     //[in] max fps for recording
        int captureCount)   //[in] capture count for HFR frames
    {
        int outputFormats[] = { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888 };
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, FLAGS_VIDEO_ENCODER };
        int32_t fpsRange[] = { fpsMin, fpsMax };
        Size resolutions[] = { resolution, resolution };
        int frameNumber = 0;

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // Open camera
            Camera3Device::OpenCamera(cameraId);
            camera_info info;
            if (0 != GetCamModule()->get_camera_info(cameraId, &info))
            {
                NT_ASSERT(1 == 0, "Can't get camera info for camera id = %d \n", cameraId);
            }

            NT_ASSERT(IsHighSpeedSupported(cameraId, info), "High speed support checks failed for camera %d !", cameraId);

            // Verify that requested fps configuration is supported
            camera_metadata_entry configs;
            int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS,
                &configs);
            NT_ASSERT(val == 0, "Failed to obtain available High Speed Video Configurations \n");

            int matchIndex = FindMatchingConfig(resolution, fpsMin, fpsMax, configs);
            if (matchIndex == -1)
            {
                NT_LOG_UNSUPP("Unsupported arguments provided for HFR on camera %d, width: %d, height: %d, "
                    "fpsMin: %d, fspMax: %d", cameraId, resolution.width, resolution.height, fpsMin, fpsMax);
                Camera3Device::CloseCamera(cameraId);
                continue;
            }

            // Get batch size for requested configuration
            int32_t batchSize = configs.data.i32[matchIndex + RecordingTest::AHSVC_OFFSET_BATCH_SIZE];
            NT_ASSERT(batchSize<= GetPipelineDepth(), "PipelineDepth not supported for current HFR batch size %d ",batchSize);

            // Perform test
            NT_LOG_INFO("Testing HFR on camera %d, width: %d, height: %d, fpsMin: %d, fpsMax: %d, batchSize: %d",
                cameraId, resolution.width, resolution.height, fpsMin, fpsMax, batchSize);

            SetHighSpeedSession();
            Camera3Stream::CreateMultiStreams(resolutions, outputFormats, 2, usageFlags, streamTypes);

            NT_ASSERT(Camera3Stream::ConfigStream(
                (IsHighSpeedSession() ?
                    CAMERA3_STREAM_CONFIGURATION_CONSTRAINED_HIGH_SPEED_MODE :
                    CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE),
                cameraId) == 0, "Configure streams failed");

            std::ostringstream testName;
            testName << "TestHFRRecording_" << ConvertResToString(resolution).c_str() << "_FPS"<< fpsRange[0] << "-" << fpsRange[1];

#if !defined(DISABLE3A)
            frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
                cameraId,
                frameNumber,
                CAMERA3_TEMPLATE_PREVIEW, // requestType,
                0, // output stream index
                false,
                "");
            if (frameNumber == -1)
            {
                NT_EXPECT(1 == 0, "3A did not converge");
                frameNumber = MAX_REPEAT_NUM;
            }
#endif //_LINUX

            // Create hal request
            InitHalRequest(cameraId, frameNumber, nullptr, nullptr, 0, CAMERA3_TEMPLATE_VIDEO_RECORD);
            Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_TARGET_FPS_RANGE, fpsRange, 2);

            /*
            * Since HFR frames are captured in batches, we need to divide the total capture counts
            * by batch size to obtain requested count
            * e.g., if 20 frames are requested, then 20/4 = 5, hence 5 times we capture 4 frames per request
            * If requested number of frames is not multiple of batch size then we will not capture
            * exact same count
            */
            if (captureCount % batchSize != 0)
            {
                NT_LOG_WARN("Requested frame capture count: [%d] is not a multiple of batch size: [%d],"
                    " Capturing [%d] frames", captureCount, batchSize, (captureCount / batchSize)*batchSize);
            }
            for (int batchIndex = 0; batchIndex < (captureCount / batchSize); batchIndex++)
            {
                StartHighSpeedStreaming(cameraId, batchSize, frameNumber, testName.str());//non fatal
            }

            VerifyFinalTargetFPS(fpsRange, cameraId);

            // Cleanup
            Camera3Stream::DeleteHalRequest(cameraId);
            Camera3Device::ClearMetadataMap(cameraId);
            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::ClearOutStreams();
            ClearHighSpeedSession();

            // Close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * FlushTest::TestFlushRecordingHDR
    *
    * @brief
    *   Test basic video recording with preview and video stream, with flush operation every 7th frame
    * @return
    *   None
    **************************************************************************************************************************/
    void FlushTest::TestFlushRecordingHDR(
        int previewFormat,     //[in] preview format
        int videoFormat,       //[in] video format
        Size previewRes,       //[in] preview resolution
        Size videoRes,         //[in] video resolution
        int captureCount,      //[in] number of frames to capture
        int fps,               //[in] fps to be used
        const char* testName)  //[in] filename to be used
    {
        const int resPreviewIndex = 0;
        const int resVideoIndex = 1;
        NATIVETEST_UNUSED_PARAM(resPreviewIndex);

        Size resolutions[] = { previewRes, videoRes };
        int outputFormats[] = { previewFormat, videoFormat };
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, 0 };
        int32_t fpsRange[] = { fps, fps };
        bool vendorTagsInit = false;
        bool vendorTags = false;
        // Setup and resolve maxPreviewSize vendor tag
        camera_metadata_tag_t maxPreviewTag;
        char tagNam[] = "MaxPreviewSize";
        char sectionNam[] = "org.quic.camera.MaxPreviewSize";

        // Initialize vendor tags if needed
        if (!vendorTags)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
            vendorTags = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagNam, sectionNam);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);

        int numberOfFormats = sizeof(outputFormats) / sizeof(outputFormats[0]);

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            //MaxPreviewSize vendorTag
            camera_metadata_entry_t tagEntry;
            camera_info* pCamInfoMx = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfoMx, "Unable to retrieve camera info for camera Id: %d", cameraId);
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoMx->static_camera_characteristics),
                maxPreviewTag, &tagEntry);
            NT_ASSERT(ret == 0,"find_camera_metadata_entry max_preview_size failed");
            if (previewRes.width*previewRes.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                previewRes.width = tagEntry.data.i32[0];
                previewRes.height = tagEntry.data.i32[1];
                NT_LOG_INFO("OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    previewRes.width, previewRes.height);
            }
            bool matched = true;
            for (int index = 0; index < numberOfFormats; index++)
            {
                if (!CheckSupportedResolution(cameraId, outputFormats[index], resolutions[index]))
                {
                    NT_LOG_UNSUPP("Given format: [%s] and resolution: [%s] not supported for cameraId : [%d]",
                        ConvertFormatToString(outputFormats[index]).c_str(), ConvertResToString(resolutions[index]).c_str(),
                        cameraId);
                    matched = false;
                    break;
                }
            }
            if (!matched)
            {
                continue;
            }

            // Verify requested fps range is supported
            camera_metadata_entry configs;
            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);
            bool isFpsSupported = false;
            camera_metadata_tag_t fpsTag;

            int metadataTagFetchResult = -1;
            // Regular case: aeAvailableTargetFpsRanges (30 fps and below)
            if (fps <= RecordingTest::CUSTOM_HFR_RANGE_LOW)
            {
                fpsTag = ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES;
                metadataTagFetchResult = GetCameraMetadataEntryByTag(
                    const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics), fpsTag, &configs);
                NT_ASSERT(metadataTagFetchResult == 0, "GetCameraMetadataEntryByTag FPS support tag: %d failed",
                    static_cast<int>(fpsTag));
            }
            else if (RecordingTest::CUSTOM_HFR_RANGE_LOW < fps && fps <= RecordingTest::CUSTOM_HFR_RANGE_HIGH)
            {// Vendor tag case: CustomHFRFpsTable (60 and 90 fps)
                // Setup and resolve CustomHFRFpsTable vendor tag
                char tagName[] = "CustomHFRFpsTable";
                char sectionName[] = "org.quic.camera2.customhfrfps.info";

                // Initialize vendor tags if needed
                if (!vendorTagsInit)
                {
                    NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
                    vendorTagsInit = true;
                }

                tagNum = static_cast<camera_metadata_tag_t>(VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName));
                if (tagNum == UINT32_MAX)
                {
                    NT_LOG_UNSUPP("CustomHFRFpsTable vendor tag not available!");
                    return;
                }
                fpsTag = static_cast<camera_metadata_tag_t>(tagNum);
                metadataTagFetchResult = GetCameraMetadataEntryByTag(
                    const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics), fpsTag, &configs);
                NT_ASSERT(metadataTagFetchResult == 0, "GetCameraMetadataEntryByTag FPS support tag: %d failed",
                    static_cast<int>(fpsTag));
            }
            // High speed video case: availableHighSpeedVideoConfigurations (120 and 240 fps)
            // Should not use this function for high speed session. Use TestConstrainedHighSpeedRecording instead.
            else
            {
                NT_ASSERT(metadataTagFetchResult != -1, "Invalid fps configuration ( %d ) for this test function!", fps);
            }

            for (size_t i = 0; i < configs.count; /*increment depends on case*/)
            {
                // Use CustomHFRFpsTable
                if (RecordingTest::CUSTOM_HFR_RANGE_LOW < fps && fps <= RecordingTest::CUSTOM_HFR_RANGE_HIGH)
                {
                    // Look for res that exceeds width and height of requested res, and satifies requested fps
                    if ((i + RecordingTest::CUSTOM_HFR_OFFSET_FPS) < configs.count &&
                        configs.data.i32[i + RecordingTest::CUSTOM_HFR_OFFSET_WIDTH] >= static_cast<int32_t>(resolutions[resVideoIndex].width) &&
                        configs.data.i32[i + RecordingTest::CUSTOM_HFR_OFFSET_HEIGHT] >= static_cast<int32_t>(resolutions[resVideoIndex].height) &&
                        configs.data.i32[i + RecordingTest::CUSTOM_HFR_OFFSET_FPS] == fps)
                    {
                        isFpsSupported = true;
                    }
                    i += RecordingTest::CUSTOM_HFR_CONFIG_SIZE;
                }
                // Use aeAvailableTargetFpsRanges
                else
                {
                    // Look for supported fps range in the form [fps, fps]
                    if ((i + RecordingTest::TARGET_FPS_OFFSET_FPS_MAX) < configs.count &&
                        configs.data.i32[i + RecordingTest::TARGET_FPS_OFFSET_FPS_MIN] == fps &&
                        configs.data.i32[i + RecordingTest::TARGET_FPS_OFFSET_FPS_MAX] == fps)
                    {
                        isFpsSupported = true;
                    }
                    i += RecordingTest::TARGET_FPS_CONFIG_SIZE;
                }
            }

            // Pass with unsupported if camera does not support resolution/fps combination
            if (!isFpsSupported)
            {
                NT_LOG_UNSUPP("Camera %d does not support res and fps combination: %dx%d @ %dfps. Skipping...",
                    cameraId,
                    resolutions[resVideoIndex].width,
                    resolutions[resVideoIndex].height,
                    fps);
                return;
            }

            Camera3Device::OpenCamera(cameraId);
            Camera3Stream::CreateMultiStreams(resolutions, outputFormats, 2, usageFlags, streamTypes);
            int res = Camera3Stream::ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId);
            NT_ASSERT(res == 0, "Config stream failed");

            int frameNumber = 0;

#ifdef ENABLE3A
            frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
                cameraId,
                frameNumber,
                CAMERA3_TEMPLATE_PREVIEW, // requestType,
                0, // output stream index
                false,
                "");
            NT_ASSERT(frameNumber != -1, "3A did not converge");
#endif //_LINUX

            for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
            {
                // Flush prep
                ClearErrorNotifyFlag(frameNumber);
                bool isFlushFrame = (frameIndex % FLUSH_FRAME_INTERVAL) == 0;   // Flush every 7 frames
                if (isFlushFrame)
                {
                    SetFlushTestFrame();
                }
                else
                {
                    ClearFlushTestFrame();
                }

                frameNumber++;
                std::ostringstream prefixPreview;
                std::ostringstream prefixVideo;
                prefixPreview << "Prev_" << testName;
                prefixVideo << "Video_" << testName;
                std::string outputName = ConstructOutputFileName(cameraId, previewFormat, frameNumber,
                    prefixPreview.str().c_str());
                std::string outputName2 = ConstructOutputFileName(cameraId, videoFormat, frameNumber,
                    prefixVideo.str().c_str());
                CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_VIDEO_RECORD,
                    numberOfFormats, frameNumber);//non fatal

                //Update fps
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_TARGET_FPS_RANGE, fpsRange, 2);

                // set HDR mode
                uint8_t modeHDR = ANDROID_CONTROL_SCENE_MODE_HDR;
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_SCENE_MODE, &modeHDR, 1);
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr), "Start Stream Failed!");
                Camera3Stream::DumpBuffer(0, frameNumber, outputName.c_str());
                Camera3Stream::DumpBuffer(1, frameNumber, outputName2.c_str());

                if (!isFlushFrame)
                {
                    VerifyFinalTargetFPS(fpsRange, cameraId);
                }

                Camera3Stream::DeleteHalRequest(cameraId);
            }
            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::ClearOutStreams();
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************
    *   FlushTest::PrepareEISSnapshotRequest()
    *
    *   @brief
    *       Helper function to perform 3A convergence and obtain the frame number
    *       of the converged frame. The function also creates HAL request for
    *       an EIS test using the STILL_CAPTURE template
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::PrepareEISSnapshotRequest(
        int cameraId,           //[in] camera Id
        FlushItem preview,      //[in] preview format and resolution
        FlushItem snapshot,     //[in] snapshot format and resolution
        int &frameNumber,       //[in/out] frame number
        uint32_t opmode)        //[in] operation mode for configuring streams
    {
        // Create preview and snapshot streams
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            preview.resolution,
            0,
            preview.format,
            CAMERA3_STREAM_ROTATION_0);

        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            snapshot.resolution,
            0,
            snapshot.format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(Camera3Stream::ConfigStream(opmode, cameraId) == 0, "configure streams failed");

#if !defined(DISABLE3A)
        frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
            cameraId,
            frameNumber,
            CAMERA3_TEMPLATE_PREVIEW, // requestType,
            0, // output stream index
            false,
            "");

        NT_ASSERT(frameNumber != -1, "3A cannot converge!");
#endif
        camera3_stream_buffer_t* outputBuffer = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(outputBuffer != nullptr, "Failed to get output buffer for request!");

        // create hal request
        InitHalRequest(cameraId, frameNumber++, nullptr, outputBuffer, 1, CAMERA3_TEMPLATE_STILL_CAPTURE);

    }

    /***************************************************************************************************
    *   FlushTest::PrepareEISVideoRequest()
    *
    *   @brief
    *       Helper function to perform 3a convergence and obtain the frame number
    *       of the converged frame. The function also creates HAL request for
    *       an EIS test using the VIDEO_SNAPSHOT template
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::PrepareEISVideoRequest(
        int cameraId,           //[in] camera Id
        FlushItem preview,  //[in] preview format and resolution
        FlushItem video,    //[in] video format and resolution
        FlushItem snapshot, //[in] snapshot format and resolution
        int &frameNumber,       //[in/out] frame number
        uint32_t opmode)        //[in] operation mode for configuring streams
    {
        // Create preview, video, and snapshot streams
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            preview.resolution,
            VendorTagsTest::GrallocUsageHwCameraWrite | VendorTagsTest::GrallocUsageHwTexture,
            preview.format,
            CAMERA3_STREAM_ROTATION_0);

        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            video.resolution,
            VendorTagsTest::GrallocUsageHwVideoEncoder,
            video.format,
            CAMERA3_STREAM_ROTATION_0);

        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            snapshot.resolution,
            0,
            snapshot.format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(Camera3Stream::ConfigStream(opmode, cameraId) == 0, "configure streams failed");

#if !defined(DISABLE3A)
        frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
            cameraId,
            frameNumber,
            CAMERA3_TEMPLATE_PREVIEW, // requestType,
            0, // output stream index
            false,
            "");

        NT_ASSERT(frameNumber != -1, "3A cannot converge!");
#endif
        camera3_stream_buffer_t* outputBuffer = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(outputBuffer != nullptr, "Failed to get output buffer for request!");

        // create hal request
        InitHalRequest(cameraId, frameNumber++, nullptr, outputBuffer, 1, CAMERA3_TEMPLATE_VIDEO_SNAPSHOT);

    }

    /***************************************************************************************************
    *   FlushTest::VerifyEISResult()
    *
    *   @brief
    *       Verifies that electronic image stabilization (EIS) vendor tag took in capture result
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::VerifyEISResult(
        int cameraId,           //[in] camera Id
        int &frameNumber,       //[in/out] frame number
        uint32_t tagId,         //[in] EIS tag Id
        const uint8_t *value,   //[in] value of the EIS tag to verify
        bool isVideoOn)         //[in] is video stream included in verification
    {
        int i = 0;
        int success = 0;
        const camera_metadata_t* metaFromFile;

        while (i <= VendorTagsTest::PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(1, frameNumber);
            if (isVideoOn)
            {
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(2, frameNumber);
            }
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for request!");

            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                "Start Streaming Failed ! ");
            metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");

            camera_metadata_entry_t entry;
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile), tagId, &entry);

            if (ret == 0 && entry.count > 0 && entry.data.u8[0] == *value)
            {
                NT_LOG_INFO("Key in capture result is matches requested value");
                success = 1;
                break;
            }

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
            i++;
        }
        NT_ASSERT(success == 1, "Capture Results does not contain the value set");
    }

    /***************************************************************************************************
    *   FlushTest::TestEIS()
    *
    *   @brief
    *       Tests electronic image stabilization (EIS) vendor tag with three streams: preview, video,
    *       and snapshot for given number of frames. A vendor tag is also used to stop the recording.
    *
    *       eisMode values and their respective usecases:
    *           "EISV2" - EISV2 on preview stream, EISV2 on video stream
    *           "EISV3" - EISV2 on preview stream, EISV3 on video stream
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void FlushTest::TestFlushRecordingEIS(
        FlushItem preview,      //[in] preview format and resolution
        FlushItem video,        //[in] video format and resolution
        FlushItem snapshot,     //[in] snapshot format and resolution
        const char* eisMode,    //[in] determines which EIS usecase to select
        int captureCount,       //[in] number of frames to run the test
        bool isVideoOn)         //[in] turns video stream on or off
    {
        uint32_t eisTag = strncmp(eisMode, "EISV2", 5) == 0 ?
            VendorTagsTest::CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV2 :
            VendorTagsTest::CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3;

        char eisv3TagName[] = "EISV3Enable";
        char eisv3SectionName[] = "org.quic.camera.eis3enable";
        char stopRecordTagName[] = "endOfStream";
        char stopRecordSectionName[] = "org.quic.camera.recording";

        const uint8_t EISV3_OFF = 0;
        const uint8_t EISV3_ON = 1;
        const uint8_t STOP_RECORD_OFF = 0;
        const uint8_t STOP_RECORD_ON = 1;
        // Setup and resolve vendor tag
        bool vendorTags = false;
        camera_metadata_tag_t liveShotSupp;
        char tagName[] = "isLiveshotSizeSameAsVideoSize";
        char sectionName[] = "org.quic.camera.LiveshotSize";

        // Initialize vendor tags if needed
        if (!vendorTags)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
            vendorTags = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("isLiveshotSizeSameAsVideoSize vendor tag not available!");
            return;
        }
        liveShotSupp = static_cast<camera_metadata_tag_t>(tagNum);

        NATIVETEST_UNUSED_PARAM(EISV3_OFF);
        NATIVETEST_UNUSED_PARAM(STOP_RECORD_OFF);

        uint32_t eisv3TagId = UINT32_MAX;
        if (eisTag == VendorTagsTest::CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3)
        {
            // Resolve EISV3 vendor tag
            eisv3TagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, eisv3TagName, eisv3SectionName);
            if (eisv3TagId == UINT32_MAX)
            {
                NT_LOG_UNSUPP("EISV3 vendor tag not available!");
                return;
            }
        }

        // Resolve stop recording vendor tag
        uint32_t stopRecordTagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, stopRecordTagName, stopRecordSectionName);
        if (stopRecordTagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("endOfStream vendor tag not available!");
            return;
        }
        // Setup and resolve maxPreviewSize vendor tag
        camera_metadata_tag_t maxPreviewTag;
        char prTagName[] = "MaxPreviewSize";
        char prSectionName[] = "org.quic.camera.MaxPreviewSize";

        tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, prTagName, prSectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);

        //Setup and resolve VideoConfigurationsTable Vendor Tags
        camera_metadata_tag_t vidConfig;
        char vcTagName[] = "VideoConfigurationsTable";
        char vcSectionName[] = "org.quic.camera2.VideoConfigurations.info";

        tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, vcTagName, vcSectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("VideoConfigurations vendor tag not available!");
            return;
        }
        vidConfig = static_cast<camera_metadata_tag_t>(tagNum);

        std::vector<int> camList = Camera3Module::GetCameraList();
        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];
             //MaxPreviewSize vendorTag
            camera_metadata_entry_t tagEntry;
            camera_info* pCamInfoLv = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfoLv, "Unable to retrieve camera info for camera Id: %d", cameraId);
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoLv->static_camera_characteristics),
                maxPreviewTag, &tagEntry);
            NT_ASSERT(ret == 0,"find_camera_metadata_entry max_preview_size failed");
            if (preview.resolution.width*preview.resolution.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                preview.resolution.width = tagEntry.data.i32[0];
                preview.resolution.height = tagEntry.data.i32[1];
                NT_LOG_INFO("OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    preview.resolution.width, preview.resolution.height);
            }

            //VideoConfig Vendor Tags
            ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoLv->static_camera_characteristics), vidConfig, &tagEntry);
            NT_ASSERT(ret == 0,"find_camera_metadata_entry max_preview_size failed");
            int index = -1;
            for (int i = 0; i<static_cast<int>(tagEntry.count); i++)
            {
                if (i % 6 == 0 && video.resolution.width == static_cast<uint32_t>(tagEntry.data.i32[i]))
                {
                    index = i;
                    break;
                }
            }
            if (index != -1)//Check if resolution match
            {
                if (tagEntry.data.i32[index + 4] == 1)//isLiveShot Check
                {
                    //LiveShotSameVideo
                    camera_metadata_entry_t tagEntryLive;
                    ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoLv->static_camera_characteristics), liveShotSupp, &tagEntryLive);
                    NT_ASSERT(ret == 0, "find_camera_metadata_entry LiveshotSizeSameAsVideoSize failed");
                    if (tagEntry.data.i32[0] == 1)
                    {
                        snapshot = video;
                        NT_LOG_INFO("Live shot set to video resolution!");
                    }
                }
                else
                {
                    NT_LOG_UNSUPP("LiveShot is not supported");
                    return;
                }
                if (tagEntry.data.i32[index + 5] == 0) //isEIS Check
                {
                    NT_LOG_UNSUPP("EIS is not supported");
                    return;
                }
            }
            else
            {
                NT_LOG_UNSUPP("Video Resolution (%d x %d) not supported", video.resolution.width, video.resolution.height);
                return;
            }

            // Check preview stream parameters are supported
            if (!CheckSupportedResolution(cameraId, preview.format, preview.resolution))
            {
                NT_LOG_UNSUPP("Preview stream format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    preview.format, ConvertResToString(preview.resolution).c_str(), cameraId);
                continue;
            }
            // Check video stream parameters are supported
            if (isVideoOn && !CheckSupportedResolution(cameraId, video.format, video.resolution))
            {
                NT_LOG_UNSUPP("Video stream format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    video.format, ConvertResToString(video.resolution).c_str(), cameraId);
                continue;
            }
            // Check snapshot stream parameters are supported
            if (!CheckSupportedResolution(cameraId, snapshot.format, snapshot.resolution))
            {
                NT_LOG_UNSUPP("Snapshot stream format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    snapshot.format, ConvertResToString(snapshot.resolution).c_str(), cameraId);
                continue;
            }

            // Open camera
            OpenCamera(cameraId);
            int frameNumber = 1;
            if (isVideoOn)
            {
                PrepareEISVideoRequest(cameraId, preview, video, snapshot, frameNumber, eisTag);//non fatal
            }
            else
            {
                PrepareEISSnapshotRequest(cameraId, preview, snapshot, frameNumber, eisTag);//non fatal
            }

            // Set EIS metadata tags
            uint8_t eisv2Val = ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_ON;
            SetKey(cameraId, ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &eisv2Val, 1);
            if (eisTag == VendorTagsTest::CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3)
            {
                SetKey(cameraId, eisv3TagId, &EISV3_ON, 1);
            }

            // Start recording and verify the tags are correct in the capture result
            VerifyEISResult(cameraId, frameNumber, ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &eisv2Val, isVideoOn);
            if (eisTag == VendorTagsTest::CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3)
            {
                VerifyEISResult(cameraId, frameNumber, eisv3TagId, &EISV3_ON, isVideoOn);
            }
            NT_LOG_DEBUG("EIS tags were found correctly in the capture result");

            // Continue to run test for number of requested frames
            for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
            {
                // Flush prep
                ClearErrorNotifyFlag(frameNumber);
                bool isFlushFrame = (frameIndex % FLUSH_FRAME_INTERVAL) == 0;   // Flush every 7 frames
                if (isFlushFrame)
                {
                    SetFlushTestFrame();
                }
                else
                {
                    ClearFlushTestFrame();
                }

                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(1, frameNumber);
                if (isVideoOn)
                {
                    GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(2, frameNumber);
                }
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal returned Null");

                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                    "Start Streaming Failed ! ");
            }

            // Dump buffers
            std::ostringstream outputNamePreview;
            std::ostringstream outputNameVideo;
            std::ostringstream outputNameSnapshot;
            outputNamePreview << "EIS_Preview";
            outputNameVideo << "EIS_Video";
            outputNameSnapshot << "EIS_Snapshot";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, preview.format, -1, outputNamePreview.str().c_str()).c_str());
            if (isVideoOn)
            {
                DumpBuffer(1, frameNumber, ConstructOutputFileName(cameraId, video.format, -1, outputNameVideo.str().c_str()).c_str());
                DumpBuffer(2, frameNumber, ConstructOutputFileName(cameraId, snapshot.format, -1, outputNameSnapshot.str().c_str()).c_str());
            }
            else
            {
                DumpBuffer(1, frameNumber, ConstructOutputFileName(cameraId, snapshot.format, -1, outputNameSnapshot.str().c_str()).c_str());
            }

            // Stop recording (send stop recording vendor tag)
            Camera3Stream::SetKey(cameraId, stopRecordTagId, &STOP_RECORD_ON, 1);
            VerifyEISResult(cameraId, frameNumber, stopRecordTagId, &STOP_RECORD_ON, isVideoOn);
            NT_LOG_DEBUG("endOfStream tag found in capture result");

            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::DeleteHalRequest(cameraId);

            // clear streams
            Camera3Stream::ClearOutStreams();

            // close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }
}
