////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  e2e_suite.h
/// @brief Definitions for the offline test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "e2e_suite.h"

namespace chitests
{

    /***************************************************************************************************************************
    *   E2ESuite::TestE2EPipeline
    *
    *   @brief
    *       Test end-to-end (mixed) pipelines
    *   @param
    *       [in] TestType   testType            Test identifier
    *   @return
    *       None
    ***************************************************************************************************************************/
    void E2ESuite::TestE2EPipeline(TestId testType)
    {
        m_testId = testType;
        Setup();

        // Get list of cameras to test
        std::vector <int> camList = m_pChiModule->GetCameraList();

        m_pipelineConfig = RealtimeConfig;

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            m_currentTestCameraId = camList[camCounter];

            // Query static capabilities
            if(CmdLineParser::isTestGenMode())
            {
                NT_ASSERT(CDKResultSuccess == QueryStaticCapabilities(MetadataUsecases::Testgen, m_currentTestCameraId),
                                                "Could not query testgen capabilities for cameraID %d", m_currentTestCameraId);
            }

            //1. Create chistream objects from streamcreatedata
            NT_ASSERT(SetupStreams() == CDKResultSuccess, "Fatal failure at SetupStreams() !");

            /*Set session level parameters
            if(CmdLineParser::isTestGenMode())
            {
                NT_ASSERT(CDKResultSuccess == SetSessionParameters(MetadataUsecases::Testgen, m_currentTestCameraId, m_streamInfo.maxRes),
                                                "Could not set session level parameters for cameraID %d", m_currentTestCameraId);
            }
            */

            m_testSensorMode = m_pChiModule->GetClosestSensorMode(m_currentTestCameraId, m_streamInfo.maxRes);

            NT_ASSERT(m_testSensorMode != nullptr,"Sensor mode is NULL for cameraId: %d", m_currentTestCameraId);

            NT_LOG_INFO( "Camera %d chosen sensor mode: %u, width: %u, height: %u", m_currentTestCameraId,
                m_testSensorMode->modeIndex, m_testSensorMode->frameDimension.width, m_testSensorMode->frameDimension.height);

            // TODO: Set number of batched frames based on usecase
            m_testSensorMode->batchedFrames = 1;

            // For RDI cases, we need to make sure stream resolution matches sensor mode
            for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
            {
                if ((IsRDIStream(m_pRequiredStreams[streamIndex].format)))
                {
                    if (!m_userRDISize.isInvalidSize()) {
                        NT_LOG_INFO("OVERRIDE: RDI resolution set by User width: %d, height: %d ",
                            m_userRDISize.width, m_userRDISize.height);
                        m_pRequiredStreams[streamIndex].width  = m_userRDISize.width;
                        m_pRequiredStreams[streamIndex].height = m_userRDISize.height;
                    }
                    else if (!m_isCamIFTestGen)
                    {
                        NT_LOG_INFO("Selected RDI resolution based on Sensor mode width: %d, height: %d ",
                            m_testSensorMode->frameDimension.width, m_testSensorMode->frameDimension.height);
                        m_pRequiredStreams[streamIndex].width  = m_testSensorMode->frameDimension.width;
                        m_pRequiredStreams[streamIndex].height = m_testSensorMode->frameDimension.height;
                    }
                }
            }

            //2. Create pipelines
            NT_ASSERT(SetupPipelines(m_currentTestCameraId, m_testSensorMode) == CDKResultSuccess,"Pipelines Setup failed");

            //3. Create sessions
            NT_ASSERT(CreateSessions() == CDKResultSuccess, "Necessary sessions could not be created");

            //4. Initialize buffer manager and metabuffer pools
            NT_ASSERT(InitializeBufferManagers(m_currentTestCameraId) == CDKResultSuccess, "Buffer managers could not be initialized");
            NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(m_currentTestCameraId, MAX_REQUESTS) == CDKResultSuccess,
                "Input metabuffer pool for camera Id: %d could not be created!", m_currentTestCameraId);
            NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
                "Output metabuffer pool could not be created!");

            //5. Create and submit capture requests
            std::set<int> snap_list = CmdLineParser::GetSnapFrameNumbers();
            bool bSnapshotRequest = false;
            NT_LOG_INFO("Is ZSL Enabled?: %d", CmdLineParser::IsZSLEnabled());
            for (UINT32 frameNumber = START_FRAME; frameNumber <= m_captureCount; frameNumber++)
            {
                // skip snapshot for certain frames based on user input
                if (snap_list.find(frameNumber) ==  snap_list.end())
                {
                    bSnapshotRequest = false;
                }
                else
                {
                    bSnapshotRequest = true;
                }

                // ZSL = you get what you see, preview and snapshot frame number should be same
                if (CmdLineParser::IsZSLEnabled())
                {
                    NT_ASSERT(GenerateRealtimeCaptureRequest(frameNumber, bSnapshotRequest) == CDKResultSuccess,
                        "Submit E2E realtime pipeline request failed for frameNumber: %d", frameNumber);
                }
                // NonZSL = you get delayed snapshot frame after preview
                else
                {
                    if (bSnapshotRequest)
                    {
                        // Send only snapshot request
                        NT_ASSERT(GenerateRealtimeCaptureRequest(frameNumber, bSnapshotRequest, true) == CDKResultSuccess,
                            "Submit E2E offline pipeline request failed for frameNumber: %d", frameNumber);
                    }
                    else
                    {
                        // submit preview request
                        NT_ASSERT(GenerateRealtimeCaptureRequest(frameNumber) == CDKResultSuccess,
                            "Submit E2E realtime pipeline request failed for frameNumber: %d", frameNumber);

                    }
                }

            }

            //6. Wait for all results
            NT_ASSERT(WaitForResults() == CDKResultSuccess,
                "There are pending buffers not returned by driver after several retries");

            //7. Destroy all resources created
            DestroyResources();
        }
    }

    /***************************************************************************************************************************
    *   E2ESuite::SetupStreams()
    *
    *   @brief
    *       Overridden function implementation which defines the stream information based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if stream objects could be created or failure
    ***************************************************************************************************************************/
    CDKResult E2ESuite::SetupStreams()
    {

        CDKResult result = CDKResultSuccess;
        CHISTREAMFORMAT noFormatOverride = static_cast<CHISTREAMFORMAT>(-1);
        int streamIndex = 0;

        switch (m_testId)
        {
        case TestPreviewJpegSnapshot:
        case TestPreviewJpegSnapshot3AStats:
            m_numStreams = 2;

            //1. Preview stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI format / size not user configurable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD_1440 : m_userPrvSize;
            m_streamId[streamIndex]   = PREVIEW;
            m_isRealtime[streamIndex] = true;
            streamIndex++;

            //2. Snapshot stream (JPEG)
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_resolution[streamIndex] = (m_userSnapSize.isInvalidSize()) ? FULLHD_1440 : m_userSnapSize;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.num_streams = m_numStreams;
            break;
        case TestPreviewYuvSnapshot:
        case TestPreviewYuvSnapshot3AStats:
            m_numStreams = 2;

            //1. Preview stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI format / size not user configurable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD_1440 : m_userPrvSize;
            m_streamId[streamIndex]   = PREVIEW;
            m_isRealtime[streamIndex] = true;
            streamIndex++;

            //2. Snapshot stream (YUV)
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userSnapFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userSnapFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI format / size not user configurable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userSnapSize.isInvalidSize()) ? FULLHD_1440 : m_userSnapSize;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.num_streams = m_numStreams;
            break;
        case TestPreviewCallbackSnapshotThumbnail:
            m_numStreams = 4;

            //1. Preview stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI format / size not user configurable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD_1440 : m_userPrvSize;
            m_streamId[streamIndex]   = PREVIEW;
            m_isRealtime[streamIndex] = true;
            streamIndex++;

            //2. Preview callback stream stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI format / size not user configurable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userDispSize.isInvalidSize()) ? FULLHD_1440 : m_userDispSize;
            m_streamId[streamIndex]   = PREVIEWCALLBACK;
            m_isRealtime[streamIndex] = false;
            streamIndex++;

            //3. Snapshot stream (JPEG)
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_resolution[streamIndex] = (m_userSnapSize.isInvalidSize()) ? FULLHD_1440 : m_userSnapSize;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_isRealtime[streamIndex] = false;
            streamIndex++;

            //4. Thumbnail stream
            // Fixed Stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
            m_resolution[streamIndex] = VGA;
            m_streamId[streamIndex]   = THUMBNAIL;
            m_isRealtime[streamIndex] = false;
            m_streamInfo.num_streams = m_numStreams;
            break;
        case TestLiveVideoSnapshot:
            m_numStreams = 3;

            //1. Preview stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI format / size not user configurable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD_1440 : m_userPrvSize;
            m_streamId[streamIndex]   = PREVIEW;
            m_isRealtime[streamIndex] = true;
            streamIndex++;

            //2. Video Stream
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            if (IsRDIStream(m_format[streamIndex]))
            {
                NT_LOG_UNSUPP("RDI size not applicable");
                return CDKResultEUnsupported;
            }
            m_resolution[streamIndex] = (m_userVidSize.isInvalidSize()) ? FULLHD_1440 : m_userVidSize;
            m_streamId[streamIndex]   = VIDEO;
            m_isRealtime[streamIndex] = false;
            streamIndex++;

            //3. Snapshot stream (JPEG)
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_resolution[streamIndex] = (m_userSnapSize.isInvalidSize()) ? FULLHD_1440 : m_userSnapSize;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_isRealtime[streamIndex] = false;
            m_streamInfo.num_streams = m_numStreams;
            break;

        case TestHDRPreviewVideo:
        {
            m_numStreams = 3;
            UINT32 nExposures = CmdLineParser::GetNumExposures();

            //1. IFE RAW0 out stream, 1-exposure (configurable size)
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_isRealtime[streamIndex] = true;
            m_format[streamIndex]     = ChiStreamFormatRaw10;
            m_streamId[streamIndex]   = IFEOutputPortRAW0;

            if(nExposures >= 2) // 2-exposure and 3-exposure
            {
                //2. IFE RAW1 out stream, 2-exposure (configurable size)
                streamIndex++;
                m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_isRealtime[streamIndex] = true;
                m_format[streamIndex]     = ChiStreamFormatRaw10;
                m_streamId[streamIndex]   = IFEOutputPortRAW1;
                m_numStreams++;

                if(nExposures == 3) // 3-exposure
                {
                    //3. IFE RAW2 out stream (configurable size)
                    streamIndex++;
                    m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_isRealtime[streamIndex] = true;
                    m_format[streamIndex]     = ChiStreamFormatRaw10;
                    m_streamId[streamIndex]   = IFEOutputPortRAW2;
                    m_numStreams++;
                }
            }

            //4. Preview stream (configurable)
            streamIndex++;
            m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD: m_userPrvSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]   = PREVIEW;
            m_isRealtime[streamIndex] = true;

            //5. Video Stream (configurable)
            streamIndex++;
            m_resolution[streamIndex] = m_userVidSize.isInvalidSize() ? FULLHD : m_userVidSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]   = VIDEO;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.num_streams  = m_numStreams;
            break;
        }
        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            result = CDKResultENoSuch;
            break;
        }

        if(result == CDKResultSuccess)
        {
            if (m_streamInfo.num_streams <= 0)
            {
                NT_LOG_ERROR("Number of streams should be greater than 0, given: %d", m_streamInfo.num_streams);
                result = CDKResultEFailed;
            }
            else
            {
                m_pRequiredStreams = SAFE_NEW() CHISTREAM[m_streamInfo.num_streams];
                if (NULL == m_pRequiredStreams)
                {
                    NT_LOG_ERROR("Failed to initialize CHI streams");
                    result = CDKResultEFailed;
                }
                else
                {
                    camera3_stream_t* pCamStreams = CreateStreams(m_streamInfo);

                    if (nullptr == pCamStreams)
                    {
                        NT_LOG_ERROR("Unable to create streams");
                        result = CDKResultEFailed;
                    }
                    else
                    {
                        ConvertCamera3StreamToChiStream(pCamStreams, m_pRequiredStreams, m_streamInfo.num_streams);
                        delete[] pCamStreams;   // No longer needed, converted to m_pRequiredStreams

                        if (m_pRequiredStreams == nullptr)
                        {
                            NT_LOG_ERROR("Provided streams could not be configured for testcase Id: [%d]", m_testId);
                            result = CDKResultEFailed;
                        }
                        else
                        {
                            for (int index = 0; index < m_numStreams; index++)
                            {
                                m_streamIdMap[m_streamInfo.streamIds[index]] = &m_pRequiredStreams[index];
                            }

                            if (m_streamIdMap.size() != static_cast<size_t>(m_numStreams))
                            {
                                result = CDKResultEFailed;
                            }
                        }
                    }
                }
            }
        }

        return result;
    }

    /***************************************************************************************************************************
    *   E2ESuite::SetupPipelines()
    *
    *   @brief
    *       Overridden function implementation which creates pipeline objects based on the test Id
    *   @param
    *       [in]  int                 cameraId    cameraId to be associated with the pipeline
    *       [in]  ChiSensorModeInfo*  sensorMode  sensor mode to be used for the pipeline
    *   @return
    *       CDKResult success if pipelines could be created or failure
    ***************************************************************************************************************************/
    CDKResult E2ESuite::SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode)
    {
        CDKResult result = CDKResultSuccess;

        switch(m_testId)
        {
        case TestPreviewJpegSnapshot:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::PreviewJpegSnapshot);
            break;
        case TestPreviewYuvSnapshot:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::PreviewYuvSnapshot);
            break;
        case TestPreviewJpegSnapshot3AStats:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::PreviewJpegSnapshot3AStats);
            break;
        case TestPreviewYuvSnapshot3AStats:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::PreviewYuvSnapshot3AStats);
            break;
        case TestLiveVideoSnapshot:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::LiveVideoSnapshot);
            break;
        case TestPreviewCallbackSnapshotThumbnail:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::PreviewCallbackSnapshotThumbnail);
            break;
        case TestHDRPreviewVideo:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId,
                                                sensorMode,
                                                m_streamIdMap,
                                                PipelineType::HDRPreviewVideo);
            break;
        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            return CDKResultENoSuch;
        }

        result = (m_pChiPipeline[Realtime] != nullptr) ? m_pChiPipeline[Realtime]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        return result;
    }

    /***************************************************************************************************************************
    *   E2ESuite::CreateSessions()
    *
    *   @brief
    *       Overridden function implementation which creates required number of sessions based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if sessions could be created or failure
    ***************************************************************************************************************************/
    CDKResult E2ESuite::CreateSessions()
    {
        CDKResult result = CDKResultSuccess;
        ChiCallBacks callbacks = { 0 };

        callbacks.ChiNotify                      = ChiTestCase::ProcessMessage;
        callbacks.ChiProcessCaptureResult        = ChiTestCase::QueueCaptureResult;
        callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

        mPerSessionPvtData[RealtimeSession].pTestInstance = this;
        mPerSessionPvtData[RealtimeSession].sessionId     = RealtimeSession;
        mPerSessionPvtData[RealtimeSession].cameraId      = m_currentTestCameraId;

        mPerSessionPvtData[RealtimeSession].pChiSession   = ChiSession::Create(&m_pChiPipeline[Realtime], 1, &callbacks,
                                                            &mPerSessionPvtData[RealtimeSession]);
        mPerSessionPvtData[RealtimeSession].sessionHandle = mPerSessionPvtData[RealtimeSession].pChiSession->GetSessionHandle();

        if (mPerSessionPvtData[RealtimeSession].pChiSession == nullptr)
        {
            NT_LOG_ERROR("End-to-end session could not be created");
            result = CDKResultEFailed;
        }
        else
        {
            if(!m_nEarlyPCR) // if early PCR not enabled, activate pipeline here
            {
                result = m_pChiPipeline[Realtime]->ActivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
            }

            if (result == CDKResultSuccess)
            {
                result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                    mPerSessionPvtData[RealtimeSession].sessionHandle,
                    m_pChiPipeline[Realtime]->GetPipelineHandle(), &m_pPipelineMetadataInfo[Realtime]);
            }
        }
        return result;
    }

    /***************************************************************************************************************************
    *   E2ESuite::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources/objects
    *   @param
    *       None
    *   @return
    *       None
    ***************************************************************************************************************************/
    void E2ESuite::DestroyResources()
    {
        if ((nullptr != mPerSessionPvtData[RealtimeSession].pChiSession) && (nullptr != m_pChiPipeline[Realtime]))
        {
            m_pChiPipeline[Realtime]->DeactivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
        }

        if (mPerSessionPvtData[RealtimeSession].pChiSession != nullptr)
        {
            mPerSessionPvtData[RealtimeSession].pChiSession->DestroySession();
            mPerSessionPvtData[RealtimeSession].pChiSession = nullptr;
        }

        if (m_pChiPipeline[Realtime] != nullptr)
        {
            m_pChiPipeline[Realtime]->DestroyPipeline();
            m_pChiPipeline[Realtime] = nullptr;
        }

        DestroyCommonResources();
    }
}
