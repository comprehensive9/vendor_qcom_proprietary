 //*************************************************************************************************
 // Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
 // All Rights Reserved.
 // Confidential and Proprietary - Qualcomm Technologies, Inc.
 //*************************************************************************************************

#include "camera3device.h"
#include <chrono>

namespace haltests {

    /***************************************************************
     * Default Constructor
     ***************************************************************/

    Camera3Device::Camera3Device() : isAEConverged(false),
                                     isAFConverged(false),
                                     isAWBConverged(false),
                                     mDeviceVersion(0),
                                     mHasInputBufferInRequest(true),
                                     mHasResult(false)
    {
        camera3_callback_ops_t::process_capture_result = &aprocess_capture_result;
        camera3_callback_ops_t::notify = &anotify;

        NATIVETEST_UNUSED_PARAM(mHasResult);
        NATIVETEST_UNUSED_PARAM(mDeviceVersion);
        NATIVETEST_UNUSED_PARAM(mHasInputBufferInRequest);
    }

    /***************************************************************
    * Default Destructor
    ****************************************************************/
    Camera3Device::~Camera3Device()
    {
        for (size_t i=0; i<mCamDevices.size(); i++)
        {
            CloseCamera(mCamDevices.at(i).camId);
        }
        mCamDevices.clear();
    }

    /***************************************************************
    * Gtest setup() invoked before starting the test
    ***************************************************************/
    void Camera3Device::Setup()
    {
        Camera3Module::Setup();
        int camNum = Camera3Module::GetNumCams();
        NT_ASSERT(0 < camNum,"No cameras found");

        for (uint32_t i = 0; i < NATIVETEST_ELEMENTS(mErrorNotifyBitFlag); i++)
        {
            ClearErrorNotifyFlag(i);
        }
    }

    void Camera3Device::Teardown()
    {
        Camera3Module::Teardown();
    }

    /***************************************************************
    * Camera3Device::OpenCamera()
    * @brief
    *     Opens the camera device for a given camerId
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::OpenCamera(
        int cameraId)  //[in] cameraId for opening the device
    {
        NT_LOG_DEBUG("Opening camera: %d", cameraId);
        NT_ASSERT(nullptr != GetCamModule()->common.methods->open, "Camera open() is unimplemented");

        NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info, "get_camera_info is not implemented");

        camera_info info;
        // need to have this to open camera?
        NT_ASSERT(0 == GetCamModule()->get_camera_info(cameraId, &info), "Can't get camera info for camera %d",cameraId);

        hw_device_t* localDevice = nullptr;

        int res = GetCamModule()->common.methods->open(
            (const hw_module_t*)GetCamModule(), std::to_string(cameraId).c_str(), &localDevice);

        if (res == CamxResultETooManyUsers)
        {
           NT_LOG_WARN("Hardware resource insufficient for camera: %d", cameraId);
           return;
        }
        else
        {
            NT_ASSERT(0 == res ,"Can't open camera device %d",cameraId);
        }

        NT_ASSERT(nullptr != localDevice, "Camera open(%d) returned a NULL device ", cameraId);

        NT_ASSERT(kVersion3_0 <= localDevice->version, "The device does not support HAL3");

        camera3_device_t* camDevice = reinterpret_cast<camera3_device_t*>(localDevice);

        mCamDevices.push_back(CamDevice(camDevice, cameraId));

        NT_ASSERT(mCamDevices.size() > 0, "No camera device is available!");

        NT_ASSERT(nullptr != GetCamDevice(cameraId), "Camera device is NULL for camera id %d", cameraId);

        NT_ASSERT(nullptr != camDevice->ops,"Camera device ops are NULL");

        mDeviceVersion = camDevice->common.version;

        // initialize camera
       NT_ASSERT(0 == camDevice->ops->initialize(camDevice, this), "Camera device initializing failed!");

       SetNumPartialResults(cameraId);
       SetPipelineDepth(cameraId);

    }

    /***************************************************************
    * Camera3Device::CloseCamera()
    *
    * @brief
    *     close the camera device for a given camerId
    *
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::CloseCamera(
        int cameraId)   //[in] cameraId for closing the device
    {
        NT_LOG_DEBUG("Closing camera: %d", cameraId);
        PartialMetadataSet* pPartialSet = GetLastPartialMetadata();
        while (NULL != pPartialSet)
        {
            pPartialSet->metadata.ClearMetadata();
            delete pPartialSet;
            pPartialSet = GetLastPartialMetadata();
        }
        mMetadata[cameraId].clear();
        for (uint32_t i = 0; i < mCamDevices.size(); i++)
        {
            CamDevice myDevice = mCamDevices.at(i);
            if (myDevice.camId == cameraId)
            {
                camera3_device_t* device = myDevice.camDevice;

                NT_ASSERT(0 == device->common.close(reinterpret_cast<hw_device_t*>(device)),
                    "Can't close camera deivce %d",cameraId);

                mCamDevices.erase(mCamDevices.begin() + i);
                mPartialResultCount[cameraId] = 0;
                mPipelineDepth = 1;
                break;
            }
        }

    }

    /***************************************************************
    * Camera3Device::ProcessCaptureResult()
    *
    * @brief
    *     Result callback invoked by the driver when returning the
    *     captured image and metadata
    *
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::ProcessCaptureResult(
        const camera3_capture_result *pCaptureResult)  //[out] result returned by the driver
    {
        const uint32_t frameNumber = pCaptureResult->frame_number;
        const uint32_t partialResult = pCaptureResult->partial_result;
        const uint32_t numOutputBufs = pCaptureResult->num_output_buffers;
        const camera_metadata_t* metadata = pCaptureResult->result;

       NT_LOG_INFO("ProcessCaptureResult for frame [%u] with partial result [%u]", frameNumber, partialResult);

        // If more than one camera is open, camId is decided on frameNumber
        int camId = GetCamIdFromFrameNumber(frameNumber);

        if (metadata != nullptr)
        {
            // Clone and push partial metadata result onto queue to analyze later
            PartialMetadataSet* clonedPartial = SAFE_NEW() PartialMetadataSet(frameNumber, partialResult,
                Camera3Metadata(metadata));
            m_partialMetadataQueue.push(clonedPartial);
        }

        // Check result metadata (non-zero partial result should contain metadata)
        if (partialResult != 0)
        {
            // Keep track of partial results received
            nPartialResultsRemaining[camId]--;
            NT_EXPECT(1 <= partialResult && partialResult <= mPartialResultCount[camId],
                "Result is malformed, partial_result (%d ) not in range!", partialResult);

            // Metadata can only be NULL if this was a flush test frame and got an error notify (REQUEST or RESULT)
            NT_ASSERT(metadata != nullptr || (IsFlushTestFrame() && (CheckErrorNotifyFlag(frameNumber,
                CAMERA3_MSG_ERROR_RESULT) || CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_REQUEST))),
                "Capture result metadata is NULL for frame [%d ], partial result [%u]", frameNumber, partialResult);

            // Save metadata for later analysis
            if (metadata != nullptr)
            {
                AddMetadatatoMap(pCaptureResult);
            }
            else
            {
               NT_LOG_WARN("Cannot write metadata for frame %u because it is NULL", frameNumber);
            }

            // Handle 3A
#ifdef ENABLE3A
            if (!isAEConverged)
            {
                IfAEConverged(mMetadata[camId].find(frameNumber)->second.GetCamMetadata());
            }
            if (!isAFConverged)
            {
                IfAFConverged(mMetadata[camId].find(frameNumber)->second.GetCamMetadata());
            }
            if (!isAWBConverged)
            {
                IfAWBConverged(mMetadata[camId].find(frameNumber)->second.GetCamMetadata());
            }
#else
            // Skip convergence check for presil testing or if 3A disabled
            isAEConverged = true;
            isAFConverged = true;
            isAWBConverged = true;
#endif
        }
        else  // partial_result == 0 (result contains buffers only)
        {
            NT_ASSERT((numOutputBufs > 0 && pCaptureResult->output_buffers != nullptr)
                || pCaptureResult->input_buffer != nullptr,"Capture result contains no metadata or buffers!");
        }

        // Keep track of output buffers received
        nOutputBuffersRemaining[camId] -= numOutputBufs;
        NT_LOG_INFO("Number of output buffers in this result: %u", numOutputBufs);
        NT_LOG_INFO("Number of output buffers remaining: %d", nOutputBuffersRemaining[camId]);
        NT_EXPECT(nOutputBuffersRemaining[camId] >= 0,"Too many output buffers were received!");

        // Keep track of input buffers received
        uint32_t numInputBufs = (nullptr != pCaptureResult->input_buffer) ? 1 : 0;
        nInputBuffersRemaining[camId] -= numInputBufs;
        NT_LOG_INFO("Number of input buffers in this result: %u", numInputBufs);
        NT_LOG_INFO("Number of input buffers remaining: %d", nInputBuffersRemaining[camId]);
        NT_EXPECT(nInputBuffersRemaining[camId] >= 0, "Too many input buffers were received!");

        // Expect no more partial results if frame was flushed and error notify was received (RESULT or REQUEST)
        if (IsFlushTestFrame() && (CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_RESULT) ||
            CheckErrorNotifyFlag(frameNumber, CAMERA3_MSG_ERROR_REQUEST)))
        {
           NT_LOG_WARN("No more partial results are expected due to ERROR_RESULT or ERROR_REQUEST");
            nPartialResultsRemaining[camId] = 0;
        }
        NT_LOG_INFO("Number of partial results remaining: %d", nPartialResultsRemaining[camId]);
        NT_EXPECT(nPartialResultsRemaining[camId] >= 0, "Too many partial results were received!");

        // Validate output buffers
        if (numOutputBufs > 0)
        {
            NT_ASSERT(pCaptureResult->output_buffers != nullptr,
                "Capture result reported num_output_buffers [%d ], but output_buffers pointer is NULL!", numOutputBufs);

            // Check status of each output buffer
            for (uint32_t bufIndex = 0; bufIndex < numOutputBufs; bufIndex++)
            {
                // Record buffer status to analyze later
                mOutputBufferStatus[FRAME_REQUEST_MOD(frameNumber)] = pCaptureResult->output_buffers[bufIndex].status;
                NT_LOG_DEBUG("Buffer: %u received in format: %d", bufIndex,
                    pCaptureResult->output_buffers[bufIndex].stream->format);

                // Allow flush test frames to have buffer status error
                if (IsFlushTestFrame())
                {
                    if (pCaptureResult->output_buffers[bufIndex].status == CAMERA3_BUFFER_STATUS_ERROR)
                    {
                       NT_LOG_WARN("Got CAMERA3_BUFFER_STATUS_ERROR for flushed frame [%u], buffer [%u]",
                            frameNumber, bufIndex);
                    }
                }
                else
                {
                    NT_EXPECT(pCaptureResult->output_buffers[bufIndex].status != CAMERA3_BUFFER_STATUS_ERROR,
                        "Got CAMERA3_BUFFER_STATUS_ERROR for frame [%u ], buffer [%d]",frameNumber,bufIndex);
                }
            }
        }

        // Check for all buffers and partial results received
        if (nOutputBuffersRemaining[camId] == 0 && nInputBuffersRemaining[camId] == 0 && nPartialResultsRemaining[camId] < static_cast<int>(mPartialResultCount[camId]))
        {
            NT_LOG_DEBUG("Obtained all input/output buffers and metadata results");

            /* START critical section - protecting isCaptureResultsReadyArray */
            std::unique_lock<std::mutex> resultReadyLock(Camera3Device::resultReadyMutexArray[camId]);
            NT_LOG_DEBUG("Set captureResultsReady to true and unlock thread with condition variable for camera: %d", camId);
            isCaptureResultsReadyArray[camId] = true;

            // Notify that results and output buffers are ready
            resultReadyCondArray[camId].notify_one();
            resultReadyLock.unlock();
            /* END critical section - protecting isCaptureResultsReadyArray */
        }

    }
    /***************************************************************
    * Camera3Device::partialResultsUpdate()
    *
    * @brief
    *     Function to update partial results during batch process
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::partialResultsUpdate(int camId, int batchSize)
    {
        mPartialResultCount[camId] = batchSize * mPartialResultCount[camId];
    }

    /***************************************************************
    * Camera3Device::partialResultResets()
    *
    * @brief
    *     Function resets partial results to original value by dividing by batchSize
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::partialResultsReset(int camId, int batchSize)
    {
        mPartialResultCount[camId] = mPartialResultCount[camId]/batchSize;
    }

    /***************************************************************
    * Camera3Device::sNotify()
    *
    * @brief
    *     Notification callback invoked by the driver to specify
    *     any device sepcific message
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::sNotify(
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
            NT_LOG_ERROR("Notify error message %s (%d) for frame [%u] stream [%pK]",
                mErrorNotifyStrings[errMessage.error_code], errMessage.error_code, errMessage.frame_number,
                errMessage.error_stream);
            break;
        }
        case CAMERA3_MSG_SHUTTER:
        {
            camera3_shutter_msg_t shutterMessage = msg->message.shutter;
           NT_LOG_INFO("Notify shutter message for frame [%u] timestamp %llu", shutterMessage.frame_number,
                shutterMessage.timestamp);
            break;
        }
        default:
            NT_LOG_DEBUG("msg->type = %d", msg->type);
            NT_ASSERT(1 == 0, "Unknown notify message from HAL.");
            break;
        }
    }

    /***************************************************************
    * Camera3Device::ClearErrorNotifyFlag()
    *
    * @brief
    *     Clears the error notify flag bits for given frame number
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::ClearErrorNotifyFlag(uint32_t frameNum)
    {
        mErrorNotifyBitFlag[frameNum % MAX_REQUESTS] &= 0;
    }

    /***************************************************************
    * Camera3Device::CheckErrorNotifyFlag()
    *
    * @brief
    *     Gets the desired bit from the error notify bit flag
    * @return
    *     int bit of the position requested, otherwise -1
    ****************************************************************/
    int Camera3Device::CheckErrorNotifyFlag(uint32_t frameNum, uint32_t pos) const
    {
        if (pos <= 31)
        {
            return mErrorNotifyBitFlag[frameNum % MAX_REQUESTS] & (1 << pos);
        }
        else
        {
            NT_LOG_ERROR("Invalid error notify flag position! %u", pos);
            return -1;
        }
    }

    /***************************************************************
    * Camera3Device::SetErrorNotifyFlag()
    *
    * @brief
    *     Gets the desired bit from the error notify bit flag
    * @return
    *     None
    ****************************************************************/
    void Camera3Device::SetErrorNotifyFlag(uint32_t frameNum, uint32_t pos)
    {
        if (pos <= 31)
        {
            mErrorNotifyBitFlag[frameNum % MAX_REQUESTS] |= (1 << pos);
        }
        else
        {
            NT_LOG_ERROR("Invalid error notify flag position! %u", pos);
        }
    }

    /***********************************************************
     *   Camera3Device::FindCameraMetadatafromTemplate
     *
     *   @brief
     *     Finds a specific medadata based on the given tag and
     *     template
     *   @return
     *      None
     **************************************************************/
    int Camera3Device::FindCameraMetadataFromTemplate(
        camera_metadata_t* templateRequest,  //[in] metadata object
        camera_metadata_tag_t tag)           //[in] tag to be searched
    {
        camera_metadata_entry entry;
        int val = find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(templateRequest),
            tag, &entry);
        return val;
    }

    /*********************************************************
    *   Camera3Device::GetCameraMetadataEntryByTag
    *
    *   @brief
    *     Gets a specific medadata entry based on the given tag
    *   @return
    *     zero if found successfully, non zero otherwise
    ************************************************************/
    int Camera3Device::GetCameraMetadataEntryByTag(
        camera_metadata_t* metadata,     //[in ] metadata object
        camera_metadata_tag_t tag,       //[in ] tag to be searched
        camera_metadata_entry_t *entry)  //[out] entry for the given tag
    {
        int val = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metadata), tag, entry);

        return val;
    }

    /************************************************************************
    *  Camera3Device::GetCameraMetadataByFrame
    *  @brief
    *     Gets metadata of the requested frame from metadata map
    *  @return
    *     copy of camera_metadata_t* of desired frame, nullptr if metadata
    *     not found
    **************************************************************************/
    const camera_metadata_t * Camera3Device::GetCameraMetadataByFrame(
        uint32_t frameNumber) //[in] framenumber for getting corresponding metadata
    {
        int camId = GetCamIdFromFrameNumber(frameNumber);
        NT_LOG_INFO("Find metadata for frame: %d", unsigned(frameNumber));
        if (mMetadata[camId].find(frameNumber) == mMetadata[camId].end())
        {
            return nullptr;
        }
        return mMetadata[camId].find(frameNumber)->second.GetClonedCamMetadata();
    }

    /************************************************************************
    *  Camera3Device::IF3AConverged
    *  @brief
    *     Check if 3A state are converged, which means we can do
    *     still capture or reprocessing
    *  @return
    *     None
    *************************************************************************/
    void Camera3Device::IfAEConverged(
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

    /************************************************************************
    *  Camera3Device::IFAFConverged
    *  @brief
    *     Check if AF state are converged, which means we can do
    *     still capture or reprocessing
    *  @return
    *     None
    *************************************************************************/
    void Camera3Device::IfAFConverged(
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


    /*************************************************************************
    *  Camera3Device::IFAWBConverged
    *  @brief
    *     Check if 3A state are converged, which means we can do
    *     still capture or reprocessing
    *  @return
    *     None
    **************************************************************************/
    void Camera3Device::IfAWBConverged(
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

    /************************************************************************
    *  Camera3Device::GetCamDevice
    *  @brief
    *      Gets the camera device object based on cameraId
    *  @return
    *      camera3_device_t object for given cameraId
    *************************************************************************/
    camera3_device_t* Camera3Device::GetCamDevice(int cameraId)  //[in] id of the camera device to be returned
    {

        for (uint32_t i = 0; i < mCamDevices.size(); i++)
        {
            CamDevice device = mCamDevices.at(i);
            if (device.camId == cameraId)
            {
                if (device.camDevice)
                {
                    return device.camDevice;
                }
            }
        }
        return nullptr;
    }

    /************************************************************************
    *  Camera3Device::GetPartialResultCount
    *  @brief
    *      Get the partial result count based on cameraId
    *  @return
    *      uint32_t partial result count
    *************************************************************************/
    uint32_t Camera3Device::GetPartialResultCount(
        int camId) const  //[in] camera id
    {
        return mPartialResultCount[camId];
    }

    /************************************************************************
    *  Camera3Device::GetPipelineDepth
    *  @brief
    *      Get the pipeline depth
    *  @return
    *      int pipeline depth
    *************************************************************************/
    int Camera3Device::GetPipelineDepth() const
    {
        return static_cast<int>(mPipelineDepth);
    }

    /************************************************************************
    *  Camera3Device::SetNumPartialResults
    *  @brief
    *      Set the partial result count for given cameraId
    *  @return
    *      None
    *************************************************************************/
    void Camera3Device::SetNumPartialResults(
        int camId)  //[in] camera Id
    {
        camera_info *pCamInfo = GetCameraInfoWithId(camId);
        NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", camId);
        camera_metadata_entry *partialResultCount = GetCameraMetadataEntry(pCamInfo, ANDROID_REQUEST_PARTIAL_RESULT_COUNT);

        NT_ASSERT(partialResultCount != nullptr, "Can't find partial result count in static metadata for cameraId: %d", camId);

        NT_ASSERT(partialResultCount->count != 0, "partial result count is zero in static metadata for cameraId: %d", camId);
        mPartialResultCount[camId] = partialResultCount->data.i32[0];

        NT_LOG_DEBUG("Number of partial results for cameraId: %d is: [%d]", camId, mPartialResultCount[camId]);
    }

    /************************************************************************
    *  Camera3Device::SetPipelineDepth
    *  @brief
    *      Set the pipeline depth count for given cameraId
    *  @return
    *      None
    *************************************************************************/
    void Camera3Device::SetPipelineDepth(
        int camId)  //[in] camera Id
    {
        camera_info *pCamInfo = GetCameraInfoWithId(camId);
        NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", camId);
        camera_metadata_entry *pipelineDepth = GetCameraMetadataEntry(pCamInfo, ANDROID_REQUEST_PIPELINE_MAX_DEPTH);

        NT_LOG_DEBUG("Default pipeline depth for cameraId: %d is: [%d]", camId, mPipelineDepth);

        NT_ASSERT(pipelineDepth != nullptr, "Can't find pipeline depth in static metadata for cameraId: %d", camId);

        NT_ASSERT(pipelineDepth->count != 0, "pipeline depth count is zero in static metadata for cameraId: %d", camId);

        mPipelineDepth = pipelineDepth->data.u8[0];

        NT_LOG_DEBUG("pipeline depth from static settings for cameraId: %d is: [%d]", camId, mPipelineDepth);
    }


    /************************************************************************
    *  Camera3Device::AddMetadatatoMap
    *  @brief
    *     Adds metadata from the given frame into the map
    *  @param
    *     Result metadata for the received frame
    *************************************************************************/
    void Camera3Device::AddMetadatatoMap(
        const camera3_capture_result *resultMeta) //[in] metadata to be stored in map
    {
        uint32_t frameNumber = resultMeta->frame_number;
        int camId = GetCamIdFromFrameNumber(frameNumber);
        if (mMetadata[camId].find(frameNumber) == mMetadata[camId].end())
        {
            NT_LOG_DEBUG("Writing new metadata for frame: %d", unsigned(frameNumber));
            mMetadata[camId].insert(std::pair<uint32_t, Camera3Metadata>(frameNumber, Camera3Metadata(resultMeta->result)));
        }
        else
        {
            NT_LOG_DEBUG("Appending metadata for frame: %d", unsigned(frameNumber));
            mMetadata[camId].find(frameNumber)->second.Append(resultMeta->result);
        }
    }

    /************************************************************************
    *  Camera3Device::ClearMetadataMap
    *  @brief
    *     Clears all metadata from the map
    *  @return
    *     None
    *************************************************************************/
    void Camera3Device::ClearMetadataMap(
        int cameraId)           //[in] camera Id
    {
        mMetadata[cameraId].clear();
    }

    /************************************************************************
    *  Camera3Device::GetMaxSize
    *  @brief
    *     Gets the max resolution for a given image format
    *  @return
    *     None
    *************************************************************************/
    Size Camera3Device::GetMaxSize(
        int format,                     //[in] format of the image
        camera_info* info,              //[in] static camera info
        bool usePixelArraySize) const
    {
        if (usePixelArraySize)
        {
            return GetPixelArraySize(const_cast<camera_metadata_t*>(info->static_camera_characteristics));
        }
        return GetMaxResolution(const_cast<camera_metadata_t*>(info->static_camera_characteristics), format);
    }

    /************************************************************************
    *  Camera3Device::getJpegBufferSize
    *  @brief
    *     Gets the jpeg buffer size based on max jpeg resolution
    *  @return
    *     jpeg buffer size
    *************************************************************************/
    size_t Camera3Device::getJpegBufferSize(
        uint32_t width,          //[in] width of jpeg image
        uint32_t height,         //[in] height of jpeg image
        camera_info info) const  //[in] static camera info
    {
        size_t maxJpegBufferSize;
        Size maxJpegResolution =
            GetMaxResolution(const_cast<camera_metadata_t*>(info.static_camera_characteristics), HAL_PIXEL_FORMAT_BLOB);
        if (maxJpegResolution.width == 0 || maxJpegResolution.height == 0)
        {
           NT_LOG_WARN("Can't find valid available jpeg sizes in static metadata!");
            return -1;
        }

        camera_metadata_entry_t jpegBufMaxSize;
        int val = GetCameraMetadataEntryByTag(
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_JPEG_MAX_SIZE,
            &jpegBufMaxSize);
        if (0 != val || jpegBufMaxSize.count == 0)
        {
           NT_LOG_WARN("Can't find the metadata entry for ANDROID_JPEG_MAX_SIZE!");
            return -1;
        }

        maxJpegBufferSize = jpegBufMaxSize.data.i32[0];

        float scaleFactor = static_cast<float>(width * height) / (maxJpegResolution.width * maxJpegResolution.height);
        size_t jpegBufferSize =
            static_cast<size_t>(scaleFactor *  (maxJpegBufferSize - kMinJpegBufferSize) + kMinJpegBufferSize);
        if (jpegBufferSize > maxJpegBufferSize)
        {
            jpegBufferSize = maxJpegBufferSize;
        }

        return jpegBufferSize;
    }

    /*************************************************************************
    * Camera3Device::GetMaxResolution
    * @brief
    *     Finds the maximum resolution from the device static info metadata based
    *     on the given format
    * @return
    *     Max Size if successful or Size(0,0)
    *************************************************************************/
    Size Camera3Device::GetMaxResolution(
        camera_metadata_t* metadata,
        int format) const
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
        int ret = find_camera_metadata_entry(metadata,
                                             streamConfigTag,
                                             &availableStreamConfigs);

        if (ret != 0 || availableStreamConfigs.count == 0)
        {
            NT_LOG_ERROR("Stream configuration map could not be found or is empty!")
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

    /***************************************************************************************************
    *   Camera3Device::SetQueriedMaxResolution()
    *
    *   @brief
    *       When stream resolution is MAX_RES/DEPTH_RES or the format is RAW, this helper function will
    *       query for the maximum resolution and replace the resolution values stored in the
    *       ReprocessItems.
    *
    *   @return
    *        void
    ****************************************************************************************************/
    void Camera3Device::SetQueriedMaxResolution(
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
                camera_info *pCamInfo = GetCameraInfoWithId(cameraId);
                NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);
                streams[index].resolution = GetMaxSize(streams[index].format, pCamInfo, false);

                if (streams[index].resolution.width == 0 && streams[index].resolution.height == 0)
                {
                    NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        streams[index].format, ConvertResToString(streams[index].resolution).c_str(), cameraId);
                }
                else
                {
                    NT_LOG_DEBUG("Max resolution for cameraId: [%d] and format: [%d] is [%s]", cameraId, streams[index].format,
                        ConvertResToString(streams[index].resolution).c_str());
                }
            }
        }
    }

    /***********************************************************************
    * Camera3Device::GetPixelArraySize
    * @brief
    *     Finds the Pixel Array Size from the device static info metadata
    * @return
    *    Pixel array size if successful or Size(0,0)
    ***********************************************************************/
    Size Camera3Device::GetPixelArraySize(
        camera_metadata_t* metadata) //[in] static metadata
    {
        int maxWidth  = 0;
        int maxHeight = 0;
        camera_metadata_entry activeArray;

        //Max raw resolution is sensor active pixel region
        int ret = find_camera_metadata_entry(metadata,
                                             ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
                                             &activeArray);

        if (ret != 0 || activeArray.count == 0)
        {
            NT_LOG_ERROR("Unable to query static metadata ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE" );
        }
        else
        {
            // Sample Layout of android.sensor.info.activeArraySize (f0000): int32[4]
            // [0 0 5488 4112]
            maxWidth =  activeArray.data.i32[2];
            maxHeight = activeArray.data.i32[3];
        }
        return Size(maxWidth, maxHeight);
    }

    /***********************************************************************
    * Camera3Device::CheckAvailableCapability
    * @brief
    *     Checks if given android capability is supported
    * @return
    *    True if supported else false
    *************************************************************************/
    bool Camera3Device::CheckAvailableCapability(
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
            if(entry.data.u8[i] == capability)
            {
                return true;
            }
        }
        return false;
    }

    /***********************************************************************
    * Camera3Device::CheckSupportedInputOutputFormat
    * @brief
    *     Check if a given input format is supported and if the given output
    *     format can be reprocessed from input
    * @return
    *    True if supported else false
    *************************************************************************/
    bool Camera3Device::CheckSupportedInputOutputFormat(
        int cameraId,           //[in] camera id
        int inputFormat,        //[in] input format to be checked
        int outputFormat) const //[in] output format to be checked for given input
    {
        camera_info info;
        if (0 != GetCamModule()->get_camera_info(cameraId, &info))
        {
            NT_LOG_ERROR("Can't get camera info for camera id: %d", cameraId);
            return false;
        }
        camera_metadata_entry_t entry;
        int val = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                                              ANDROID_SCALER_AVAILABLE_INPUT_OUTPUT_FORMATS_MAP,
                                              &entry);
        if (val != 0)
        {
            NT_LOG_ERROR("Can't find the metadata entry for ANDROID_SCALER_AVAILABLE_INPUT_OUTPUT_FORMATS_MAP.");
            return false;
        }

        for (size_t i = 0; i < entry.count; i+=4)
        {
            if (entry.data.i32[i] == inputFormat)
            {
                int formatOffset = static_cast<int>(i + 2);
                int32_t format_count = entry.data.i32[i + 1];
                for (int j = formatOffset; j < (formatOffset + format_count); j++)
                {
                    if (entry.data.i32[j] == outputFormat)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /***********************************************************************
    * Camera3Device::CheckSupportedResolution
    * @brief
    *     Check if a given format and resolution is supported
    * @return
    *    True if supported else false
    *************************************************************************/
    bool Camera3Device::CheckSupportedResolution(
        int cameraId,           //[in] camera id
        int format,             //[in] format to be checked
        Size resolution,        //[in] resolution to be checked
        int direction) const    //[in] direction of the stream
    {
        camera_info info;
        if (0 != GetCamModule()->get_camera_info(cameraId, &info))
        {
            NT_LOG_ERROR("Can't get camera info for camera id: %d", cameraId);
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
            NT_LOG_ERROR("Can't find the metadata entry for %s.", (mbDepthTest) ?
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
                NT_LOG_ERROR("Invalid stream direction: %d", direction);
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
        if(direction == CAMERA3_STREAM_BIDIRECTIONAL && matched >= 2)
        {
            return true;
        }
        // matched may be greater than 1 if there are duplicate entries in dumpsys (CAMX-3410)
        if(direction != CAMERA3_STREAM_BIDIRECTIONAL && matched >= 1)
        {
            return true;
        }
        return false;
    }

    /***********************************************************************
    * aprocess_capture_result
    * @brief
    *      Static callback forwarding method from HAL to instance which is called
    *      when a capture result is ready
    * @return
    *    None
    *************************************************************************/
    void aprocess_capture_result(
        const camera3_callback_ops *cb,        //[in] fn pointer to callbacks
        const camera3_capture_result *result)  //[in] capture result for a request
    {

        Camera3Device *device = const_cast<Camera3Device*>(static_cast<const Camera3Device*>(cb));

        device->ProcessCaptureResult(result);

    }

    /***********************************************************************
    * anotify
    * @brief
    *      Static callback forwarding method from HAL to instance which provides
    *      message notification on events
    * @return
    *    None
    *************************************************************************/
    void anotify(
        const camera3_callback_ops *cb,  //[in] fn pointer to callback methods
        const camera3_notify_msg *msg)   //[in] message notification
    {
        Camera3Device *device =
            const_cast<Camera3Device*>(static_cast<const Camera3Device*>(cb));
        device->sNotify(msg);
    }

    /***********************************************************************
    * GetLastPartialMetadata
    * @brief
    *    Gets the oldest partial metadata not yet analyzed. Caller is
    *    responsible for deleting metadata and struct once done.
    * @return
    *    camera3_capture_result_t*
    *************************************************************************/
    Camera3Device::PartialMetadataSet* Camera3Device::GetLastPartialMetadata()
    {
        if (m_partialMetadataQueue.empty())
        {
            return NULL;
        }
        PartialMetadataSet* retVal = m_partialMetadataQueue.front();
        m_partialMetadataQueue.pop();
        return retVal;
    }

    /***************************************************************************************************
    *   Camera3Device::SetFlushTestFrame()
    *
    *   @brief
    *       Sets the boolean to indicate flush test frame
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Device::SetFlushTestFrame()
    {
        isFlushTestFrame = true;
    }

    /***************************************************************************************************
    *   Camera3Device::IsFlushTestFrame()
    *
    *   @brief
    *       Returns the boolean to indicate flush test frame
    *
    *   @return
    *       bool
    ****************************************************************************************************/
    bool Camera3Device::IsFlushTestFrame() const
    {
        return isFlushTestFrame;
    }

    /***************************************************************************************************
    *   Camera3Device::ClearFlushTestFrame()
    *
    *   @brief
    *       Clears the boolean to indicate end of flush test frame
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Device::ClearFlushTestFrame()
    {
        isFlushTestFrame = false;
    }

    /***************************************************************************************************
    *   Camera3Device::GetCamIdFromFrameNumber()
    *
    *   @brief
    *       Returns the camera Id based on the frame number.
    *
    *   @return
    *       camera ID associated with this frame
    ****************************************************************************************************/
    int Camera3Device::GetCamIdFromFrameNumber(
        uint32_t frameNumber) //[in] framenumber for getting camera Id
    {
        int openCams = static_cast<int>(mCamDevices.size());
        int camId = (openCams > 1) ? ((frameNumber - 1) % openCams) : mCamDevices.at(0).camId;
        return camId;
    }

    /***************************************************************************************************
    *   Camera3Device::SetDepthTest()
    *
    *   @brief
    *       Sets the depth test flag
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void Camera3Device::SetDepthTest(bool bDepthTest)
    {
        mbDepthTest = bDepthTest;
    }

    }  // namespace haltests


