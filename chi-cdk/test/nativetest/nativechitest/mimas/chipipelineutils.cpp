//*******************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************************

#include "chipipelineutils.h"
#include "commonchipipelineutils.h"
#include "camera3stream.h"
#include "cmdlineparser.h"

namespace chitests {

    /**************************************************************************************************
    *   PipelineUtils::SetupPipeline
    *
    *   @brief
    *       Setup Pipeline create parameters for a given usecase
    *   @param
    *       [in]  BufferManager *                      manager                Pointer to buffer manager
    *       [out] PipelineCreateData*                  pPipelineCreate        Pointer to PipelineCreateData
    *       [in]  PipelineType                         type                   PipeLine type for this usecase
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineCreateData* pPipelineCreate, chitests::PipelineType type)
    {
        m_pPipelineCreateData = pPipelineCreate;
        m_totalOutputStreams  = getTotalOutputStreams(streamIdMap);
        CDKResult result      = CDKResultSuccess;

        switch (type)
        {
            case PipelineType::RealtimeTFERDI0:
            case PipelineType::RealtimeTFERDI1RawMIPI:
            case PipelineType::RealtimeTFERDI1RawPlain16:
                result = SetupRealtimePipelineTFERDI(streamIdMap, type);
                break;
            case PipelineType::RealtimeTFEGeneric:
            case PipelineType::RealtimeTFERDIOutMIPI12:
            case PipelineType::RealtimeTFEGeneric2Stream:
                result = SetupRealtimeTFEGeneric(streamIdMap, type);
                break;
            case PipelineType::RealtimeTFE:
            case PipelineType::RealtimeTFE3A:
                result = SetupRealtimePipelineTFE(streamIdMap, type);
                break;
            case PipelineType::RealtimeTFEStats:
                result = SetupRealtimePipelineTFEStats(streamIdMap, type);
                break;
            case PipelineType::RealtimePrevWithMemcpyTFEOPE:
            case PipelineType::RealtimePrevFromMemcpyTFEOPE:
            case PipelineType::RealtimeBypassNodeTFEOPE:
                result = SetupRealtimePipelineExternalTFEOPE(streamIdMap, type);
                break;
            case PipelineType::OfflineJPEGSWSnapshot:
                result = SetupOfflinePipelineJPEGSW(streamIdMap);
                break;
            case PipelineType::RealtimePrevRDITFEOPE:
                result = SetupRealtimePipelinePrevRDITFEOPE(streamIdMap, type);
                break;
            case PipelineType::OfflineOPEGeneric:
            case PipelineType::OfflineOPEGeneric2StreamDisplay:
            case PipelineType::OfflineOPEGeneric3StreamDisplayStats:
                result = SetupOfflineOPEGeneric(streamIdMap, type);
                break;
            case PipelineType::RealtimeOPE:
                result = SetupRealtimePipelineOPE(streamIdMap, type);
                break;
            case PipelineType::OfflineOPEDisplayOnNV12:
            case PipelineType::OfflineOPEDisplayOnNV21:
                result = SetupOfflinePipelineOPEDisplay(streamIdMap, type);
                break;
            case PipelineType::OfflineOPEVideoNV12:
            case PipelineType::OfflineOPEVideoNV21:
                result = SetupOfflinePipelineOPEVideo(streamIdMap, type);
                break;
            case PipelineType::OfflineOPEInputConfigOutNV12:
            case PipelineType::OfflineOPEInputConfigOutNV21:
                result = SetupOfflinePipelineOPERaw(streamIdMap, type);
                break;
            case PipelineType::OfflineOPEDisplayNV21VideoNV12:
            case PipelineType::OfflineOPEDisplayNV12VideoNV21:
            case PipelineType::OfflineOPEDispVideoNV12:
            case PipelineType::OfflineOPEDispVideoNV21:
                result = SetupOfflinePipelineOPEDisplayVideo(streamIdMap, type);
                break;
            case PipelineType::OfflineOPEJPEGSWSnapshot:
                result = SetupOfflinePipelineOPEJPEGSW(streamIdMap);
                break;
            case PipelineType::OfflineOPEStats:
                result = SetupOfflinePipelineOPEStats(streamIdMap, type);
                break;
            case PipelineType::PreviewJpegSnapshot:
            case PipelineType::PreviewYuvSnapshot:
            case PipelineType::PreviewJpegSnapshot3AStats:
            case PipelineType::PreviewYuvSnapshot3AStats:
                result = SetupE2EZSLPreviewSnapshot(streamIdMap, type);
                break;
            case PipelineType::LiveVideoSnapshot:
                result = SetupE2EPreviewLiveVideoSnapshot(streamIdMap, type);
                break;
            default:
                result = CDKResultEInvalidArg;
                NT_LOG_ERROR("Invalid pipeline type: [%d]", type);
                return result;
        }

        m_pPipelineCreateData->pInputBufferRequirements  = m_pipelineMultiInputBufferRequirements;
        m_pPipelineCreateData->pInputDescriptors         = m_pipelineMultiInputBuffer;
        m_pPipelineCreateData->pOutputDescriptors        = m_pPipelineOutputBuffer;
        m_pPipelineCreateData->pPipelineCreateDescriptor = &m_pipelineDescriptor;
        m_pPipelineCreateData->pPipelineName             = m_pipelineName;

        pPipelineCreate = m_pPipelineCreateData;
        return result;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimeTFEGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic TFE tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimeTFEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;
        uint32_t CONFIG_INDEX = 0;

        // Select usecase and target create descriptor based on pipeline type
        switch (type)
        {
        case PipelineType::RealtimeTFEGeneric:
            selectUsecase = Usecases2Target[UsecaseMimasGenericId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[TFEGeneric];
            CONFIG_INDEX = 1;
            break;
        case PipelineType::RealtimeTFEGeneric2Stream:
            selectUsecase = Usecases3Target[UsecaseMimasGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[TFEGeneric2Stream];
            CONFIG_INDEX = 1;
            break;
        case PipelineType::RealtimeTFERDIOutMIPI12:
            selectUsecase = Usecases1Target[UsecaseMimasTFERDIOutMIPI12Id];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[TFERDIOutMIPI12];
            CONFIG_INDEX = 1;
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime TFE Generic pipeline type given : %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        m_pipelineDescriptor           = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName                 = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo       = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo       = selectTargetCreateDescriptor.sinkTarget;

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_OUTPUT");
            return CDKResultENoSuch;
        }

        CSLTFEOutputPortId selectTfePort = CommonPipelineUtils::SelectTfePortByOutputFormat(
            streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format);

        // Edit the pipeline descriptor with selected output port
        // TODO intelligently find CONFIG_INDEX by going through links, find where source is TFE (what we want based on port
        // and instance)
        // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
        // buffers). Here it is the first output (0) buffer in all cases
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectTfePort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId = selectTfePort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectTfePort;

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectTfePort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        if (type == PipelineType::RealtimeTFEGeneric2Stream)
        {
            // Setup static full output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdFull;
            if (streamIdMap.find(StreamUsecases::TFEOutputPortFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortFull");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortFull];
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineOPEGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic OPE tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineOPEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;
        uint32_t CONFIG_INDEX = 1;

        switch (type)
        {
        case PipelineType::OfflineOPEGeneric:
            selectUsecase = Usecases2Target[UsecaseMimasGenericId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[OPEGeneric];
            break;
        case PipelineType::OfflineOPEGeneric2StreamDisplay:
            selectUsecase = Usecases3Target[UsecaseMimasGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[OPEGeneric2StreamDisplay];
            break;
        case PipelineType::OfflineOPEGeneric3StreamDisplayStats:
            selectUsecase = Usecases3Target[UsecaseMimasGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[OPEGeneric3StreamDisplayStats];
            break;
        default:
            NT_LOG_ERROR( "Invalid Offline OPE Generic pipeline type given : %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        m_pipelineDescriptor     = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName           = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo = selectTargetCreateDescriptor.sinkTarget;

        InitPortBufferDescriptor();

        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_OUTPUT");
            return CDKResultENoSuch;
        }

        CSLOPEOutputPortId selectOpeOutputPort = CommonPipelineUtils::SelectOpePortByOutputFormat(
            streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format);

        // Edit the pipeline descriptor with selected output port
        // TODO intelligently find CONFIG_INDEX by going through links, find where source is OPE (what we want based on port
        // and instance)
        // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
        // buffers). Here it is the first output (0) buffer in all cases
        // NOTE: 2 link barring source link (though it is 3rd link), where source is input buffer to OPE, first link is listed
        // in source links, refer to g_testpipelines.h
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectOpeOutputPort;
        // node position in nodes list
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId = selectOpeOutputPort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectOpeOutputPort;

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = CONFIG_INDEX;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectOpeOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        if (type == PipelineType::OfflineOPEGeneric2StreamDisplay ||
            type == PipelineType::OfflineOPEGeneric3StreamDisplayStats)
        {
            // Setup static output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDVideo;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortVideo];
        }

        if (type == PipelineType::OfflineOPEGeneric3StreamDisplayStats)
        {
            // Setup static output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortDisplay];
        }

        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }

        CSLOPEInputPortId selectOpeInputPort;

        // If user provided an input port, override the default from format
        int overrideOpeInputPort = CmdLineParser::GetInputPort();
        if (INVALID_PORT != overrideOpeInputPort)
        {
            selectOpeInputPort = static_cast<CSLOPEInputPortId>(overrideOpeInputPort);
            NT_LOG_INFO( "Overriding default input port with [%d]", overrideOpeInputPort);
        }
        else
        {
            selectOpeInputPort = CSLOPEInputPortId::CSLOPEInputPortIDFull;
            NT_LOG_INFO("Default input port for format %d is [%d]", streamIdMap[StreamUsecases::CONFIG_INPUT]->format,
                selectOpeInputPort);
        }

        // Edit the pipeline descriptor with selected input port
        //TODO intelligently identify nodes and target descriptors
        m_pipelineDescriptor.pNodes[0].nodeAllPorts.pInputPorts[0].portId = selectOpeInputPort; // 1st OPE node
        m_pipelineDescriptor.pNodes[1].nodeAllPorts.pInputPorts[0].portId = selectOpeInputPort; // 2nd OPE node
        selectTargetCreateDescriptor.sourceTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectOpeInputPort;

        // Setup input buffer
        for (UINT32 inputBufferIndex = 0; inputBufferIndex < m_pipelineSrcTgtDescInfo.numTargets; inputBufferIndex++)
        {
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_INPUT];

            for (UINT32 inputNodeIndex = 0; inputNodeIndex < m_pipelineSrcTgtDescInfo.pTargetPortDesc->numNodePorts;inputNodeIndex++)
            {
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeId         = CamX::OPE;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeInstanceId = inputNodeIndex;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodePortId     = selectOpeInputPort;

                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeId         = CamX::OPE;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeInstanceId = inputNodeIndex;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodePortId     = selectOpeInputPort;

                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.width        = 3840;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.height       = 2160;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.width        = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.height       = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.width    = 1920;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.height   = 1080;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            }
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineTFEStats
    *
    *   @brief
    *       Setup Pipeline parameters for real time tfe usecase with all stats ports
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineTFEStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        switch (type)
        {
        case PipelineType::RealtimeTFEStats:
            m_pipelineDescriptor = Usecases6Target[TestTFEFullStatsId].pPipelineTargetCreateDesc[TFEFullStats].pipelineCreateDesc;
            m_pipelineName = Usecases6Target[TestTFEFullStatsId].pPipelineTargetCreateDesc[TFEFullStats].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases6Target[TestTFEFullStatsId].pPipelineTargetCreateDesc[TFEFullStats].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases6Target[TestTFEFullStatsId].pPipelineTargetCreateDesc[TFEFullStats].sinkTarget;
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdFull;
        if (streamIdMap.find(StreamUsecases::TFEOutputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortFull");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortFull];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdStatAECBG;
        if (streamIdMap.find(StreamUsecases::TFEOutputPortStatsAECBG) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortStatsAECBG");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortStatsAECBG];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdStatBHIST;
        if (streamIdMap.find(StreamUsecases::TFEOutputPortStatsBHIST) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortStatsBHIST");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortStatsBHIST];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdStatTLBG;
        if (streamIdMap.find(StreamUsecases::TFEOutputPortStatsTLBG) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortStatsTLBG");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortStatsTLBG];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdStatBF;
        if (streamIdMap.find(StreamUsecases::TFEOutputPortStatsBAF) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortStatsBAF");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortStatsBAF];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLTFEOutputPortId::CSLTFEPortIdStatAWBBG;
        if (streamIdMap.find(StreamUsecases::TFEOutputPortStatsAWBBG) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for TFEOutputPortStatsAWBBG");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortStatsAWBBG];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineTFERDI
    *
    *   @brief
    *       Setup Pipeline parameters for real time TFE Raw RDI usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineTFERDI(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        CSLTFEOutputPortId selectedTfePort;
        UINT32 outputBufferIndex = 0;
        switch (type)
        {
        case PipelineType::RealtimeTFERDI0:
            if (streamIdMap.find(StreamUsecases::TFEOutputPortRDI0) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for TFEOutputPortRDI0");
                return CDKResultENoSuch;
            }
            m_pipelineDescriptor = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI0].pipelineCreateDesc;
            m_pipelineName       = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI0].pPipelineName;

            // source and destination information
            m_pipelineSrcTgtDescInfo  = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI0].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI0].sinkTarget;

            selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdRDI0;
            break;
        case PipelineType::RealtimeTFERDI1RawMIPI:
        case PipelineType::RealtimeTFERDI1RawPlain16:
            if (streamIdMap.find(StreamUsecases::TFEOutputPortRDI1) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for TFEOutputPortRDI1");
                return CDKResultENoSuch;
            }
            if(type == PipelineType::RealtimeTFERDI1RawMIPI){
                m_pipelineDescriptor = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI1Raw].
                    pipelineCreateDesc;
                m_pipelineName = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI1Raw].pPipelineName;

                // source and destination information
                m_pipelineSrcTgtDescInfo  = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[
                    TFERDI1Raw].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[
                    TFERDI1Raw].sinkTarget;
            }
            else if(type == PipelineType::RealtimeTFERDI1RawPlain16){
                m_pipelineDescriptor = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[
                    TFERDI1Raw].pipelineCreateDesc;
                m_pipelineName = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[TFERDI1Raw].pPipelineName;

                // source and destination information
                m_pipelineSrcTgtDescInfo  = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[
                    TFERDI1Raw].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseMimasRawId].pPipelineTargetCreateDesc[
                    TFERDI1Raw].sinkTarget;
            }
            selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdRDI1;
            break;
        default:
            NT_LOG_ERROR("Invalid Realtime TFE RDI pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectedTfePort;

        if (type == PipelineType::RealtimeTFERDI0)
        {
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortRDI0];
        }
        else if(type == PipelineType::RealtimeTFERDI1RawMIPI || type == PipelineType::RealtimeTFERDI1RawPlain16)
        {
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::TFEOutputPortRDI1];
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineTFE
    *
    *   @brief
    *       Setup Pipeline parameters for real time tfe usecase
    *   @param     *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineTFE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        StreamUsecases activeStreamName;
        uint32_t CONFIG_INDEX = 0;

        // First search if correct stream name present, else return
        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) != streamIdMap.end())          // MultiCameraTest
        {
            activeStreamName = StreamUsecases::CONFIG_OUTPUT;
        }
        else if (streamIdMap.find(StreamUsecases::TFEOutputPortFull) != streamIdMap.end())   // RealtimePipelineTest
        {
            activeStreamName = StreamUsecases::TFEOutputPortFull;
        }
        else
        {
            NT_LOG_WARN("Cannot find chistream for CONFIG_OUTPUT or TFEOutputPortFull");
            return CDKResultENoSuch;
        }

        ChiUsecase                        selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        switch(type)
        {
            case PipelineType::RealtimeTFE:
                selectUsecase                = Usecases1Target[TestTFEFullId];
                selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[TFEFull];
                CONFIG_INDEX = 1;
                break;
            case PipelineType::RealtimeTFE3A:
                selectUsecase                = Usecases1Target[TestZSLUssecaseMimasId];
                selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[TFEFull3A];
                CONFIG_INDEX = 1;
                break;
            default:
                NT_LOG_ERROR("Invalid Realtimepipeline type given: %d", static_cast<int>(type));
                return CDKResultEInvalidArg;
        }

        m_pipelineDescriptor     = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName           = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo = selectTargetCreateDescriptor.sinkTarget;
        CSLTFEOutputPortId   selectTfePort = CSLTFEOutputPortId::CSLTFEPortIdFull;   //default

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        if (activeStreamName == StreamUsecases::CONFIG_OUTPUT)  //configurable for Multicamera
        {
            CSLTFEOutputPortId selectTfePort = CommonPipelineUtils::SelectTfePortByOutputFormat(
            streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format);

            // Edit the pipeline descriptor with selected output port
            // TODO intelligently find CONFIG_INDEX by going through links, find where source is TFE (what we want based on
            // port and instance)
            // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
            // buffers). Here it is the first output (0) buffer in all cases
            m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId                     = selectTfePort;
            m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId    = selectTfePort;
            selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectTfePort;
        }

       //Setup output Buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectTfePort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[activeStreamName];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineExternalTFEOPE
    *
    *   @brief
    *       Setup Pipeline parameters for real time memcpy usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineExternalTFEOPE(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }

        if (PipelineType::RealtimePrevWithMemcpyTFEOPE == type || PipelineType::RealtimeBypassNodeTFEOPE == type)
        {
            if (PipelineType::RealtimePrevWithMemcpyTFEOPE == type)
            {
                m_pipelineDescriptor = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[PreviewWithMemcpyTFEOPE].
                    pipelineCreateDesc;
                m_pipelineName = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[PreviewWithMemcpyTFEOPE].
                    pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[
                    PreviewWithMemcpyTFEOPE].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[
                    PreviewWithMemcpyTFEOPE].sinkTarget;
            }
            if (PipelineType::RealtimeBypassNodeTFEOPE == type)
            {
                m_pipelineDescriptor = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[BypassNodeTFEOPE].
                    pipelineCreateDesc;
                m_pipelineName = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[BypassNodeTFEOPE].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[
                    BypassNodeTFEOPE].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[
                    BypassNodeTFEOPE].sinkTarget;
            }
            InitPortBufferDescriptor();
            m_pPipelineOutputBuffer[0].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[0].pNodePort->nodeId = CamX::OPE;
            m_pPipelineOutputBuffer[0].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[0].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
            m_pPipelineOutputBuffer[0].pStream = streamIdMap[StreamUsecases::PREVIEW];
        }
        else if (PipelineType::RealtimePrevFromMemcpyTFEOPE == type)
        {
            m_pipelineDescriptor = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[PreviewFromMemcpyTFEOPE].
                pipelineCreateDesc;
            m_pipelineName = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[PreviewFromMemcpyTFEOPE].
                pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[
                PreviewFromMemcpyTFEOPE].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases1Target[TestMemcpyTFEOPEId].pPipelineTargetCreateDesc[
                PreviewFromMemcpyTFEOPE].sinkTarget;
            InitPortBufferDescriptor();
            m_pPipelineOutputBuffer[0].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[0].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
            m_pPipelineOutputBuffer[0].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[0].pNodePort->nodePortId = externalportid_t::ChiNodeOutputPort0;
            m_pPipelineOutputBuffer[0].pStream = streamIdMap[StreamUsecases::PREVIEW];
        }
        else
        {
            NT_LOG_ERROR( "Invalid external pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineJPEGSW
    *
    *   @brief
    *       Setup Pipeline parameters for mimas offline jpeg snapshot usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineJPEGSW(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {

        m_pipelineDescriptor = Usecases2Target[TestOPEOfflineJPEGSWId].pPipelineTargetCreateDesc[OPEJPEGSWSnapshot].
            pipelineCreateDesc;
        m_pipelineName = Usecases2Target[TestOPEOfflineJPEGSWId].pPipelineTargetCreateDesc[OPEJPEGSWSnapshot].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOPEOfflineJPEGSWId].pPipelineTargetCreateDesc[
            OPEJPEGSWSnapshot].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOPEOfflineJPEGSWId].pPipelineTargetCreateDesc[
            OPEJPEGSWSnapshot].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::JPEGSW;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = jpegswportid_t::JPEGSWInputPortMain;
        if (streamIdMap.find(StreamUsecases::JPEGSWInputPortMain) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for JPEGSWInputPortMain");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::JPEGSWInputPortMain];

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
        if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for SNAPSHOT");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::JPEGSW;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = jpegswportid_t::JPEGSWInputPortMain;

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 4096;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
        return CDKResultSuccess;
    }

   /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineOPEVideo
    *
    *   @brief
    *       Setup Pipeline parameters for offline OPE video port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineOPEVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::OPEOutputPortVideo) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortVideo");
            return CDKResultENoSuch;
        }

        if (streamIdMap.find(StreamUsecases::OPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEInputPortFull");
            return CDKResultENoSuch;
        }

        if (type == PipelineType::OfflineOPEVideoNV12)
        {
            m_pipelineDescriptor = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV12].pipelineCreateDesc;
            m_pipelineName = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV12].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV12].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV12].sinkTarget;
        }
        else if (type == PipelineType::OfflineOPEVideoNV21)
        {
            m_pipelineDescriptor = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV21].pipelineCreateDesc;
            m_pipelineName = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV21].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV21].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OpeVideoNV21].sinkTarget;
        }
        else
        {
            NT_LOG_ERROR( "Invalid Offline pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();

        uint32_t outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDVideo;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortVideo];
        UINT32 inputBufferIndex = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEInputPortFull];
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::OPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelinePrevRDITFEOPE
    *
    *   @brief
    *       Setup Pipeline parameters for real time preview usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelinePrevRDITFEOPE(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        // First check if streams setup
        if (streamIdMap.find(StreamUsecases::TFEOutputPortRDI0) == streamIdMap.end())
        {
            NT_LOG_ERROR("cannot find chistream for TFEOutputPortRDI0");
            return CDKResultENoSuch;
        }

        // Get pipeline descriptor
        switch (type)
        {
        case PipelineType::RealtimePrevRDITFEOPE:
            m_pipelineDescriptor = Usecases2Target[UsecaseRealtimePrevTFEOPEId].pPipelineTargetCreateDesc[PrevRDITFEOPE].
                pipelineCreateDesc;
            m_pipelineName       = Usecases2Target[UsecaseRealtimePrevTFEOPEId].pPipelineTargetCreateDesc[PrevRDITFEOPE].
                pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[UsecaseRealtimePrevTFEOPEId].pPipelineTargetCreateDesc[
                PrevRDITFEOPE].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[UsecaseRealtimePrevTFEOPEId].pPipelineTargetCreateDesc[
                PrevRDITFEOPE].sinkTarget;
            break;
        default:
            NT_LOG_ERROR("Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }
        // Setup output buffer for pipelines
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::TFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = CSLTFEOutputPortId::CSLTFEPortIdRDI0;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::TFEOutputPortRDI0];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::OPEOutputPortDisplay];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineOPERaw
    *
    *   @brief
    *       Setup Pipeline parameters for offline OPE fullport usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineOPERaw(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::OPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEInputPortFull");
            return CDKResultENoSuch;
        }

        if (streamIdMap.find(StreamUsecases::OPEOutputPortDisplay) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortDisplay");
            return CDKResultENoSuch;
        }

        if (type == PipelineType::OfflineOPEInputConfigOutNV12)
        {
            m_pipelineDescriptor = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV12].
                pipelineCreateDesc;
            m_pipelineName = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV12].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV12].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV12].sinkTarget;
        }
        else if (type == PipelineType::OfflineOPEInputConfigOutNV21)
        {
            m_pipelineDescriptor = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV21].
                pipelineCreateDesc;
            m_pipelineName = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV21].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV21].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestOfflineOPERawId].pPipelineTargetCreateDesc[OfflineOPERawNV21].sinkTarget;
        }
        else
        {
            NT_LOG_ERROR( "Invalid Offline pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        uint32_t outputBufferIndex = 0;

        for(UINT32 inputBufferIndex = 0; inputBufferIndex < m_pipelineSrcTgtDescInfo.numTargets; inputBufferIndex++)
        {
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEInputPortFull];
            for (UINT32 inputNodeIndex=0; inputNodeIndex < m_pipelineSrcTgtDescInfo.pTargetPortDesc->numNodePorts;
                inputNodeIndex++)
            {
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeId = CamX::OPE;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeInstanceId = inputNodeIndex;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[
                    inputNodeIndex].nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;

                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeId = CamX::OPE;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeInstanceId = inputNodeIndex;
                m_pipelineMultiInputBufferRequirements[
                    inputNodeIndex].nodePort.nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.width = 3840;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.height = 2160;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.width = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.height = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.width = 1920;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.height = 1080;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            }
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortDisplay];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineOPE
    *
    *   @brief
    *       Setup Pipeline parameters for real time OPE usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineOPE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for PREVIEW");
                return CDKResultENoSuch;
            }
        switch(type)
        {
        case PipelineType::RealtimeOPE:
            m_pipelineDescriptor = Usecases2Target[UsecaseOPEId].pPipelineTargetCreateDesc[TestOPEFull].pipelineCreateDesc;
            m_pipelineName = Usecases2Target[UsecaseOPEId].pPipelineTargetCreateDesc[TestOPEFull].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[UsecaseOPEId].pPipelineTargetCreateDesc[TestOPEFull].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[UsecaseOPEId].pPipelineTargetCreateDesc[TestOPEFull].sinkTarget;
            m_pPipelineOutputBuffer[0].pStream = streamIdMap[StreamUsecases::PREVIEW];
            break;
        default:
            NT_LOG_ERROR("Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        m_pPipelineOutputBuffer[0].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[0].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[0].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[0].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineOPEDisplay
    *
    *   @brief
    *       Setup Pipeline parameters for offline OPE display usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineOPEDisplay(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::OPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEInputPortFull");
            return CDKResultENoSuch;
        }
        if (type == PipelineType::OfflineOPEDisplayOnNV12)
        {
            m_pipelineDescriptor = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OPEInputNV12DisplayNV12].
                pipelineCreateDesc;
            m_pipelineName = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OPEInputNV12DisplayNV12].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[
                OPEInputNV12DisplayNV12].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[
                OPEInputNV12DisplayNV12].sinkTarget;
        }
        else if (type == PipelineType::OfflineOPEDisplayOnNV21)
        {
            m_pipelineDescriptor = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OPEInputNV12DisplayNV21].
                pipelineCreateDesc;
            m_pipelineName = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[OPEInputNV12DisplayNV21].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[
                OPEInputNV12DisplayNV21].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestOfflineOPEId].pPipelineTargetCreateDesc[
                OPEInputNV12DisplayNV21].sinkTarget;
        }
        else
        {
            NT_LOG_ERROR( "Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEInputPortFull];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::OPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineOPEDisplayVideo
    *
    *   @brief
    *       Setup Pipeline parameters for offline OPE Display+Video port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineOPEDisplayVideo(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::OPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEInputPortFull");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::OPEOutputPortDisplay) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortDisplay");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::OPEOutputPortVideo) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortVideo");
            return CDKResultENoSuch;
        }
        if (type == PipelineType::OfflineOPEDisplayNV12VideoNV21)
        {
            m_pipelineDescriptor = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV12VideoNV21].pipelineCreateDesc;
            m_pipelineName = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV12VideoNV21].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV12VideoNV21].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV12VideoNV21].sinkTarget;
        }
        else if (type == PipelineType::OfflineOPEDisplayNV21VideoNV12)
        {
            m_pipelineDescriptor = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV21VideoNV12].pipelineCreateDesc;
            m_pipelineName = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV21VideoNV12].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV21VideoNV12].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[TestOPEInputNV12DisplayNV21VideoNV12].sinkTarget;
        }
        else if (type == PipelineType::OfflineOPEDispVideoNV12)
        {
            m_pipelineDescriptor = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV12].pipelineCreateDesc;
            m_pipelineName = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV12].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV12].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV12].sinkTarget;
        }
        else if (type == PipelineType::OfflineOPEDispVideoNV21)
        {
            m_pipelineDescriptor = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV21].pipelineCreateDesc;
            m_pipelineName = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV21].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV21].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases5Target[UsecaseOfflineOPEDisplayVideoId].
                pPipelineTargetCreateDesc[OPEOfflineDispVideoNV21].sinkTarget;
        }
        else
        {
            NT_LOG_ERROR( "Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        uint32_t outputBufferIndex = 0;
        uint32_t inputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortDisplay];

        // OPE Video --> Sink Buffer
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDVideo;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortVideo];

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEInputPortFull];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::OPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLOPEInputPortId::CSLOPEInputPortIDFull;

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineOPEJPEGSW
    *
    *   @brief
    *       Setup Pipeline parameters for offline OPE Jpegsw port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineOPEJPEGSW(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for SNAPSHOT");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::OPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEInputPortFull");
            return CDKResultENoSuch;
        }

        m_pipelineDescriptor = Usecases3Target[UsecaseOfflineOPEJPEGSWId].pPipelineTargetCreateDesc[OfflineOPEJPEGSW].pipelineCreateDesc;
        m_pipelineName = Usecases3Target[UsecaseOfflineOPEJPEGSWId].pPipelineTargetCreateDesc[OfflineOPEJPEGSW].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases3Target[UsecaseOfflineOPEJPEGSWId].pPipelineTargetCreateDesc[OfflineOPEJPEGSW].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases3Target[UsecaseOfflineOPEJPEGSWId].pPipelineTargetCreateDesc[OfflineOPEJPEGSW].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];

        for (UINT32 inputBufferIndex = 0; inputBufferIndex < m_pipelineSrcTgtDescInfo.numTargets; inputBufferIndex++)
        {
            m_pipelineMultiInputBuffer[inputBufferIndex].size                                              = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream                                           = streamIdMap[StreamUsecases::OPEInputPortFull];

            for (UINT32 inputNodeIndex = 0; inputNodeIndex < m_pipelineSrcTgtDescInfo.pTargetPortDesc->numNodePorts;inputNodeIndex++)
            {
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeId                = CamX::OPE;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeInstanceId        = inputNodeIndex;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodePortId            = CSLOPEInputPortId::CSLOPEInputPortIDFull;

                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeId                       = CamX::OPE;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeInstanceId               = inputNodeIndex;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodePortId                   = CSLOPEInputPortId::CSLOPEInputPortIDFull;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.width      = 3840;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.height     = 2160;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.width      = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.height     = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.width  = 1920;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.height = 1080;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.size                    = sizeof(CHIBUFFEROPTIONS);
            }
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineOPEStats
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS full + stats port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineOPEStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::OPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEInputPortFull");
            return CDKResultENoSuch;
        }

        if (streamIdMap.find(StreamUsecases::OPEOutputPortDisplay) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortDisplay");
            return CDKResultENoSuch;
        }

        if (streamIdMap.find(StreamUsecases::OPEOutputPortStatsRS) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortStatsRS");
            return CDKResultENoSuch;
        }

        if (streamIdMap.find(StreamUsecases::OPEOutputPortStatsIHIST) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for OPEOutputPortStatsIHIST");
            return CDKResultENoSuch;
        }

        m_pipelineDescriptor = Usecases4Target[OfflineOPEStatsId].pPipelineTargetCreateDesc[OPEStats].pipelineCreateDesc;
        m_pipelineName = Usecases4Target[OfflineOPEStatsId].pPipelineTargetCreateDesc[OPEStats].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases4Target[OfflineOPEStatsId].pPipelineTargetCreateDesc[OPEStats].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases4Target[OfflineOPEStatsId].pPipelineTargetCreateDesc[OPEStats].sinkTarget;

        InitPortBufferDescriptor();

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortDisplay];

        // RS Stream
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDStatRS;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortStatsRS];

        //IHist Stream
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDStatIHIST;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::OPEOutputPortStatsIHIST];

        for (UINT32 inputBufferIndex = 0; inputBufferIndex < m_pipelineSrcTgtDescInfo.numTargets; inputBufferIndex++)
        {
            m_pipelineMultiInputBuffer[inputBufferIndex].size                                              = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream                                           = streamIdMap[StreamUsecases::OPEInputPortFull];

            for (UINT32 inputNodeIndex = 0; inputNodeIndex < m_pipelineSrcTgtDescInfo.pTargetPortDesc->numNodePorts;inputNodeIndex++)
            {
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeId                = CamX::OPE;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodeInstanceId        = inputNodeIndex;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort[inputNodeIndex].nodePortId            = CSLOPEInputPortId::CSLOPEInputPortIDFull;

                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeId                       = CamX::OPE;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodeInstanceId               = inputNodeIndex;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].nodePort.nodePortId                   = CSLOPEInputPortId::CSLOPEInputPortIDFull;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.width      = 3840;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.maxDimension.height     = 2160;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.width      = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.minDimension.height     = 0;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.width  = 1920;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.optimalDimension.height = 1080;
                m_pipelineMultiInputBufferRequirements[inputNodeIndex].bufferOptions.size                    = sizeof(CHIBUFFEROPTIONS);
            }
        }

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupE2EZSLPreviewSnapshot
    *
    *   @brief
    *       Setup pipeline parameters for E2E Preview + Snapshot tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupE2EZSLPreviewSnapshot(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for SNAPSHOT");
            return CDKResultENoSuch;
        }
        ChiUsecase UsecaseE2EPreviewSnapshotMimas = Usecases3Target[UsecaseE2EPreviewSnapshotMimasId];
        switch (type)
        {
        case PipelineType::PreviewJpegSnapshot:
        case PipelineType::PreviewJpegSnapshot3AStats:
            m_pipelineDescriptor = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewJpegSnapshotOPE].pipelineCreateDesc;
            m_pipelineName = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewJpegSnapshotOPE].pPipelineName;
            m_pipelineSrcTgtDescInfo = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewJpegSnapshotOPE].sourceTarget;
            m_pipelineSnkTgtDescInfo = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewJpegSnapshotOPE].sinkTarget;
            break;
        case PipelineType::PreviewYuvSnapshot:
        case PipelineType::PreviewYuvSnapshot3AStats:
            m_pipelineDescriptor = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewYuvSnapshotOPE].pipelineCreateDesc;
            m_pipelineName = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewYuvSnapshotOPE].pPipelineName;
            m_pipelineSrcTgtDescInfo = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewYuvSnapshotOPE].sourceTarget;
            m_pipelineSnkTgtDescInfo = UsecaseE2EPreviewSnapshotMimas.pPipelineTargetCreateDesc[PreviewYuvSnapshotOPE].sinkTarget;
            break;
        default:
            NT_LOG_ERROR("Invalid UsecaseE2EPreviewSnapshot pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];

        outputBufferIndex++;

        switch (type)
        {
        case PipelineType::PreviewJpegSnapshot:
        case PipelineType::PreviewJpegSnapshot3AStats:
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];
            break;
        case PipelineType::PreviewYuvSnapshot:
        case PipelineType::PreviewYuvSnapshot3AStats:
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 2;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];
            break;
        default:
            NT_LOG_ERROR("Invalid UsecaseE2EPreviewSnapshot pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupE2EPreviewLiveVideoSnapshot
    *
    *   @brief
    *       Setup Pipeline parameters for realtime preview callback snapshot & video
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupE2EPreviewLiveVideoSnapshot(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for SNAPSHOT");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::VIDEO) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for VIDEO");
            return CDKResultENoSuch;
        }

        ChiUsecase UsecaseE2ELiveVideoSnapshotMimas = Usecases4Target[UsecaseE2ELiveVideoSnapshotMimasId];
        UINT32 outputBufferIndex = 0;

        switch (type)
        {
        case PipelineType::LiveVideoSnapshot:
            m_pipelineDescriptor = UsecaseE2ELiveVideoSnapshotMimas.pPipelineTargetCreateDesc[LiveVideoSnapshotOPE].pipelineCreateDesc;
            m_pipelineName = UsecaseE2ELiveVideoSnapshotMimas.pPipelineTargetCreateDesc[LiveVideoSnapshotOPE].pPipelineName;
            m_pipelineSrcTgtDescInfo = UsecaseE2ELiveVideoSnapshotMimas.pPipelineTargetCreateDesc[LiveVideoSnapshotOPE].sourceTarget;
            m_pipelineSnkTgtDescInfo = UsecaseE2ELiveVideoSnapshotMimas.pPipelineTargetCreateDesc[LiveVideoSnapshotOPE].sinkTarget;
            break;
        default:
            NT_LOG_ERROR("Invalid UsecaseE2EPreviewSnapshotMimas pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];
        outputBufferIndex++;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::OPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLOPEOutputPortId::CSLOPEOutputPortIDVideo;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::VIDEO];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }
        return CDKResultSuccess;
    }

}
