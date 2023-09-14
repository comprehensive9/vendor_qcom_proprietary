//*************************************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************
#include "cam3device.h"
#include <system/camera_metadata.h>
#include "vendor_tags_test.h"
#include <chrono>

namespace haltests {

    Cam3Device::Cam3Device(uint32_t cameraId) : halRequest(nullptr),
                                                mCamId(cameraId)
    {
        mCamId = cameraId;
        camera3_callback_ops_t::process_capture_result = &aprocess_capture_result;
        camera3_callback_ops_t::notify = &anotify;
    }

    Cam3Device::~Cam3Device()
    {
        mStreams.clear();
        allocatedStreamsforHFR.clear();
        nativeBuffers.clear();
    }

    void Cam3Device::Setup()
    {
        Camera3SuperModule::Setup();
        int camNum = Camera3SuperModule::GetNumCams();
        NT_ASSERT(0< camNum, "No cameras found");

        mCamInfo = GetCameraInfoWithId(mCamId);
        NT_ASSERT(mCamInfo != nullptr, "Could not fetch camera info for camera ID %d", mCamId);

        for (uint32_t i = 0; i < NATIVETEST_ELEMENTS(mErrorNotifyBitFlag); i++)
        {
            ClearErrorNotifyFlag(i);
        }
        PCRTracker.clear();
        mGetActivePhysCam = false;
        DeleteHalRequest();

        UINT32 tagId;
        if (HalJsonParser::sMFNR)
        {
            tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, "CustomNoiseReduction",
                "org.quic.camera.CustomNoiseReduction");
            NT_ASSERT(tagId < UINT32_MAX, "Custom Noise Reduction vendor tag not available");
            mTagIdMap.insert({VENDOR_NOISE_REDUCTION, tagId});

            NT_ASSERT(StaticMetadata::IsNoiseReductionSupported(*mCamInfo, ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY),
                "Noise Reduction High Quality is not supported by the device");
        }

        if (HalJsonParser::sExpPattern.size() > 0 || HalJsonParser::sRandExpPattern)
        {
            tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, "ExposureCount",
                "com.qti.stats_control");
            NT_ASSERT(tagId < UINT32_MAX, "com.qti.stats_control.ExposureCount vendor tag not available");
            mTagIdMap.insert({VENDOR_STATS_EXPOSURECOUNT, tagId});
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::OpenCamera()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::OpenCamera()
    {
        NT_LOG_DEBUG("[CAMERAID %u] Opening camera: %d", mCamId, mCamId);

        NT_ASSERT(nullptr != Camera3SuperModule::GetCamModule()->common.methods->open
            , "Camera open() is unimplemented");

        NT_ASSERT(nullptr != Camera3SuperModule::GetCamModule()->get_camera_info
            , "get_camera_info is not implemented");

        camera_info info;
        NT_ASSERT(0 == Camera3SuperModule::GetCamModule()->get_camera_info(mCamId, &info) // need to have this to open camera?
            , "Can't get camera info for camera %d", mCamId);

        hw_device_t* localDevice = nullptr;

        int res = GetCamModule()->common.methods->open(
            (const hw_module_t*)GetCamModule(), std::to_string(mCamId).c_str(), &localDevice);

        if (res == CamxResultETooManyUsers)
        {
            NT_LOG_WARN("[CAMERAID %u] Hardware resource insufficient for camera", mCamId);
            return;
        }
        else
        {
            NT_ASSERT(0 == res, "Can't open camera device %d", mCamId);
        }

        NT_ASSERT(nullptr != localDevice
            , "Camera open(%d) returned a NULL device", mCamId);

        NT_ASSERT(kVersion3_0 <= localDevice->version
            , "The device does not support HAL3");

        mCamDevice = reinterpret_cast<camera3_device_t*>(localDevice);

        NT_ASSERT(nullptr != mCamDevice, "No camera device is available!");

        NT_ASSERT(nullptr != GetCamDevice(), "Camera device is NULL for camera id %d", mCamId);

        NT_ASSERT(nullptr != mCamDevice->ops, "Camera device ops are NULL");

        mDeviceVersion = mCamDevice->common.version;

        // initialize camera
        NT_ASSERT(0 == mCamDevice->ops->initialize(mCamDevice, this), "Camera device initializing failed!");

        SetNumPartialResults();
        SetPipelineDepth();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SetNumPartialResults()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetNumPartialResults()
    {
        camera_info* pCamInfo = GetCameraInfoWithId(mCamId);
        NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", mCamId);
        camera_metadata_entry *partialResultCount = GetCameraMetadataEntry(pCamInfo, ANDROID_REQUEST_PARTIAL_RESULT_COUNT);

        NT_ASSERT(partialResultCount != nullptr, "Can't find partial result count in static metadata for camId: %d", mCamId);

        NT_ASSERT(partialResultCount->count != 0, "partial result count is zero in static metadata for camId: %d", mCamId);

        mPartialResultCount = partialResultCount->data.i32[0];
        NT_LOG_DEBUG("[CAMERAID %u] Number of partial results is: [%d]", mCamId, mPartialResultCount);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SetPipelineDepth()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetPipelineDepth()
    {
        camera_info* pCamInfo = GetCameraInfoWithId(mCamId);
        NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", mCamId);
        camera_metadata_entry *pipelineDepth = GetCameraMetadataEntry(pCamInfo, ANDROID_REQUEST_PIPELINE_MAX_DEPTH);

        NT_ASSERT(pipelineDepth != nullptr, "Can't find pipeline depth in static metadata for cameraId: %d", mCamId);

        NT_ASSERT(pipelineDepth->count != 0, "pipeline depth count is zero in static metadata for cameraId: %d", mCamId);

        mPipelineDepth = pipelineDepth->data.u8[0];

        NT_LOG_INFO("[CAMERAID %u] pipeline depth from static settings is: [%d]", mCamId, mPipelineDepth);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::CloseCamera()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::CloseCamera()
    {
        NT_LOG_INFO("[CAMERAID %u] Closing camera", mCamId);
        //Delete every component of PCRequestTracker safely
        PartialMetadataSet* pPartialSet = GetLastPartialMetadata();
        while (NULL != pPartialSet)
        {
            pPartialSet->metadata.ClearMetadata();
            delete pPartialSet;
            pPartialSet = GetLastPartialMetadata();
        }
        mMetadata.clear();
        if (mCamDevice != nullptr)
        {
            NT_ASSERT(0 == mCamDevice->common.close(reinterpret_cast<hw_device_t*>(mCamDevice))
                , "Can't close camera deivce %d", mCamId);
        }
        mPartialResultCount = 0;
        mPipelineDepth = 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::GetLastPartialMetadata()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Cam3Device::PartialMetadataSet* Cam3Device::GetLastPartialMetadata()
    {
        //change logic using tracker
        if (m_partialMetadataQueue.empty())
        {
            return NULL;
        }
        PartialMetadataSet* retVal = m_partialMetadataQueue.front();
        m_partialMetadataQueue.pop();
        return retVal;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::sNotify()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::sNotify(
        const camera3_notify_msg *msg)  //[out] notification message
    {

        NT_ASSERT(msg != nullptr, "HAL sent NULL notify message!");

        switch (msg->type)
        {
        case CAMERA3_MSG_ERROR:
        {
            camera3_error_msg_t errMessage = msg->message.error;
            // if error code number out of range, set it to point to ERROR_UNKNOWN
            if (errMessage.error_code >= CAMERA3_MSG_NUM_ERRORS)
            {
                errMessage.error_code = 0; // points to mErrorNotifyStrings[0] while printing
            }
            SetErrorNotifyFlag(errMessage.frame_number, errMessage.error_code);
            NT_LOG_ERROR("[CAMERAID %u] Notify error message %s (%d) for frame [%u] stream [%pK]",
                mCamId, mErrorNotifyStrings[errMessage.error_code], errMessage.error_code, errMessage.frame_number,
                errMessage.error_stream);
            break;
        }
        case CAMERA3_MSG_SHUTTER:
        {
            camera3_shutter_msg_t shutterMessage = msg->message.shutter;
            NT_LOG_INFO("[CAMERAID %u] Notify shutter message for frame [%u] timestamp %llu", mCamId, shutterMessage.frame_number,
                shutterMessage.timestamp);
            break;
        }
        default:
            NT_LOG_DEBUG("[CAMERAID %u] msg->type = %d", mCamId, msg->type);
            NT_ASSERT(1 == 0, "Unknown notify message from HAL.");
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::ClearErrorNotifyFlag()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::ClearErrorNotifyFlag(uint32_t frameNum)
    {
        mErrorNotifyBitFlag[frameNum % MAX_FRAME_REQUESTS] &= 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SetErrorNotifyFlag()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetErrorNotifyFlag(uint32_t frameNum, uint32_t pos)
    {
        if (pos <= 31)
        {
            mErrorNotifyBitFlag[frameNum % MAX_FRAME_REQUESTS] |= (1 << pos);
        }
        else
        {
            NT_LOG_ERROR("[CAMERAID %u] Invalid error notify flag position! %u", mCamId, pos);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::CheckErrorNotifyFlag()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::CheckErrorNotifyFlag(uint32_t frameNum, uint32_t pos) const
    {
        if (pos <= 31)
        {
            return mErrorNotifyBitFlag[frameNum % MAX_FRAME_REQUESTS] & (1 << pos);
        }
        else
        {
            NT_LOG_ERROR("[CAMERAID %u] Invalid error notify flag position! %u", mCamId, pos);
            return -1;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::GetCameraMetadataByFrame
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    camera_metadata_t * Cam3Device::GetCameraMetadataByFrame(
        uint32_t frameNumber) const //[in] framenumber for getting corresponding metadata
    {
        NT_LOG_INFO("[CAMERAID %u] Find metadata for frame: %d", mCamId, unsigned(frameNumber));
        if (mMetadata.find(frameNumber) == mMetadata.end())
        {
            return nullptr;
        }
        return mMetadata.find(frameNumber)->second.GetClonedCamMetadata();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::FindCameraMetadatafromTemplate
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::FindCameraMetadataFromTemplate(
        camera_metadata_t* templateRequest,  //[in] metadata object
        camera_metadata_tag_t tag) const           //[in] tag to be searched
    {
        camera_metadata_entry entry;
        int val = find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(templateRequest),
            tag, &entry);
        return val;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetCameraMetadataEntryByTag
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::GetCameraMetadataEntryByTag(
        camera_metadata_t* metadata,     //[in ] metadata object
        camera_metadata_tag_t tag,       //[in ] tag to be searched
        camera_metadata_entry_t *entry) const //[out] entry for the given tag
    {
        int val = find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(metadata),
            tag, entry);
        return val;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::GetCamDevice
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    camera3_device_t* Cam3Device::GetCamDevice() const
    {
        if (mCamDevice != nullptr)
        {
            return mCamDevice;
        }
        NT_LOG_ERROR("[CAMERAID %u] Cannot get camera device with id: %d", mCamId, mCamId);
        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::GetCamDevice
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Cam3Device::GetPartialResultCount() const
    {
        return mPartialResultCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::GetPipelineDepth
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::GetPipelineDepth() const
    {
        return static_cast<int>(mPipelineDepth);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::IF3AConverged
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::IfAEConverged(
        camera_metadata_t* metadata)  //[in] metadata of current frame
    {
        camera_metadata_entry_t entry;

        int ret = find_camera_metadata_entry(metadata, ANDROID_CONTROL_AE_STATE, &entry);
        if (ret == 0 && entry.count > 0)
        {

            uint8_t aeState = entry.data.u8[0];
            if (aeState == ANDROID_CONTROL_AE_STATE_CONVERGED ||
                aeState == ANDROID_CONTROL_AE_STATE_LOCKED)
            {

                isAEConverged = true;
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::IfAFConverged
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::IfAFConverged(
        camera_metadata_t* metadata) //[in] metadata of current frame
    {
        camera_metadata_entry_t entry;

        // Make sure the candidate frame has good focus.
        int ret = find_camera_metadata_entry(metadata, ANDROID_CONTROL_AF_STATE, &entry);
        if (ret == 0 && entry.count > 0)
        {
            uint8_t afState = entry.data.u8[0];
            if (afState == ANDROID_CONTROL_AF_STATE_PASSIVE_FOCUSED ||
                afState == ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED ||
                afState == ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED)
            {
                isAFConverged = true;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::IFAWBConverged
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::IfAWBConverged(
        camera_metadata_t* metadata)  //[in] metadata of current frame
    {
        camera_metadata_entry_t entry;

        int ret = find_camera_metadata_entry(metadata, ANDROID_CONTROL_AWB_STATE, &entry);
        if (ret == 0 && entry.count > 0)
        {
            uint8_t awbState = entry.data.u8[0];
            if (awbState == ANDROID_CONTROL_AWB_STATE_CONVERGED ||
                awbState == ANDROID_CONTROL_AWB_STATE_LOCKED)
            {
                isAWBConverged = true;
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Cam3Device::getJpegBufferSize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Cam3Device::getJpegBufferSize(
        uint32_t width,          //[in] width of jpeg image
        uint32_t height) const   //[in] height of jpeg image
    {
        int32_t maxJpegBufferSize;
        std::map<int, std::vector<Size>> fmtSizesMap = StaticMetadata::GetAvailableFormatSizesMap(*mCamInfo);
        Size maxJpegResolution = Size(0, 0);
        if (fmtSizesMap.size() > 0)
        {
            maxJpegResolution = (fmtSizesMap[HAL_PIXEL_FORMAT_BLOB].size() > 0) ? fmtSizesMap[HAL_PIXEL_FORMAT_BLOB].at(0) :
                Size(0, 0);
        }
        if (maxJpegResolution.width == 0 || maxJpegResolution.height == 0)
        {
            NT_LOG_WARN("[CAMERAID %u] Can't find valid available jpeg sizes in static metadata!", mCamId);
            return -1;
        }

        maxJpegBufferSize = StaticMetadata::GetJPEGMaxSize(*mCamInfo);
        if (maxJpegBufferSize == -1)
        {
            NT_LOG_WARN("[CAMERAID %u] Can't find the metadata entry for ANDROID_JPEG_MAX_SIZE!", mCamId);
            return -1;
        }

        float scaleFactor = static_cast<float>(width * height) / (maxJpegResolution.width * maxJpegResolution.height);
        size_t jpegBufferSize =
            static_cast<size_t>(scaleFactor *  (maxJpegBufferSize - kMinJpegBufferSize) + kMinJpegBufferSize);

        if (!HalCmdLineParser::isQCFASize() || !StaticMetadata::isQCFASensor(*mCamInfo, vTag))
        {
            if (jpegBufferSize > static_cast<size_t>(maxJpegBufferSize))
            {
                jpegBufferSize = static_cast<size_t>(maxJpegBufferSize);
            }
        }

        return jpegBufferSize;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// aprocess_capture_result
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void aprocess_capture_result(const camera3_callback_ops *cb, const camera3_capture_result *result)
    {
        Cam3Device *device = const_cast<Cam3Device*>(static_cast<const Cam3Device*>(cb));
        device->ProcessCaptureResult(result);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anotify
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void anotify(
        const camera3_callback_ops *cb,  //[in] fn pointer to callback methods
        const camera3_notify_msg *msg)   //[in] message notification
    {
        Cam3Device *device = const_cast<Cam3Device*>(static_cast<const Cam3Device*>(cb));
        device->sNotify(msg);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Camera3Device::AddMetadatatoMap
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::AddMetadatatoMap(
        const camera3_capture_result *resultMeta) //[in] metadata to be stored in map
    {
        //Change logic to add metadata to struct
        uint32_t frameNumber = resultMeta->frame_number;
        if (mMetadata.find(frameNumber) == mMetadata.end())
        {
            NT_LOG_INFO("[CAMERAID %u] Writing new metadata for frame: %d", mCamId, unsigned(frameNumber));
            mMetadata.insert(std::pair<uint32_t, Camera3Metadata>(frameNumber, Camera3Metadata(resultMeta->result)));
        }
        else
        {
            NT_LOG_INFO("[CAMERAID %u] Appending metadata for frame: %d", mCamId, unsigned(frameNumber));
            mMetadata.find(frameNumber)->second.Append(resultMeta->result);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::GetCameraId
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Cam3Device::GetCameraId() const
    {
        return mCamId;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::GetMasterCameraOfLogical
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string Cam3Device::GetMasterCameraOfLogical() const
    {
        return masterCameraOfLogical;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::GetSlaveCameraOfLogical
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string Cam3Device::GetSlaveCameraOfLogical() const
    {
        return slaveCameraOfPhysical;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::GetMaxSize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Size Cam3Device::GetMaxSize(
        int format,                     //[in] format of the image
        camera_info* camInfo,
        bool usePixelArraySize) const
    {
        if (usePixelArraySize)
        {
            return StaticMetadata::GetPixelArraySize(*camInfo);
        }
        Size maxRes = Size(0, 0);
        std::map<int, std::vector<Size>> fmtSizesMap = StaticMetadata::GetAvailableFormatSizesMap(*camInfo);
        if (fmtSizesMap.size() > 0)
        {
            maxRes = fmtSizesMap[format].size() > 0 ? fmtSizesMap[format].at(0) : Size(0, 0);
        }
        return maxRes;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::CheckSupportedResolution
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::CheckSupportedResolution(
        int format,             //[in] format to be checked
        Size resolution,        //[in] resolution to be checked
        int direction) const    //[in] direction of the stream
    {
        camera_info info;
        if (0 != GetCamModule()->get_camera_info(mCamId, &info))
        {
            NT_LOG_ERROR("[CAMERAID %u] Can't get camera info for camera id", mCamId);
            return false;
        }
        camera_metadata_entry_t entry;
        camera_metadata_tag_t streamConfigTag = (mbDepthTest) ? ANDROID_DEPTH_AVAILABLE_DEPTH_STREAM_CONFIGURATIONS :
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS;
        int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            streamConfigTag,
            &entry);
        if (val != 0)
        {
            NT_LOG_ERROR("[CAMERAID %u] Can't find the metadata entry for %s.", mCamId, (mbDepthTest) ?
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
                NT_LOG_ERROR("[CAMERAID %u] Invalid stream direction: %d", mCamId, direction);
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::SetQueriedMaxResolution()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetQueriedMaxResolution(
        int cameraId,           //[in] camera for which to query max resolution for
        StreamItem streams[],   //[in] streams that will have their resolution checked
        int numStreams)         //[in] number of streams to in stream array
    {
        // Overwrite RAW or DEPTH resolution to queried maximum
        for (int index = 0; index < numStreams; index++)
        {
            if ((streams[index].resolution.width == MAX_RES.width && streams[index].resolution.height == MAX_RES.height) ||
                (streams[index].resolution.width == DEPTH_RES.width && streams[index].resolution.height == DEPTH_RES.height) ||
                streams[index].format == HAL_PIXEL_FORMAT_RAW16 ||
                streams[index].format == HAL_PIXEL_FORMAT_RAW10)
            {
                camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
                NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);
                streams[index].resolution = GetMaxSize(streams[index].format, pCamInfo, false);
                if (streams[index].resolution.width == 0 && streams[index].resolution.height == 0)
                {
                    NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        streams[index].format, ConvertResToString(streams[index].resolution).c_str(), cameraId);
                }
                else
                {
                    NT_LOG_UNSUPP("Max resolution for cameraId: [%d] and format: [%d] is [%s]", cameraId, streams[index].format,
                        ConvertResToString(streams[index].resolution).c_str());
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::CheckAvailableCapability
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::CheckAvailableCapability(
        int cameraId,  //[in] camera id
        camera_metadata_enum_android_request_available_capabilities capability) const  //[in] capability to check
    {
        camera_info info;
        if (0 != GetCamModule()->get_camera_info(cameraId, &info))
        {
            NT_LOG_ERROR("Can't get camera info for camera id: %d", cameraId);
            return false;
        }
        camera_metadata_entry_t entry;
        int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES,
            &entry);
        if (val != 0)
        {
            NT_LOG_ERROR("Can't find the metadata entry for ANDROID_REQUEST_AVAILABLE_CAPABILITIES.");
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SetDepthTest
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetDepthTest(bool bDepthTest)
    {
        mbDepthTest = bDepthTest;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SetActivePhysCamCheck
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetActivePhysCamCheck(bool getActivePhysCam)
    {
        mGetActivePhysCam = getActivePhysCam;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CreateStream()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::CreateStream(
        int streamType,                         //[in] stream type specifies if stream is O/P, I/P or bidirectional type
        Size resolution,                        //[in] resolution of the stream
        uint32_t usage,                         //[in] usage flag required by gralloc
        int format,                             //[in] format of the stream
        camera3_stream_rotation_t rotation,     //[in] rotation of camera
        int dataspace,                          //[in] dataspace, defaults to -1 if none provided
        std::string physicalCameraId)                   //[in] physicalCameraId, defaults to null if none provided
    {
        NATIVETEST_UNUSED_PARAM(physicalCameraId);
        std::shared_ptr<camera3_stream_t> newStream(std::make_shared<camera3_stream_t>());

        newStream->stream_type = streamType;
        newStream->width = resolution.width;
        newStream->height = resolution.height;
        newStream->format = format;

        // If dataspace was provided...
        if (-1 != dataspace)
        {
            newStream->data_space = static_cast<android_dataspace_t>(dataspace);
        }
        // Otherwise use a default dataspace according to format
        else
        {
            switch (format)
            {
            case HAL_PIXEL_FORMAT_RAW16:
            case HAL_PIXEL_FORMAT_RAW12:
            case HAL_PIXEL_FORMAT_RAW10:
            case HAL_PIXEL_FORMAT_RAW_OPAQUE:
                newStream->data_space = HAL_DATASPACE_ARBITRARY;
                break;
            case HAL_PIXEL_FORMAT_BLOB:
                newStream->data_space = HAL_DATASPACE_V0_JFIF;
                break;
            case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
#ifdef _LINUX
                newStream->data_space = HAL_DATASPACE_UNKNOWN;
                break;
#endif
            case HAL_PIXEL_FORMAT_YCbCr_420_888:
            case HAL_PIXEL_FORMAT_Y16:
                newStream->data_space = HAL_DATASPACE_STANDARD_BT709;
                break;
            default:
                NT_ASSERT(1 == 0, "Invalid stream format");
            }
        }

        newStream->rotation = rotation;
        newStream->usage = usage;
        newStream->max_buffers = 0;
        newStream->priv = nullptr;
        mStreams.push_back(newStream);
        if (isHighspeedSession)
        {
            allocatedStreamsforHFR.push_back(std::move(newStream));
        }

        NT_ASSERT(!mStreams.empty(), "stream vector is empty.");
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::ConfigStream
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::ConfigStream(uint32_t opmode, bool setResourceCostCheck, bool enableRTB)
    {
        NATIVETEST_UNUSED_PARAM(enableRTB);
        config = {};  // Ensure all new members are 0 initialized (session_parameters)
        config.operation_mode = opmode;

        if (mStreams.empty())
        {
            NT_LOG_ERROR("[CAMERAID %d] mStreams is empty", mCamId);
        }

        config.num_streams = static_cast<uint32_t>(mStreams.size());
        config.streams = SAFE_NEW() camera3_stream_t*[config.num_streams];  //including input and output
        for (int i = 0; i < static_cast<int>(mStreams.size()); i++)
        {
            //Get raw pointer from vector of sp and assign to config.streams
            config.streams[i] = mStreams.at(i).get();
        }

        char sectionName[] = "org.codeaurora.qcamera3.sessionParameters";
        if (setResourceCostCheck)
        {
            char tagName[] = "overrideResourceCostValidation";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("overrideResourceCostValidation vendor tag not available");
                return -1;
            }
            unsigned char overrideResourceCost = 1;
            NT_LOG_DEBUG("[CAMERAID %d] Overriding resource cost validation!", mCamId);
            UpdateSessionParameters(tagId, &overrideResourceCost, 1);
        }

        if (HalJsonParser::sMCBOnly)
        {
            // master cb
            char tagName[] = "EnableMCXMasterCb"; // planned for deprecation

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableMCXMasterCb vendor tag not available");
                return -1;
            }

            INT32 masterCb = 1;
            NT_LOG_INFO("Enabling Master Call back!");
            UpdateSessionParameters(tagId, &masterCb, 1);

            char yuvTagName[] = "McxYuvCallbackInfo";
            char rawTagName[] = "McxRawCallbackInfo";

            uint32_t yuvTagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, yuvTagName, sectionName);
            uint32_t rawTagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, rawTagName, sectionName);


            unsigned char mcxCbModeMasterOnly = 2; //McxCbModeMasterOnly enum in ChiMcxCbMode
            NT_LOG_DEBUG("[CAMERAID %d] setting session parameter mcxCbModeMasterOnly!", mCamId);

            if (yuvTagId == UINT32_MAX)
            {
                NT_LOG_WARN("McxYuvCallbackInfo vendor tag not available");
                return -1;
            }
            else
            {
                UpdateSessionParameters(yuvTagId, &mcxCbModeMasterOnly, 1);
            }

            if (rawTagId == UINT32_MAX)
            {
                NT_LOG_WARN("McxRawCallbackInfo vendor tag not available");
                return -1;
            }
            else
            {
                UpdateSessionParameters(rawTagId, &mcxCbModeMasterOnly, 1);
            }
        }

        if (HalJsonParser::sNumExposures > 0) {
            char tagName[] = "numHDRexposure";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("numHDRexposure vendor tag not available");
                return -1;
            }

            NT_LOG_INFO("Enabling NumExposures Session Parameter!");
            UpdateSessionParameters(tagId, &HalJsonParser::sNumExposures, 1);
        }

        if (HalJsonParser::sAutoHDR) {
            char tagName[] = "EnableAutoHDR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableAutoHDR vendor tag not available");
                return -1;
            }

            INT32 autohdr = 1;
            NT_LOG_INFO("Enabling Auto HDR Session Parameter!");
            UpdateSessionParameters(tagId, &autohdr, 1);
        }

        if (HalJsonParser::sQHDR) {
            char tagName[] = "EnableQHDR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableQHDR vendor tag not available");
                return -1;
            }

            INT32 qhdr = 1;
            NT_LOG_INFO("Enabling QHDR Session Parameter!");
            UpdateSessionParameters(tagId, &qhdr, 1);
        }

        if (HalJsonParser::sMFHDR)
        {
            char tagName[] = "EnableMFHDR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableMFHDR vendor tag not available");
                return -1;
            }

            INT32 mfhdr = 1;
            NT_LOG_INFO("Enabling MFHDR Session Parameter!");
            UpdateSessionParameters(tagId, &mfhdr, 1);
        }

        if (HalJsonParser::sMFNR)
        {
            char tagName[] = "enableMFNR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("enableMFNR vendor tag not available");
                return -1;
            }

            INT32 mfnr = 1;
            NT_LOG_INFO("Enabling MFNR session parameter!");
            UpdateSessionParameters(tagId, &mfnr, 1);
        }

        if (HalJsonParser::sSHDR)
        {
            char tagName[] = "EnableSHDR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableSHDR vendor tag not available");
                return -1;
            }

            INT32 shdr = 1;
            NT_LOG_INFO("Enabling SHDR Session Parameter!");
            UpdateSessionParameters(tagId, &shdr, 1);
        }

        NT_LOG_INFO("[CAMERAID %d] Configuring %d streams ...", mCamId, config.num_streams);
        int res = mCamDevice->ops->configure_streams(mCamDevice, &config);
        delete[] config.streams;
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::ConfigStreamSingle
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::ConfigStreamSingle(bool isConstrainedHighSpeedConfiguration, int streamIndex, bool generateBuffers,
        bool setResourceCostCheck, bool enableRTB)
    {
        NATIVETEST_UNUSED_PARAM(generateBuffers);
        NATIVETEST_UNUSED_PARAM(enableRTB);
        config = {};  // Ensure all new members are 0 initialized (session_parameters)
        config.operation_mode = isConstrainedHighSpeedConfiguration ?
            CAMERA3_STREAM_CONFIGURATION_CONSTRAINED_HIGH_SPEED_MODE :
            CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE;


        if (mStreams.empty())
        {
            NT_LOG_ERROR("[CAMERAID %d] mStreams is empty", mCamId);
        }

        config.num_streams = 1;
        config.streams = SAFE_NEW() camera3_stream_t*[config.num_streams]; //including input and output;

        config.streams[0] = mStreams.at(streamIndex).get();

        char sectionName[] = "org.codeaurora.qcamera3.sessionParameters";
        if (setResourceCostCheck)
        {
            char tagName[] = "overrideResourceCostValidation";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("overrideResourceCostValidation vendor tag not available");
                return -1;
            }
            unsigned char overrideResourceCost = 1;
            NT_LOG_DEBUG("[CAMERAID %d] Overriding resource cost validation!", mCamId);
            UpdateSessionParameters(tagId, &overrideResourceCost, 1);
        }

        if (HalJsonParser::sMFHDR)
        {
            char tagName[] = "EnableMFHDR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableMFHDR vendor tag not available");
                return -1;
            }

            INT32 mfhdr = 1;
            NT_LOG_INFO("Enabling MFHDR Session Parameter!");
            UpdateSessionParameters(tagId, &mfhdr, 1);
        }

        if (HalJsonParser::sMFNR)
        {
            char tagName[] = "enableMFNR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("enableMFNR vendor tag not available");
                return -1;
            }

            INT32 mfnr = 1;
            NT_LOG_INFO("Enabling MFNR session parameter!");
            UpdateSessionParameters(tagId, &mfnr, 1);
        }

        if (HalJsonParser::sSHDR)
        {
            char tagName[] = "EnableSHDR";

            uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);

            if (tagId == UINT32_MAX)
            {
                NT_LOG_WARN("EnableSHDR vendor tag not available");
                return -1;
            }

            INT32 shdr = 1;
            NT_LOG_INFO("Enabling SHDR Session Parameter!");
            UpdateSessionParameters(tagId, &shdr, 1);
        }

        int res = mCamDevice->ops->configure_streams(mCamDevice, &config);
        delete[] config.streams;
        return res;

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::UpdateSessionParameters
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::UpdateSessionParameters(
        uint32_t tag,       //[in] tag to update data for
        const void* value,  //[in] pointer to data to write to tag
        size_t dataCount)   //[in] number of data pieces to write
    {
        Camera3Metadata meta = Camera3Metadata(config.session_parameters);

        NT_ASSERT(value != nullptr, "Key value is null");

        int type = get_camera_metadata_tag_type(tag);
        if (type == -1)
        {
            NT_ASSERT(1 == 0, "Unknown tag type at UpdateKey");
            return;
        }

        size_t bufferSize = get_camera_metadata_size(const_cast<camera_metadata_t*>(config.session_parameters));

        // Safety check - ensure that data isn't pointing to this metadata, since
        // that would get invalidated if a resize is needed
        uintptr_t bufAddr = reinterpret_cast<uintptr_t>(const_cast<camera_metadata_t*>(config.session_parameters));
        uintptr_t dataAddr = reinterpret_cast<uintptr_t>(value);
        if (dataAddr > bufAddr && dataAddr < (bufAddr + bufferSize)) {
            NT_ASSERT(1 == 0, "Update attempted with data from the same metadata buffer!");
            return;
        }

        size_t data_size = calculate_camera_metadata_entry_data_size(type, dataCount);
        meta.ResizeIfNeeded(1, data_size);

        // Check if current field exists in the metadata
        camera_metadata_entry_t targetEntry;
        int val = find_camera_metadata_entry(
            reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
            tag, &targetEntry);

        if (val != 0)
        {
            int res;
            NT_LOG_DEBUG("add metadata key: %u", tag);
            res = add_camera_metadata_entry(reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
                tag,
                reinterpret_cast<const void*> (value),
                dataCount);
            NT_ASSERT(res == 0, "add_camera_metadata_entry failed");
        }
        else
        {
            int res;
            NT_LOG_DEBUG("Update metadata key: %u", tag);
            res = update_camera_metadata_entry(reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
                targetEntry.index,
                reinterpret_cast<const void*> (value),
                dataCount,
                nullptr);
            NT_ASSERT(res == 0, "update_camera_metadata_entry failed");
        }

        if (config.session_parameters != nullptr)
        {
            free_camera_metadata(const_cast<camera_metadata_t *>(config.session_parameters));
        }
        config.session_parameters = clone_camera_metadata(const_cast<const camera_metadata_t *> (meta.GetCamMetadata()));
        meta.ClearMetadata();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::GetLastProcessedFrameNumber
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::GetLastProcessedFrameNumber()
    {
        return lastProcessedFrameNumber;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::FlushStream
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::FlushStream()
    {
        NT_LOG_DEBUG("[CAMERAID %d] FlushStream..", mCamId);
        int res = mCamDevice->ops->flush(mCamDevice);
        NT_ASSERT(res == 0, "flush failed");
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::TestFlush()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::TestFlush()
    {
        NT_LOG_INFO("[CAMERAID %d] Testing flush with delay : %d ", mCamId, HalCmdLineParser::g_flushDelay);
        std::this_thread::sleep_for(std::chrono::microseconds(HalCmdLineParser::g_flushDelay));

        auto flushStartTime = std::chrono::steady_clock::now();
        int res = mCamDevice->ops->flush(mCamDevice);
        auto flushEndTime = std::chrono::steady_clock::now();
        double flushTime = std::chrono::duration<double, std::milli>(flushEndTime - flushStartTime).count();
        NT_LOG_INFO("[CAMERAID %d] KPI: flush return time took %f ms", mCamId, flushTime);
        NT_EXPECT(flushTime <= 1000.0, "Flush call took longer than maximum timing requirement of 1 second!");
        return res;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CreateMultiStreams()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::CreateMultiStreams(
        Size outResolutions[],   //[in] Array of resolutions corresponding to each stream
        int outputFormats[],     //[in] Array of formats corresponding to each stream
        int numStreams,          //[in] Number of streams to be configured
        uint32_t usageFlags[],   //[in] Array of usage flags corresponding to each stream
        int streamTypes[])       //[in] Array of stream type corresponding to each stream
    {
        for (int i = 0; i < numStreams; i++)
        {
            CreateStream(streamTypes[i], outResolutions[i], usageFlags[i], outputFormats[i], CAMERA3_STREAM_ROTATION_0);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CheckType()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::CheckType(
        uint32_t tag,           //[in] tag to check type of
        uint8_t expectedType)   //[in] type of data that is expected for the tag
    {
        int type = get_camera_metadata_tag_type(tag);
        if (type == -1)
        {
            NT_LOG_ERROR("[CAMERAID %d] Tag not found: %d", mCamId, tag);
            return false;
        }

        if ((type != expectedType))
        {
            NT_LOG_ERROR("[CAMERAID %d] Mismatched tag type: %d Expected type is: %d, got type: %d", mCamId, tag, static_cast<int>(expectedType), type);
            return false;
        }

        return true;

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::SetKey()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetKey(
        uint32_t tag,           //[in] tag to set data for
        const int32_t * value,  //[in] pointer to data to write for the given tag
        size_t dataCount)       //[in] number of data pieces to write
    {
        NT_ASSERT(CheckType(tag, TYPE_INT32), "CheckType failed");
        UpdateKey(tag, static_cast<const void*>(value), dataCount);
    }

    void Cam3Device::SetKey(uint32_t tag, const uint8_t * value, size_t dataCount)
    {

        NT_ASSERT(CheckType(tag, TYPE_BYTE), "CheckType failed");
        UpdateKey(tag, static_cast<const void*>(value), dataCount);
    }

    void Cam3Device::SetKey(uint32_t tag, const float * value, size_t dataCount)
    {

        NT_ASSERT(CheckType(tag, TYPE_FLOAT), "CheckType failed");
        UpdateKey(tag, static_cast<const void*>(value), dataCount);
    }

    void Cam3Device::SetKey(uint32_t tag, const int64_t * value, size_t dataCount)
    {

        NT_ASSERT(CheckType(tag, TYPE_INT64), "CheckType failed");
        UpdateKey(tag, static_cast<const void*>(value), dataCount);
    }

    void Cam3Device::SetKey(uint32_t tag, const double * value, size_t dataCount)
    {

        NT_ASSERT(CheckType(tag, TYPE_DOUBLE), "CheckType failed");
        UpdateKey(tag, static_cast<const void*>(value), dataCount);
    }

    void Cam3Device::SetKey(uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount)
    {
        NT_ASSERT(CheckType(tag, TYPE_RATIONAL), "CheckType failed");
        UpdateKey(tag, static_cast<const void*>(value), dataCount);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::UpdateKey()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::UpdateKey(
        uint32_t tag,       //[in] tag to update data for
        const void* value,  //[in] pointer to data to write to tag
        size_t dataCount)   //[in] number of data pieces to write
    {
        NT_ASSERT(halRequest != nullptr, "No hal request object!");
        NT_ASSERT(halRequest->settings != nullptr, "No settings on hal request!");
        Camera3Metadata meta = Camera3Metadata(halRequest->settings);

        NT_ASSERT(value != nullptr, "Key value is null");

        int type = get_camera_metadata_tag_type(tag);
        if (type == -1)
        {
            NT_ASSERT(1 == 0, "Unknown tag type at UpdateKey");
            return;
        }

        size_t bufferSize = get_camera_metadata_size(const_cast<camera_metadata_t*>(halRequest->settings));

        // Safety check - ensure that data isn't pointing to this metadata, since
        // that would get invalidated if a resize is needed
        uintptr_t bufAddr = reinterpret_cast<uintptr_t>(const_cast<camera_metadata_t*>(halRequest->settings));
        uintptr_t dataAddr = reinterpret_cast<uintptr_t>(value);
        if (dataAddr > bufAddr && dataAddr < (bufAddr + bufferSize)) {
            NT_ASSERT(1 == 0, "Update attempted with data from the same metadata buffer!");
            return;
        }

        size_t data_size = calculate_camera_metadata_entry_data_size(type, dataCount);
        meta.ResizeIfNeeded(1, data_size);

        // Check if current field exists in the metadata
        camera_metadata_entry_t targetEntry;
        int val = find_camera_metadata_entry(
            reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
            tag, &targetEntry);

        if (val != 0)
        {
            int res;
            NT_LOG_DEBUG("[CAMERAID %d] add metadata key: %u", mCamId, tag);
            res = add_camera_metadata_entry(reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
                tag,
                reinterpret_cast<const void*> (value),
                dataCount);
            NT_ASSERT(res == 0, "add_camera_metadata_entry failed");
        }
        else
        {
            int res;
            NT_LOG_DEBUG("[CAMERAID %d] Update metadata key: %u", mCamId, tag);
            res = update_camera_metadata_entry(reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
                targetEntry.index,
                reinterpret_cast<const void*> (value),
                dataCount,
                nullptr);
            NT_ASSERT(res == 0, "update_camera_metadata_entry failed");
        }

        if (halRequest->settings != nullptr)
        {
            free_camera_metadata(const_cast<camera_metadata_t *>(halRequest->settings));
        }
        halRequest->settings = clone_camera_metadata(const_cast<const camera_metadata_t *> (meta.GetCamMetadata()));
        meta.ClearMetadata();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CheckKeySetInResult()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::CheckKeySetInResult(
        uint32_t tag,           //[in] tag to check in metadata
        const uint8_t *value,   //[in] expected data to validate in metadata
        uint32_t frameNum)      //[in] frame number of the capture result
    {
        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNum);
        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] Meta read from file is null", mCamId);
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %d] find_camera_metadata_entry returned ERROR or empty entry", mCamId);
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.u8[i] != value[i])
            {

                // Special handle for Video stabilization verification
                // It can return any available modes.
                if (tag == ANDROID_CONTROL_VIDEO_STABILIZATION_MODE)
                {
                    if (entry.data.u8[i] == ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_OFF ||
                        entry.data.u8[i] == ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_ON)
                    {
                        free(const_cast<camera_metadata_t*>(metaFromFile));
                        return true;
                    }
                }
                NT_LOG_DEBUG("[CAMERAID %d] Expected value: %d, got: %d", mCamId, static_cast<int>(value[i]), static_cast<int>(entry.data.u8[i]));
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }
        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Cam3Device::CheckKeySetInResult(uint32_t tag, const int32_t * value, float errorMarginRate, int frameNum)
    {
        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] Meta read from file is null", mCamId);
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %d] find_camera_metadata_entry returned ERROR or empty entry", mCamId);
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.i32[i] != value[i])
            {
                // check if within error margin
                int32_t delta = std::abs(value[i] - entry.data.i32[i]);
                int32_t errorMarginDelta = static_cast<int32_t>(errorMarginRate * value[i]);
                if (delta > errorMarginDelta)
                {
                    NT_LOG_DEBUG("[CAMERAID %d] Expected value: %d with margin delta: %d, got: %d", mCamId, static_cast<int>(value[i]), errorMarginDelta, static_cast<int>(entry.data.i32[i]));
                    free(const_cast<camera_metadata_t*>(metaFromFile));
                    return false;
                }
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Cam3Device::CheckKeySetInResult(uint32_t tag, const float * value, int frameNum)
    {

        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] Meta read from file is null", mCamId);
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %d] find_camera_metadata_entry returned ERROR or empty entry", mCamId);
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.f[i] != value[i])
            {
                NT_LOG_DEBUG("[CAMERAID %d] Expected value: %f, got: %f", mCamId, value[i], entry.data.f[i]);
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Cam3Device::CheckKeySetInResult(uint32_t tag, const int64_t * value, float errorMarginRate, int frameNum)
    {

        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] Meta read from file is null", mCamId);
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %d] find_camera_metadata_entry returned ERROR or empty entry", mCamId);
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.i64[i] != value[i])
            {
                // check if within error margin
                int64_t delta = std::abs(value[i] - entry.data.i64[i]);
                int64_t errorMarginDelta = static_cast<int64_t>(errorMarginRate * value[i]);

                // special case for validating exposure time
                if (tag == ANDROID_SENSOR_EXPOSURE_TIME)
                {
                    const long EXPOSURE_TIME_ERROR_MARGIN_NS = 100000L; // 100us, Approximation.
                    errorMarginDelta = (std::max(EXPOSURE_TIME_ERROR_MARGIN_NS, (long)(errorMarginRate * value[i])));
                }
                if (delta > errorMarginDelta)
                {
                    NT_LOG_ERROR("[CAMERAID %d] Expected value: %d with margin delta: %d, got: %d", mCamId, value[i], errorMarginDelta, entry.data.i64[i]);
                    free(const_cast<camera_metadata_t*>(metaFromFile));
                    return false;
                }
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Cam3Device::CheckKeySetInResult(uint32_t tag, const double * value, int frameNum)
    {
        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] Meta read from file is null", mCamId);
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %d] find_camera_metadata_entry returned ERROR or empty entry", mCamId);
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.d[i] != value[i])
            {
                NT_LOG_ERROR("[CAMERAID %d] Expected value: %d, got: %d", mCamId, value[i], entry.data.d[i]);
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Cam3Device::CheckKeySetInResult(uint32_t tag, const camera_metadata_rational_t * value, int frameNum)
    {
        const camera_metadata_t* metaFromFile = GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] Meta read from file is null", mCamId);
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("[CAMERAID %d] find_camera_metadata_entry returned ERROR or empty entry", mCamId);
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            // Note, rationals may be not simplified, and still equal to the requested value if
            // the exact values aren't the same
            int lhs = entry.data.r[i].numerator * value[i].denominator;
            int rhs = entry.data.r[i].denominator * value[i].numerator;

            if (lhs != rhs)
            {
                NT_LOG_ERROR("[CAMERAID %d] Expected Rational [%d, %d], got [%d, %d]", mCamId, value[i].numerator, value[i].denominator,
                    entry.data.r[i].numerator, entry.data.r[i].denominator);
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetGrallocSize()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Size Cam3Device::GetGrallocSize(
        Size resolution,         //[in] resolution of the image
        int outputFormat)        //[in] format of the image
    {
        Size grallocResolution;
        // Gralloc implementation maps Raw Opaque to BLOB
        if (outputFormat == HAL_PIXEL_FORMAT_BLOB ||
            outputFormat == HAL_PIXEL_FORMAT_RAW_OPAQUE)
        {
            Size defaultGrallocRes = Size(static_cast<uint32_t>(resolution.width * resolution.height * 3 + sizeof(camera3_jpeg_blob)), 1);

            size_t jpegBufferSize = getJpegBufferSize(resolution.width, resolution.height);
            if (jpegBufferSize == static_cast<size_t>(-1))
            {
                NT_LOG_WARN("[CAMERAID %d] Failed to calculate Jpeg gralloc size!", mCamId);
                return defaultGrallocRes;
            }

            grallocResolution = Size(jpegBufferSize, 1);
        }
        else
        {
            grallocResolution = resolution;
        }

        return grallocResolution;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SetEnableZsl()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetEnableZsl()
    {
        camera_metadata_entry_t entry;
        uint8_t setVal;

        // Ensure that capture intent is set to STILL_CAPTURE
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(halRequest->settings), ANDROID_CONTROL_CAPTURE_INTENT, &entry);
        NT_ASSERT(ret == 0, "GetCameraMetadataEntryByTag ANDROID_CONTROL_CAPTURE_INTENT failed");

        if (entry.count == 1 && entry.data.u8[0] == ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE)
        {
            NT_LOG_INFO("CAPTURE_INTENT is STILL_CAPTURE (%d), ENABLE_ZSL is effective", entry.data.u8[0]);
        }
        else
        {
            NT_LOG_WARN("CAPTURE_INTENT is %d, setting to STILL_CAPTURE to ensure that ENABLE_ZSL is effective", entry.data.u8[0]);
            setVal = ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE;
            SetKey(ANDROID_CONTROL_CAPTURE_INTENT, &setVal, 1);
        }

        // Set ENABLE_ZSL to true
        setVal = ANDROID_CONTROL_ENABLE_ZSL_TRUE;
        SetKey(ANDROID_CONTROL_ENABLE_ZSL, &setVal, 1);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::SendProcessCaptureRequest()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SendProcessCaptureRequest(int frameNumber, int requestType, int numStreams,
        int outputStreamIndex, std::map<int, std::vector<setMetadataTag>> setMap, bool enableZSL,
        std::vector<std::string> fileNameList, std::vector<std::string> physCamIdsList, std::vector<int> streamIndicesToRequest,
        bool zoom, std::vector<bool> dumpBufferList)
    {
        NATIVETEST_UNUSED_PARAM(physCamIdsList);
        std::unique_lock<std::mutex> halReqLock(halRequestMutex);
        NT_LOG_INFO("[CAMERAID %d] Creating a new HAL request for frame number %d", mCamId, frameNumber);
        CreateCaptureRequestWithMultBuffers(outputStreamIndex, requestType, numStreams, frameNumber, streamIndicesToRequest);

        if (halRequest == nullptr)
        {
            halReqLock.unlock();
            NT_ASSERT(false, "[CAMERAID %d] halRequest is nullptr for frameNumber %d", mCamId, frameNumber);
        }

        if (enableZSL)
        {
            SetEnableZsl();
        }

        std::vector<setMetadataTag> setMetadataList = setMap[frameNumber];
        for (size_t i = 0; i < setMetadataList.size(); i++)
        {
            UpdateKey(setMetadataList.at(i).tag, setMetadataList.at(i).data, setMetadataList.at(i).dataCount);
        }

        if (halRequest->output_buffers == nullptr)
        {
            DeleteHalRequest();
            halReqLock.unlock();
            NT_ASSERT(false, "[CAMERAID %d] Failed to get output buffer for frame number: %d", mCamId, frameNumber);
        }
        int res = -1;
        std::unique_ptr<RequestTracker_t> request = std::make_unique<RequestTracker_t>(halRequest->num_output_buffers,
            (nullptr != halRequest->input_buffer) ? 1 : 0, halRequest->num_output_buffers, mPartialResultCount);
        request->zoom = zoom;
        request->fileNameList = fileNameList;
        request->dumpBufferList = dumpBufferList;
        PCRTracker.insert(std::pair<int, std::unique_ptr<RequestTracker_t>>(frameNumber, std::move(request)));
        NT_LOG_INFO("[CAMERAID %d] sending process capture request to frameNumber %d", mCamId, frameNumber);
        res = mCamDevice->ops->process_capture_request(mCamDevice, halRequest);
        if (res != 0)
        {
            DeleteHalRequest();
            halReqLock.unlock();
            NT_ASSERT(res == 0, "[CAMERAID %d] process_capture_request failed for frame number: %d", mCamId, frameNumber);
        }
        NT_LOG_INFO("[CAMERAID %d] process capture request to frameNumber %d sent", mCamId, frameNumber);
        PCRTracker[frameNumber]->resultStatus = REQUEST_SENT;
        DeleteHalRequest();
        halReqLock.unlock();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::PCRStopCondition()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::PCRStopCondition(std::chrono::time_point<std::chrono::steady_clock> endTime, int frameNumber, int totalFrames)
    {
        if (HalJsonParser::sDuration == -1) // Go with --frames if set
        {
            if (frameNumber > totalFrames)
            {
                return true;
            }
        }
        else if (totalFrames == -1) // Go with --duration if set
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds> (endTime - std::chrono::steady_clock::now()).count() <= 0)
            {
                return true;
            }
        }
        else // Go with --frames if none are set
        {
            if (frameNumber > totalFrames)
            {
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::SendPCRByRequestType()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SendPCRByRequestType(int frameNumber,
        std::vector<streamRequestType> streamRequestType, int outputStreamIndex)
    {
        for (size_t index = 0; index < streamRequestType.size(); index++)
        {
            int requestType = streamRequestType[index].requestType;
            int numStreams = streamRequestType[index].numStreams;
            bool enableZSL = streamRequestType[index].enableZSL;
            std::map<int, std::vector<setMetadataTag>> setMap = streamRequestType[index].setMap;
            std::vector<std::string> fileNameList = streamRequestType[index].fileNameList;
            std::vector<std::string> physCamIdsList = streamRequestType[index].physCamIdsList;
            std::vector<int> streamIndicesToRequest = streamRequestType[index].streamIndicesToRequest;
            bool zoom = streamRequestType[index].zoom;
            std::vector<bool> dumpBufferList = streamRequestType[index].dumpBufferList;

            bool sendPCR = false;
            switch (streamRequestType[index].intervalType)
            {
            case MULTIPLE:
            {
                // Keep sending PCRs except for the skipFrame number
                if (streamRequestType[index].skipFrame > 0 && frameNumber % streamRequestType[index].skipFrame != 0)
                {
                    sendPCR = true;
                }
                // Send PCR with sendFrame number
                if (streamRequestType[index].sendFrame > 0 && frameNumber % streamRequestType[index].sendFrame == 0)
                {
                    sendPCR = true;
                }
                break;
            }
            case NOINTERVAL:
            {
                sendPCR = true;
                break;
            }
            case NHX:
            {
                for (int streamIdx = 0;  streamIdx < numStreams; streamIdx++)
                {
                    if (std::find(streamToPCRSendMap.at(streamIdx).begin(), streamToPCRSendMap.at(streamIdx).end(),
                        frameNumber) != streamToPCRSendMap.at(streamIdx).end())
                    {
                        NT_LOG_INFO("frame number %d includes stream %d", frameNumber, streamIdx);
                        streamIndicesToRequest.push_back(streamIdx);
                    }
                }
                numStreams = streamIndicesToRequest.size();
                sendPCR=TRUE;
                break;
            }
            default:
                NT_LOG_ERROR("Please provide a valid interval type [MULTIPLE, NOINTERVAL, NHX]");
                return;
            }

            if (sendPCR)
            {
                auto PCRThread = std::async(std::launch::async, [&] { SendProcessCaptureRequest(frameNumber,
                    requestType, numStreams, outputStreamIndex, setMap, enableZSL, fileNameList, physCamIdsList,
                    streamIndicesToRequest, zoom, dumpBufferList); });
                if (PCRThread.wait_for(std::chrono::milliseconds(5*DEFAULT_WAIT_TIME)) == std::future_status::timeout)
                {
                    NT_EXPECT(false, "Waited too long for hal request frame number %d", frameNumber);
                    PcrSuccess = FALSE;
                    stopPcrFlag = TRUE;
                    break;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::VerifyResults()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::VerifyResults(int frameNumber)
    {
        NT_LOG_INFO("[CAMERAID %d] Verifying results of frameNumber %d", mCamId, frameNumber);
        std::unique_lock<std::mutex> resThreadLock(resThreadMutex);
        Cam3Device::RequestTracker_t *request = GetRequestByFrameNumber(frameNumber);
        if (request != nullptr && request->resultStatus >= RESULTS_RECEIVED)
        {
            NT_LOG_INFO("[CAMERAID %d] Obtained all input/output buffers and metadata results for frameNumber %d", mCamId,
                frameNumber);
            if (HalCmdLineParser::g_vstats == 2 || HalCmdLineParser::g_vstats == 3)
            {
                bool ifAWBConverged = request->isAWBConverged;
                bool ifAFConverged = request->isAFConverged;
                bool ifAEConverged = request->isAEConverged;
                // Need to implement verification of stats based on 2A and 3A. Further discussion needed
                NATIVETEST_UNUSED_PARAM(ifAEConverged);
                NATIVETEST_UNUSED_PARAM(ifAFConverged);
                NATIVETEST_UNUSED_PARAM(ifAWBConverged);
            }
            lastProcessedFrameNumber = frameNumber;
            request = nullptr;
        }
        resThreadLock.unlock();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::DumpOutputBuffers()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::DumpOutputBuffers(buffer_handle_t* bufferHandle, int frameNumber, camera3_stream_t *camStream)
    {
        NT_LOG_INFO("[CAMERAID %d] Dumping output buffers for frameNumber %d", mCamId, frameNumber);
        std::unique_lock<std::mutex> dumpThreadLock(dumpThreadMutex);
        if (bufferHandle != nullptr)
        {
            std::string fileName = "";
            bool dumpBuffer = false;
            std::vector<std::string> fileNames = PCRTracker[frameNumber]->fileNameList;
            std::vector<bool> dumpBuffers = PCRTracker[frameNumber]->dumpBufferList;
            if (!fileNames.empty())
            {
                for (size_t streamId = 0; streamId < mStreams.size(); streamId++)
                {
                    if (AreCameraStreamsEqual(*mStreams[streamId], *camStream))
                    {
                        if (static_cast<int>(streamId) < PCRTracker[frameNumber]->nOutputBuffers)
                        {
                            fileName = fileNames[streamId];
                            dumpBuffer = dumpBuffers[streamId];
                        }
                        else
                        {
                            // This condition is mainly for tests that has more than one streams configured. The filenames and
                            // dumpbuffers list don't match one to one to the streams requested. For example:
                            // Say, 6 streams are configured with indices: 0,1,2,3,4,5 and capture requests are sent containing
                            // output buffers with only these stream indices 3,4,5. The filenames and dumpbuffers list are filled
                            // with indices 0,1,2 (total size of 3 each). So, in this case, the mapping is not one-to-one.
                            // Therefore, we can fetch the filename and dumpbuffer with the formula:
                            // num_of_fileNames-(num_of_configured_streams-stream_index) and
                            // num_of_dumpBuffers-(num_of_configure_streams-stream_index).
                            // So, if stream with index 4 is requested, we can fetch the second fileName and dumpBuffer as
                            // 3-(6-4) = 1.
                            fileName = fileNames[fileNames.size() - (mStreams.size() - streamId)];
                            dumpBuffer = dumpBuffers[dumpBuffers.size() - (mStreams.size() - streamId)];
                        }
                        break;
                    }
                }
            }
            std::string outputName = ConstructOutputFileName(mCamId, camStream->format, frameNumber, fileName.c_str());
            /* START critical section - native buffer map */
            std::unique_lock<std::mutex> nativeBufferLock(nativeBufferMutex);
            if (nativeBuffers.find(camStream) != nativeBuffers.end())
            {
                if (nativeBuffers[camStream].find(bufferHandle) != nativeBuffers[camStream].end())
                {
                    if (dumpBuffer)
                    {
                        NT_LOG_INFO("[CAMERAID %d] Dumping output buffer frameNumber %d", mCamId, frameNumber);
                        DumpBuffer(camStream, bufferHandle, frameNumber, outputName.c_str());
                    }
                    else
                    {
                        NT_LOG_INFO("[CAMERAID %d] Not dumping output buffer for frame number %d", mCamId, frameNumber);
                    }
#ifdef _LINUX
                    nativeBuffers[camStream].at(bufferHandle).outNativeBuffer->clear();
#else
                    nativeBuffers[camStream].at(bufferHandle).outNativeBuffer->get()->FreeBuffer();
#endif //_LINUX
                    nativeBuffers[camStream].erase(bufferHandle);
                    nativeBufferLock.unlock();
                    /* END critical section - native buffer map */
                }
                else
                {
                    NT_LOG_ERROR("[CAMERAID %d] Unknown buffer handle. Could not dump output buffer for frame number %d",
                        mCamId, frameNumber);
                    PcrSuccess = FALSE;
                    stopPcrFlag = TRUE;
                    nativeBufferLock.unlock();
                    /* END critical section - native buffer map */
                    return;
                }
            }
        }
        bufferHandle = nullptr;
        dumpThreadLock.unlock();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::DoesBufferSizeExceedPipelineDepth()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::DoesBufferSizeExceedPipelineDepth(int frameNumber)
    {
        NATIVETEST_UNUSED_PARAM(frameNumber);
        for (size_t i = 0; i < mStreams.size(); i++)
        {
            std::unique_lock<std::mutex> nativeBuffersLock(nativeBufferMutex);
            int natBufSize = static_cast<int>(nativeBuffers[mStreams[i].get()].size());
            nativeBuffersLock.unlock();
            if (natBufSize >= static_cast<int>(mPipelineDepth))
            {
                return false;
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::ProcessCaptureRequestAsync()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::ProcessCaptureRequestAsync(std::vector<streamRequestType> streamRequestType, int outputStreamIndex,
        int totalFrames, int frameNumber)
    {
        std::chrono::time_point<std::chrono::steady_clock> mLastPcrTime;
        PCRTracker.clear();
        nativeBuffers.clear();
        stopPcrFlag = FALSE;
        PcrSuccess = TRUE;
        std::chrono::time_point<std::chrono::steady_clock> endTime;
        while (stopPcrFlag == FALSE)
        {
            if (DoesBufferSizeExceedPipelineDepth(frameNumber))
            {
                for (auto const& f : mReconfigureFramesMap)
                {
                    if (f.second == REQUEST_SENT) {
                        NT_LOG_INFO("Adding frame: %d to probable flush list, as reconfigure frame is still with driver",
                            frameNumber);
                        mFlushFramesSet.insert(frameNumber);
                        break;
                    }
                }

                if (mReconfigureFramesMap.find(frameNumber) != mReconfigureFramesMap.end())
                {
                    mReconfigureFramesMap.at(frameNumber) = REQUEST_SENT;
                    UpdateFlushFramesSet(frameNumber);
                }

                SendPCRByRequestType(frameNumber, streamRequestType, outputStreamIndex);
                mLastPcrTime = std::chrono::steady_clock::now();
                if (PCRTracker.size() == 1)
                {
                    endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(HalJsonParser::sDuration);
                }
                frameNumber++;
            }
            else
            {
                if (std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - mLastPcrTime).count() >
                    5 * DEFAULT_WAIT_TIME)
                {
                    PcrSuccess = FALSE;
                    break;
                }
                std::vector<int> frameNumbers = GetUnprocessedFramenumbers();
                int unProcessedFrame = frameNumbers.at(0);
                NT_LOG_INFO("[CAMERAID %d] Waiting result for frame number %d", mCamId, unProcessedFrame);
                std::this_thread::sleep_for(std::chrono::milliseconds(33));
            }
            //Stop sending PCRs when duration is reached
            if (PCRStopCondition(endTime, frameNumber, totalFrames))
            {
                NT_LOG_INFO("[CAMERAID %d] Stop condition reached! Stopping sending PCRs...", mCamId);
                stopPcrFlag = TRUE;
                break;
            }
        }
        if (PcrSuccess)
        {
            if (stopPcrFlag)
            {
                UINT perFrameTrackTime = 0;
                int unProcessedFrame = -1;
                std::vector<int> frameNumbers;
                auto timeAfterStop = std::chrono::steady_clock::now();
                while (GetUnprocessedFramenumbers().size() > 0)
                {
                    frameNumbers = GetUnprocessedFramenumbers();
                    unProcessedFrame = (frameNumbers.size() > 0) ? frameNumbers.at(0) : -1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(33)); //Sleep for 33 milliseconds
                    perFrameTrackTime = perFrameTrackTime + 33;
                    if (std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - timeAfterStop).count() >
                        DEFAULT_WAIT_TIME * 2)
                    {
                        NT_LOG_ERROR("[CAMERAID %d] Timed out waiting for results", mCamId);
                        PcrSuccess = FALSE;
                        break;
                    }
                    if (unProcessedFrame != -1)
                    {
                        if (PCRTracker[unProcessedFrame]->resultStatus >= RESULTS_RECEIVED)
                        {
                            NT_LOG_INFO("[CAMERAID %d] Waited for results for frame number %d for %d milliseconds", mCamId,
                                unProcessedFrame, perFrameTrackTime);
                            perFrameTrackTime = 0;
                        }
                    }
                }

                frameNumbers = GetUnprocessedFramenumbers();
                if (frameNumbers.size() > 0)
                {
                    unProcessedFrame = frameNumbers.at(0);

                    // finally when test timeout has happened, printing for last frame waited
                    if (perFrameTrackTime != 0 && PCRTracker[unProcessedFrame]->resultStatus >= RESULTS_RECEIVED)
                    {
                        NT_LOG_INFO("[CAMERAID %d] Waited for results for frame number %d for %d milliseconds", mCamId,
                            unProcessedFrame, perFrameTrackTime);
                        perFrameTrackTime = 0;
                    }
                }
            }
        }
        else
        {
            std::vector<int> frameNumbers = GetUnprocessedFramenumbers();
            for (size_t i = 0; i < frameNumbers.size(); i++)
            {
                NT_LOG_ERROR("[CAMERAID %d] Could not obtain all input/output buffers and metadata results for frameNumber %d", mCamId, frameNumbers[i]);
            }
        }
        NT_ASSERT(PcrSuccess == TRUE, "Timed out waiting buffer for camera ID %d", mCamId);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::SetStopPcrFlag()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetStopPcrFlag(bool stopPcr) {
        Cam3Device::stopPcrFlag = stopPcr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::InitHalRequest()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::InitHalRequest(
        int frameNumber,                        //[in] frame number
        camera3_stream_buffer_t * inputBuffer,  //[in] location of buffer where driver should get input from
        camera3_stream_buffer_t * outputBuffer, //[in] location of buffer where driver should output
        int num_output_buffers,                 //[in] number of output buffers
        int templateType)                       //[in] template to provide the default metadata settings
    {
        halRequest = SAFE_NEW() camera3_capture_request_t();
        halRequest->frame_number = frameNumber;
        halRequest->input_buffer = inputBuffer;
        halRequest->num_output_buffers = num_output_buffers;
        halRequest->output_buffers = outputBuffer;

        const camera_metadata_t *settings = mCamDevice->ops->construct_default_request_settings(mCamDevice, templateType);
        NT_ASSERT(NULL != settings, "Camera settings for the template type are NULL");
        halRequest->settings = clone_camera_metadata(settings);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::DeleteHalRequest()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::DeleteHalRequest()   //[in] camera Id for which to delete HAL request
    {
        if (halRequest != nullptr)
        {
            NT_LOG_DEBUG("[CAMERAID %d] Deleting HAL request..", mCamId);
            if (halRequest->settings != nullptr)
            {
                free_camera_metadata(const_cast<camera_metadata_t *>(halRequest->settings));
            }

            delete halRequest;
            halRequest = nullptr;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::DumpBuffer()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::DumpBuffer(camera3_stream_t *stream, buffer_handle_t *bufferHandle, int frameNumber, const char* fileName) const
    {
        if (HalCmdLineParser::g_noImageDump)
        {
            NT_LOG_WARN("[CAMERAID %d] Result image dump is disabled, skipping...", mCamId);
            return;
        }

        NT_ASSERT(fileName != nullptr, "filename is null");

        int res;

#ifdef _LINUX
        sp<GraphicBuffer> outNativeBuffer;
#else
        std::shared_ptr<CamxBuffer> outNativeBuffer;
#endif
        outNativeBuffer = *nativeBuffers.at(stream).at(bufferHandle).outNativeBuffer;
        if (outNativeBuffer == nullptr)
        {
            NT_LOG_ERROR("[CAMERAID %d] outNativeBuffer is NULL for frame number: [%d]", mCamId, frameNumber);
            return;
        }

        NT_LOG_DEBUG("[CAMERAID %d] Saving image to file: %s", mCamId, fileName);
#ifdef _LINUX

        char* outputBufferPtr = nullptr;

        res = outNativeBuffer->lock(GRALLOC_USAGE_SW_READ_OFTEN, reinterpret_cast<void**>(&outputBufferPtr));
        NT_ASSERT(res == 0, "outputNativeBuffer lock failed");

        // Ensure capture folder gets created or exists
        // Make upper camera folder first
        if (mkdir(sImageUpperPath.c_str(), 0777) != 0)
        {
            NT_ASSERT(EEXIST == errno, "Failed to create capture camera folder, Error: %d", errno);
        }

        // Make lower nativetest folder second
        if (mkdir(sImageRootPath.c_str(), 0777) != 0)
        {
            NT_ASSERT(EEXIST == errno, "Failed to create capture root folder, Error: %d", errno);
        }
#else
        buffer_handle_t* pBuffer = outNativeBuffer->GetNativeHandle();
        char* outputBufferPtr = *reinterpret_cast<char**>(const_cast<native_handle_t *>(*pBuffer)->data);

        // make sure capture folder gets created or exists
        if (_mkdir(sImageRootPath.c_str()) != 0)
        {
            DWORD error = GetLastError();
            NT_ASSERT(ERROR_ALREADY_EXISTS == error, "Failed to create capture folder, Error: %d", error);
        }

#endif

        int output_stride = outNativeBuffer->getStride();
        int output_height = outNativeBuffer->getHeight();
        int output_width = outNativeBuffer->getWidth();

        char out_fname[256];
        snprintf(out_fname, sizeof(out_fname), "%s", fileName);

        FILE* pFile = fopen(out_fname, "wb");
        NT_ASSERT(pFile != nullptr, "Output file creation failed");

        size_t written;

        int format = stream->format;
        if (format == HAL_PIXEL_FORMAT_YCbCr_420_888 || format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED)
        {
            int Y_size = PAD_64(output_width) * output_height;
            int UV_offset = Y_size;
            int UV_size = PAD_64(output_width) * (output_height / 2);
#ifdef _LINUX
#ifdef LEGACY
            UV_offset = PAD_64(output_width) * PAD_64(output_height);
#endif // LEGACY
#endif // _LINUX


            written = fwrite(outputBufferPtr, Y_size, 1, pFile);
            NT_EXPECT(written == 1, "Output Y plane write failed obtained:");

            // note buffer created on windows doesn't have same offset as gralloc
            written = fwrite(outputBufferPtr + UV_offset, UV_size, 1, pFile);
            NT_EXPECT(written == 1, "Output UV plane write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_RAW16)
        {
            int size = output_stride * output_height * 2;
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_EXPECT(written == 1, "Output RAW_16 write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_RAW_OPAQUE)
        {
            int size = output_stride * output_height;
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_EXPECT(written == 1, "Output RAW_OPAQUE write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_RAW10)
        {
            int size = output_stride * output_height * (5 / 4);
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_EXPECT(written == 1, "Output RAW_10 write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_BLOB)
        {
            // get the actual size of data in buffer
            int maxSize = output_stride * output_height * 4;
            int size = GetJpegTrimmedBufferSize(outputBufferPtr, maxSize);
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_EXPECT(written == 1, "Output JPEG write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_Y16)
        {
            int size = output_stride * output_height * 2;
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_EXPECT(written == 1, "Output Y16 write failed");
        }
#ifdef _LINUX
        res = outNativeBuffer->unlock();
        NT_ASSERT(res == 0, "outputNativeBuffer unlock failed");
#endif //_LINUX
        res = fclose(pFile);
        NT_ASSERT(res == 0, "Close file failed");
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetJpegTrimmedBufferSize()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::GetJpegTrimmedBufferSize(char* currentBufferPtr, int maxSize)
    {
        int actualSize = 0;
        bool endOfData = false;
        char *nextBufferPtr = currentBufferPtr;

        // Iterate over the buffer from beginning till end of JPEG data (0xFF,0xD9)
        while (!endOfData)
        {
            // break if max possible size reached
            if (actualSize >= maxSize)
            {
                break;
            }
            ++nextBufferPtr;
            if (static_cast<int>(*currentBufferPtr) == 255) //0xFF
            {
                if (static_cast<int>(*nextBufferPtr) == 217) //0xD9
                {
                    endOfData = true;
                }
            }
            ++actualSize;
            currentBufferPtr = nextBufferPtr;
        }
        return ++actualSize; // increase by one to include end of image byte (0xD9)
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::FillNV12Buffer()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::FillNV12Buffer(buffer_handle_t *bufferHandle, camera3_stream_t *stream)
    {
        int res;
        char* inputBufferPtr;

#ifdef _LINUX
        sp<GraphicBuffer> inputNativeBuffer;
#else
        std::shared_ptr<CamxBuffer> inputNativeBuffer;
#endif
        std::unique_lock<std::mutex> nativeBufferLock(nativeBufferMutex);
        inputNativeBuffer = *nativeBuffers.at(stream).at(bufferHandle).outNativeBuffer;
        nativeBufferLock.unlock();
        NT_ASSERT(inputNativeBuffer != nullptr, "InputNativeBuffer is Null");

        int input_stride = inputNativeBuffer->getStride();
        int input_height = inputNativeBuffer->getHeight();
        int input_width = inputNativeBuffer->getWidth();

        int Y_size = input_width * input_height;
        int UV_size = input_width * (input_height / 2);

#ifdef _LINUX
        res = inputNativeBuffer->lock(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN, reinterpret_cast<void**>(&inputBufferPtr));
        NT_ASSERT(res == 0, "inputNativeBuffer lock failed");
        NATIVETEST_UNUSED_PARAM(Y_size);
        NATIVETEST_UNUSED_PARAM(UV_size);
#else
        inputBufferPtr = static_cast<char*>(malloc(sizeof(char) * (Y_size + UV_size)));
#endif
        uint8_t * test = reinterpret_cast<uint8_t*>(inputBufferPtr);
        //uint8_t intensity = 0xFF;

#define RED_Y 0x40; //77
#define RED_U 0x66; //85
#define RED_V 0xF0; //255

#define GREEN_Y 0xB0; //149
#define GREEN_U 0x2A; //43
#define GREEN_V 0x1A; //21

#define BLUE_Y 0x20; //29
#define BLUE_U 0xF0; //255
#define BLUE_V 0x76; //107

#define WHITE_Y 0xF0; //255
#define WHITE_U 0x80; //128
#define WHITE_V 0x80; //128


        //int yOffset = 0;
        int yStride = input_stride;
        int UVOffset = input_stride*input_height;
        int UVStride = yStride;

        //uint8_t* dst = test;
        uint8_t* base = test;

        // fill Y
        for (int row = 0; row < input_height; ++row)
        {
            for (int col = 0; col < yStride; ++col)
            {
                if ((row < input_height / 2) && (col < yStride / 2))
                {
                    test[col] = RED_Y; //red
                }
                else if ((row < input_height / 2) && (col >= yStride / 2))
                {
                    test[col] = GREEN_Y; //green
                }
                else if ((row >= input_height / 2) && (col < yStride / 2))
                {
                    test[col] = BLUE_Y; //blue
                }
                else
                {
                    test[col] = WHITE_Y; //black
                }
            }
            test += yStride;
        }

        test = base + UVOffset;

        //fill U/V
        for (int row = 0; row < (input_height / 2); ++row)
        {
            for (int col = 0; col < (input_width / 2); ++col)
            {
                if ((row < input_height / 4) && (col < input_width / 4))
                {
                    test[2 * col] = RED_U; //red
                    test[2 * col + 1] = RED_V;
                }
                else if ((row < input_height / 4) && (col >= input_width / 4))
                {
                    test[2 * col] = GREEN_U; //green
                    test[2 * col + 1] = GREEN_V;
                }
                else if ((row >= input_height / 4) && (col < input_width / 4))
                {
                    test[2 * col] = BLUE_U; //blue
                    test[2 * col + 1] = BLUE_V;
                }
                else
                {
                    test[2 * col] = WHITE_U; //white
                    test[2 * col + 1] = WHITE_V;
                }
            }
            test += UVStride;
        }
#ifdef _LINUX
        res = inputNativeBuffer->unlock();
        NT_ASSERT(res == 0, "inputNativeBuffer unlock failed");
#else
        res = inputNativeBuffer->FillBuffer(inputBufferPtr);
        free(inputBufferPtr);
#endif

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetConfigedStreams()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::shared_ptr<camera3_stream_t>> Cam3Device::GetConfigedStreams() const
    {
        return  mStreams;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::ClearOutStreams()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::ClearOutStreams()
    {
        mStreams.clear();

        if (!isHighspeedSession && !allocatedStreamsforHFR.empty())
        {
            allocatedStreamsforHFR.clear();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CreateOutputStreamBuffer()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _LINUX
    void  Cam3Device::CreateOutputStreamBuffer(
        camera3_stream_buffer_t &outputBuffer,  //[out] location for the new stream buffer
        camera3_stream_t* outputStream,         //[in] stream to use for the initial capture from sensor
        Size outputResolution,                  //[in] resolution of the initial capture from sensor
        int outputFormat,                       //[in] format for the initial capture from sensor
        sp<GraphicBuffer>* outputNativeBuffer)  //[out] location for the new native buffer
#else
    void  Cam3Device::CreateOutputStreamBuffer(
        camera3_stream_buffer_t &outputBuffer,              //[out] location for the new stream buffer
        camera3_stream_t* outputStream,                     //[in] stream to use for the initial capture from sensor
        Size outputResolution,                              //[in] resolution of the initial capture from sensor
        int outputFormat,                                   //[in] format for the initial capture from sensor
        std::shared_ptr<CamxBuffer>* outputNativeBuffer)    //[out] location for the new native buffer

#endif
    {
        outputBuffer.stream = outputStream;

        Size grallocDimensions = GetGrallocSize(outputResolution, outputFormat);
#ifdef _LINUX
        //output native buffer
        sp<GraphicBuffer> nativeBuffer =
            SAFE_NEW() GraphicBuffer(grallocDimensions.width,
                grallocDimensions.height,
                outputFormat,
                outputBuffer.stream->usage);

        *outputNativeBuffer = nativeBuffer;

        ANativeWindowBuffer *anbOutput = nativeBuffer->getNativeBuffer();
        NT_ASSERT(anbOutput != nullptr, "Native window buffer for output stream is NULL");
        outputBuffer.buffer = &(anbOutput->handle);
#else
        std::shared_ptr <CamxBuffer> nativeBuffer(std::make_shared<CamxBuffer>(grallocDimensions.width, grallocDimensions.height, outputFormat));
        *outputNativeBuffer = nativeBuffer;
        outputBuffer.buffer = nativeBuffer->GetNativeHandle();
#endif
        outputBuffer.status = CAMERA3_BUFFER_STATUS_OK;
        outputBuffer.acquire_fence = -1; //no waiting for now
        outputBuffer.release_fence = -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CreateOutputBuffer()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    camera3_stream_buffer_t* Cam3Device::CreateOutputBuffer(camera3_stream_t* stream)
    {
        std::unique_lock<std::mutex> createOutputBufferLock(outputBufferMutex);
        camera3_stream_buffer_t* outputBuffer = nullptr;
        try
        {
            outputBuffer = SAFE_NEW() camera3_stream_buffer_t;
#ifdef _LINUX
            sp<GraphicBuffer>* outNativeBuffer = SAFE_NEW() sp<GraphicBuffer>();
#else
            std::shared_ptr <CamxBuffer>* outNativeBuffer = SAFE_NEW() std::shared_ptr<CamxBuffer>();
#endif
            if (stream != nullptr)
            {
                Size curResolution(stream->width, stream->height);
                int curFormat = stream->format;
                CreateOutputStreamBuffer(*outputBuffer, stream, curResolution, curFormat,
                    outNativeBuffer);
                if (outputBuffer->buffer == nullptr)
                {
                    NT_LOG_ERROR("[CAMERAID %d] output buffer handle found to be null!", mCamId);
                    createOutputBufferLock.unlock();
                    return nullptr;
                }

                buffer_handle_t *bufferHandle = outputBuffer->buffer;
                outputBufferInfo bufferInfo;
                bufferInfo.outNativeBuffer = outNativeBuffer;
                /* START critical section - native buffer map */
                std::unique_lock<std::mutex> nativeBufferLock(nativeBufferMutex);
                nativeBuffers[stream].insert(std::make_pair(bufferHandle, bufferInfo));
                nativeBufferLock.unlock();
                /* END critical section - native buffer map */
            }

            else
            {
                createOutputBufferLock.unlock();
                return nullptr;
            }
            createOutputBufferLock.unlock();
        }
        catch (std::bad_alloc ex)
        {
            NT_LOG_ERROR("[CAMERAID %d] Bad memory allocated exception thrown!", mCamId);
            createOutputBufferLock.unlock();
            return nullptr;
        }
        return outputBuffer;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AreCameraStreamsEqual()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::AreCameraStreamsEqual(camera3_stream_t &lhs, camera3_stream_t &rhs)
    {
        if (lhs.format == rhs.format && lhs.width == rhs.width && lhs.height == rhs.height && lhs.data_space == rhs.data_space
            && lhs.max_buffers == rhs.max_buffers && lhs.priv == rhs.priv &&
            lhs.rotation == rhs.rotation && lhs.stream_type == rhs.stream_type && lhs.usage == rhs.usage)
        {
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::CreateCaptureRequestWithMultBuffers()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::CreateCaptureRequestWithMultBuffers(
        int streamStartIndex,    //[in] start index of streams
        int requestTypes,        //[in] capture template type
        int num_streams,         //[in] number of streams
        int frameNumber,
        std::vector<int> streamIndicesToRequest)
    {
        camera3_stream_buffer_t* outputBuffer = nullptr;
        camera3_stream_buffer_t *selectedOpBuffersPerRequest = SAFE_NEW() camera3_stream_buffer_t[num_streams];
        if (streamIndicesToRequest.size() == 0)
        {
            for (int streamId = 0; streamId < num_streams; streamId++)
            {
                camera3_stream_t* camStream = (streamStartIndex < static_cast<int>(GetConfigedStreams().size())) ?
                    GetConfigedStreams().at(streamStartIndex).get() : nullptr;
                outputBuffer = CreateOutputBuffer(camStream);
                if (outputBuffer == nullptr)
                {
                    delete[] selectedOpBuffersPerRequest;
                    NT_ASSERT(false, "Cannot create output buffer for stream %d", streamId);
                }
                NT_LOG_DEBUG("[CAMERAID %d] Created output buffer for stream: %d", mCamId, streamStartIndex);
                selectedOpBuffersPerRequest[streamId] = *outputBuffer;
                streamStartIndex++;
            }
            InitHalRequest(frameNumber, nullptr, &selectedOpBuffersPerRequest[0], num_streams, requestTypes);
        }
        else
        {
            camera3_stream_buffer_t *streamBuffer = nullptr;
            for (UINT streamId = 0; streamId < streamIndicesToRequest.size(); streamId++)
            {
                camera3_stream_t* camStream = GetConfigedStreams().at(streamIndicesToRequest.at(streamId)).get();
                streamBuffer = CreateOutputBuffer(camStream);
                if (streamBuffer == nullptr)
                {
                    delete[] selectedOpBuffersPerRequest;
                    NT_ASSERT(false, "Cannot create output buffer for stream %d", streamIndicesToRequest.at(streamId));
                }
                NT_LOG_DEBUG("[CAMERAID %d] Created output buffer for stream: %d", mCamId,
                    streamIndicesToRequest.at(streamId));
                selectedOpBuffersPerRequest[streamId] = *streamBuffer;
            }
            InitHalRequest(frameNumber, nullptr, &selectedOpBuffersPerRequest[0], num_streams, requestTypes);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::LoadImageToBuffer()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Cam3Device::LoadImageToBuffer(
        const char* imageFile,         //[in] name of image to load
        buffer_handle_t *bufferHandle,
        camera3_stream_t *stream)
    {
        int ret;

        if (imageFile != nullptr)
        {
            void* byteBuf;
            const std::string imagePath = sLogRootPath + imageFile;

            // Open image for reading
            FILE *fp = fopen(imagePath.c_str(), "rb");
            if (!fp)
            {
                NT_LOG_ERROR("Failed to open image: %s", imageFile);
                return -1;
            }
            // Calculate size of image
            fseek(fp, 0L, SEEK_END);
            size_t imageSize = ftell(fp);
            rewind(fp);

#ifdef _LINUX
            sp<GraphicBuffer> pOutNativeBuffer;
#else
            std::shared_ptr<CamxBuffer> pOutNativeBuffer;
#endif
            std::unique_lock<std::mutex> nativeBufferLock(nativeBufferMutex);
            pOutNativeBuffer = *nativeBuffers.at(stream).at(bufferHandle).outNativeBuffer;
            nativeBufferLock.unlock();
            if (pOutNativeBuffer == nullptr)
            {
                NT_LOG_ERROR("[CAMERAID %d] Failed to get pOutNativeBuffer from circular buffer array!", mCamId);
                fclose(fp);
                return -1;
            }
#ifdef _LINUX
            // Lock buffer and ready for reading
            ret = pOutNativeBuffer->lock(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN, &byteBuf);
            if (ret != 0)
            {
                NT_LOG_ERROR("pOutNativeBuffer lock failed!");
                fclose(fp);
                return -1;
            }
#else
            byteBuf = malloc(sizeof(unsigned char) * imageSize);
#endif
            if (!byteBuf)
            {
                NT_LOG_ERROR("[CAMERAID %d] Failed to acquire/allocate buffer byteBuf for loading image!", mCamId);
                fclose(fp);
                return -1;
            }
            // Read image bytes into buffer
            size_t bytesRead = fread(byteBuf, imageSize, 1, fp);
            fclose(fp);

#ifdef _LINUX
            // Unlock buffer
            ret = pOutNativeBuffer->unlock();
            if (ret != 0)
            {
                NT_LOG_ERROR("pOutNativeBuffer unlock failed!");
                return -1;
            }
#else
            pOutNativeBuffer->FillBuffer(static_cast<char*>(byteBuf));
            free(byteBuf);
#endif
            if (bytesRead > 0)
                return 0;
        }
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::SetHighSpeedSession()
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetHighSpeedSession()
    {
        isHighspeedSession = true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::SetPreviewFPSforHFR()
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::SetPreviewFPSforHFR(
        int32_t fpsMin, //[in] preview fps minimum to set
        int32_t fpsMax) //[in] preview fps maximum to set
    {
        previewFpsRange[0] = fpsMin;
        previewFpsRange[1] = fpsMax;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::IsHighSpeedSession()
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::IsHighSpeedSession() const
    {
        return isHighspeedSession;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::ClearHighSpeedSession()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::ClearHighSpeedSession()
    {
        isHighspeedSession = false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetRequestByFrameNumber()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Cam3Device::RequestTracker_t* Cam3Device::GetRequestByFrameNumber(int frameNumber) const
    {
        if (PCRTracker.find(frameNumber) == PCRTracker.end())
        {
            return nullptr;
        }
        return PCRTracker.at(frameNumber).get();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetIfAWBConverged()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::GetIfAWBConverged(int frameNumber) const
    {
        return PCRTracker.at(frameNumber)->isAWBConverged;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetIfAFConverged()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::GetIfAFConverged(int frameNumber) const
    {
        return PCRTracker.at(frameNumber)->isAFConverged;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetIfAEConverged()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::GetIfAEConverged(int frameNumber) const
    {
        return PCRTracker.at(frameNumber)->isAEConverged;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::ClearMetadataQueue()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::ClearMetadataQueue(std::queue<PartialMetadataSet *> metadataQueue)
    {
        std::queue<PartialMetadataSet *> emptyQueue;
        std::swap(metadataQueue, emptyQueue);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::AreFramesProcessed()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Cam3Device::AreFramesProcessed()
    {
        std::map<int, std::unique_ptr<RequestTracker_t>>::iterator iter = PCRTracker.begin();
        while (iter != PCRTracker.end())
        {
            if (iter->second->resultStatus < RESULTS_RECEIVED)
            {
                return false;
            }
            iter++;
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   Cam3Device::GetUnprocessedFramenumbers()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<int> Cam3Device::GetUnprocessedFramenumbers()
    {
        std::vector<int> frameNumbers;
        std::map<int, std::unique_ptr<RequestTracker_t>>::iterator iter = PCRTracker.begin();
        while (iter != PCRTracker.end())
        {
            if (iter->second->resultStatus < RESULTS_RECEIVED)
            {
                frameNumbers.push_back(iter->first);
            }
            iter++;
        }
        return frameNumbers;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UpdateFlushFramesSet
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::UpdateFlushFramesSet(UINT32 frameNumber)
    {
        for (UINT32 fn = frameNumber - 1; fn > ((frameNumber > mPipelineDepth) ? (frameNumber - mPipelineDepth) : 0 ); fn --)
        {
            if (PCRTracker[fn]->resultStatus == REQUEST_SENT) {
                NT_LOG_INFO("Adding frame to probable flush list: %d", fn);
                mFlushFramesSet.insert(fn);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetTagID
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 Cam3Device::GetTagID(UINT32 featVendorTag)
    {
        return mTagIdMap.at(featVendorTag);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Cam3Device::ProcessCaptureResult
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Cam3Device::ProcessCaptureResult(const camera3_capture_result *pCaptureResult)
    {
        const uint32_t frameNumber = pCaptureResult->frame_number;
        const uint32_t partialResult = pCaptureResult->partial_result;
        const uint32_t numOutputBufs = pCaptureResult->num_output_buffers;
        const camera_metadata_t* metadata = pCaptureResult->result;

        NT_LOG_INFO("[CAMERAID %u] ProcessCaptureResult for frame [%u] with partial result [%u]", mCamId, frameNumber, partialResult);

        camera_info *camInfo = GetCameraInfoWithId(mCamId);
        NT_ASSERT(camInfo != nullptr, "Cannot fetch camera info for camera ID %d", mCamId);
        // Clone and push partial metadata result onto queue to analyze later
        // Save metadata for later analysis
        if (metadata != nullptr)
        {
            PartialMetadataSet* clonedPartial = SAFE_NEW() PartialMetadataSet(partialResult, Camera3Metadata(metadata));
            PCRTracker[frameNumber]->metadataQueue.push(clonedPartial);
        }

        // Check result metadata (non-zero partial result should contain metadata)
        if (partialResult != 0)
        {
            PCRTracker[frameNumber]->nPartialResultsRemaining--;
            NT_EXPECT(1 <= partialResult && partialResult <= mPartialResultCount,
                "Result is malformed, partial_result (%d) not in range!", partialResult);
            // Save metadata for later analysis
            if (metadata != nullptr)
            {
                AddMetadatatoMap(pCaptureResult);
            }
            else
            {
                NT_LOG_WARN("[CAMERAID %u] Cannot write metadata for frame %u because it is NULL", mCamId, frameNumber);
            }
            if (!isAEConverged)
            {
                IfAEConverged(mMetadata.find(frameNumber)->second.GetCamMetadata());
            }
            if (!isAFConverged)
            {
                IfAFConverged(mMetadata.find(frameNumber)->second.GetCamMetadata());
            }
            if (!isAWBConverged)
            {
                IfAWBConverged(mMetadata.find(frameNumber)->second.GetCamMetadata());
            }
        }
        else
        {
            NT_ASSERT(numOutputBufs > 0 && pCaptureResult->output_buffers != nullptr
                || pCaptureResult->input_buffer != nullptr, "Capture result contains no metadata or buffers!");
        }

        // Keep track of output buffers received
        PCRTracker[frameNumber]->nOutputBuffersRemaining -= numOutputBufs;
        NT_LOG_INFO("[CAMERAID %u] Number of output buffers in this result: %u", mCamId, numOutputBufs);
        NT_LOG_INFO("[CAMERAID %u] Number of output buffers remaining: %d", mCamId, PCRTracker[frameNumber]->nOutputBuffersRemaining);
        NT_EXPECT(PCRTracker[frameNumber]->nOutputBuffersRemaining >= 0, "Too many output buffers were received!");

        // Keep track of input buffers received
        uint32_t numInputBufs = (nullptr != pCaptureResult->input_buffer) ? 1 : 0;
        PCRTracker[frameNumber]->nInputBuffersRemaining -= numInputBufs;
        NT_LOG_INFO("[CAMERAID %u] Number of input buffers in this result: %u", mCamId, numInputBufs);
        NT_LOG_INFO("[CAMERAID %u] Number of input buffers remaining: %d", mCamId, PCRTracker[frameNumber]->nInputBuffersRemaining);
        NT_EXPECT(PCRTracker[frameNumber]->nInputBuffersRemaining >= 0, "Too many input buffers were received!");

        NT_LOG_INFO("[CAMERAID %u] Number of partial results remaining: %d", mCamId, PCRTracker[frameNumber]->nPartialResultsRemaining);
        NT_EXPECT(PCRTracker[frameNumber]->nPartialResultsRemaining >= 0, "Too many partial results were received!");

        if (numOutputBufs > 0)
        {
            NT_ASSERT(pCaptureResult->output_buffers != nullptr,
                "Capture result reported num_output_buffers [%d], but output_buffers pointer is NULL!", numOutputBufs);

            // Check status of each output buffer
            for (uint32_t bufIndex = 0; bufIndex < numOutputBufs; bufIndex++)
            {
                // Record buffer status to analyze later
                mOutputBufferStatus[FRAME_REQUEST_MOD(frameNumber)] = pCaptureResult->output_buffers[bufIndex].status;
                NT_LOG_DEBUG("[CAMERAID %u] Buffer: %u received in format: %d", mCamId, bufIndex,
                    pCaptureResult->output_buffers[bufIndex].stream->format);

                buffer_handle_t *bufferHandle = pCaptureResult->output_buffers[bufIndex].buffer;
                camera3_stream_t *camStream = pCaptureResult->output_buffers[bufIndex].stream;
                std::future<void> dumpThread = std::async(std::launch::async, [&] { DumpOutputBuffers(bufferHandle,
                    frameNumber, camStream); });
                if (dumpThread.wait_for(std::chrono::milliseconds(5 * DEFAULT_WAIT_TIME)) == std::future_status::timeout)
                {
                    NT_LOG_ERROR("[CAMERAID %d] Timed out dumping output buffer for frame number %d", mCamId, frameNumber);
                    PcrSuccess = FALSE;
                    stopPcrFlag = TRUE;
                }
                dumpThread.get();
            }
        }

        if (PCRTracker[frameNumber]->nInputBuffersRemaining == 0 &&
            PCRTracker[frameNumber]->nOutputBuffersRemaining == 0 &&
            PCRTracker[frameNumber]->nPartialResultsRemaining < static_cast<int>(mPartialResultCount) &&
            PCRTracker[frameNumber]->resultStatus != FLUSHED)
        {
            PCRTracker[frameNumber]->resultStatus = RESULTS_RECEIVED;
            if (mReconfigureFramesMap.find(frameNumber) != mReconfigureFramesMap.end()) {
                NT_LOG_INFO("Results received for reconfig frame");
                mReconfigureFramesMap[frameNumber] = RESULTS_RECEIVED;
            }
            std::future<void> resultThread = std::async(std::launch::async, [&] { VerifyResults(frameNumber); });
            if (resultThread.wait_for(std::chrono::milliseconds(5 * DEFAULT_WAIT_TIME)) == std::future_status::timeout)
            {
                NT_LOG_ERROR("[CAMERAID %d] Timed out verifying results for frame number %d", mCamId, frameNumber);
                PcrSuccess = FALSE;
                stopPcrFlag = TRUE;
            }
            resultThread.get();
        }
    }
}
