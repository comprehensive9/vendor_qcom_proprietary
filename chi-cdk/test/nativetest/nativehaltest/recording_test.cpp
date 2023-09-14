//*************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#include "recording_test.h"
#include "vendor_tags_test.h"

#include <sstream>

/*
* Use Case:
*     TODO: Add remaining use cases and their descriptions for the recording test suite
*     5) HFR recording
*
* Desc:
*     Tests High speed recording with supported configurations on both bayer1 and bayer2
*
* Parameters:
*     None
*
* Expected Output:
*     5 yuv image files (1 preview frame and 4 video frames) for each supported configuartion
*
* Error Output:
*     Test fails if the callback does not provide the metadata/image buffers
*
* Full Description:
*     The test opens the camera and queries the available high speed configurations, then
*     for each confiuration [width, height, fps_min, fps_max, batchsize] two stream are configured
*     in high speed mode. 3A convergence is performed using just preview stream and after convergence
*     high speed recording is started using video record template and setting the ANDROID_CONTROL_AE_TARGET_FPS_RANGE
*     using fps_min and fps_max. Batchsize determines number of requests to be sent down simultaneously without
*     waiting for results. Finally images are saved and also the result metadata is checked to see if the target
*     fps range is within the range set by the application.
*
*/


namespace haltests
{

#define FLAGS_VIDEO_ENCODER 0x00010000

    // Declare static variables
    const int64_t RecordingTest::CAPTURE_RESULT_TIMEOUT;

    RecordingTest::RecordingTest()
    {
    }

    RecordingTest::~RecordingTest()
    {}

    void RecordingTest::Setup()
    {
        Camera3Stream::Setup();
    }

    /**************************************************************************************************************************
    * RecordingTest::TestBasicVideoRecording
    *
    * @brief
    *   Test basic video recording with preview and video stream
    * @return
    *   None
    **************************************************************************************************************************/
    void RecordingTest::TestBasicVideoRecording(
        int previewFormat,     //[in] preview format
        int videoFormat,       //[in] video format
        Size previewRes,       //[in] preview resolution
        Size videoRes,         //[in] video resolution
        int frameCount,        //[in] number of frames to capture
        int fps,               //[in] fps to be used
        const char* testName)  //[in] filename to be used
    {
        const int resPreviewIndex = 0;
        const int resVideoIndex   = 1;
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
        char tagName[] = "MaxPreviewSize";
        char sectionName[] = "org.quic.camera.MaxPreviewSize";

        // Initialize vendor tags if needed
        if (!vendorTags)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0,"Vendor tags could not be initialized!");
            vendorTags = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);
        int numberOfFormats = sizeof(outputFormats) / sizeof(outputFormats[0]);

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()),"No cameras found");

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
            NT_ASSERT(ret == 0, "find_camera_metadata_entry max_preview_size failed");
            if (previewRes.width*previewRes.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                previewRes.width = tagEntry.data.i32[0];
                previewRes.height = tagEntry.data.i32[1];
                NT_LOG_UNSUPP("OVERRIDE: Reseting preview to max preview size supported %d x %d",
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

            // Regular case: aeAvailableTargetFpsRanges (30 fps and below)
            if (fps <= CUSTOM_HFR_RANGE_LOW)
            {
                fpsTag = ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES;
            }
            // Vendor tag case: CustomHFRFpsTable (60 and 90 fps)
            else if (CUSTOM_HFR_RANGE_LOW < fps && fps <= CUSTOM_HFR_RANGE_HIGH)
            {
                // Setup and resolve CustomHFRFpsTable vendor tag
                char tagName[] = "CustomHFRFpsTable";
                char sectionName[] = "org.quic.camera2.customhfrfps.info";

                fpsTag = static_cast<camera_metadata_tag_t>(VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName));
                if (tagNum == UINT32_MAX)
                {
                    NT_LOG_UNSUPP("CustomHFRFpsTable vendor tag not available!");
                    return;
                }
                fpsTag = static_cast<camera_metadata_tag_t>(tagNum);
            }
            // High speed video case: availableHighSpeedVideoConfigurations (120 and 240 fps)
            // Should not use this function for high speed session. Use TestConstrainedHighSpeedRecording instead.
            else
            {
                NT_LOG_ERROR("Invalid fps configuration (%d) for this test function!", fps);
                return;
            }

            int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics),
                fpsTag,
                &configs);
            NT_ASSERT(val == 0, "GetCameraMetadataEntryByTag FPS support tag: %d failed", static_cast<int>(fpsTag));

            for (size_t i = 0; i < configs.count; /*increment depends on case*/)
            {
                // Use CustomHFRFpsTable
                if (CUSTOM_HFR_RANGE_LOW < fps && fps <= CUSTOM_HFR_RANGE_HIGH)
                {
                    // Look for res that exceeds width and height of requested res, and satifies requested fps
                    if ((i + CUSTOM_HFR_OFFSET_FPS) < configs.count &&
                        configs.data.i32[i + CUSTOM_HFR_OFFSET_WIDTH]  >= static_cast<int32_t>(resolutions[resVideoIndex].width) &&
                        configs.data.i32[i + CUSTOM_HFR_OFFSET_HEIGHT] >= static_cast<int32_t>(resolutions[resVideoIndex].height) &&
                        configs.data.i32[i + CUSTOM_HFR_OFFSET_FPS]    == fps)
                    {
                        isFpsSupported = true;
                    }
                    i += CUSTOM_HFR_CONFIG_SIZE;
                }
                // Use aeAvailableTargetFpsRanges
                else
                {
                    // Look for supported fps range in the form [fps, fps]
                    if ((i + TARGET_FPS_OFFSET_FPS_MAX) < configs.count &&
                        configs.data.i32[i + TARGET_FPS_OFFSET_FPS_MIN] == fps &&
                        configs.data.i32[i + TARGET_FPS_OFFSET_FPS_MAX] == fps)
                    {
                        isFpsSupported = true;
                    }
                    i += TARGET_FPS_CONFIG_SIZE;
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
            NT_ASSERT(res == 0,"Config stream failed");

            int frameNumber = 0;

#ifdef ENABLE3A
            frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
                cameraId,
                frameNumber,
                CAMERA3_TEMPLATE_PREVIEW, // requestType,
                0, // output stream index
                false,
                "");
            NT_ASSERT(frameNumber != -1,"3A did not converge");
#endif //_LINUX

            for (int frameIndex = 1; frameIndex <= frameCount; frameIndex++)
            {
                frameNumber++;
                std::ostringstream prefixPreview;
                std::ostringstream prefixVideo;
                prefixPreview << "Prev_" << testName;
                prefixVideo   << "Video_" << testName;
                std::string outputName  = ConstructOutputFileName(cameraId, previewFormat, frameNumber,
                    prefixPreview.str().c_str());
                std::string outputName2 = ConstructOutputFileName(cameraId, videoFormat, frameNumber,
                    prefixVideo.str().c_str());
                CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_VIDEO_RECORD,
                    numberOfFormats, frameNumber);//non fatal

                //Update fps
                Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_AE_TARGET_FPS_RANGE, fpsRange, 2);

                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr), "Start Streaming Failed");
                Camera3Stream::DumpBuffer(0, frameNumber, outputName.c_str());
                Camera3Stream::DumpBuffer(1, frameNumber, outputName2.c_str());

                VerifyFinalTargetFPS(fpsRange, cameraId);

                Camera3Stream::DeleteHalRequest(cameraId);
            }
            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::ClearOutStreams();
            Camera3Device::CloseCamera(cameraId);
        }

    }

    /**************************************************************************************************************************
    * RecordingTest::TestBasicVideoLiveSnapshot
    *
    * @brief
    *   Test basic video live snapshot with preview, video stream, snapshot stream
    *   Test runs requested frames of preview and video and takes a snapshot every n/2 frames
    * @return
    *   None
    **************************************************************************************************************************/
    void RecordingTest::TestBasicVideoLiveSnapshot(
        int previewFormat,     //[in] preview format
        int videoFormat,       //[in] video format
        int snapFormat,        //[in] snapshot format
        Size previewRes,       //[in] preview resolution
        Size videoRes,         //[in] video resolution
        Size snapshotRes,      //[in] snapshot resolution
        int captureCount,      //[in] capture count
        const char* testName)  //[in] filename for the capture
    {
        int outputFormats[] = { previewFormat, videoFormat, snapFormat };
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, 0, 0 };
        int numberOfFormats = sizeof(outputFormats) / sizeof(outputFormats[0]);
        int interval = captureCount / 2;
        interval = (interval == 0) ? 1 : interval;
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");
         // Setup and resolve vendor tag
        bool vendorTags = false;
        camera_metadata_tag_t liveShotSupp;
        char tagName[] = "isLiveshotSizeSameAsVideoSize";
        char sectionName[] = "org.quic.camera.LiveshotSize";

        // Setup and resolve maxPreviewSize vendor tag
        camera_metadata_tag_t maxPreviewTag;
        char prTagName[] = "MaxPreviewSize";
        char prSectionName[] = "org.quic.camera.MaxPreviewSize";

        // Setup and resolve VideoConfig vendor tag
        camera_metadata_tag_t vidConfig;
        char vcTagName[] = "VideoConfigurationsTable";
        char vcSectionName[] = "org.quic.camera2.VideoConfigurations.info";

        // Initialize vendor tags if needed
        if (!vendorTags)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0,"Vendor tags could not be initialized!");
            vendorTags = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("isLiveshotSizeSameAsVideoSize vendor tag not available!");
            return;
        }
        liveShotSupp = static_cast<camera_metadata_tag_t>(tagNum);

        tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, prTagName, prSectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);

        tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, vcTagName, vcSectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("VideoConfigurations vendor tag not available!");
            return;
        }
        vidConfig = static_cast<camera_metadata_tag_t>(tagNum);

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
            if (previewRes.width*previewRes.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                previewRes.width = tagEntry.data.i32[0];
                previewRes.height = tagEntry.data.i32[1];
                NT_LOG_INFO("OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    previewRes.width, previewRes.height);
            }

            //VideoConfig Vendor Tags
            ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoLv->static_camera_characteristics), vidConfig, &tagEntry);
            NT_ASSERT(ret == 0, "find_camera_metadata_entry max_preview_size failed");
            int index = -1;
            for (int i = 0; i<static_cast<int>(tagEntry.count); i++)
            {
                if (i % 6 == 0 && videoRes.width == static_cast<uint32_t>(tagEntry.data.i32[i]))
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
                    if (tagEntryLive.data.i32[0] == 1)
                    {
                        snapshotRes = videoRes;
                        NT_LOG_INFO("Live shot set to video resolution!");
                    }
                }
                else
                {
                    NT_LOG_UNSUPP("LiveShot is not supported");
                    return;
                }
            }
            else
            {
                NT_LOG_UNSUPP("Video Resolution (%d x %d) not supported", videoRes.width, videoRes.height);
                return;
            }

            // Overwrite resolution for RAW usecase to MAX
            if (snapFormat == HAL_PIXEL_FORMAT_RAW16 || snapFormat == HAL_PIXEL_FORMAT_RAW10)
            {
                camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
                NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);
                snapshotRes = GetMaxSize(snapFormat, pCamInfo);
                if (snapshotRes.width == 0 && snapshotRes.height == 0)
                {
                    NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        snapFormat, ConvertResToString(snapshotRes).c_str(), cameraId);
                    continue;
                }
                NT_LOG_DEBUG("Raw resolution width %d height %d", snapshotRes.width, snapshotRes.height);
            }

            // Validate resolution
            Size resolutions[] = { previewRes, videoRes, snapshotRes };
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

            Camera3Device::OpenCamera(cameraId);

            Camera3Stream::CreateMultiStreams(resolutions, outputFormats, 3, usageFlags, streamTypes);

            NT_ASSERT(Camera3Stream::ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) == 0, "Config stream failed");

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
                frameNumber++;
                std::ostringstream prefixPreview;
                std::ostringstream prefixVideo;
                std::ostringstream prefixSnap;
                prefixPreview << "Prev_" << testName;
                prefixVideo << "Video_" << testName;
                prefixSnap << "Snap_" << testName;

                std::string previewImageName = ConstructOutputFileName(cameraId, previewFormat, frameNumber,
                    prefixPreview.str().c_str());
                std::string videoImageName   = ConstructOutputFileName(cameraId, videoFormat, frameNumber,
                    prefixVideo.str().c_str());
                std::string snapImageName    = ConstructOutputFileName(cameraId, snapFormat, frameNumber,
                    prefixSnap.str().c_str());

                if (frameIndex % interval == 0)
                {
                    //Will init request with all 3 buffers for live snapshot request
                    CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_VIDEO_SNAPSHOT,
                        numberOfFormats, frameNumber);
                    NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                        "Start Streaming Failed");
                    Camera3Stream::DumpBuffer(0, frameNumber, previewImageName.c_str());
                    Camera3Stream::DumpBuffer(1, frameNumber, videoImageName.c_str());
                    Camera3Stream::DumpBuffer(2, frameNumber, snapImageName.c_str());
                    Camera3Stream::DeleteHalRequest(cameraId);
                }

                else
                {
                    CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_VIDEO_RECORD,
                        numberOfFormats - 1, frameNumber);
                    NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                        "Start Streaming Failed");
                    Camera3Stream::DumpBuffer(0, frameNumber, previewImageName.c_str());
                    Camera3Stream::DumpBuffer(1, frameNumber, videoImageName.c_str());
                    Camera3Stream::DeleteHalRequest(cameraId);
                }

            }

            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::ClearOutStreams();
            Camera3Device::CloseCamera(cameraId);
        }

    }

    /**************************************************************************************************************************
    * RecordingTest::TestVideoStabilization
    *
    * @brief
    *   Test basic video stablization with preview(implementation defined) and video(yuv_420) stream
    *   Sets video stabilization key in request and verifies result for each available mode
    * @return
    *   None
    **************************************************************************************************************************/
    void RecordingTest::TestVideoStabilization()
    {
        Size resolutions[] = { HD, HD };
        int outputFormats[] = { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED , HAL_PIXEL_FORMAT_YCbCr_420_888 };
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, 0 };
        int numberOfFormats = sizeof(outputFormats) / sizeof(outputFormats[0]);
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()),"No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

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

            Camera3Device::OpenCamera(cameraId);

            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);

            Camera3Stream::CreateMultiStreams(resolutions, outputFormats, 2, usageFlags, streamTypes);
            int res = Camera3Stream::ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId);
            NT_ASSERT(res == 0,"Config stream failed");

            int frameNumber = 0;

#ifdef ENABLE3A
            frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
                cameraId,
                frameNumber,
                CAMERA3_TEMPLATE_PREVIEW, // requestType,
                0, // output stream index
                false,
                "");
            NT_ASSERT(frameNumber != -1,"3A did not converge");
#endif //_LINUX

            camera_metadata_entry_t entry;
            int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics),
                ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES,
                &entry);
            NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_CONTROL_VIDEO_STABILIZATION failed");
            for (size_t entryIndex = 0; entryIndex < entry.count; entryIndex++)
            {
                int curSetting = int(entry.data.u8[entryIndex]);
                NT_LOG_DEBUG("start testing VideoStabilization mode: %d", curSetting);

                bool keySet = false;

                for(int frameIndex = 0; frameIndex < PROCESS_CAPTURE_TIMEOUT && !keySet; frameIndex++)
                {
                    CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_VIDEO_RECORD,
                        numberOfFormats, ++frameNumber);
                    Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &entry.data.u8[entryIndex], 1);
                    NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                        "Start Streaming Failed ! ");
                    if (Camera3Stream::CheckKeySetInResult(ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &entry.data.u8[entryIndex],
                        GetHalRequest(cameraId)->frame_number))
                    {
                        NT_LOG_INFO("Key in capture result is set to the requested value");
                        keySet = true;
                    }

                    Camera3Stream::DeleteHalRequest(cameraId);
                }

                NT_ASSERT(keySet, "VideoStabilization key was not set");
            }

            Camera3Stream::ClearOutStreams();
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * RecordingTest::TestVideoZoom
    *
    * @brief
    *   Test basic video zoom with preview(implementation defined) and video(yuv_420) stream
    *   goes through 15 zoom steps and dumps resulting frames
    * @return
    *   None
    **************************************************************************************************************************/
    void RecordingTest::TestVideoZoom()
    {
        Size resolutions[] = { HD, HD };
        int outputFormats[] = { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED , HAL_PIXEL_FORMAT_YCbCr_420_888 };
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, 0 };
        int numberOfFormats = sizeof(outputFormats) / sizeof(outputFormats[0]);
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

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

            Camera3Device::OpenCamera(cameraId);

            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);

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
            NT_ASSERT(frameNumber != -1,"3A did not converge");
#endif //_LINUX

            // Determine zoom test values
            const int ZOOM_STEPS = 15;
            PointF *testZoomValues;
            int testValueCount;
            float errorMargin;
            camera_metadata_entry_t entry;
            int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics),
                ANDROID_SCALER_CROPPING_TYPE,
                &entry);
            NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_SCALER_CROPPING_TYPE failed");
            uint8_t croppingType = entry.data.u8[0];

            if (croppingType == ANDROID_SCALER_CROPPING_TYPE_FREEFORM)
            {
                NT_LOG_DEBUG("Testing zoom with ANDROID_SCALER_CROPPING_TYPE_FREEFORM");
                testZoomValues = static_cast<PointF *>(malloc(sizeof(PointF) * 5));
                if (testZoomValues)
                {
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
                    NT_LOG_ERROR("PointF malloc failed");
                    return;
                }
            }
            else
            {
                NT_LOG_DEBUG("Testing zoom with ANDROID_SCALER_CROPPING_TYPE_CENTER_ONLY");
                testZoomValues = static_cast<PointF *>(malloc(sizeof(PointF)));
                if (testZoomValues)
                {
                    testValueCount = 1;
                    testZoomValues[0] = { 0.5f, 0.5f }; // Center point
                    errorMargin = CROP_REGION_ERROR_PERCENT_CENTERED;
                }
                else
                {
                    NT_LOG_ERROR("PointF malloc failed");
                    return;
                }
            }

            ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics),
                ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM,
                &entry);
            NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM failed");
            float maxZoom = entry.data.f[0];
            NT_ASSERT(maxZoom >= 1.0, "Invalid maxZoom");

            ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics),
                ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
                &entry);
            NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE failed");
            Rect activeArraySize = { entry.data.i32[0], entry.data.i32[1], entry.data.i32[2], entry.data.i32[3] };
            Rect cropRegions[ZOOM_STEPS];
            for (int count = 0; count < testValueCount; count++)
            {
                for (int stepIndex = 0; stepIndex < ZOOM_STEPS; stepIndex++)
                {
                    float zoomFactor = static_cast<float>(1.0f + (maxZoom - 1.0) * stepIndex / ZOOM_STEPS);
                    NT_ASSERT(zoomFactor >= 1.0, "Invalid zoom factor");
                    NT_ASSERT(testZoomValues[count].x <= 1.0 && testZoomValues[count].x >= 0,
                        "Invalid center.x, should be in range of [0, 1.0]");
                    NT_ASSERT(testZoomValues[count].y <= 1.0 && testZoomValues[count].y >= 0,
                        "Invalid center.y, should be in range of [0, 1.0]");
                    cropRegions[stepIndex] = GetCropRegionForZoom(zoomFactor, testZoomValues[count], maxZoom, activeArraySize);

                    int32_t scalerCropRegion[4];
                    scalerCropRegion[0] = cropRegions[stepIndex].left;
                    scalerCropRegion[1] = cropRegions[stepIndex].top;
                    scalerCropRegion[2] = cropRegions[stepIndex].right - cropRegions[stepIndex].left;
                    scalerCropRegion[3] = cropRegions[stepIndex].bottom - cropRegions[stepIndex].top;

                    bool keySet = false;

                    for(int frameIndex = 0; frameIndex < PROCESS_CAPTURE_TIMEOUT && !keySet; frameIndex++)
                    {
                        CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_VIDEO_RECORD,
                            numberOfFormats, ++frameNumber);
                        Camera3Stream::SetKey(cameraId, ANDROID_SCALER_CROP_REGION, &scalerCropRegion[0], 4);
                        NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                            "Start Streaming Failed ! ");
                        if (Camera3Stream::CheckKeySetInResult(ANDROID_SCALER_CROP_REGION, &scalerCropRegion[0], errorMargin,
                            GetHalRequest(cameraId)->frame_number))
                        {
                            NT_LOG_INFO("Key in capture result is set to the requested value");
                            keySet = true;
                        }

                        Camera3Stream::DeleteHalRequest(cameraId);
                    }

                    std::ostringstream outputName;
                    std::ostringstream outputName2;
                    outputName << sImageRootPath << "Prv_YUV420_Cam_" << cameraId << "_" << "Zoom" << stepIndex << ".yuv";
                    outputName2 << sImageRootPath << "Video_YUV420_Cam" << cameraId << "_" << "Zoom" << stepIndex << ".yuv";
                    Camera3Stream::DumpBuffer(0, frameNumber, outputName.str().c_str());
                    Camera3Stream::DumpBuffer(1, frameNumber, outputName2.str().c_str());
                    NT_ASSERT(keySet, "Zoom key was not set");
                }

            }
            if (testZoomValues)
                free(testZoomValues);
            Camera3Stream::ClearOutStreams();
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * RecordingTest::GetCropRegionForZoom
    *
    * @brief
    *   Get a crop region for a given zoom factor and center position.
    * @return
    *   None
    **************************************************************************************************************************/
    RecordingTest::Rect RecordingTest::GetCropRegionForZoom(
        float zoomFactor,         //[in] zoom factor
        PointF center,            //[in] center point
        float maxZoom,            //[in] max zoom
        Rect activeArray) const   //[in] active array size
    {
        float minCenterLength = std::min(std::min(center.x, 1.0f - center.x), std::min(center.y, 1.0f - center.y));
        float minEffectiveZoom = 0.5f / minCenterLength;

        if (minEffectiveZoom > maxZoom)
        {
            NT_LOG_WARN("Requested center has minimal zoomable factor which exceeds max zoom factor");
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

    /***************************************************************************************************
    *   RecordingTest::TestConstrainedHighSpeedRecording
    *
    *   @brief
    *       Tests high speed recording with supported configurations on both bayer1 and bayer2
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void RecordingTest::TestConstrainedHighSpeedRecording(
        Size resolution,    //[in] resolution of the video
        int32_t fpsMin,     //[in] min fps for recording
        int32_t fpsMax,     //[in] max fps for recording
        int captureCount)   //[in] capture count for HFR frames
    {
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
                NT_ASSERT(1 == 0, "Can't get camera info for camera id = %d", cameraId);
            }

            NT_ASSERT(IsHighSpeedSupported(cameraId, info), "High speed support checks failed for camera %d", cameraId);

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
            int32_t batchSize = configs.data.i32[matchIndex + AHSVC_OFFSET_BATCH_SIZE];
            NT_ASSERT(batchSize<= GetPipelineDepth(),"PipelineDepth not supported for current HFR batch size %d", batchSize);

            // Perform test
            TestConstrainedHighSpeedRecordingHelper(cameraId, resolution, fpsMin, fpsMax, batchSize, captureCount);

            // Close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************
    *   RecordingTest::TestConstrainedHighSpeedRecordingHelper
    *
    *   @brief
    *       Helper to test high speed recording with requested configuration on selected camera. This
    *       function does not open/close the camera or check for camera support of the requested config.
    *       The caller must do this instead.
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void RecordingTest::TestConstrainedHighSpeedRecordingHelper(
        int cameraId,       //[in] camera Id to test
        Size resolution,    //[in] resolution of the video
        int32_t fpsMin,     //[in] min fps for recording
        int32_t fpsMax,     //[in] max fps for recording
        int32_t batchSize,  //[in] max allowed batch size for high speed config
        int captureCount)   //[in] capture count for HFR frames
    {
        int outputFormats[] = { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888 };
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, FLAGS_VIDEO_ENCODER };
        int32_t fpsRange[] = { fpsMin, fpsMax };
        Size resolutions[] = { resolution, resolution };
        int frameNumber = 0;

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
        testName << "TestHFRRecording_" << ConvertResToString(resolution).c_str() << "_FPS" << fpsRange[0] << "-" << fpsRange[1];

#ifdef ENABLE3A
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
            if (captureCount < batchSize)
            {
                NT_LOG_UNSUPP("Requested frame capture count: [%d] is less than batch size: [%d], aborting capture request", captureCount, batchSize);
                return;
            }
            NT_LOG_WARN("Requested frame capture count: [%d] is not a multiple of batch size: [%d],"
                " Capturing [%d] frames", captureCount, batchSize, (captureCount / batchSize)*batchSize);
        }
        for (int batchIndex = 0; batchIndex < (captureCount / batchSize); batchIndex++)
        {
            StartHighSpeedStreaming(cameraId, batchSize, frameNumber, testName.str()); //non fatal
        }

        VerifyFinalTargetFPS(fpsRange, cameraId);

        // Cleanup
        Camera3Stream::DeleteHalRequest(cameraId);
        Camera3Device::ClearMetadataMap(cameraId);
        Camera3Stream::FlushStream(cameraId);
        Camera3Stream::ClearOutStreams();
        ClearHighSpeedSession();
    }

    /***************************************************************************************************
    *   RecordingTest::TestConstrainedHighSpeedRecordingSweep
    *
    *   @brief
    *       Sweeps through all supported fixed fps, high speed recording configurations
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void RecordingTest::TestConstrainedHighSpeedRecordingSweep(
        int captureCount)  //[in] capture count for HFR frames
    {
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
                NT_ASSERT(1 == 0, "Can't get camera info for camera id = %d \n",cameraId);
            }

            NT_ASSERT(IsHighSpeedSupported(cameraId, info), "High speed support checks failed for camera %d", cameraId);

            // Get all supported fixed high speed FPS ranges
            camera_metadata_entry configs;
            int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS,
                &configs);
            NT_ASSERT(val == 0, "Failed to obtain available High Speed Video Configurations \n");

            for (size_t i = 0; i < configs.count; i += AHSVC_CONFIG_SIZE)
            {
                // Only test configs that have a fixed fps range (fpsMin == fpsMax)
                if ((i + AHSVC_CONFIG_SIZE - 1) < configs.count &&
                    configs.data.i32[i + AHSVC_OFFSET_FPS_MIN] == configs.data.i32[i + AHSVC_OFFSET_FPS_MAX])
                {
                    Size res = Size(configs.data.i32[i + AHSVC_OFFSET_WIDTH], configs.data.i32[i + AHSVC_OFFSET_HEIGHT]);
                    int fpsMin = configs.data.i32[i + AHSVC_OFFSET_FPS_MIN];
                    int fpsMax = configs.data.i32[i + AHSVC_OFFSET_FPS_MAX];
                    int batchSize = configs.data.i32[i + AHSVC_OFFSET_BATCH_SIZE];
                    NT_ASSERT(batchSize<= GetPipelineDepth(),"PipelineDepth not supported for current HFR batch size");

                    // Run test on configuration
                    TestConstrainedHighSpeedRecordingHelper(cameraId, res, fpsMin, fpsMax, batchSize,
                        captureCount);
                        //<< "Failure in high speed test config: " << ConvertResToString(res) << " @ " << fpsMax
                        //<< "fps, batchSize " << batchSize;
                }
            }

            // Close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************
    *   RecordingTest::StartHighSpeedStreaming
    *
    *   @brief
    *       Helper function to create output buffers and send HAL requests in batches and finally dumps
    *       the image buffers returned back by the camera device.
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void RecordingTest::StartHighSpeedStreaming(
        int cameraId,           // [in] cameraId for the current open camera
        int batchSize,          // [in] batch size for simultaneous requests
        int& frameNumber,       // [in] frame number to start with
        std::string testName)   // [in] testcase name for saving image file
    {
        NT_ASSERT(GetHalRequest(cameraId) != nullptr, "No hal request object!");

        Camera3Device::isCaptureResultsReadyArray[cameraId] = false;
        camera3_stream_buffer_t outBuffers[2];

        std::unique_lock<std::mutex> lck(Camera3Device::halRequestMutex);
        Camera3Device::nOutputBuffersRemaining[cameraId] = batchSize + 1;
        Camera3Device::nInputBuffersRemaining[cameraId] = (nullptr != GetHalRequest(cameraId)->input_buffer) ? 1 : 0;
        Camera3Device::nPartialResultsRemaining[cameraId] = batchSize * GetPartialResultCount(cameraId);
        Camera3Device::partialResultsUpdate(cameraId, batchSize);
        for (int frameIndex = 1; frameIndex <= batchSize; frameIndex++)
        {
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
            int res=-1;
            camera3_device_t* pCamDev = GetCamDevice(cameraId);
            NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
            res = pCamDev->ops->process_capture_request(pCamDev, GetHalRequest(cameraId));

            GetHalRequest(cameraId)->frame_number = frameNumber;
            NT_ASSERT(res == 0,"process_capture_request failed");
        }

        int retries = 0;
        while (!isCaptureResultsReadyArray[cameraId] && retries < MAX_WAIT_RETRIES)
        {
            Camera3Device::resultReadyCondArray[cameraId].wait_for(lck, std::chrono::seconds(HalCmdLineParser::g_timeout));
            retries++;
        }

        // If we have not received a buffer after MAX_WAIT_RETRIES, we need to flush stream so the driver returns the buffers
        // in progress
        if (!isCaptureResultsReadyArray[cameraId])
        {
            FlushStream(cameraId);
        }

        NT_ASSERT(isCaptureResultsReadyArray[cameraId], "Failed to obtain output buffer(s)/metadata(s) after several retires");
        Camera3Device::isCaptureResultsReadyArray[cameraId] = false;
        lck.unlock();

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
        Camera3Device::partialResultsReset(cameraId, batchSize);
    }

    /***************************************************************************************************
    *   RecordingTest::VerifyFinalTargetFPS
    *
    *   @brief
    *       Helper function to read final metadata for target fps range
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void RecordingTest::VerifyFinalTargetFPS(
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
    *   RecordingTest::FindMatchingConfig
    *
    *   @brief
    *       Helper function to find matching availableHighSpeedVideoConfigurations for given parameters
    *
    *   @return
    *       index of matching HFR configuration
    ****************************************************************************************************/

    int RecordingTest::FindMatchingConfig(
        Size res,                      //[in] resolution for the HFR stream
        int32_t fpsMin,                //[in] minimum fps
        int32_t fpsMax,                //[in] maximum fps
        camera_metadata_entry configs) //[in] metadata entry for HFR configurations
    {
        for (int32_t j = 0; (j + AHSVC_CONFIG_SIZE - 1) < static_cast<int32_t>(configs.count); j += AHSVC_CONFIG_SIZE)
        {
            if (res.width == static_cast<uint32_t>(configs.data.i32[j + AHSVC_OFFSET_WIDTH]) &&
                res.height == static_cast<uint32_t>(configs.data.i32[j + AHSVC_OFFSET_HEIGHT]) &&
                fpsMin == configs.data.i32[j + AHSVC_OFFSET_FPS_MIN] &&
                fpsMax == configs.data.i32[j + AHSVC_OFFSET_FPS_MAX])
            {
                Camera3Stream::SetPreviewFPSforHFR(
                    configs.data.i32[j + AHSVC_OFFSET_FPS_MIN],
                    configs.data.i32[j + AHSVC_OFFSET_FPS_MAX]);
                return j;
            }
        }

        return -1;
    }

    /***************************************************************************************************
    *   RecordingTest::IsHighSpeedSupported
    *
    *   @brief
    *       Helper function perform standard high speed support checks
    *
    *   @return
    *       bool true if supported, false otherwise
    ****************************************************************************************************/

    bool RecordingTest::IsHighSpeedSupported(int cameraId, camera_info info)
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

}

