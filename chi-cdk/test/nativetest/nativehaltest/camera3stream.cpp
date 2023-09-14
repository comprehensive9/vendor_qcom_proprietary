 //*************************************************************************************************
 // Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
 // All Rights Reserved.
 // Confidential and Proprietary - Qualcomm Technologies, Inc.
 //*************************************************************************************************

#include <chrono>
#include<string>
#include "camera3stream.h"
#include "camera3metadata.h"
#include "vendor_tags_test.h"

namespace haltests {

    jmp_buf envo;
    sig_atomic_t erSignal = 0;

    void signalHandlr(int signum)
    {
        erSignal = signum;
        signal(signum, SIG_DFL);
        longjmp(envo, 1);
    }

    CDKResult Camera3Stream::StreamStarter(int cameraId, uint32_t frameNumber, bool bDumpOutputBuffer,
        int outputStreamIndex, const char * fileName)
    {
        CDKResult result = CDKResultEFailed;
        if (setjmp(envo) == 0) //Initially env is set to 0 and continues with normal execution
        {
            signal(SIGABRT, &signalHandlr); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, &signalHandlr);
            signal(SIGFPE, &signalHandlr);
            signal(SIGILL, &signalHandlr);
            Camera3Stream::StartStreaming(cameraId, frameNumber, bDumpOutputBuffer, outputStreamIndex, fileName);
            signal(SIGABRT, SIG_DFL); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, SIG_DFL);
            signal(SIGFPE, SIG_DFL);
            signal(SIGILL, SIG_DFL);
            result = CDKResultSuccess;
        }
        else
        {
            switch (erSignal)
            {
            case SIGILL:
            {
                NT_LOG_ERROR("Illegal Instruction Called !");
                NT_LOG_ERROR("%s Stopped", HalCmdLineParser::GetTestSuiteName().c_str());
                ShutDown();
                break;
            }
            case SIGABRT:
            {
                NT_LOG_ERROR("Abort Called !");
                NT_LOG_ERROR("%s Stopped", HalCmdLineParser::GetTestSuiteName().c_str());
                ShutDown();
                break;
            }
            case SIGSEGV:
            {
                NT_LOG_ERROR("Segmentation Violation Called  !");
                NT_LOG_ERROR("%s Stopped", HalCmdLineParser::GetTestSuiteName().c_str());
                ShutDown();
                break;
            }
            case SIGFPE:
            {
                NT_LOG_ERROR("Floating-Point arithmetic Exception !");
                NT_LOG_ERROR("%s Stopped", HalCmdLineParser::GetTestSuiteName().c_str());
                ShutDown();
                break;
            }
            }
        }
        return result;
    }


    Camera3Stream::Camera3Stream() : m_ppOutNativeBuffers(nullptr),
        m_ppOutBuffers(nullptr),
        m_pOutBuffersPerRequest(nullptr)
    {
        for (size_t i = 0; i < NATIVETEST_ELEMENTS(halRequest); i++)
        {
            halRequest[i] = nullptr;
        }
    }

    Camera3Stream:: ~Camera3Stream()
    {
        mStreams.clear();
        allocatedStreamsforHFR.clear();
    }

    void  Camera3Stream::Setup()
    {
        Camera3Device::Setup();
        // Initialize vendor tags
        NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
        // Cleanup any leftover hal requests (possibly from a previous test)
        // Without this code, ExtendedCameraCharacteristicsTest was generating a hal request with 0 num_output_buffers
        int numCameras = Camera3Device::GetNumCams();
        for (int camId = 0; camId < numCameras; camId++)
        {
            DeleteHalRequest(camId);
        }
    }

    void Camera3Stream::Teardown()
    {
        Camera3Device::Teardown();
    }


    /**************************************************************************************************************************
    *   Camera3Stream::CreateStream()
    *
    *   @brief
    *       Helper function to create camera3_stream_t object which is used to configure the stream
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::CreateStream(
        int streamType,                         //[in] stream type specifies if stream is O/P, I/P or bidirectional type
        Size resolution,                        //[in] resolution of the stream
        uint32_t usage,                         //[in] usage flag required by gralloc
        int format,                             //[in] format of the stream
        camera3_stream_rotation_t rotation,     //[in] rotation of camera
        int dataspace)                          //[in] dataspace, defaults to -1 if none provided
    {

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
                NT_ASSERT(1 == 0,"Invalid stream format");
            }
        }

        newStream->rotation = rotation;
        newStream->usage = usage;
        newStream->max_buffers = 0;
        newStream->priv = nullptr;

        mStreams.push_back(newStream);

        if (isHighspeedSession)
        {
            allocatedStreamsforHFR.push_back(newStream);
        }

        NT_ASSERT(!mStreams.empty(),"stream vector is empty.");

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Camera3Stream::ConfigStream
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Camera3Stream::ConfigStream(uint32_t opmode, int cameraId, bool setResourceCostCheck)
    {
        config = {};  // Ensure all new members are 0 initialized (session_parameters)
        config.operation_mode = opmode;

        if (mStreams.empty())
        {
            NT_LOG_ERROR("mStreams is empty");
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
            NT_LOG_ERROR("Overriding resource cost validation!");
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

        NT_LOG_DEBUG("Configuring %d streams ...", config.num_streams);
        int res = -1;
        camera3_device_t* pCamDev = GetCamDevice(cameraId);
        if(nullptr == pCamDev)
        {
            NT_LOG_ERROR("Unable to get camera device for Camera ID: %d", cameraId);
            return res;
        }

        res = pCamDev->ops->configure_streams(pCamDev, &config);
        if (res == 0)
        {
            NT_LOG_INFO("Generating output buffers for configured streams");
            res = CreateOutputBuffers(cameraId);
        }
        delete[] config.streams;
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Camera3Stream::ConfigStreamSingle
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Camera3Stream::ConfigStreamSingle(bool isConstrainedHighSpeedConfiguration, int streamIndex, int cameraId,
        bool generateBuffers, bool setResourceCostCheck)
    {
        config = {};  // Ensure all new members are 0 initialized (session_parameters)
        config.operation_mode = isConstrainedHighSpeedConfiguration ?
            CAMERA3_STREAM_CONFIGURATION_CONSTRAINED_HIGH_SPEED_MODE :
            CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE;


        if (mStreams.empty())
        {
            NT_LOG_ERROR("mStreams is empty");
        }

        config.num_streams = 1;
        config.streams = SAFE_NEW() camera3_stream_t*[config.num_streams]; //including input and output;
        std::unique_lock<std::mutex> lck_request(Camera3Device::halRequestMutex);
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
            NT_LOG_ERROR("Overriding resource cost validation!");
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

        int res = -1;
        camera3_device_t* pCamDev = GetCamDevice(cameraId);
        if (nullptr == pCamDev)
        {
            NT_LOG_ERROR("Unable to get camera device for Camera ID: %d", cameraId);
            return res;
        }
        res = pCamDev->ops->configure_streams(pCamDev, &config);
        if (res == 0 && generateBuffers)
        {
            NT_LOG_ERROR("Generating output buffers for configured stream");
            res = CreateOutputBuffers(cameraId);
        }
        lck_request.unlock();
        delete[] config.streams;
        return res;

    }

    void Camera3Stream::UpdateSessionParameters(
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
            NT_LOG_ERROR("add metadata key: %u", tag);
            res = add_camera_metadata_entry(reinterpret_cast<camera_metadata_t *>(meta.GetCamMetadata()),
                tag,
                reinterpret_cast<const void*> (value),
                dataCount);
            NT_ASSERT(res == 0, "add_camera_metadata_entry failed");
        }
        else
        {
            int res;
            NT_LOG_ERROR("Update metadata key: %u", tag);
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

    /**************************************************************************************************************************
    *   Camera3Stream::GetBufferForRequest()
    *
    *   @brief
    *       return the buffer for a given stream and frame
    *
    *   @return
    *       camera3_stream_buffer_t*
    **************************************************************************************************************************/
    camera3_stream_buffer_t* Camera3Stream::GetBufferForRequest(int streamIndex, int frameNumber) const
    {
        if (streamIndex >= static_cast<int>(GetConfigedStreams().size()))
        {
            NT_LOG_ERROR("streamIndex: [%d] cannot be greater than number of available streams: [%d]",
                streamIndex, GetConfigedStreams().size());
            return nullptr;
        }

        m_pOutBuffersPerRequest[streamIndex] = m_ppOutBuffers[streamIndex][frameNumber % GetPipelineDepth()];

        if (m_pOutBuffersPerRequest[streamIndex].release_fence != -1)
        {
            NT_LOG_ERROR("Release fence for buffer is not set for frame number: %d", frameNumber);
            return nullptr;
        }
        NT_LOG_DEBUG("Getting buffer number: [%d] for stream: %d", frameNumber % GetPipelineDepth(), streamIndex);
        return &m_pOutBuffersPerRequest[streamIndex];
    }

    /**************************************************************************************************************************
    *   Camera3Stream::FlushStream()
    *
    *   @brief
    *       Flushes the pending requests for given camera id
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::FlushStream(
        int cameraId)  //[in] camera id for the current open camera
    {
        NT_LOG_DEBUG("FlushStream..");
        camera3_device_t* pCamDev = GetCamDevice(cameraId);
        NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
        NT_ASSERT(pCamDev->ops->flush(pCamDev) == 0, "flush failed");
    }

    /**************************************************************************************************************************
    *   Camera3Stream::TestFlush()
    *
    *   @brief
    *       Flushes the pending requests for given camera id and measures the flush time
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    int Camera3Stream::TestFlush(
        int cameraId)       //[in] camera id for the current open camera
    {
        NT_LOG_INFO("Testing flush with delay : %d ", HalCmdLineParser::g_flushDelay);
        std::this_thread::sleep_for(std::chrono::microseconds(HalCmdLineParser::g_flushDelay));

        auto flushStartTime = std::chrono::steady_clock::now();
        int res = -1;
        camera3_device_t* pCamDev = GetCamDevice(cameraId);
        if (nullptr == pCamDev)
        {
            NT_LOG_ERROR("Unable to get camera device for Camera ID: %d", cameraId);
            return res;
        }
        res = pCamDev->ops->flush(pCamDev);
        auto flushEndTime = std::chrono::steady_clock::now();
        double flushTime = std::chrono::duration<double, std::milli>(flushEndTime - flushStartTime).count();
        NT_LOG_INFO("KPI: flush return time took %f ms", flushTime);
        if(flushTime > 1000.0)
        {
            NT_LOG_ERROR("Flush call took longer than maximum timing requirement of 1 second!");
        }
        return res;
    }

    /**************************************************************************************************************************
    *   Camera3Stream::CreateMultiStreams()
    *
    *   @brief
    *       Helper function to create multiple stream objects
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::CreateMultiStreams(
        Size outResolutions[],   //[in] Array of resolutions corresponding to each stream
        int outputFormats[],     //[in] Array of formats corresponding to each stream
        int numStreams,          //[in] Number of streams to be configured
        uint32_t usageFlags[],   //[in] Array of usage flags corresponding to each stream
        int streamTypes[])       //[in] Array of stream type corresponding to each stream
    {
        for (int i = 0; i < numStreams; i++)
        {
            Camera3Stream::CreateStream(
                streamTypes[i],
                outResolutions[i],
                usageFlags[i],
                outputFormats[i],
                CAMERA3_STREAM_ROTATION_0);
        }
    }

    /**************************************************************************************************************************
    *   Camera3Stream::CheckType()
    *
    *   @brief
    *       Helper function to check that a tag returns metadata of the expected type
    *
    *   @return
    *       bool true if type was as expected, false otherwise
    **************************************************************************************************************************/
    bool Camera3Stream::CheckType(
        uint32_t tag,           //[in] tag to check type of
        uint8_t expectedType)   //[in] type of data that is expected for the tag
    {
        int type = get_camera_metadata_tag_type(tag);
        if (type == -1)
        {
            NT_LOG_ERROR("Tag not found: %d", tag);
            return false;
        }

        if ((type != expectedType))
        {
            NT_LOG_ERROR("Mismatched tag type: %d Expected type is: %d, got type: %d", tag, static_cast<int>(expectedType), type);
            return false;
        }

        return true;

    }

    bool Camera3Stream::SetMFNRMode(int cameraId)
    {
        char tagName[] = "CustomNoiseReduction";
        char sectionName[] = "org.quic.camera.CustomNoiseReduction";
        const uint8_t tagValuesNR[] = { 1 };

        //Initialize all vendor tags
        if (VendorTagsTest::InitializeVendorTags(&vTag) != 0)
        {
            NT_LOG_ERROR("Vendor tags could not be initialized!");
            return false;
        }
        //Resolve the tags
        uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagId == UINT32_MAX)
        {
            NT_LOG_ERROR("Custom Noise Reduction vendor tag not available");
            return false;
        }
        // apply requested vendor tag value to the requested key
        SetKey(cameraId, tagId, tagValuesNR, 1);

        const camera_module_t* camMod = GetCamModule();
        if (nullptr == camMod)
        {
            NT_LOG_ERROR("camera module not defined");
            return false;
        }
        if (nullptr == camMod->get_camera_info)
        {
            NT_LOG_ERROR("get_camera_info is not implemented");
            return false;
        }
        camera_info info;
        int val = camMod->get_camera_info(cameraId, &info);
        if (val != 0)
        {
            NT_LOG_ERROR("Unable to fetch camera info");
            return false;
        }
        uint32_t tagIDNRMode = ANDROID_NOISE_REDUCTION_MODE;
        uint8_t  tagValueNRHQ = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;
        camera_metadata_entry_t entry;
        int ret = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES,
            &entry);
        if (ret != 0)
        {
            NT_LOG_ERROR("GetCameraMetadataEntryByTag ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES failed");
            return false;
        }
        bool isHQModeSupported = false;
        for (size_t j = 0; j < entry.count; j++)
        {
            // if HIGH_QUALITY mode supported, set and validate
            if (entry.data.u8[j] == tagValueNRHQ)
            {
                isHQModeSupported = true;
                SetKey(cameraId, tagIDNRMode, &tagValueNRHQ, 1);
                break;
            }
        }
        return isHQModeSupported;
    }

    /**************************************************************************************************************************
    *   Camera3Stream::SetKey()
    *
    *   @brief
    *       Helper function to set data for a given tag and camera. A HAL request must exist for this function to succeed.
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::SetKey(
        int cameraId,           //[in] camera Id
        uint32_t tag,           //[in] tag to set data for
        const int32_t * value,  //[in] pointer to data to write for the given tag
        size_t dataCount)       //[in] number of data pieces to write
    {
        NT_ASSERT(CheckType(tag, TYPE_INT32),"CheckType failed");
        UpdateKey(cameraId, tag, static_cast<const void*>(value), dataCount);
    }

    void Camera3Stream::SetKey(int cameraId, uint32_t tag, const uint8_t * value, size_t dataCount)
    {

        NT_ASSERT(CheckType(tag, TYPE_BYTE), "CheckType failed");
        UpdateKey(cameraId, tag, static_cast<const void*>(value), dataCount);
    }

    void Camera3Stream::SetKey(int cameraId, uint32_t tag, const float * value, size_t dataCount)
    {
        NT_ASSERT(CheckType(tag, TYPE_FLOAT), "CheckType failed");
        UpdateKey(cameraId, tag, static_cast<const void*>(value), dataCount);
    }

    void Camera3Stream::SetKey(int cameraId, uint32_t tag, const int64_t * value, size_t dataCount)
    {
        NT_ASSERT(CheckType(tag, TYPE_INT64),"CheckType failed");
        UpdateKey(cameraId, tag, static_cast<const void*>(value), dataCount);
    }

    void Camera3Stream::SetKey(int cameraId, uint32_t tag, const double * value, size_t dataCount)
    {
        NT_ASSERT(CheckType(tag, TYPE_DOUBLE), "CheckType failed");
        UpdateKey(cameraId, tag, static_cast<const void*>(value), dataCount);
    }

    void Camera3Stream::SetKey(int cameraId, uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount)
    {
        NT_ASSERT(CheckType(tag, TYPE_RATIONAL),"CheckType failed");
        UpdateKey(cameraId, tag, static_cast<const void*>(value), dataCount);
    }

    /**************************************************************************************************************************
    *   Camera3Stream::UpdateKey()
    *
    *   @brief
    *       Update key value in the camera metadata. If key is not yet present in the metadata object, add the key and the
    *       corresponding value.
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::UpdateKey(
        int cameraId,       //[in] camera Id
        uint32_t tag,       //[in] tag to update data for
        const void* value,  //[in] pointer to data to write to tag
        size_t dataCount)   //[in] number of data pieces to write
    {
        NT_ASSERT(halRequest[cameraId] != nullptr && halRequest[cameraId]->settings != nullptr,"No hal request object!");
        Camera3Metadata meta = Camera3Metadata(halRequest[cameraId]->settings);

        NT_ASSERT(value != nullptr,"Key value is null");

        int type = get_camera_metadata_tag_type(tag);
        if (type == -1)
        {
            NT_ASSERT(1 == 0,"Unknown tag type at UpdateKey");
            return;
        }

        size_t bufferSize = get_camera_metadata_size(const_cast<camera_metadata_t*>(halRequest[cameraId]->settings));

        // Safety check - ensure that data isn't pointing to this metadata, since
        // that would get invalidated if a resize is needed
        uintptr_t bufAddr = reinterpret_cast<uintptr_t>(const_cast<camera_metadata_t*>(halRequest[cameraId]->settings));
        uintptr_t dataAddr = reinterpret_cast<uintptr_t>(value);
        if (dataAddr > bufAddr && dataAddr < (bufAddr + bufferSize)) {
            NT_ASSERT(1 == 0,"Update attempted with data from the same metadata buffer!");
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
            NT_ASSERT(res == 0,"update_camera_metadata_entry failed");
        }

        if (halRequest[cameraId]->settings != nullptr)
        {
            free_camera_metadata(const_cast<camera_metadata_t *>(halRequest[cameraId]->settings));
        }
        halRequest[cameraId]->settings = clone_camera_metadata(const_cast<const camera_metadata_t *> (meta.GetCamMetadata()));
        meta.ClearMetadata();
    }

    /**************************************************************************************************************************
    *   Camera3Stream::CheckKeySetInResult()
    *
    *   @brief
    *       Helper function to check a result frame for an expected tag value.
    *
    *   @return
    *       bool true if result data matches the expected value, false otherwise
    **************************************************************************************************************************/
    bool Camera3Stream::CheckKeySetInResult(
        uint32_t tag,           //[in] tag to check in metadata
        const uint8_t *value,   //[in] expected data to validate in metadata
        uint32_t frameNum)      //[in] frame number of the capture result
    {
        const camera_metadata_t* metaFromFile = Camera3Device::GetCameraMetadataByFrame(frameNum);
        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("Meta read from file is null");
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("find_camera_metadata_entry returned ERROR or empty entry");
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
                NT_LOG_DEBUG("Expected value: %d, got: %d", static_cast<int>(value[i]), static_cast<int>(entry.data.u8[i]));
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }
        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Camera3Stream::CheckKeySetInResult(uint32_t tag, const int32_t * value, float errorMarginRate, int frameNum)
    {
        const camera_metadata_t* metaFromFile = Camera3Device::GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("Meta read from file is null");
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("find_camera_metadata_entry returned ERROR or empty entry");
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
                    NT_LOG_DEBUG("Expected value: %d with margin delta: %d, got: %d", static_cast<int>(value[i]), errorMarginDelta, static_cast<int>(entry.data.i32[i]));
                    free(const_cast<camera_metadata_t*>(metaFromFile));
                    return false;
                }
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;

    }

    bool Camera3Stream::CheckKeySetInResult(uint32_t tag, const float * value, int frameNum)
    {

        const camera_metadata_t* metaFromFile = Camera3Device::GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("Meta read from file is null");
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("find_camera_metadata_entry returned ERROR or empty entry");
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.f[i] != value[i])
            {
                NT_LOG_ERROR("Expected value: %f, got: %f", value[i], entry.data.f[i]);
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Camera3Stream::CheckKeySetInResult(uint32_t tag, const int64_t * value, float errorMarginRate, int frameNum)
    {

        const camera_metadata_t* metaFromFile = Camera3Device::GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("Meta read from file is null");
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("find_camera_metadata_entry returned ERROR or empty entry");
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
                    NT_LOG_ERROR("Expected value: %d with margin delta: %d, got: %d", value[i], errorMarginDelta, entry.data.i64[i]);
                    free(const_cast<camera_metadata_t*>(metaFromFile));
                    return false;
                }
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Camera3Stream::CheckKeySetInResult(uint32_t tag, const double * value, int frameNum)
    {
        const camera_metadata_t* metaFromFile = Camera3Device::GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("Meta read from file is null");
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("find_camera_metadata_entry returned ERROR or empty entry");
            free(const_cast<camera_metadata_t*>(metaFromFile));
            return false;
        }

        size_t i = 0;
        while (i < entry.count)
        {
            if (entry.data.d[i] != value[i])
            {
                NT_LOG_ERROR("Expected value: %d, got: %d", value[i], entry.data.d[i]);
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }

    bool Camera3Stream::CheckKeySetInResult(uint32_t tag, const camera_metadata_rational_t * value, int frameNum)
    {
        const camera_metadata_t* metaFromFile = Camera3Device::GetCameraMetadataByFrame(frameNum);

        if (metaFromFile == nullptr)
        {
            NT_LOG_ERROR("Meta read from file is null");
            return false;
        }

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
            tag, &entry);
        if (ret != 0 || entry.count == 0)
        {
            NT_LOG_ERROR("find_camera_metadata_entry returned ERROR or empty entry");
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
                NT_LOG_ERROR("Expected Rational [%d, %d], got [%d, %d]", value[i].numerator, value[i].denominator,
                    entry.data.r[i].numerator, entry.data.r[i].denominator);
                free(const_cast<camera_metadata_t*>(metaFromFile));
                return false;
            }
            i++;
        }

        free(const_cast<camera_metadata_t*>(metaFromFile));
        return true;
    }
    /**************************************************************************************************************************
    *   Camera3Stream::ProcessCaptureRequestWith3AConverged()
    *
    *   @brief
    *       Process capture request when 3A are all converged. This function will try to use an existing HAL request to perform
    *       captures. Otherwise this function will create and destroy its own HAL requests as needed.
    *
    *   @return
    *       int frame number when 3A are converged, -1 otherwise
    **************************************************************************************************************************/
    int Camera3Stream::ProcessCaptureRequestWith3AConverged(
        int cameraId,           //[in] id of the target camera
        uint32_t frameNumber,   //[in] frame number to start with
        int requestType,        //[in] request template type
        int outputStreamIndex,  //[in] stream to use for the capture
        bool bDumpOutputBuffer, //[in] whether to dump the output capture
        const char * fileName)  //[in] filename for the output capture dump
    {

        Camera3Device::isAEConverged = false;
        Camera3Device::isAFConverged = false;
        Camera3Device::isAWBConverged = false;

        // For loop used to limit the number of attempts to converge
        for (uint32_t fn = frameNumber; fn <= MAX_REPEAT_NUM + frameNumber; fn++)
        {
            if (!ProcessCaptureRequest(
                cameraId,
                fn,
                requestType,
                outputStreamIndex,
                bDumpOutputBuffer,
                fileName,
                true))
            {
                NT_LOG_ERROR("There are no more buffers to send next request, so fail the test");
                return -1;
            }

            if (Camera3Device::isAEConverged &&
                Camera3Device::isAFConverged &&
                Camera3Device::isAWBConverged)
            {
                NT_LOG_INFO("3A stats converged");
                return fn;
            }

        }

        return -1;
    }

    /**************************************************************************************************************************
    *   Camera3Stream::GetGrallocSize()
    *
    *   @brief
    *       Determines the gralloc resolution for a given format. Gralloc dimensions may be different due to requirement of the
    *       format.
    *
    *   @return
    *       Size struct containing the gralloc resolution
    **************************************************************************************************************************/
    Size Camera3Stream::GetGrallocSize(
        Size resolution,    //[in] resolution of the image
        int outputFormat,   //[in] format of the image
        int cameraId)       //[in] camera Id
    {
        Size grallocResolution;
        // Gralloc implementation maps Raw Opaque to BLOB
        if (outputFormat == HAL_PIXEL_FORMAT_BLOB ||
            outputFormat == HAL_PIXEL_FORMAT_RAW_OPAQUE)
        {
            Size defaultGrallocRes = Size(static_cast<uint32_t>(
                resolution.width * resolution.height * 3 + sizeof(camera3_jpeg_blob)), 1);

            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);

            if (pCamInfo == nullptr)
            {
                NT_LOG_WARN("Failed to calculate Jpeg gralloc size!");
                return defaultGrallocRes;
            }

            size_t jpegBufferSize = Camera3Device::getJpegBufferSize(resolution.width, resolution.height, *pCamInfo);
            if (jpegBufferSize == static_cast<size_t>(-1))
            {
                NT_LOG_WARN("Failed to calculate Jpeg gralloc size!");
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

    /***************************************************************************************************************************
    *   Camera3Stream::ProcessCaptureRequest()
    *
    *   @brief
    *       Creates a HAL request (if needed), attaches a buffer, and sends a process_capture_request to the driver. The HAL
    *       request will then be deleted if this function created one. Otherwise, this function will assume the caller will
    *       cleanup the HAL request.
    *   @return
    *       bool false to terminate the test, true to continue sending
    ***************************************************************************************************************************/
    bool Camera3Stream::ProcessCaptureRequest(
        int cameraId,           //[in] id for the target camera
        uint32_t frameNumber,   //[in] frame number for this capture
        int requestType,        //[in] request template type
        int outputStreamIndex,  //[in] stream to use for this capture
        bool bDumpOutputBuffer, //[in] whether to dump the output capture
        const char * fileName,  //[in] filename for the output capture dump
        bool bStartStream)     //[in] start streaming right away)
    {
        // Decides if the HAL request should be deleted at the end of this function
        bool cleanupHal = false;

        // If no existing HAL request, then initialize one
        if (GetHalRequest(cameraId) == nullptr)
        {
            NT_LOG_DEBUG("No existing HAL request, creating a new one");
            InitHalRequest(cameraId, frameNumber, nullptr, &m_pOutBuffersPerRequest[outputStreamIndex], 1, requestType);
            cleanupHal = true;
        }
        else
            NT_LOG_DEBUG("Using existing HAL request.");

        // Assign output buffers to HAL request
        GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(outputStreamIndex, frameNumber);
        if (GetHalRequest(cameraId)->output_buffers == nullptr)
        {
            NT_LOG_ERROR("Failed to get output buffer for frame number: %d", frameNumber);
            return false;
        }

        if (isHighspeedSession)
        {
            SetKey(cameraId, ANDROID_CONTROL_AE_TARGET_FPS_RANGE, previewFpsRange, 2);
        }

        if (bStartStream)
        {
            if(CDKResultSuccess != Camera3Stream::StreamStarter(cameraId, frameNumber, bDumpOutputBuffer, outputStreamIndex, fileName))
               return false;
        }

        if (cleanupHal)
        {
            DeleteHalRequest(cameraId);
        }

        return true;
    }

    /**************************************************************************************************************************
    *   Camera3Stream::InitHalRequest()
    *
    *   @brief
    *       Sets up HAL request object for submitting to process
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::InitHalRequest(
        int cameraId,                           //[in] camera Id
        int frameNumber,                        //[in] frame number
        camera3_stream_buffer_t * inputBuffer,  //[in] location of buffer where driver should get input from
        camera3_stream_buffer_t * outputBuffer, //[in] location of buffer where driver should output
        int num_output_buffers,                 //[in] number of output buffers
        int templateType)                       //[in] template to provide the default metadata settings
    {
        halRequest[cameraId] = SAFE_NEW() camera3_capture_request_t();
        halRequest[cameraId]->frame_number = frameNumber;
        halRequest[cameraId]->input_buffer = inputBuffer;
        halRequest[cameraId]->num_output_buffers = num_output_buffers;
        halRequest[cameraId]->output_buffers = outputBuffer;

        camera3_device_t* pCamDev = GetCamDevice(cameraId);
        NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
        const camera_metadata_t *settings = pCamDev->ops->construct_default_request_settings(pCamDev, templateType);
        NT_ASSERT(NULL != settings, "Camera settings for the template type are NULL for frame %d", frameNumber);
        halRequest[cameraId]->settings = clone_camera_metadata(settings);
    }

    /**************************************************************************************************************************
    *   Camera3Stream::DeleteHalRequest()
    *
    *   @brief
    *       Deletes an existing HAL request for the given camera
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::DeleteHalRequest(
        int cameraId)   //[in] camera Id for which to delete HAL request
    {
        if (halRequest[cameraId] != nullptr)
        {
            NT_LOG_DEBUG("Deleting HAL request..");
            if (halRequest[cameraId]->settings != nullptr)
            {
                free_camera_metadata(const_cast<camera_metadata_t *>(halRequest[cameraId]->settings));
            }
            delete halRequest[cameraId];
            halRequest[cameraId] = nullptr;
        }
    }

    /**************************************************************************************************************************
    *   Camera3Stream::StartStreaming()
    *
    *   @brief
    *       Calls process_capture_request with the current halRequest object. Wait for the capture result to complete and dump
    *       output buffer if needed.
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::StartStreaming(
        int cameraId,           //[in] camera Id
        uint32_t frameNumber,   //[in] frame number
        bool bDumpOutputBuffer, //[in] decides if output buffer should be dumped to an image file
        int outputStreamIndex,  //[in] selects desired output stream (starts at 0)
        const char * fileName)  //[in] file name to be used if capture is to be dumped to a file
    {
        NT_ASSERT(halRequest[cameraId] != nullptr,"No hal request object!");
        NT_ASSERT(halRequest[cameraId]->num_output_buffers > 0, "Hal request number of output buffers(%d) must be greater than 0!"
            ,halRequest[cameraId]->num_output_buffers);
        NT_ASSERT(halRequest[cameraId]->output_buffers != nullptr,"Hal request output buffers are null!");
        isCaptureResultsReadyArray[cameraId] = false;

        nInputBuffersRemaining[cameraId] = (nullptr != halRequest[cameraId]->input_buffer) ? 1 : 0;
        nOutputBuffersRemaining[cameraId]   = halRequest[cameraId]->num_output_buffers;
        nPartialResultsRemaining[cameraId] = GetPartialResultCount(cameraId);

        halRequest[cameraId]->frame_number = frameNumber;

        // Remove metadata for same frame if it exists
        ClearMetadataMap(cameraId);

        /* START critical section - hal request */
        std::unique_lock<std::mutex> halRequestLock(halRequestMutex);

        NT_LOG_DEBUG("Sending process capture request to cameraId %d, frameNumber %d ", cameraId, frameNumber);
        int res = -1;
        camera3_device_t* pCamDev = GetCamDevice(cameraId);
        NT_ASSERT(nullptr != pCamDev, "Unable to get camera device for Camera ID: %d", cameraId);
        res = pCamDev->ops->process_capture_request(pCamDev, halRequest[cameraId]);
        // Driver overwrites frameNumber in request to >800, need this to write back to original number
        halRequest[cameraId]->frame_number = frameNumber;
        NT_ASSERT(res == 0,"process_capture_request failed");

        halRequestLock.unlock();
        /* END critical section - hal request */

        if (IsFlushTestFrame())
        {
            int flushRes = TestFlush(cameraId);
            NT_ASSERT(flushRes == 0,"flush request failed");
        }

        /* START critical section - result ready */
        std::unique_lock<std::mutex> resultReadyLock(Camera3Device::resultReadyMutexArray[cameraId]);
        int retries = 0;
        while (!isCaptureResultsReadyArray[cameraId] && retries < MAX_WAIT_RETRIES) {
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
            NT_LOG_ERROR("Failed to obtain output buffer(s)/metadata(s) after several retries for camera:"
                " %d\n", cameraId);
        }
        else
        {
            // If this was a flushed frame...
            if (IsFlushTestFrame())
            {
                // If buffer error or request error notify occurred...
                if (CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_REQUEST) ||
                    CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_BUFFER))
                {
                    NT_LOG_INFO("Notify from flush for frame %u found", frameNumber);
                    // Then expect that buffer status was bad
                    NT_EXPECT((CAMERA3_BUFFER_STATUS_ERROR == mOutputBufferStatus[FRAME_REQUEST_MOD(frameNumber)])
                        , "Buffer status of flushed frame should be CAMERA3_BUFFER_STATUS_ERROR (%d), but got %d instead !"
                        , CAMERA3_BUFFER_STATUS_ERROR, mOutputBufferStatus[FRAME_REQUEST_MOD(frameNumber)]);
                }
                else
                {
                    NT_LOG_WARN("Notify from flush for frame %u did not occur!", frameNumber);
                }
            }
        }
        NT_ASSERT(isCaptureResultsReadyArray[cameraId],
            "Failed to obtain output buffer(s)/metadata(s) after several retries for camera: %d", cameraId);

        Camera3Device::isCaptureResultsReadyArray[cameraId] = false;
        resultReadyLock.unlock();
        /* END critical section - result ready */

        if (triggerFlush)
        {
            FlushStream(cameraId);
            NT_LOG_ERROR("Capture results did not return before timeout! Flush was triggered.");
        }

        if (bDumpOutputBuffer)
        {
            DumpBuffer(outputStreamIndex, frameNumber, fileName);
        }

    }

    /**************************************************************************************************************************
    *   Camera3Stream::DumpBuffer()
    *
    *   @brief
    *       Dump content from output buffer to an image file
    *
    *   @return
    *       void
    **************************************************************************************************************************/
    void Camera3Stream::DumpBuffer(
        int streamIndex,            //[in] selects desired output stream to dump
        int frameNumber,            //[in] frame number
        const char* fileName) const //[in] file name of the resulting image file
    {
        if (HalCmdLineParser::g_noImageDump)
        {
            NT_LOG_WARN("Camera3Stream: Result image dump is disabled, skipping...");
            return;
        }

        NT_ASSERT(fileName != nullptr,"filename is null");

        NT_LOG_DEBUG("Camera3Stream: Saving image to file: %s", fileName);

        int res;

#ifdef _LINUX
        sp<GraphicBuffer> outNativeBuffer;
#else
        std::shared_ptr<CamxBuffer> outNativeBuffer;
#endif

        outNativeBuffer = m_ppOutNativeBuffers[streamIndex][frameNumber % GetPipelineDepth()];

        if(outNativeBuffer == nullptr)
        {
            NT_LOG_ERROR("outNativeBuffer is NULL for given stream index: [%d] and frame number: [%d]",
                streamIndex, frameNumber);
            return;
        }

#ifdef _LINUX

        char* outputBufferPtr = nullptr;

        res = outNativeBuffer->lock(GRALLOC_USAGE_SW_READ_OFTEN, reinterpret_cast<void**>(&outputBufferPtr));
        NT_ASSERT(res == 0,"outputNativeBuffer lock failed");

        // Ensure capture folder gets created or exists
        // Make upper camera folder first
        if (mkdir(sImageUpperPath.c_str(), 0777) != 0)
        {
            NT_ASSERT(EEXIST == errno,"Failed to create capture camera folder, Error: %d", errno);
        }

        // Make lower nativetest folder second
        if (mkdir(sImageRootPath.c_str(), 0777) != 0)
        {
            NT_ASSERT(EEXIST == errno, "Failed to create capture root folder, Error: %d" ,errno);
        }
#else
        buffer_handle_t* pBuffer = outNativeBuffer->GetNativeHandle();
        char* outputBufferPtr = *reinterpret_cast<char**>(const_cast<native_handle_t *>(*pBuffer)->data);

        // make sure capture folder gets created or exists
        if (_mkdir(sImageRootPath.c_str()) != 0)
        {
            DWORD error = GetLastError();
            NT_ASSERT(ERROR_ALREADY_EXISTS == error,"Failed to create capture folder, Error: %d", error);
        }

#endif

        int output_stride = outNativeBuffer->getStride();
        int output_height = outNativeBuffer->getHeight();
        int output_width  = outNativeBuffer->getWidth();

        char out_fname[256];
        snprintf(out_fname, sizeof(out_fname), "%s", fileName);

        FILE* pFile = fopen(out_fname,"wb");
        NT_ASSERT(pFile!=nullptr,"Output file creation failed");

        size_t written;

        int format = GetConfigedStreams().at(streamIndex)->format;

        if (format == HAL_PIXEL_FORMAT_YCbCr_420_888 ||
            format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED)
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
            NT_ASSERT(written == 1,"Output Y plane write failed obtained:");

            // note buffer created on windows doesn't have same offset as gralloc
            written = fwrite(outputBufferPtr + UV_offset, UV_size, 1, pFile);
            NT_ASSERT(written == 1,"Output UV plane write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_RAW16)
        {
            int size = output_stride * output_height * 2;
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_ASSERT(written == 1,"Output RAW_16 write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_RAW_OPAQUE)
        {
            int size = output_stride * output_height;
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_ASSERT(written == 1,"Output RAW_OPAQUE write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_RAW10)
        {
            int size = output_stride * output_height * (5/4);
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_ASSERT(written == 1, "Output RAW_10 write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_BLOB)
        {
            // get the actual size of data in buffer
            int maxSize = output_stride * output_height * 4;
            int size = GetJpegTrimmedBufferSize(outputBufferPtr, maxSize);
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_ASSERT(written == 1, "Output JPEG write failed");
        }
        else if (format == HAL_PIXEL_FORMAT_Y16)
        {
            int size = output_stride * output_height * 2;
            written = fwrite(outputBufferPtr, size, 1, pFile);
            NT_ASSERT(written == 1, "Output Y16 write failed");
        }
#ifdef _LINUX
        res = outNativeBuffer->unlock();
        NT_ASSERT(res == 0,"outputNativeBuffer unlock failed");
#endif //_LINUX
        res = fclose(pFile);
        NT_ASSERT(res == 0,"Close file failed");
    }

    /**************************************************************************************************************************
    *   Camera3Stream::GetJpegTrimmedBufferSize()
    *
    *   @brief
    *       Returns the actual size of data in buffer after finding end of JPEG data (0xFF,0xD9)
    *
    *   @return
    *        void
    **************************************************************************************************************************/
    int Camera3Stream::GetJpegTrimmedBufferSize(char* currentBufferPtr, int maxSize)
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

    /**************************************************************************************************************************
    *   Camera3Stream::FillNV12Buffer()
    *
    *   @brief
    *       Fill an input buffer with a four color test image
    *
    *   @return
    *        void
    **************************************************************************************************************************/
    void Camera3Stream::FillNV12Buffer(
        int streamIndex, //[in] stream index of buffer to load image into
        int frameNumber) //[in] frame number of the hal request
    {
        int res;
        char* inputBufferPtr;

#ifdef _LINUX
        sp<GraphicBuffer> inputNativeBuffer;
#else
        std::shared_ptr<CamxBuffer> inputNativeBuffer;
#endif

        inputNativeBuffer = m_ppOutNativeBuffers[streamIndex][frameNumber % GetPipelineDepth()];
        NT_ASSERT(inputNativeBuffer != nullptr, "Input Native buffer is null");

        int input_stride = inputNativeBuffer->getStride();
        int input_height = inputNativeBuffer->getHeight();
        int input_width = inputNativeBuffer->getWidth();

        int Y_size = input_width * input_height;
        int UV_size = input_width * (input_height / 2);

#ifdef _LINUX
        res = inputNativeBuffer->lock(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN, reinterpret_cast<void**>(&inputBufferPtr));
        NT_ASSERT(res == 0,"inputNativeBuffer lock failed");
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
    /**
    * Get the configed stream
    */
    std::vector<std::shared_ptr<camera3_stream_t>> Camera3Stream::GetConfigedStreams() const
    {
        return  mStreams;
    }

    /**
    * Clears the output stream vector
    */
    void Camera3Stream::ClearOutStreams()
    {

        ClearOutputBuffers();
        mStreams.clear();

        if (!isHighspeedSession && !allocatedStreamsforHFR.empty())
        {
            allocatedStreamsforHFR.clear();
        }
    }

    /**************************************************************************************************************************
    *   Camera3Stream::CreateOutputStreamBuffer()
    *
    *   @brief
    *       Creates an output stream buffer
    *
    *   @return
    *       void
    **************************************************************************************************************************/
#ifdef _LINUX
    void  Camera3Stream::CreateOutputStreamBuffer(
        camera3_stream_buffer_t &outputBuffer,  //[out] location for the new stream buffer
        int outputStreamIndex,                  //[in] stream to use for the initial capture from sensor
        Size outputResolution,                  //[in] resolution of the initial capture from sensor
        int outputFormat,                       //[in] format for the initial capture from sensor
        sp<GraphicBuffer>* outputNativeBuffer,  //[out] location for the new native buffer
        int cameraId)                           //[in] camera Id
#else
    void  Camera3Stream::CreateOutputStreamBuffer(
        camera3_stream_buffer_t &outputBuffer,              //[out] location for the new stream buffer
        int outputStreamIndex,                              //[in] stream to use for the initial capture from sensor
        Size outputResolution,                              //[in] resolution of the initial capture from sensor
        int outputFormat,                                   //[in] format for the initial capture from sensor
        std::shared_ptr<CamxBuffer>* outputNativeBuffer,    //[out] location for the new native buffer
        int cameraId)                                       //[in] camera Id

#endif
    {
        outputBuffer.stream = mStreams.at(outputStreamIndex).get();
        Size grallocDimensions = GetGrallocSize(outputResolution, outputFormat, cameraId);

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

    /***************************************************************************************************
    *   Camera3Stream::CreateOutputBuffers()
    *
    *   @brief
    *       Helper function to generate an array of buffers for each camera stream
    *
    *   @return
    *       int 0 if successful, -1 otherwise
    ****************************************************************************************************/
    int Camera3Stream::CreateOutputBuffers(
        int cameraId)   //[in] camera Id
    {
        int num_streams = static_cast<int>(GetConfigedStreams().size());

        try
        {
            // First we create a buffer array for each configured stream
            m_ppOutBuffers          = SAFE_NEW() camera3_stream_buffer_t*[num_streams];
            m_pOutBuffersPerRequest = SAFE_NEW() camera3_stream_buffer_t[num_streams];
#ifdef _LINUX
            m_ppOutNativeBuffers    = SAFE_NEW() sp<GraphicBuffer>*[num_streams];
#else
            m_ppOutNativeBuffers    = SAFE_NEW() std::shared_ptr<CamxBuffer>*[num_streams];
#endif
            /*
             * For each configured stream we create "pipelineDepth" number of buffers
             * and initialze the buffer objects with corresponding stream parameters
             */
            for (int streamIndex = 0; streamIndex < num_streams; streamIndex++)
            {
                m_ppOutBuffers[streamIndex]       = SAFE_NEW() camera3_stream_buffer_t[GetPipelineDepth()];
#ifdef _LINUX
                m_ppOutNativeBuffers[streamIndex] = SAFE_NEW() sp<GraphicBuffer>[GetPipelineDepth()];
#else
                m_ppOutNativeBuffers[streamIndex] = SAFE_NEW() std::shared_ptr<CamxBuffer>[GetPipelineDepth()];
#endif
                camera3_stream_t* curStream = GetConfigedStreams().at(streamIndex).get();
                Size curResolution(curStream->width, curStream->height);
                int curFormat = curStream->format;

                for (int bufferIndex = 0; bufferIndex < GetPipelineDepth(); bufferIndex++)
                {
                    CreateOutputStreamBuffer(m_ppOutBuffers[streamIndex][bufferIndex], streamIndex, curResolution, curFormat,
                        &(m_ppOutNativeBuffers[streamIndex][bufferIndex]), cameraId);
                }
            }
        }
        catch(std::bad_alloc ex)
        {
            NT_LOG_ERROR("Bad memory allocated exception thrown!");
            return -1;
        }

        return 0;
    }

    /***************************************************************************************************
    *   Camera3Stream::CreateCaptureRequestWithMultBuffers()
    *
    *   @brief
    *       Helper function to create capture request with multiple buffers
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Stream::CreateCaptureRequestWithMultBuffers(
        int cameraId,            //[in] camera Id
        int streamStartIndex,    //[in] start index of streams
        int requestTypes,        //[in] capture template type
        int num_streams,         //[in] number of streams
        int frameNumber)         //[in] framenumber for the request
    {

        for (int streamIndex = 0; streamIndex < num_streams; streamIndex++)
        {
            NT_ASSERT(GetBufferForRequest(streamStartIndex + streamIndex, frameNumber) != nullptr, "GetBufferForRequest failed");
        }

        InitHalRequest(cameraId, frameNumber, nullptr, &m_pOutBuffersPerRequest[streamStartIndex], num_streams, requestTypes);
    }

    /***************************************************************************************************
    *   Camera3Stream::ClearOutputBuffers()
    *
    *   @brief
    *       Clears all output buffers created in CreateOutputBuffers()
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Stream::ClearOutputBuffers()
    {

        for (size_t streamIndex = 0; streamIndex < GetConfigedStreams().size(); streamIndex++)
        {
            for(int bufferIndex = 0; bufferIndex < GetPipelineDepth(); bufferIndex++)
            {
#ifdef _LINUX
                m_ppOutNativeBuffers[streamIndex][bufferIndex].clear();
#else
                m_ppOutNativeBuffers[streamIndex][bufferIndex]->FreeBuffer();
#endif //_LINUX
            }

            delete[] m_ppOutBuffers[streamIndex];
            delete[] m_ppOutNativeBuffers[streamIndex];
        }
        delete[] m_ppOutBuffers;
        delete[] m_ppOutNativeBuffers;
        delete[] m_pOutBuffersPerRequest;
    }

    /**************************************************************************************************************************
    *   Camera3Stream::LoadImageToBuffer()
    *
    *   @brief
    *       Loads specified image file to native buffer
    *
    *   @return
    *        0 on success
    *       -1 on failure
    **************************************************************************************************************************/
    int Camera3Stream::LoadImageToBuffer(
        const char* imageFile, //[in] name of image to load
        int streamIndex,       //[in] stream index of buffer to load image into
        int frameNumber)       //[in] frame number of the hal request
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
            pOutNativeBuffer = m_ppOutNativeBuffers[streamIndex][frameNumber % GetPipelineDepth()];
            if (pOutNativeBuffer == nullptr)
            {
                NT_LOG_ERROR("Failed to get pOutNativeBuffer from circular buffer array!");
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
                NT_LOG_ERROR("Failed to acquire/allocate buffer byteBuf for loading image!");
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

    /***************************************************************************************************
    *   Camera3Stream::SetHighSpeedSession()
    *
    *   @brief
    *       Sets the boolean to indicate high speed capture session
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Stream::SetHighSpeedSession()
    {
        isHighspeedSession = true;
    }

    /***************************************************************************************************
    *   Camera3Stream::SetPreviewFPSforHFR()
    *
    *   @brief
    *       Sets the FPS range for preview only mode in High speed session
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Stream::SetPreviewFPSforHFR(
        int32_t fpsMin, //[in] preview fps minimum to set
        int32_t fpsMax) //[in] preview fps maximum to set
    {
        previewFpsRange[0] = fpsMin;
        previewFpsRange[1] = fpsMax;
    }

    /***************************************************************************************************
    *   Camera3Stream::IsHighSpeedSession()
    *
    *   @brief
    *       Returns the boolean to indicate high speed capture session
    *
    *   @return
    *       bool
    ****************************************************************************************************/
    bool Camera3Stream::IsHighSpeedSession() const
    {
        return isHighspeedSession;
    }
    /***************************************************************************************************
    *   Camera3Stream::ClearHighSpeedSession()
    *
    *   @brief
    *       Clears the boolean to indicate end of high speed capture session
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Stream::ClearHighSpeedSession()
    {
        isHighspeedSession = false;
    }

}  // namespace haltests
