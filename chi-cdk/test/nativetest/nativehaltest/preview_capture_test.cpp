//******************************************************************************
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "preview_capture_test.h"
#include "vendor_tags_test.h"
#include <sstream>

namespace haltests
{
    PreviewTest::PreviewTest()
    {}

    PreviewTest::~PreviewTest()
    {}

    void PreviewTest::Setup()
    {
        HalTestCase::Setup();
    }

    /**************************************************************************************************
    *  PreviewTest::TestPreviewWithDualCapture
    *
    * @brief
    *   Tests preview, preview dump, and a snapshot, constantly saving preview, taking a snap after
    *   given interval

    *   snapFormat   int              snapshotFormat
    *   previewRes   Size             previewResolution
    *   snapshotRes  Size             snapshotResolution
    *   testName     char*            test name
    *   nSnaps       int              number of snapshots to capture
    *   dumpAll      bool             flag that decides if preview buffers are dumped when snapshot is dumped
    *   setting      SpecialSetting   request setting
    ***************************************************************************************************/
    void PreviewTest::TestPreviewWithDualCapture(
        int snapFormat,
        Size previewRes,
        Size snapshotRes,
        const char * testName,
        int nSnaps,
        bool dumpAll,
        SpecialSetting setting)
    {
        // Setup and resolve vendor tag
        camera_metadata_tag_t maxPreviewTag;
        char tagName[]        = "MaxPreviewSize";
        char sectionName[]    = "org.quic.camera.MaxPreviewSize";
        // Initialize vendor tags if needed

        NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);

        Size     resolutions[]   = { previewRes, previewRes, snapshotRes };
        int      outputFormats[] = { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, snapFormat };
        int      streamTypes[]   = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[]    = { 0, 0, 0 };
        int      numStreams      = 3;

        std::ostringstream prefixPreview;
        std::ostringstream prefixPreview2;
        std::ostringstream prefixSnap;
        prefixPreview  << "Prv_"  << testName;
        prefixPreview2 << "YuvCb" << testName;
        prefixSnap     << "YuvSnap" << testName;

        int interval = GetIntervalForSnapshot(nSnaps);
        NT_LOG_DEBUG("Interval calculated is %d", interval);

        bool enableZSL = (setting == SettingZSL) ? true : false;
        std::vector<int> camList = HalTestCase::GetCameraList();

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            // Get metadata from MaxPreviewSize vendorTag
            camera_metadata_entry_t tagEntry;
            camera_info* pCamInfoMx = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfoMx, "Unable to retrieve camera info for camera Id: %d", cameraId);

            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoMx->static_camera_characteristics),
                maxPreviewTag, &tagEntry);
            NT_ASSERT(ret == 0, "find_camera_metadata_entry max_preview_size failed");

            if (previewRes.width * previewRes.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                previewRes.width = tagEntry.data.i32[0];
                previewRes.height = tagEntry.data.i32[1];
                NT_LOG_INFO( "OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    previewRes.width, previewRes.height);
            }

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
                continue;
            }

            OpenCamera(cameraId);

            HalTestCase::CreateMultiStreams(cameraId, resolutions, outputFormats, numStreams, usageFlags, streamTypes);
            NT_ASSERT(HalTestCase::ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE) == 0, "configure streams failed");

            //TODO: Implement for a true non ZSL usecase (Should just take snapshots without preview)
            std::vector<Cam3Device::streamRequestType> requestType;
            requestType.push_back({
                //|requestType|format|skipFrame|sendFrame|intervalType|numStreams|fileName|enableZSL|dumpBuffer|
                CAMERA3_TEMPLATE_PREVIEW,{ HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888 }, interval,
                -1, MULTIPLE, 2, { prefixPreview.str(), prefixPreview2.str() }, enableZSL,{ true, true } });

            requestType.push_back({
                //|requestType|format|skipFrame|sendFrame|intervalType|numStreams|fileName|enableZSL|dumpBuffer|
                CAMERA3_TEMPLATE_STILL_CAPTURE,{ HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888,
                    snapFormat },-1, interval, MULTIPLE, numStreams,{ prefixPreview.str(), prefixPreview2.str(),
                    prefixSnap.str() }, enableZSL,{ dumpAll, dumpAll, true } });

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

    void PreviewTest::TestBasicPreview()
    {
        std::vector<int> camList = GetCameraList();

        int previewFormat = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
        Size previewRes = FULLHD_43;

        std::ostringstream prefixPreview;
        prefixPreview << "TestBasicPreview";

        for (size_t camIndex = 0; camIndex < camList.size(); camIndex++)
        {
            int cameraId = camList[camIndex];

            camera_metadata_entry configs;
            camera_info info;
            const camera_module_t *camMod = GetCamModule();
            NT_ASSERT(camMod != nullptr, "Cannot fetch camera module");

            int val = camMod->get_camera_info(cameraId, &info);
            NT_ASSERT(val == 0, "Could not fetch camera info for camera ID %d", cameraId);

            if (HalCmdLineParser::isQCFASize())
            {
                if (StaticMetadata::isQCFASensor(info, vTag))
                {
                    previewRes = StaticMetadata::GetQCFAFullSize(info, vTag);
                    NT_ASSERT(previewRes.width * previewRes.height != 0, "Cannot fetch QCFA full resolution for camera %d", cameraId);
                    NT_LOG_INFO("QCFA resolution found! width %d height %d", previewRes.width, previewRes.height);
                }
                else
                {
                    NT_LOG_WARN("Camera %d is not a QCFA sensor. Skipping...", cameraId);
                    continue;
                }
            }

            OpenCamera(cameraId);

            CreateStream(cameraId, CAMERA3_STREAM_OUTPUT, previewRes, 0, previewFormat, CAMERA3_STREAM_ROTATION_0, -1);
            NT_ASSERT(ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE) == 0,
                "Configuration of streams failed for camera %d", cameraId);

            std::vector<Cam3Device::streamRequestType> requestType;
            requestType.push_back({
                //| requestType             | format     | skipFrame | sendFrame | intervalType | numStreams | fileName         | enableZSL| dumpBuffer |
                CAMERA3_TEMPLATE_PREVIEW,{ previewFormat },  -1,   -1,          NOINTERVAL,      1,{ prefixPreview.str().c_str() },   false, { true }
            });

            if (HalJsonParser::sMFNR)
            {
                requestType[0].setMap.insert(std::map<int, std::vector<Cam3Device::setMetadataTag>>::value_type(1,
                    std::vector<Cam3Device::setMetadataTag>()));
                //Set MFNR mode for every preview frame
                int startFrame = 1;
                int endFrame = HalJsonParser::sFrames;
                SetMFNROnStreamRequest(cameraId, requestType[0], startFrame, endFrame);
            }

            // Sends just 1 frame request to the driver with output stream index starting from 0
            SendContinuousPCR(cameraId, requestType, 0);

            FlushStream(cameraId);
            ClearOutStreams(cameraId);
            CloseCamera(cameraId);
        }
    }


    void PreviewTest::TestPreviewWithMaxSnapshot()
    {
        std::vector<int> camList = GetCameraList();
        int previewFormat = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
        Size previewRes = FULLHD_43;

        int snapFormat = HAL_PIXEL_FORMAT_YCbCr_420_888;
        Size snapRes;

        int outputFormats[] = { previewFormat, snapFormat };
        int numStreams = 2;
        int streamTypes[] = { CAMERA3_STREAM_OUTPUT, CAMERA3_STREAM_OUTPUT };
        uint32_t usageFlags[] = { 0, 0 }; // No usage flags set by default

        std::ostringstream prefixPreview;
        std::ostringstream prefixSnap;
        prefixPreview << "Prev_TestPreviewWithMaxSnapshot";
        prefixSnap << "Snap_TestPreviewWithMaxSnapshot";

        int interval = GetIntervalForSnapshot(HalJsonParser::sSnaps);
        NT_LOG_DEBUG("Interval calculated is %d", interval);

        for (size_t camIndex = 0; camIndex < camList.size(); camIndex++)
        {
            int cameraId = camList[camIndex];

            camera_metadata_entry configs;
            camera_info info;
            const camera_module_t *camMod = GetCamModule();
            NT_ASSERT(camMod != nullptr, "Cannot fetch camera module");

            int val = camMod->get_camera_info(cameraId, &info);
            NT_ASSERT(val == 0, "Could not fetch camera info for camera ID %d", cameraId);

            if (HalCmdLineParser::isQCFASize())
            {
                if (StaticMetadata::isQCFASensor(info, vTag))
                {
                    snapRes = StaticMetadata::GetQCFAFullSize(info, vTag);
                    NT_ASSERT(snapRes.width * snapRes.height != 0, "Cannot fetch QCFA full resolution for camera %d",
                        cameraId);
                    NT_LOG_INFO("QCFA resolution found for cameraid %d! width %d height %d", cameraId,
                        snapRes.width, snapRes.height);
                }
                else
                {
                    NT_LOG_UNSUPP("Camera %d is not a QCFA sensor. Skipping...", cameraId);
                    continue;
                }
            }
            else
            {
                std::map<int, std::vector<Size>> fmtSizesMap = StaticMetadata::GetAvailableFormatSizesMap(info);
                if (fmtSizesMap.size() > 0)
                {
                    snapRes = (fmtSizesMap[snapFormat].size() > 0) ? fmtSizesMap[snapFormat].at(0) : Size(0, 0);
                }
                if (snapRes.width == 0 && snapRes.height == 0)
                {
                    NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]", snapFormat,
                        ConvertResToString(snapRes).c_str(), cameraId);
                    continue;
                }
                NT_LOG_INFO("Max resolution width %d height %d for camera %d", snapRes.width, snapRes.height, cameraId);
            }

            Size resolutions[] = { previewRes, snapRes };

            OpenCamera(cameraId);

            HalTestCase::CreateMultiStreams(cameraId, resolutions, outputFormats, numStreams, usageFlags, streamTypes);
            NT_ASSERT(ConfigStream(cameraId, CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE) == 0,
                "Configuration of streams failed for camera %d", cameraId);

            std::vector<Cam3Device::streamRequestType> requestType;
            requestType.push_back({
                //| requestType| format|skipFrame|sendFrame|intervalType|numStreams|fileName|enableZSL|dumpBuffer|
                CAMERA3_TEMPLATE_PREVIEW,{ previewFormat },  interval, -1, MULTIPLE, 1, { prefixPreview.str().c_str() }, false,
                { true }
            });

            if (HalJsonParser::sMFNR)
            {
                requestType[0].setMap.insert(std::map<int, std::vector<Cam3Device::setMetadataTag>>::value_type(1,
                    std::vector<Cam3Device::setMetadataTag>()));
                //Set MFNR mode for every preview frame except for interval
                int startFrame = 1;
                int endFrame = HalJsonParser::sFrames;
                SetMFNROnStreamRequest(cameraId, requestType[0], startFrame, endFrame, interval);
            }

            if (HalCmdLineParser::isQCFASize()) {
                requestType.push_back({
                    //| requestType|format|skipFrame|sendFrame|intervalType|numStreams|fileName|enableZSL|dumpBuffer|
                    CAMERA3_TEMPLATE_STILL_CAPTURE, { snapFormat }, -1, interval, MULTIPLE, 1, { prefixSnap.str().c_str() },
                    false, { true }
                });
                // pushing stream index 1, as only snapshot stream is needed
                requestType[1].streamIndicesToRequest.push_back(1);
                for (int fn = interval; fn <= HalJsonParser::sFrames; fn += interval)
                {
                    NT_LOG_INFO("Adding frame: %d to reconfigure list", fn);
                    UpdateReconfigureTriggerFrames(cameraId, static_cast<UINT32>(fn));
                }
            }
            else
            {
                requestType.push_back({
                    //| requestType|format|skipFrame|sendFrame|intervalType|numStreams|fileName|enableZSL|dumpBuffer|
                    CAMERA3_TEMPLATE_STILL_CAPTURE, { previewFormat, snapFormat}, -1, interval, MULTIPLE, numStreams,
                    { prefixPreview.str().c_str(), prefixSnap.str().c_str() }, false, { true, true }
                });
            }

            if (HalJsonParser::sMFNR)
            {
                requestType[1].setMap.insert(std::map<int, std::vector<Cam3Device::setMetadataTag>>::value_type(1,
                    std::vector<Cam3Device::setMetadataTag>()));
                // Set MFNR mode for snapshot stream at interval
                int startFrame = 1;
                int endFrame   = HalJsonParser::sFrames;
                SetMFNROnStreamRequest(cameraId, requestType[1], startFrame, endFrame, -1, interval);
            }

            // Sends just 1 frame request to the driver with output stream index starting from 0
            SendContinuousPCR(cameraId, requestType, 0);

            FlushStream(cameraId);
            ClearOutStreams(cameraId);
            CloseCamera(cameraId);
        }
    }
}
