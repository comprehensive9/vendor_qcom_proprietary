////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  multi_camera_test.cpp
/// @brief Definitions for the MultiCamera test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "multi_camera_test.h"

namespace chitests
{
    // static members
    std::vector<MultiCameraTest::MultiCameraPCRequest> MultiCameraTest::m_PCReqTracker;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setup
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void MultiCameraTest::Setup()
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupStreamsForCamera
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult MultiCameraTest::SetupStreamsForCamera(int camIter)
    {
        CDKResult       result           = CDKResultSuccess;
        CHISTREAMFORMAT noFormatOverride = static_cast<CHISTREAMFORMAT>(-1);
        int             streamIndex      = 0;

        switch (m_testId)
        {
        case MultiCameraIFEFull:
        case MultiCameraIFEFullFlushSinglePipeline:
            // output stream
            m_resolution[camIter][streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            m_direction[camIter][streamIndex]  = ChiStreamTypeOutput;
            m_format[camIter][streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[camIter][streamIndex]   = CONFIG_OUTPUT;

            m_numStreams[camIter]              = streamIndex + 1;
            m_streamInfo[camIter].num_streams  = m_numStreams[camIter];
            break;
        case MultiCameraTFEFull:
            // output stream
            m_direction[camIter][streamIndex]  = ChiStreamTypeOutput;
            m_format[camIter][streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRawOpaque : m_userPrvFormat;
            if (IsRDIStream(m_format[camIter][streamIndex]) && (!m_userRDISize.isInvalidSize()))
            {
                m_resolution[camIter][streamIndex] = m_userRDISize;
            }
            else
            {
                m_resolution[camIter][streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
            }
            m_streamId[camIter][streamIndex]   = TFEOutputPortFull;
            m_numStreams[camIter]              = streamIndex + 1;
            m_streamInfo[camIter].num_streams  = m_numStreams[camIter];
            break;
        default:
            NT_LOG_ERROR("Unknown testcase Id provided: [%d]", m_testId);
            result = CDKResultENoSuch;
            break;
        }

        if (m_streamInfo[camIter].num_streams <= 0)
        {
            NT_LOG_ERROR("Number of streams should be greater than 0, given: %d", m_streamInfo[camIter].num_streams);
            result = CDKResultEFailed;
        }

        if (result == CDKResultSuccess)
        {
            m_pRequiredStreams[camIter] = SAFE_NEW() CHISTREAM[m_streamInfo[camIter].num_streams];
            if (NULL == m_pRequiredStreams[camIter])
            {
                NT_LOG_ERROR("Failed to initialize CHI streams");
                result = CDKResultEFailed;
            }
            else
            {
                camera3_stream_t* pCamStreams = CreateStreams(m_streamInfo[camIter]);
                if (nullptr == pCamStreams)
                {
                    NT_LOG_ERROR("Failed to created streams");
                    result = CDKResultEFailed;
                }
                else
                {
                    ConvertCamera3StreamToChiStream(pCamStreams, m_pRequiredStreams[camIter], m_streamInfo[camIter].num_streams);
                    delete[] pCamStreams;   // No longer needed, converted to m_pRequiredStreams

                    if (m_pRequiredStreams[camIter] == nullptr)
                    {
                        NT_LOG_ERROR("Provided streams could not be configured for testcase Id: [%d]", m_testId);
                        result = CDKResultEFailed;
                    }
                    else
                    {
                        for (int index = 0; index < m_numStreams[camIter]; index++)
                        {
                            m_isRealtime[camIter][index] = true;
                            m_streamIdMap[camIter][m_streamInfo[camIter].streamIds[index]] = &m_pRequiredStreams[camIter][index];
                        }
                        if (m_streamIdMap[camIter].size() != static_cast<size_t>(m_numStreams[camIter]))
                        {
                            result = CDKResultEFailed;
                        }
                    }
                }
            }
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupMultiCameraPipelines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult MultiCameraTest::SetupMultiCameraPipelines(int cameraId)
    {
        CDKResult result = CDKResultSuccess;
        switch (m_testId)
        {
        case MultiCameraIFEFull:
        case MultiCameraIFEFullFlushSinglePipeline:
            m_pChiPipeline[cameraId] = ChiPipeline::Create(cameraId, m_pChiModule->m_pSelectedSensorInfo[cameraId], m_streamIdMap[cameraId], PipelineType::RealtimeIFE);
            break;
        case MultiCameraTFEFull:
            m_pChiPipeline[cameraId] = ChiPipeline::Create(cameraId, m_pChiModule->m_pSelectedSensorInfo[cameraId], m_streamIdMap[cameraId], PipelineType::RealtimeTFE);
            break;
        default:
            NT_LOG_ERROR("Unknown testcase Id provided: [%d]", m_testId);
            return CDKResultENoSuch;
        }

        result = (m_pChiPipeline[cameraId] != nullptr) ? m_pChiPipeline[cameraId]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateSessions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult MultiCameraTest::CreateSession(std::vector <UINT32> cameraList)
    {
        CDKResult result = CDKResultSuccess;
        ChiCallBacks callbacks = { 0 };

        callbacks.ChiNotify                      = ChiTestCase::ProcessMessage;
        callbacks.ChiProcessCaptureResult        = ChiTestCase::QueueCaptureResult;
        callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

        mPerSessionPvtData[RealtimeSession].pTestInstance = this;
        mPerSessionPvtData[RealtimeSession].sessionId     = RealtimeSession;    // to enforce realtime session only
        mPerSessionPvtData[RealtimeSession].cameraId      = FIXED_INDEX;        // needed to access mpChiSessions later

        // Create single session encompassing all cameras with multiple pipelines
        // do not use camera ID with m_pChiSessions; using FIXED_INDEX to enforce single session for all cameras
        mPerSessionPvtData[RealtimeSession].pChiSession   = ChiSession::Create(m_pChiPipeline,
                                                            m_cameraList.size(),
                                                            &callbacks,
                                                            &mPerSessionPvtData[RealtimeSession]);
        mPerSessionPvtData[RealtimeSession].sessionHandle = mPerSessionPvtData[RealtimeSession].pChiSession->GetSessionHandle();

        // Activate all pipelines for the session
        if (mPerSessionPvtData[RealtimeSession].pChiSession == nullptr)
        {
            NT_LOG_ERROR("Multi camera session could not be created");
            result = CDKResultEFailed;
        }
        else
        {
            std::vector <UINT32>::iterator pCamID;
            for (pCamID=cameraList.begin(); pCamID<cameraList.end(); pCamID++)
            {
                if(!m_nEarlyPCR) // if early PCR not enabled, activate pipeline here
                {
                    result |= m_pChiPipeline[*pCamID]->ActivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
                }

                if (result == CDKResultSuccess)
                {
                    result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                                mPerSessionPvtData[RealtimeSession].sessionHandle,
                                m_pChiPipeline[*pCamID]->GetPipelineHandle(), &m_pCameraPipelineMetadataInfo);
                }
            }
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetCamIdFromStream
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int MultiCameraTest::GetCamIdFromStream(CHISTREAM* pStream)
    {
        std::vector<MultiCameraPCRequest>::iterator pPCReqItr;

        for (pPCReqItr = m_PCReqTracker.begin(); pPCReqItr != m_PCReqTracker.end(); pPCReqItr++)
        {
            if (pPCReqItr->pStream == pStream)
            {
                return pPCReqItr->camId;
            }
        }

        return INVALID_CAMID_INT;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CommonProcessCaptureResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //TODO: Move this function to chitestcase.cpp
    void MultiCameraTest::CommonProcessCaptureResult(
        ChiCaptureResult * pCaptureResult,
        SessionPrivateData * pSessionPrivateData)
    {
        NT_ASSERT(pCaptureResult != nullptr, "pCaptureResult is null. Aborting!");

        CDKResult result           = CDKResultSuccess;
        CDKResult bufferResult     = CDKResultSuccess;
        uint32_t  resultFrameNum   = pCaptureResult->frameworkFrameNum;
        uint32_t  numOutputBuffers = pCaptureResult->numOutputBuffers;
        SessionId sessionId        = static_cast<SessionId>(pSessionPrivateData->sessionId);

        if (m_pChiMetadataUtil != nullptr)
        {
            m_pChiMetadataUtil->VerifyCaptureResultMetadata(static_cast<CHIMETAHANDLE>(pCaptureResult->pOutputMetadata));
        }

        // For valid buffer result, get camera ID and process output buffers
        if(numOutputBuffers > 0)
        {
            NT_ASSERT(pCaptureResult->pOutputBuffers->pStream != nullptr, "pStream is null for frame result %u", resultFrameNum);

            int camID = GetCamIdFromStream(pCaptureResult->pOutputBuffers->pStream);
            NT_ASSERT(camID != INVALID_CAMID_INT, "Invalid camera ID for frame result %u", resultFrameNum);
            mCurrentResultFrame[camID]       = static_cast<UINT64>(resultFrameNum);

            if (sessionId == RealtimeSession)
            {
                for (uint32_t bufferIndex = 0; bufferIndex < numOutputBuffers; bufferIndex++)
                {
                    ChiBufferManager::NativeChiBuffer* outBuffer = const_cast<ChiBufferManager::NativeChiBuffer*>(&pCaptureResult->
                        pOutputBuffers[bufferIndex]);
                    ChiBufferManager* manager = m_streamBufferMap[camID][outBuffer->pStream];

                    if (manager == nullptr)
                    {
                        NT_LOG_ERROR("Could not find buffer manger for cameraId: %d, given stream: %p, frame: %d",
                            camID, outBuffer->pStream, resultFrameNum);
                        result = CDKResultEInvalidPointer;
                    }
                    else
                    {
                        bool isSnapshotStream = (outBuffer->pStream == m_streamIdMap[camID][IFEOutputPortRDI0] ||
                            outBuffer->pStream == m_streamIdMap[camID][IFEOutputPortRDI1] ||
                            outBuffer->pStream == m_streamIdMap[camID][TFEOutputPortRDI0] ||
                            outBuffer->pStream == m_streamIdMap[camID][TFEOutputPortRDI1]);
                        bool isBlobStream = (outBuffer->pStream->format == ChiStreamFormatBlob);
                        // Only dump blob (stats) or raw snapshot streams every 5th frame
                        bool dump = ((isBlobStream || isSnapshotStream) && resultFrameNum % 5 != 0) ? false : true;

                        ChiBufferManager::GenericBuffer genericBuffer(outBuffer);

                        // process image buffer from the result
                        if (manager->ProcessBufferFromResult(resultFrameNum, &genericBuffer, dump) != 0)
                        {
                            bufferResult = CDKResultEFailed;
                        }
                        int bufferStatus = manager->GetStatusFromBuffer(&genericBuffer);

                        // If flush triggered for the frame
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
                        // If frame was flushed, allow buffer status to be error, otherwise just get result like normal
                        result = (m_bIsFlushTriggered && bufferResult == CDKResultEFailed &&
                            bufferStatus == CAMERA3_BUFFER_STATUS_ERROR) ?
                            CDKResultSuccess : bufferResult;
                    }
                }
            }
            else
            {
                NT_LOG_ERROR("Unknown Session Id: %d obtained", sessionId);
                result = CDKResultEInvalidState;
            }

            if (result == CDKResultSuccess)
            {
                AtomicDecrement(numOutputBuffers);
                NT_LOG_DEBUG("MC: Number of pending buffers remaining after receiving the result: %d", GetPendingBufferCount());
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DestroyResources
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void MultiCameraTest::DestroyResources(std::vector <UINT32> cameraList)
    {
        // Deactivate pipeline for each camera
        for (size_t camIter = 0; camIter < cameraList.size(); camIter++)
        {
            if ((m_pChiPipeline[camIter] != nullptr) && (nullptr != mPerSessionPvtData[RealtimeSession].pChiSession))
            {
                m_pChiPipeline[camIter]->DeactivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
            }
        }
        // Destroy the session
        if (mPerSessionPvtData[RealtimeSession].pChiSession != nullptr)
        {
            mPerSessionPvtData[RealtimeSession].pChiSession->DestroySession();
            mPerSessionPvtData[RealtimeSession].pChiSession = nullptr;
        }
        for (size_t camIter = 0; camIter < cameraList.size(); camIter++)
        {
            if (m_pChiPipeline[camIter] != nullptr)
            {
                m_pChiPipeline[camIter]->DestroyPipeline();
                m_pChiPipeline[camIter] = nullptr;
            }
        }
        // Destroy required streams and buffer managers for each camera
        for (size_t camIter = 0; camIter < cameraList.size(); camIter++)
        {
            for (int streamIndex = 0; streamIndex < m_numStreams[camIter]; streamIndex++)
            {
                if (m_streamBufferMap[camIter].find(&m_pRequiredStreams[camIter][streamIndex]) != m_streamBufferMap[camIter].end())
                {
                    ChiBufferManager* manager = m_streamBufferMap[camIter].at(&m_pRequiredStreams[camIter][streamIndex]);

                    if (manager != nullptr)
                    {
                        manager->DestroyBuffers();
                        m_streamBufferMap[camIter].erase(&m_pRequiredStreams[camIter][streamIndex]);
                    }
                }
            }

            NT_EXPECT(m_streamBufferMap[camIter].empty(), "There are still pending buffermanagers not freed!");

            if (m_pRequiredStreams[camIter] != nullptr)
            {
                delete[] m_pRequiredStreams[camIter];
                m_pRequiredStreams[camIter] = nullptr;
            }

            m_streamIdMap[camIter].clear();
            m_numStreams[camIter] = 0;
        }
        DestroyCommonResources();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InitializeBufferManagers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult MultiCameraTest::InitializeBufferManagers(int cameraId)
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GenerateMultiCameraCaptureRequest
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult MultiCameraTest::GenerateMultiCameraCaptureRequest(
        std::vector <UINT32> cameraList,
        UINT32 frameId)
    {
        CDKResult result = CDKResultSuccess;

        uint32_t requestId = FRAME_REQUEST_MOD(frameId);

        if (m_flushFrame)
        {
            // Clear notify flag before sending any capture requests
            ClearErrorNotifyCode(frameId);
        }

        std::vector <UINT32>::iterator pCamID;
        for (pCamID = cameraList.begin(); pCamID < cameraList.end(); pCamID++)
        {
            memset(&m_multiCamRequest[*pCamID][requestId], 0, sizeof(CHICAPTUREREQUEST));

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

            m_multiCamRequest[*pCamID][requestId].frameNumber     = frameId;
            m_multiCamRequest[*pCamID][requestId].hPipelineHandle = m_pChiPipeline[*pCamID]->GetPipelineHandle();
            m_multiCamRequest[*pCamID][requestId].numOutputs      = numOutBuffers;
            m_multiCamRequest[*pCamID][requestId].pOutputBuffers  = outBuffers;
            m_multiCamRequest[*pCamID][requestId].pPrivData       = &m_cameraRequestPvtData[*pCamID];

            // Create new PC request per camera and add to tracker
            MultiCameraPCRequest PCReq;
            PCReq.camId       = *pCamID;
            PCReq.frameNumber = frameId;
            PCReq.pStream     = outBuffers->pStream; // use stream ID as unique identifier to match PC request with PC result
            m_PCReqTracker.push_back(PCReq);

            // Get the input metabuffer from the pool
            m_multiCamRequest[*pCamID][requestId].pInputMetadata = m_pChiMetadataUtil->GetInputMetabufferFromPool(requestId + *pCamID);
            NT_EXPECT(m_multiCamRequest[*pCamID][requestId].pInputMetadata != nullptr, "Failed to create Input Metabuffer before sending request");

            // Get the output metabuffer from the pool
            m_multiCamRequest[*pCamID][requestId].pOutputMetadata = m_pChiMetadataUtil->GetOutputMetabufferFromPool(requestId + *pCamID,
                m_pCameraPipelineMetadataInfo.publishTagArray, m_pCameraPipelineMetadataInfo.publishTagCount);
            NT_EXPECT(m_multiCamRequest[*pCamID][requestId].pOutputMetadata != nullptr, "Failed to create Output Metabuffer before sending request");
        }

        for (pCamID = cameraList.begin(); pCamID < cameraList.end(); pCamID++)
        {
            memset(&m_submitRequest[*pCamID][RealtimeSession], 0, sizeof(CHIPIPELINEREQUEST));
            // using FIXED_INDEX to enforce single session for all cameras
            m_submitRequest[*pCamID][RealtimeSession].pSessionHandle   = mPerSessionPvtData[RealtimeSession].sessionHandle;
            m_submitRequest[*pCamID][RealtimeSession].numRequests      = 1;
            m_submitRequest[*pCamID][RealtimeSession].pCaptureRequests = &m_multiCamRequest[*pCamID][requestId];

            // submit the process capture request
            CDKResult submitResult = m_pChiModule->SubmitPipelineRequest(&m_submitRequest[*pCamID][RealtimeSession]);
            if (CDKResultSuccess == submitResult)
            {
                // Only increment pending buffer count if submit pipeline request succeeds
                AtomicIncrement(m_multiCamRequest[*pCamID][requestId].numOutputs);
                NT_LOG_DEBUG("Number of pending buffers remaining: %d after sending request for frame: %" PRIu64,
                    GetPendingBufferCount(), frameId);
            }
            else
            {
                NT_LOG_ERROR("SubmitPipelineRequest to driver failed!");
            }

            // Activate pipeline here if EPCR enabled, after sending early requests
            if(submitResult == CDKResultSuccess && frameId == m_nEarlyPCR)
            {
                submitResult = m_pChiPipeline[*pCamID]->ActivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
            }

            // Ignore failures from SubmitPipelineRequest if flush has already been triggered
            result |= (m_bIsFlushTriggered) ? CDKResultSuccess : submitResult;
        }

        // trigger flush after requested frame, if enabled
        if (m_flushFrame == frameId)
        {
            // Set signal flag to flush and notify flushing thread
            SignalFlushThread(SignalFlushTrigger);

            // Ensure flush thread is completely done flushing before continuing
            SignalFlushThread(SignalFlushReady);
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TestMultiCamera
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void MultiCameraTest::TestMultiCamera(TestId testType)
    {
        m_testId = testType;
        Setup();

        // Continue only if more than one camera found in the camera list
        int numOfCameras = m_pChiModule->GetNumberCameras();
        NT_ASSERT(numOfCameras > 1, "multi camera test requires at least 2 cameras. Device reported %d cameras! Aborting.", numOfCameras);

        //Ignore user input for number of cameras and use all available physical cameras
        for (UINT32 currCamera = 0;
            (currCamera < static_cast<UINT32>(numOfCameras)) && (m_cameraList.size() < MAX_PHY_CAMERAS); currCamera++)
        {
            const CHICAMERAINFO* chiInfo;
            chiInfo = m_pChiModule->GetCameraInfo(currCamera);
            NT_ASSERT(nullptr != chiInfo, "Can't get camera info for %d", currCamera);
            camera_info* info = reinterpret_cast<camera_info*>(chiInfo->pLegacy);
            int facing = info->facing;
            bool isDepth = m_pChiMetadataUtil->CheckAvailableCapability(currCamera,
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT);
            NT_LOG_INFO("filtering camera ID %d based on facing: %d and depth capability: %d", currCamera, facing, isDepth);
            if((facing == CAMERA_FACING_FRONT) || isDepth)
            {
                NT_LOG_INFO("skipping camera %d", currCamera);
                continue;
            }
            NT_LOG_INFO("pushing camera %d", currCamera);
            m_cameraList.push_back(currCamera);
        }

        NT_LOG_WARN("Can test only upto max available physical cameras : %d", MAX_PHY_CAMERAS);

        //1. Setup streams for each camera
        for (std::vector <UINT32>::iterator pCamID =m_cameraList.begin(); pCamID< m_cameraList.end(); pCamID++)
        {
            NT_ASSERT(SetupStreamsForCamera(*pCamID) == CDKResultSuccess, "Fatal failure at SetupStreamsForCamera() camId %d",*pCamID);
        }

        //2. Get sensor mode for each camera and store in a list
        for (std::vector <UINT32>::iterator pCamID =m_cameraList.begin(); pCamID< m_cameraList.end(); pCamID++)
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

        //3. Setup multi camera pipelines
        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            NT_ASSERT(SetupMultiCameraPipelines(*pCamID) == CDKResultSuccess, "Pipelines Setup failed");
        }

        //4. Create single session
        NT_ASSERT(CreateSession(m_cameraList) == CDKResultSuccess,"Necessary session could not be created");

        //5. Initialize buffer manager and metabuffer pools for each camera
        for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
        {
            NT_ASSERT(InitializeBufferManagers(*pCamID) == CDKResultSuccess, "Buffer managers for camera %d could not be initialized!", *pCamID);
        }

        NT_ASSERT(m_pChiMetadataUtil != nullptr, "m_pChiMetadataUtil is NULL !");
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
            // generate capture request for each camera/stream
            NT_ASSERT(GenerateMultiCameraCaptureRequest(m_cameraList, frameIndex) == CDKResultSuccess,
                    "Submit pipeline request failed for multi capture frame request %d", frameIndex);
        }

        //7. Wait for all results
        NT_ASSERT(WaitForResults() == CDKResultSuccess, "There are pending buffers not returned by driver after several retries");

        //8. Destroy all resources created
        DestroyResources(m_cameraList);
    }
} // namespace chitests
