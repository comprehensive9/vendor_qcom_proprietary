////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  pip_camera_test.cpp
/// @brief Definitions for the Picture-in-Picture test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "pip_camera_test.h"

namespace chitests
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setup
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void PIPCameraTest::Setup()
    {
        for (int camIter = 0; camIter < MAX_CAMERAS; camIter++)
        {
            m_streamInfo[camIter].streamIds   = m_streamId[camIter];
            m_streamInfo[camIter].directions  = reinterpret_cast<camera3_stream_type_t*>(m_direction[camIter]);
            m_streamInfo[camIter].formats     = reinterpret_cast<android_pixel_format_t*>(m_format[camIter]);
            m_streamInfo[camIter].resolutions = m_resolution[camIter];
            m_streamInfo[camIter].usageFlags  = m_usageFlag[camIter];
            m_streamInfo[camIter].isRealtime  = m_isRealtime[camIter];
            m_streamInfo[camIter].isJpeg      = true;
            m_numStreams[camIter]             = 0;
            m_streamBufferMap[camIter].clear();
            m_streamIdMap[camIter].clear();
        }

        ChiTestCase::Setup();
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::TestPIPCamera
    *
    *   @brief
    *       Test multiple realtime pipelines (multiple sessions) depending on pipelineType
    *   @param
    *       [in] TestType             testType       Test identifier
    *   @return
    *       None
    **************************************************************************************************************************/
    void PIPCameraTest::TestPIPCamera(TestId testType)
    {
        m_testId = testType;
        ChiTestCase::sbIsCameraIDSessionID = true;
        Setup();

        // Continue only if more than one camera found in the camera list
        int numOfCameras = m_pChiModule->GetNumberCameras();
        NT_ASSERT(numOfCameras > 1, "PIP camera test requires at least 2 cameras. Device reported %d cameras! Aborting.", numOfCameras);

        for (uint32_t currCamera = 0;
            (currCamera < static_cast<uint32_t>(numOfCameras)) && (m_cameraList.size() < 2); currCamera++)
        {
            const CHICAMERAINFO* chiInfo;
            chiInfo = m_pChiModule->GetCameraInfo(currCamera);
            NT_ASSERT(nullptr != chiInfo, "Can't get camera info for %d", currCamera);
            camera_info* info = reinterpret_cast<camera_info*>(chiInfo->pLegacy);
            int facing = info->facing;
            bool isDepth = m_pChiMetadataUtil->CheckAvailableCapability(currCamera,
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT);
            NT_LOG_INFO("filtering camera ID %d based on facing: %d and depth capability: %d", currCamera, facing, isDepth);
            /*
            if((facing == CAMERA_FACING_FRONT) || isDepth)
            {
                NT_LOG_INFO("skipping camera %d", currCamera);
                continue;
            }
            */
            NT_LOG_INFO("pushing camera %d", currCamera);
            m_cameraList.push_back(currCamera);
        }

        //1. Setup streams for each camera
        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            NT_ASSERT(SetupStreamsForCamera(*pCamID) == CDKResultSuccess, "Fatal failure at SetupStreamsForCamera() camId %d", *pCamID);
        }

        //2. Get sensor mode for each camera
        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            ChiSensorModeInfo* currentCamSensorMode = m_pChiModule->GetClosestSensorMode(*pCamID, m_streamInfo[*pCamID].maxRes);

            NT_ASSERT(currentCamSensorMode != nullptr, "Sensor mode is NULL for camera Id: %d", *pCamID); // check if sensor mode is not NULL

            NT_LOG_INFO("Camera %d chosen sensor mode: %u, width: %u, height: %u", *pCamID,
                currentCamSensorMode->modeIndex, currentCamSensorMode->frameDimension.width, currentCamSensorMode->frameDimension.height);
            currentCamSensorMode->batchedFrames = 1;

            // For RDI cases, we need to make sure stream resolution matches sensor mode
            for (int streamIndex = 0; streamIndex < m_numStreams[*pCamID]; streamIndex++)
            {

                if ((IsRDIStream(m_pRequiredStreams[*pCamID][streamIndex].format)) &&
                        (!IsSensorModeMaxResRawUnsupportedTFEPort(m_streamId[*pCamID][streamIndex])))
                {
                    if (!m_userRDISize.isInvalidSize())
                    {
                        NT_LOG_INFO("OVERRIDE: RDI resolution set by User width: %d, height: %d ",
                                m_userRDISize.width, m_userRDISize.height);
                        m_pRequiredStreams[*pCamID][streamIndex].width  = m_userRDISize.width;
                        m_pRequiredStreams[*pCamID][streamIndex].height = m_userRDISize.height;
                    }
                    else if (!m_isCamIFTestGen)
                    {
                        NT_LOG_INFO("Selected RDI resolution based on Sensor mode width: %d, height: %d ",
                                currentCamSensorMode->frameDimension.width, currentCamSensorMode->frameDimension.height);
                        m_pRequiredStreams[*pCamID][streamIndex].width  = currentCamSensorMode->frameDimension.width;
                        m_pRequiredStreams[*pCamID][streamIndex].height = currentCamSensorMode->frameDimension.height;
                    }
                }
            }
        }

        //3. Setup PIP camera pipelines
        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            NT_ASSERT(SetupPIPCameraPipelines(*pCamID) == CDKResultSuccess, "Pipelines Setup failed");
        }

        //4. Create new session per camera
        NT_ASSERT(CreateSessions(m_cameraList) == CDKResultSuccess, "Necessary session could not be created!");

        //5. Initialize buffer manager and metabuffer pools for each camera
        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            NT_ASSERT(InitializeBufferManagers(*pCamID) == CDKResultSuccess, "Buffer managers for camera %d could not be initialized!", *pCamID);
        }

        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            // Input metabuffer
            NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(*pCamID, MAX_REQUESTS) == CDKResultSuccess,
                "Input metabuffer pool for camera Id: %d could not be created!", *pCamID);
        }
        // Output metabuffer
        NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
            "Output metabuffer pool could not be created!");
        //// @todo: In case of multiple cameras, is there a need for output meta buffer pool separately for each camera?

        //6. Create and submit capture requests
        /*
        Example: For 3 cameras:
        * Default requested frames per camera = 20
        * Total capture requests to be sent to driver = (20 x 3 = 60 capture requests in all)
        * Capture requests are sent as batches : {1,1,1} , {2,2,2}, so on till {20,20,20}
        */

        // Iterate over frames asked to submit at runtime for each camera
        for (UINT32 frameIndex = START_FRAME; frameIndex <= m_captureCount; frameIndex++)
        {
            // generate capture request for current batch of capture requests
            NT_ASSERT(GeneratePIPCaptureRequest(m_cameraList, frameIndex) == CDKResultSuccess,
                    "Submit pipeline request failed for multi capture frame request %d", frameIndex);
        }

        //7. Wait for all results
        NT_ASSERT(WaitForResults() == CDKResultSuccess, "There are pending buffers not returned by driver after several retries");

        //8. Destroy all resources created
        DestroyResources(m_cameraList);

    }

    /**************************************************************************************************************************
    *   PIPCameraTest::SetupStreamsForCamera()
    *
    *   @brief
    *       Overridden function implementation which defines the stream information based on the test Id
    *   @param
    *       [in]     int     camID    ID of the camera we want to Setup streams for
    *   @return
    *       CDKResult success if stream objects could be created or failure
    **************************************************************************************************************************/
    CDKResult PIPCameraTest::SetupStreamsForCamera(int camID)
    {
        CDKResult       result           = CDKResultSuccess;
        int             streamIndex      = 0;
        CHISTREAMFORMAT noFormatOverride = static_cast<CHISTREAMFORMAT>(-1);

        switch (m_testId)
        {
        case PIPCameraIFEFull:
            // Preview stream
            m_resolution[camID][streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            m_direction[camID][streamIndex]  = ChiStreamTypeOutput;
            m_format[camID][streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[camID][streamIndex]   = IFEOutputPortFull;

            m_numStreams[camID]              = streamIndex + 1;
            m_streamInfo[camID].num_streams  = m_numStreams[camID];
            break;

        case PIPCameraTFEFull:
            // Preview stream
            if (IsRDIStream(m_format[camID][streamIndex]) && (!m_userRDISize.isInvalidSize()))
            {
                m_resolution[camID][streamIndex] = m_userRDISize;
            }
            else
            {
                m_resolution[camID][streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
            }
            m_direction[camID][streamIndex]  = ChiStreamTypeOutput;
            m_format[camID][streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRawOpaque : m_userPrvFormat;
            m_streamId[camID][streamIndex]   = TFEOutputPortFull;

            m_numStreams[camID]              = streamIndex + 1;
            m_streamInfo[camID].num_streams  = m_numStreams[camID];
            break;
        case PIPCameraIFEGeneric:

            //1. Configurable output stream
            m_direction[camID][streamIndex]  = ChiStreamTypeOutput;
            m_format[camID][streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            if (IsRDIStream(m_format[camID][streamIndex]) && (!m_userRDISize.isInvalidSize()))
            {
                m_resolution[camID][streamIndex] = m_userRDISize;
            }
            else
            {
                m_resolution[camID][streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
            }
            m_streamId[camID][streamIndex]   = CONFIG_OUTPUT;
            m_numStreams[camID]              = streamIndex + 1;
            m_streamInfo[camID].num_streams = m_numStreams[camID];
        break;

        default:
            NT_LOG_ERROR("Unknown testcase Id provided: [%d]", m_testId);
            result = CDKResultENoSuch;
            break;
        }

        if (m_streamInfo[camID].num_streams <= 0)
        {
            NT_LOG_ERROR("Number of streams should be greater than 0, given: %d", m_streamInfo[camID].num_streams);
            result = CDKResultEFailed;
        }

        if (result == CDKResultSuccess)
        {
            m_pRequiredStreams[camID] = SAFE_NEW() CHISTREAM[m_streamInfo[camID].num_streams];
            if (NULL == m_pRequiredStreams[camID])
            {
                NT_LOG_ERROR("Failed to initialize CHI streams");
                result = CDKResultEFailed;
            }
            else
            {
                camera3_stream_t* pCamStreams = CreateStreams(m_streamInfo[camID]);
                if (nullptr == pCamStreams)
                {
                    NT_LOG_ERROR("Failed to created streams");
                    result = CDKResultEFailed;
                }
                else
                {
                    ConvertCamera3StreamToChiStream(pCamStreams, m_pRequiredStreams[camID], m_streamInfo[camID].num_streams);
                    delete[] pCamStreams;   // No longer needed, converted to m_pRequiredStreams

                    if (m_pRequiredStreams[camID] == nullptr)
                    {
                        NT_LOG_ERROR("Provided streams could not be configured for testcase Id: [%d]", m_testId);
                        result = CDKResultEFailed;
                    }
                    else
                    {
                        for (int index = 0; index < m_numStreams[camID]; index++)
                        {
                            m_isRealtime[camID][index] = true;
                            m_streamIdMap[camID][m_streamInfo[camID].streamIds[index]] = &m_pRequiredStreams[camID][index];
                        }
                        if (m_streamIdMap[camID].size() != static_cast<size_t>(m_numStreams[camID]))
                        {
                            result = CDKResultEFailed;
                        }
                    }
                }
            }
        }

        return result;
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::SetupPIPCameraPipelines()
    *
    *   @brief
    *       Creates pipeline objects for PIP cameras based on the test Id
    *   @param
    *       [in]  int      cameraId          Camera ID to be associated with the pipeline
    *   @return
    *       CDKResult success if pipelines could be created or failure
    **************************************************************************************************************************/
    CDKResult PIPCameraTest::SetupPIPCameraPipelines(
        int cameraId)
    {
        CDKResult result = CDKResultSuccess;
        switch (m_testId)
        {
        case PIPCameraIFEFull:
            m_pCameraChiPipeline[cameraId] = ChiPipeline::Create(cameraId, m_pChiModule->m_pSelectedSensorInfo[cameraId], m_streamIdMap[cameraId], PipelineType::RealtimeIFE);
            break;
        case PIPCameraTFEFull:
            m_pCameraChiPipeline[cameraId] = ChiPipeline::Create(cameraId, m_pChiModule->m_pSelectedSensorInfo[cameraId], m_streamIdMap[cameraId], PipelineType::RealtimeTFE);
            break;
        case PIPCameraIFEGeneric:
            m_pCameraChiPipeline[cameraId] = ChiPipeline::Create(cameraId, m_pChiModule->m_pSelectedSensorInfo[cameraId], m_streamIdMap[cameraId], PipelineType::RealtimeIFEGeneric);
            break;
        default:
            NT_LOG_ERROR("Unknown testcase Id provided: [%d]", m_testId);
            return CDKResultENoSuch;
        }

        result |= (m_pCameraChiPipeline[cameraId] != nullptr) ? m_pCameraChiPipeline[cameraId]->CreatePipelineDesc() : CDKResultEInvalidPointer;

        return result;
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::CreateSessions()
    *
    *   @brief
    *       Overridden function implementation which creates required number of sessions based on the test Id
    *   @param
    *       [in] std::vector <UINT32> camList           List of camera IDs to test
    *   @return
    *       CDKResult success if sessions could be created or failure
    **************************************************************************************************************************/
    CDKResult PIPCameraTest::CreateSessions(std::vector <UINT32> camList)
    {
        CDKResult     result    = CDKResultSuccess;
        ChiCallBacks  callbacks = { 0 };

        callbacks.ChiNotify                      = ChiTestCase::ProcessMessage;
        callbacks.ChiProcessCaptureResult        = ChiTestCase::QueueCaptureResult;
        callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

        std::vector <UINT32>::iterator pCamID;
        for (pCamID = camList.begin(); pCamID < camList.end(); pCamID++)
        {
            mPerSessionPvtData[*pCamID].pTestInstance = this;
            mPerSessionPvtData[*pCamID].sessionId     = *pCamID;  // use camera ID as unique session ID
            mPerSessionPvtData[*pCamID].cameraId      = *pCamID;
            mPerSessionPvtData[*pCamID].pChiSession   = ChiSession::Create(&m_pCameraChiPipeline[*pCamID], // pointer to all pipelines for given camera
                                                        1,                        // 1 pipeline per session
                                                        &callbacks,
                                                        &mPerSessionPvtData[*pCamID]);
            mPerSessionPvtData[*pCamID].sessionHandle = mPerSessionPvtData[*pCamID].pChiSession->GetSessionHandle();

            if (mPerSessionPvtData[*pCamID].pChiSession == nullptr)
            {
                NT_LOG_ERROR("Could not create session for camera %d", *pCamID);
                result = CDKResultEFailed;
            }
            else
            {
                if(!m_nEarlyPCR) // if early PCR not enabled, activate pipeline here
                {
                    result = m_pCameraChiPipeline[*pCamID]->ActivatePipeline(mPerSessionPvtData[*pCamID].sessionHandle);
                }

                if (result == CDKResultSuccess)
                {

                    result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                        mPerSessionPvtData[*pCamID].sessionHandle,
                        m_pCameraChiPipeline[*pCamID]->GetPipelineHandle(), &m_pCameraPipelineMetadataInfo);
                }
            }
        }

        return result;
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::CommonProcessCaptureResult()
    *
    *   @brief
    *       Overridden function implementation which processes the capture result obtained from driver
    *   @param
    *        [in]  CHICAPTURERESULT*     pCaptureResult         pointer to capture result
    *        [in]  void*                 pPrivateCallbackData   pointer private callback data
    *   @return
    *       None
    **************************************************************************************************************************/
    //TODO:Move this function to chitestcase.cpp
    void PIPCameraTest::CommonProcessCaptureResult(
        ChiCaptureResult   * pCaptureResult,
        SessionPrivateData * pSessionPrivateData)
    {
        CDKResult result           = CDKResultSuccess;
        uint32_t  resultFrameNum   = pCaptureResult->frameworkFrameNum;
        uint32_t  numOutputBuffers = pCaptureResult->numOutputBuffers;
        UINT32    sessionId        = pSessionPrivateData->sessionId;

        NT_ASSERT(m_pChiMetadataUtil != nullptr, "ChiMetadataUtil is NULL!");

        m_pChiMetadataUtil->VerifyCaptureResultMetadata(static_cast<CHIMETAHANDLE>(pCaptureResult->pOutputMetadata));

        if (sessionId == mPerSessionPvtData[sessionId].sessionId)

        {
            for (uint32_t bufferIndex = 0; bufferIndex < numOutputBuffers; bufferIndex++)
            {
                CDKResult bufferResult = CDKResultSuccess; // reset for each buffer to be processed
                ChiBufferManager::NativeChiBuffer* outBuffer = const_cast<ChiBufferManager::NativeChiBuffer*>(&pCaptureResult->
                    pOutputBuffers[bufferIndex]);
                ChiBufferManager* manager = m_streamBufferMap[sessionId][outBuffer->pStream];
                if (manager == nullptr)
                {
                    NT_LOG_ERROR("Could not find buffer manager for cameraId: %d, given stream: %p, frame: %d",
                        sessionId, outBuffer->pStream, resultFrameNum);
                    result |= CDKResultEInvalidPointer;
                }
                else
                {
                    bool isSnapshotStream = (outBuffer->pStream == m_streamIdMap[sessionId][IFEOutputPortRDI0] ||
                        outBuffer->pStream == m_streamIdMap[sessionId][IFEOutputPortRDI1]);
                    bool isBlobStream = (outBuffer->pStream->format == ChiStreamFormatBlob);
                    // Only dump blob (stats) or raw snapshot streams every 5th frame
                    bool dump = ((isBlobStream || isSnapshotStream) && resultFrameNum % 5 != 0) ? false : true;

                    ChiBufferManager::GenericBuffer genericBuffer(outBuffer);
                    if (manager->ProcessBufferFromResult(resultFrameNum, &genericBuffer, true) != 0)
                    {
                        bufferResult = CDKResultEFailed;
                    }
                    int bufferStatus = manager->GetStatusFromBuffer(&genericBuffer);

                    // If flush triggered
                    if (m_bIsFlushTriggered)
                    {
                        // Check for error notify associated with this frame...
                        if (CheckErrorNotifyCode(resultFrameNum, CHIERRORMESSAGECODE::MessageCodeRequest) ||
                                CheckErrorNotifyCode(resultFrameNum, CHIERRORMESSAGECODE::MessageCodeBuffer, outBuffer->pStream))
                        {
                            // Then check for buffer status error due to flush
                            NT_EXPECT(bufferResult == CDKResultEFailed && bufferStatus == CAMERA3_BUFFER_STATUS_ERROR,
                                    "Expected a buffer status error (1) due to flush. Got status %d instead!", bufferStatus);
                        }
                        else
                        {
                            // Warning that no error notify received for this flushed frame (not a hard requirement)
                            NT_LOG_WARN("Notify from flush for frame %d did not occur!", resultFrameNum);

                        }
                    }
                    if (m_bIsFlushTriggered)
                    {
                        if (!(bufferResult == CDKResultEFailed && bufferStatus == CAMERA3_BUFFER_STATUS_ERROR))
                        {
                            result = bufferResult;
                        }
                    }
                    else if (bufferStatus == CAMERA3_BUFFER_STATUS_ERROR || bufferResult != CDKResultSuccess)
                    {
                        result == bufferResult == CDKResultSuccess ? CDKResultEFailed : bufferResult;
                    }
                }
            }

        }
        else
        {
            NT_LOG_WARN("Unknown Session Id: %d obtained", sessionId);
            result = CDKResultEInvalidState;
        }

        if (result == CDKResultSuccess)
        {
            AtomicDecrement(numOutputBuffers);
            NT_LOG_DEBUG("PIP: Number of pending buffers remaining after receiving the result: %d", GetPendingBufferCount());
        }
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources / objects
    *   @param
    *       None
    *   @return
    *       None
    **************************************************************************************************************************/
    void PIPCameraTest::DestroyResources(std::vector <UINT32> camList)
    {
        // Deactivate pipeline for each camera
        for (std::vector <UINT32>::iterator pCamID = camList.begin(); pCamID < camList.end(); pCamID++)
        {
            if ((m_pCameraChiPipeline[*pCamID] != nullptr) && (mPerSessionPvtData[*pCamID].pChiSession != nullptr))
            {
                m_pCameraChiPipeline[*pCamID]->DeactivatePipeline(mPerSessionPvtData[*pCamID].sessionHandle);
            }

            // Destroy the session
            if (mPerSessionPvtData[*pCamID].pChiSession != nullptr)
            {
                mPerSessionPvtData[*pCamID].pChiSession->DestroySession();
                mPerSessionPvtData[*pCamID].pChiSession = nullptr;
            }

            if (m_pCameraChiPipeline[*pCamID] != nullptr)
            {
                m_pCameraChiPipeline[*pCamID]->DestroyPipeline();
                m_pCameraChiPipeline[*pCamID] = nullptr;
            }
        }
        // Destroy required streams and buffer managers for each camera
        for (std::vector <UINT32>::iterator pCamID = camList.begin(); pCamID < camList.end(); pCamID++)
        {
            for (int streamIndex = 0; streamIndex < m_numStreams[*pCamID]; streamIndex++)
            {
                if (m_streamBufferMap[*pCamID].find(&m_pRequiredStreams[*pCamID][streamIndex]) != m_streamBufferMap[*pCamID].end())
                {
                    ChiBufferManager* manager = m_streamBufferMap[*pCamID].at(&m_pRequiredStreams[*pCamID][streamIndex]);

                    if (manager != nullptr)
                    {
                        manager->DestroyBuffers();
                        m_streamBufferMap[*pCamID].erase(&m_pRequiredStreams[*pCamID][streamIndex]);
                    }
                }
            }

            NT_EXPECT(m_streamBufferMap[*pCamID].empty(), "There are still pending buffermanagers not freed!");

            if (m_pRequiredStreams[*pCamID] != nullptr)
            {
                delete[] m_pRequiredStreams[*pCamID];
                m_pRequiredStreams[*pCamID] = nullptr;
            }

            m_streamIdMap[*pCamID].clear();
            m_numStreams[*pCamID] = 0;
        }
        DestroyCommonResources();
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::InitializeBufferManagers
    *
    *   @brief
    *       Initializes buffer manager for each required stream
    *   @param
    *       [in]    int         cameraId    actual camera ID to test
    *   @return
    *       CDKResult success if buffer manager could be created / failure
    **************************************************************************************************************************/
    CDKResult PIPCameraTest::InitializeBufferManagers(int cameraId)
    {
        CDKResult result = CDKResultSuccess;

        if (m_numStreams[cameraId] <= 0)
        {
            NT_LOG_ERROR("Cannot create buffermanagers with stream count: %d", m_numStreams[cameraId]);
            result = CDKResultENeedMore;
        }

        if (result == CDKResultSuccess)
        {
            for (int streamIndex = 0; streamIndex < m_numStreams[cameraId]; streamIndex++)
            {
                ChiBufferManager::NativeChiStream* currentStream = &m_pRequiredStreams[cameraId][streamIndex];
                ChiBufferManager::GenericStream genericStream(currentStream);

                ChiBufferManager* manager = SAFE_NEW() ChiBufferManager(m_streamId[cameraId][streamIndex]);

                m_pBufferManagers[cameraId][streamIndex] = manager->Initialize(
                    cameraId,
                    &genericStream,
                    m_streamInfo[cameraId].streamIds[streamIndex],
                    m_streamInfo[cameraId].inputImages[streamIndex]);

                if (m_pBufferManagers[cameraId][streamIndex] == nullptr)
                {
                    NT_LOG_ERROR("Failed to allocate memory for buffer manager for stream index: %d", streamIndex);
                    result = CDKResultENoMemory;
                    break;
                }

                m_streamBufferMap[cameraId][currentStream] = m_pBufferManagers[cameraId][streamIndex];
            }
        }

        return result;
    }

    /**************************************************************************************************************************
    *   PIPCameraTest::GeneratePIPCaptureRequest()
    *
    *   @brief
    *       Generate a PIP camera pipeline request
    *   @param
    *       [in] std::vector <UINT32>      camList       list of cameras to test
    *       [in] int                    frameId          Frame number requested
    *   @return
    *       CDKResult success if request could be submitted or failure
    **************************************************************************************************************************/
    CDKResult PIPCameraTest::GeneratePIPCaptureRequest(
        std::vector <UINT32> camList,
        UINT32 frameId)
    {
        CDKResult result   = CDKResultSuccess;

        uint32_t requestId = FRAME_REQUEST_MOD(frameId);
        if (m_flushFrame)
        {
            // Clear notify flag before sending any capture requests
            ClearErrorNotifyCode(frameId);
        }

        std::vector <UINT32>::iterator pCamID;
        for (pCamID = camList.begin(); pCamID < camList.end(); pCamID++)
        {
            memset(&m_pipCamRequest[*pCamID][requestId], 0, sizeof(CHICAPTUREREQUEST));

            static const uint32_t numOutBuffers        = m_numStreams[*pCamID];
            ChiBufferManager::NativeChiBuffer* outBuffers = SAFE_NEW() ChiBufferManager::NativeChiBuffer[numOutBuffers];
            std::map<ChiBufferManager::NativeChiStream*, ChiBufferManager*>::iterator it;
            uint32_t outputIndex;

            for (it = m_streamBufferMap[*pCamID].begin(), outputIndex = 0; it != m_streamBufferMap[*pCamID].end(); ++it, outputIndex++)
            {
                ChiBufferManager* manager = it->second;
                void* buffer = manager->GetOutputBufferForRequest();
                if (buffer == nullptr)
                {
                    delete[] outBuffers;
                    return CDKResultENoMore;
                }

                outBuffers[outputIndex] = *static_cast<ChiBufferManager::NativeChiBuffer*>(buffer);
            }

            m_pipCamRequest[*pCamID][requestId].frameNumber     = frameId;
            m_pipCamRequest[*pCamID][requestId].hPipelineHandle = m_pCameraChiPipeline[*pCamID]->GetPipelineHandle();
            m_pipCamRequest[*pCamID][requestId].numOutputs      = numOutBuffers;
            m_pipCamRequest[*pCamID][requestId].pOutputBuffers  = outBuffers;
            m_pipCamRequest[*pCamID][requestId].pPrivData       = &m_cameraRequestPvtData[*pCamID];

            // Get the input metabuffer from the pool
            m_pipCamRequest[*pCamID][requestId].pInputMetadata = m_pChiMetadataUtil->GetInputMetabufferFromPool(requestId + *pCamID);
            NT_EXPECT(m_pipCamRequest[*pCamID][requestId].pInputMetadata != nullptr, "Failed to create Input Metabuffer before sending request");

            // Get the output metabuffer from the pool
            m_pipCamRequest[*pCamID][requestId].pOutputMetadata = m_pChiMetadataUtil->GetOutputMetabufferFromPool(requestId + *pCamID,
                m_pCameraPipelineMetadataInfo.publishTagArray, m_pCameraPipelineMetadataInfo.publishTagCount);
            NT_EXPECT(m_pipCamRequest[*pCamID][requestId].pOutputMetadata != nullptr, "Failed to create Output Metabuffer before sending request");

            memset(&m_submitRequest[*pCamID][RealtimeSession], 0, sizeof(CHIPIPELINEREQUEST));
            m_submitRequest[*pCamID][RealtimeSession].pSessionHandle = mPerSessionPvtData[*pCamID].sessionHandle;
            m_submitRequest[*pCamID][RealtimeSession].numRequests = 1;
            m_submitRequest[*pCamID][RealtimeSession].pCaptureRequests = &m_pipCamRequest[*pCamID][requestId];

            AtomicIncrement(m_pipCamRequest[*pCamID][requestId].numOutputs);
        }

        for (pCamID = camList.begin(); pCamID < camList.end(); pCamID++)
        {
            result = m_pChiModule->SubmitPipelineRequest(&m_submitRequest[*pCamID][RealtimeSession]);
            // activate pipeline here if EPCR enabled, after sending early requests
            if(result == CDKResultSuccess && frameId == m_nEarlyPCR)
            {
                result = m_pCameraChiPipeline[*pCamID]->ActivatePipeline(mPerSessionPvtData[*pCamID].sessionHandle);
            }
        }

        NT_LOG_DEBUG("Number of pending buffers remaining: %d after sending frame request batch %d",
            GetPendingBufferCount(), frameId);

        // trigger flush after requested frame, if enabled
        if (frameId == m_flushFrame)
        {
            // Set signal flag to flush and notify flushing thread
            SignalFlushThread(SignalFlushTrigger);

            // Ensure flush thread is completely done flushing before continuing
            SignalFlushThread(SignalFlushReady);
        }

        return result;
    }

} // namespace chitests
