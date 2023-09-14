////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxhwcontext.cpp
/// @brief HwContext class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxhwcontext.h"
#include "camxhwenvironment.h"
#include "camxhwfactory.h"
#include "camximagesensormoduledata.h"
#include "camxincs.h"
#include "camxmem.h"
#include "camxpacket.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::Create(
    HwContextCreateData* pCreateData)
{
    CamxResult result = CamxResultEInvalidState;

    CAMX_ASSERT((NULL != pCreateData)                 &&
                (NULL != pCreateData->pHwEnvironment) &&
                (NULL != pCreateData->pHwEnvironment->GetHwStaticEntry()->Create));

    result = pCreateData->pHwEnvironment->GetHwStaticEntry()->Create(pCreateData);

    pCreateData->pHwContext->Initialize(pCreateData);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::Initialize(
    HwContextCreateData* pCreateData)
{
    CamxResult result = CamxResultSuccess;

    CAMX_ASSERT(NULL != pCreateData);

    m_pHwEnvironment = pCreateData->pHwEnvironment;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::~HwContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HwContext::~HwContext()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::FlushLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::FlushLock(
    CSLHandle           hCSLSession,
    const CSLFlushInfo& rCSLFlushInfo)
{
    CamxResult result = CamxResultEInvalidState;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLFlushLock(hCSLSession, rCSLFlushInfo);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::FlushUnlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::FlushUnlock(
    CSLHandle hCSLSession)
{
    CamxResult result = CamxResultEInvalidState;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLFlushUnlock(hCSLSession);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::FlushRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::FlushRequest(
    CSLHandle     hCSLSession,
    CSLLinkHandle hLink,
    UINT64        cslSyncId,
    UINT64        requestId)
{

    CamxResult result = CamxResultSuccess;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        CAMX_LOG_INFO(CamxLogGroupCore, "Sending cancel request for requestId %llu cslSyncId", requestId, cslSyncId);
        CSLFlushInfo cslFlushInfo = {};

        cslFlushInfo.flushType      = CSLFlushRequest;
        cslFlushInfo.phSyncLink     = &hLink;
        cslFlushInfo.firstCSLSyncId = cslSyncId;
        cslFlushInfo.lastCSLSyncId  = cslSyncId;
        cslFlushInfo.firstRequestId = requestId;
        cslFlushInfo.lastRequestId  = requestId;
        result                      = CSLCancelRequest(hCSLSession, cslFlushInfo);
    }
    else
    {
        result = CamxResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::Link
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::Link(
    CSLHandle        hCSLSession,
    CSLLinkHandle*   phLink,
    CSLDeviceHandle* phDeviceHandles)
{
    CamxResult result = CamxResultSuccess;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLLink(hCSLSession, phDeviceHandles, 0, phLink);
    }
    else
    {
        result = CamxResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::Unlink
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::Unlink(
    CSLHandle        hCSLSession,
    CSLLinkHandle*   phLink,
    CSLDeviceHandle* phDeviceHandles)
{
    CamxResult result = CamxResultSuccess;

    CAMX_UNREFERENCED_PARAM(phDeviceHandles);

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLUnlink(hCSLSession, phLink);
    }
    else
    {
        result = CamxResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::StreamOn
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::StreamOn(
    CSLHandle        hCSLSession,
    CSLLinkHandle*   phLink,
    CSLDeviceHandle* phDeviceHandles)
{
    CamxResult result = CamxResultSuccess;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLStreamOn(hCSLSession, phLink, phDeviceHandles);
    }
    else
    {
        result = CamxResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::StreamOff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::StreamOff(
    CSLHandle                   hCSLSession,
    CSLLinkHandle*              phLink,
    CSLDeviceHandle*            phDeviceHandles,
    CHIDEACTIVATEPIPELINEMODE   mode)
{
    CamxResult result = CamxResultSuccess;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLStreamOff(hCSLSession, phLink, phDeviceHandles, static_cast<CSLDeactivateMode>(mode));
    }
    else
    {
        result = CamxResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::Submit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::Submit(
    CSLHandle       hCSLSession,
    CSLDeviceHandle hDevice,
    Packet*         pPacket)
{
    CamxResult result = CamxResultSuccess;

    CAMX_ASSERT(CSLInvalidHandle != hCSLSession);

    if (CSLInvalidHandle != hCSLSession)
    {
        result = CSLSubmit(hCSLSession, hDevice, pPacket->GetMemHandle(), pPacket->GetOffset());
    }
    else
    {
        result = CamxResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::GetImageSensorModuleData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ImageSensorModuleData* HwContext::GetImageSensorModuleData(
    UINT32 cameraId
    ) const
{
    return m_pHwEnvironment->GetImageSensorModuleData(cameraId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::GetDeviceVersion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::GetDeviceVersion(
    CSLDeviceType   deviceType,
    CSLVersion*     pVersion
    ) const
{
    return m_pHwEnvironment->GetDeviceVersion(deviceType, pVersion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::GetDriverVersion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::GetDriverVersion(
    CSLDeviceType   deviceType,
    CSLVersion*     pVersion
    ) const
{
    return m_pHwEnvironment->GetDriverVersion(deviceType, pVersion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::GetStaticSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const StaticSettings* HwContext::GetStaticSettings()
{
    return m_pHwEnvironment->GetSettingsManager()->GetStaticSettings();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HwContext::DumpRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult HwContext::DumpRequest(
    CSLHandle              hCSLSession,
    CSLDumpRequestInfo*    pDumpRequestInfo,
    SIZE_T*                pFilledLength)
{
    CamxResult result = CamxResultEInvalidArg;

    if ((CSLInvalidHandle != hCSLSession) && (NULL != pDumpRequestInfo) && (NULL != pFilledLength))
    {
        CAMX_LOG_DUMP(CamxLogGroupCore, "DUMP issueRequestId:%llu issueSyncId: %llu hBuf: %u",
            pDumpRequestInfo->issueRequestId, pDumpRequestInfo->issueSyncId, pDumpRequestInfo->hBuf);
        result = CSLDumpRequest(hCSLSession, pDumpRequestInfo, pFilledLength);

        if (result != CamxResultSuccess)
        {
            CAMX_LOG_ERROR(CamxLogGroupCore, "DUMP failed issue req:%llu sync: %llu hbuf: %u",
                pDumpRequestInfo->issueRequestId, pDumpRequestInfo->issueSyncId, pDumpRequestInfo->hBuf);
        }
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupCSL, "Invalid parameters: hCSLSession: %u pDumpRequestInfo %p pFilledLength %p",
            hCSLSession, pDumpRequestInfo, pFilledLength);
    }

    return result;
}
CAMX_NAMESPACE_END
