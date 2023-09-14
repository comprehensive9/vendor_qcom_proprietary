////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  mixed_pipeline_test.h
/// @brief Definitions for the offline test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mixed_pipeline_test.h"
#include "hardware/camera3.h"

namespace chitests
{

    /***************************************************************************************************************************
    *   MixedPipelineTest::TestMixedPipelineTest
    *
    *   @brief
    *       Test a Mixed pipeline involving real time session and offline session simultaneously
    *   @param
    *       [in] TestType   testType            Test identifier
    *       [in] bool       bUseExternalSensor  Set if test uses external sensor (defaults to false)
    *   @return
    *       None
    ***************************************************************************************************************************/
    void MixedPipelineTest::TestMixedPipelineTest(TestId testType, bool bUseExternalSensor)
    {
        m_selectedRdiOutputSize = Size(CmdLineParser::GetRdiOutputWidth(), CmdLineParser::GetRdiOutputHeight());

        // Get list of cameras to test
        std::vector <int> camList = m_pChiModule->GetCameraList();

        // Only test camera 0 if using external sensor (camera 1 and 2 not supported with external sensor)
        if (bUseExternalSensor)
        {
            camList.clear();
            camList.push_back(0);
        };

        m_testId = testType;

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            m_lastRealtimeFrameReceived = 0;

            //1. Create chistream objects from streamcreatedata
            NT_ASSERT(SetupStreams() == CDKResultSuccess, "Fatal failure at SetupStreams() !");

            ChiSensorModeInfo* testSensorMode = nullptr;
            testSensorMode = m_pChiModule->GetClosestSensorMode(cameraId, m_streamInfo.maxRes);

            NT_ASSERT(testSensorMode != nullptr,"Sensor mode is NULL for cameraId: %d", cameraId);

            NT_LOG_INFO( "Camera %d chosen sensor mode: %u, width: %u, height: %u", cameraId,
                testSensorMode->modeIndex, testSensorMode->frameDimension.width, testSensorMode->frameDimension.height);

            // TODO: Set number of batched frames based on usecase
            testSensorMode->batchedFrames = 1;

            for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
            {
                if (IsRDIStream(m_pRequiredStreams[streamIndex].format))
                {
                    if (!m_selectedRdiOutputSize.isInvalidSize()) {
                        NT_LOG_INFO("OVERRIDE: RDI resolution set by User width: %d, height: %d ",
                            m_selectedRdiOutputSize.width, m_selectedRdiOutputSize.height);
                        m_pRequiredStreams[streamIndex].width = m_selectedRdiOutputSize.width;
                        m_pRequiredStreams[streamIndex].height = m_selectedRdiOutputSize.height;
                    }
                    else if (!m_isCamIFTestGen)
                    {
                        NT_LOG_INFO("Selected RDI resolution based on Sensor mode width: %d, height: %d ",
                            testSensorMode->frameDimension.width, testSensorMode->frameDimension.height);
                        m_pRequiredStreams[streamIndex].width = testSensorMode->frameDimension.width;
                        m_pRequiredStreams[streamIndex].height = testSensorMode->frameDimension.height;
                    }
                }
            }

            //2. Create pipelines
            NT_ASSERT(SetupPipelines(cameraId, testSensorMode) == CDKResultSuccess,"Pipelines Setup failed");

            //3. Create sessions
            NT_ASSERT(CreateSessions() == CDKResultSuccess, "Necessary sessions could not be created");

            //4. Initialize buffer manager and metabuffer pools
            NT_ASSERT(InitializeBufferManagers(cameraId) == CDKResultSuccess, "Buffer managers could not be initialized");
#ifndef OLD_METADATA
            NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(cameraId, MAX_REQUESTS) == CDKResultSuccess,
                "Input metabuffer pool for camera Id: %d could not be created!", cameraId);
            NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
                "Output metabuffer pool could not be created!");
#endif // OLD_METADATA

            //5. Create and submit capture requests
            uint64_t frameNumber = START_FRAME;
            for (int frameIndex = 1; frameIndex <= CmdLineParser::GetFrameCount(); frameIndex++, frameNumber++)
            {
                bool SnapshotRequest = frameIndex % 5 == 0;
                /*
                 * For live video snapshot, RDI buffer should not be sent during recording
                 * RDI buffer should be included in request just before taking snapshot
                 */
                if(m_testId == LiveVideoSnapShot && !SnapshotRequest)
                {
                    NT_ASSERT(GenerateRealtimeRequest(frameNumber, false) == CDKResultSuccess,
                        "Submit realtime pipeline request failed for frameNumber: %d", frameNumber);
                }
                else
                {
                    NT_ASSERT(GenerateRealtimeRequest(frameNumber, true) == CDKResultSuccess,
                        "Submit realtime pipeline request failed for frameNumber: %d", frameNumber);
                }

                if (SnapshotRequest)
                {
                    switch (m_testId) {
                        // For ZSL cases, frame number must be along with preview
                        // ZSL = what you see (preview) is what you get (snapshot)
                        case PreviewZSLYuvSnapshot:
                        case PreviewZSLYuvSnapshotWithStats:
                        case PreviewZSLJpegSnapshot:
                        case PreviewZSLJpegSnapshotWithStats:
                            NT_ASSERT(GenerateOfflineRequest(frameNumber) == CDKResultSuccess,
                                "Submit offline pipeline request failed for frameNumber: %d", frameNumber);
                            break;
                        default:
                            frameNumber += 1;
                            NT_ASSERT(GenerateOfflineRequest(frameNumber) == CDKResultSuccess,
                                "Submit offline pipeline request failed for frameNumber: %d", frameNumber);
                            break;
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
    *   MixedPipelineTest::SetupStreams()
    *
    *   @brief
    *       Overridden function implementation which defines the stream information based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if stream objects could be created or failure
    ***************************************************************************************************************************/
    CDKResult MixedPipelineTest::SetupStreams()
    {

        CDKResult result = CDKResultSuccess;

        int streamIndex = 0;

        switch (m_testId)
        {
            case PreviewZSLYuvSnapshot:
            case PreviewZSLYuvSnapshotWithStats:
            case PreviewNZSLYuvSnapshot:
            case PreviewNZSLYuvSnapshotWithStats:
                m_numStreams = 3;

                //1. preview stream
                m_resolution[streamIndex] = VGA;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCNV12;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //2. RDI stream
                m_resolution[streamIndex] = UHD;
                m_direction[streamIndex]  = ChiStreamTypeBidirectional;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_isRealtime[streamIndex] = true;
                if((m_testId == PreviewZSLYuvSnapshotWithStats) || (m_testId == PreviewNZSLYuvSnapshotWithStats))
                {
                    m_streamId[streamIndex] = IFEOutputPortRDI1;
                }
                else
                {
                    m_streamId[streamIndex] = IFEOutputPortRDI0;
                }
                streamIndex++;
                //3. Snapshot stream (YUV)
                m_resolution[streamIndex]  = UHD;
                m_direction[streamIndex]   = ChiStreamTypeOutput;
                m_format[streamIndex]      = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]    = SNAPSHOT;
                m_isRealtime[streamIndex]  = false;

                m_streamInfo.num_streams = m_numStreams;
                break;
            case PreviewZSLJpegSnapshot:
            case PreviewZSLJpegSnapshotWithStats:
            case PreviewNZSLJpegSnapshot:
            case PreviewNZSLJpegSnapshotWithStats:
                m_numStreams = 3;

                //1. preview stream
                m_resolution[streamIndex] = VGA;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCNV12;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                m_isRealtime[streamIndex] = true;
                streamIndex++;
                //2. RDI stream
                m_resolution[streamIndex]  = UHD;
                m_direction[streamIndex]   = ChiStreamTypeBidirectional;
                m_format[streamIndex]      = ChiStreamFormatRaw16;
                m_isRealtime[streamIndex]  = true;
                if ((m_testId == PreviewZSLJpegSnapshotWithStats) || (m_testId == PreviewNZSLJpegSnapshotWithStats))
                {
                    m_streamId[streamIndex] = IFEOutputPortRDI1;
                }
                else
                {
                    m_streamId[streamIndex] = IFEOutputPortRDI0;
                }
                streamIndex++;
                //3. Snapshot stream (JPEG)
                m_resolution[streamIndex]  = UHD;
                m_direction[streamIndex]   = ChiStreamTypeOutput;
                m_format[streamIndex]      = ChiStreamFormatBlob;
                m_streamId[streamIndex]    = SNAPSHOT;
                m_isRealtime[streamIndex]  = false;

                m_streamInfo.num_streams = m_numStreams;
                break;
            case TestPreviewCallbackSnapshotWithThumbnail:
                m_numStreams = 5;

                //1. preview stream
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCNV12;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //2. Preview callback stream
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = PREVIEWCALLBACK;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //3. RDI stream
                m_resolution[streamIndex] = UHD;
                m_direction[streamIndex]  = ChiStreamTypeBidirectional;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_streamId[streamIndex]   = IFEOutputPortRDI1;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //4. Snapshot stream (JPEG)
                m_resolution[streamIndex] = UHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = SNAPSHOT;
                m_isRealtime[streamIndex] = false;
                streamIndex++;

                //5. YUV thumbnail
                m_resolution[streamIndex] = VGA;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = THUMBNAIL;
                m_isRealtime[streamIndex] = false;

                m_streamInfo.num_streams = m_numStreams;
                break;
            case LiveVideoSnapShot:
                m_numStreams = 4;

                //1. preview stream
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCNV12;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //2. Video stream
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IPEOutputPortVideo;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //3. RDI stream
                m_resolution[streamIndex] = UHD;
                m_direction[streamIndex]  = ChiStreamTypeBidirectional;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_streamId[streamIndex]   = IFEOutputPortRDI1;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //4. Snapshot stream (JPEG)
                m_resolution[streamIndex] = UHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = SNAPSHOT;
                m_isRealtime[streamIndex] = false;

                m_streamInfo.num_streams = m_numStreams;
                break;
            case TestIPEABCStatsTestgenOfflineJPEG:
            case TestIPEABCStatsTestgenOfflineYUV:
            case TestIPEABCStatsTestgenOfflineRaw:
            case TestIPEABCStatsOfflineJPEG:
            case TestIPEABCStatsOfflineYUV:
            case TestIPEABCStatsOfflineRaw:
                m_numStreams = 10;

                //1. IPE Display (A)
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //2. IPE Video (B)
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IPEOutputPortVideo;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //3. IFE FD (C)
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IFEOutputPortFD;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //4. IFE Stats BF
                m_resolution[streamIndex] = Size(5768, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsBF;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //5. IFE Stats AWBBG
                m_resolution[streamIndex] = Size(691200, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsAWBBG;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //6. IFE Stats TLBG
                m_resolution[streamIndex] = Size(147456, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsTLBG;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //7. IFE Stats IHIST
                m_resolution[streamIndex] = Size(2048, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsIHIST;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                //8. IFE Stats RS
                m_resolution[streamIndex] = Size(32768, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsRS;
                m_isRealtime[streamIndex] = true;
                streamIndex++;

                if (m_testId == TestIPEABCStatsTestgenOfflineJPEG ||
                    m_testId == TestIPEABCStatsOfflineJPEG)
                {
                    //9. Offline Input stream
                    m_resolution[streamIndex] = HD4K;
                    m_direction[streamIndex]  = ChiStreamTypeInput;
                    m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                    m_streamId[streamIndex]   = JPEGInputPort;
                    m_streamInfo.inputImages[streamIndex]     = IPE_EMULATEDSENSOR_4K;
                    m_isRealtime[streamIndex] = false;
                    streamIndex++;

                    //10. Offline Snapshot stream
                    m_resolution[streamIndex] = HD4K;
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = SNAPSHOT;
                    m_isRealtime[streamIndex] = false;
                }
                else if (m_testId == TestIPEABCStatsTestgenOfflineYUV ||
                         m_testId == TestIPEABCStatsOfflineYUV)
                {
                    //9. Offline Input stream
                    m_resolution[streamIndex] = HD4K;
                    m_direction[streamIndex]  = ChiStreamTypeInput;
                    m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                    m_streamId[streamIndex]   = IPEInputPortFull;
                    m_streamInfo.inputImages[streamIndex]     = IPE_EMULATEDSENSOR_4K;
                    m_isRealtime[streamIndex] = false;
                    streamIndex++;

                    //10. Offline IPE Display
                    m_resolution[streamIndex] = HD4K;
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                    m_streamId[streamIndex]   = PREVIEW;
                    m_isRealtime[streamIndex] = false;
                }
                else // m_testId == TestIPEABCStatsTestgenOfflineRaw || m_testId == TestIPEABCStatsOfflineRaw
                {
                    //9. Offline Input stream
                    m_resolution[streamIndex] = UHD;
                    m_direction[streamIndex]  = ChiStreamTypeInput;
                    m_format[streamIndex]     = ChiStreamFormatRaw16;
                    m_streamId[streamIndex]   = BPSInputPort;
                    m_streamInfo.inputImages[streamIndex]     = BPS_IDEALRAW_UHD;
                    m_isRealtime[streamIndex] = false;
                    streamIndex++;

                    //10. Offline IPE Display
                    m_resolution[streamIndex] = UHD;
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                    m_streamId[streamIndex]   = PREVIEW;
                    m_isRealtime[streamIndex] = false;
                }

                // Some of these tests have both jpeg and stat dumps of blob format. Let camera3stream determine if each stream is jpeg.
                m_streamInfo.num_streams = m_numStreams;
                break;
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
    *   MixedPipelineTest::SetupPipelines()
    *
    *   @brief
    *       Overridden function implementation which creates pipeline objects based on the test Id
    *   @param
    *       [in]  int                 cameraId    cameraId to be associated with the pipeline
    *       [in]  ChiSensorModeInfo*  sensorMode  sensor mode to be used for the pipeline
    *   @return
    *       CDKResult success if pipelines could be created or failure
    ***************************************************************************************************************************/
    CDKResult MixedPipelineTest::SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode)
    {
        CDKResult result = CDKResultSuccess;
        switch(m_testId)
        {
        case PreviewZSLYuvSnapshot:
        case PreviewZSLJpegSnapshot:
        case PreviewNZSLYuvSnapshot:
        case PreviewNZSLJpegSnapshot:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimePrevRDI);
            m_Pipelines[Offline]  = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::OfflineZSlSnap);
            break;
        case PreviewZSLYuvSnapshotWithStats:
        case PreviewZSLJpegSnapshotWithStats:
        case PreviewNZSLYuvSnapshotWithStats:
        case PreviewNZSLJpegSnapshotWithStats:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimePrevRDIWithStats);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineZSlSnapWithDSports);
            break;
        case TestPreviewCallbackSnapshotWithThumbnail:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimePrevWithCallbackAndRDIWithStats);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineZSlSnapAndThumbnailWithDSports);
            break;
        case LiveVideoSnapShot:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimePrevAndVideoWithStats);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineZSlSnapWithDSports);
            break;
        case TestIPEABCStatsTestgenOfflineJPEG:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimeIPEABCStatsTestgen);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineJpegSnapshot);
            break;
        case TestIPEABCStatsTestgenOfflineYUV:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimeIPEABCStatsTestgen);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineIPEDisp);
            break;
        case TestIPEABCStatsTestgenOfflineRaw:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimeIPEABCStatsTestgen);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineBPSWithIPEAndDS);
            break;
        case TestIPEABCStatsOfflineJPEG:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimeIPEABCStats);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineJpegSnapshot);
            break;
        case TestIPEABCStatsOfflineYUV:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimeIPEABCStats);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineIPEDisp);
            break;
        case TestIPEABCStatsOfflineRaw:
            m_Pipelines[Realtime] = ChiPipeline::Create(cameraId,
                                                        sensorMode,
                                                        m_streamIdMap,
                                                        PipelineType::RealtimeIPEABCStats);
            m_Pipelines[Offline] = ChiPipeline::Create(cameraId,
                                                       sensorMode,
                                                       m_streamIdMap,
                                                       PipelineType::OfflineBPSWithIPEAndDS);
            break;
        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            return CDKResultENoSuch;
        }

        result |= (m_Pipelines[Realtime] != nullptr) ? m_Pipelines[Realtime]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        result |= (m_Pipelines[Offline] != nullptr) ? m_Pipelines[Offline]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        return result;
    }

    /***************************************************************************************************************************
    *   MixedPipelineTest::CreateSessions()
    *
    *   @brief
    *       Overridden function implementation which creates required number of sessions based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if sessions could be created or failure
    ***************************************************************************************************************************/
    CDKResult MixedPipelineTest::CreateSessions()
    {
        CDKResult result = CDKResultSuccess;
        ChiCallBacks callbacks = { 0 };

        callbacks.ChiNotify                      = ChiTestCase::ProcessMessage;
        callbacks.ChiProcessCaptureResult        = ChiTestCase::QueueCaptureResult;
        callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

        switch (m_testId)
        {
        case PreviewZSLYuvSnapshot:
        case PreviewZSLYuvSnapshotWithStats:
        case PreviewZSLJpegSnapshot:
        case PreviewZSLJpegSnapshotWithStats:
        case PreviewNZSLYuvSnapshot:
        case PreviewNZSLYuvSnapshotWithStats:
        case PreviewNZSLJpegSnapshot:
        case PreviewNZSLJpegSnapshotWithStats:
        case TestPreviewCallbackSnapshotWithThumbnail:
        case LiveVideoSnapShot:
        case TestIPEABCStatsTestgenOfflineJPEG:
        case TestIPEABCStatsTestgenOfflineYUV:
        case TestIPEABCStatsTestgenOfflineRaw:
        case TestIPEABCStatsOfflineJPEG:
        case TestIPEABCStatsOfflineYUV:
        case TestIPEABCStatsOfflineRaw:
            m_perSessionPvtData[PreviewRdi].pTestInstance = this;
            m_perSessionPvtData[PreviewRdi].sessionId     = PreviewRdi;

            m_perSessionPvtData[Snapshot].pTestInstance = this;
            m_perSessionPvtData[Snapshot].sessionId     = Snapshot;

            m_pSession[PreviewRdi] = ChiSession::Create(&m_Pipelines[Realtime],
                                                        1,
                                                        &callbacks,
                                                        &m_perSessionPvtData[PreviewRdi]);
            if(m_pSession[PreviewRdi] == nullptr)
            {
                NT_LOG_ERROR( "PreviewRdi session could not be created");
                result = CDKResultEFailed;
                break;
            }

            m_pSession[Snapshot]   = ChiSession::Create(&m_Pipelines[Offline],
                                                        1,
                                                        &callbacks,
                                                        &m_perSessionPvtData[Snapshot]);
            if (m_pSession[Snapshot] == nullptr)
            {
                NT_LOG_ERROR( "Snapshot session could not be created");
                result = CDKResultEFailed;
                break;
            }

            result = m_Pipelines[Realtime]->ActivatePipeline(m_pSession[PreviewRdi]->GetSessionHandle());
#ifndef OLD_METADATA
            if (result == CDKResultSuccess)
            {
                result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                    m_pSession[PreviewRdi]->GetSessionHandle(), m_Pipelines[Realtime]->GetPipelineHandle(),
                    &m_pPipelineMetadataInfo[Realtime]);
            }
#endif // OLD_METADATA

            result |= m_Pipelines[Offline]->ActivatePipeline(m_pSession[Snapshot]->GetSessionHandle());
#ifndef OLD_METADATA
            if (result == CDKResultSuccess)
            {
                result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                    m_pSession[Snapshot]->GetSessionHandle(), m_Pipelines[Offline]->GetPipelineHandle(),
                    &m_pPipelineMetadataInfo[Offline]);
            }
#endif // OLD_METADATA
            break;
        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            result = CDKResultENoSuch;
            break;
        }

        return result;
    }

    /***************************************************************************************************************************
    *   MixedPipelineTest::GenerateRealtimeRequest()
    *
    *   @brief
    *       Generate a realtime pipeline request
    *   @param
    *       [in] uint64_t    frameNumber    framenumber associated with the request
    *       [in] bool        includeRaw     bool to determine if we want RDI buffer in request
    *   @return
    *       CDKResult success if request could be submitted or failure
    ***************************************************************************************************************************/
    CDKResult MixedPipelineTest::GenerateRealtimeRequest(uint64_t frameNumber, bool includeRaw)
    {
        CDKResult result = CDKResultSuccess;
        uint32_t outputIndex = 0;

        for(int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
        {
            if(!m_isRealtime[streamIndex])
            {
                continue;
            }

            CHISTREAM* currentStream = m_streamIdMap[m_streamId[streamIndex]];

            if (IsRDIStream(currentStream->format) && !includeRaw)
            {
                continue;
            }
            void* buffer = m_streamBufferMap[currentStream]->GetOutputBufferForRequest();
            if (buffer == nullptr)
            {
                return CDKResultENoMore;
            }
            m_realtimeOutputBuffers[outputIndex] = *static_cast<BufferManager::NativeChiBuffer*>(buffer);
            outputIndex++;
        }

        uint32_t requestId = FRAME_REQUEST_MOD(frameNumber);
        memset(&m_realtimeRequest[requestId], 0, sizeof(CHICAPTUREREQUEST));

        m_realtimeRequest[requestId].frameNumber       = frameNumber;
        m_realtimeRequest[requestId].hPipelineHandle   = m_Pipelines[Realtime]->GetPipelineHandle();
        m_realtimeRequest[requestId].numOutputs        = outputIndex;
        m_realtimeRequest[requestId].pOutputBuffers    = m_realtimeOutputBuffers;
        m_realtimeRequest[requestId].pPrivData         = &m_requestPvtData[PreviewRdi];

#ifdef OLD_METADATA
        m_realtimeRequest[requestId].pMetadata = allocate_camera_metadata(METADATA_ENTRIES, 100 * 1024);
        NATIVETEST_UNUSED_PARAM(m_pPipelineMetadataInfo);
#else
        // Get the input metabuffer from the pool
        m_realtimeRequest[requestId].pInputMetadata = m_pChiMetadataUtil->GetInputMetabufferFromPool(requestId);
        NT_EXPECT(m_realtimeRequest[requestId].pInputMetadata != nullptr, "Failed to create Input Metabuffer before sending request");

        // Get the output metabuffer from the pool
        m_realtimeRequest[requestId].pOutputMetadata = m_pChiMetadataUtil->GetOutputMetabufferFromPool(requestId,
            m_pPipelineMetadataInfo[Realtime].publishTagArray, m_pPipelineMetadataInfo[Realtime].publishTagCount);
        NT_EXPECT(m_realtimeRequest[requestId].pOutputMetadata != nullptr, "Failed to create Output Metabuffer before sending request");
#endif // OLD_METADATA

        memset(&m_submitRequest, 0, sizeof(CHIPIPELINEREQUEST));
        m_submitRequest.pSessionHandle   = m_pSession[PreviewRdi]->GetSessionHandle();
        m_submitRequest.numRequests      = 1;
        m_submitRequest.pCaptureRequests = &m_realtimeRequest[requestId];

        AtomicIncrement(outputIndex);

        NT_LOG_DEBUG( "Number of pending buffers remaining: %d after sending request: %" PRIu64,
            GetPendingBufferCount(), frameNumber);

        result = m_pChiModule->SubmitPipelineRequest(&m_submitRequest);

        return result;
    }

    /***************************************************************************************************************************
    *   MixedPipelineTest::GenerateOfflineRequest()
    *
    *   @brief
    *       Generate an offline snapshot pipeline request
    *   @param
    *       [in] uint64_t    frameNumber    framenumber associated with the request
    *   @return
    *       CDKResult success if request could be submitted or failure
    ***************************************************************************************************************************/
    CDKResult MixedPipelineTest::GenerateOfflineRequest(uint64_t frameNumber)
    {
        CDKResult result = CDKResultSuccess;
        uint32_t outputIndex = 0;

        for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
        {
            CHISTREAM* currentStream = m_streamIdMap[m_streamId[streamIndex]];

            // Skip if stream is realtime or if it is an offline input stream
            if (m_isRealtime[streamIndex] || currentStream->streamType == ChiStreamTypeInput)
            {
                continue;
            }

            void* buffer = m_streamBufferMap[currentStream]->GetOutputBufferForRequest();
            if (buffer == nullptr)
            {
                return CDKResultENoMore;
            }
            m_offlineOutputBuffers[outputIndex] = *static_cast<BufferManager::NativeChiBuffer*>(buffer);
            outputIndex++;
        }

        // Find the input stream, according to test Id
        CHISTREAM* inputStream = nullptr;
        switch (m_testId)
        {
        case TestIPEABCStatsTestgenOfflineJPEG:
        case TestIPEABCStatsOfflineJPEG:
            inputStream = m_streamIdMap[StreamUsecases::JPEGInputPort];
            break;
        case TestIPEABCStatsTestgenOfflineYUV:
        case TestIPEABCStatsOfflineYUV:
            inputStream = m_streamIdMap[StreamUsecases::IPEInputPortFull];
            break;
        case TestIPEABCStatsTestgenOfflineRaw:
        case TestIPEABCStatsOfflineRaw:
            inputStream = m_streamIdMap[StreamUsecases::BPSInputPort];
            break;
        case PreviewZSLJpegSnapshot:
        case PreviewZSLYuvSnapshot:
        case PreviewNZSLJpegSnapshot:
        case PreviewNZSLYuvSnapshot:
            inputStream = m_streamIdMap[StreamUsecases::IFEOutputPortRDI0];
            break;
        case PreviewZSLYuvSnapshotWithStats:
        case PreviewZSLJpegSnapshotWithStats:
        case PreviewNZSLYuvSnapshotWithStats:
        case PreviewNZSLJpegSnapshotWithStats:
        case LiveVideoSnapShot:
        case TestPreviewCallbackSnapshotWithThumbnail:
            inputStream = m_streamIdMap[StreamUsecases::IFEOutputPortRDI1];
            break;
        default:
            NT_LOG_ERROR( "GenerateOfflineRequest: unknown testcase Id provided: [%d]", m_testId);
            break;
        }

        // Get input buffer from input stream
        m_pInputBuffer = static_cast<BufferManager::NativeChiBuffer*>(m_streamBufferMap[inputStream]->GetInputBufferForRequest());
        if(m_pInputBuffer == nullptr)
        {
            return CDKResultEInvalidPointer;
        }

        uint32_t requestId = FRAME_REQUEST_MOD(frameNumber);

        // fetch the last RT preview frame number
        while (m_lastRealtimeFrameReceived < (frameNumber - 1)) {
            NT_LOG_DEBUG("GenerateOfflineRequest: waiting for latest frame from RT pipeline");
            // sleep for ONE_FRAME_TIME
            OsUtils::SleepMicroseconds(ONE_FRAME_TIME);
        }

        uint32_t prevRequestId = FRAME_REQUEST_MOD(m_lastRealtimeFrameReceived);

        memset(&m_snapshotRequest[requestId], 0, sizeof(CHICAPTUREREQUEST));

        m_snapshotRequest[requestId].frameNumber       = frameNumber;
        m_snapshotRequest[requestId].hPipelineHandle   = m_Pipelines[Offline]->GetPipelineHandle();
        m_snapshotRequest[requestId].numInputs         = 1;
        m_snapshotRequest[requestId].numOutputs        = outputIndex;
        m_snapshotRequest[requestId].pInputBuffers     = m_pInputBuffer;
        m_snapshotRequest[requestId].pOutputBuffers    = m_offlineOutputBuffers;
        m_snapshotRequest[requestId].pPrivData         = &m_requestPvtData[Snapshot];

#ifdef OLD_METADATA
        m_snapshotRequest[requestId].pMetadata = allocate_camera_metadata(METADATA_ENTRIES, 100 * 1024);
        NATIVETEST_UNUSED_PARAM(m_pPipelineMetadataInfo);
        NATIVETEST_UNUSED_PARAM(prevRequestId);
#else
        switch (m_testId)
        {
        // These tests do not pass buffers nor metadata from realtime to offline pipeline
        case TestIPEABCStatsTestgenOfflineJPEG:
        case TestIPEABCStatsTestgenOfflineYUV:
        case TestIPEABCStatsTestgenOfflineRaw:
        case TestIPEABCStatsOfflineJPEG:
        case TestIPEABCStatsOfflineYUV:
        case TestIPEABCStatsOfflineRaw:
            // Get the input metabuffer from the pool
            m_snapshotRequest[requestId].pInputMetadata = m_pChiMetadataUtil->GetInputMetabufferFromPool(requestId);
            NT_EXPECT(m_snapshotRequest[requestId].pInputMetadata != nullptr, "Failed to create Input Metabuffer before sending request");
            break;
        // These tests pass buffers and metadata from realtime to offline pipeline
        default:
            // Get the input metabuffer from previous realtime output metabuffer (using previous request id)
            m_snapshotRequest[requestId].pInputMetadata = m_pChiMetadataUtil->GetExistingOutputMetabufferFromPool(prevRequestId, false);
            NT_EXPECT(m_snapshotRequest[requestId].pInputMetadata != nullptr,"Failed to get previous realtime Output Metabuffer before sending request");
            break;
        }

        // Get the output metabuffer from the pool
        m_snapshotRequest[requestId].pOutputMetadata = m_pChiMetadataUtil->GetOutputMetabufferFromPool(requestId,
            m_pPipelineMetadataInfo[Offline].publishTagArray, m_pPipelineMetadataInfo[Offline].publishTagCount);
        NT_EXPECT(m_snapshotRequest[requestId].pOutputMetadata != nullptr, "Failed to create Output Metabuffer before sending request");
#endif // OLD_METADATA

        memset(&m_submitRequest, 0, sizeof(CHIPIPELINEREQUEST));
        m_submitRequest.pSessionHandle     = m_pSession[Snapshot]->GetSessionHandle();
        m_submitRequest.numRequests        = 1;
        m_submitRequest.pCaptureRequests   = &m_snapshotRequest[requestId];

        AtomicIncrement(outputIndex);

        NT_LOG_DEBUG( "Number of pending buffers remaining: %d after sending request: %" PRIu64,
            GetPendingBufferCount(), frameNumber);

        result = m_pChiModule->SubmitPipelineRequest(&m_submitRequest);

        return result;
    }


    /***************************************************************************************************************************
    *   MixedPipelineTest::CommonProcessCaptureResult()
    *
    *   @brief
    *       Overridden function implementation which processes the capture result obtained from driver
    *   @param
    *        [in]  CHICAPTURERESULT*     pCaptureResult         pointer to capture result
    *        [in]  void*                 pPrivateCallbackData   pointer private callback data
    *   @return
    *       None
    ***************************************************************************************************************************/
    void MixedPipelineTest::CommonProcessCaptureResult(
        ChiCaptureResult*   pCaptureResult,
        SessionPrivateData* pSessionPrivateData)
    {
        CDKResult result           = CDKResultSuccess;
        uint32_t  resultFrameNum   = pCaptureResult->frameworkFrameNum;
        uint32_t  numOutputBuffers = pCaptureResult->numOutputBuffers;
        SessionId sessionId        = static_cast<SessionId>(pSessionPrivateData->sessionId);

        NT_LOG_DEBUG( "Number of buffers obtained in result: [%d] for frameNumber: [%d]",
            numOutputBuffers, resultFrameNum);

#ifndef OLD_METADATA
        if (m_pChiMetadataUtil != nullptr)
        {
           m_pChiMetadataUtil->VerifyCaptureResultMetadata(static_cast<CHIMETAHANDLE>(pCaptureResult->pOutputMetadata));
        }
#endif // OLD_METADATA

        if(sessionId == PreviewRdi)
        {
            NT_LOG_DEBUG("CommonProcessCaptureResult: Last Real Time Frame Received %d", resultFrameNum);
            m_lastRealtimeFrameReceived = resultFrameNum;
            for(uint32_t bufferIndex = 0; bufferIndex < numOutputBuffers; bufferIndex++)
            {
                BufferManager::NativeChiBuffer* outBuffer = const_cast<BufferManager::NativeChiBuffer*>(&pCaptureResult->
                    pOutputBuffers[bufferIndex]);
                BufferManager::GenericBuffer genericBuffer(outBuffer);

                // Get the RDI buffer
                if(outBuffer->pStream == m_streamIdMap[StreamUsecases::IFEOutputPortRDI0] ||
                   outBuffer->pStream == m_streamIdMap[StreamUsecases::IFEOutputPortRDI1])
                {
                    // Do not dump raw frame (bidirectional buffer)
                    result = m_streamBufferMap[outBuffer->pStream]->ProcessBufferFromResult(resultFrameNum, &genericBuffer, false);
                }
                else
                {
                    // Dump preview frame
                    result = m_streamBufferMap[outBuffer->pStream]->ProcessBufferFromResult(resultFrameNum, &genericBuffer, true);
                }
            }
        }
        else if(sessionId == Snapshot)
        {
            for (uint32_t bufferIndex = 0; bufferIndex < numOutputBuffers; bufferIndex++)
            {
                BufferManager::NativeChiBuffer* outBuffer = const_cast<BufferManager::NativeChiBuffer*>(&pCaptureResult->pOutputBuffers[bufferIndex]);
                BufferManager::GenericBuffer genericBuffer(outBuffer);
                result = m_streamBufferMap[outBuffer->pStream]->ProcessBufferFromResult(resultFrameNum, &genericBuffer, true);
            }
        }
        else
        {
            NT_LOG_ERROR( "Unknown Session Id: %d obtained", sessionId);
            result = CDKResultEInvalidState;
        }

        if(result == CDKResultSuccess)
        {
            AtomicDecrement(numOutputBuffers);
            NT_LOG_DEBUG( "Number of pending buffers remaining: [%d] after receiving the result for frameNumber: [%d]",
                GetPendingBufferCount(), resultFrameNum);
        }
    }

    /***************************************************************************************************************************
    *   MixedPipelineTest::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources/objects
    *   @param
    *       None
    *   @return
    *       None
    ***************************************************************************************************************************/
    void MixedPipelineTest::DestroyResources()
    {

        switch(m_testId)
        {
            case PreviewZSLYuvSnapshot:
            case PreviewZSLYuvSnapshotWithStats:
            case PreviewZSLJpegSnapshot:
            case PreviewZSLJpegSnapshotWithStats:
            case PreviewNZSLYuvSnapshot:
            case PreviewNZSLYuvSnapshotWithStats:
            case PreviewNZSLJpegSnapshot:
            case PreviewNZSLJpegSnapshotWithStats:
            case LiveVideoSnapShot:
            case TestPreviewCallbackSnapshotWithThumbnail:
            case TestIPEABCStatsTestgenOfflineJPEG:
            case TestIPEABCStatsTestgenOfflineYUV:
            case TestIPEABCStatsTestgenOfflineRaw:
            case TestIPEABCStatsOfflineJPEG:
            case TestIPEABCStatsOfflineYUV:
            case TestIPEABCStatsOfflineRaw:
                m_Pipelines[Realtime]->DeactivatePipeline(m_pSession[PreviewRdi]->GetSessionHandle());
                m_Pipelines[Offline]->DeactivatePipeline(m_pSession[Snapshot]->GetSessionHandle());

                m_pSession[PreviewRdi]->DestroySession();
                m_pSession[Snapshot]->DestroySession();

                m_Pipelines[Realtime]->DestroyPipeline();
                m_Pipelines[Offline]->DestroyPipeline();
                break;
            default:
                NT_LOG_ERROR( "DestroyResources: unknown testcase Id provided: [%d]", m_testId);
                break;
        }

        DestroyCommonResources();
    }
}
