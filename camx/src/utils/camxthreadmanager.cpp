////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxthreadmanager.cpp
///
/// @brief Specific factory method declaration and implementation for Thread Pool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxincs.h"
#include "camxmem.h"
#include "camxthreadmanager.h"
#include "camxthreadcore.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::Create(
    ThreadManager** ppInstance,
    const CHAR*     pName,
    UINT32          numThreads)
{
    CAMX_ENTRYEXIT_SCOPE(CamxLogGroupUtils, SCOPEEventThreadManagerCreate);

    CamxResult      result          = CamxResultEFailed;
    ThreadManager*  pLocalInstance  = NULL;

    CAMX_ASSERT(NULL == *ppInstance);

    pLocalInstance = CAMX_NEW ThreadManager(numThreads);
    if (NULL != pLocalInstance)
    {
        result = pLocalInstance->Initialize(pName);
        if (CamxResultSuccess != result)
        {
            CAMX_DELETE pLocalInstance;
            pLocalInstance = NULL;
        }
    }

    if (CamxResultSuccess == result)
    {
        *ppInstance = pLocalInstance;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::ThreadManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ThreadManager::ThreadManager(
    UINT32 numThreads)
{
    if (numThreads > MaxThreadsPerPool)
    {
        numThreads = MaxThreadsPerPool;
    }

    m_numThreads = numThreads;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::~ThreadManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ThreadManager::~ThreadManager()
{
    if (NULL != m_pCore)
    {
        CAMX_DELETE(m_pCore);
        m_pCore = NULL;
    }
    if (NULL != m_pJobList)
    {
        CAMX_DELETE(m_pJobList);
        m_pJobList = NULL;
    }
    if (NULL != m_pJobRegistry)
    {
        CAMX_DELETE(m_pJobRegistry);
        m_pJobRegistry = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ThreadManager::Destroy()
{
    CAMX_ENTRYEXIT_SCOPE(CamxLogGroupUtils, SCOPEEventThreadManagerDestroy);

    CAMX_DELETE this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::Initialize(
    const CHAR* pName)
{
    CamxResult result = CamxResultSuccess;

    m_pJobRegistry = CAMX_NEW JobRegistry;
    if (NULL != m_pJobRegistry)
    {
        result= m_pJobRegistry->Initialize();
    }
    else
    {
        result = CamxResultEFailed;
    }

    if (CamxResultSuccess == result)
    {
        m_pJobList = CAMX_NEW JobList(m_pJobRegistry);
        if (NULL != m_pJobList)
        {
            result= m_pJobList->Initialize();
        }
        else
        {
            result = CamxResultEFailed;
        }
    }

    if (CamxResultSuccess == result)
    {
        m_pCore = CAMX_NEW ThreadCore(m_pJobRegistry, m_pJobList, pName, m_numThreads);
        if (NULL != m_pCore)
        {
            result= m_pCore->Initialize();
        }
        else
        {
            result = CamxResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::RegisterJobFamily
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::RegisterJobFamily(
    JobFunc     jobFuncAddr,
    const CHAR* pJobFuncName,
    JobCb       flushDoneCb,
    JobPriority priority,
    BOOL        isSerialize,
    JobHandle*  phJob)
{
    CamxResult result = CamxResultSuccess;

    result = m_pJobRegistry->RegisterNewJob(jobFuncAddr, pJobFuncName, flushDoneCb,
                                            priority, isSerialize, phJob);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::UnregisterJobFamily
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::UnregisterJobFamily(
    JobFunc     jobFuncAddr,
    const CHAR* pJobFuncName,
    JobHandle   hJob)
{
    CamxResult result = CamxResultSuccess;

    // Flush the job family.
    result = FlushJobFamily(hJob, this, TRUE);

    // Prune it from the registry
    if (CamxResultSuccess == result)
    {
        result = m_pJobRegistry->UnregisterJob(jobFuncAddr, pJobFuncName, hJob);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::PostJob
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::PostJob(
    JobHandle   hJob,
    JobCb       stoppedCb,
    VOID**      ppData,
    BOOL        isSplitable,
    BOOL        isBlocking)
{
    CAMX_ASSERT(NULL != ppData);

    CamxResult result = CamxResultSuccess;

    result = m_pCore->AcceptNewJob(hJob, stoppedCb, ppData, isSplitable, isBlocking);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::FlushJobFamily
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::FlushJobFamily(
    JobHandle   hJob,
    VOID*       pUserData,
    BOOL        isBlocking)
{
    CamxResult result = CamxResultSuccess;

    result = m_pCore->FlushJob(hJob, pUserData, isBlocking);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::ResumeJobFamily
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult ThreadManager::ResumeJobFamily(
    JobHandle   hJob)
{
    CamxResult result = CamxResultSuccess;

    result = m_pCore->ResumeJob(hJob);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::GetJobCount
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ThreadManager::GetJobCount(
    JobHandle  hJob
    ) const
{
    UINT32 jobCount = 0;

    if (InvalidJobHandle != hJob)
    {
        jobCount = m_pCore->GetJobCount(hJob);
    }
    else
    {
        CAMX_LOG_VERBOSE(CamxLogGroupUtils, "InvalidJobHandle returning jobcount as 0");
    }

    return jobCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThreadManager::GetInFlightCount
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ThreadManager::GetInFlightCount(
    JobHandle  hJob
    ) const
{
    UINT32 inFlightCount = 0;

    if (InvalidJobHandle != hJob)
    {
        inFlightCount = m_pCore->GetInFlightCount(hJob);
    }
    else
    {
        CAMX_LOG_VERBOSE(CamxLogGroupUtils, "InvalidJobHandle returning inFlightCount as 0");
    }

    return inFlightCount;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ThreadManager::DumpStateToFile
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ThreadManager::DumpStateToFile(
    INT     fd,
    UINT32  indent)
{
    m_pJobRegistry->DumpStateToFile(fd, indent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ThreadManager::DumpStateToLog
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ThreadManager::DumpStateToLog()
{
    m_pJobRegistry->DumpStateToLog();
}

CAMX_NAMESPACE_END
