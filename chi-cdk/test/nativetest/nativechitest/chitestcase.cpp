////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chitestcase.cpp
/// @brief Definitions for the chitestcase manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "chitestcase.h"
#include "chibuffermanager.h"
#include <fstream>

namespace chitests
{
// Initialize static variables
UINT32                                          ChiTestCase::m_errorNotifyList[MAX_REQUESTS];
std::queue<ChiCaptureResult*>                   ChiTestCase::m_pResultsToProcess;
std::queue<ChiTestCase::SessionPrivateData*>    ChiTestCase::m_pPrivateDataToProcess;
Mutex*                                          ChiTestCase::m_pResultQueueMutex;
Condition*                                      ChiTestCase::m_pBufferCountCond;

ChiTestCase::ProcessSignal                      ChiTestCase::m_processingSignaler;
Mutex*                                          ChiTestCase::m_pProcessingMutex;
Condition*                                      ChiTestCase::m_pProcessingCond;

std::map<ChiStream*, std::vector<uint32_t>>     ChiTestCase::m_bufferErrorStreamMap;
bool                                            ChiTestCase::ms_bEnableNewImageRead = false;
bool                                            ChiTestCase::sbIsCameraIDSessionID = false;
CHICONTEXTOPS*                                  ChiTestCase::spChiOps;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Setup
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::Setup()
{
    NT_LOG_INFO("Test ID is %d", m_testId);

    m_captureCount      = CmdLineParser::GetFrameCount();
    m_userInputFormat   = static_cast<CHISTREAMFORMAT>(CmdLineParser::GetInputFormat());
    m_userPrvFormat     = static_cast<CHISTREAMFORMAT>(CmdLineParser::GetOutputFormat());
    m_userVidFormat     = static_cast<CHISTREAMFORMAT>(CmdLineParser::GetOutFormatVideo());
    m_userSnapFormat    = static_cast<CHISTREAMFORMAT>(CmdLineParser::GetOutFormatSnapshot());
    m_userInputSize     = Size(CmdLineParser::GetInputWidth(), CmdLineParser::GetInputHeight());
    m_userPrvSize       = Size(CmdLineParser::GetOutputWidth(), CmdLineParser::GetOutputHeight());
    m_userRDISize       = Size(CmdLineParser::GetRdiOutputWidth(), CmdLineParser::GetRdiOutputHeight());
    m_userVidSize       = Size(CmdLineParser::GetVideoWidth(), CmdLineParser::GetVideoHeight());
    m_userSnapSize      = Size(CmdLineParser::GetSnapshotWidth(), CmdLineParser::GetSnapshotHeight());
    m_userDispSize      = Size(CmdLineParser::GetDisplayPortOrDualFoVWidth(), CmdLineParser::GetDisplayPortOrDualFoVHeight());
    m_flushFrame        = CmdLineParser::GetFlushFrame();
    m_nEarlyPCR         = CmdLineParser::GetEarlyPCRCount();
    NT_ASSERT(m_nEarlyPCR < m_captureCount,"Early capture request count exceeds total capture requests");

    //Check for snaps requested in NZSL case
    if(!CmdLineParser::IsZSLEnabled()){
        NT_ASSERT(CmdLineParser::GetSnapCount() <= static_cast<UINT32>(ceil(float(m_captureCount)/2)),"Cannot request more than %d snaps with %d frames",static_cast<UINT32>(ceil(float(m_captureCount)/2)),m_captureCount);
    }

    int dirCheck = mkdir("/vendor/etc/camera", 0777);

    NT_LOG_INFO("make dir return code: %d", dirCheck);

    NT_ASSERT(CDKResultSuccess == EnableOverrides(), "Unable to set overrides required for the test");

    // Setup mutexes, conditions, and threads first in case emergency Teardown happens
    m_pBufferCountMutex = Mutex::Create("m_pBufferCountMutex");
    m_pBufferCountCond  = Condition::Create("m_pBufferCountCond");
    m_pResultQueueMutex = Mutex::Create("m_pResultQueueMutex");
    m_pProcessingMutex  = Mutex::Create("m_pProcessingMutex");
    m_pProcessingCond   = Condition::Create("m_pProcessingCond");

    if (m_flushFrame || (CmdLineParser::GetTestSuiteName() == "FlushTest"))
    {
        if (m_flushFrame > CmdLineParser::GetFrameCount())
        {
            NT_FAIL("Invalid input for flush frame number");
        }

        /* Create mutex and condition variables for Flush */
        m_pFlushSignalMutex = Mutex::Create("m_pFlushSignalMutex");
        m_pFlushSignalCond = Condition::Create("m_pFlushSignalCond");
        CDKResult result = OsUtils::ThreadCreate(WaitForFlushSignal, this, &m_flushThread);
        NT_ASSERT(CDKResultSuccess == result, "Failed to create flush thread!");
        m_usFlushDelay = (CmdLineParser::GetFlushDelay() >= 0) ? CmdLineParser::GetFlushDelay() : 0;
    }

    // Setup the long running thread in charge of processing capture results
    CDKResult result = OsUtils::ThreadCreate(ProcessCaptureResults, this, &m_processingThread);
    NT_ASSERT(CDKResultSuccess == result, "Failed to create processing thread!");

    m_pChiModule = ChiModule::GetInstance();
    NT_ASSERT(m_pChiModule != nullptr, " ChiModule could not be instantiated");

    ChiTestCase::spChiOps = const_cast<CHICONTEXTOPS*>(m_pChiModule->GetChiOps());
    NT_ASSERT(ChiTestCase::spChiOps != nullptr, "ChiContextOps not initialized");

    // Setup buffer manager library
    NT_EXPECT(0 == ChiBufferManager::LoadBufferLibs(m_pChiModule->GetLibrary()), "Failed to load symbols for buffer manager!");

    //Check for custom RDI ask on Sensor testcase
    if (!CmdLineParser::isTestGenMode() && !CmdLineParser::RemoveSensor()){
        NT_ASSERT(m_userRDISize.isInvalidSize(), "RDI output width and height parameters not allowed for Sensor testcase");
    }

    m_streamInfo.streamIds = m_streamId;
    //TODO: Let common framework do the typecasting
    m_streamInfo.directions  = reinterpret_cast<camera3_stream_type_t*>(m_direction);
    m_streamInfo.formats     = reinterpret_cast<android_pixel_format_t*>(m_format);
    m_streamInfo.resolutions = m_resolution;
    m_streamInfo.usageFlags  = m_usageFlag;
    m_streamInfo.isRealtime  = m_isRealtime;
    m_streamInfo.isJpeg      = true;

    m_numStreams       = 0;
    m_numInputStreams  = 0;
    m_isCamIFTestGen   = false;
    m_buffersRemaining = 0;
    mLastFrameReceived = 0;

    m_streamBufferMap.clear();
    m_streamIdMap.clear();

    for (uint32_t i = 0; i < NATIVETEST_ELEMENTS(m_errorNotifyList); i++)
    {
        ClearErrorNotifyCode(i);
    }

    // Setup metadata util instance
    m_pChiMetadataUtil = ChiMetadataUtil::GetInstance();
    NT_ASSERT(m_pChiMetadataUtil != nullptr, " ChiMetadataUtil could not be instantiated");

    m_socId = ChiPipeline::GetSocID();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Teardown
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::Teardown()
{
    NT_LOG_INFO("Destroying Resources used for the test");
    DestroyResources();

    if (m_pProcessingMutex != nullptr) {
        // Shut down process capture result thread
        SignalProcessingThread(SignalDone);
        OsUtils::ThreadWait(m_processingThread);
    }

    if (m_flushFrame)
    {
        /*Destroy flush resources*/
        SignalFlushThread(SignalFlushDone);
        OsUtils::ThreadWait(m_flushThread);

        // Clean up mutex/condition variables
        m_pFlushSignalMutex->Destroy();
        m_pFlushSignalCond->Destroy();
        m_pFlushSignalMutex = NULL;
        m_pFlushSignalCond = NULL;
    }
    // Do not destroy mutex/condition variables in DestroyCommonResources(),
    // as that function is called multiple times per testcase (and these variables are only created once)
    if (nullptr != m_pBufferCountMutex)
    {
        m_pBufferCountMutex->Destroy();
        m_pBufferCountMutex = nullptr;
    }
    if (nullptr != m_pBufferCountCond)
    {
        m_pBufferCountCond->Destroy();
        m_pBufferCountCond = nullptr;
    }
    if (nullptr != m_pResultQueueMutex)
    {
        m_pResultQueueMutex->Destroy();
        m_pResultQueueMutex = nullptr;
    }
    if (nullptr != m_pProcessingMutex)
    {
        m_pProcessingMutex->Destroy();
        m_pProcessingMutex = nullptr;
    }
    if (nullptr != m_pProcessingCond)
    {
        m_pProcessingCond->Destroy();
        m_pProcessingCond = nullptr;
    }

    DestroyCommonResources();
    ChiModule::DestroyInstance();
    m_pChiModule = nullptr;

    NT_LOG_INFO("Restoring initial override settings present before the test")
    NT_ASSERT(CDKResultSuccess == WriteCamXORMapToFile(m_camXORMap), "Unable to reset camxoverride settings after the test");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// QueueCaptureResult
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::QueueCaptureResult(CHICAPTURERESULT* pResult, void* pPrivateCallbackData)
{
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);

    /* START critical section */
    m_pResultQueueMutex->Lock();

    // Copy the result (since pResult is volatile, needs to be in critical section)
    ChiCaptureResult* pSavedResult = NULL;
    pSavedResult = DeepCopyCaptureResult(pResult);
    if (NULL == pSavedResult)
    {
        m_pResultQueueMutex->Unlock();
        NT_ASSERT(NULL != pSavedResult, "Failed to copy ChiCaptureResult!");
    }


    // Copy the private data
    SessionPrivateData* pSavedPrivate = SAFE_NEW() SessionPrivateData();
    memcpy(pSavedPrivate, pSessionPrivateData, sizeof(SessionPrivateData));

    // Push copied data on queue
    m_pResultsToProcess.push(pSavedResult);
    m_pPrivateDataToProcess.push(pSavedPrivate);
    //NT_LOG_DEBUG( "Queuing result to process for frame %d", pSavedResult->frameworkFrameNum);
    //NT_LOG_DEBUG( "Number of results to process: %d", m_pResultsToProcess.size());

    m_pResultQueueMutex->Unlock();
    /* END critical section */

    // Signal the processing thread to process the queued capture result
    SignalProcessingThread(SignalProcess);
}

/***************************************************************************************************************************
*   ChiTestCase::ProcessCaptureResults
*
*   @brief
*       Process queued up capture results, dispatching to the correct instance of the testcase.
*       This is designed to be run in a separate worker thread from the main thread.
*   @param
*       [in]    VOID*   pArg    pointer to a ChiTestCase instance
*   @return
*       VOID* not used
***************************************************************************************************************************/
VOID* ChiTestCase::ProcessCaptureResults(VOID* pArg)
{
    bool isDone = false;
    ChiTestCase* testInstance = NULL;
    ChiCaptureResult* pCurrResult = NULL;
    SessionPrivateData* pCurrPrivateData = NULL;

    // Get the ChiTestCase instance
    if (NULL != pArg)
    {
        testInstance = reinterpret_cast<ChiTestCase*>(pArg);
    }
    else
    {
        NT_EXPECT(NULL != pArg, "No ChiTestCase instance provided!");
        return NULL;
    }

    /* START critical section */
    testInstance->m_pProcessingMutex->Lock();

    while (!isDone)
    {
        // Wait for next signal
        testInstance->m_pProcessingCond->Wait(testInstance->m_pProcessingMutex->GetNativeHandle());

        switch (testInstance->m_processingSignaler)
        {
        case SignalReady:
            // Do nothing
            NT_LOG_DEBUG( "Process thread ready...");
            break;
        case SignalProcess:
            // Process all queued results
            while (true)
            {
                // Thread-safe queue operations
                m_pResultQueueMutex->Lock();
                if (!m_pResultsToProcess.empty() && !m_pPrivateDataToProcess.empty())
                {
                    // Get pointers and remove from queue
                    pCurrResult      = m_pResultsToProcess.front();
                    pCurrPrivateData = m_pPrivateDataToProcess.front();
                    m_pResultsToProcess.pop();
                    m_pPrivateDataToProcess.pop();

                    // Verify pointers are valid
                    if (NULL != pCurrResult && NULL != pCurrPrivateData)
                    {
                        // Reset wait retry count, so we don't time out
                        testInstance->ResetWaitForResultsRetryCount();
                    }
                    else
                    {
                        NT_LOG_ERROR( "Found invalid CaptureResult/PrivateData pair! (at least one is NULL)");
                        m_pResultQueueMutex->Unlock();
                        break;
                    }
                }
                else  // No more results to process
                {
                    m_pResultQueueMutex->Unlock();
                    break;
                }
                m_pResultQueueMutex->Unlock();

                pCurrPrivateData->pTestInstance->CommonProcessCaptureResult(pCurrResult, pCurrPrivateData);

                // Clean up pointers after processing complete
                DeepDestroyCaptureResult(pCurrResult);
                if (NULL != pCurrPrivateData)
                {
                    delete pCurrPrivateData;
                }
            }

            // Notify main thread that results have been processed
            m_pBufferCountCond->Signal();

            break;
        case SignalDone:
            // Ready to exit
            NT_LOG_DEBUG( "Process thread done...");
            isDone = true;
            break;
        default:
            NT_LOG_ERROR( "Invalid ProcessSignal! (%d)", testInstance->m_processingSignaler);
            break;
        }

        // Notify main thread that process thread is ready for next command
        testInstance->m_processingSignaler = SignalReady;
        testInstance->m_pProcessingCond->Broadcast();
    }

    testInstance->m_pProcessingMutex->Unlock();
    /* END critical section */

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// QueuePartialCaptureResult
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::QueuePartialCaptureResult(CHIPARTIALCAPTURERESULT* pPartialResult, void* pPrivateCallbackData)
{
    NATIVETEST_UNUSED_PARAM(pPartialResult);
    NATIVETEST_UNUSED_PARAM(pPrivateCallbackData);
    // TODO TDEV-2292
}

/**************************************************************************************************************************
*   ChiTestCase::SignalProcessingThread()
*
*   @brief
*       Changes the signaler and then notifies the process thread
*   @param
*       [in]    ProcessSignal     signal      command to direct the flush thread
*   @return
*       void
**************************************************************************************************************************/
void ChiTestCase::SignalProcessingThread(ProcessSignal signal)
{
    /* START critical section */
    m_pProcessingMutex->Lock();

    // Wait for thread to be ready
    while (SignalReady != m_processingSignaler)
    {
        m_pProcessingCond->Wait(m_pProcessingMutex->GetNativeHandle());
    }

    // Set the signal
    m_processingSignaler = signal;

    // Notify the thread
    const char* processSignalStrings[3] = { "SignalReady", "SignalProcess", "SignalDone" };
    NT_LOG_DEBUG( "Signaling process thread: %s", processSignalStrings[signal]);
    m_pProcessingCond->Broadcast();

    m_pProcessingMutex->Unlock();
    /* END critical section */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ProcessMessage
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::ProcessMessage(const CHIMESSAGEDESCRIPTOR* pMessageDescriptor, void* pPrivateCallbackData)
{
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);

    NT_LOG_INFO( "Received notify from driver");

    switch (pMessageDescriptor->messageType)
    {
    case CHIMESSAGETYPE::ChiMessageTypeError:
    {
        CHIERRORMESSAGE errMessage = pMessageDescriptor->message.errorMessage;
        NT_LOG_ERROR( "Notify ERROR message when processing frame %d, stream %p",
            errMessage.frameworkFrameNum, errMessage.pErrorStream);

        switch (errMessage.errorMessageCode)
        {
        case CHIERRORMESSAGECODE::MessageCodeDevice:
            SetErrorNotifyCode(errMessage.frameworkFrameNum, MessageCodeDevice);
            NT_LOG_ERROR( "Notify ERROR [DEVICE]: serious failure occurred and no further frames will be"
                " produced by the device");
            //TODO:Stop sending requests for this type of error
            break;
        case CHIERRORMESSAGECODE::MessageCodeRequest:
            SetErrorNotifyCode(errMessage.frameworkFrameNum, MessageCodeRequest);
            NT_LOG_ERROR( "Notify ERROR [REQUEST]: error has occurred in processing a request and no output"
                " will be produced for this request");
            break;
        case CHIERRORMESSAGECODE::MessageCodeResult:
            SetErrorNotifyCode(errMessage.frameworkFrameNum, MessageCodeResult);
            NT_LOG_ERROR( "Notify ERROR [RESULT]: error has occurred in producing an output result metadata"
                " buffer for a request");
            break;
        case CHIERRORMESSAGECODE::MessageCodeBuffer:
            SetErrorNotifyCode(errMessage.frameworkFrameNum, MessageCodeBuffer, errMessage.pErrorStream);
            NT_LOG_ERROR( "Notify ERROR [BUFFER]: error has occurred in placing an output buffer into a"
                " stream for a request");
            break;
        case CHIERRORMESSAGECODE::MessageCodeTriggerRecovery:
            SetErrorNotifyCode(errMessage.frameworkFrameNum, MessageCodeTriggerRecovery);
            NT_LOG_ERROR( "Notify ERROR [RECOVERY]: error has occurred and we need to trigger recovery");
            break;
        default:
            NT_LOG_ERROR( "Notify ERROR [UNKNOWN]: unknown error message code %d",
                errMessage.errorMessageCode);
            break;
        }
        break;
    }
    case CHIMESSAGETYPE::ChiMessageTypeShutter:
    {
        CHISHUTTERMESSAGE shutterMessage = pMessageDescriptor->message.shutterMessage;
        NT_LOG_INFO( "Notify shutter message frame: %d, timestamp: %llu",
            shutterMessage.frameworkFrameNum, shutterMessage.timestamp);
        break;
    }
    case CHIMESSAGETYPE::ChiMessageTypeSof:
        NT_LOG_INFO( "Start of frame event triggered");
        break;
    case CHIMESSAGETYPE::ChiMessageTypeMetaBufferDone:
        NT_LOG_INFO( "Metabuffer is done processing");
        break;
        // This message type was removed in a later version of chi.h
        //case CHIMESSAGETYPE::ChiMessageTypePartialMetaBufferDone:
        //    NT_LOG_INFO( "Partial metabuffer is available");
        //    break;
    default:
        NT_LOG_ERROR( "Unknown message type %d", pMessageDescriptor->messageType);
        break;
    }
}

/***************************************************************************************************************************
*   ChiTestCase::WaitForResults
*
*   @brief
*       Common waiting condition for all testcases, Once the test sends required number of requests this waiting condition
*       is invoked to wait for all results to be returned back by driver
*   @param
*        None
*   @return
*       CDKResult success if all results returned by driver/ fail if buffers were not returned within buffer timeout
***************************************************************************************************************************/
CDKResult ChiTestCase::WaitForResults()
{
    m_pBufferCountMutex->Lock();
    CDKResult result = CDKResultSuccess;
    int timeout = CmdLineParser::GetTimeoutOverride();

    if (timeout <= 0) // no timeout
    {
        while (m_buffersRemaining.load() != 0)
        {
            // Wait forever for buffer(s)
            m_pBufferCountCond->Wait(m_pBufferCountMutex->GetNativeHandle());
        }
        m_pBufferCountMutex->Unlock();
    }
    else
    {
        m_waitForResultsRetries = 0;
        while (m_buffersRemaining.load() != 0 && m_waitForResultsRetries < TIMEOUT_RETRIES)
        {
            NT_LOG_DEBUG( "Waiting for buffers to be returned, pending buffer count: %d", m_buffersRemaining.load());
            m_pBufferCountCond->TimedWait(m_pBufferCountMutex->GetNativeHandle(), timeout);
            m_waitForResultsRetries++;
        }
        m_pBufferCountMutex->Unlock();

        if (m_buffersRemaining.load() != 0)
        {
            NT_LOG_ERROR( "Buffer wait condition timed out");
            result = CDKResultETimeout;
        }
    }

    return result;
}

/***************************************************************************************************************************
*   ChiTestCase::InitializeBufferManagers
*
*   @brief
*       Initializes buffer manager for each required stream
*   @param
*       [in]    int cameraId    camera to initialize buffers for (used in file naming)
*   @return
*       CDKResult success if buffer manager could be created / failure
***************************************************************************************************************************/
CDKResult ChiTestCase::InitializeBufferManagers(int cameraId)
{
    CDKResult result = CDKResultSuccess;

    if (m_numStreams <= 0)
    {
        NT_LOG_ERROR( "Cannot create buffermanagers with stream count: %d", m_numStreams);
        result = CDKResultENeedMore;
    }

    if (result == CDKResultSuccess)
    {
        int inStreamIdx = -1;
        for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
        {
            ChiBufferManager::NativeChiStream* currentStream = &m_pRequiredStreams[streamIndex];
            ChiBufferManager::GenericStream genericStream(currentStream);

            const char* pInputImage = nullptr;
            pInputImage = m_streamInfo.inputImages[streamIndex];

            ChiBufferManager* manager = SAFE_NEW() ChiBufferManager(m_streamId[streamIndex]);
            if (m_direction[streamIndex] == ChiStreamTypeInput)
            {
                inStreamIdx++;
                m_pBufferManagers[streamIndex] = manager->Initialize(cameraId, &genericStream,
                    m_streamInfo.streamIds[streamIndex], pInputImage, inStreamIdx, m_streamInfo.delayedFrame[streamIndex]);
            }
            else
            {
                m_pBufferManagers[streamIndex] = manager->Initialize(cameraId, &genericStream,
                    m_streamInfo.streamIds[streamIndex], pInputImage);
            }

            if (m_pBufferManagers[streamIndex] == nullptr)
            {
                NT_LOG_ERROR( "Failed to allocate memory for buffer manager for stream index: %d", streamIndex);
                result = CDKResultENoMemory;
                break;
            }
            m_streamBufferMap[currentStream] = m_pBufferManagers[streamIndex];
        }
    }
    return result;
}

/**************************************************************************************************
*   ChiTestCase::IsRDIStream
*
*   @brief
*       Helper function to determine if stream is RDI
*   @param
*       [in] CHISTREAMFORMAT    format    Stream format
*   @return
*       bool indicating whether the stream is RDI
**************************************************************************************************/
bool ChiTestCase::IsRDIStream(CHISTREAMFORMAT format)
{
    return (format == ChiStreamFormatRawOpaque ||
            format == ChiStreamFormatRaw10     ||
            format == ChiStreamFormatRaw12     ||
            format == ChiStreamFormatRaw16     ||
            format == ChiStreamFormatRaw64     );
}

/**************************************************************************************************
*   ChiTestCase::IsSensorModeMaxResRawUnsupportedTFEPort
*
*   @brief
*       Helper function to determine if sensor mode max resolution raw format is supported at
*       provided TFE port
*   @param
*       [in] StreamUsecases    tfePort    TFE Output Port
*   @return
*       bool indicating whether supported or not
**************************************************************************************************/
bool ChiTestCase::IsSensorModeMaxResRawUnsupportedTFEPort(StreamUsecases tfePort)
{
    return (tfePort == StreamUsecases::TFEOutputPortFull);
}

/***************************************************************************************************************************
*   ChiTestCase::DestroyCommonResources
*
*   @brief
*       Destroy member variables present in the base class, i.e., common to all testcases
*   @param
*        None
*   @return
*       None
***************************************************************************************************************************/
void ChiTestCase::DestroyCommonResources()
{
    if (NULL != m_pChiMetadataUtil)
    {
        NT_EXPECT(m_pChiMetadataUtil->DestroyMetabufferPools() == CDKResultSuccess,
            "Failed to destroy metabuffer pools!");
    }

    NT_LOG_DEBUG( "Deleting allocated buffermanagers");
    for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
    {
        if (m_streamBufferMap.find(&m_pRequiredStreams[streamIndex]) != m_streamBufferMap.end())
        {
            ChiBufferManager* manager = m_streamBufferMap.at(&m_pRequiredStreams[streamIndex]);

            if (manager != nullptr)
            {
                manager->DestroyBuffers();
                m_streamBufferMap.erase(&m_pRequiredStreams[streamIndex]);
            }
        }
    }

    NT_EXPECT(m_streamBufferMap.empty(), "There are still pending buffermanagers not freed!");

    if (m_pRequiredStreams != nullptr)
    {
        delete[] m_pRequiredStreams;
        m_pRequiredStreams = nullptr;
    }

    m_streamIdMap.clear();
    m_frameToIpBuffHandleMap.clear();
    m_numStreams = 0;
    m_buffersRemaining = 0;
}

/***************************************************************************************************************************
*   ChiTestCase::AtomicIncrement
*
*   @brief
*       Thread safe increment of buffers remaining
*   @param
*        [in]   int count   increment by count, this is an optional variable by default it will be incremented by 1
*   @return
*       None
***************************************************************************************************************************/
void ChiTestCase::AtomicIncrement(int count)
{
    m_buffersRemaining += count;
}

/***************************************************************************************************************************
*   ChiTestCase::AtomicDecrement
*
*   @brief
*       Thread safe decerement of buffers remaining, if buffers remaining reached zero then it will signal all results
*       obtained
*   @param
*        [in]   int count   decrement by count, this is an optional variable by default it will be decremented by 1
*   @return
*       None
***************************************************************************************************************************/
void ChiTestCase::AtomicDecrement(int count)
{
    m_buffersRemaining -= count;
    if (m_buffersRemaining == 0)
    {
        m_pBufferCountCond->Signal();
    }
}

/***************************************************************************************************************************
*   ChiTestCase::GetPendingBufferCount
*
*   @brief
*       gets the buffer remaining count in a thread safe manner
*   @param
*        None
*   @return
*       uint32_t buffer remaining count
***************************************************************************************************************************/
uint32_t ChiTestCase::GetPendingBufferCount() const
{
    return m_buffersRemaining.load();
}

/***************************************************************************************************************************
*   ChiTestCase::ConvertCamera3StreamToChiStream
*
*   @brief
*       Convert a camera3_stream_t object to a CHISTREAM object
*   @param
*       [in]    camera3_stream_t*   inCamStreams     array of input streams to convert from
*       [out]   CHISTREAM*          outChiStreams    array of output streams to convert to
*       [in]    int                 numStreams      number of streams
*   @return
*       None
***************************************************************************************************************************/
void ChiTestCase::ConvertCamera3StreamToChiStream(camera3_stream_t* inCamStreams, CHISTREAM* outChiStreams, int numStreams)
{
    for (int i = 0; i < numStreams; i++)
    {
        outChiStreams[i].streamType = static_cast<CHISTREAMTYPE>(inCamStreams[i].stream_type);
        outChiStreams[i].width = inCamStreams[i].width;
        outChiStreams[i].height = inCamStreams[i].height;
        outChiStreams[i].format = static_cast<CHISTREAMFORMAT>(inCamStreams[i].format);
        outChiStreams[i].grallocUsage = inCamStreams[i].usage;
        outChiStreams[i].maxNumBuffers = inCamStreams[i].max_buffers;
        outChiStreams[i].pPrivateInfo = inCamStreams[i].priv;
        outChiStreams[i].dataspace = static_cast<CHIDATASPACE>(inCamStreams[i].data_space);
        outChiStreams[i].rotation = static_cast<CHISTREAMROTATION>(inCamStreams[i].rotation);
        outChiStreams[i].pHalStream = NULL;
        outChiStreams[i].streamParams.planeStride = 0;
        outChiStreams[i].streamParams.sliceHeight = 0;
        // since the camx driver does not care about chi stream intent, only the framework
        // cares about it. Also this stream intent does not have any impact how IFE/IPE/BPS
        // processes the frame in createstreams
        // Long term plan is to set this correctly per stream in each test case
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearErrorNotifyCode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::ClearErrorNotifyCode(UINT32 frame)
{
    m_errorNotifyList[frame % MAX_REQUESTS] = 0;
    m_bufferErrorStreamMap.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CheckErrorNotifyCode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ChiTestCase::CheckErrorNotifyCode(UINT32 frame, CHIERRORMESSAGECODE errorCode, CHISTREAM* pCurrentStream)
{
    // In case of buffer error notify, check the map for correct stream
    if (errorCode == CHIERRORMESSAGECODE::MessageCodeBuffer)
    {
        // 1. look for matching stream in the map
        if (m_bufferErrorStreamMap.find(pCurrentStream) != m_bufferErrorStreamMap.end())
        {
            std::vector<uint32_t> frameList = m_bufferErrorStreamMap.find(pCurrentStream)->second;
            // 2. check if given frame had buffer error for given stream
            for (std::vector<uint32_t>::iterator pFrame = frameList.begin(); pFrame != frameList.end(); ++pFrame)
            {
                if (frame == *pFrame)
                {
                    return true;
                }
            }
        }
        // return false if either stream or matching frame not found
        return false;
    }

    // for other notify error types, use the error notify list
    // TODO: remove this list and use the above map for all notify error types
    return m_errorNotifyList[frame % MAX_REQUESTS] == static_cast<UINT32>(errorCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetErrorNotifyCode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::SetErrorNotifyCode(UINT32 frame, CHIERRORMESSAGECODE errorCode, CHISTREAM* pCurrentStream)
{
    // For buffer error notify, track the stream and frame numbers in a map
    if (errorCode == CHIERRORMESSAGECODE::MessageCodeBuffer)
    {
        // create a new entry if not already present
        if (m_bufferErrorStreamMap.find(pCurrentStream) == m_bufferErrorStreamMap.end())
        {
            std::vector<uint32_t> frameList;
            frameList.push_back(frame);
            m_bufferErrorStreamMap.insert(std::make_pair(pCurrentStream, frameList));
        }
    }
    else // For other types of error notify, use the error notify list
    {
        // TODO: remove this list and use the above map for all notify error types
        m_errorNotifyList[frame % MAX_REQUESTS] = static_cast<UINT32>(errorCode);
    }
}

/**************************************************************************************************************************
*   ChiTestCase::DeepCopyCaptureResult
*
*   @brief
*       Deep copies all parts of a ChiCaptureResult, allocating memory as necessary
*   @param
*       [in]    ChiCaptureResult*   pCaptureResult  capture result to deep copy
*   @return
*       ChiCaptureResult* if succeeded or NULL if failed
**************************************************************************************************************************/
ChiCaptureResult* ChiTestCase::DeepCopyCaptureResult(ChiCaptureResult* pCaptureResult)
{
    // Allocate structures
    ChiCaptureResult* pCopiedResult = SAFE_NEW() ChiCaptureResult();
    CHIMETAHANDLE pCopiedInputMetadata = pCaptureResult->pInputMetadata;
    CHIMETAHANDLE pCopiedOutputMetadata = pCaptureResult->pOutputMetadata;

    CHISTREAMBUFFER* pCopiedOutputBuffers = SAFE_NEW() CHISTREAMBUFFER[pCaptureResult->numOutputBuffers];
    CHISTREAMBUFFER* pCopiedInputBuffer = SAFE_NEW() CHISTREAMBUFFER();
    CHIPRIVDATA* pCopiedPrivData = SAFE_NEW() CHIPRIVDATA;

    // Allocate and copy PIP camera variables
#if defined(CAMX_ANDROID_API) && (CAMX_ANDROID_API >= 28) // Android-P or better
    CHAR** ppCopiedPhysCamIds = SAFE_NEW() CHAR*[pCaptureResult->numPhysCamMetadata];
    camera_metadata_t** ppCopiedPhysCamMetadata = SAFE_NEW() camera_metadata_t*[pCaptureResult->numPhysCamMetadata];

    if (NULL == ppCopiedPhysCamIds || NULL == ppCopiedPhysCamMetadata)
    {
        NT_LOG_ERROR( "Failed to allocate memory for ChiCaptureResult deep copy!");
        return NULL;
    }

    for (uint32_t i = 0; i < pCaptureResult->numPhysCamMetadata; i++)
    {
        size_t length = strlen(pCaptureResult->physCamIds[i]);
        ppCopiedPhysCamIds[i] = SAFE_NEW() CHAR[length + 1];
        ppCopiedPhysCamMetadata[i] = clone_camera_metadata(
            reinterpret_cast<const camera_metadata_t*>(pCaptureResult->physCamMetadata[i]));

        if (NULL == ppCopiedPhysCamIds[i] || NULL == ppCopiedPhysCamMetadata[i])
        {
            NT_LOG_ERROR( "Failed to allocate memory for ChiCaptureResult deep copy!");
            return NULL;
        }
        // using temp variables to convert each char* in array to string to avoid banned API strcpy
        std::string tempCopiedPhysCamId(ppCopiedPhysCamIds[i]);
        std::string tempPhysCamId(pCaptureResult->physCamIds[i]);
        tempCopiedPhysCamId = tempPhysCamId; // deep copy
        ppCopiedPhysCamIds[i] = const_cast<CHAR*>(tempCopiedPhysCamId.c_str());
    }
#endif

    // Check allocating succeeded
    if (NULL == pCopiedResult ||
        NULL == pCopiedOutputBuffers ||
        NULL == pCopiedInputBuffer ||
        NULL == pCopiedPrivData)
    {
        NT_LOG_ERROR( "Failed to allocate memory for ChiCaptureResult deep copy!");
        return NULL;
    }

    // Memcpy all structures
    memcpy(pCopiedResult, pCaptureResult, sizeof(ChiCaptureResult));    // Shallow copy of capture result
    memcpy(pCopiedOutputBuffers, pCaptureResult->pOutputBuffers, sizeof(CHISTREAMBUFFER)*pCaptureResult->numOutputBuffers);
    if (NULL != pCaptureResult->pInputBuffer)   // May be NULL
    {
        memcpy(pCopiedInputBuffer, pCaptureResult->pInputBuffer, sizeof(CHISTREAMBUFFER));
    }
    memcpy(pCopiedPrivData, pCaptureResult->pPrivData, sizeof(CHIPRIVDATA));

    // Assemble copied capture result
    pCopiedResult->pInputMetadata = pCopiedInputMetadata;
    pCopiedResult->pOutputMetadata = pCopiedOutputMetadata;
    pCopiedResult->pOutputBuffers = pCopiedOutputBuffers;
    pCopiedResult->pInputBuffer = pCopiedInputBuffer;
#if defined(CAMX_ANDROID_API) && (CAMX_ANDROID_API >= 28) // Android-P or better
    pCopiedResult->physCamIds = const_cast<const CHAR**>(ppCopiedPhysCamIds);
    pCopiedResult->physCamMetadata = const_cast<const VOID**>(reinterpret_cast<VOID**>(ppCopiedPhysCamMetadata));
    pCopiedResult->pPrivData = pCopiedPrivData;
#endif

    return pCopiedResult;
}

/**************************************************************************************************************************
*   ChiTestCase::DeepDestroyCaptureResult
*
*   @brief
*       Destroys all parts of a ChiCaptureResult that was dynamically allocated
*   @param
*       [in]    ChiCaptureResult*   pCaptureResult  capture result to deep destroy
*   @return
*       void
**************************************************************************************************************************/
void ChiTestCase::DeepDestroyCaptureResult(ChiCaptureResult* pCaptureResult)
{
    if (NULL != pCaptureResult)
    {
        // Destroy sub-structures
        if (NULL != pCaptureResult->pOutputBuffers)
        {
            delete[] pCaptureResult->pOutputBuffers;
        }
        if (NULL != pCaptureResult->pInputBuffer)
        {
            delete pCaptureResult->pInputBuffer;
        }
#if defined(CAMX_ANDROID_API) && (CAMX_ANDROID_API >= 28) // Android-P or better
        for (uint32_t i = 0; i < pCaptureResult->numPhysCamMetadata; i++)
        {
            delete pCaptureResult->physCamIds[i];
            free_camera_metadata(const_cast<camera_metadata_t*>(
                reinterpret_cast<const camera_metadata_t*>(pCaptureResult->physCamMetadata[i])));
        }
        delete[] pCaptureResult->physCamIds;
        delete[] pCaptureResult->physCamMetadata;
#endif
        if (NULL != pCaptureResult->pPrivData)
        {
            delete static_cast<CHIPRIVDATA*>(pCaptureResult->pPrivData);
        }

        // Destroy capture result
        delete pCaptureResult;
    }
}

/**************************************************************************************************************************
*   ChiTestCase::ResetWaitForResultsRetryCount
*
*   @brief
*       Resets the retry count when waiting for results, in a thread-safe way. Used when a result comes back during the
*       wait for results period of a test.
*   @param
*       None
*   @return
*       void
**************************************************************************************************************************/
void ChiTestCase::ResetWaitForResultsRetryCount()
{
    m_pBufferCountMutex->Lock();
    m_waitForResultsRetries = 0;
    m_pBufferCountMutex->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryStaticCapabilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::QueryStaticCapabilities(MetadataUsecases usecase, UINT32 cameraID)
{
    CDKResult result = CDKResultSuccess;

    switch (usecase)
    {
        case MetadataUsecases::Testgen:
        {
            const CHAR* sectionName = "org.quic.camera.IFETestGenCapability";
            const CHAR* tagName     = "IFETestGenCapability";
            NT_LOG_INFO("Quering session-level capabilities for %s for camera : %u", tagName, cameraID);

            camera_metadata_entry_t tagEntry;
            if(CDKResultSuccess != m_pChiModule->GetStaticMetadataEntry(cameraID, tagName, sectionName, &tagEntry)
                || tagEntry.count == 0)
            {
                NT_LOG_ERROR("Could not get tag entry for %s", tagName);
                result = CDKResultEFailed;
                break;
            }
            else
            {
                NT_LOG_INFO("Successfully got tag entry for %s", tagName);
                m_pTestGenCaps = reinterpret_cast<CHITESTGENCAPS*>(tagEntry.data.i32);
                break;
            }
        }
        default:
        {
            NT_LOG_ERROR(" Unknown usecase provided. Aborting.");
            result = CDKResultEFailed;
            break;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetSessionParameters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*CDKResult ChiTestCase::SetSessionParameters(MetadataUsecases usecase, UINT32 cameraID, Size maxStreamSize)
{
    CDKResult result = CDKResultSuccess;

    switch (usecase)
    {
        case MetadataUsecases::Testgen:
        {
            // set session level static parameter
            const CHAR* sectionName = "org.quic.camera.IFETestGenConfig";
            const CHAR* tagName     = "IFETestGenConfig";
            NT_LOG_INFO("Setting session-level parameters for %s for camera %u", tagName, cameraID);

            // Set default testgen resolution based on max size of all output streams
            Size userTestgenSize = maxStreamSize;
            // Fail the test if one or more streams exceed max RDI stream output resolution, to match testgen input
            for (int streamId = 0; streamId < m_numStreams; streamId++)
            {
                if (IsRDIStream(m_format[streamId]))
                {
                    if(m_resolution[streamId] < maxStreamSize)
                    {
                        NT_LOG_UNSUPP("One or more streams exceed max RDI stream resolution in the test.");
                        return CDKResultEUnsupported;
                    }
                }
            }

            // Set IFETestGenConfig based on user input and IFETestGenCapability
            // IFETestGenConfig testgenConfigValues;

            // Get requested list of testgen capabilities and number of exposures from user inputs
            std::vector<CmdLineParser::SensorCaps> userCapList = CmdLineParser::GetSensorCapsList();
            UINT32 nExposures = CmdLineParser::GetNumExposures();

            // HDR tests default to SHDR3, in case of no user input
            // todo: remove this when NCX is ready
            if((m_testId == TestIFEHDR || m_testId == TestHDRPreviewVideo) && userCapList.empty())
            {
                userCapList.push_back(CmdLineParser::SHDR3);
            }

            // lambda function to check if a testgen capability is requested by user
            auto isUserRequested = [&] (CmdLineParser::SensorCaps cap)
            {
                return (std::find(userCapList.begin(), userCapList.end(), cap) != userCapList.end());
            };

            // lambda function to check if user requested testgen resolution is within limits
            auto isValidResolution = [&] (UINT32 maxWidth, UINT32 maxHeight)
            {
                NT_LOG_INFO("Requested testgen resolution is (%d x %d), max supported is (%d x %d)",
                              userTestgenSize.width, userTestgenSize.height, maxWidth, maxHeight);
                if((userTestgenSize.width > maxWidth) || (userTestgenSize.height > maxHeight) || userTestgenSize.isInvalidSize())
                {
                    NT_LOG_UNSUPP("Requested testgen resolution is not supported. Aborting!");
                    return false;
                }
                else
                {
                    return true;
                }
            };

            if(userCapList.empty()) // Default case if no user input for testgen capabilities
            {
                NT_LOG_WARN("No user override for testgen config. Setting IFETestGenConfig values to Bayer");
                testgenConfigValues.testgenMode       = TestGenBayer;
                testgenConfigValues.numberOfExposures = 1;
                if(!isValidResolution(m_pTestGenCaps->maxBayerWidth, m_pTestGenCaps->maxBayerHeight))
                {
                    return CDKResultEUnsupported;
                }
            }
            else if (m_pTestGenCaps->bIsQCFASupported && isUserRequested(CmdLineParser::QCFA)) // QCFA
            {
                NT_LOG_WARN("Setting IFETestGenConfig values to QCFA");
                testgenConfigValues.testgenMode       = TestGenQCFA;
                testgenConfigValues.numberOfExposures = 1;
                if(!isValidResolution(m_pTestGenCaps->maxQCFAWidth, m_pTestGenCaps->maxQCFAHeight))
                {
                    return CDKResultEUnsupported;
                }
            }
            else if(m_pTestGenCaps->bISSHDRSupported && ( isUserRequested(CmdLineParser::SHDR)  ||  // SHDR
                                                          isUserRequested(CmdLineParser::SHDR2) ||
                                                          isUserRequested(CmdLineParser::SHDR3) ))
            {
                NT_LOG_WARN("Setting IFETestGenConfig values to SHDR");
                testgenConfigValues.testgenMode = TestGenSHDR;
                if(nExposures > m_pTestGenCaps->maxNumberOfExposuresinSHDR)
                {
                    NT_LOG_UNSUPP("Requested number of exposures not supported. Aborting!");
                    return CDKResultEUnsupported;
                }
                else
                {
                    testgenConfigValues.numberOfExposures = nExposures;
                }
                if(!isValidResolution(m_pTestGenCaps->maxSHDRWidth, m_pTestGenCaps->maxSHDRHeight))
                {
                    return CDKResultEUnsupported;
                }
            }
            else if(m_pTestGenCaps->bIsQCFAHDRSupported && ( isUserRequested(CmdLineParser::QHDR)  ||  // QHDR
                                                             isUserRequested(CmdLineParser::QHDR2) ||
                                                             isUserRequested(CmdLineParser::QHDR3) ))
            {
                NT_LOG_WARN("Setting IFETestGenConfig values to QCFAHDR");
                testgenConfigValues.testgenMode = TestGenQCFAHDR;
                if(nExposures > m_pTestGenCaps->maxNumberOfExposuresinSHDR)
                {
                    NT_LOG_UNSUPP("Requested number of exposures not supported. Aborting!");
                    return CDKResultEUnsupported;
                }
                else
                {
                    testgenConfigValues.numberOfExposures = nExposures;
                }
                if(!isValidResolution(m_pTestGenCaps->maxQCFAHDRWidth, m_pTestGenCaps->maxQCFAHDRHeight))
                {
                    return CDKResultEUnsupported;
                }
            }
            else if(m_pTestGenCaps->bIsFSSupported && isUserRequested(CmdLineParser::FS)) // FS
            {
                NT_LOG_WARN("Setting IFETestGenConfig values for FS");
                testgenConfigValues.testgenMode       = TestGenFS;
                testgenConfigValues.numberOfExposures = 1;
                if(!isValidResolution(m_pTestGenCaps->maxBayerWidth, m_pTestGenCaps->maxBayerHeight))
                {
                    return CDKResultEUnsupported;
                }
            }
            else if (m_pTestGenCaps->bIsPDAFSupported && isUserRequested(CmdLineParser::PDAF)) // PDAF
            {
                NT_LOG_INFO("Setting IFETestGenConfig values for PDAF");
                testgenConfigValues.testgenMode = TestGenPDAF;
                testgenConfigValues.numberOfExposures = 1;
                if(!isValidResolution(m_pTestGenCaps->maxPDAFWidth, m_pTestGenCaps->maxPDAFHeight))
                {
                    return CDKResultEUnsupported;
                }
            }
            else // if user requested capability not supported by driver
            {
                NT_LOG_UNSUPP("Requested testgen capability is not supported. Aborting!");
                return CDKResultEUnsupported;
            }

            testgenConfigValues.width             = userTestgenSize.width;
            testgenConfigValues.height            = userTestgenSize.height;
            testgenConfigValues.bitWidth          = DEFAULT_BIT_WIDTH; // TODO: change this based on user / test
            NT_LOG_WARN("Setting number of exposures to %d", testgenConfigValues.numberOfExposures);

            result = m_pChiModule->SetStaticMetadata(cameraID, tagName, sectionName,
                                                        static_cast<void*>(&testgenConfigValues), sizeof(IFETestGenConfig));
        }
        break;
        default:
        {
            NT_LOG_ERROR(" Unknown usecase provided. Aborting.");
            result = CDKResultEFailed;
            break;
        }
    }

    return result;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetUsecaseMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::SetUsecaseMetadata(MetadataUsecases usecase, UINT64 fNum, CHICAPTUREREQUEST chiCapReq, Size resMax,
    int camId)
{
    NT_UNUSED_PARAM(resMax);

    CDKResult        res = CDKResultSuccess;
    CHIMETADATAENTRY mdEntry;
    const CHAR*      pTagSec;
    const CHAR*      pTag;

    switch (usecase)
    {
        case FD:
        {
            NT_LOG_DEBUG( "Setting metadata ANDROID_STATISTICS_FACE_DETECT_MODE_FULL for frame:%d", fNum);
            UINT32 tagID = ANDROID_STATISTICS_FACE_DETECT_MODE;
            UINT32 pData = ANDROID_STATISTICS_FACE_DETECT_MODE_FULL;
            UINT32 count = 1;
            res = m_pChiMetadataUtil->GetMetadataOps().pSetTag(chiCapReq.pInputMetadata, tagID, &pData, count);
            break;
        }
        case DualFov:
        {
            NT_LOG_INFO("Enabling Dual FOV in metadata for frame: %d", fNum);
            pTagSec = "org.quic.camera.forceFullFOV";
            pTag = "ForceFullFOV";

            res = m_pChiMetadataUtil->GetMetadataOps().pGetVendorTagEntry(chiCapReq.pInputMetadata, pTagSec, pTag, &mdEntry);
            if (res != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to get vendor tag details: %d", res);
                break;
            }

            UINT8 *tagVal = SAFE_NEW() UINT8[mdEntry.size];
            memset(tagVal, (UINT8)1, mdEntry.size);

            res = m_pChiMetadataUtil->GetMetadataOps().pSetVendorTag(chiCapReq.pInputMetadata, pTagSec, pTag, tagVal,
                mdEntry.count);
            if (res != CDKResultSuccess)
            {
                NT_LOG_ERROR("Failed to set %s.%s", pTagSec, pTag);
            }
            break;
        }
        case Zoom:
        {
            float zm = CmdLineParser::GetZoomFactor();
            NT_LOG_INFO("Applying zoom factor %f on frame: %d", zm, fNum);

            res = m_pChiMetadataUtil->GetMetadataOps().pGetTagEntry(chiCapReq.pInputMetadata, ANDROID_SCALER_CROP_REGION,
                &mdEntry);
            if (res != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to get ANDROID tag details: %d", res);
                break;
            }

            UINT32 xCenter = m_pChiModule->m_pSelectedSensorInfo[camId]->activeArrayCropWindow.width / 2;
            UINT32 yCenter = m_pChiModule->m_pSelectedSensorInfo[camId]->activeArrayCropWindow.height / 2;
            UINT32 xDelta  = xCenter / zm;
            UINT32 yDelta  = yCenter / zm;

            UINT32 scalerCropRegion[4];
            scalerCropRegion[0] = xCenter - xDelta; // left
            scalerCropRegion[1] = yCenter - yDelta; // top
            scalerCropRegion[2] = xCenter + xDelta - scalerCropRegion[0]; // width of new rectangle
            scalerCropRegion[3] = yCenter + yDelta - scalerCropRegion[1]; // height of new rectangle

            mdEntry.pTagData = &scalerCropRegion[0];

            res = m_pChiMetadataUtil->GetMetadataOps().pSetTag(chiCapReq.pInputMetadata, ANDROID_SCALER_CROP_REGION,
                mdEntry.pTagData, mdEntry.count);
            if (res != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to set Zoom on frame %d", fNum);
            }
            break;
        }
        case MaxMetaDataUsecases:
        default:
        {
            NT_LOG_ERROR("Invalid usecase for setting metadata");
            break;
        }
    }
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CommonProcessCaptureResult
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::CommonProcessCaptureResult(ChiCaptureResult * pCaptureResult, SessionPrivateData * pSessionPrivateData)
{
    CDKResult result = CDKResultSuccess;
    uint32_t  resultFrameNum = pCaptureResult->frameworkFrameNum;
    uint32_t  numOutputBuffers = pCaptureResult->numOutputBuffers;
    uint32_t  numPartialMetadata = pCaptureResult->numPartialMetadata;
    SessionId sessionId = static_cast<SessionId>(pSessionPrivateData->sessionId);

    NT_LOG_DEBUG( "Number of buffers obtained in result: [%d] for frameNumber: [%d]", numOutputBuffers, resultFrameNum);

    NT_ASSERT(m_pChiMetadataUtil != nullptr, "ChiMetadataUtil is NULL");

    m_pChiMetadataUtil->VerifyCaptureResultMetadata(static_cast<CHIMETAHANDLE>(pCaptureResult->pOutputMetadata));

    mLastFrameReceived = resultFrameNum;
    if (sessionId == RealtimeSession || sessionId == OfflineSession)
    {
        for (uint32_t bufferIndex = 0; bufferIndex < numOutputBuffers; bufferIndex++)
        {
            CDKResult bufferResult = CDKResultSuccess; // reset for each buffer to be processed
            ChiBufferManager::NativeChiBuffer* outBuffer = const_cast<ChiBufferManager::NativeChiBuffer*>(&pCaptureResult->
                pOutputBuffers[bufferIndex]);
            ChiBufferManager* manager = m_streamBufferMap[outBuffer->pStream];
            if(manager == nullptr)
            {
                NT_LOG_ERROR( "Could not find buffer manger for given stream: %p in frame: %d",
                    outBuffer->pStream, resultFrameNum);
                result = CDKResultEInvalidPointer;
            }
            else
            {
                ChiBufferManager::GenericBuffer genericBuffer(outBuffer);
                if (manager->ProcessBufferFromResult(resultFrameNum, &genericBuffer, true) != 0)
                {
                    bufferResult = CDKResultEFailed;
                }

                int bufStatus = manager->GetStatusFromBuffer(&genericBuffer);

                // If session was flushed...
                if (m_bIsFlushTriggered)
                {
                    // Check for error notify associated with this frame...
                    if (CheckErrorNotifyCode(resultFrameNum, CHIERRORMESSAGECODE::MessageCodeRequest) ||
                            CheckErrorNotifyCode(resultFrameNum, CHIERRORMESSAGECODE::MessageCodeBuffer, outBuffer->pStream))
                    {
                        // Then check for buffer status error due to flush
                        NT_EXPECT(bufferResult == CDKResultEFailed && bufStatus == CAMERA3_BUFFER_STATUS_ERROR,
                                "Expected a buffer status error (1) due to flush. Got status %d instead!", bufStatus);
                    }
                    else
                    {
                        // Warning that no error notify received for this flushed frame (not a hard requirement)
                        NT_LOG_WARN("Notify from flush for frame %d did not occur!", resultFrameNum);
                    }
                }

                if (m_bIsFlushTriggered)
                {
                    if (!(bufferResult == CDKResultEFailed && bufStatus == CAMERA3_BUFFER_STATUS_ERROR))
                    {
                        result = bufferResult;
                    }
                }
                else if (bufStatus == CAMERA3_BUFFER_STATUS_ERROR || bufferResult != CDKResultSuccess)
                {
                    result == bufferResult == CDKResultSuccess ? CDKResultEFailed : bufferResult;
                }
            }
        }

        // Only for CustomNode tests : populate vendortag with current processed frame, verify vendor tag frame matches expected frame number
        if (m_testId == TestVendorTagWrite)
        {
            NT_EXPECT(ValidateCustomNodeResult(pCaptureResult) == CDKResultSuccess,"Custom node result validation failure!");
        }
    }
    else
    {
        NT_LOG_ERROR( "Unknown Session Id: %d obtained", sessionId);
        result = CDKResultEInvalidState;
    }

    if (result == CDKResultSuccess)
    {
        if (m_frameToIpBuffHandleMap.find(pCaptureResult->frameworkFrameNum) != m_frameToIpBuffHandleMap.end()) {
            m_frameToIpBuffHandleMap[
                pCaptureResult->frameworkFrameNum].perFramePendingOpBuffers -= pCaptureResult->numOutputBuffers;
            if (m_frameToIpBuffHandleMap[pCaptureResult->frameworkFrameNum].perFramePendingOpBuffers <= 0) {
                for (std::map<CHIBUFFERHANDLE, ChiBufferManager*>::iterator cbit =
                    m_frameToIpBuffHandleMap[pCaptureResult->frameworkFrameNum].inputBufferHandlesManagersMap.begin();
                    cbit != m_frameToIpBuffHandleMap[pCaptureResult->frameworkFrameNum].inputBufferHandlesManagersMap.end();
                    ++cbit)
                {
                    NT_LOG_DEBUG("releasing reference to input image buffer: %p", cbit->first);
                    NT_ASSERT(cbit->second->ReleaseReferenceToBuffer(cbit->first) == CDKResultSuccess,
                        "releasing reference to input image buffer failed: %p", cbit->first);
                }
                m_frameToIpBuffHandleMap.erase(pCaptureResult->frameworkFrameNum);
            }
        }
        AtomicDecrement(numOutputBuffers);
        NT_LOG_DEBUG( "Number of pending buffers remaining: %d after receiving the result for frame: %d",
            GetPendingBufferCount(), pCaptureResult->frameworkFrameNum);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ValidateCustomNodeResult
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::ValidateCustomNodeResult(ChiCaptureResult* pCaptureResult)
{
    CDKResult result = CDKResultSuccess;
    uint32_t  resultFrameNum = pCaptureResult->frameworkFrameNum;
    UINT64  processedFrameHolder = 0;
    UINT64* pProcessedFrame = &processedFrameHolder;
    UINT32    tagLocation = 0;

    if (nullptr != GetOutputMetadata(pCaptureResult))
    {
        result = m_pChiModule->GetChiVendorTagOps().pQueryVendorTagLocation("com.bots.node.vendortagwrite",
            "ProcessedFrameNumber", &tagLocation);
        if (CDKResultSuccess != result)
        {
            NT_LOG_ERROR( "Could not get vendor tag with section com.bots.node.vendortagwrite");
        }
        else
        {
            m_pChiMetadataUtil->GetVendorTag(const_cast<CHIMETADATAHANDLE>(GetOutputMetadata(pCaptureResult)),
                "com.bots.node.vendortagwrite", "ProcessedFrameNumber", reinterpret_cast<VOID**>(&pProcessedFrame));

            NT_LOG_DEBUG( "Current frameNumber from result metadata: %" PRIu32, resultFrameNum);
            NT_LOG_DEBUG( "Value of frameNumber written by custom node: %" PRIu64, *pProcessedFrame);

            if (*pProcessedFrame != resultFrameNum)
            {
                NT_LOG_ERROR( "Vendor tag processed frame does not match framework result frame");
                result = CDKResultEFailed;
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GenerateOfflineCaptureRequest
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::GenerateOfflineCaptureRequest(UINT64 frameNumber)
{
    CDKResult result = CDKResultSuccess;

    uint32_t requestId = FRAME_REQUEST_MOD(frameNumber);
    memset(&m_offlineRequest[requestId], 0, sizeof(CHICAPTUREREQUEST));

    m_offlineRequest[requestId].numOutputs        = m_numStreams - m_numInputStreams; // not considering the input streams
    m_offlineRequest[requestId].numInputs         = m_numInputStreams;
    m_offlineRequest[requestId].pOutputBuffers    = SAFE_NEW() ChiBufferManager::NativeChiBuffer[m_offlineRequest[requestId].numOutputs];
    m_offlineRequest[requestId].pInputBuffers     = SAFE_NEW() ChiBufferManager::NativeChiBuffer[m_numInputStreams];

    PerFrameData perFrmData;
    perFrmData.perFramePendingOpBuffers      = m_offlineRequest[requestId].numOutputs;
    if (m_flushFrame)
    {
        // Clear notify flag before sending any capture requests
        ClearErrorNotifyCode(frameNumber);
    }

    uint32_t outputIndex = 0;
    uint32_t inputIndex = 0;

    for(std::map<ChiStream*, ChiBufferManager*>::iterator it = m_streamBufferMap.begin(); it != m_streamBufferMap.end(); ++it)
    {
        void* pBuffer = nullptr;
        ChiBufferManager* manager = it->second;
        if (it->first->streamType != ChiStreamTypeInput)
        {
            pBuffer = manager->GetOutputBufferForRequest();
            if (nullptr == pBuffer)
            {
                delete[] m_offlineRequest[requestId].pOutputBuffers;
                return CDKResultENoMore;
            }
            m_offlineRequest[requestId].pOutputBuffers[outputIndex] = *static_cast<ChiBufferManager::NativeChiBuffer*>(pBuffer);
            outputIndex++;
        }
        else
        {
            /* For first 8 requests, use buffers from the input image queue*/
            if (!ms_bEnableNewImageRead)
            {
                pBuffer = manager->GetInputBufferForRequest();
                if (nullptr == pBuffer)
                {
                    delete[] m_offlineRequest[requestId].pInputBuffers;
                    return CDKResultENoMore;
                }

                m_offlineRequest[requestId].pInputBuffers[inputIndex] = *static_cast<ChiBufferManager::NativeChiBuffer*>(pBuffer);
            }
            /* After first 8 requests, start reusing same buffers, but read new images in the buffers*/
            else
            {
                pBuffer = manager->GetInputBufferForRequest(true);
                if (nullptr == pBuffer)
                {
                    delete[] m_offlineRequest[requestId].pInputBuffers;
                    return CDKResultENoMore;
                }

                ChiBufferManager::GenericBuffer* genericBuffer = static_cast<ChiBufferManager::GenericBuffer*>(pBuffer);
                ChiBufferManager::NativeChiStream* streamName = &m_pRequiredStreams[inputIndex];
                ChiBufferManager::GenericStream genericStream(streamName);

                // read new image to be fed to the buffer
                ChiBufferManager::m_lastImageRead[inputIndex] = ChiBufferManager::GetNextImageName(inputIndex);
                int res = manager->GenerateInputBuffer(genericBuffer, &genericStream,
                    ChiBufferManager::m_lastImageRead[inputIndex].c_str());
                if (res != 0)
                {
                    NT_LOG_ERROR("generation of input buffer failed");
                    return CDKResultEFailed;
                }

                m_offlineRequest[requestId].pInputBuffers[inputIndex] = *genericBuffer->pChiBuffer;
            }

            if (pBuffer != nullptr)
            {
                perFrmData.inputBufferHandlesManagersMap[
                    m_offlineRequest[requestId].pInputBuffers[inputIndex].bufferInfo.phBuffer] = manager;
            }
            inputIndex++;
        }

        if (pBuffer == nullptr)
        {
            delete[] m_offlineRequest[requestId].pOutputBuffers;
            delete[] m_offlineRequest[requestId].pInputBuffers;
            return CDKResultENoMore;
        }
    }

    m_frameToIpBuffHandleMap[frameNumber] = perFrmData;

    m_offlineRequest[requestId].frameNumber     = frameNumber;
    m_offlineRequest[requestId].hPipelineHandle = m_pChiPipeline[Offline]->GetPipelineHandle();
    m_offlineRequest[requestId].pPrivData       = &m_requestPvtData[OfflineSession];

    //Get the input metabuffer from the pool
    m_offlineRequest[requestId].pInputMetadata = m_pChiMetadataUtil->GetInputMetabufferFromPool(requestId);
    NT_EXPECT(m_offlineRequest[requestId].pInputMetadata != nullptr, "Failed to create Input Metabuffer before sending request");

    switch (m_testId) {
        case TestFDInputNV12VGAOutputBlob:
        case TestOfflineFD:
            result = SetUsecaseMetadata(FD, frameNumber, m_offlineRequest[requestId], m_streamInfo.maxRes,
                m_currentTestCameraId);
            break;
        case TestIPEMFHDRSnapshot:
        case TestIPEMFHDRPreview:
        case TestIPEMFHDRSnapshotE2E:
        case TestIPEMFHDRPreviewE2E:
            result = SetUsecaseMetadata(MFHDR, frameNumber, m_offlineRequest[requestId], m_streamInfo.maxRes,
                m_currentTestCameraId);
            break;
        default:
            break;
    }

    if ((CmdLineParser::GetZoomFactor() != DEFAULT_ZOOM) && (result == CDKResultSuccess))
    {
        result = SetUsecaseMetadata(MetadataUsecases::Zoom, frameNumber, m_offlineRequest[requestId],
            m_streamInfo.maxRes,  m_currentTestCameraId);
    }

    if (result == CDKResultSuccess)
    {
        result = SetUsecaseMetadata(MetadataUsecases::DefaultOffline, frameNumber, m_offlineRequest[requestId],
            m_streamInfo.maxRes,  m_currentTestCameraId);
    }

    if (result != CDKResultSuccess)
    {
        NT_LOG_ERROR("Unable to set custom metadata required for the test");
        return result;
    }

    //Get the output metabuffer from the pool
    m_offlineRequest[requestId].pOutputMetadata = m_pChiMetadataUtil->GetOutputMetabufferFromPool(requestId,
        &m_pPipelineMetadataInfo[Offline].publishTagArray[0], m_pPipelineMetadataInfo[Offline].publishTagCount);
    NT_EXPECT(m_offlineRequest[requestId].pOutputMetadata != nullptr, "Failed to create Output Metabuffer before sending request");

    memset(&m_submitRequest[m_currentTestCameraId][OfflineSession], 0, sizeof(CHIPIPELINEREQUEST));
    m_submitRequest[m_currentTestCameraId][OfflineSession].pSessionHandle = mPerSessionPvtData[OfflineSession].sessionHandle;
    m_submitRequest[m_currentTestCameraId][OfflineSession].numRequests = 1;
    m_submitRequest[m_currentTestCameraId][OfflineSession].pCaptureRequests = &m_offlineRequest[requestId];

    // Due to FD limitation of 30 FPS, add delay between frames
    if (m_testId == TestFDInputNV12VGAOutputBlob || m_testId == TestOfflineFD || m_testId == TestOfflineIPEFD)
    {
        OsUtils::SleepMilliseconds(FD_MIN_FRAME_INTERVAL);
    }

    result = m_pChiModule->SubmitPipelineRequest(&m_submitRequest[m_currentTestCameraId][OfflineSession]);
    if (CDKResultSuccess == result)
    {
        AtomicIncrement(m_numStreams - m_numInputStreams);
        NT_LOG_DEBUG("Number of pending buffers remaining: %d after sending request for frame: %" PRIu64,
            GetPendingBufferCount(), frameNumber);
    }
    else
    {
        NT_LOG_ERROR("SubmitPipelineRequest to driver failed!");
    }

    // activate pipeline here if EPCR enabled, after sending early requests
    if(result == CDKResultSuccess && frameNumber == m_nEarlyPCR)
    {
        result = m_pChiPipeline[Offline]->ActivatePipeline(mPerSessionPvtData[OfflineSession].sessionHandle);
    }

    // After sending 8th request, enable buffer reuse for reading new images, for further requests
    if (requestId == 7 && ms_bEnableNewImageRead == false)
    {
        ms_bEnableNewImageRead = true; // toggles only once on frame 8
    }
    // trigger flush after requested frame, if enabled
    if (frameNumber == m_flushFrame)
    {
        // Set signal flag to flush and notify flushing thread
        SignalFlushThread(SignalFlushTrigger);

        // Ensure flush thread is completely done flushing before continuing
        SignalFlushThread(SignalFlushReady);
    }


    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GenerateRealtimeCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::GenerateRealtimeCaptureRequest(UINT64 frameNumber, bool bSnapshot, bool bSkipE2EPreview)
{
    CDKResult result = CDKResultSuccess;
    uint32_t outputIndex = 0;

    if (m_flushFrame)
    {
        // Clear notify flag before sending any capture requests
        ClearErrorNotifyCode(frameNumber);
    }

    for(int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
    {
        // skip offline stream if snapshot not requested
        if (!bSnapshot && !m_isRealtime[streamIndex])
        {
            continue;
        }

        if (bSkipE2EPreview && m_isRealtime[streamIndex])
        {
            continue;
        }

        if (!bSnapshot && bSkipE2EPreview) {
            return CDKResultENoMore;
        }

        CHISTREAM* currentStream = m_streamIdMap[m_streamId[streamIndex]];

        void* buffer = m_streamBufferMap[currentStream]->GetOutputBufferForRequest();

        if (buffer == nullptr)
        {
            return CDKResultENoMore;
        }

        m_realtimeOutputBuffers[outputIndex] = *static_cast<ChiBufferManager::NativeChiBuffer*>(buffer);
        outputIndex++;
    }

    uint32_t requestId = FRAME_REQUEST_MOD(frameNumber);
    memset(&m_realtimeRequest[requestId], 0, sizeof(CHICAPTUREREQUEST));

    m_realtimeRequest[requestId].frameNumber       = frameNumber;
    m_realtimeRequest[requestId].hPipelineHandle   = m_pChiPipeline[Realtime]->GetPipelineHandle();
    m_realtimeRequest[requestId].numOutputs        = outputIndex;
    m_realtimeRequest[requestId].pOutputBuffers    = m_realtimeOutputBuffers;
    m_realtimeRequest[requestId].pPrivData         = &m_requestPvtData[RealtimeSession];

    // Get the input metabuffer from the pool
    m_realtimeRequest[requestId].pInputMetadata = m_pChiMetadataUtil->GetInputMetabufferFromPool(requestId);

    if (bSnapshot && bSkipE2EPreview)
    {
        // fetch the last RT preview frame number
        while (mLastFrameReceived < (frameNumber - 1)) {
            if (frameNumber == 1)
            {
                break;
            }
            NT_LOG_DEBUG("GenerateSnapshotRequestofE2EPipeline: waiting for latest frame");
            // sleep for ONE_FRAME_TIME
            OsUtils::SleepMicroseconds(ONE_FRAME_TIME);
        }

        UINT32 prevReqId = FRAME_REQUEST_MOD(mLastFrameReceived);
        m_realtimeRequest[requestId].pInputMetadata = m_pChiMetadataUtil->GetExistingOutputMetabufferFromPool(prevReqId, false);
    }

    NT_EXPECT(m_realtimeRequest[requestId].pInputMetadata != nullptr, "Failed to create Input Metabuffer before sending request");

    switch (m_testId) {
        case TestIFEFullDualFoV:
            result = SetUsecaseMetadata(MetadataUsecases::DualFov, frameNumber, m_realtimeRequest[requestId],
                m_streamInfo.maxRes, m_currentTestCameraId);
            break;
        default:
            break;
    }

    if (CmdLineParser::GetZoomFactor() != DEFAULT_ZOOM)
    {
        result = SetUsecaseMetadata(MetadataUsecases::Zoom, frameNumber, m_realtimeRequest[requestId],
            m_streamInfo.maxRes,  m_currentTestCameraId);
    }

    if (result != CDKResultSuccess)
    {
        NT_LOG_ERROR("Unable to set custom metadata required for the test");
        return result;
    }

    // Get the output metabuffer from the pool
    m_realtimeRequest[requestId].pOutputMetadata = m_pChiMetadataUtil->GetOutputMetabufferFromPool(requestId,
        m_pPipelineMetadataInfo[Realtime].publishTagArray, m_pPipelineMetadataInfo[Realtime].publishTagCount);
    NT_EXPECT(m_realtimeRequest[requestId].pOutputMetadata != nullptr, "Failed to create Output Metabuffer before sending request");

    memset(&m_submitRequest[m_currentTestCameraId][RealtimeSession], 0, sizeof(CHIPIPELINEREQUEST));
    m_submitRequest[m_currentTestCameraId][RealtimeSession].pSessionHandle = mPerSessionPvtData[RealtimeSession].sessionHandle;
    m_submitRequest[m_currentTestCameraId][RealtimeSession].numRequests = 1;
    m_submitRequest[m_currentTestCameraId][RealtimeSession].pCaptureRequests = &m_realtimeRequest[requestId];

    result = m_pChiModule->SubmitPipelineRequest(&m_submitRequest[m_currentTestCameraId][RealtimeSession]);
    if (CDKResultSuccess == result)
    {
        // Only increment pending buffer count if submit pipeline request succeeds
        AtomicIncrement(outputIndex);
        NT_LOG_DEBUG("Number of pending buffers remaining: %d after sending request for frame: %" PRIu64,
            GetPendingBufferCount(), frameNumber);
    }
    else
    {
        NT_LOG_ERROR("SubmitPipelineRequest to driver failed!");
    }

    // activate pipeline here if EPCR enabled, after sending early requests
    if(result == CDKResultSuccess && frameNumber == m_nEarlyPCR)
    {
        result = m_pChiPipeline[Realtime]->ActivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
    }

    // trigger flush after requested frame, if enabled
    if (frameNumber == m_flushFrame)
    {
        // Set signal flag to flush and notify flushing thread
        SignalFlushThread(SignalFlushTrigger);

        // Ensure flush thread is completely done flushing before continuing
        SignalFlushThread(SignalFlushReady);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// EnableOverrides
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::EnableOverrides()
{
    CDKResult res = CDKResultSuccess;

    NT_LOG_INFO("Current overrides in the device are:");
    ReadCamXOR(true);

    std::map<std::string, std::string> camxorMapMirror = m_camXORMap;

    switch(m_testId) {
        case TestOfflineFD:
        case TestOfflineIPEFD:
        case TestFDInputNV12VGAOutputBlob:
        case TestIFEFD:
            camxorMapMirror["enableOfflineFD"] = "TRUE";
            camxorMapMirror["enableFDManagerThreading"] = "FALSE";
            camxorMapMirror["FDPreprocessing"] = "Disable";
            break;
        case TestPreviewHVX:
            camxorMapMirror["enableHVXStreaming"]="1";
            break;
        case TestIPEMFHDRSnapshot:
        case TestIPEMFHDRPreview:
        case TestIPEMFHDRSnapshotE2E:
        case TestIPEMFHDRPreviewE2E:
            camxorMapMirror["bypassIPEICADependency"]="TRUE";
            break;
        default:
            NT_LOG_INFO("Removing unnecessary overrides");
            camxorMapMirror.erase("enableTPG"); // Non-testgen tests are not intended to run with this
            camxorMapMirror.erase("numPCRsBeforeStreamOn"); // having this and not sending PCR before session is not useful
            camxorMapMirror.erase("bypassIPEICADependency"); // Bypass ICA dependency isn't good if its not MFHDR offline
            camxorMapMirror.erase("enableHVXStreaming"); // HVX is not good for other tests
            camxorMapMirror.erase("enableOfflineFD"); // Unless it Native CHI FD, these are not required
            camxorMapMirror.erase("enableFDManagerThreading"); // Unless it Native CHI FD, these are not required
            camxorMapMirror.erase("FDPreprocessing"); // Unless it Native CHI FD, these are not required
            camxorMapMirror.erase("IFETestImageSizeHeight"); // not required unless testgen mode
            camxorMapMirror.erase("IFETestImageSizeWidth"); // not required unless testgen mode

            if (CmdLineParser::RemoveSensor())
            {
                camxorMapMirror["enableTPG"]="TRUE";

                if (m_userPrvSize.height != 0 || m_userRDISize.height != 0)
                {
                    // For RDI streams, set testgen resolution to RDI resolution
                    if(IsRDIStream(static_cast<CHISTREAMFORMAT>(m_userPrvFormat)))
                    {
                        camxorMapMirror["IFETestImageSizeHeight"] = std::to_string(m_userRDISize.height);
                    }
                    // Set greater of preview or RDI for all other except stats blob streams (height is 1)
                    else if (!(m_userPrvFormat == ChiStreamFormatBlob && m_userPrvSize.height == 1))
                    {
                        camxorMapMirror["IFETestImageSizeHeight"] = m_userPrvSize.height > m_userRDISize.height ?
                            m_userPrvSize.height : m_userRDISize.height;
                    }
                }
                else if(m_testId == TestCVPDME || m_testId == TestCVPDMEDisplayPort || m_testId == TestCVPDMEICA)
                {
                    camxorMapMirror["IFETestImageSizeHeight"] = "2160";
                }

                if (m_userPrvSize.width != 0 || m_userRDISize.width != 0)
                {
                    // For RDI streams, set testgen resolution to RDI resolution
                    if(IsRDIStream(static_cast<CHISTREAMFORMAT>(m_userPrvFormat)))
                    {
                        camxorMapMirror["IFETestImageSizeWidth"] = std::to_string(m_userRDISize.width);
                    }
                    // Set greater of preview or RDI for all other except stats blob streams (height is 1)
                    else if (!(m_userPrvFormat == ChiStreamFormatBlob && m_userPrvSize.height == 1))
                    {
                        camxorMapMirror["IFETestImageSizeWidth"] = m_userPrvSize.width > m_userRDISize.width ?
                            m_userPrvSize.width : m_userRDISize.width;
                    }
                }
                else if(m_testId == TestCVPDME || m_testId == TestCVPDMEDisplayPort || m_testId == TestCVPDMEICA)
                {
                    camxorMapMirror["IFETestImageSizeWidth"] = "4096";
                }
            }

            break;
    }

    if(m_nEarlyPCR)
    {
        camxorMapMirror["numPCRsBeforeStreamOn"] = std::to_string(m_nEarlyPCR);
    }
    else
    {
        camxorMapMirror.erase("numPCRsBeforeStreamOn");
    }
    NT_LOG_INFO("Enabling below override settings:");
    return WriteCamXORMapToFile(camxorMapMirror);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ReadCamXOR
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::ReadCamXOR(bool preserve)
{
    CDKResult res = CDKResultSuccess;

    std::ifstream camxorFile {CAMXORFILE};

    if (!camxorFile.is_open())
    {
        return res;
    }

    std::map<std::string, std::string> readORMap;

    while (camxorFile)
    {
        std::string setting{}, value{};
        getline(camxorFile, setting, '=') && getline(camxorFile, value);
        if (camxorFile) {
            readORMap[setting] = value;
        }
    }

    for (std::map<std::string, std::string>::iterator camxor = readORMap.begin(); camxor != readORMap.end(); ++camxor)
    {
        NT_LOG_INFO("setting %s: %s", camxor->first.c_str(), camxor->second.c_str());
    }

    if (preserve)
    {
        m_camXORMap = readORMap;
    }
    readORMap.clear();

    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// WriteCamXORMapToFile
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiTestCase::WriteCamXORMapToFile(std::map<std::string, std::string> writeMap)
{
    std::ofstream writecamxor(CAMXORFILE);

    if (!writecamxor.is_open()) {
        NT_LOG_ERROR("Unable to open file for writing camxoverrides");
        return CDKResultEFailed;
    }

    for (std::map<std::string, std::string>::iterator camxor = writeMap.begin(); camxor != writeMap.end(); ++camxor)
    {
        writecamxor << camxor->first.c_str();
        writecamxor << "=";
        writecamxor << camxor->second.c_str();
        writecamxor << "\n";
    }
    writecamxor.close();

    return ReadCamXOR();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// WaitForFlushSignal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* ChiTestCase::WaitForFlushSignal(VOID* pArg)
{
    bool isDone = false;
    ChiTestCase* testInstance = NULL;

    // Get the ChiTestCase instance
    if (nullptr != pArg)
    {
        testInstance = reinterpret_cast<ChiTestCase*>(pArg);
    }
    else
    {
        NT_EXPECT(nullptr != pArg, "No ChiTestCase instance provided!");
        return nullptr;
    }

    /* START critical section */
    testInstance->m_pFlushSignalMutex->Lock();

    // Main loop
    while (!isDone)
    {
        // Wait for next signal
        testInstance->m_pFlushSignalCond->Wait(testInstance->m_pFlushSignalMutex->GetNativeHandle());

        switch (testInstance->m_flushSignaler)
        {
            case SignalFlushReady:
                // Do nothing
                NT_LOG_DEBUG("Flush thread ready...");
                break;
            case SignalFlushTrigger:
                // Delay and trigger flush
                NT_LOG_INFO("Delay before flush: %u us (%u ms)", testInstance->m_usFlushDelay,testInstance->m_usFlushDelay / 1000);
                OsUtils::SleepMicroseconds(testInstance->m_usFlushDelay);

                switch (testInstance->m_pipelineConfig)
                {
                    case RealtimeConfig:
                        testInstance->TriggerFlush(RealtimeSession);
                        break;
                    case OfflineConfig:
                        testInstance->TriggerFlush(OfflineSession);
                        break;
                    case MixedConfig:
                        testInstance->TriggerFlush(RealtimeSession);
                        testInstance->TriggerFlush(OfflineSession);
                        break;
                    default:
                        NT_LOG_ERROR("Invalid PipelineConfig provided! %d", testInstance->m_pipelineConfig);
                        break;
                }
                break;
            case SignalFlushDone:
                // Ready to exit
                NT_LOG_DEBUG("Flush thread done...");
                isDone = true;
                break;
            default:
                NT_LOG_ERROR("Invalid FlushSignal! (%d)", testInstance->m_flushSignaler);
                break;
        }

        // Notify that flush thread is ready for next command
        testInstance->m_flushSignaler = SignalFlushReady;
        testInstance->m_pFlushSignalCond->Broadcast();
    }

    testInstance->m_pFlushSignalMutex->Unlock();
    /* END critical section */

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SignalFlushThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::SignalFlushThread(FlushSignal signal)
{
    /* START critical section */
    m_pFlushSignalMutex->Lock();

    // Wait for thread to be ready
    while (SignalFlushReady != m_flushSignaler)
    {
        m_pFlushSignalCond->Wait(m_pFlushSignalMutex->GetNativeHandle());
    }

    // Set the signal
    m_flushSignaler = signal;

    m_pFlushSignalMutex->Unlock();
    /* END critical section */

    // Notify the thread
    NT_LOG_DEBUG("Signaling flush thread: %s", m_flushSignalStrings[signal]);
    m_pFlushSignalCond->Broadcast();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TriggerFlush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChiTestCase::TriggerFlush(SessionId sessionId)
{
    CDKResult result = CDKResultSuccess;
    m_bIsFlushTriggered = true;

    /* Start tracking flush time */
    m_flushStartTime = OsUtils::GetNanoSeconds();

    // TODO: enable pipeline flush support to all tests
    if(m_testId == MultiCameraIFEFullFlushSinglePipeline)
    {
        // For MC tests : trigger pipeline flush only if pipeline ID in valid range
        int pipelineId = CmdLineParser::GetFlushPipelineId();
        if (pipelineId >= 0 && pipelineId < MaximumPipelinesPerSession)
        {
            NT_LOG_INFO("Triggering pipeline flush on pipeline # %d after sending frame: %d", pipelineId,
                mCurrentResultFrame[pipelineId]);
            result = mPerSessionPvtData[sessionId].pChiSession->FlushPipeline(m_pChiPipeline[pipelineId]->GetPipelineHandle(),
                mCurrentResultFrame[pipelineId]);
            NT_EXPECT(CDKResultSuccess == result, "FlushPipeline call to driver failed with result %d !", result);
        }
    }
    else
    {
        if (ChiTestCase::sbIsCameraIDSessionID)
        {
            NT_LOG_INFO("Triggering flush on all sessions...");
            // todo: remove m_currentTestCameraId and use m_cameraList across all suites
            if (std::find(m_cameraList.begin(), m_cameraList.end(), m_currentTestCameraId) == m_cameraList.end())
            {
                m_cameraList.push_back(m_currentTestCameraId);
            }
            for (std::vector <UINT32>::iterator pCamID = m_cameraList.begin(); pCamID < m_cameraList.end(); pCamID++)
            {
                if(mPerSessionPvtData[*pCamID].pChiSession != nullptr)
                {
                    result = mPerSessionPvtData[*pCamID].pChiSession->FlushSession();
                    NT_EXPECT(CDKResultSuccess == result, "FlushSession call to driver failed with result %d !", result);
                }
            }
        }
        else
        {
            result = mPerSessionPvtData[sessionId].pChiSession->FlushSession();
            NT_EXPECT(CDKResultSuccess == result, "FlushSession call to driver failed with result %d !", result);
        }
    }

    /* End tracking flush time */
    m_flushEndTime = OsUtils::GetNanoSeconds();
    double flushTime = (m_flushEndTime - m_flushStartTime) / 1000000000.0;
    NT_LOG_PERF("KPI: flush return time took %f seconds", flushTime);
}

}
