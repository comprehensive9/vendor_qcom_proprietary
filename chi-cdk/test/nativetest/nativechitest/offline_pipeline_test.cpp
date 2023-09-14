////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  offline_pipeline_test.h
/// @brief Definitions for the offline test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "offline_pipeline_test.h"

namespace chitests
{

    /**************************************************************************************************
    *   OfflinePipelineTest::TestOfflinePipeline
    *
    *   @brief
    *       Test an offline pipeline depending on pipelineType
    *   @param
    *       [in] TestId     testType      enum representing test Id
    *   @return
    *       None
    **************************************************************************************************/
    void OfflinePipelineTest::TestOfflinePipeline(TestId testType)
    {
        m_testId = testType;
        Setup();

        std::vector <int> camList = m_pChiModule->GetCameraList();
        m_pipelineConfig = OfflineConfig;

        // Check for FD database testing, run regular FD pipeline test otherwise
        if (m_testId == TestFDInputNV12VGAOutputBlob || m_testId == TestOfflineFD || m_testId == TestOfflineIPEFD)
        {
            if (CDKResultSuccess == ChiBufferManager::ReadFDDatabase(CmdLineParser::GetFDTestImage()))
            {
                NT_LOG_WARN("Running database testing for FD pipeline.");
            }
            else
            {
                NT_LOG_WARN("Skipping FD database testing, running regular FD pipeline test.");
            }
        }

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            m_currentTestCameraId = camList[camCounter];

            // For tests with ChiDepthNode, check that device can support depth capability
            if (m_testId == TestChiDepthNode)
            {
                bool bDepthSupported = m_pChiMetadataUtil->CheckAvailableCapability(m_currentTestCameraId, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT);
                if (!bDepthSupported)
                {
                    NT_LOG_UNSUPP( "Depth capability not supported on device, skipping...");
                    return;
                }
            }

            //1. Create chistream objects from streamcreatedata
            NT_ASSERT(SetupStreams() == CDKResultSuccess, "Fatal failure in SetupStreams() !");

            //2. Create pipelines
            NT_ASSERT(SetupPipelines(m_currentTestCameraId, nullptr) == CDKResultSuccess, "Pipelines Setup failed");

            //3. Create sessions
            NT_ASSERT(CreateSessions() == CDKResultSuccess, "Necessary sessions could not be created");

            //4. Initialize buffer managers and metabuffer pools
            NT_ASSERT(InitializeBufferManagers(m_currentTestCameraId) == CDKResultSuccess,
                "Buffer managers could not be initialized");

            // though its offline, default metadata varies from sensor to sensor, ignoring camera id is not good
            NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(m_currentTestCameraId, MAX_REQUESTS) == CDKResultSuccess,
                "Input metabuffer pool could not be created!");
            NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
                "Output metabuffer pool could not be created!");

            // start keeping time for FD results, before sending requests
            if (ChiBufferManager::m_isFDDatabaseTest || JsonParser::IsVideoFDTest())
            {
                auto now = std::chrono::steady_clock::now().time_since_epoch();  // time since beginning
                ChiBufferManager::m_startTimeFD = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
            }

            //5. Create and submit capture requests
            for (UINT32 frameNumber = START_FRAME;frameNumber <= m_captureCount; frameNumber++)
            {
                NT_ASSERT(GenerateOfflineCaptureRequest(frameNumber) == CDKResultSuccess,
                    "Submit pipeline request failed for frameNumber: %d", frameNumber);
                // For video FD database testing, wait for result before sending next request (synchronous PCR)
                if (JsonParser::IsVideoFDTest())
                {
                    NT_ASSERT(WaitForResults() == CDKResultSuccess,
                        "There are pending buffers not returned by driver after several retries");
                }
            }

            //6. Wait for remaining results (not needed in case of video FD tests)
            if (!JsonParser::IsVideoFDTest())
            {
                NT_ASSERT(WaitForResults() == CDKResultSuccess,
                    "There are pending buffers not returned by driver after several retries");
            }

            //7. Destroy all resources created
            DestroyResources();

            // For FD database testing, print FD metrics for first camera run and skip other cameras
            if (ChiBufferManager::m_isFDDatabaseTest)
            {
                ChiBufferManager::PrintFDMetrics();
                break;
            }
        }
    }

    /***************************************************************************************************************************
    *   OfflinePipelineTest::SetupStreams()
    *
    *   @brief
    *       Overridden function implementation which defines the stream information based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if stream objects could be created or failure
    ***************************************************************************************************************************/
    CDKResult OfflinePipelineTest::SetupStreams()
    {
        CDKResult result = CDKResultSuccess;

        int streamIndex = 0;

        CHISTREAMFORMAT noFormatOverride = static_cast<CHISTREAMFORMAT>(-1);

        const char* inputFileName = CmdLineParser::GetInputImage();    // Get user input at runtime;

        // Check if input width and height set for input image at runtime
        if (strlen(inputFileName) != 0)
        {
            if (m_userInputSize.isInvalidSize())
            {
                NT_LOG_ERROR("Please provide input width and height required with input image. Aborting.");
                return CDKResultEInvalidArg;
            }
        }
        m_numInputStreams = 1;
        switch (m_testId)
        {
        case TestIPEInputYUVOutputJPEG:
        case TestIPEJPEGSnapshot:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            // 2. Output stream [JPEG Snapshot]
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? UHD : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = ChiStreamFormatBlob;
            m_streamId[streamIndex]               = SNAPSHOT;

            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayNV12:
        case TestIPEDisplay:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayUBWCTP10:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? HD : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userPrvFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayUBWCNV12:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userPrvFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputUBWCTP10DisplayNV12:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_BARBARA_FHD_UBWCTP10 : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12VideoNV12:
        case TestIPEVideo:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12VideoUBWCNV12:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12VideoUBWCTP10:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? HD : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayVideoNV12:
        case TestIPEDisplayVideo:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayVideoUBWCNV12:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayVideoUBWCTP10:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? HD : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? HD : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayNV12VideoUBWCNV12:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayUBWCNV12VideoNV12:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayNV12VideoUBWCTP10:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? HD : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEInputNV12DisplayUBWCTP10VideoNV12:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? HD : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? VGA : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEBlurModule:

            m_userInputSize = m_userInputSize.isInvalidSize() ? RAW_12MP : m_userInputSize;
            m_numStreams        = 4;
            m_numInputStreams   = 3;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BLURMAPINPUT  : inputFileName;
            streamIndex++;

            //2. Input DS4 stream
            m_resolution[streamIndex]             = m_userInputSize/4;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = ChiStreamFormatY8;
            m_streamId[streamIndex]               = IPEInputDS4BlurMapBlur;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BMAP_DS4_INPUT : inputFileName;
            streamIndex++;

            //3. Input DS16 stream
            m_resolution[streamIndex]             = m_userInputSize/16;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = ChiStreamFormatY8;
            m_streamId[streamIndex]               = IPEInputDS16BlurMapBlur;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BMAP_DS16_INPUT : inputFileName;
            streamIndex++;

            //4. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? RAW_12MP : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userPrvFormat;
            m_streamId[streamIndex]               = IPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestIPEMFHDRSnapshot:
        case TestIPEMFHDRPreview:
        case TestIPEMFHDRSnapshotE2E:
        case TestIPEMFHDRPreviewE2E:
            NT_LOG_WARN("only input and output resolutions are configurable, user has to match input file names!");

            if (m_testId == TestIPEMFHDRSnapshot)
            {
                m_numStreams = 8;
                m_numInputStreams = 6;
                mNumberOfExposures = 3;
            }
            else if(m_testId == TestIPEMFHDRPreview)
            {
                m_numStreams = 6;
                m_numInputStreams = 4;
                mNumberOfExposures = 2;
            }
            else if(m_testId == TestIPEMFHDRSnapshotE2E)
            {
                m_numStreams = 7;
                m_numInputStreams = 6;
                mNumberOfExposures = 3;
            }
            else if(m_testId == TestIPEMFHDRPreviewE2E)
            {
                m_numStreams = 5;
                m_numInputStreams = 4;
                mNumberOfExposures = 2;
            }


            m_userInputSize = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;

            //1. Input stream
            m_resolution[streamIndex]              = m_userInputSize;
            m_direction[streamIndex]               = ChiStreamTypeInput;
            m_format[streamIndex]                  = ChiStreamFormatUBWCTP10;
            m_streamId[streamIndex]                = IPEInputPortFull;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? MFHDR_FHD_UBWCTP10_SHORTEXP : inputFileName;
            streamIndex++;

            //2. Input stream
            m_resolution[streamIndex]              = m_userInputSize;
            m_direction[streamIndex]               = ChiStreamTypeInput;
            m_format[streamIndex]                  = ChiStreamFormatUBWCTP10;
            m_streamId[streamIndex]                = IPEInputPortFullRef;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? MFHDR_FHD_UBWCTP10_LONGEXP : inputFileName;
            streamIndex++;

            if (m_testId == TestIPEMFHDRSnapshot || m_testId == TestIPEMFHDRSnapshotE2E) {
                //3. Input stream
                m_resolution[streamIndex]              = m_userInputSize;
                m_direction[streamIndex]               = ChiStreamTypeInput;
                m_format[streamIndex]                  = ChiStreamFormatP010;
                m_streamId[streamIndex]                = IPEInputPort2Full;
                m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? MFHDR_FHD_P010_MIDEXP : inputFileName;
                streamIndex++;
            }

            //4. Input stream
            m_resolution[streamIndex]              = m_userInputSize / 4;
            m_direction[streamIndex]               = ChiStreamTypeInput;
            m_format[streamIndex]                  = ChiStreamFormatPD10;
            m_streamId[streamIndex]                = IPEInputPortDS4;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? MFHDR_FHD_DS4_PD10_SHORTEXP : inputFileName;
            streamIndex++;

            //5. Input stream
            m_resolution[streamIndex]              = m_userInputSize / 4;
            m_direction[streamIndex]               = ChiStreamTypeInput;
            m_format[streamIndex]                  = ChiStreamFormatPD10;
            m_streamId[streamIndex]                = IPEInputPortDS4Ref;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? MFHDR_FHD_DS4_PD10_LONGEXP : inputFileName;
            streamIndex++;

            if (m_testId == TestIPEMFHDRSnapshot || m_testId == TestIPEMFHDRSnapshotE2E) {
                //6. Input stream
                m_resolution[streamIndex]              = m_userInputSize / 4;
                m_direction[streamIndex]               = ChiStreamTypeInput;
                m_format[streamIndex]                  = ChiStreamFormatPD10;
                m_streamId[streamIndex]                = IPEInputPort2DSX;
                m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? MFHDR_FHD_DS4_PD10_MIDEXP : inputFileName;
                streamIndex++;
            }

            m_userPrvSize = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;

            if (m_testId == TestIPEMFHDRSnapshot || m_testId == TestIPEMFHDRPreview)
            {
                //7. Full ref out stream
                m_resolution[streamIndex] = m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCTP10;
                m_streamId[streamIndex]   = IPEOutputPortFullRef;
                streamIndex++;

                //8. DS4 ref out stream
                m_resolution[streamIndex] = m_userPrvSize / 4;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatPD10;
                m_streamId[streamIndex]   = IPEOutputPortDS4Ref;
            }
            else if(m_testId == TestIPEMFHDRSnapshotE2E || m_testId == TestIPEMFHDRPreviewE2E)
            {
                // DisplayOut stream
                m_resolution[streamIndex] = m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
            }
            m_streamInfo.num_streams    = m_numStreams;
            break;

        case TestBPSInputRaw16OutFullUBWCTP10:
        case TestBPSSnapshot:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? FULLHD : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userSnapFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userSnapFormat;
            m_streamId[streamIndex]               = BPSOutputPortFull;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw10OutFullUBWCTP10:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? HD4K_ALT : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw10 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_MIPI_RAW_10_4K : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? HD4K_ALT : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userSnapFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userSnapFormat;
            m_streamId[streamIndex]               = BPSOutputPortFull;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw16IPEDispNV12:
        case TestBPSIPEYuvSnapshot:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? FULLHD : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userSnapFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userSnapFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw16IPEJpeg:
        case TestBPSIPEJPEGSnapshot:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? FULLHD : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = ChiStreamFormatBlob;
            m_streamId[streamIndex]               = SNAPSHOT;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw16IPEDispUBWCNV12:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? FULLHD : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userSnapFormat == noFormatOverride) ? ChiStreamFormatUBWCNV12 : m_userSnapFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw16IPEDispUBWCTP10:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = m_userSnapSize.isInvalidSize() ? FULLHD : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userSnapFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userSnapFormat;
            m_streamId[streamIndex]               = PREVIEW;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw16StatsOut:
        case TestBPSInputFHDRaw16StatsOut:
        case TestBPSStats:
            m_numStreams = 4;

            //1. Input stream
            m_direction[streamIndex] = ChiStreamTypeInput;
            m_format[streamIndex]    = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]  = BPSInputPort;
            if (m_testId == TestBPSInputFHDRaw16StatsOut)
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
                streamIndex++;

                //2. BPS Full stream (offline preview/reprocess)
                m_resolution[streamIndex] =  m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            }
            else
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? UHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
                streamIndex++;

                //2. BPS Full stream (offline preview/reprocess)
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize;
            }

            m_direction[streamIndex] = ChiStreamTypeOutput;
            m_format[streamIndex]    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userPrvFormat;
            m_streamId[streamIndex]  = BPSOutputPortFull;
            streamIndex++;

            //3. BPS BG stats stream
            m_resolution[streamIndex] = Size(691200, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = BPSOutputPortStatsBG;
            streamIndex++;

            //4. BPS bHist stream
            m_resolution[streamIndex] = Size(3072, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = BPSOutputPortStatsBhist;
            m_streamInfo.isJpeg       = false;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestBPSInputFHDRaw16StatsOutWithIPE:
        case TestBPSInputUHDRaw16StatsOutWithIPE:
        case TestBPSIPEYuvStats:
            m_numStreams = 4;

            //1. Input stream
            m_direction[streamIndex] = ChiStreamTypeInput;
            m_format[streamIndex]    = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]  = BPSInputPort;
            if (m_testId == TestBPSInputFHDRaw16StatsOutWithIPE)
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
                streamIndex++;

                //2. IPE Disp stream
                m_resolution[streamIndex] =  m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            }
            else
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? UHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
                streamIndex++;

                //2. IPE Disp stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize;
            }

            m_direction[streamIndex] = ChiStreamTypeOutput;
            m_format[streamIndex]    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]  = IPEOutputPortDisplay;
            streamIndex++;

            //3. BPS BG stats stream
            m_resolution[streamIndex] = Size(691200, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = BPSOutputPortStatsBG;
            streamIndex++;

            //4. BPS bHist stream
            m_resolution[streamIndex] = Size(3072, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = BPSOutputPortStatsBhist;
            m_streamInfo.isJpeg       = false;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestBPSInputFHDRaw16BGStatsWithIPE:
        case TestBPSInputUHDRaw16BGStatsWithIPE:
        case TestBPSIPEYuvStatsBG:
            m_numStreams = 3;

            //1. Input stream
            m_direction[streamIndex] = ChiStreamTypeInput;
            m_format[streamIndex]    = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]  = BPSInputPort;
            if (m_testId == TestBPSInputFHDRaw16BGStatsWithIPE || m_testId  == TestBPSIPEYuvStatsBG)
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
                streamIndex++;

                //2. IPE Disp stream
                m_resolution[streamIndex] =  m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            }
            else
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? UHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
                streamIndex++;

                //2. IPE Disp stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize;
            }

            m_direction[streamIndex] = ChiStreamTypeOutput;
            m_format[streamIndex]    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]  = IPEOutputPortDisplay;
            streamIndex++;

            //3. BPS BG stats stream
            m_resolution[streamIndex] = Size(691200, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = BPSOutputPortStatsBG;

            m_streamInfo.isJpeg       = false;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestBPSInputFHDRaw16HistStatsWithIPE:
        case TestBPSInputUHDRaw16HistStatsWithIPE:
        case TestBPSIPEYuvStatsBHist:
            m_numStreams = 3;

            //1. Input stream
            m_direction[streamIndex] = ChiStreamTypeInput;
            m_format[streamIndex]    = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]  = BPSInputPort;
            if (m_testId == TestBPSInputFHDRaw16HistStatsWithIPE || m_testId == TestBPSIPEYuvStatsBHist)
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
                streamIndex++;

                //2. IPE Disp stream
                m_resolution[streamIndex] =  m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            }
            else
            {
                m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? UHD : m_userInputSize;
                m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
                streamIndex++;

                //2. IPE Disp stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize;
            }

            m_direction[streamIndex] = ChiStreamTypeOutput;
            m_format[streamIndex]    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]  = IPEOutputPortDisplay;
            streamIndex++;

            //3. BPS bHist stream
            m_resolution[streamIndex] = Size(3072, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = BPSOutputPortStatsBhist;

            m_streamInfo.isJpeg       = false;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestJPEGInputNV12VGAOutputBlob:
        case TestJPEGNodeJPEGSnapshot:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]  = ChiStreamTypeInput;
            m_format[streamIndex]     = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]   = JPEGInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex] = m_userSnapSize.isInvalidSize() ? VGA : m_userSnapSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestJPEGInputNV124KOutputBlob:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? HD4K : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = JPEGInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_EMULATEDSENSOR_4K : inputFileName;
            streamIndex++;

            //2. Snapshot stream
            m_resolution[streamIndex]             = (m_userSnapSize.isInvalidSize()) ? HD4K : m_userSnapSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = ChiStreamFormatBlob;
            m_streamId[streamIndex]               = SNAPSHOT;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSInputRaw16OutputGPUPorts:
        case TestBPSGPUWithDS:
            m_numStreams = 5;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? UHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ?  ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
            streamIndex++;

            //2. GPU FULL stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatP010 : m_userPrvFormat;
            m_streamId[streamIndex]               = GPUOutputPortFull;
            streamIndex++;

            //3. GPU DS4 stream
            m_resolution[streamIndex]             = (m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize) / 4;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = ChiStreamFormatP010;
            m_streamId[streamIndex]               = GPUOutputPortDS4;
            streamIndex++;

            //4. GPU DS16 stream
            m_resolution[streamIndex]             = (m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize) / 16;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = ChiStreamFormatP010;
            m_streamId[streamIndex]               = GPUOutputPortDS16;
            streamIndex++;

            //5. GPU DS64 stream
            m_resolution[streamIndex]             = (m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize) / 64;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = ChiStreamFormatP010;
            m_streamId[streamIndex]               = GPUOutputPortDS64;

            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestFDInputNV12VGAOutputBlob:
        case TestOfflineFD:
        case TestOfflineIPEFD:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex] = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]  = ChiStreamTypeInput;
            m_format[streamIndex]     = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]   = ChiNodeInputPort0;
            if (ChiBufferManager::m_isFDDatabaseTest || JsonParser::IsVideoFDTest()) // In case of FD database testing, pick preformatted input
            {
                (m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? FD_FIRST_INPUT_FRAME : inputFileName);
            }
            else
            {
                (m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName);
            }
            streamIndex++;

            //2. FD manager stream
            m_resolution[streamIndex] = Size(16384, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = FDManagerOutputPort;

            m_streamInfo.isJpeg       = false;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestBPSInputRaw10WithIPEAndDS:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? HD4K_ALT : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw10 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_MIPI_RAW_10_4K : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? HD4K_ALT : m_userPrvSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]   = PREVIEW;

            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestBPSInputRaw16WithIPEAndDS:
        case TestBPSIPEWithDS: // IPE with DS property enabled
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? UHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ?  ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]             = m_userPrvSize.isInvalidSize() ? UHD : m_userPrvSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]               = PREVIEW;

            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestBPSIdealRaw:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ?  ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Ideal raw stream (DS4) -- why DS4?
            m_resolution[streamIndex]             = m_userRDISize.isInvalidSize() ? FULLHD : m_userRDISize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userPrvFormat;
            m_streamId[streamIndex]               = BPSOutputPortDS4;

            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestChiExternalNodeBPSIPE:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = FULLHD;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = ChiStreamFormatRaw16;
            m_streamId[streamIndex]               = ChiNodeInputPort0;
            m_streamInfo.inputImages[streamIndex] = BPS_COLORBAR_FHD;
            m_isRealtime[streamIndex]             = false;
            streamIndex++;

            //2. SNAPSHOT stream
            m_resolution[streamIndex] = FULLHD;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.num_streams = m_numStreams;
            break;

        case TestChiExternalNodeBPSIPEJpeg:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = UHD;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = ChiStreamFormatRaw16;
            m_streamId[streamIndex]               = ChiNodeInputPort0;
            m_streamInfo.inputImages[streamIndex] = BPS_IDEALRAW_UHD;
            m_isRealtime[streamIndex]             = false;
            streamIndex++;

            //2. SNAPSHOT stream
            m_resolution[streamIndex] = FULLHD;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestBPSIPE2Streams:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]             = FULLHD;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = ChiStreamFormatRaw16;
            m_streamId[streamIndex]               = BPSInputPort;
            m_streamInfo.inputImages[streamIndex] = BPS_COLORBAR_FHD;
            m_isRealtime[streamIndex]             = false;
            streamIndex++;

            //2. IPE Display
            m_resolution[streamIndex] = FULLHD;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
            m_streamId[streamIndex]   = IPEOutputPortDisplay;
            m_isRealtime[streamIndex] = false;
            streamIndex++;

            //3. IPE Video
            m_resolution[streamIndex] = FULLHD;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
            m_streamId[streamIndex]   = IPEOutputPortVideo;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.num_streams = m_numStreams;
            break;

        case TestChiDepthNode:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = FULLHD;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = ChiStreamFormatRaw16;
            m_streamId[streamIndex]               = ChiNodeInputPort0;
            m_streamInfo.inputImages[streamIndex] = BPS_COLORBAR_FHD;
            m_isRealtime[streamIndex]             = false;
            streamIndex++;

            //2. Depth stream
            m_resolution[streamIndex] = FULLHD;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatY16;
            m_streamId[streamIndex]   = ChiNodeOutputPort0;
            m_isRealtime[streamIndex] = false;

            m_streamInfo.isJpeg = false;
            m_streamInfo.num_streams = m_numStreams;
            break;

        case TestOPEInputNV12DisplayNV12:
        case TestOPEInputNV12DisplayNV21:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]              = m_userInputSize.isInvalidSize() ? VGA : m_userInputSize;
            m_direction[streamIndex]               = ChiStreamTypeInput;
            m_format[streamIndex]                  = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]                = OPEInputPortFull;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex]              = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
            m_direction[streamIndex]               = ChiStreamTypeOutput;
            m_format[streamIndex]                  = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]                = PREVIEW;
            m_streamInfo.num_streams               = m_numStreams;
            break;

        case TestOPEInputNV12VideoNV12:
        case TestOPEInputNV12VideoNV21:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]             = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]              = ChiStreamTypeInput;
            m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]               = OPEInputPortFull;
            m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Video stream
            m_resolution[streamIndex]             = m_userVidSize.isInvalidSize() ? FULLHD : m_userVidSize;
            m_direction[streamIndex]              = ChiStreamTypeOutput;
            m_format[streamIndex]                 = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]               = OPEOutputPortVideo;
            m_streamInfo.num_streams              = m_numStreams;
            break;

        case TestOPEInputConfigOutFullNV12:
        case TestOPEInputConfigOutFullNV21:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]              = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_format[streamIndex]                  = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            m_direction[streamIndex]                   = ChiStreamTypeInput;
            m_streamId[streamIndex]                    = OPEInputPortFull;
            streamIndex++;

            //2. OPE output full stream
            m_resolution[streamIndex]                  = m_userPrvSize.isInvalidSize() ? HD4K_ALT : m_userPrvSize;
            m_direction[streamIndex]                   = ChiStreamTypeOutput;
            m_format[streamIndex]                      = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]                    = OPEOutputPortDisplay;
            m_streamInfo.num_streams                   = m_numStreams;
            break;

        case TestOPEInputNV12DisplayNV21VideoNV12:
        case TestOPEInputNV12DisplayNV12VideoNV21:
        case TestOPEInputNV12DisplayVideoNV12:
        case TestOPEInputNV12DisplayVideoNV21:
            m_numStreams = 3;

            //1. Input stream
            m_resolution[streamIndex]               = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]                = ChiStreamTypeInput;
            m_format[streamIndex]                   = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
            m_streamId[streamIndex]                 = OPEInputPortFull;
            m_streamInfo.inputImages[streamIndex]   = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. Preview stream
            m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]   = OPEOutputPortDisplay;
            streamIndex++;

            //3. Video stream
            m_resolution[streamIndex] = m_userVidSize.isInvalidSize() ? FULLHD : m_userVidSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = (m_userVidFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userVidFormat;
            m_streamId[streamIndex]   = OPEOutputPortVideo;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestOPEJPEGSWSnapshot:
            m_numStreams = 2;

            //1. Input stream
            m_resolution[streamIndex]              = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_direction[streamIndex]               = ChiStreamTypeInput;
            m_format[streamIndex]                  = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]                = OPEInputPortFull;
            m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. snapshot stream
            m_resolution[streamIndex] = (m_userSnapSize.isInvalidSize()) ? FULLHD : m_userSnapSize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = SNAPSHOT;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        case TestOPEInputConfigStatsOut:
            m_numStreams = 4;

            //1. Input stream
            m_direction[streamIndex]                  = ChiStreamTypeInput;
            m_format[streamIndex]                     = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
            m_streamId[streamIndex]                   = OPEInputPortFull;
            m_resolution[streamIndex]                 = m_userInputSize.isInvalidSize() ? FULLHD : m_userInputSize;
            m_streamInfo.inputImages[streamIndex]     = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
            streamIndex++;

            //2. OPE Display stream (offline preview/reprocess)
            m_resolution[streamIndex] =  m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;

            m_direction[streamIndex] = ChiStreamTypeOutput;
            m_format[streamIndex]    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
            m_streamId[streamIndex]  = OPEOutputPortDisplay;
            streamIndex++;

            //3. OPE RS stats stream
            m_resolution[streamIndex] = Size(691200, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = OPEOutputPortStatsRS;
            streamIndex++;

            //4. OPE IHist stream
            m_resolution[streamIndex] = Size(3072, 1);
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_format[streamIndex]     = ChiStreamFormatBlob;
            m_streamId[streamIndex]   = OPEOutputPortStatsIHIST;
            m_streamInfo.isJpeg       = false;
            m_streamInfo.num_streams  = m_numStreams;
            break;

        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            result = CDKResultENoSuch;
            break;
        }

        if (result == CDKResultSuccess)
        {
            m_streamInfo.num_ipstreams = m_numInputStreams;
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
                                // add usage flag for appropriate format
                                if (m_format[index] == ChiStreamFormatUBWCTP10)
                                {
                                    m_usageFlag[streamIndex] = GrallocUsageTP10;
                                }
                                else if (m_format[index] == ChiStreamFormatP010)
                                {
                                    m_usageFlag[streamIndex] = GrallocUsageP010;
                                }
                                m_isRealtime[index]                          = false;
                                m_pRequiredStreams[index].maxNumBuffers      = MAX_REQUESTS;
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
    *   OfflinePipelineTest::SetupPipelines()
    *
    *   @brief
    *       Overridden function implementation which creates pipeline objects based on the test Id
    *   @param
    *       [in]  int                 cameraId    cameraId to be associated with the pipeline
    *       [in]  ChiSensorModeInfo*  sensorMode  sensor mode to be used for the pipeline
    *   @return
    *       CDKResult success if pipelines could be created or failure
    ***************************************************************************************************************************/
    CDKResult OfflinePipelineTest::SetupPipelines(int cameraId, ChiSensorModeInfo * sensorMode)
    {
        CDKResult result = CDKResultSuccess;

        switch (m_testId)
        {
        case TestIPEInputYUVOutputJPEG:
        case TestIPEJPEGSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEJPEG);
            break;
        case TestIPEInputNV12DisplayNV12:
        case TestIPEInputNV12DisplayUBWCTP10:
        case TestIPEInputNV12DisplayUBWCNV12:
        case TestIPEInputUBWCTP10DisplayNV12:
        case TestIPEDisplay:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEDisp);
            break;
        case TestIPEInputNV12VideoNV12:
        case TestIPEInputNV12VideoUBWCNV12:
        case TestIPEInputNV12VideoUBWCTP10:
        case TestIPEVideo:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEVideo);
            break;
        case TestIPEInputNV12DisplayVideoNV12:
        case TestIPEInputNV12DisplayVideoUBWCNV12:
        case TestIPEInputNV12DisplayVideoUBWCTP10:
        case TestIPEInputNV12DisplayNV12VideoUBWCNV12:
        case TestIPEInputNV12DisplayUBWCNV12VideoNV12:
        case TestIPEInputNV12DisplayNV12VideoUBWCTP10:
        case TestIPEInputNV12DisplayUBWCTP10VideoNV12:
        case TestIPEDisplayVideo:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEDispVideo);
            break;
        case TestIPEBlurModule:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEBlurModule);
            break;
        case TestIPEMFHDRSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEMFHDRSnapshot);
            break;
        case TestIPEMFHDRSnapshotE2E:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEMFHDRSnapshotE2E);
            break;
        case TestIPEMFHDRPreview:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEMFHDRPreview);
            break;
        case TestIPEMFHDRPreviewE2E:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEMFHDRPreviewE2E);
            break;
        case TestBPSInputRaw16OutFullUBWCTP10:
        case TestBPSInputRaw10OutFullUBWCTP10:
        case TestBPSSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPS);
            break;
        case TestBPSInputRaw16IPEDispNV12:
        case TestBPSInputRaw16IPEDispUBWCNV12:
        case TestBPSInputRaw16IPEDispUBWCTP10:
        case TestBPSIPEYuvSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSIPE);
            break;
        case TestBPSInputRaw16IPEJpeg:
        case TestBPSIPEJPEGSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSIPEJpeg);
            break;
        case TestBPSInputRaw16StatsOut:
        case TestBPSInputFHDRaw16StatsOut:
        case TestBPSStats:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSStats);
            break;
        case TestBPSInputFHDRaw16StatsOutWithIPE:
        case TestBPSInputUHDRaw16StatsOutWithIPE:
        case TestBPSIPEYuvStats:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSStatsWithIPE);
            break;
        case TestBPSInputFHDRaw16BGStatsWithIPE:
        case TestBPSInputUHDRaw16BGStatsWithIPE:
        case TestBPSIPEYuvStatsBG:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSBGStatsWithIPE);
            break;
        case TestBPSInputFHDRaw16HistStatsWithIPE:
        case TestBPSInputUHDRaw16HistStatsWithIPE:
        case TestBPSIPEYuvStatsBHist:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSHistStatsWithIPE);
            break;
        case TestJPEGInputNV12VGAOutputBlob:
        case TestJPEGInputNV124KOutputBlob:
        case TestJPEGNodeJPEGSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineJpegSnapshot);
            break;
        case TestBPSInputRaw16OutputGPUPorts:
        case TestBPSGPUWithDS:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSGPUpipeline);
            break;
        case TestFDInputNV12VGAOutputBlob:
        case TestOfflineFD:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineFDPipeline);
            break;
        case TestOfflineIPEFD:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEFDPipeline);
            break;
        case TestBPSInputRaw10WithIPEAndDS:
        case TestBPSInputRaw16WithIPEAndDS:
        case TestBPSIPEWithDS:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSWithIPEAndDS);
            break;
        case TestBPSIdealRaw:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSDS4);
            break;
         case TestChiExternalNodeBPSIPE:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineExtBPSIPE);
            break;
        case TestChiExternalNodeBPSIPEJpeg:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineExtBPSIPEJPEG);
            break;
        case TestBPSIPE2Streams:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSIPEDispVideo);
            break;
         case TestChiDepthNode:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineDepthNode);
            break;
        case TestOPEInputNV12DisplayNV12:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEDisplayOnNV12);
            break;
        case TestOPEInputNV12DisplayNV21:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEDisplayOnNV21);
            break;
        case TestOPEInputNV12VideoNV12:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEVideoNV12);
            break;
        case TestOPEInputNV12VideoNV21:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEVideoNV21);
            break;
        case TestOPEInputConfigOutFullNV12:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEInputConfigOutNV12);
            break;
        case TestOPEInputConfigOutFullNV21:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEInputConfigOutNV21);
            break;
        case TestOPEInputNV12DisplayNV21VideoNV12:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEDisplayNV21VideoNV12);
            break;
        case TestOPEInputNV12DisplayNV12VideoNV21:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEDisplayNV12VideoNV21);
            break;
        case TestOPEInputNV12DisplayVideoNV12:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEDispVideoNV12);
            break;
        case TestOPEInputNV12DisplayVideoNV21:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEDispVideoNV21);
            break;
        case TestOPEJPEGSWSnapshot:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEJPEGSWSnapshot);
            break;
        case TestOPEInputConfigStatsOut:
            m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEStats);
            break;
        default:
            NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
            return CDKResultENoSuch;
        }

        result = (m_pChiPipeline[Offline] != nullptr) ? m_pChiPipeline[Offline]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        return result;
    }

    /***************************************************************************************************************************
    *   OfflinePipelineTest::CreateSessions()
    *
    *   @brief
    *       Overridden function implementation which creates required number of sessions based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if sessions could be created or failure
    ***************************************************************************************************************************/
    CDKResult OfflinePipelineTest::CreateSessions()
    {
        CDKResult result = CDKResultSuccess;
        ChiCallBacks callbacks = { 0 };

        callbacks.ChiNotify                      = ChiTestCase::ProcessMessage;
        callbacks.ChiProcessCaptureResult        = ChiTestCase::QueueCaptureResult;
        callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

        mPerSessionPvtData[OfflineSession].pTestInstance = this;
        mPerSessionPvtData[OfflineSession].sessionId     = OfflineSession;
        mPerSessionPvtData[OfflineSession].cameraId      = m_currentTestCameraId;

        mPerSessionPvtData[OfflineSession].pChiSession   = ChiSession::Create(&m_pChiPipeline[Offline], 1, &callbacks,
                                                            &mPerSessionPvtData[OfflineSession]);
        mPerSessionPvtData[OfflineSession].sessionHandle = mPerSessionPvtData[OfflineSession].pChiSession->GetSessionHandle();

        if (mPerSessionPvtData[OfflineSession].pChiSession == nullptr)
        {
            NT_LOG_ERROR( "offline session could not be created");
            result = CDKResultEFailed;
        }
        else
        {
            if(!m_nEarlyPCR) // if early PCR not enabled, activate pipeline here
            {
                result = m_pChiPipeline[Offline]->ActivatePipeline(mPerSessionPvtData[OfflineSession].sessionHandle);
            }

            if (result == CDKResultSuccess)
            {
                result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                    mPerSessionPvtData[OfflineSession].sessionHandle,
                    m_pChiPipeline[Offline]->GetPipelineHandle(), &m_pPipelineMetadataInfo[Offline]);
            }
        }

        return result;
    }


    /***************************************************************************************************************************
    *   OfflinePipelineTest::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources/objects
    *   @param
    *       None
    *   @return
    *       None
    ***************************************************************************************************************************/
    void OfflinePipelineTest::DestroyResources()
    {
        if ((nullptr != mPerSessionPvtData[OfflineSession].pChiSession) && (nullptr != m_pChiPipeline[Offline]))
        {
            m_pChiPipeline[Offline]->DeactivatePipeline(mPerSessionPvtData[OfflineSession].sessionHandle);
        }

        if (mPerSessionPvtData[OfflineSession].pChiSession != nullptr)
        {
            mPerSessionPvtData[OfflineSession].pChiSession->DestroySession();
            mPerSessionPvtData[OfflineSession].pChiSession = nullptr;
        }

        if (m_pChiPipeline[Offline] != nullptr)
        {
            m_pChiPipeline[Offline]->DestroyPipeline();
            m_pChiPipeline[Offline] = nullptr;
        }
        DestroyCommonResources();
    }
}
