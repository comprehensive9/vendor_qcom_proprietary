//******************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "still_capture_test.h"
#include "capture_request_test.h"
#include "vendor_tags_test.h"

namespace haltests
{

    StillCaptureTest::StillCaptureTest()
    {}

    StillCaptureTest::~StillCaptureTest()
    {}

    void StillCaptureTest::Setup()
    {
        HalTestCase::Setup();
    }

    void StillCaptureTest::Teardown()
    {
    }

    /***************************************************************************************************************************
    * StillCaptureTest::TestSingleCameraCapture()
    *
    * @brief
    *   Opens a single camera and captures frame(s) in specified format
    *
    * @return
    *   void
    **************************************************************************************************************************/
    void StillCaptureTest::TestSingleCameraCapture(
        int format,             //[in] image format to be captured
        Size resolution,        //[in] image resolution
        const char * testName,  //[in] test name
        int numberOfCaptures,   //[in] number of captures to take
        bool mustSupportRes,    //[in] dictates if the test should fail if resolution is unsupported
        SpecialSetting setting) //[in] special setting to apply
    {
        // If stream is MAX_RES resolution or RAW format, need to query each camera for its max resolution
        bool shouldQueryMaxRes = false;
        if ((resolution.width == MAX_RES.width && resolution.height == MAX_RES.height) ||
            format == HAL_PIXEL_FORMAT_RAW16 ||
            format == HAL_PIXEL_FORMAT_RAW10)
        {
            shouldQueryMaxRes = true;
        }

        std::vector<int> camList = GetCameraList();
        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // open camera
            OpenCamera(cameraId);

            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);

            // Query and overwrite with queried maximum resolution
            if (shouldQueryMaxRes)
            {
                resolution = GetMaxSize(cameraId, format, pCamInfo);
                if (resolution.width == 0 && resolution.height == 0)
                {
                    NT_LOG_INFO("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        format, ConvertResToString(resolution).c_str(), cameraId);
                    CloseCamera(cameraId);
                    continue;
                }
                NT_LOG_INFO("Raw resolution width %d height %d", resolution.width, resolution.height);
            }

            if (!CheckSupportedResolution(cameraId, pCamInfo, format, resolution))
            {
                NT_LOG_INFO("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]", format,
                    ConvertResToString(resolution).c_str(), cameraId);
                // Automation looks for on the wording of the following log message. DO NOT CHANGE without warning them first!
                NT_EXPECT(!mustSupportRes,"Format %d and resolution %s  must be supported for this testcase!", format,
                    ConvertResToString(resolution).c_str());
                CloseCamera(cameraId);
                continue;
            }

            // Apply any special settings
            switch (setting)
            {
            case SettingZSL:
                NT_LOG_WARN("ZSL is enabled by default");
                break;
            case SettingSWMF:
                NT_LOG_WARN("This test requires \"advanceFeatureMask=0x8\" in camxoverridesettings.txt");
                break;
            case SettingSAT:
                NT_LOG_WARN("This test requires \"multiCameraEnable=1\", \"multiCameraSATEnable=1\" and "
                    "\"multiCamera3ASync=MultiCamera3ASyncQTI\" in camxoverridesettings.txt");
                break;
            case SettingBokeh:
                NT_LOG_WARN("This test requires \"multiCameraEnable=1\" in camxoverridesettings.txt");
                break;
            default:
                NT_LOG_INFO("No special settings applied.");
                break;
            }

            TestCapture(cameraId, format, resolution, testName, numberOfCaptures, setting); //non fatal

            FlushStream(cameraId);
            // clear streams
            ClearOutStreams(cameraId);
            // close camera
            CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * StillCaptureTest::TestSnapshotWithPreview()
    *
    * @brief
    *   Tests preview and up to 2 snapshots, constantly saving preview, taking up to two simultaneous snapshots every 10
    *   frames. Snap2 is optional and will be skipped if its format value is -1. Snap1 format cannot be -1, and will never be
    *   skipped.
    *
    * @return
    *   void
    **************************************************************************************************************************/
    void StillCaptureTest::TestSnapshotWithPreview(
        int previewFormat,     //[in] preview format
        int snap1Format,       //[in] snapshot 1 format
        int snap2Format,       //[in] snapshot 2 format
        Size previewRes,       //[in] preview resolution
        Size snap1Res,         //[in] snapshot 1 resolution
        Size snap2Res,         //[in] snapshot 2 resolution
        const char * testName, //[in] test name
        int captureCount,      //[in] number of frames to capture
        bool dumpPreview,      //[in] flag that decides if preview buffers are dumped
        bool enableZsl)        //[in] flag that decides if ENABLE_ZSL tag should be tested
    {
        // Setup and resolve maxPreviewSize vendor tag
        NATIVETEST_UNUSED_PARAM(captureCount);
        camera_metadata_tag_t maxPreviewTag;
        bool vendorTagsInit = false;
        char tagName[] = "MaxPreviewSize";
        char sectionName[] = "org.quic.camera.MaxPreviewSize";
        // Initialize vendor tags if needed
        if (!vendorTagsInit)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
            vendorTagsInit = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);

        std::vector<int> camList = HalTestCase::GetCameraList();

        int outputFormats[] = { previewFormat, snap1Format, snap2Format };
        bool isSnap2On = (snap2Format == -1) ? false : true;
        int numStreams = isSnap2On ? 3 : 2;
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, 0, 0 }; // No usage flags set by default

        // Set GRALLOC_USAGE_HW_IMAGE_ENCODER for snapshot streams in HEIF tests
        if (0 == strcmp(testName, "TestHEIF"))
        {
            usageFlags[0] = 0; // preview stream
            usageFlags[1] = 0; // snapshot 1
            usageFlags[2] = GRALLOC_USAGE_HW_IMAGE_ENCODER; // snapshot 2
        };

        // If stream is MAX_RES resolution or RAW format, need to query each camera for its max resolution
        bool shouldQueryMaxRes_snap1 = false;
        bool shouldQueryMaxRes_snap2 = false;
        if ((snap1Res.width == MAX_RES.width && snap1Res.height == MAX_RES.height) ||
            snap1Format == HAL_PIXEL_FORMAT_RAW16 ||
            snap1Format == HAL_PIXEL_FORMAT_RAW10)
        {
            shouldQueryMaxRes_snap1 = true;
        }
        if (isSnap2On && ((snap2Res.width == MAX_RES.width && snap2Res.height == MAX_RES.height) ||
            snap2Format == HAL_PIXEL_FORMAT_RAW16 ||
            snap2Format == HAL_PIXEL_FORMAT_RAW10))
        {
            shouldQueryMaxRes_snap2 = true;
        }

        std::ostringstream prefixPreview;
        std::ostringstream prefixSnap1;
        std::ostringstream prefixSnap2;
        prefixPreview << "Prev_" << testName;
        prefixSnap1 << "Snap1_" << testName;
        prefixSnap2 << "Snap2_" << testName;

        int interval = GetIntervalForSnapshot(HalJsonParser::sSnaps);
        NT_LOG_DEBUG("Interval calculated is %d", interval);

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
                NT_LOG_INFO("OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    previewRes.width, previewRes.height);
            }

            HalTestCase::OpenCamera(cameraId);
            // Snapshot1: query and overwrite with queried maximum resolution
            if (shouldQueryMaxRes_snap1)
            {
                snap1Res = HalTestCase::GetMaxSize(cameraId, snap2Format, pCamInfoMx);
                if (snap1Res.width == 0 && snap1Res.height == 0)
                {
                    NT_LOG_INFO("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        snap1Format, ConvertResToString(snap1Res).c_str(), cameraId);
                    CloseCamera(cameraId);
                    continue;
                }
                NT_LOG_INFO("Raw resolution width %d height %d", snap1Res.width, snap1Res.height);
            }

            // Snapshot2: query and overwrite with queried maximum resolution
            if (shouldQueryMaxRes_snap2)
            {
                snap2Res = GetMaxSize(cameraId, snap2Format, pCamInfoMx);
                if (snap2Res.width == 0 && snap2Res.height == 0)
                {
                    NT_LOG_INFO("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        snap2Format, ConvertResToString(snap2Res).c_str(), cameraId);
                    CloseCamera(cameraId);
                    continue;
                }
                NT_LOG_INFO("Raw resolution width %d height %d", snap2Res.width, snap2Res.height);
            }

            Size resolutions[] = { previewRes, snap1Res, snap2Res };
            bool matched = true;
            for (int index = 0; index < numStreams; index++)
            {
                if (!CheckSupportedResolution(cameraId, pCamInfoMx, outputFormats[index], resolutions[index]))
                {
                    NT_LOG_INFO("Given format: [%s] and resolution: [%s] not supported for cameraId : [%d]",
                        ConvertFormatToString(outputFormats[index]).c_str(), ConvertResToString(resolutions[index]).c_str(),
                        cameraId);
                    matched = false;
                    break;
                }

            }
            if (!matched)
            {
                CloseCamera(cameraId);
                continue;
            }

           HalTestCase::CreateMultiStreams(
                cameraId,
                resolutions,
                outputFormats,
                numStreams,
                usageFlags,
                streamTypes
            );
            NT_ASSERT(HalTestCase::ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE) == 0, "configure streams failed");

            //TODO: Implement for a true non ZSL usecase (Should just take snapshots without preview)
            std::vector<Cam3Device::streamRequestType> requestType;
            requestType.push_back({
             //| requestType             | format     | skipFrame | sendFrame | intervalType | numStreams | fileName                      | enableZSL      | dumpBuffer |
            CAMERA3_TEMPLATE_PREVIEW,{  previewFormat },  interval,   -1,          MULTIPLE,      1,        { prefixPreview.str().c_str() },   false,  { dumpPreview }
            });

            requestType.push_back({
            //| requestType             | format                        | skipFrame | sendFrame | intervalType | numStreams | fileName                                                     | enableZSL    | dumpBuffer |
            CAMERA3_TEMPLATE_STILL_CAPTURE,{ previewFormat, snap1Format },  -1,        interval   , MULTIPLE,      numStreams, { prefixPreview.str().c_str(), prefixSnap1.str().c_str() },   enableZsl , { dumpPreview, true }
            });


            if (isSnap2On)
            {
                requestType[1].formatList.push_back(snap2Format);
                requestType[1].fileNameList.push_back(prefixSnap2.str());
                requestType[1].dumpBufferList.push_back(isSnap2On);
            }
            HalTestCase::SendContinuousPCR(
                cameraId,
                requestType,
                0
            );

            HalTestCase::FlushStream(cameraId);
            HalTestCase::ClearOutStreams(cameraId);
            HalTestCase::CloseCamera(cameraId);
        }

    }

    /***************************************************************************************************************************
    * StillCaptureTest::TestSnapshotGeneric()
    *
    * @brief
    *   Tests a generic amount of previews and snapshots
    *
    * @return
    *   void
    ***************************************************************************************************************************/
    void StillCaptureTest::TestSnapshotGeneric(
        StillCaptureItem previews[],    //[in] preview streams
        int numPreviews,                //[in] number of preview streams
        StillCaptureItem snapshots[],   //[in] snapshot streams
        int numSnapshots,               //[in] number of snapshot streams
        const char * testName,          //[in] test name
        int captureCount)               //[in] number of frames to capture
    {
        std::vector<int> camList = HalTestCase::GetCameraList();
        NATIVETEST_UNUSED_PARAM(captureCount);
        int totalStreams = numPreviews + numSnapshots;

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            HalTestCase::OpenCamera(cameraId);
            // Get maximum resolutions
            HalTestCase::SetQueriedMaxResolution(cameraId, previews, numPreviews);
            HalTestCase::SetQueriedMaxResolution(cameraId, snapshots, numSnapshots);
            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);


            // Check all streams for supported resolutions
            if (!SupportedResolutionHelper(cameraId, pCamInfo, previews, numPreviews) ||
                !SupportedResolutionHelper(cameraId, pCamInfo, snapshots, numSnapshots))
            {
                // Some of the streams have unsupported resolutions, skip to next camera
                continue;
            }

            CreateStreamHelper(cameraId, previews, numPreviews, snapshots, numSnapshots);
            NT_ASSERT(HalTestCase::ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) == 0, "configure streams failed");

            std::vector<Cam3Device::streamRequestType> requestType;
            requestType.push_back({
             //| requestType             | format  | skipFrame | sendFrame | intervalType | numStreams | fileName | enableZSL| dumpBuffer |
             CAMERA3_TEMPLATE_STILL_CAPTURE,{  },       -1,             -1 ,      NOINTERVAL, totalStreams, {  },        true,  { true }
            });

            for (int index = 0; index < numPreviews; index++)
            {
                std::ostringstream prefixPreview;
                prefixPreview << "Prev" << index+1 << "_" << testName;
                requestType[0].formatList.push_back(previews[index].format);
                requestType[0].fileNameList.push_back(prefixPreview.str());
            }
            for (int index = 0; index < numSnapshots; index++)
            {
                std::ostringstream prefixSnap;
                prefixSnap << "Snap" << index + 1 << "_" << testName;
                requestType[0].formatList.push_back(snapshots[index].format);
                requestType[0].fileNameList.push_back(prefixSnap.str());
            }

            HalTestCase::FlushStream(cameraId);
            HalTestCase::ClearOutStreams(cameraId);
            HalTestCase::CloseCamera(cameraId);
        }

    }

    /****************************************************************************************************************************
    * StillCaptureTest::TestCapture()
    *
    * @brief
    *   Initiates capture requests to the specified camera, wait for 3A convergences and captures the defined number of
    *   subsequent frames
    *
    * @return
    *   void
    ***************************************************************************************************************************/
    void StillCaptureTest::TestCapture(
        int cameraId,         //[in] camera ID
        int format,           //[in] output stream format
        Size resolution,      //[in] output stream resolution
        const char* testName, //[in] test case name to be attached in the output file name
        int captureCount,     //[in] count of captures
        int setting)          //[in] request setting
    {
        NATIVETEST_UNUSED_PARAM(captureCount);
        // Create one output stream
        HalTestCase::CreateStream(
            cameraId,
            CAMERA3_STREAM_OUTPUT,
            resolution,
            0,
            format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(HalTestCase::ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE) == 0, "configure streams failed");

        std::vector<Cam3Device::streamRequestType> requestType;
        if (HalCmdLineParser::g_vstats != -1)
        {
            requestType.push_back
            (  //| requestType                  | format   | skipFrame | sendFrame | intervalType | numStreams | fileName                | enableZSL | dumpBuffer |
                { CAMERA3_TEMPLATE_PREVIEW,      { format }, -1,        -1,          NOINTERVAL,      1,          { "" },                    false,     { false } }
            );
        }
        requestType.push_back
        (
            //| requestType                  | format   | skipFrame | sendFrame | intervalType | numStreams | fileName                | enableZSL                       | dumpBuffer |
            { CAMERA3_TEMPLATE_STILL_CAPTURE, { format }, -1,        -1,          NOINTERVAL,      1,         { std::string(testName) }, (setting==SettingZSL)?true:false, { true } }
        );

        HalTestCase::SendContinuousPCR(cameraId, requestType, 0); // output stream index
    }

    /***************************************************************************************************************************
    * StillCaptureTest::TestDepthCamera()
    *
    * @brief
    *   Test cameras that support depth capability using HAL_DATASPACE_DEPTH. Secure streaming mode may also be activated.
    * @return
    *   void
    ***************************************************************************************************************************/
    void StillCaptureTest::TestDepthCamera(
        StillCaptureItem preview,       //[in] preview stream
        const char *testName,           //[in] test name
        int captureCount,               //[in] capture count
        bool bEnableSecureStreaming,    //[in] enables secure streaming
        bool bVerifyIntrinsics)         //[in] test for len intrinsics, defaults to false
    {
        NATIVETEST_UNUSED_PARAM(captureCount);
        std::vector<int> camList = HalTestCase::GetCameraList();
        int numPreviews = 1;
     // Setup and resolve maxPreviewSize vendor tag
        camera_metadata_tag_t maxPreviewTag;
        bool vendorTagsInit = false;
        char tagName[] = "MaxPreviewSize";
        char sectionName[] = "org.quic.camera.MaxPreviewSize";

        // Initialize vendor tags if needed
        if (!vendorTagsInit)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0,"Vendor tags could not be initialized!");
            vendorTagsInit = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);
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
            NT_EXPECT(ret == 0, "find_camera_metadata_entry max_preview_size failed");
            if (preview.resolution.width*preview.resolution.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                preview.resolution.width = tagEntry.data.i32[0];
                preview.resolution.height = tagEntry.data.i32[1];
                NT_LOG_INFO("OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    preview.resolution.width, preview.resolution.height);
            }

            HalTestCase::OpenCamera(cameraId);
            HalTestCase::SetDepthTest(cameraId, true);

            // Check that camera can support depth capability
            bool bDepthSupported = StaticMetadata::IsCapabilitySupported(*pCamInfoMx, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT);
            if (!bDepthSupported)
            {
                NT_LOG_INFO("Depth not supported on camera %d, skipping...", cameraId);
                HalTestCase::CloseCamera(cameraId);
                continue;
            }

            // Verify static camera lens intrinsics
            if (bVerifyIntrinsics)
            {
                NT_LOG_INFO("Testing depth lens intrinsics in static camera characteristics");
                camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
                NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);
                NT_EXPECT(CDKResultSuccess !=VerifyDepthLensIntrinsics(cameraId, pCamInfo->static_camera_characteristics),
                    "Failed to verify depth lens intrinsics for static camera characteristics!");
            }

            // Query the max depth resolution supported
            SetQueriedMaxResolution(cameraId, &preview, numPreviews);

            // Check all streams for supported resolutions
            if (!SupportedResolutionHelper(cameraId, pCamInfoMx, &preview, numPreviews))
            {
                CloseCamera(cameraId);
                // Some of the streams have unsupported resolutions, skip to next camera
                continue;
            }

            // Setup usage flags for secure streaming, if enabled
            uint32_t usage = 0;
            if (bEnableSecureStreaming)
            {
#ifdef OS_ANDROID
                usage |= GRALLOC1_PRODUCER_USAGE_PROTECTED;
                NT_LOG_INFO("Setting secure streaming usage flag: [%u]", usage);
#else
                NT_LOG_INFO("Secure streaming cannot be set on presilicon platform!");
#endif
            }

            // configure streams
             HalTestCase::CreateStream(cameraId, CAMERA3_STREAM_OUTPUT, preview.resolution, usage, preview.format,
                CAMERA3_STREAM_ROTATION_0, HAL_DATASPACE_DEPTH);
             NT_ASSERT(HalTestCase::ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE) == 0, "configure streams failed");

            std::ostringstream prefixPreview;
            prefixPreview << "Prev_" << testName;

            std::vector<Cam3Device::streamRequestType> requestType;
            Cam3Device::streamRequestType streamRequest;

            requestType.push_back
            (
                //| requestType             | format  | skipFrame | sendFrame | intervalType | numStreams | fileName                        | enableZSL | dumpBuffer |
            { CAMERA3_TEMPLATE_PREVIEW,{ preview.format },  -1,       -1   ,    NOINTERVAL,   1,            { prefixPreview.str().c_str() },    false,   {true } }
            );

            HalTestCase::SendContinuousPCR(
                cameraId,
                requestType,
                0); // output stream index,


              // Verify lens intrinsics on the last capture result's metadata
            if (bVerifyIntrinsics)
            {
                int lastProcessedFrameNumber = HalTestCase::GetLastProcessdFrameNumber(cameraId);
                NT_LOG_INFO("Testing depth lens intrinsics in capture result frame %d", lastProcessedFrameNumber);
                const camera_metadata_t* resultMetadata = HalTestCase::GetCameraMetadataByFrame(cameraId, lastProcessedFrameNumber);
                NT_EXPECT(CDKResultSuccess !=VerifyDepthLensIntrinsics(cameraId, resultMetadata),
                    "Failed to verify depth lens intrinsics for capture results!");
                free_camera_metadata(const_cast<camera_metadata_t*>(resultMetadata));
            }

            HalTestCase::FlushStream(cameraId);
            HalTestCase::ClearOutStreams(cameraId);
            HalTestCase::CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************************************
    * StillCaptureTest::SupportedResolutionHelper()
    *
    * @brief
    *   Helper to batch check supported resolution on StillCaptureItems
    *
    * @return
    *   bool true if all streams have supported resolutions, otherwise false
    ***************************************************************************************************************************/
    bool StillCaptureTest::SupportedResolutionHelper(
        int cameraId,                       //[in] camera ID
        camera_info* info,
        const StillCaptureItem streams[],   //[in] streams to check
        int numStreams) const               //[in] number of streams

    {
        bool result = true;
        for (int s = 0; s < numStreams; s++)
        {
            if (!CheckSupportedResolution(cameraId, info, streams[s].format, streams[s].resolution, 0))
            {
                NT_LOG_INFO("Given format: [%s] and resolution: [%s] not supported for cameraId : [%d]",
                    ConvertFormatToString(streams[s].format).c_str(), ConvertResToString(streams[s].resolution).c_str(),
                    cameraId);
                result = false;
            }
        }
        return result;
    }

    /***************************************************************************************************************************
    * StillCaptureTest::CreateStreamHelper()
    *
    * @brief
    *   Helper to create streams for still capture tests
    *
    * @return
    *   void
    ***************************************************************************************************************************/
    void StillCaptureTest::CreateStreamHelper(
        int cameraId,
        const StillCaptureItem previews[],
        int numPreviews,
        const StillCaptureItem snapshots[],
        int numSnapshots)
    {
        int index;

        // Create a stream for every preview output
        for (index = 0; index < numPreviews; index++)
        {
            HalTestCase::CreateStream(
                cameraId,
                CAMERA3_STREAM_OUTPUT,
                previews[index].resolution,
                0,
                previews[index].format,
                CAMERA3_STREAM_ROTATION_0);
        }

        // Create a stream for every snapshot output
        for (index = 0; index < numSnapshots; index++)
        {
            HalTestCase::CreateStream(
                cameraId,
                CAMERA3_STREAM_OUTPUT,
                snapshots[index].resolution,
                0,
                snapshots[index].format,
                CAMERA3_STREAM_ROTATION_0);
        }
    }

    /***************************************************************************************************************************
    * StillCaptureTest::VerifyDepthLensIntrinsics()
    *
    * @brief
    *   Helper verify depth camera lens intrinsics in specified metadata
    *
    * @return
    *   CDKResult
    ***************************************************************************************************************************/
    CDKResult StillCaptureTest::VerifyDepthLensIntrinsics(int cameraId, const camera_metadata_t* metadata)
    {
        const uint32_t expectNumOfParams = 5;

        // Check metadata for lens intrinsics
        camera_metadata_entry_t entry;
        int ret = HalTestCase::GetCameraMetadataEntryByTag(cameraId, const_cast<camera_metadata_t*>(metadata), ANDROID_LENS_INTRINSIC_CALIBRATION,
            &entry);
        if (0 != ret)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_LENS_INTRINSIC_CALIBRATION failed");
            return CDKResultEFailed;
        }

        // Verify count of parameters
        NT_LOG_INFO("Number of lens intrinsic parameters: %u", entry.count);
        if (expectNumOfParams != entry.count)
        {
            NT_LOG_ERROR("Number of lens intrinsics parameters not as expected! (expect: %u, found: %u)",
                expectNumOfParams, entry.count);
            return CDKResultEFailed;
        }

        // Print parameters
        NT_LOG_INFO("Depth lens intrinsics f_x: %f, f_y: %f, c_x: %f, c_y: %f, s: %f", entry.data.f[0],
            entry.data.f[1], entry.data.f[2], entry.data.f[3], entry.data.f[4]);

        // Check for all 0's (not a hard requirement)
        float sum = 0.0;
        for (size_t i = 0; i < entry.count; i++)
        {
            sum += entry.data.f[i];
        }
        if (0.0 == sum)
        {
            NT_LOG_WARN("All lens intrinsics for camera %d are 0!", cameraId);
        }

        return CDKResultSuccess;
    }

} // namespace tests
