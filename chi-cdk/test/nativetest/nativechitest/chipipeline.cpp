//******************************************************************************************************************************
// Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************

#include "chipipeline.h"
#include "chimetadatautil.h"

namespace chitests {

/***************************************************************************************************************************
*   ChiPipeline::ChiPipeline
*
*   @brief
*       Constructor for ChiPipeline
****************************************************************************************************************************/
ChiPipeline::ChiPipeline()
{
    m_createdPipeline = nullptr;
    m_pipelineCreateData = { 0 };
    m_pipelineInfo = { 0 };
    m_cameraId = -1;
    m_selectedSensorMode = nullptr;
    m_pChiModule = ChiModule::GetInstance();
    if (nullptr == m_pChiModule)
    {
        NT_LOG_ERROR("ChiModule could not be instantiated");
    }
}

/***************************************************************************************************************************
*   ChiPipeline::ChiPipeline
*
*   @brief
*       Destructor for ChiPipeline
***************************************************************************************************************************/
ChiPipeline::~ChiPipeline()
{
    m_createdPipeline = nullptr;
    m_pipelineCreateData = { 0 };
    m_pipelineInfo = { 0 };
    m_cameraId = -1;
    m_selectedSensorMode = nullptr;
    m_pChiModule = ChiModule::GetInstance();
    if (nullptr == m_pChiModule)
    {
        NT_LOG_ERROR("ChiModule could not be instantiated");
    }
}

/***************************************************************************************************************************
*   ChiPipeline::Create
*
*   @brief
*       static function to create an instance of the pipeline
*   @param
*        [in]  int                                  cameraId     cameraId associated with the pipeline
*        [in]  CHISENSORMODEINFO*                   sensorMode   sensorMode associated with the pipeline
*        [in]  std::map<StreamUsecases, ChiStream*> streamIdMap  streamId map to determine stream object for target surface
*        [in]  PipelineType                         type         enum to determine which pipeline to create
*   @return
*       ChiPipeline*  instance to the newly created ChiPipeline
***************************************************************************************************************************/
ChiPipeline* ChiPipeline::Create(
    int cameraId,
    CHISENSORMODEINFO* sensorMode,
    std::map<StreamUsecases, ChiStream*> streamIdMap,
    PipelineType type)
{
    ChiPipeline* pNewPipeline = SAFE_NEW() ChiPipeline();
    CDKResult result;

    if (pNewPipeline != nullptr)
    {
        result = pNewPipeline->Initialize(cameraId, sensorMode, streamIdMap, type);
    }
    else
    {
        result = CDKResultEInvalidPointer;
        NT_LOG_ERROR( "Failed to create an instance of Chipipeline");
    }

    if (result == CDKResultSuccess)
    {
        return pNewPipeline;
    }

    if (pNewPipeline != nullptr)
    {
        delete pNewPipeline;
        pNewPipeline = nullptr;
    }
    return nullptr;

}

/***************************************************************************************************************************
*   ChiPipeline::Initialize
*
*   @brief
*       member function to initialize newly created instance of the pipeline
*   @param
*        [in]  int                                  cameraId     cameraId associated with the pipeline
*        [in]  CHISENSORMODEINFO*                   sensorMode   sensorMode associated with the pipeline
*        [in]  std::map<StreamUsecases, ChiStream*> streamIdMap  streamId map to determine stream object for target surface
*        [in]  PipelineType                         type         enum to determine which pipeline to create
*   @return
*       CDKResult  result code to determine outcome of the function
***************************************************************************************************************************/
CDKResult ChiPipeline::Initialize(
    int cameraId,
    CHISENSORMODEINFO* sensorMode,
    std::map<StreamUsecases, ChiStream*> streamIdMap,
    PipelineType type)
{
    m_cameraId = cameraId;
    m_selectedSensorMode = sensorMode;
    m_pipelineCreateData = { 0 };

    if (nullptr == m_pChiModule)
    {
        return CDKResultEInvalidPointer;
    }

    m_PipelineUtils.m_socId = GetSocID();

    CDKResult result = m_PipelineUtils.SetupPipeline(streamIdMap, &m_pipelineCreateData, type);
    m_pipelineCreateData.pPipelineCreateDescriptor->cameraId = cameraId;
    ChiMetadataUtil* m_pMetadataUtil = ChiMetadataUtil::GetInstance();

    if (m_pMetadataUtil == nullptr)
    {
        return CDKResultEInvalidPointer;
    }

    CHIMETAHANDLE m_pDefaultMetaHandle;
    result |= m_pMetadataUtil->CreateDefaultMetadata(m_cameraId, &m_pDefaultMetaHandle);

    if (result == CDKResultSuccess)
    {
        m_pipelineCreateData.pPipelineCreateDescriptor->hPipelineMetadata = m_pDefaultMetaHandle;
    }

    return result;
}

/***************************************************************************************************************************
*   ChiPipeline::GetSocID
*
*   @brief
*       member function to get the SoC ID from the platform
*   @param
*       None
*   @return
*       INT32  SoC ID
***************************************************************************************************************************/
INT32 ChiPipeline::GetSocID()
{
    INT32  socFileHandle;
    CHAR   socBuffer[SoCBufferSize] = { 0 };
    UINT   socID = 0;
    INT    result  = 0;

    // open the SOC file on device and query
    socFileHandle = open(SoCDeviceName, O_RDONLY);

    if (0 < socFileHandle)
    {
        result = read(socFileHandle, socBuffer, sizeof(socBuffer) - 1);

        if (-1 == result)
        {
            NT_LOG_ERROR("Unable to read SoC ID !");
        }
        else
        {
            socID = atoi(socBuffer);
        }

        close(socFileHandle);
    }

    return socID;
}

/***************************************************************************************************************************
*   ChiPipeline::CreatePipeline
*
*   @brief
*       Create pipeline based on given requirements
*   @return
*       CDKResult    success or failure based on pipeline create
***************************************************************************************************************************/
CDKResult ChiPipeline::CreatePipelineDesc()
{
    NT_LOG_INFO( "Creating pipeline %s descriptor on context %p", m_pipelineCreateData.pPipelineName, m_pChiModule->GetContext());
    m_createdPipeline = m_pChiModule->GetChiOps()->pCreatePipelineDescriptor(
        m_pChiModule->GetContext(),
        m_pipelineCreateData.pPipelineName,
        m_pipelineCreateData.pPipelineCreateDescriptor,
        m_pipelineCreateData.numOutputs,
        m_pipelineCreateData.pOutputDescriptors,
        m_pipelineCreateData.numInputs,
        m_pipelineCreateData.pInputBufferRequirements);

    NT_LOG_DEBUG( "Returned pipeline descriptor %p", m_createdPipeline);

    if (m_createdPipeline == nullptr)
    {
        NT_LOG_ERROR( "Pipeline creation failed!");
        return CDKResultEInvalidPointer;
    }

    m_pipelineInfo.hPipelineDescriptor = m_createdPipeline;
    m_pipelineInfo.pipelineOutputInfo.hPipelineHandle = reinterpret_cast<CHIPIPELINEHANDLE>(m_createdPipeline);

    if (m_pipelineCreateData.pPipelineCreateDescriptor->isRealTime)
    {
        m_pipelineInfo.pipelineInputInfo.isInputSensor = true;
        m_pipelineInfo.pipelineInputInfo.sensorInfo.cameraId = m_cameraId;
        m_pipelineInfo.pipelineInputInfo.sensorInfo.pSensorModeInfo = m_selectedSensorMode;
    }
    else
    {
        m_pipelineInfo.pipelineInputInfo.isInputSensor = false;
        m_pipelineInfo.pipelineInputInfo.inputBufferInfo.numInputBuffers = m_pipelineCreateData.numInputs;
        m_pipelineInfo.pipelineInputInfo.inputBufferInfo.pInputBufferDescriptors = m_pipelineCreateData.pInputDescriptors;
    }

    return CDKResultSuccess;
}

CHIPIPELINEINFO ChiPipeline::GetPipelineInfo() const
{
    return m_pipelineInfo;
}

/***************************************************************************************************************************
*   ChiPipeline::ActivatePipeline
*
*   @brief
*       Activate pipeline with given sensor mode info
*   @param
*       None
*   @return
*       CDKResult   success or failure based on outcome of API call
***************************************************************************************************************************/
CDKResult ChiPipeline::ActivatePipeline(CHIHANDLE hSession) const
{
    NT_LOG_INFO( "Activate pipeline %p", m_createdPipeline);
    //Make sure pipeline is created before activating
    return (m_pChiModule->GetChiOps()->pActivatePipeline(ChiModule::GetInstance()->GetContext(),
        hSession,
        m_createdPipeline,
        m_selectedSensorMode));
}

/***************************************************************************************************************************
*   ChiPipeline::DeactivatePipeline
*
*   @brief
*       Deactivate pipeline
*   @return
*       CDKResult   success or failure based on outcome of API call
***************************************************************************************************************************/
CDKResult ChiPipeline::DeactivatePipeline(CHIHANDLE hSession) const
{
    NT_LOG_INFO( "Deactivate pipeline %p", m_createdPipeline);
    return(m_pChiModule->GetChiOps()->pDeactivatePipeline(m_pChiModule->GetContext(),
        hSession,
        m_createdPipeline,
        CHIDeactivateModeDefault));
}

/***************************************************************************************************************************
*   ChiPipeline::DestroyPipeline
*
*   @brief
*       Destroy created pipeline
*   @return
*       Void
***************************************************************************************************************************/
void ChiPipeline::DestroyPipeline()
{
    //Destroy memory allocated in PipelineUtils
    m_CommonPipelineUtils.DestroyNodePorts(&m_pipelineCreateData);
    if (m_createdPipeline != nullptr)
    {
        NT_LOG_INFO( "Destroying pipeline %p", m_createdPipeline);
        m_pChiModule->GetChiOps()->pDestroyPipelineDescriptor(m_pChiModule->GetContext(),
            m_createdPipeline);
        m_createdPipeline = nullptr;
    }

    delete this;
}

/***************************************************************************************************************************
*   ChiPipeline::GetPipelineHandle
*
*   @brief
*       return the created pipeline handle
*   @return
*       CHIPIPELINEHANDLE created pipeline
***************************************************************************************************************************/
CHIPIPELINEHANDLE ChiPipeline::GetPipelineHandle() const
{
    return reinterpret_cast<CHIPIPELINEHANDLE>(m_createdPipeline);
}
}
