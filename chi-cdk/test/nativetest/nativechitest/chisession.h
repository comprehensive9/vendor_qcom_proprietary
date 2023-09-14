//*************************************************************************************************
// Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __CHI_SESSION__
#define __CHI_SESSION__

#include "chipipeline.h"
#include "chimodule.h"

namespace chitests {

static const int MAXIMUM_CAMERAS  = 8;
static const int MAXIMUM_SESSIONS = 32;
static const int MaxActiveRealtimePipelines = 6;

class ChiSession
{

public:

    static ChiSession* Create(ChiPipeline** ppPipelines, int numPipelines, ChiCallBacks* pCallbacks, void* pPrivateData);
    CDKResult          Initialize(ChiPipeline** ppPipelines, int numPipelines, ChiCallBacks* pCallbacks, void* pPrivateData);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiSession::CreateSession
    ///
    /// @brief Create a chi session based on the given parameters
    ///
    /// @return CHIHANDLE   handle to the created session
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHIHANDLE          CreateSession();

    CHIHANDLE          GetSessionHandle() const;
    CDKResult          FlushSession() const;
    CDKResult          FlushPipeline(CHIPIPELINEHANDLE chiPipelineHandle, UINT64 frameNo) const;
    void               DestroySession() const;

private:
    CHIHANDLE              m_hSession;  // created session handle
    SessionCreateData      m_sessionCreateData; // data required for session creation
    ChiModule*             m_pChiModule; // pointer to the ChiModule singleton

    ChiSession();
    ~ChiSession();

    /// Do not allow the copy constructor or assignment operator
    ChiSession(const ChiSession&) = delete;
    ChiSession& operator= (const ChiSession&) = delete;
};

} //namespace chitests

#endif  //#ifndef __CHI_SESSION_