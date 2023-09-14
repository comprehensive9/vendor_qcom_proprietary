////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  custom_node_test.cpp
/// @brief Declarations for the Custom node Tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "custom_node_test.h"

namespace chitests
{
    /***************************************************************************************************************************
    *   CustomNodeTest::TestCustomNode
    *
    *   @brief
    *       Test a custom node
    *   @param
    *       [in] TestType             testType       Test identifier
    *   @return
    *       None
    ***************************************************************************************************************************/
    void CustomNodeTest::TestCustomNode(TestId testType)
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

            //1. Create chistream objects from streamcreatedata
            NT_ASSERT(SetupStreams() == CDKResultSuccess, "Fatal failure at SetupStreams() !");

            ChiSensorModeInfo* testSensorMode = nullptr;
            testSensorMode = m_pChiModule->GetClosestSensorMode(m_currentTestCameraId, m_streamInfo.maxRes);

            NT_ASSERT(testSensorMode != nullptr, "Sensor mode is NULL for cameraId: %d", m_currentTestCameraId);

            NT_LOG_INFO( "Camera %d chosen sensor mode: %u, width: %u, height: %u", m_currentTestCameraId,
                testSensorMode->modeIndex, testSensorMode->frameDimension.width, testSensorMode->frameDimension.height);

            // TODO: Set number of batched frames from sensormode based on usecase
            testSensorMode->batchedFrames = 1;

            //2. Create pipelines
            NT_ASSERT(SetupPipelines(m_currentTestCameraId, testSensorMode) == CDKResultSuccess, "Pipelines Setup failed");

            //3. Create sessions
            NT_ASSERT(CreateSessions() == CDKResultSuccess, "Necessary sessions could not be created");

            //4. Initialize buffer manager and metabuffer pools
            NT_ASSERT(InitializeBufferManagers(m_currentTestCameraId) == CDKResultSuccess, "Buffer managers could not be initialized");

            NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(m_currentTestCameraId, MAX_REQUESTS) == CDKResultSuccess,
                "Input metabuffer pool for camera Id: %d could not be created!");
            NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
                "Output metabuffer pool could not be created!");

            //5. Create and submit capture requests
            uint64_t frameNumber = START_FRAME;
            for (UINT32 frameIndex = 1; frameIndex <= CmdLineParser::GetFrameCount(); frameIndex++, frameNumber++)
            {
                NT_ASSERT(GenerateRealtimeCaptureRequest(frameNumber) == CDKResultSuccess,
                    "Submit pipeline request failed");
            }

            //6. Wait for all results
            NT_ASSERT(WaitForResults() == CDKResultSuccess,
                "There are pending buffers not returned by driver after several retries");

            //7. Destroy all resources created
            DestroyResources();
        }
    }

    /***************************************************************************************************************************
    *   CustomNodeTest::SetupStreams()
    *
    *   @brief
    *       Overridden function implementation which defines the stream information based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if stream objects could be created or failure
    ***************************************************************************************************************************/
    CDKResult CustomNodeTest::SetupStreams()
    {
        CDKResult result = CDKResultSuccess;

        int streamIndex = 0;

        switch (m_testId)
        {
        case TestVendorTagWrite:
            m_numStreams = 1;

            //1. preview stream
            m_resolution[streamIndex] = FULLHD;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
            m_streamId[streamIndex]   = PREVIEW;
            m_isRealtime[streamIndex] = true;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            result = CDKResultENoSuch;
            break;
        }

        if (result == CDKResultSuccess)
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
    *   CustomNodeTest::SetupPipelines()
    *
    *   @brief
    *       Overridden function implementation which creates pipeline objects based on the test Id
    *   @param
    *       [in]  int                 cameraId    cameraId to be associated with the pipeline
    *       [in]  ChiSensorModeInfo*  sensorMode  sensor mode to be used for the pipeline
    *   @return
    *       CDKResult success if pipelines could be created or failure
    ***************************************************************************************************************************/
    CDKResult CustomNodeTest::SetupPipelines(int cameraId, ChiSensorModeInfo * sensorMode)
    {
        CDKResult result = CDKResultSuccess;
        switch (m_testId)
        {
        case TestId::TestVendorTagWrite:
            m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::VendorTagWrite);
            break;
        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            return CDKResultENoSuch;
        }

        result = (m_pChiPipeline[Realtime] != nullptr) ? m_pChiPipeline[Realtime]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        return result;
    }

    /***************************************************************************************************************************
    *   CustomNodeTest::CreateSessions()
    *
    *   @brief
    *       Overridden function implementation which creates required number of sessions based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if sessions could be created or failure
    ***************************************************************************************************************************/
    CDKResult CustomNodeTest::CreateSessions()
    {
        CDKResult result       = CDKResultSuccess;
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
            result = CDKResultEFailed;
        }
        else
        {
            result = m_pChiPipeline[Realtime]->ActivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
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
    *   CustomNodeTest::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources/objects
    *   @param
    *       None
    *   @return
    *       None
    ***************************************************************************************************************************/
    void CustomNodeTest::DestroyResources()
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
