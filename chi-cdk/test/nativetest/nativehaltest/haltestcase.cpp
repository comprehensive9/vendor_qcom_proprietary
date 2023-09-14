//*************************************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

/*
*@file haltestcase.cpp
*@brief Source file for hal test case
*******************************************************************
*/

/*
Desc:
Base test class that inherits from gtest
*/


#include "haltestcase.h"
#include "math.h"

namespace haltests
{

    HalTestCase::HalTestCase()
    {}

    HalTestCase::~HalTestCase()
    {
        c3dMap.clear();
    }

    void HalTestCase::Setup()
    {
        Camera3SuperModule::Setup();
        c3dMap.clear();
    }

    void HalTestCase::Teardown()
    {
        Camera3SuperModule::Teardown();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InitCam3DeviceForCamera()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalTestCase::InitCam3DeviceForCamera(uint32_t camId)
    {
        std::unique_ptr<Cam3Device> c3d = std::make_unique<Cam3Device>(camId);
        c3dMap.insert({ camId, std::move(c3d) });
        c3dMap.at(camId)->Setup();
        for (int streamIdx=0; streamIdx < HalJsonParser::sNumStreams; streamIdx++)
        {
            c3dMap.at(camId)->streamToPCRSendMap.insert({streamIdx, GetPCRFrameNumbers(streamIdx)});
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HalTestCase::AreCameraStreamsEqual()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool HalTestCase::AreCameraStreamsEqual(uint32_t camId, camera3_stream_t &lhs, camera3_stream_t &rhs)
    {
        return c3dMap.at(camId)->AreCameraStreamsEqual(lhs, rhs);
    }

    Size HalTestCase::GetMaxSize(
        uint32_t camId,
        int format,                     //[in] format of the image
        camera_info* camInfo,
        bool usePixelArraySize) const
    {
        return c3dMap.at(camId)->GetMaxSize(format, camInfo, usePixelArraySize);
    }

    Size HalTestCase::GetMaxResolution(
        uint32_t camId,
        int format,
        camera_info* info) const
    {
        int32_t maxWidth = 0;
        int32_t maxHeight = 0;

        const int STREAM_CONFIGURATION_SIZE = 4;
        const int STREAM_FORMAT_OFFSET = 0;
        const int STREAM_WIDTH_OFFSET = 1;
        const int STREAM_HEIGHT_OFFSET = 2;
        const int STREAM_IS_INPUT_OFFSET = 3;

        camera_metadata_entry_t availableStreamConfigs;
        camera_metadata_tag_t streamConfigTag = (mbDepthTest) ? ANDROID_DEPTH_AVAILABLE_DEPTH_STREAM_CONFIGURATIONS :
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            streamConfigTag,
            &availableStreamConfigs);

        if (ret != 0 || availableStreamConfigs.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %u] Stream configuration map could not be found or is empty!", camId);
            return Size(0, 0);
        }

        for (size_t i = 0; i < availableStreamConfigs.count; i += STREAM_CONFIGURATION_SIZE)
        {
            int32_t queriedFormat = availableStreamConfigs.data.i32[i + STREAM_FORMAT_OFFSET];
            int32_t width = availableStreamConfigs.data.i32[i + STREAM_WIDTH_OFFSET];
            int32_t height = availableStreamConfigs.data.i32[i + STREAM_HEIGHT_OFFSET];
            int32_t direction = availableStreamConfigs.data.i32[i + STREAM_IS_INPUT_OFFSET];
            if (direction == ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT &&
                format == queriedFormat &&
                (width * height > maxWidth * maxHeight))
            {
                maxWidth = width;
                maxHeight = height;
            }
        }
        return Size(maxWidth, maxHeight);
    }

    bool HalTestCase::CheckSupportedResolution(
        uint32_t camId,
        camera_info* info,
        int format,             //[in] format to be checked
        Size resolution,        //[in] resolution to be checked
        int direction) const   //[in] direction of the stream
    {

        camera_metadata_entry_t entry;
        camera_metadata_tag_t streamConfigTag = (mbDepthTest) ? ANDROID_DEPTH_AVAILABLE_DEPTH_STREAM_CONFIGURATIONS :
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS;

        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            streamConfigTag, &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("[CAMERAID %u] Can't find the metadata entry for %s.", camId, (mbDepthTest) ?
                "ANDROID_DEPTH_AVAILABLE_DEPTH_STREAM_CONFIGURATIONS" : "ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS");
            return false;
        }

        int matched = 0;

        if (direction != CAMERA3_STREAM_BIDIRECTIONAL)
        {
            direction = (direction == CAMERA3_STREAM_OUTPUT ? ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT :
                (direction == CAMERA3_STREAM_INPUT ? ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_INPUT : -1));

            if (direction == -1)
            {
                NT_LOG_ERROR("[CAMERAID %u] Invalid stream direction: %d", camId, direction);
                return false;
            }
        }

        const int STREAM_CONFIGURATION_SIZE = 4;
        const int STREAM_WIDTH_OFFSET = 1;
        const int STREAM_HEIGHT_OFFSET = 2;
        const int STREAM_IS_INPUT_OFFSET = 3;

        for (size_t i = 0; i < entry.count; i += STREAM_CONFIGURATION_SIZE)
        {
            /*
            * This is a special case because for bi-directional stream we need to ensure
            * given resolution is supported both as input and output
            */

            int32_t queriedFormat = entry.data.i32[i];
            uint32_t width = static_cast<uint32_t>(entry.data.i32[i + STREAM_WIDTH_OFFSET]);
            uint32_t height = static_cast<uint32_t>(entry.data.i32[i + STREAM_HEIGHT_OFFSET]);
            int32_t streamDirection = entry.data.i32[i + STREAM_IS_INPUT_OFFSET];

            if (direction == CAMERA3_STREAM_BIDIRECTIONAL)
            {
                if (streamDirection == ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT &&
                    format == queriedFormat)
                {
                    if (resolution.width == width &&
                        resolution.height == height)
                    {
                        matched++;
                    }
                }
                if (streamDirection == ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_INPUT &&
                    format == queriedFormat)
                {
                    if (resolution.width == width &&
                        resolution.height == height)
                    {
                        matched++;
                    }
                }
            }
            else
            {
                if (streamDirection == direction && format == queriedFormat)
                {
                    if (resolution.width == width &&
                        resolution.height == height)
                    {
                        matched++;
                    }
                }
            }
        }
        // matched may be greater than 2 if there are duplicate entries in dumpsys (CAMX-3410)
        if (direction == CAMERA3_STREAM_BIDIRECTIONAL && matched >= 2)
        {
            return true;
        }
        // matched may be greater than 1 if there are duplicate entries in dumpsys (CAMX-3410)
        if (direction != CAMERA3_STREAM_BIDIRECTIONAL && matched >= 1)
        {
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HalTestCase::GetIntervalForSnapshot
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int HalTestCase::GetIntervalForSnapshot(int nSnaps)
    {
        int numFrames;
        if (HalJsonParser::sFrames == -1)
        {
            NT_LOG_WARN("For duration, there maybe more or less number of snaps taken due to 3A stats and CPU/Memory"
                "utilization on device.");
            numFrames = HalJsonParser::sDuration / 33;
        }
        else
        {
            numFrames = HalJsonParser::sFrames;
        }
        return (numFrames / (nSnaps + 1) + 1);
    }

    std::string HalTestCase::GetMasterCameraOfLogical(uint32_t camId) const
    {
        return c3dMap.at(camId)->GetMasterCameraOfLogical();
    }

    std::string HalTestCase::GetSlaveCameraOfLogical(uint32_t camId) const
    {
        return c3dMap.at(camId)->GetSlaveCameraOfLogical();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsHardwareAtleast()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool HalTestCase::IsHardwareAtleast(int cameraId, int level, camera_info *info)
    {
        (void) cameraId;

        uint8_t sortedHardwareLevels[] = {
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED,
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL,
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY,
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_3,
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_EXTERNAL
        };

        uint8_t deviceLevel = StaticMetadata::GetDeviceHardwareLevel(info);
        if (level == deviceLevel)
        {
            return true;
        }

        for (size_t i = 0; i < NATIVETEST_ELEMENTS(sortedHardwareLevels); i++)
        {
            if (sortedHardwareLevels[i] == level)
            {
                return true;
            }
            else if (sortedHardwareLevels[i] == deviceLevel)
            {
                return false;
            }
        }

        return false;
    }

    camera3_device_t* HalTestCase::GetCamDevice(uint32_t camId) const
    {
        return c3dMap.at(camId)->GetCamDevice();
    }

    camera3_capture_request_t * HalTestCase::GetHalRequest(uint32_t camId) const
    {
        NT_LOG_INFO("[CAMERAID %u] Getting Hal request", camId);
        return c3dMap.at(camId)->GetHalRequest();
    }

    int HalTestCase::FindCameraMetadataFromTemplate(
        uint32_t camId,
        camera_metadata_t* templateRequest,
        camera_metadata_tag_t tag)
    {
        return c3dMap.at(camId)->FindCameraMetadataFromTemplate(templateRequest, tag);
    }

    int HalTestCase::GetCameraMetadataEntryByTag(
        uint32_t camId,
        camera_metadata_t* metadata,
        camera_metadata_tag_t tag,
        camera_metadata_entry_t *entry)
    {
        NT_LOG_INFO("[CAMERAID %u] Setting camera metadata tag...", camId);
        return c3dMap.at(camId)->GetCameraMetadataEntryByTag(metadata, tag, entry);
    }

    int HalTestCase::GetLastProcessdFrameNumber(uint32_t camId)
    {
        return c3dMap.at(camId)->GetLastProcessedFrameNumber();
    }

    void HalTestCase::SetMFNROnStreamRequest(int camId, Cam3Device::streamRequestType &streamReq,
        int startFrame, int endFrame, int skipFrame, int sendFrame, bool setPhysCamSettings)
    {
        NATIVETEST_UNUSED_PARAM(setPhysCamSettings);
        camera_info_t *camInfo = GetCameraInfoWithId(camId);
        char tagName[]         = "CustomNoiseReduction";
        char sectionName[]     = "org.quic.camera.CustomNoiseReduction";
        uint8_t  *tagValuesNR  = new uint8_t;
        *tagValuesNR           = 1;

        //Initialize all vendor tags
        NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
        //Resolve the tags
        uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        NT_ASSERT(tagId < UINT32_MAX, "Custom Noise Reduction vendor tag not available");
        for (int fN = startFrame; fN <= endFrame; fN++)
        {
            // Do not push the tags if frameNumber is equal to skipFrame and push the tags if frameNumber is equal to sendFrame
            if ((skipFrame == -1 || fN % skipFrame != 0) && (sendFrame == -1 || fN % sendFrame == 0))
            {
                Cam3Device::setMetadataTag mTag;
                mTag.tag       = tagId;
                mTag.data      = tagValuesNR;
                mTag.dataCount = 1;
                streamReq.setMap[fN].push_back(mTag);
            }
        }

        if (StaticMetadata::IsNoiseReductionSupported(*camInfo, ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY))
        {
            uint32_t tagIDNRMode   = ANDROID_NOISE_REDUCTION_MODE;
            uint8_t  *tagValueNRHQ = new uint8_t;
            *tagValueNRHQ          = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;
            for (int fN = startFrame; fN <= endFrame; fN++)
            {
                // Do not push the tags if frameNumber is equal to skipFrame and push the tags if frameNumber is equal to sendFrame
                if ((skipFrame == -1 || fN % skipFrame != 0) && (sendFrame == -1 || fN % sendFrame == 0))
                {
                    Cam3Device::setMetadataTag mTag1;
                    mTag1.tag       = tagIDNRMode;
                    mTag1.data      = tagValueNRHQ;
                    mTag1.dataCount = 1;
                    streamReq.setMap[fN].push_back(mTag1);
                }
            }
        }
    }

    void HalTestCase::SetQueriedMaxResolution(uint32_t camId, StreamItem streams[], int numStreams)
    {
        c3dMap.at(camId)->SetQueriedMaxResolution(camId, streams, numStreams);
    }

    void HalTestCase::SetKey(uint32_t camId, uint32_t tag, const int32_t * value, size_t dataCount)
    {
        c3dMap.at(camId)->SetKey(tag, value, dataCount);
    }

    void HalTestCase::SetKey(uint32_t camId, uint32_t tag, const uint8_t * value, size_t dataCount)
    {
        c3dMap.at(camId)->SetKey(tag, value, dataCount);
    }

    void HalTestCase::SetKey(uint32_t camId, uint32_t tag, const float * value, size_t dataCount)
    {
        c3dMap.at(camId)->SetKey(tag, value, dataCount);
    }

    void HalTestCase::SetKey(uint32_t camId, uint32_t tag, const int64_t * value, size_t dataCount)
    {
        c3dMap.at(camId)->SetKey(tag, value, dataCount);
    }

    void HalTestCase::SetKey(uint32_t camId, uint32_t tag, const double * value, size_t dataCount)
    {
        c3dMap.at(camId)->SetKey(tag, value, dataCount);
    }

    void HalTestCase::SetKey(uint32_t camId, uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount)
    {
        c3dMap.at(camId)->SetKey(tag, value, dataCount);
    }

    void HalTestCase::SetDepthTest(uint32_t camId, bool bDepthTest)
    {
        mbDepthTest = bDepthTest;
        c3dMap.at(camId)->SetDepthTest(bDepthTest);
    }

    camera_metadata_t * HalTestCase::GetCameraMetadataByFrame(uint32_t camId, uint32_t frameNumber) const
    {
        return c3dMap.at(camId)->GetCameraMetadataByFrame(frameNumber);
    }

    void HalTestCase::OpenCamera(uint32_t camId)
    {
        if (c3dMap.find(camId) == c3dMap.end())
        {
            InitCam3DeviceForCamera(camId);
        }
        c3dMap.at(camId)->OpenCamera();
    }

    void HalTestCase::CreateStream(
        uint32_t camId,
        int streamType,                         //[in] stream type specifies if stream is O/P, I/P or bidirectional type
        Size resolution,                        //[in] resolution of the stream
        uint32_t usage,                         //[in] usage flag required by gralloc
        int format,                             //[in] format of the stream
        camera3_stream_rotation_t rotation,     //[in] rotation of camera
        int dataspace,                          //[in] dataspace, defaults to -1 if none provided
        std::string physicalCameraId)
    {
        c3dMap.at(camId)->CreateStream(streamType, resolution, usage, format, rotation, dataspace, physicalCameraId);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HalTestCase::ConfigStream
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int HalTestCase::ConfigStream(uint32_t camId, uint32_t opmode, bool setResourceCheck, bool enableRTB)
    {
        return c3dMap.at(camId)->ConfigStream(opmode, setResourceCheck, enableRTB);
    }

    std::vector<std::shared_ptr<camera3_stream_t>> HalTestCase::GetConfigedStreams(uint32_t camId) const
    {
        return c3dMap.at(camId)->GetConfigedStreams();
    }

    void HalTestCase::CreateMultiStreams(
        uint32_t camId,
        Size outputResolutions[],   //[in] Array of resolutions corresponding to each stream
        int outputFormats[],     //[in] Array of formats corresponding to each stream
        int numStreams,          //[in] Number of streams to be configured
        uint32_t usageFlags[],   //[in] Array of usage flags corresponding to each stream
        int streamTypes[])       //[in] Array of stream type corresponding to each stream
    {
        c3dMap.at(camId)->CreateMultiStreams(outputResolutions, outputFormats, numStreams, usageFlags, streamTypes);
    }

    void HalTestCase::SendContinuousPCR(uint32_t camId, std::vector<Cam3Device::streamRequestType> streamRequestType,
        int outputStreamIndex, int totalFrames, int frameNumber)
    {
        c3dMap.at(camId)->ProcessCaptureRequestAsync(streamRequestType, outputStreamIndex, totalFrames, frameNumber);
    }

    void HalTestCase::FlushStream(uint32_t camId)
    {
        c3dMap.at(camId)->FlushStream();
    }

    void HalTestCase::ClearOutStreams(uint32_t camId)
    {
        // close streams
        c3dMap.at(camId)->ClearOutStreams();
    }

    void HalTestCase::CloseCamera(uint32_t camId)
    {
        // close camera
        c3dMap.at(camId)->CloseCamera();
        c3dMap.erase(camId);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UpdateReconfigureTriggerFrames
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalTestCase::UpdateReconfigureTriggerFrames(UINT32 camId, UINT32 frameNum)
    {
        c3dMap.at(camId)->mReconfigureFramesMap[frameNum] = Cam3Device::PCRStatus::DEFAULT;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetPCRFrameNumbers
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<int> HalTestCase::GetPCRFrameNumbers(int streamIndex)
    {
        std::vector<int> linear_spaced_frames;

        if (HalJsonParser::sStreamIntervals[streamIndex] == 0)
        {
            return linear_spaced_frames;
        }

        int numFrames = HalJsonParser::sFrames;
        if (numFrames == -1)
        {
            NT_LOG_WARN("For duration, there maybe more or less number of snaps taken due to 3A stats and CPU/Memory"
                "utilization on device.");
            numFrames = HalJsonParser::sDuration / 33;
        }

        float interval = (numFrames) / (HalJsonParser::sStreamIntervals[streamIndex] + 1);

        if (interval == 0.0)
        {
            interval = 1.0;
        }

        for(int snap = 1; snap <= HalJsonParser::sStreamIntervals[streamIndex]; snap++)
        {
            linear_spaced_frames.push_back(static_cast<int>(ceil(interval * snap)));
        }

        return linear_spaced_frames;
    }

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UpdateMetadataPerFrame
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalTestCase::UpdateMetadataPerFrame(std::map<int, std::vector<Cam3Device::setMetadataTag>> &metadataMap)
    {
        Cam3Device::setMetadataTag metadataTag;

        for (int frame = 1; frame <= HalJsonParser::sFrames; frame++)
        {
            // TODO: Enable MFNR on snapshot frame only
            if (HalJsonParser::sMFNR)
            {
                // ANDROID NOISE REDUCTION TAG
                uint8_t *tagValueNR   = new uint8_t;
                *tagValueNR           = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;
                metadataTag.tag       = ANDROID_NOISE_REDUCTION_MODE;
                metadataTag.data      = tagValueNR;
                metadataTag.dataCount = 1;
                metadataMap[frame].push_back(metadataTag);

                // custom noise reduction vendor tag
                uint8_t *tagValueCNR  = new uint8_t;
                *tagValueCNR          = 1;
                metadataTag.tag       = c3dMap.at(mCurrentTestCamId)->GetTagID(Cam3Device::VENDOR_NOISE_REDUCTION);
                metadataTag.data      = tagValueCNR;
                metadataTag.dataCount = 1;
                metadataMap[frame].push_back(metadataTag);
            }

            if (HalJsonParser::sExpPattern.size() > 0 || HalJsonParser::sRandExpPattern)
            {
                UINT32 currFrameExposureCount = 0;
                if (HalJsonParser::sExpPattern.size() > 0)
                {
                    currFrameExposureCount = HalJsonParser::sExpPattern.at(PATTERN_REQUEST_MOD(frame,
                        HalJsonParser::sExpPattern.size()));
                }
                else if (HalJsonParser::sRandExpPattern)
                {
                    currFrameExposureCount = EXPOSURES_RANGE[rand() % (sizeof(EXPOSURES_RANGE)/sizeof(EXPOSURES_RANGE[0]))];
                }

                uint8_t *tagValueHDR  = new uint8_t;
                *tagValueHDR          = currFrameExposureCount;
                metadataTag.tag       = c3dMap.at(mCurrentTestCamId)->GetTagID(Cam3Device::VENDOR_STATS_EXPOSURECOUNT);
                metadataTag.data      = tagValueHDR;
                metadataTag.dataCount = 1;
                metadataMap[frame].push_back(metadataTag);
            }
        }

    }
}
