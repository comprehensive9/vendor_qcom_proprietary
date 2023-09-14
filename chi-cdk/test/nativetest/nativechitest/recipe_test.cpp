////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  recipe_test.cpp
/// @brief Definitions for the Recipe test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "recipe_test.h"

namespace chitests
{

/***************************************************************************************************************************
*   RecipeTest::TestRealtimePipeline
*
*   @brief
*       Recipe test for realtime pipelines
*   @param
*       [in]    TestId      testId          Test identifier
*   @return
*       None
***************************************************************************************************************************/
void RecipeTest::TestRealtimePipeline(TestId testId)
{
    m_testId = testId;
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

        // Create chistream objects from streamcreatedata
        NT_ASSERT(SetupStreams() == CDKResultSuccess, "SetupStreams Failed");

        /* Set session level parameters
        if(CmdLineParser::isTestGenMode())
        {
            NT_ASSERT(CDKResultSuccess == SetSessionParameters(MetadataUsecases::Testgen, m_currentTestCameraId, m_streamInfo.maxRes),
                                            "Could not set session level parameters for cameraID %d", m_currentTestCameraId);
        }
        */

        ChiSensorModeInfo* testSensorMode = nullptr;
        testSensorMode = m_pChiModule->GetClosestSensorMode(m_currentTestCameraId, m_streamInfo.maxRes);

        NT_ASSERT(nullptr != testSensorMode, "Sensor mode is NULL for cameraId: ",m_currentTestCameraId);

        NT_LOG_INFO("Camera %d chosen sensor mode: %u, width: %u, height: %u", m_currentTestCameraId,
            testSensorMode->modeIndex, testSensorMode->frameDimension.width, testSensorMode->frameDimension.height);

        // TODO: Set number of batched frames based on usecase
        testSensorMode->batchedFrames = 1;

        // For RDI cases, we need to make sure stream resolution matches sensor mode
        for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
        {
            if ((IsRDIStream(m_pRequiredStreams[streamIndex].format)) &&
                (!IsSensorModeMaxResRawUnsupportedTFEPort(m_streamId[streamIndex])))
            {
                if (!m_userRDISize.isInvalidSize() && m_streamId[streamIndex] == CONFIG_OUTPUT) {
                    NT_LOG_INFO("OVERRIDE: RDI resolution set by User width: %d, height: %d ",
                        m_userRDISize.width, m_userRDISize.height);
                    m_pRequiredStreams[streamIndex].width  = m_userRDISize.width;
                    m_pRequiredStreams[streamIndex].height = m_userRDISize.height;
                }
                else if (!m_isCamIFTestGen)
                {
                    NT_LOG_INFO("Selected RDI resolution based on Sensor mode width: %d, height: %d ",
                        testSensorMode->frameDimension.width, testSensorMode->frameDimension.height);
                    m_pRequiredStreams[streamIndex].width  = testSensorMode->frameDimension.width;
                    m_pRequiredStreams[streamIndex].height = testSensorMode->frameDimension.height;
                }
            }
        }

        // Create pipelines
        NT_ASSERT(SetupPipelines(m_currentTestCameraId, testSensorMode) == CDKResultSuccess, "Pipelines Setup failed");
        // Create sessions
        NT_ASSERT(CreateSessions() == CDKResultSuccess,"Necessary sessions could not be created");

        // Initialize buffer manager and metabuffer pools
        NT_ASSERT(InitializeBufferManagers(m_currentTestCameraId) == CDKResultSuccess, "Buffer managers could not be initialized");

        NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(m_currentTestCameraId, MAX_REQUESTS) == CDKResultSuccess,
            "Input metabuffer pool for camera Id: %d could not be created!", m_currentTestCameraId);
        NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
            "Output metabuffer pool could not be created!");

        // Create and submit capture requests
        for (UINT32 frameNumber = START_FRAME; frameNumber <= m_captureCount; frameNumber++)
        {
            NT_ASSERT(GenerateRealtimeCaptureRequest(frameNumber) == CDKResultSuccess,
                "Submit pipeline request failed for frameNumber: %d", frameNumber);
        }

        // Wait for all results
        NT_ASSERT(WaitForResults() == CDKResultSuccess,
            "There are pending buffers not returned by driver after several retries");
        // Destroy all resources created
        DestroyResources();
    }
}

/***************************************************************************************************************************
*   RecipeTest::TestOfflinePipeline
*
*   @brief
*       Recipe test for offline pipelines
*   @param
*       [in]    TestId      testId          Test identifier
*   @return
*       None
***************************************************************************************************************************/
void RecipeTest::TestOfflinePipeline(TestId testId)
{
    m_testId = testId;
    Setup();

    m_pipelineConfig = OfflineConfig;

    // Create chistream objects from streamcreatedata
    NT_ASSERT(SetupStreams() == CDKResultSuccess, "SetupStreams Failed");

    // Create pipelines - offline pipelines don't require cameraId and sensor mode
    NT_ASSERT(SetupPipelines(0, NULL) == CDKResultSuccess,"Pipelines Setup failed");

    // Create sessions
    NT_ASSERT(CreateSessions() == CDKResultSuccess,"Necessary sessions could not be created");

    // Initialize buffer manager and metabuffer pools
    NT_ASSERT(InitializeBufferManagers(-1) == CDKResultSuccess, "Buffer managers could not be initialized");

    // hard coded the camera Id as offline piptlines does not need camera Id
    NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(0, MAX_REQUESTS) == CDKResultSuccess,
        "Input metabuffer pool for camera Id: 0 could not be created!");
    NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
        "Output metabuffer pool could not be created!");

    // Create and submit capture requests
    for (UINT32 frameNumber = START_FRAME; frameNumber <= m_captureCount; frameNumber++)
    {
        NT_ASSERT(GenerateOfflineCaptureRequest(frameNumber) == CDKResultSuccess,
            "Submit pipeline request failed for frameNumber: ", frameNumber);
    }

    // Wait for all results
    NT_ASSERT(WaitForResults() == CDKResultSuccess,
        "There are pending buffers not returned by driver after several retries");

    // Destroy all resources created
    DestroyResources();
}

/***************************************************************************************************************************
*   RecipeTest::SetupStreams()
*
*   @brief
*       Overridden function implementation which defines the stream information based on the test Id
*   @param
*       None
*   @return
*       CDKResult success if stream objects could be created or failure
***************************************************************************************************************************/
CDKResult RecipeTest::SetupStreams()
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
            NT_LOG_ERROR("Input width and height required with input image. Aborting.");
            return CDKResultEInvalidArg;
        }
    }

    m_numInputStreams = 0;
    switch (m_testId)
    {
    case TestIFEGeneric:
        m_numStreams = 1;

        //1. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestIFEHDR:
    {
        UINT32 nExposures = CmdLineParser::GetNumExposures();

        //1. IFE full out stream
        m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_isRealtime[streamIndex] = true;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        m_streamId[streamIndex]   = CONFIG_OUTPUT;

        //2. IFE RAW0 out stream, 1-exposure - default
        streamIndex++;
        m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_isRealtime[streamIndex] = true;
        m_format[streamIndex]     = ChiStreamFormatRaw10;
        m_streamId[streamIndex]   = IFEOutputPortRAW0;
        m_numStreams = 2;

        if(nExposures >= 2) // 2-exposure and 3-exposure
        {
            //3. IFE RAW1 out stream, 2-exposure
            streamIndex++;
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
            m_direction[streamIndex]  = ChiStreamTypeOutput;
            m_isRealtime[streamIndex] = true;
            m_format[streamIndex]     = ChiStreamFormatRaw10;
            m_streamId[streamIndex]   = IFEOutputPortRAW1;
            m_numStreams = 3;

            if(nExposures == 3) // 3-exposure
            {
                //4. IFE RAW2 out stream
                streamIndex++;
                m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_isRealtime[streamIndex] = true;
                m_format[streamIndex]     = ChiStreamFormatRaw10;
                m_streamId[streamIndex]   = IFEOutputPortRAW2;
                m_numStreams = 4;
            }
        }

        m_streamInfo.num_streams  = m_numStreams;
        break;
    }
    case TestIFEGenericOffline:
        m_numStreams = 2;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? HD4K : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_userInputFormat                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_format[streamIndex]                 = m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_userPrvFormat    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        m_format[streamIndex]     = m_userPrvFormat;
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestIFEDownscaleGeneric:
        m_numStreams = 3;

        //1. IFE full out stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //2. DS4 stream
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize/4;
        }
        else
        {
            m_resolution[streamIndex] = ((m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize) / 4;
        }
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatPD10;
        m_streamId[streamIndex]   = IFEOutputPortDS4;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //2. DS16 stream
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize / 16;
        }
        else
        {
            m_resolution[streamIndex] = ((m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize) / 16;
        }
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatPD10;
        m_streamId[streamIndex]   = IFEOutputPortDS16;
        m_isRealtime[streamIndex] = true;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestIPEGeneric:
        m_numStreams = 2;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? VGA : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestJPEGGeneric:
        m_numStreams = 2;
        m_numInputStreams = 1;

        // 1. Configurable input stream
        m_resolution[streamIndex] = (m_userInputSize.isInvalidSize()) ? VGA : m_userInputSize;
        m_direction[streamIndex]  = ChiStreamTypeInput;
        m_format[streamIndex]     = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
        m_streamId[streamIndex]   = JPEGInputPort;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        // 2. Configurable output stream
        m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? VGA : m_userPrvSize;
        if(m_userPrvSize != m_userInputSize)
        {
            NT_LOG_UNSUPP("Output Resolution must match input resolution");
            return CDKResultEInvalidArg;
        }
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatBlob;
        m_streamId[streamIndex]   = SNAPSHOT;
        m_isRealtime[streamIndex] = false;
        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestIPEGeneric2StreamDisplay:

        m_numStreams = 3;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? VGA : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_COLORBAR_VGA : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //3. Static DISPLAY output stream
        m_resolution[streamIndex] = VGA;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]   = IPEOutputPortDisplay;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestBPSGeneric:
        m_numStreams = 2;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? FULLHD : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatUBWCTP10 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestIFEGeneric2Stream:
    case TestIFEGeneric2StreamDisplay:
        m_numStreams = 2;

        //1. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userPrvFormat;
        // for non-RDI stream/format, use non-RDI size (if given at runtime)
        if (!IsRDIStream(m_format[streamIndex]) && (!m_userPrvSize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userPrvSize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //2. Static out stream
        m_resolution[streamIndex] = FULLHD;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]   = (m_testId == TestIFEGeneric2Stream) ? IFEOutputPortFull : IFEOutputPortDisplayFull;;
        m_isRealtime[streamIndex] = true;

        m_streamInfo.num_streams  = m_numStreams;
        break;
    case TestIFELiteTLBGStatsOut:
        m_numStreams = 2;

        //1. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userPrvFormat;
        // for non-RDI stream/format, use non-RDI size (if given at runtime)
        if (!IsRDIStream(m_format[streamIndex]) && (!m_userPrvSize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userPrvSize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //2. Static output stream
        m_resolution[streamIndex] = Size(147456, 1);;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatBlob;
        m_streamId[streamIndex]   = IFELITEOutputPortStatsBG;
        m_isRealtime[streamIndex] = true;

        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestIFELiteRawTLBGStatsOut:
        m_numStreams = 3;

        //1. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userPrvFormat;
        // for non-RDI stream/format, use non-RDI size (if given at runtime)
        if (!IsRDIStream(m_format[streamIndex]) && (!m_userPrvSize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userPrvSize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //2. Static output stream
        m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatRawOpaque;
        m_streamId[streamIndex]   = IFELITEOutputPortPreProcessRaw;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //3. Stats Out
        m_resolution[streamIndex] = Size(147456, 1);;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatBlob;
        m_streamId[streamIndex]   = IFELITEOutputPortStatsBG;
        m_isRealtime[streamIndex] = true;

        m_streamInfo.num_streams  = m_numStreams;
        break;
    case TestBPSDownscaleGeneric:
        m_numStreams = 5;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex] = (m_userInputSize.isInvalidSize()) ? HD4K : m_userInputSize;
        m_direction[streamIndex]  = ChiStreamTypeInput;
        m_format[streamIndex]     = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_streamId[streamIndex]   = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_IDEALRAW_UHD : inputFileName;
        m_isRealtime[streamIndex] = false;
        // Warn if ICA limit is hit on given input resolution stream (since DS64 is being used)
        if (m_resolution[streamIndex].isICALimitHit(64))
        {
            NT_LOG_WARN("Selected resolution is hitting ICA minimum width and height requirements (%d %d)", ICAMinWidthPixels, ICAMinHeightPixels);
        }
        streamIndex++;

        //2. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? HD4K : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //3. BPS DS4 out stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatPD10;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize / 4;
        }
        else
        {
            m_resolution[streamIndex] = ((m_userPrvSize.isInvalidSize()) ? HD4K : m_userPrvSize) / 4;
        }
        m_streamId[streamIndex]   = BPSOutputPortDS4;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //4. BPS DS16 out stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatPD10;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize / 16;
        }
        else
        {
            m_resolution[streamIndex] = ((m_userPrvSize.isInvalidSize()) ? HD4K : m_userPrvSize) / 16;
        }
        m_streamId[streamIndex]   = BPSOutputPortDS16;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //5. BPS DS64 out stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatPD10;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize / 64;
        }
        else
        {
            m_resolution[streamIndex] = ((m_userPrvSize.isInvalidSize()) ? HD4K : m_userPrvSize) / 64;
        }
        m_streamId[streamIndex]   = BPSOutputPortDS64;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestBPSGeneric2Stream:
        m_numStreams = 3;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? FULLHD : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        // Defaults to DS4 port, so size should be 1/4th default input size
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatPD10 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? (FULLHD / 4) : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //3. Static FULL output stream
        m_resolution[streamIndex] = FULLHD;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatUBWCTP10;
        m_streamId[streamIndex]   = BPSOutputPortFull;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams  = m_numStreams;
        break;

     case TestChiGPUNode:
        m_numStreams      = 2;
        m_numInputStreams = 1;

        //1. Input stream
        m_resolution[streamIndex]             = VGA;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]               = GPUInputPort;
        m_streamInfo.inputImages[streamIndex] = IPE_COLORBAR_VGA;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. GPU Full out stream
        m_resolution[streamIndex] = VGA;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]   = GPUOutputPortFull;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestTFEGeneric:
        m_numStreams = 1;
        if (CmdLineParser::RemoveSensor())
        {
            m_isCamIFTestGen = true;
        }

        //1. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userPrvFormat;
        // for non-RDI stream/format, use non-RDI size (if given at runtime)
        if ((!IsRDIStream(m_format[streamIndex])) && (!m_userPrvSize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userPrvSize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestTFERDIOutMIPI12:
        m_numStreams = 1;
        if (CmdLineParser::RemoveSensor())
        {
            m_isCamIFTestGen = true;
        }

        //1. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatRawOpaque;
        // for non-RDI stream/format, use non-RDI size (if given at runtime)
        if ((!IsRDIStream(m_format[streamIndex])) && (!m_userPrvSize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userPrvSize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD_1440 : m_userRDISize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestOPEGeneric:
        m_numStreams = 2;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? FULLHD : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        // for RDI stream/format, use RDI size (if given at runtime)
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestInputConfigJpegSwOut:
        m_numStreams = 2;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? HD4K : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userInputFormat;
        m_streamId[streamIndex]               = JPEGSWInputPortMain;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? IPE_EMULATEDSENSOR_4K : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Output stream
        m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? HD4K : m_userPrvSize;
        if(m_userPrvSize != m_userInputSize)
        {
            NT_LOG_UNSUPP("Output Resolution must match input resolution");
            return CDKResultEInvalidArg;
        }
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatBlob;
        m_streamId[streamIndex]   = SNAPSHOT;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestTFEGeneric2Stream:
        m_numStreams = 2;

        //1. Configurable output stream
        m_direction[streamIndex] = ChiStreamTypeOutput;
        m_format[streamIndex]    = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userPrvFormat;
        if ((!IsRDIStream(m_format[streamIndex])) && (!m_userPrvSize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userPrvSize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = true;
        streamIndex++;

        //2. Static output stream
        m_resolution[streamIndex] = FULLHD;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatRaw16;
        m_streamId[streamIndex]   = TFEOutputPortFull;
        m_isRealtime[streamIndex] = true;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestOPEGeneric2StreamDisplay:

        m_numStreams = 3;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? FULLHD : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatYCbCr420_888 : m_userPrvFormat;
        if (IsRDIStream(m_format[streamIndex]) && (!m_userRDISize.isInvalidSize()))
        {
            m_resolution[streamIndex] = m_userRDISize;
        }
        else
        {
            m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? FULLHD : m_userPrvSize;
        }
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //3. Static DISPLAY output stream
        m_resolution[streamIndex] = VGA;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]   = OPEOutputPortVideo;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams  = m_numStreams;
        break;

    case TestOPEGeneric3StreamDisplayStats:

        m_numStreams = 4;
        m_numInputStreams = 1;

        //1. Configurable input stream
        m_resolution[streamIndex]             = (m_userInputSize.isInvalidSize()) ? FULLHD : m_userInputSize;
        m_direction[streamIndex]              = ChiStreamTypeInput;
        m_format[streamIndex]                 = (m_userInputFormat == noFormatOverride) ? ChiStreamFormatRaw16 : m_userInputFormat;
        m_streamId[streamIndex]               = CONFIG_INPUT;
        m_streamInfo.inputImages[streamIndex] = (strlen(inputFileName) == 0) ? BPS_COLORBAR_FHD : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Configurable output stream
        m_resolution[streamIndex] = Size(32768, 1);
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = (m_userPrvFormat == noFormatOverride) ? ChiStreamFormatBlob : m_userPrvFormat;
        m_streamId[streamIndex]   = CONFIG_OUTPUT;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //3. Static VIDEO output stream
        m_resolution[streamIndex] = FULLHD;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]   = OPEOutputPortVideo;
        m_isRealtime[streamIndex] = false;
        streamIndex++;

        //4. Static DISPLAY output stream
        m_resolution[streamIndex] = VGA;
        m_direction[streamIndex]  = ChiStreamTypeOutput;
        m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
        m_streamId[streamIndex]   = OPEOutputPortDisplay;
        m_isRealtime[streamIndex] = false;

        m_streamInfo.num_streams = m_numStreams;
        break;

    case TestCVPDenseMotion:
        m_numStreams = 2;
        m_numInputStreams = 2;

        //1. Input stream
        m_resolution[streamIndex]              = (m_userInputSize.isInvalidSize()) ? Size1008_568 : m_userInputSize;
        m_direction[streamIndex]               = ChiStreamTypeInput;
        m_format[streamIndex]                  = ChiStreamFormatPD10;
        m_streamId[streamIndex]                = CVPInputPortTARIFEDS4;
        m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? CVP_INPUT_DS4 : inputFileName;
        m_isRealtime[streamIndex]             = false;
        streamIndex++;

        //2. Input stream (same as above stream except port and delayed input)
        m_resolution[streamIndex]              = (m_userInputSize.isInvalidSize()) ? Size1008_568 : m_userInputSize;
        m_direction[streamIndex]               = ChiStreamTypeInput;
        m_format[streamIndex]                  = ChiStreamFormatPD10;
        m_streamId[streamIndex]                = CVPInputPortIFEDS4DELAY;
        m_streamInfo.delayedFrame[streamIndex] = 1;
        m_streamInfo.inputImages[streamIndex]  = (strlen(inputFileName) == 0) ? CVP_INPUT_DS4 : inputFileName;
        m_isRealtime[streamIndex]             = false;

        m_streamInfo.num_streams = m_numStreams;
        break;

    default:
        NT_LOG_ERROR("Unknown testcase Id provided: [%d]", m_testId);
        result = CDKResultENoSuch;
        break;
    }

    // Log the user input image name
    for (int streamIdx = 0; streamIndex < m_numStreams && m_direction[streamIdx] == ChiStreamTypeInput; streamIdx++) {
        NT_LOG_DEBUG("Using input image: %s for stream: %d", m_streamInfo.inputImages[streamIdx], streamIdx);
    }

    if (result == CDKResultSuccess)
    {
        m_pRequiredStreams = SAFE_NEW() CHISTREAM[m_streamInfo.num_streams];
        camera3_stream_t* pCamStreams = CreateStreams(m_streamInfo);
        if (nullptr == pCamStreams)
        {
            NT_LOG_ERROR("Failed to create pCamStreams from m_streamInfo!");
            result = CDKResultEFailed;
        }
        else if (nullptr == m_pRequiredStreams) {
            NT_LOG_ERROR("Failed to initialize CHI streams");
            result = CDKResultEFailed;
        }
        else
        {
            ConvertCamera3StreamToChiStream(pCamStreams, m_pRequiredStreams, m_streamInfo.num_streams);
            delete[] pCamStreams;   // Clean up after converting to m_pRequiredStreams

            if (nullptr == m_pRequiredStreams)
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

    return result;
}

/***************************************************************************************************************************
*   RecipeTest::SetupPipelines()
*
*   @brief
*       Overridden function implementation which creates pipeline objects based on the test Id
*   @param
*       [in]  int                 cameraId    cameraId to be associated with the pipeline
*       [in]  ChiSensorModeInfo*  sensorMode  sensor mode to be used for the pipeline
*   @return
*       CDKResult success if pipelines could be created or failure
***************************************************************************************************************************/
CDKResult RecipeTest::SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode)
{
    CDKResult result = CDKResultSuccess;

    switch (m_testId)
    {
    case TestIFEGeneric:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEGeneric);
        break;
    case TestIFEHDR:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEHDR);
        break;
    case TestIFEGenericOffline:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIFEGeneric);
        break;
    case TestIFEDownscaleGeneric:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEDownscaleGeneric);
        break;
    case TestIPEGeneric:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEGeneric);
        break;
    case TestJPEGGeneric:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineJpegSnapshot);
        break;
    case TestBPSGeneric:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSGeneric);
        break;
    case TestBPSGeneric2Stream:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSGeneric2Stream);
        break;
    case TestIFELiteTLBGStatsOut:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFELiteTLBGStatsOut);
        break;
    case TestIFELiteRawTLBGStatsOut:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFELiteRawTLBGStatsOut);
        break;
    case TestIFEGeneric2Stream:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEGeneric2Stream);
        break;
    case TestIFEGeneric2StreamDisplay:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEGeneric2StreamDisplay);
        break;
    case TestBPSDownscaleGeneric:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineBPSDownscaleGeneric);
        break;
    case TestIPEGeneric2StreamDisplay:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineIPEGeneric2StreamDisplay);
        break;
     case TestChiGPUNode:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineChiGPUNode);
        break;
    case TestTFEGeneric:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFEGeneric);
        break;
    case TestOPEGeneric:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEGeneric);
        break;
    case TestTFERDIOutMIPI12:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFERDIOutMIPI12);
        break;
    case TestInputConfigJpegSwOut:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineJPEGSWSnapshot);
        break;
    case TestTFEGeneric2Stream:
        m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFEGeneric2Stream);
        break;
    case TestOPEGeneric2StreamDisplay:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEGeneric2StreamDisplay);
        break;
    case TestOPEGeneric3StreamDisplayStats:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineOPEGeneric3StreamDisplayStats);
        break;
    case TestCVPDenseMotion:
        m_pChiPipeline[Offline] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::OfflineCVPStandAlone);
        break;
    default:
        NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
        return CDKResultENoSuch;
    }

    switch (m_pipelineConfig)
    {
    case RealtimeConfig:
        result = (NULL != m_pChiPipeline[Realtime]) ? m_pChiPipeline[Realtime]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        break;
    case OfflineConfig:
        result = (NULL != m_pChiPipeline[Offline]) ? m_pChiPipeline[Offline]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        break;
    case MixedConfig:
        result = (NULL != m_pChiPipeline[Realtime]) ? m_pChiPipeline[Realtime]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        result |= (NULL != m_pChiPipeline[Offline]) ? m_pChiPipeline[Offline]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        break;
    default:
        NT_LOG_ERROR("Invalid PipelineConfig provided! %d", m_pipelineConfig);
        break;
    }

    return result;
}

/***************************************************************************************************************************
*   RecipeTest::CreateSessions()
*
*   @brief
*       Overridden function implementation which creates required number of sessions based on the test Id
*   @param
*       None
*   @return
*       CDKResult success if sessions could be created or failure
***************************************************************************************************************************/
CDKResult RecipeTest::CreateSessions()
{
    CDKResult result = CDKResultSuccess;

    switch (m_pipelineConfig)
    {
    case RealtimeConfig:
        result = CreateSessionHelper(RealtimeSession);
        break;
    case OfflineConfig:
        result = CreateSessionHelper(OfflineSession);
        break;
    case MixedConfig:
        result = CreateSessionHelper(RealtimeSession);
        result |= CreateSessionHelper(OfflineSession);
        break;
    default:
        NT_LOG_ERROR("Invalid PipelineConfig provided! %d", m_pipelineConfig);
        break;
    }

    return result;
}

/***************************************************************************************************************************
*   RecipeTest::CreateSessionHelper()
*
*   @brief
*       Helper function to create a single session, using the session Id given
*   @param
*       [in]    SessionId   sessionId   session Id to create session for
*   @return
*       CDKResult success if session could be created or failure
***************************************************************************************************************************/
CDKResult RecipeTest::CreateSessionHelper(SessionId sessionId)
{
    CDKResult result = CDKResultSuccess;
    ChiCallBacks callbacks = {};
    PipelineIndex pipeIndex = (sessionId == RealtimeSession) ? Realtime : Offline;

    callbacks.ChiNotify = ChiTestCase::ProcessMessage;
    callbacks.ChiProcessCaptureResult = ChiTestCase::QueueCaptureResult;
    callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

    mPerSessionPvtData[sessionId].pTestInstance = this;
    mPerSessionPvtData[sessionId].sessionId     = sessionId;
    mPerSessionPvtData[sessionId].cameraId      = m_currentTestCameraId;

    mPerSessionPvtData[sessionId].pChiSession   = ChiSession::Create(&m_pChiPipeline[pipeIndex], 1, &callbacks,
                                                    &mPerSessionPvtData[sessionId]);
    mPerSessionPvtData[sessionId].sessionHandle = mPerSessionPvtData[sessionId].pChiSession->GetSessionHandle();

    if (nullptr == mPerSessionPvtData[sessionId].pChiSession)
    {
        NT_LOG_ERROR("%s session could not be created", (sessionId == RealtimeSession) ? "Realtime" : "Offline");
        result = CDKResultEFailed;
    }
    else
    {
        if(!m_nEarlyPCR) // if early PCR not enabled, activate pipeline here
        {
            result = m_pChiPipeline[pipeIndex]->ActivatePipeline(mPerSessionPvtData[sessionId].sessionHandle);
        }

        if (result == CDKResultSuccess)
        {
            result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                mPerSessionPvtData[sessionId].sessionHandle, m_pChiPipeline[pipeIndex]->GetPipelineHandle(),
                &m_pPipelineMetadataInfo[pipeIndex]);
        }
    }

    return result;
}

/***************************************************************************************************************************
*   RecipeTest::DestroyResources()
*
*   @brief
*       Overridden function implementation which destroys all created resources / objects
*   @param
*       None
*   @return
*       None
***************************************************************************************************************************/
void RecipeTest::DestroyResources()
{
    switch (m_pipelineConfig)
    {
    case RealtimeConfig:
        DestroyResourcesHelper(RealtimeSession);
        break;
    case OfflineConfig:
        DestroyResourcesHelper(OfflineSession);
        break;
    case MixedConfig:
        DestroyResourcesHelper(RealtimeSession);
        DestroyResourcesHelper(OfflineSession);
        break;
    default:
        NT_LOG_ERROR("Invalid PipelineConfig provided! %d", m_pipelineConfig);
        break;
    }

    DestroyCommonResources();
}

/***************************************************************************************************************************
*   RecipeTest::DestroyResourcesHelper()
*
*   @brief
*       Helper for DestroyResources to destroy resources depending on session Id
*   @param
*       [in]    SessionId   sessionId   session Id to destoy resources for
*   @return
*       None
***************************************************************************************************************************/
void RecipeTest::DestroyResourcesHelper(SessionId sessionId)
{
    PipelineIndex pipeIndex = (sessionId == RealtimeSession) ? Realtime : Offline;

    if ((nullptr != mPerSessionPvtData[sessionId].pChiSession) && (nullptr != m_pChiPipeline[pipeIndex]))
    {
        m_pChiPipeline[pipeIndex]->DeactivatePipeline(mPerSessionPvtData[sessionId].sessionHandle);
    }

    if (nullptr != mPerSessionPvtData[sessionId].pChiSession)
    {
        mPerSessionPvtData[sessionId].pChiSession->DestroySession();
        mPerSessionPvtData[sessionId].pChiSession = nullptr;
    }

    if (nullptr != m_pChiPipeline[pipeIndex])
    {
        m_pChiPipeline[pipeIndex]->DestroyPipeline();
        m_pChiPipeline[pipeIndex] = nullptr;
    }
}

} // namespace chitests
