//******************************************************************************************************************************
// Copyright (c) 2016 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************
//==============================================================================================================================
// IMPLEMENTATION of ChiSession
//==============================================================================================================================

#include "chisession.h"

namespace chitests
{

/***************************************************************************************************************************
*   ChiSession::ChiSession
*
*   @brief
*       Constructor for ChiSession
***************************************************************************************************************************/
ChiSession::ChiSession()
{
    m_hSession = nullptr;
    m_pChiModule = ChiModule::GetInstance();
    if (nullptr == m_pChiModule) {
        NT_LOG_ERROR("ChiModule could not be instantiated");
    }
}

/***************************************************************************************************************************
*   ChiSession::~ChiSession
*
*   @brief
*       Destructor for ChiSession
***************************************************************************************************************************/
ChiSession::~ChiSession()
{
    m_hSession = nullptr;
    m_pChiModule = ChiModule::GetInstance();
    if (nullptr == m_pChiModule) {
        NT_LOG_ERROR("ChiModule could not be instantiated");
    }
}

/***************************************************************************************************************************
*   ChiSession::Create
*
*   @brief
*       static function to create an instance of the chisession
*   @param
*        [in]  ChiPipeline**   ppPipelines    pointer to all created pipelines
*        [in]  int             numPipelines   number of pipelines present for the session
*        [in]  ChiCallBacks*   pCallbacks     callback function for driver to return the result
*        [in]  void*           pPrivateData   pointer to privatedata specific to the testcase
*   @return
*       ChiSession*  instance to the newly created session
***************************************************************************************************************************/
ChiSession* ChiSession::Create(ChiPipeline** ppPipelines, int numPipelines, ChiCallBacks* pCallbacks, void* pPrivateData)
{
    CDKResult result = CDKResultSuccess;
    ChiSession* pNewSession = SAFE_NEW() ChiSession();

    if (pNewSession != nullptr)
    {
        result = pNewSession->Initialize(ppPipelines, numPipelines, pCallbacks, pPrivateData);

        if (result != CDKResultSuccess)
        {
            delete pNewSession;
            pNewSession = nullptr;
        }
    }

    return pNewSession;
}

/***************************************************************************************************************************
*   ChiSession::Initialize
*
*   @brief
*       member function to initialize newly created instance of the chisession
*   @param
*        [in]  ChiPipeline**   ppPipelines    pointer to all created pipelines
*        [in]  int             numPipelines   number of pipelines present for the session
*        [in]  ChiCallBacks*   pCallbacks     callback function for driver to return the result
*        [in]  void*           pPrivateData   pointer to privatedata specific to the testcase
*   @return
*       CDKResult  result code to determine outcome of the function
***************************************************************************************************************************/
CDKResult ChiSession::Initialize(ChiPipeline** ppPipelines, int numPipelines, ChiCallBacks* pCallbacks, void* pPrivateData)
{
    CDKResult         result = CDKResultSuccess;

    memset(&m_sessionCreateData, 0, sizeof(SessionCreateData));   // Zero out session create data
    m_sessionCreateData.numPipelines = numPipelines;
    m_sessionCreateData.pCallbacks = pCallbacks;
    m_sessionCreateData.pPrivateCallbackData = pPrivateData;

    if (numPipelines > MaximumPipelinesPerSession)
    {
        NT_LOG_ERROR( "Session cannot have more than %d pipelines", MaximumPipelinesPerSession);
        return CDKResultEFailed;
    }

    int pipelineCount =0;
    UINT32 rtPipelineCount = 0;
    for (int i = 0; pipelineCount < numPipelines && i < MaximumPipelinesPerSession; i++)
    {
        if (pipelineCount == MaximumPipelinesPerSession)
        {
            NT_LOG_ERROR( "pipelineCount is %d and Session cannot have more than %d pipelines",
                pipelineCount, MaximumPipelinesPerSession);
            result = CDKResultEFailed;
            break;
        }

        //TODO : Find an elegant way to achieve this
        if ((nullptr != ppPipelines[i]) && (MaximumPipelinesPerSession > pipelineCount))
        {
            m_sessionCreateData.pPipelineInfos[pipelineCount++] = ppPipelines[i]->GetPipelineInfo();

            if (m_sessionCreateData.pPipelineInfos[pipelineCount].pipelineInputInfo.isInputSensor == 1)
            {
                rtPipelineCount++;
                if (rtPipelineCount >= MaxActiveRealtimePipelines)
                {
                    NT_LOG_ERROR("Realtime Pipeline Count (%d) crossed threshold of %d", rtPipelineCount,
                        MaxActiveRealtimePipelines);
                    result = CDKResultETooManyUsers;
                    break;
                }
            }
        }
    }

    if (CDKResultSuccess == result)
    {
        m_hSession = CreateSession();

        NT_LOG_DEBUG( "Created session handle: %p", m_hSession);

        if (NULL == m_hSession)
        {
            result = CDKResultEFailed;
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSession::CreateSession
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIHANDLE ChiSession::CreateSession()
{
    ChiModule* pChiModule = ChiModule::GetInstance();
    if (nullptr == pChiModule) {
        NT_LOG_ERROR("ChiModule could not be instantiated");
        return NULL;
    }

    NT_LOG_INFO( "Creating session on context: %p", pChiModule->GetContext());
    CHISESSIONFLAGS flags;
    flags.u.isNativeChi = 1;
    NT_LOG_DEBUG( "ChiContext = %p, PipelineInfo = %p, Callback = %p", pChiModule->GetContext(),
        m_sessionCreateData.pPipelineInfos, m_sessionCreateData.pCallbacks);

    return pChiModule->GetChiOps()->pCreateSession(
        pChiModule->GetContext(), m_sessionCreateData.numPipelines, m_sessionCreateData.pPipelineInfos,
        m_sessionCreateData.pCallbacks, m_sessionCreateData.pPrivateCallbackData, flags);
}

/***************************************************************************************************************************
*   ChiSession::GetSessionHandle
*
*   @brief
*       return the created session handle
*   @return
*       CHIHANDLE created session handle
***************************************************************************************************************************/
CHIHANDLE ChiSession::GetSessionHandle() const
{
    return m_hSession;
}

/***************************************************************************************************************************
*   ChiSession::FlushSession
*
*   @brief
*       Flush the session
*   @return
*       CDKResult
***************************************************************************************************************************/
CDKResult ChiSession::FlushSession() const
{
    // passing session handle through flush session info for pipeline flush imeplementation
    CHISESSIONFLUSHINFO hSessionFlushInfo = { 0 };
    hSessionFlushInfo.pSessionHandle = m_hSession;

    NT_LOG_INFO( "Flushing session: %p on context: %p", m_hSession, m_pChiModule->GetContext());
    return m_pChiModule->GetChiOps()->pFlushSession(m_pChiModule->GetContext(), hSessionFlushInfo);
}

/***************************************************************************************************************************
*   ChiSession::FlushPipeline
*
*   @brief
*       Flush the pipeline
*   @return
*       CDKResult
***************************************************************************************************************************/
CDKResult ChiSession::FlushPipeline(CHIPIPELINEHANDLE chiPipelineHandle, UINT64 frameNo) const
{
    NT_UNUSED_PARAM(frameNo);
    NT_LOG_INFO("Flushing chiPipelineHandle %p on session: %p on context: %p", chiPipelineHandle, m_hSession,
        m_pChiModule->GetContext());

    CHISESSIONFLUSHINFO  hSessionFlushInfo = { 0 };
    CHIPIPELINEFLUSHINFO pipelineFlushInfo;

    pipelineFlushInfo.flushType              = FlushAll;
    pipelineFlushInfo.hPipelineHandle        = chiPipelineHandle;
    hSessionFlushInfo.numPipelines           = 1;
    hSessionFlushInfo.pSessionHandle         = m_hSession;
    hSessionFlushInfo.pPipelineFlushInfo     = &pipelineFlushInfo;

    return m_pChiModule->GetChiOps()->pFlushSession(m_pChiModule->GetContext(), hSessionFlushInfo);
}

/***************************************************************************************************************************
*   ChiSession::DestroySession
*
*   @brief
*       Destroy session
*   @return
*      none
***************************************************************************************************************************/
void ChiSession::DestroySession() const
{
    NT_LOG_INFO( "Destroying session: %p on context: %p", m_hSession, m_pChiModule->GetContext());
    m_pChiModule->GetChiOps()->pDestroySession(m_pChiModule->GetContext(), m_hSession, false);
    delete this;
}
}
