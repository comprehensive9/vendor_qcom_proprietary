//*******************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************************

#include "chipipelineutils.h"
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
        m_streamIdMapSize = streamIdMap.size();
        m_totalOutputStreams = getTotalOutputStreams(streamIdMap);
        m_pPipelineCreateData = pPipelineCreate;
        CDKResult result = CDKResultSuccess;

        // pipeline types w/ both switch ports [Disp<->Video] [Disp<->Full]
        // should terminate when switch is called to avoid ioctl failure
        switch (type)
        {
            case PipelineType::RealtimeIFEDualFoV:
            case PipelineType::RealtimePrevWith3AWrappers:
            case PipelineType::OfflineIPEDispVideo:
            case PipelineType::OfflineIPEGeneric2StreamDisplay:
            case PipelineType::LiveVideoSnapshot:
            case PipelineType::PreviewCallbackSnapshotThumbnail:
                if (CmdLineParser::GetSwitchPortType() != CmdLineParser::InvalidSwitch)
                {
                    NT_LOG_ERROR("Switch port not possible ! Exiting.");
                    return CDKResultENoSuch;
                }
                break;
            default:
                break;
        }

        switch (type)
        {
            // realtime pipeline
            case PipelineType::RealTimeIPE:
            case PipelineType::RealtimePrevHVX:
                result = SetupRealtimePipelineIPE(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFE:
            case PipelineType::RealtimeIFE3A:
            case PipelineType::RealtimeIFEDualFoV:
                result = SetupRealtimePipelineIFE(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFELiteTLBGStatsOut:
                result = SetupRealtimePipelineIFELite(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFELiteRawTLBGStatsOut:
                result = SetupRealtimePipelineIFELiteRaw(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFEStats:
                result = SetupRealtimePipelineIFEStats(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFERDI0:
                result = SetupRealtimePipelineIFERDIO(streamIdMap);
                break;
            case PipelineType::RealtimeIFERawCamif:
                result = SetupRealtimePipelineIFERawCamif(streamIdMap);
                break;
            case PipelineType::RealtimeIFERawLsc:
                result = SetupRealtimePipelineIFERawLsc(streamIdMap);
                break;
            case PipelineType::RealtimePrevRDI:
                result = SetupRealtimePrevRDI(streamIdMap, type);
                break;
            case PipelineType::RealtimePrevWithCallbackWithStats:
            case PipelineType::RealtimePrevWith3AWrappers:
                result = SetupPreviewCallbackSnapshotWithThumbnail(streamIdMap, type);
                break;
            case PipelineType::RealtimePrevWithMemcpy:
            case PipelineType::RealtimePrevFromMemcpy:
            case PipelineType::RealtimeChiDepth:
            case PipelineType::RealtimeBypassNode:
            case PipelineType::RealtimeInplaceNode:
                result = SetupRealtimePipelineExternal(streamIdMap, type);
                break;
            case PipelineType::RealtimeCVPDME:
            case PipelineType::RealtimeCVPDMEDisplayPort:
            case PipelineType::RealtimeCVPDMEICA:
            case PipelineType::OfflineCVPStandAlone:
                result = SetupCVPPipelines(streamIdMap, type);
                break;
            case PipelineType::RealtimeIPE3A:
                result = SetupRealtimeIPE3A(streamIdMap, type);
                break;
            case PipelineType::RealtimeRDI1Only:
                result = SetupRealtimePIPCam(streamIdMap, type);
                break;
            case PipelineType::RealtimeIPEJpeg:
            case PipelineType::RealtimeBPSIPEJpeg:
                result = SetupRealtimeJpeg(streamIdMap, type);
                break;
            case PipelineType::RealtimeFD:
                result = SetupRealtimeFD(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFEGeneric:
                result = SetupRealtimeIFEGeneric(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFEGeneric2Stream:
                result = SetupRealtimeIFEGeneric(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFEDownscaleGeneric:
                result = SetupRealtimeIFEDownscaleGeneric(streamIdMap, type);
                break;
            case PipelineType::RealtimeIFEGeneric2StreamDisplay:
                result = SetupRealtimeIFEGeneric(streamIdMap, type);
                break;
            case PipelineType::VendorTagWrite:
                result = SetupCustomVendorWritePipeline(streamIdMap);
                break;

            // offline pipeline
            case PipelineType::OfflineIPEJPEG:
                result = SetupOfflinePipelineIPEJPEG(streamIdMap);
                break;
            case PipelineType::OfflineIPEDisp:
                result = SetupOfflinePipelineIPEDisplay(streamIdMap);
                break;
            case PipelineType::OfflineIPEVideo:
                result = SetupOfflinePipelineIPEVideo(streamIdMap);
                break;
            case PipelineType::OfflineIPEDispVideo:
            case PipelineType::OfflineIPEBlurModule:
                result = SetupOfflinePipelineIPEDispVideo(streamIdMap, type);
                break;
            case PipelineType::OfflineIPEMFHDRSnapshot:
            case PipelineType::OfflineIPEMFHDRPreview:
            case PipelineType::OfflineIPEMFHDRSnapshotE2E:
            case PipelineType::OfflineIPEMFHDRPreviewE2E:
                result = SetupPipelineMFHDR(streamIdMap, type);
                break;
            case PipelineType::OfflineBPS:
                result = SetupOfflinePipelineBPS(streamIdMap);
                break;
            case PipelineType::OfflineBPSDS4:
                result = SetupOfflinePipelineBPSDS4(streamIdMap);
                break;
            case PipelineType::OfflineBPSIPE:
                result = SetupOfflinePipelineBPSIPE(streamIdMap);
                break;
            case PipelineType::OfflineBPSIPEJpeg:
                result = SetupOfflinePipelineBPSIPEJpeg(streamIdMap);
                break;
            case PipelineType::OfflineBPSIPEDispVideo:
                result = SetupOfflinePipelineBPSIPEDispVideo(streamIdMap);
                break;
            case PipelineType::OfflineBPSStats:
            case PipelineType::OfflineBPSStatsWithIPE:
                result = SetupOfflinePipelineBPSStats(streamIdMap, type);
                break;
            case PipelineType::OfflineBPSBGStatsWithIPE:
            case PipelineType::OfflineBPSHistStatsWithIPE:
                result = SetupOfflinePipelineBPSStats(streamIdMap, type);
                break;
            case PipelineType::OfflineJpegSnapshot:
                result = SetupOfflineJpeg(streamIdMap, type);
                break;
            case PipelineType::OfflineBPSGPUpipeline:
                result = SetupOfflineBPSGPUPipeline(streamIdMap);
                break;
            case PipelineType::OfflineFDPipeline:
            case PipelineType::OfflineIPEFDPipeline:
                result = SetupOfflineFDPipeline(streamIdMap, type);
                break;
            case PipelineType::OfflineBPSWithIPEAndDS:
                result = SetupOfflineBPSWithIPEAndDSPipeline(streamIdMap);
                break;
            case PipelineType::OfflineChiGPUNode:
                result = SetupOfflineChiGPUNode(streamIdMap);
                break;
            case PipelineType::OfflineExtBPSIPE:
            case PipelineType::OfflineExtBPSIPEJPEG:
                result = SetupOfflineChiExternalNode(streamIdMap, type);
                break;
            case PipelineType::OfflineDepthNode:
                result = SetupOfflineDepthNode(streamIdMap, type);
                break;
            case PipelineType::OfflineIFEGeneric:
                result = SetupOfflineIFEGeneric(streamIdMap, type);
                break;
            case PipelineType::OfflineIPEGeneric:
                result = SetupOfflineIPEGeneric(streamIdMap, type);
                break;
            case PipelineType::OfflineIPEGeneric2StreamDisplay:
                result = SetupOfflineIPEGeneric(streamIdMap, type);
                break;
            case PipelineType::OfflineBPSGeneric:
                result = SetupOfflineBPSGeneric(streamIdMap, type);
                break;
            case PipelineType::OfflineBPSGeneric2Stream:
                result = SetupOfflineBPSGeneric(streamIdMap, type);
                break;
            case PipelineType::OfflineBPSDownscaleGeneric:
                result = SetupOfflineBPSDownscaleGeneric(streamIdMap, type);
                break;
            // E2E pipelines
            case PipelineType::PreviewJpegSnapshot:
            case PipelineType::PreviewYuvSnapshot:
            case PipelineType::PreviewJpegSnapshot3AStats:
            case PipelineType::PreviewYuvSnapshot3AStats:
                result = SetupE2EZSLPreviewSnapshot(streamIdMap, type);
                break;
            case PipelineType::LiveVideoSnapshot:
            case PipelineType::PreviewCallbackSnapshotThumbnail:
                result = SetupE2EPreviewVideoCallback(streamIdMap, type);
                break;
            case PipelineType::HDRPreviewVideo:
            case PipelineType::RealtimeIFEHDR:
                result = SetupE2EHDRPreviewVideo(streamIdMap, type);
                break;
            default:
               result = CDKResultEInvalidArg;
               NT_LOG_ERROR("Invalid pipeline type: [%d]", type);
               break;
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
    *   PipelineUtils::SetupRealtimePipelineIPE
    *
    *   @brief
    *       Setup Pipeline parameters for real time ipe usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIPE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (type == PipelineType::RealTimeIPE)
        {
            m_pipelineDescriptor     = Usecases1Target[TestIPEFullId].pPipelineTargetCreateDesc[Ipefull].pipelineCreateDesc;
            m_pipelineName           = Usecases1Target[TestIPEFullId].pPipelineTargetCreateDesc[Ipefull].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases1Target[TestIPEFullId].pPipelineTargetCreateDesc[Ipefull].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases1Target[TestIPEFullId].pPipelineTargetCreateDesc[Ipefull].sinkTarget;
        }
        else if (type == PipelineType::RealtimePrevHVX)
        {
            NT_LOG_WARN( "This test needs enableHVXStreaming=1 in overridesettings");
            INT pipelineId = fetchPipelineID(Usecases1Target[TestPreviewHVXId], "PreviewHVX");
            m_pipelineDescriptor     = Usecases1Target[TestPreviewHVXId].pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
            m_pipelineName           = Usecases1Target[TestPreviewHVXId].pPipelineTargetCreateDesc[pipelineId].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases1Target[TestPreviewHVXId].pPipelineTargetCreateDesc[pipelineId].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases1Target[TestPreviewHVXId].pPipelineTargetCreateDesc[pipelineId].sinkTarget;
        }
        else
        {
            NT_LOG_ERROR( "Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        InitPortBufferDescriptor();

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort       = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected %d ", portChangeResult);
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
        {
            selectIpePort = static_cast<CSLIPEOutputPortId>(CSLIPEOutputPortId::CSLIPEOutputPortIdVideo);
        }

        m_pPipelineOutputBuffer[0].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[0].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[0].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[0].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[0].pStream = streamIdMap[StreamUsecases::PREVIEW];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineIFE
    *
    *   @brief
    *       Setup Pipeline parameters for real time ife usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIFE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        StreamUsecases activeStreamName;
        uint32_t CONFIG_INDEX = 1;

        // First search if correct stream name present, else return
        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) != streamIdMap.end())          // MultiCameraTest
        {
            activeStreamName = StreamUsecases::CONFIG_OUTPUT;
        }
        else if (streamIdMap.find(StreamUsecases::IFEOutputPortFull) != streamIdMap.end())   // RealtimePipelineTest
        {
            activeStreamName = StreamUsecases::IFEOutputPortFull;
        }
        else
        {
            NT_LOG_WARN("Cannot find chistream for CONFIG_OUTPUT or IFEOutputPortFull");
            return CDKResultENoSuch;
        }

        ChiUsecase                        selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        if (type == PipelineType::RealtimeIFE)
        {
            selectUsecase                = Usecases2Target[TestIFEFullId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IfeFull];
        }
        else if (type == PipelineType::RealtimeIFE3A)
        {
            selectUsecase                = Usecases3Target[TestZSLUseCaseId];
            INT pipelineId = fetchPipelineID(selectUsecase, "IFEFull3A");
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[pipelineId];
        }
        else if (type == PipelineType::RealtimeIFEDualFoV)
        {
            selectUsecase                = Usecases2Target[TestIFEFullId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IfeFullDualFoV];
        }
        else
        {
            NT_LOG_ERROR("Invalid Realtime pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        m_pipelineDescriptor               = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName                     = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo           = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo           = selectTargetCreateDescriptor.sinkTarget;

        IfePortId            selectIfePort = ifeportid_t::IFEOutputPortFull;   //default

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
            CONFIG_INDEX = 0;
        }

        InitPortBufferDescriptor();

        if (activeStreamName == StreamUsecases::CONFIG_OUTPUT)  // configurable port for MultiCameraTest
        {
            CHISTREAMFORMAT selectFormat = streamIdMap[activeStreamName]->format;

            // Set the default output port based on format
            switch (selectFormat)
            {
            case ChiStreamFormatYCrCb420_SP:
            case ChiStreamFormatYCbCr420_888:
            case ChiStreamFormatImplDefined:
            case ChiStreamFormatUBWCTP10:
            case ChiStreamFormatUBWCNV12:
            case ChiStreamFormatUBWCNV124R:
            case ChiStreamFormatP010:
            case ChiStreamFormatNV12HEIF:
                selectIfePort = ifeportid_t::IFEOutputPortFull;
                break;
            case ChiStreamFormatRaw16:
            case ChiStreamFormatRawOpaque:
            case ChiStreamFormatRaw10:
            case ChiStreamFormatRaw64:
                selectIfePort = ifeportid_t::IFEOutputPortRDI0;
                break;
            case ChiStreamFormatPD10:
                selectIfePort = ifeportid_t::IFEOutputPortDS4;
                break;
            case ChiStreamFormatBlob:
                selectIfePort = ifeportid_t::IFEOutputPortStatsRS;
                break;
                // Formats which we don't know the output port on IFE
            case ChiStreamFormatY8:
            case ChiStreamFormatY16:
                selectIfePort = ifeportid_t::IFEOutputPortFull;
                NT_LOG_WARN("Output port is unknown for CHISTREAMFORMAT %d on IFE generic pipeline. "
                    "Setting to default output port %d...", selectFormat, selectIfePort);
                break;
            default:
                selectIfePort = ifeportid_t::IFEOutputPortFull;
                NT_LOG_WARN("Unknown format %d. Setting to default output port %d...", selectFormat, selectIfePort);
                break;
            }
            NT_LOG_INFO("Default output port for format %d is [%d]", selectFormat, selectIfePort);

            // If user provided an output port, override the default from format
            IfePortId overrideIfePort = static_cast<IfePortId>(CmdLineParser::GetOutputPort());
            if (IfePortId::IFEPortInvalid != overrideIfePort)
            {
                selectIfePort = overrideIfePort;
                NT_LOG_INFO("Overriding default output port with [%d]", overrideIfePort);
            }

            // Edit the pipeline descriptor with selected output port
            // TODO intelligently find CONFIG_INDEX by going through links, find where source is IFE (what we want based on
            // port and instance)
            // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
            // buffers). Here it is the first output (0) buffer in all cases.
            m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId                                = selectIfePort;
            m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId    = selectIfePort;
            selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIfePort;
        }


        //Switch port IFE[fulltodisp]
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IFE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IfeFulltoDisp && type != PipelineType::RealtimeIFEDualFoV)
        {
            selectIfePort = ifeportid_t::IFEOutputPortDisplayFull;
        }

        // Setup output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = selectIfePort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[activeStreamName];

        if (type == PipelineType::RealtimeIFEDualFoV)
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = ifeportid_t::IFEOutputPortDisplayFull;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[
                StreamUsecases::IFEOutputPortDisplayFull];
        }

        return CDKResultSuccess;

    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineIFEStats
    *
    *   @brief
    *       Setup Pipeline parameters for real time ife usecase with all stats ports
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIFEStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        NT_UNUSED_PARAM(type);

        INT pipelineId = fetchPipelineID(Usecases13Target[TestIFEFullStatsId], "IfeFullStats");
        m_pipelineDescriptor     = Usecases13Target[TestIFEFullStatsId].pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
        m_pipelineName           = Usecases13Target[TestIFEFullStatsId].pPipelineTargetCreateDesc[pipelineId].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases13Target[TestIFEFullStatsId].pPipelineTargetCreateDesc[pipelineId].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases13Target[TestIFEFullStatsId].pPipelineTargetCreateDesc[pipelineId].sinkTarget;

        InitPortBufferDescriptor();

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        UINT32 outputBufferIndex = 0;

        //Switch port IFE[fulltodisp]
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();
        IfePortId   selectIfePort = ifeportid_t::IFEOutputPortFull;   //default

        CDKResult portChangeResult = SwitchPortByType(CamX::IFE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IfeFulltoDisp)
        {
            selectIfePort = ifeportid_t::IFEOutputPortDisplayFull;
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePort;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortFull");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortFull];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsRS;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsRS) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsRS");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsRS];

        if (m_socId == 457)
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsBFW;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsBFW) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsBFW");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsBFW];
        }
        else
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsCS;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsCS) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsCS");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsCS];
        }

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsIHIST;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsIHIST) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsIHIST");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsIHIST];

        if (m_socId == 457)
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsAECBHIST;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsAECBHIST) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsAECBHIST");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsAECBHIST];

            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsAECBE;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsAECBE) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsAECBE");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsAECBE];

            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsGTMBHIST;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsGTMBHIST) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsGTMBHIST");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsGTMBHIST];
        }
        else
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsBHIST;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsBHIST) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsBHIST");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsBHIST];

            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsHDRBE;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsHDRBE) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsHDRBE");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsHDRBE];

            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsHDRBHIST;
            if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsHDRBHIST) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsHDRBHIST");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsHDRBHIST];
        }

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsTLBG;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsTLBG) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsTLBG");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsTLBG];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsBF;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsBF) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsBF");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsBF];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortStatsAWBBG;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortStatsAWBBG) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortStatsAWBBG");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortStatsAWBBG];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineIFERawCamif
    *
    *   @brief
    *       Setup Pipeline parameters for real time ife Rawcamif usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIFERawCamif(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Camif].pipelineCreateDesc;
        m_pipelineName           = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Camif].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Camif].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Camif].sinkTarget;

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortCAMIFRaw;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortCAMIFRaw) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortCAMIFRaw");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortCAMIFRaw];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineMemcpy
    *
    *   @brief
    *       Setup Pipeline parameters for real time memcpy usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineExternal(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (PipelineType::RealtimePrevWithMemcpy == type || PipelineType::RealtimeBypassNode == type)
        {
            if (PipelineType::RealtimePrevWithMemcpy == type)
            {
                m_pipelineDescriptor     = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewWithMemcpy].pipelineCreateDesc;
                m_pipelineName           = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewWithMemcpy].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewWithMemcpy].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewWithMemcpy].sinkTarget;
            }
            if (PipelineType::RealtimeBypassNode == type)
            {
                m_pipelineDescriptor     = Usecases1Target[TestBypassNodeId].pPipelineTargetCreateDesc[BypassNode].pipelineCreateDesc;
                m_pipelineName           = Usecases1Target[TestBypassNodeId].pPipelineTargetCreateDesc[BypassNode].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[TestBypassNodeId].pPipelineTargetCreateDesc[BypassNode].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[TestBypassNodeId].pPipelineTargetCreateDesc[BypassNode].sinkTarget;
            }

            InitPortBufferDescriptor();

            //Switch port IPE[disptovideo]
            CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
            CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

            CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
            if (portChangeResult != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to switch port type selected");
                return portChangeResult;
            }
            if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
            {
                selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            }

            m_pPipelineOutputBuffer[0].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[0].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[0].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[0].pNodePort->nodePortId = selectIpePort;
            if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[0].pStream = streamIdMap[StreamUsecases::PREVIEW];
        }
        else if (PipelineType::RealtimePrevFromMemcpy == type || PipelineType::RealtimeInplaceNode == type ||
            PipelineType::RealtimeChiDepth == type)
        {
            if (PipelineType::RealtimePrevFromMemcpy == type)
            {
                m_pipelineDescriptor     = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewFromMemcpy]. pipelineCreateDesc;
                m_pipelineName           = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewFromMemcpy].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewFromMemcpy].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[TestMemcpyId].pPipelineTargetCreateDesc[PreviewFromMemcpy].sinkTarget;
            }
            if (PipelineType::RealtimeInplaceNode == type)
            {
                m_pipelineDescriptor     = Usecases1Target[TestInplaceNodeId].pPipelineTargetCreateDesc[InplaceNode].pipelineCreateDesc;
                m_pipelineName           = Usecases1Target[TestInplaceNodeId].pPipelineTargetCreateDesc[InplaceNode].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[TestInplaceNodeId].pPipelineTargetCreateDesc[InplaceNode].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[TestInplaceNodeId].pPipelineTargetCreateDesc[InplaceNode].sinkTarget;
            }
            if (PipelineType::RealtimeChiDepth == type)
            {
                m_pipelineDescriptor     = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[ChiDepthNode].pipelineCreateDesc;
                m_pipelineName           = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[ChiDepthNode].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[ChiDepthNode].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[ChiDepthNode].sinkTarget;
            }

            InitPortBufferDescriptor();
            m_pPipelineOutputBuffer[0].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[0].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
            m_pPipelineOutputBuffer[0].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[0].pNodePort->nodePortId = externalportid_t::ChiNodeOutputPort0;
            if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[0].pStream = streamIdMap[StreamUsecases::PREVIEW];
        }
        else
        {
            NT_LOG_ERROR( "Invalid external pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineIFERawLsc
    *
    *   @brief
    *       Setup Pipeline parameters for real time ife Raw LSC usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIFERawLsc(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Lsc].pipelineCreateDesc;
        m_pipelineName           = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Lsc].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Lsc].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[Lsc].sinkTarget;

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortLSCRaw;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortLSCRaw) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortLSCRaw");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortLSCRaw];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineIPEDisplay
    *
    *   @brief
    *       Setup Pipeline parameters for offline ipe display usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineIPEDisplay(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeDisp].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeDisp].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeDisp].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeDisp].sinkTarget;

        InitPortBufferDescriptor();

        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();


        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
        {
            selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;
        if (streamIdMap.find(StreamUsecases::IPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IPEInputPortFull");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortFull];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;

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
    *   PipelineUtils::SetupOfflinePipelineIPEJPEG
    *
    *   @brief
    *       Setup Pipeline parameters for offline ipe + jpeg usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineIPEJPEG(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[UsecaseIPEJPEGId].pPipelineTargetCreateDesc[OFflineIPEInputYUVOutputJPEG].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[UsecaseIPEJPEGId].pPipelineTargetCreateDesc[OFflineIPEInputYUVOutputJPEG].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[UsecaseIPEJPEGId].pPipelineTargetCreateDesc[OFflineIPEInputYUVOutputJPEG].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[UsecaseIPEJPEGId].pPipelineTargetCreateDesc[OFflineIPEInputYUVOutputJPEG].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;
        if (streamIdMap.find(StreamUsecases::IPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IPEInputPortFull");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortFull];

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
        if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for SNAPSHOT");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;

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
    *   PipelineUtils::SetupOfflinePipelineBPS
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS fullport usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineBPS(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSFull].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSFull].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSFull].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSFull].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex    = 0;
        uint32_t outputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSOutputPortFull;
        if (streamIdMap.find(StreamUsecases::BPSOutputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortFull");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortFull];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflinePipelineBPSDS4
    *
    *   @brief
    *       Setup pipeline parameters for offline BPS with DS4 (ideal raw) out usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineBPSDS4(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestBPSIdealRawId].pPipelineTargetCreateDesc[BPSIdealRaw].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestBPSIdealRawId].pPipelineTargetCreateDesc[BPSIdealRaw].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestBPSIdealRawId].pPipelineTargetCreateDesc[BPSIdealRaw].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestBPSIdealRawId].pPipelineTargetCreateDesc[BPSIdealRaw].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex    = 0;
        uint32_t outputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSOutputPortDS4;
        if (streamIdMap.find(StreamUsecases::BPSOutputPortDS4) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortDS4");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortDS4];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflinePipelineIPEVideo
    *
    *   @brief
    *       Setup Pipeline parameters for offline IPE video port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineIPEVideo(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeVideo].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeVideo].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeVideo].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineIPEId].pPipelineTargetCreateDesc[IpeVideo].sinkTarget;

        InitPortBufferDescriptor();
        uint32_t outputBufferIndex = 0;
        UINT32 inputBufferIndex    = 0;

        //Switch port IPE[videotodisp]
        CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeVideotoDisp)
        {
            selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::IPEOutputPortVideo) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IPEOutputPortVideo");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortVideo];

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId =  CamX::IPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;
        if (streamIdMap.find(StreamUsecases::IPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IPEInputPortFull");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortFull];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;

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
    *   PipelineUtils::SetupOfflinePipelineIPEDispVideo
    *
    *   @brief
    *       Setup Pipeline parameters for offline IPE Display+Video port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineIPEDispVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {

        if (streamIdMap.find(StreamUsecases::IPEInputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IPEInputPortFull");
            return CDKResultENoSuch;
        }

        if (streamIdMap.find(StreamUsecases::IPEOutputPortVideo) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IPEOutputPortVideo");
            return CDKResultENoSuch;
        }

        if (type == PipelineType::OfflineIPEDispVideo)
        {
            m_pipelineDescriptor     = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[OfflineDispVideo].pipelineCreateDesc;
            m_pipelineName           = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[OfflineDispVideo].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[OfflineDispVideo].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[OfflineDispVideo].sinkTarget;
        }
        else if (type == PipelineType::OfflineIPEBlurModule)
        {
            m_pipelineDescriptor     = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[IPEBlurModule].pipelineCreateDesc;
            m_pipelineName           = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[IPEBlurModule].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[IPEBlurModule].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases5Target[TestOfflineIPESIMOId].pPipelineTargetCreateDesc[IPEBlurModule].sinkTarget;
        }

        InitPortBufferDescriptor();
        uint32_t outputBufferIndex = 0;
        UINT32 inputBufferIndex    = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortFull];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width  = 4000;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 3000;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width  = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width  = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);

        if (type == PipelineType::OfflineIPEDispVideo)
        {
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;

            if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEOutputPortDisplay");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];
            outputBufferIndex++;
        }
        else if (type == PipelineType::OfflineIPEBlurModule)
        {
            inputBufferIndex++;
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFullRef;
            if (streamIdMap.find(StreamUsecases::IPEInputDS4BlurMapBlur) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEInputDS4BlurMapBlur");
                return CDKResultENoSuch;
            }
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputDS4BlurMapBlur];

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFullRef;

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width  = 4000;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 3000;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width  = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width  = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            inputBufferIndex++;

            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFullRef;
            if (streamIdMap.find(StreamUsecases::IPEInputDS16BlurMapBlur) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEInputDS16BlurMapBlur");
                return CDKResultENoSuch;
            }
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputDS16BlurMapBlur];

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFullRef;

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width  = 4000;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 3000;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width  = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width  = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
        }

        // IPE Video --> Sink Buffer (remain consistent w/ topology)
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortVideo];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflinePipelineBPSIPE
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS+IPE Display port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineBPSIPE(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDisp].
            pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDisp].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDisp].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDisp].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();


        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
        {
            selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflinePipelineBPSIPEJpeg
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS+IPE Jpeg port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineBPSIPEJpeg(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestOfflineBPSIPEJpegId].pPipelineTargetCreateDesc[OfflineBPSIPEJpeg].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineBPSIPEJpegId].pPipelineTargetCreateDesc[OfflineBPSIPEJpeg].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineBPSIPEJpegId].pPipelineTargetCreateDesc[OfflineBPSIPEJpeg].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineBPSIPEJpegId].pPipelineTargetCreateDesc[OfflineBPSIPEJpeg].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = bpsportid_t::BPSInputPort;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

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

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflinePipelineBPSIPEDispVideo
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS+IPE Display and Video port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineBPSIPEDispVideo(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDispVideo].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDispVideo].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDispVideo].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineBPSIPEId].pPipelineTargetCreateDesc[OfflineBPSIPEDispVideo].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];


        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
        if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IPEOutputPortDisplay");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        if (streamIdMap.find(StreamUsecases::IPEOutputPortVideo) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IPEOutputPortVideo");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortVideo];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflinePipelineBPSStats
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS full + stats port usecase
    *       Setup Pipeline parameters for offline BPS stats port + IPE display port usecase
    *       Setup Pipeline parameters for offline BPS BGstats port + IPE display port usecase
    *       Setup Pipeline parameters for offline BPS Histstats port + IPE display port usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflinePipelineBPSStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        UINT32 outputBufferIndex = 0;

        switch (type)
        {
        case PipelineType::OfflineBPSStats:
            m_pipelineDescriptor     = Usecases4Target[OfflineBPSStatsId].pPipelineTargetCreateDesc[BPSStats].pipelineCreateDesc;
            m_pipelineName           = Usecases4Target[OfflineBPSStatsId].pPipelineTargetCreateDesc[BPSStats].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases4Target[OfflineBPSStatsId].pPipelineTargetCreateDesc[BPSStats].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases4Target[OfflineBPSStatsId].pPipelineTargetCreateDesc[BPSStats].sinkTarget;
            InitPortBufferDescriptor();

            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSOutputPortFull;
            if (streamIdMap.find(StreamUsecases::BPSOutputPortFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortFull");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortFull];
            break;

        case PipelineType::OfflineBPSStatsWithIPE:
            m_pipelineDescriptor     = Usecases4Target[OfflineBPSStatsWithIPEId].pPipelineTargetCreateDesc[BPSAllStats].pipelineCreateDesc;
            m_pipelineName           = Usecases4Target[OfflineBPSStatsWithIPEId].pPipelineTargetCreateDesc[BPSAllStats].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases4Target[OfflineBPSStatsWithIPEId].pPipelineTargetCreateDesc[BPSAllStats].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases4Target[OfflineBPSStatsWithIPEId].pPipelineTargetCreateDesc[BPSAllStats].sinkTarget;
            InitPortBufferDescriptor();
            break;

        case PipelineType::OfflineBPSBGStatsWithIPE:
            m_pipelineDescriptor     = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSBGStats].pipelineCreateDesc;
            m_pipelineName           = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSBGStats].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSBGStats].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSBGStats].sinkTarget;
            InitPortBufferDescriptor();
            break;

        case PipelineType::OfflineBPSHistStatsWithIPE:
            m_pipelineDescriptor     = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSHistStats].pipelineCreateDesc;
            m_pipelineName           = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSHistStats].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSHistStats].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[OfflineBPSSingleStatWithIPEId].pPipelineTargetCreateDesc[BPSHistStats].sinkTarget;
            InitPortBufferDescriptor();
            break;

        default:
            NT_LOG_ERROR( "Invalid BPSStats pipeline type given: %d", type);
            return CDKResultEInvalidArg;
        }

        if (type != PipelineType::OfflineBPSStats)
        {
            //Switch port IPE[disptovideo]
            CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
            CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

            CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
            if (portChangeResult != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to switch port type selected");
                return portChangeResult;
            }
            if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
            {
                selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            }

            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
            if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEOutputPortDisplay");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];
        }

        UINT32 inputBufferIndex = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

        if (type != PipelineType::OfflineBPSHistStatsWithIPE)
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSOutputPortStatsBG;
            if (streamIdMap.find(StreamUsecases::BPSOutputPortStatsBG) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortStatsBG");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortStatsBG];
        }

        if (type != PipelineType::OfflineBPSBGStatsWithIPE)
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSOutputPortStatsBHist;
            if (streamIdMap.find(StreamUsecases::BPSOutputPortStatsBhist) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortStatsBhist");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortStatsBhist];
        }


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
    *   PipelineUtils::SetupPreviewCallbackSnapshotWithThumbnail
    *
    *   @brief
    *       Setup Pipeline parameters for offline preview callback snapshot with thumbnail
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupPreviewCallbackSnapshotWithThumbnail(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {

        if (type == PipelineType::RealtimePrevWithCallbackWithStats || type == PipelineType::RealtimePrevWith3AWrappers)
        {
            ChiUsecase useCase =Usecases5Target[TestPreviewCallbackSnapshotWithThumbnailId];
            INT pipelineId = -1;

            if (type == PipelineType::RealtimePrevWith3AWrappers)
            {
                pipelineId = fetchPipelineID(useCase, "RealtimePrevWithAECWrapper");
            }
            else if (type == PipelineType::RealtimePrevWithCallbackWithStats)
            {
                pipelineId = fetchPipelineID(useCase, "RealtimePrevwithCallback");
            }

            if (pipelineId < 0)
            {
                NT_LOG_ERROR("Invalid pipeline name. Please check topology XML.");
                return CDKResultENoSuch;
            }

            m_pipelineDescriptor     = useCase.pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
            m_pipelineName           = useCase.pPipelineTargetCreateDesc[pipelineId].pPipelineName;
            m_pipelineSrcTgtDescInfo = useCase.pPipelineTargetCreateDesc[pipelineId].sourceTarget;
            m_pipelineSnkTgtDescInfo = useCase.pPipelineTargetCreateDesc[pipelineId].sinkTarget;

            InitPortBufferDescriptor();
            UINT32 outputBufferIndex = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
            if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IPEOutputPortDisplay");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];

            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            if (streamIdMap.find(StreamUsecases::PREVIEWCALLBACK) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for PREVIEWCALLBACK");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEWCALLBACK];

        }
        else
        {
            NT_LOG_ERROR( "Invalid pipeline type");
            return CDKResultEInvalidArg;
        }

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineJpeg
    *
    *   @brief
    *       Setup Pipeline parameters for offline jpeg snapshot usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineJpeg(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        NATIVETEST_UNUSED_PARAM(type);
        m_pipelineDescriptor     = Usecases2Target[TestOfflineJPEGId].pPipelineTargetCreateDesc[JpegSnapshot].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineJPEGId].pPipelineTargetCreateDesc[JpegSnapshot].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineJPEGId].pPipelineTargetCreateDesc[JpegSnapshot].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineJPEGId].pPipelineTargetCreateDesc[JpegSnapshot].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::JPEG;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = jpegportid_t::JPEGInputPort;
        if (streamIdMap.find(StreamUsecases::JPEGInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for JPEGInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::JPEGInputPort];


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

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::JPEG;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = jpegportid_t::JPEGInputPort;

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
    *   PipelineUtils::SetupCustomVendorWritePipeline
    *
    *   @brief
    *       Setup Pipeline parameters for real time preview usecase with custom vendortag node
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupCustomVendorWritePipeline(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases1Target[TestCustomNodeId].pPipelineTargetCreateDesc[VendorTagWrite].pipelineCreateDesc;
        m_pipelineName           = Usecases1Target[TestCustomNodeId].pPipelineTargetCreateDesc[VendorTagWrite].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases1Target[TestCustomNodeId].pPipelineTargetCreateDesc[VendorTagWrite].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases1Target[TestCustomNodeId].pPipelineTargetCreateDesc[VendorTagWrite].sinkTarget;
        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort       = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

            CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
            if (portChangeResult != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to switch port type selected");
                return portChangeResult;
            }
            if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
            {
                selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineIFERDIO
    *
    *   @brief
    *       Setup Pipeline parameters for real time ife Raw RDI0 usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIFERDIO(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[RDI0].pipelineCreateDesc;
        m_pipelineName           = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[RDI0].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[RDI0].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseRawId].pPipelineTargetCreateDesc[RDI0].sinkTarget;
        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortRDI0;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortRDI0) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortRDI0");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortRDI0];
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePrevRDI
    *
    *   @brief
    *       Setup Pipeline parameters for real time Preview + RDI usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePrevRDI(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        NT_UNUSED_PARAM(type);

        UINT32 outputBufferIndex = 0;
        m_pipelineDescriptor     = Usecases3Target[TestZSLUseCaseId].pPipelineTargetCreateDesc[ZSLPreviewRaw].pipelineCreateDesc;
        m_pipelineName           = Usecases3Target[TestZSLUseCaseId].pPipelineTargetCreateDesc[ZSLPreviewRaw].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases3Target[TestZSLUseCaseId].pPipelineTargetCreateDesc[ZSLPreviewRaw].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases3Target[TestZSLUseCaseId].pPipelineTargetCreateDesc[ZSLPreviewRaw].sinkTarget;

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();


            CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
            if (portChangeResult != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to switch port type selected");
                return portChangeResult;
            }
            if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
            {
                selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IPEOutputPortDisplay");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];

        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortRDI0;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortRDI0) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for IFEOutputPortRDI0");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortRDI0];

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineBPSGPUPipeline
    *
    *   @brief
    *       Setup Pipeline parameters for offline BPS with GPU node
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineBPSGPUPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases5Target[TestGPUDSPortsId].pPipelineTargetCreateDesc[OfflineBPSGPU].pipelineCreateDesc;
        m_pipelineName           = Usecases5Target[TestGPUDSPortsId].pPipelineTargetCreateDesc[OfflineBPSGPU].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases5Target[TestGPUDSPortsId].pPipelineTargetCreateDesc[OfflineBPSGPU].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases5Target[TestGPUDSPortsId].pPipelineTargetCreateDesc[OfflineBPSGPU].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex  = 0;
        UINT32 outputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR(" Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = 255;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = gpuportid_t::GPUOutputPortFull;
        if (streamIdMap.find(StreamUsecases::GPUOutputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for GPUOutputPortFull");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::GPUOutputPortFull];
        outputBufferIndex++;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = 255;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = gpuportid_t::GPUOutputPortDS4;
        if (streamIdMap.find(StreamUsecases::GPUOutputPortDS4) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for GPUOutputPortDS4");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::GPUOutputPortDS4];
        outputBufferIndex++;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = 255;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = gpuportid_t::GPUOutputPortDS16;
        if (streamIdMap.find(StreamUsecases::GPUOutputPortDS16) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for GPUOutputPortDS16");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::GPUOutputPortDS16];
        outputBufferIndex++;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = 255;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = gpuportid_t::GPUOutputPortDS64;
        if (streamIdMap.find(StreamUsecases::GPUOutputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for GPUOutputPortDS64");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::GPUOutputPortDS64];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflineFDPipeline
    *
    *   @brief
    *       Setup Pipeline parameters for offline FD usecases
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineFDPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        if (streamIdMap.find(StreamUsecases::ChiNodeInputPort0) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for ChiNodeInputPort0");
            return CDKResultENoSuch;
        }
        if (streamIdMap.find(StreamUsecases::FDManagerOutputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for FDManagerOutputPort");
            return CDKResultENoSuch;
        }

        INT pipelineId = -1;

        if (type == PipelineType::OfflineFDPipeline)
        {
            pipelineId = fetchPipelineID(Usecases2Target[TestOfflineFDId], "OfflineFD");
        }
        else if (type == PipelineType::OfflineIPEFDPipeline)
        {
            pipelineId = fetchPipelineID(Usecases2Target[TestOfflineFDId], "OfflineIPEFD");
        }

        if (pipelineId < 0)
        {
            NT_LOG_ERROR("Invalid pipeline name. Please check topology XML.");
            return CDKResultENoSuch;
        }

        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex = 0;
        CDKResult nodePropertyResult = CDKResultSuccess;

        m_pipelineDescriptor     = Usecases2Target[TestOfflineFDId].pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestOfflineFDId].pPipelineTargetCreateDesc[pipelineId].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestOfflineFDId].pPipelineTargetCreateDesc[pipelineId].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestOfflineFDId].pPipelineTargetCreateDesc[pipelineId].sinkTarget;

        InitPortBufferDescriptor();

        CHINODE* pNode = m_pipelineDescriptor.pNodes;
        for(UINT nodeIter = 0; nodeIter < m_pipelineDescriptor.numNodes; nodeIter++)
        {
            if (nullptr == pNode)
            {
                return CDKResultEInvalidPointer;
            }
            nodePropertyResult = ChangeNodeProperty(*pNode, NodePropertyProcessingType);
            if (CDKResultSuccess != nodePropertyResult)
            {
                return nodePropertyResult;
            }
            pNode++;
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::FDManager;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = fdmgrportid_t::FDManagerOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::FDManagerOutputPort];

        if (type == PipelineType::OfflineFDPipeline)  // input is fed to ChiExternalNode
        {
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = externalportid_t::ChiNodeInputPort0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::ChiNodeInputPort0];

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CAMXHWDEFS_H::ChiExternalNode;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = externalportid_t::ChiNodeInputPort0;
        }
        else if (type == PipelineType::OfflineIPEFDPipeline) // input is fed to IPE node
        {
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::ChiNodeInputPort0];

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;
        }

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
    *   PipelineUtils::SetupRealtimeIPE3A
    *
    *   @brief
    *       Setup Pipeline parameters for realtime IFE + IPE with 3A
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimeIPE3A(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        switch (type)
        {
            case PipelineType::RealtimeIPE3A:
            {
                INT pipelineId = fetchPipelineID(Usecases1Target[UsecaseIPE3AId], "IPE3A");
                m_pipelineDescriptor     = Usecases1Target[UsecaseIPE3AId].pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
                m_pipelineName           = Usecases1Target[UsecaseIPE3AId].pPipelineTargetCreateDesc[pipelineId].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases1Target[UsecaseIPE3AId].pPipelineTargetCreateDesc[pipelineId].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases1Target[UsecaseIPE3AId].pPipelineTargetCreateDesc[pipelineId].sinkTarget;
                break;
            }
            default:
            {
                NT_LOG_ERROR( "Invalid pipeline type given: %d", static_cast<int>(type));
                return CDKResultEInvalidArg;
            }
        }

        InitPortBufferDescriptor();
        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
        }

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
        {
            selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        }

        //1. IPEOutputPortDisplay
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IPEOutputPortDisplay");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePIPCam
    *
    *   @brief
    *       Setup pipeline parameters for realtime IFE + IPE with ABC ports, dumped stats, and RDI.
    *       Used for PIP camera tests.
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePIPCam(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        switch (type)
        {
        case PipelineType::RealtimeRDI1Only:
            m_pipelineDescriptor     = Usecases3Target[TestPIPCamId].pPipelineTargetCreateDesc[RDI1Only].pipelineCreateDesc;
            m_pipelineName           = Usecases3Target[TestPIPCamId].pPipelineTargetCreateDesc[RDI1Only].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases3Target[TestPIPCamId].pPipelineTargetCreateDesc[RDI1Only].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases3Target[TestPIPCamId].pPipelineTargetCreateDesc[RDI1Only].sinkTarget;
            break;
        default:
            NT_LOG_ERROR( "Invalid RealtimePIPCam pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;

        // IFEOutputPortRDI1
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortRDI1;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortRDI1) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortRDI1");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortRDI1];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineBPSWithIPEAndDSPipeline
    *
    *   @brief
    *       Setup pipeline parameters for BPS -> IPE with raw input and downscaling
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineBPSWithIPEAndDSPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[TestBPSWithIPEAndDSId].pPipelineTargetCreateDesc[BPSWithIPEAndDS].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestBPSWithIPEAndDSId].pPipelineTargetCreateDesc[BPSWithIPEAndDS].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestBPSWithIPEAndDSId].pPipelineTargetCreateDesc[BPSWithIPEAndDS].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestBPSWithIPEAndDSId].pPipelineTargetCreateDesc[BPSWithIPEAndDS].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::BPSInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for BPSInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSInputPort];

        //Switch port IPE[disptovideo]
        CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
        {
            selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        }

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
        if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupOfflineChiGPUNode
    *
    *   @brief
    *       Setup Pipeline parameters for offline ChiGPUNode usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineChiGPUNode(std::map<StreamUsecases, ChiStream*> streamIdMap)
    {
        m_pipelineDescriptor     = Usecases2Target[UsecaseChiGPUNodeId].pPipelineTargetCreateDesc[TestChiGPUNode].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[UsecaseChiGPUNodeId].pPipelineTargetCreateDesc[TestChiGPUNode].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[UsecaseChiGPUNodeId].pPipelineTargetCreateDesc[TestChiGPUNode].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[UsecaseChiGPUNodeId].pPipelineTargetCreateDesc[TestChiGPUNode].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex  = 0;
        UINT32 outputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(m_pipelineInputBuffer);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = gpuportid_t::GPUInputPort;
        if (streamIdMap.find(StreamUsecases::GPUInputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for GPUInputPort");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::GPUInputPort];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = gpuportid_t::GPUInputPort;

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = gpuportid_t::GPUOutputPortFull;
        if (streamIdMap.find(StreamUsecases::GPUOutputPortFull) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for GPUOutputPortFull");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::GPUOutputPortFull];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimeJpeg
    *
    *   @brief
    *       Setup pipeline parameters for JPEG node tests.
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimeJpeg(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        switch (type)
        {
        case PipelineType::RealtimeIPEJpeg:
            m_pipelineDescriptor     = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEIPEJpeg].pipelineCreateDesc;
            m_pipelineName           = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEIPEJpeg].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEIPEJpeg].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEIPEJpeg].sinkTarget;
            break;
        case PipelineType::RealtimeBPSIPEJpeg:
            m_pipelineDescriptor     = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEBPSIPEJpeg].pipelineCreateDesc;
            m_pipelineName           = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEBPSIPEJpeg].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEBPSIPEJpeg].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEBPSIPEJpeg].sinkTarget;
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime JPEG node pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
        if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for SNAPSHOT");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimeFD
    *
    *   @brief
    *       Setup pipeline parameters for JPEG and FD nodes tests.
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimeFD(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        switch (type)
        {
        case PipelineType::RealtimeFD:
            m_pipelineDescriptor     = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEFD].pipelineCreateDesc;
            m_pipelineName           = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEFD].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEFD].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[RealtimeNodesTestId].pPipelineTargetCreateDesc[SensorIFEFD].sinkTarget;
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime FD node pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex     = 0;
        CDKResult nodePropertyResult = CDKResultSuccess;

        for (unsigned long long int node = 0; node < sizeof(m_pipelineDescriptor.pNodes); node++)
        {
            nodePropertyResult = ChangeNodeProperty(m_pipelineDescriptor.pNodes[node], NodePropertyProcessingType);
            if (CDKResultSuccess != nodePropertyResult)
            {
                return nodePropertyResult;
            }
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::FDManager;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = fdmgrportid_t::FDManagerOutputPort;
        if (streamIdMap.find(StreamUsecases::FDManagerOutputPort) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for FDManagerOutputPort");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::FDManagerOutputPort];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineExternalNode
    *
    *   @brief
    *       Setup offline pipeline parameters for using ChiExternalNode tests.
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineChiExternalNode(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        switch (type)
        {
        case PipelineType::OfflineExtBPSIPE:
            m_pipelineDescriptor     = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPE].pipelineCreateDesc;
            m_pipelineName           = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPE].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPE].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPE].sinkTarget;
            break;
        case PipelineType::OfflineExtBPSIPEJPEG:
            m_pipelineDescriptor     = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPEJPEG].pipelineCreateDesc;
            m_pipelineName           = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPEJPEG].pPipelineName;
            m_pipelineSrcTgtDescInfo = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPEJPEG].sourceTarget;
            m_pipelineSnkTgtDescInfo = Usecases2Target[UsecaseExternalNodeId].pPipelineTargetCreateDesc[OfflineExtBPSIPEJPEG].sinkTarget;
            break;
        default:
            NT_LOG_ERROR( "Invalid ExternalNode pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        InitPortBufferDescriptor();
        UINT32 inputBufferIndex  = 0;
        UINT32 outputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = externalportid_t::ChiNodeInputPort0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::ChiNodeInputPort0];

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = externalportid_t::ChiNodeInputPort0;

        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);

        if (type == PipelineType::OfflineExtBPSIPE)
        {
            //Switch port IPE[disptovideo]
            CSLIPEOutputPortId     selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;  //default
            CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

            CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
            if (portChangeResult != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to switch port type selected");
                return portChangeResult;
            }
            if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo)
            {
                selectIpePort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            }

            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpePort;
            if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for SNAPSHOT");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];
        }
        if (type == PipelineType::OfflineExtBPSIPEJPEG)
        {
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::JPEGAggregator;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = jpegaggportid_t::JPEGAggregatorOutputPort;
            if (streamIdMap.find(StreamUsecases::SNAPSHOT) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for SNAPSHOT");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::SNAPSHOT];
        }

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineDepthNode
    *
    *   @brief
    *       Setup pipeline parameters for offline depth node
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineDepthNode(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        NATIVETEST_UNUSED_PARAM(type);

        m_pipelineDescriptor     = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[DepthNodeOffline].pipelineCreateDesc;
        m_pipelineName           = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[DepthNodeOffline].pPipelineName;
        m_pipelineSrcTgtDescInfo = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[DepthNodeOffline].sourceTarget;
        m_pipelineSnkTgtDescInfo = Usecases2Target[TestChiDepthId].pPipelineTargetCreateDesc[DepthNodeOffline].sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = externalportid_t::ChiNodeOutputPort0;
        if (streamIdMap.find(StreamUsecases::ChiNodeOutputPort0) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for ChiNodeOutputPort0");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::ChiNodeOutputPort0];

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = externalportid_t::ChiNodeInputPort0;
        if (streamIdMap.find(StreamUsecases::ChiNodeInputPort0) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for ChiNodeInputPort0");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::ChiNodeInputPort0];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CAMXHWDEFS_H::ChiExternalNode;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = externalportid_t::ChiNodeInputPort0;

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
    *   PipelineUtils::SetupRealtimeIFEGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic IFE tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimeIFEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        uint32_t CONFIG_INDEX = 1;
        // Select usecase and target create descriptor based on pipeline type
        switch (type)
        {
        case PipelineType::RealtimeIFEGeneric:
            selectUsecase = Usecases2Target[UsecaseGenericId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFEGeneric];
            break;
        case PipelineType::RealtimeIFEGeneric2Stream:
            selectUsecase = Usecases4Target[UsecaseGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFEGeneric2Stream];
            break;
        case PipelineType::RealtimeIFEGeneric2StreamDisplay:
            selectUsecase = Usecases4Target[UsecaseGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFEGeneric2StreamDisplay];
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime IFE Generic pipeline type given : %d", static_cast<int>(type));
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

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
            CONFIG_INDEX = 0;
        }

        IfePortId selectIfePort;
        CHISTREAMFORMAT selectFormat = streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format;

        // Set the default output port based on format
        switch (selectFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
        case ChiStreamFormatNV12HEIF:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            break;
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatRaw12:
            selectIfePort = ifeportid_t::IFEOutputPortRDI0;
            break;
        case ChiStreamFormatPD10:
            selectIfePort = ifeportid_t::IFEOutputPortDS4;
            break;
        case ChiStreamFormatBlob:
            selectIfePort = ifeportid_t::IFEOutputPortStatsRS;
            break;
        // Formats which we don't know the output port on IFE
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Output port is unknown for CHISTREAMFORMAT %d on IFE generic pipeline. "
                "Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        default:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Unknown format %d. Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        }
        NT_LOG_INFO( "Default output port for format %d is [%d]", selectFormat, selectIfePort);

        // If user provided an output port, override the default from format
        IfePortId overrideIfePort = static_cast<IfePortId>(CmdLineParser::GetOutputPort());
        if (IfePortId::IFEPortInvalid != overrideIfePort)
        {
            selectIfePort = overrideIfePort;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideIfePort);
        }

        // Edit the pipeline descriptor with selected output port
        // TODO intelligently find CONFIG_INDEX by going through links, find where source is IFE (what we want based on port
        // and instance)
        // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
        // buffers). Here it is the first output (0) buffer in all cases.
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectIfePort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId = selectIfePort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIfePort;

        //Switch port IFE[fulltodisp]
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IFE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IfeFulltoDisp)
        {
            selectIfePort = ifeportid_t::IFEOutputPortDisplayFull;
        }

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        if (type == PipelineType::RealtimeIFEGeneric2Stream)
        {
            // Setup static full output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortFull;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortFull];
            if (streamIdMap.find(StreamUsecases::IFEOutputPortFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortFull");
                return CDKResultENoSuch;
            }
        }
        else if (type == PipelineType::RealtimeIFEGeneric2StreamDisplay)
        {
            // Setup static display full output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFEOutputPortDisplayFull;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortDisplayFull];
            if (streamIdMap.find(StreamUsecases::IFEOutputPortDisplayFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortDisplayFull");
                return CDKResultENoSuch;
            }
        }

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimeIFEDownscaleGeneric
    *
    *   @brief
    *       Setup pipeline parameters for IFE -> IPE with downscaling and configurable resolution, output format
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimeIFEDownscaleGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        NT_UNUSED_PARAM(type);
        uint32_t CONFIG_INDEX = 1;

        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        selectUsecase  = Usecases2Target[UsecaseGenericDownscaleId];
        selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFEDownscaleGeneric];

        m_pipelineDescriptor     = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName           = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo = selectTargetCreateDescriptor.sinkTarget;

        InitPortBufferDescriptor();

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
            CONFIG_INDEX = 0;
        }

        IfePortId selectIfePort;
        CHISTREAMFORMAT selectFormat = streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format;

        // Set the default output port based on format
        switch (selectFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
        case ChiStreamFormatNV12HEIF:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            break;
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatRaw12:
            selectIfePort = ifeportid_t::IFEOutputPortRDI0;
            break;
        case ChiStreamFormatPD10:
            selectIfePort = ifeportid_t::IFEOutputPortDS4;
            break;
        case ChiStreamFormatBlob:
            selectIfePort = ifeportid_t::IFEOutputPortStatsRS;
            break;
        // Formats which we don't know the output port on IFE
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Output port is unknown for CHISTREAMFORMAT %d on IFE generic pipeline. "
                "Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        default:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Unknown format %d. Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        }
        NT_LOG_INFO( "Default output port for format %d is [%d]", selectFormat, selectIfePort);

        // If user provided an output port, override the default from format
        IfePortId overrideIfePort = static_cast<IfePortId>(CmdLineParser::GetOutputPort());
        if (IfePortId::IFEPortInvalid != overrideIfePort)
        {
            selectIfePort = overrideIfePort;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideIfePort);
        }

        // Edit the pipeline descriptor with selected output port
        // TODO intelligently find CONFIG_INDEX by going through links, find where source is IFE (what we want based on port
        // and instance)
        // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
        // buffers). Here it is the first output (0) buffer in all cases.
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectIfePort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId = selectIfePort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIfePort;

        UINT32 outputBufferIndex = 0;

        //Switch port IFE[fulltodisp]
        IfePortId   selectIfePortDs4               = ifeportid_t::IFEOutputPortDS4;    //default
        IfePortId   selectIfePortDs16              = ifeportid_t::IFEOutputPortDS16;   //default
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();


        CDKResult portChangeResult = SwitchPortByType(CamX::IFE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IfeFulltoDisp)
        {
            selectIfePort = ifeportid_t::IFEOutputPortDisplayFull;
            selectIfePortDs4 = ifeportid_t::IFEOutputPortDisplayDS4;
            selectIfePortDs16 = ifeportid_t::IFEOutputPortDisplayDS16;
        }

        // IFE full out stream
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePort;
        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_OUTPUT");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        // IFE DS4 out stream
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePortDs4;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortDS4) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortDS4");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortDS4];

        // IFE DS16 out stream
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePortDs16;
        if (streamIdMap.find(StreamUsecases::IFEOutputPortDS16) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortDS16");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortDS16];

        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupOfflineIPEGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic IPE tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
        CDKResult PipelineUtils::SetupOfflineIPEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        switch (type)
        {
            case PipelineType::OfflineIPEGeneric:
                selectUsecase = Usecases2Target[UsecaseGenericId];
                selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IPEGeneric];
                break;
            case PipelineType::OfflineIPEGeneric2StreamDisplay:
                selectUsecase = Usecases4Target[UsecaseGeneric2StreamsId];
                selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IPEGeneric2StreamDisplay];
                break;
            default:
                NT_LOG_ERROR("Unknown pipeline type %d", type);
                return CDKResultEFailed;
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

        CSLIPEOutputPortId selectIpeOutputPort = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;

        NT_LOG_INFO( "Default output port for format %d is [%d]",
            streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format, selectIpeOutputPort);

        // If user provided an output port, override the default from format
        int overrideIpeOutputPort = CmdLineParser::GetOutputPort();
        if (INVALID_PORT != overrideIpeOutputPort)
        {
            selectIpeOutputPort = static_cast<CSLIPEOutputPortId>(overrideIpeOutputPort);;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideIpeOutputPort);
        }

        // Edit the pipeline descriptor with selected output port
        const uint32_t CONFIG_INDEX = 0;
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectIpeOutputPort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[CONFIG_INDEX].portId = selectIpeOutputPort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[CONFIG_INDEX].pNodePort->nodePortId = selectIpeOutputPort;


        //Switch port IPE[disptovideo]
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IPE, switchPortType);
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (switchPortType == CmdLineParser::switchport_t::IpeDisptoVideo && type == PipelineType::OfflineIPEGeneric)
        {
            selectIpeOutputPort = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
        }

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIpeOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        if (type == PipelineType::OfflineIPEGeneric2StreamDisplay)
        {
            // Setup static output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];
        }

        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }

        CSLIPEInputPortId selectIpeInputPort = CSLIPEInputPortId::CSLIPEInputPortIdFull;

        NT_LOG_INFO( "Default input port for format %d is [%d]",
            streamIdMap[StreamUsecases::CONFIG_INPUT]->format, selectIpeInputPort);

        // If user provided an input port, override the default from format
        int overrideIpeInputPort = CmdLineParser::GetInputPort();
        if (INVALID_PORT != overrideIpeInputPort)
        {
            selectIpeInputPort = static_cast<CSLIPEInputPortId>(overrideIpeInputPort);
            NT_LOG_INFO( "Overriding default input port with [%d]", overrideIpeInputPort);
        }

        // Edit the pipeline descriptor with selected input port
        m_pipelineDescriptor.pNodes[0].nodeAllPorts.pInputPorts[0].portId = selectIpeInputPort;
        selectTargetCreateDescriptor.sourceTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIpeInputPort;

        //Setup input buffer
        UINT32 inputBufferIndex = 0;

        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = selectIpeInputPort;
        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_INPUT];



        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = selectIpeInputPort;

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
    *   PipelineUtils::SetupOfflineBPSGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic BPS tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineBPSGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        // Select usecase and target create descriptor based on pipeline type
        switch (type)
        {
            case PipelineType::OfflineBPSGeneric:
                selectUsecase = Usecases2Target[UsecaseGenericId];
                selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[BPSGeneric];
                break;
            case PipelineType::OfflineBPSGeneric2Stream:
                selectUsecase = Usecases4Target[UsecaseGeneric2StreamsId];
                selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[BPSGeneric2Stream];
                break;
            default:
                NT_LOG_ERROR( "Invalid offline BPS Generic pipeline type given : %d", static_cast<int>(type));
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

        BpsPortId selectBpsOutputPort;
        CHISTREAMFORMAT selectOutputFormat = streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format;

        // Set the default output port based on format
        switch (selectOutputFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatP010:
            selectBpsOutputPort = bpsportid_t::BPSOutputPortFull;
            break;
        case ChiStreamFormatPD10:
            selectBpsOutputPort = bpsportid_t::BPSOutputPortDS4;
            break;
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatNV12HEIF:
            selectBpsOutputPort = bpsportid_t::BPSOutputPortReg1;
            break;
        case ChiStreamFormatBlob:
            selectBpsOutputPort = bpsportid_t::BPSOutputPortStatsBG;
            break;
        // Formats which we don't know the output port on BPS
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectBpsOutputPort = bpsportid_t::BPSOutputPortFull;
            NT_LOG_WARN( "Output port is unknown for CHISTREAMFORMAT %d on BPS generic pipeline. "
                "Setting to default output port %d...", selectOutputFormat, selectBpsOutputPort);
            break;
        default:
            selectBpsOutputPort = bpsportid_t::BPSOutputPortFull;
            NT_LOG_WARN( "Unknown format %d. Setting to default output port %d...", selectBpsOutputPort,
                selectBpsOutputPort);
            break;
        }
        NT_LOG_INFO( "Default output port for format %d is [%d]", selectOutputFormat, selectBpsOutputPort);

        // If user provided an output port, override the default from format
        BpsPortId overrideBpsOutputPort = static_cast<BpsPortId>(CmdLineParser::GetOutputPort());
        if (BpsPortId::BPSPortInvalid != overrideBpsOutputPort)
        {
            selectBpsOutputPort = overrideBpsOutputPort;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideBpsOutputPort);
        }

        // Edit the pipeline descriptor with selected output port
        const uint32_t CONFIG_INDEX = 0;
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectBpsOutputPort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[CONFIG_INDEX].portId = selectBpsOutputPort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[CONFIG_INDEX].pNodePort->nodePortId = selectBpsOutputPort;

        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        // Setup configurable output buffer
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectBpsOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        if (type == PipelineType::OfflineBPSGeneric2Stream)
        {
            // Setup static output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSOutputPortFull;
            if (streamIdMap.find(StreamUsecases::BPSOutputPortFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortFull");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortFull];
        }

        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }

        BpsPortId selectBpsInputPort;
        CHISTREAMFORMAT selectInputFormat = streamIdMap[StreamUsecases::CONFIG_INPUT]->format;

        // Set the default input port based on format
        // TODO: Update input port based on format. Using BPSInputPort for now.
        switch (selectInputFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
        case ChiStreamFormatNV12HEIF:
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatPD10:
        case ChiStreamFormatBlob:
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectBpsInputPort = bpsportid_t::BPSInputPort;
            NT_LOG_WARN( "Input port is unknown for CHISTREAMFORMAT %d on BPS generic pipeline. "
                "Setting to default input port %d...", selectInputFormat, selectBpsInputPort);
            break;
        default:
            selectBpsInputPort = bpsportid_t::BPSInputPort;
            NT_LOG_WARN( "Unknown format %d. Setting to default input port %d...", selectInputFormat, selectBpsInputPort);
            break;
        }
        NT_LOG_INFO( "Default input port for format %d is [%d]", selectInputFormat, selectBpsInputPort);

        // If user provided an input port, override the default from format
        BpsPortId overrideBpsInputPort = static_cast<BpsPortId>(CmdLineParser::GetInputPort());
        if (BpsPortId::BPSPortInvalid != overrideBpsInputPort)
        {
            selectBpsInputPort = overrideBpsInputPort;
            NT_LOG_INFO( "Overriding default input port with [%d]", overrideBpsInputPort);
        }

        // Edit the pipeline descriptor with selected input port
        m_pipelineDescriptor.pNodes[0].nodeAllPorts.pInputPorts[0].portId = selectBpsInputPort;
        selectTargetCreateDescriptor.sourceTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectBpsInputPort;


        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = selectBpsInputPort;
        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_INPUT];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = selectBpsInputPort;

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
    *   PipelineUtils::SetupOfflineBPSDownscaleGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic BPS downscale tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupOfflineBPSDownscaleGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap,
        PipelineType type)
    {
        NT_UNUSED_PARAM(type);

        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        selectUsecase = Usecases2Target[UsecaseGenericDownscaleId];
        selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[BPSDownscaleGeneric];

        m_pipelineDescriptor     = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName           = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo = selectTargetCreateDescriptor.sinkTarget;

        InitPortBufferDescriptor();
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        // Setup full output buffer
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = BpsPortId::BPSOutputPortFull;
        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_OUTPUT");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        // Setup DS4 output buffer
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = BpsPortId::BPSOutputPortDS4;
        if (streamIdMap.find(StreamUsecases::BPSOutputPortDS4) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortDS4");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortDS4];

        // Setup DS16 output buffer
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = BpsPortId::BPSOutputPortDS16;
        if (streamIdMap.find(StreamUsecases::BPSOutputPortDS16) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortDS16");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortDS16];

        // Setup DS64 output buffer
        outputBufferIndex++;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = BpsPortId::BPSOutputPortDS64;
        if (streamIdMap.find(StreamUsecases::BPSOutputPortDS64) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for BPSOutputPortDS64");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::BPSOutputPortDS64];


       // Setup input buffer requirements
        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::BPS;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = bpsportid_t::BPSInputPort;
        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR("Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_INPUT];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::BPS;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = bpsportid_t::BPSInputPort;

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
    *   PipelineUtils::SetupCVPPipelines
    *
    *   @brief
    *       Setup pipeline parameters for CVP Node DME and DME+ICA tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupCVPPipelines(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        NT_LOG_INFO("setup pipeline");
        IfePortId selectIfePort;
        UINT32 outputBufferIndex = 0;

        ChiPipelineTargetCreateDescriptor* pSelectedPipelineTgtCreateDesc = Usecases3Target[
            UsecaseCVPNodeId].pPipelineTargetCreateDesc;
        switch (type)
        {
        case PipelineType::RealtimeCVPDME:
        case PipelineType::RealtimeCVPDMEICA:
            if (streamIdMap.find(StreamUsecases::IFEOutputPortFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortFull");
                return CDKResultENoSuch;
            }
            if (type == PipelineType::RealtimeCVPDME)
            {
                m_pipelineDescriptor     = pSelectedPipelineTgtCreateDesc[RealtimeCVPDME].pipelineCreateDesc;
                m_pipelineName           = pSelectedPipelineTgtCreateDesc[RealtimeCVPDME].pPipelineName;
                m_pipelineSrcTgtDescInfo = pSelectedPipelineTgtCreateDesc[RealtimeCVPDME].sourceTarget;
                m_pipelineSnkTgtDescInfo = pSelectedPipelineTgtCreateDesc[RealtimeCVPDME].sinkTarget;
            }
            else if (type == PipelineType::RealtimeCVPDMEICA)
            {
                m_pipelineDescriptor     = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEICA].pipelineCreateDesc;
                m_pipelineName           = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEICA].pPipelineName;
                m_pipelineSrcTgtDescInfo = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEICA].sourceTarget;
                m_pipelineSnkTgtDescInfo = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEICA].sinkTarget;
            }

            InitPortBufferDescriptor();
            if (CmdLineParser::RemoveSensor())
            {
                ConvertSensorPipelineToTestgenPipeline();
            }
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortFull];
            break;
        case PipelineType::RealtimeCVPDMEDisplayPort:
            if (streamIdMap.find(StreamUsecases::IFEOutputPortDisplayFull) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortDisplayFull");
                return CDKResultENoSuch;
            }
            if (type == PipelineType::RealtimeCVPDMEDisplayPort)
            {
                m_pipelineDescriptor     = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEDisplayPort].pipelineCreateDesc;
                m_pipelineName           = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEDisplayPort].pPipelineName;
                m_pipelineSrcTgtDescInfo = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEDisplayPort].sourceTarget;
                m_pipelineSnkTgtDescInfo = pSelectedPipelineTgtCreateDesc[RealtimeCVPDMEDisplayPort].sinkTarget;
            }

            InitPortBufferDescriptor();
            if (CmdLineParser::RemoveSensor())
            {
                ConvertSensorPipelineToTestgenPipeline();
            }
            selectIfePort = ifeportid_t::IFEOutputPortDisplayFull;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFEOutputPortDisplayFull];
            break;
        case PipelineType::OfflineCVPStandAlone:
            NT_LOG_INFO("setting desc info");
            m_pipelineDescriptor     = pSelectedPipelineTgtCreateDesc[TestCVPDenseMotion].pipelineCreateDesc;
            m_pipelineName           = pSelectedPipelineTgtCreateDesc[TestCVPDenseMotion].pPipelineName;
            m_pipelineSrcTgtDescInfo = pSelectedPipelineTgtCreateDesc[TestCVPDenseMotion].sourceTarget;
            m_pipelineSnkTgtDescInfo = pSelectedPipelineTgtCreateDesc[TestCVPDenseMotion].sinkTarget;

            InitPortBufferDescriptor();
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime CVP pipeline type given : %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        if (type != PipelineType::OfflineCVPStandAlone)
        {
            //Switch port IFE[fulltodisp, disptofull]
            CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

            CDKResult portChangeResult = SwitchPortByType(CamX::IFE, switchPortType);
            if (portChangeResult != CDKResultSuccess)
            {
                NT_LOG_ERROR("Unable to switch port type selected");
                return portChangeResult;
            }
            if (switchPortType == CmdLineParser::switchport_t::IfeFulltoDisp)
            {
                selectIfePort = ifeportid_t::IFEOutputPortDisplayFull;
            }
            else if (switchPortType == CmdLineParser::switchport_t::IfeDisptoFull)
            {
                selectIfePort = ifeportid_t::IFEOutputPortFull;
            }

            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePort;
        }
        else if (type == PipelineType::OfflineCVPStandAlone)
        {
            NT_LOG_INFO("setting input buffers");
            UINT32 inputBufferIndex = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::CVP;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = 5;
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CVPInputPortTARIFEDS4];
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::CVP;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = 5;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = ICAMinWidthPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = ICAMinHeightPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            inputBufferIndex++;

            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::CVP;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = 33;
            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CVPInputPortIFEDS4DELAY];
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::CVP;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = 33;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = ICAMinWidthPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = ICAMinHeightPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
        }

        return CDKResultSuccess;
    }

    /***********************************************************************************************
    ***
    *   PipelineUtils::SetupOfflineIFEGeneric
    *
    *   @brief
    *       Setup pipeline parameters for generic IFE tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream obje
    cts
    *       [in]  Pipeline                                type            differentiate between pipe
    lines
    *   @return
    *       CDKResult result
    ************************************************************************************************
    **/
    CDKResult PipelineUtils::SetupOfflineIFEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap
        , PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        switch (type)
        {
        case PipelineType::OfflineIFEGeneric:
            selectUsecase = Usecases2Target[UsecaseGenericId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFEGenericOffline];
            break;
        default:
            NT_LOG_ERROR( "Invalid offline IFE Generic pipeline type given : %d",
                static_cast<int>(type));
            return CDKResultEInvalidArg;
            break;
        }

        m_pipelineDescriptor     = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName           = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo = selectTargetCreateDescriptor.sinkTarget;

        InitPortBufferDescriptor();

        m_pipelineDescriptor.isRealTime = false; // needed to enable offline IFE

        if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_OUTPUT");
            return CDKResultENoSuch;
        }

        IfePortId selectIfeOutputPort;
        CHISTREAMFORMAT selectOutputFormat = streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format;

        // Set the default output port based on format
        switch (selectOutputFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
            selectIfeOutputPort = ifeportid_t::IFEOutputPortFull;
            break;
            // Set output port to IPEOutputPortDisplay for unknown formats
        case ChiStreamFormatNV12HEIF:
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw12:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatPD10:
        case ChiStreamFormatBlob:
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectIfeOutputPort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Output port is unknown for CHISTREAMFORMAT %d on offline IFE generic pipeline. "
                "Setting to default output port %d...", selectOutputFormat, selectIfeOutputPort);
            break;
        default:
            selectIfeOutputPort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Unknown format %d. Setting to default output port %d",
                selectOutputFormat, selectIfeOutputPort);
                    break;
        }
        NT_LOG_INFO( "Default output port for format %d is [%d]", selectOutputFormat, selectIfeOutputPort);

        // If user provided an output port, override the default from format
        IfePortId overrideIfeOutputPort = static_cast<IfePortId>(CmdLineParser::GetOutputPort());
        if (IfePortId::IFEPortInvalid != overrideIfeOutputPort)
        {
            selectIfeOutputPort = overrideIfeOutputPort;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideIfeOutputPort);
        }

        // Edit the pipeline descriptor with selected output port
        const uint32_t CONFIG_INDEX = 0;
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId                                = selectIfeOutputPort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[CONFIG_INDEX].portId    = selectIfeOutputPort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[CONFIG_INDEX].pNodePort->nodePortId = selectIfeOutputPort;

        //Switch port IFE[fulltodisp, disptofull]
        CmdLineParser::switchport_t switchPortType = CmdLineParser::GetSwitchPortType();

        CDKResult portChangeResult = SwitchPortByType(CamX::IFE, CmdLineParser::GetSwitchPortType());
        if (portChangeResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to switch port type selected");
            return portChangeResult;
        }
        if (CmdLineParser::GetSwitchPortType() == CmdLineParser::switchport_t::IfeFulltoDisp)
        {
            selectIfeOutputPort = ifeportid_t::IFEOutputPortDisplayFull;
        }

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = selectIfeOutputPort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        // Setup configurable input stream
        if (streamIdMap.find(StreamUsecases::CONFIG_INPUT) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for CONFIG_INPUT");
            return CDKResultENoSuch;
        }
        IfePortId selectIfeInputPort;
        CHISTREAMFORMAT selectInputFormat = streamIdMap[StreamUsecases::CONFIG_INPUT]->format;

        // Set the default input port based on format
        switch (selectInputFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
            selectIfeInputPort = ifeportid_t::IFEInputPortRDI0;
            break;
            // Set output port to IPEInputPortFull for unknown formats
        case ChiStreamFormatNV12HEIF:
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw12:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatPD10:
        case ChiStreamFormatBlob:
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectIfeInputPort = ifeportid_t::IFEInputPortRDI0;
            NT_LOG_WARN( "Input port is unknown for CHISTREAMFORMAT %d on offline IFE generic pipeline. "
                "Setting to default input port %d...", selectInputFormat, selectIfeInputPort);
            break;
        default:
            selectIfeInputPort = ifeportid_t::IFEInputPortRDI0;
            NT_LOG_WARN( "Unknown format %d. Setting to default input port %d.",
                selectInputFormat, selectIfeInputPort);
                break;
        }
        NT_LOG_INFO( "Default input port for format %d is [%d]",
            selectInputFormat, selectIfeInputPort);

        // If user provided an input port, override the default from format
        IfePortId overrideIfeInputPort = static_cast<IfePortId>(CmdLineParser::GetInputPort());
        if (IfePortId::IFEPortInvalid != overrideIfeInputPort)
        {
            selectIfeInputPort = overrideIfeInputPort;
            NT_LOG_INFO( "Overriding default input port with [%d]", overrideIfeInputPort);
        }

        // Edit the pipeline descriptor with selected input port
        m_pipelineDescriptor.pNodes[0].nodeAllPorts.pInputPorts[0].portId                 = selectIfeInputPort;
        selectTargetCreateDescriptor.sourceTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIfeInputPort;


        UINT32 inputBufferIndex = 0;

        //Setup input buffer
        m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = selectIfeInputPort;
        m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_INPUT];


        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IFE;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
        m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId = selectIfeInputPort;

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
    *   PipelineUtils::SetupPipelineMFHDR
    *
    *   @brief
    *       Setup pipeline parameters for CVP Node DME and DME+ICA tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupPipelineMFHDR(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        UINT32 outputBufferIndex = 0;
        UINT32 inputBufferIndex  = 0;

        switch (type)
        {
        case PipelineType::OfflineIPEMFHDRSnapshot:
        case PipelineType::OfflineIPEMFHDRPreview:
        case PipelineType::OfflineIPEMFHDRSnapshotE2E:
        case PipelineType::OfflineIPEMFHDRPreviewE2E:
        {
            // --------------- Output Buffers ----------------
            // Full IPE port reference --> sink buffer
            if (type == PipelineType::OfflineIPEMFHDRSnapshot) {
                m_pipelineDescriptor     = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshot].pipelineCreateDesc;
                m_pipelineName           = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshot].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshot].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshot].sinkTarget;
            }
            else if (type == PipelineType::OfflineIPEMFHDRPreview) {
                m_pipelineDescriptor     = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreview].pipelineCreateDesc;
                m_pipelineName           = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreview].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreview].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreview].sinkTarget;
            }
            else if (type == PipelineType::OfflineIPEMFHDRSnapshotE2E) {
                m_pipelineDescriptor     = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshotE2E].pipelineCreateDesc;
                m_pipelineName           = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshotE2E].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshotE2E].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRSnapshotE2E].sinkTarget;
            }
            else if (type == PipelineType::OfflineIPEMFHDRPreviewE2E) {
                m_pipelineDescriptor     = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreviewE2E].pipelineCreateDesc;
                m_pipelineName           = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreviewE2E].pPipelineName;
                m_pipelineSrcTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreviewE2E].sourceTarget;
                m_pipelineSnkTgtDescInfo = Usecases8Target[TestOfflineMultiInputIPEId].pPipelineTargetCreateDesc[OfflineIPEMFHDRPreviewE2E].sinkTarget;
            }

            InitPortBufferDescriptor();

            if (type == PipelineType::OfflineIPEMFHDRSnapshot || type == PipelineType::OfflineIPEMFHDRPreview)
            {
                m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdFullRef;

                if (streamIdMap.find(StreamUsecases::IPEOutputPortFullRef) == streamIdMap.end())
                {
                    NT_LOG_ERROR("Cannot find chistream for IPEOutputPortFullRef");
                    return CDKResultENoSuch;
                }

                m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortFullRef];
                outputBufferIndex++;

                // DS4 IPE port reference --> sink buffer
                m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDS4Ref;

                if (streamIdMap.find(StreamUsecases::IPEOutputPortDS4Ref) == streamIdMap.end())
                {
                    NT_LOG_ERROR("Cannot find chistream for IPEOutputPortDS4Ref");
                    return CDKResultENoSuch;
                }

                m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDS4Ref];
            }
            else if(type == PipelineType::OfflineIPEMFHDRSnapshotE2E || type == PipelineType::OfflineIPEMFHDRPreviewE2E)
            {
                m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;

                if (streamIdMap.find(StreamUsecases::IPEOutputPortDisplay) == streamIdMap.end())
                {
                    NT_LOG_ERROR("Cannot find chistream for IPEOutputPortDisplay");
                    return CDKResultENoSuch;
                }

                m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEOutputPortDisplay];
            }
            else
            {
                NT_LOG_ERROR("Unknown PipelineType, SetupPipelines failed!");
                return CDKResultEFailed;
            }

            // --------------- Input Buffers ----------------
            // IPE Input port full
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFull;

            if (streamIdMap.find(StreamUsecases::IPEInputPortFull) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEInputPortFull");
                return CDKResultENoSuch;
            }

            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortFull];

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId =
                CSLIPEInputPortId::CSLIPEInputPortIdFull;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            inputBufferIndex++;

            // IPE Inputport full reference
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdFullRef;

            if (streamIdMap.find(StreamUsecases::IPEInputPortFullRef) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEInputPortFullRef");
                return CDKResultENoSuch;
            }

            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortFullRef];

            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId =
                CSLIPEInputPortId::CSLIPEInputPortIdFullRef;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            inputBufferIndex++;

            if (type == PipelineType::OfflineIPEMFHDRSnapshot || type == PipelineType::OfflineIPEMFHDRSnapshotE2E) {
                // IPE Inputport2 full reference
                m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInput2PortIdFull;

                if (streamIdMap.find(StreamUsecases::IPEInputPort2Full) == streamIdMap.end())
                {
                    NT_LOG_ERROR("Cannot find chistream for IPEInputPort2Full");
                    return CDKResultENoSuch;
                }

                m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPort2Full];
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId =
                    CSLIPEInputPortId::CSLIPEInput2PortIdFull;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = 0;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = 0;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
                inputBufferIndex++;
            }

            // IPE Inputport DS4
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdDS4;

            if (streamIdMap.find(StreamUsecases::IPEInputPortDS4) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEInputPortDS4");
                return CDKResultENoSuch;
            }

            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortDS4];
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId =
                CSLIPEInputPortId::CSLIPEInputPortIdDS4;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = ICAMinWidthPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = ICAMinHeightPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            inputBufferIndex++;

            // IPE Inputport DS4 ref
            m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInputPortIdDS4Ref;

            if (streamIdMap.find(StreamUsecases::IPEInputPortDS4Ref) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for IPEInputPortDS4Ref");
                return CDKResultENoSuch;
            }

            m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPortDS4Ref];
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId =
                CSLIPEInputPortId::CSLIPEInputPortIdDS4Ref;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = ICAMinWidthPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = ICAMinWidthPixels;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
            m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            inputBufferIndex++;

            if (type == PipelineType::OfflineIPEMFHDRSnapshot || type == PipelineType::OfflineIPEMFHDRSnapshotE2E) {
                // IPE Inputport2 DS4
                m_pipelineMultiInputBuffer[inputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeId = CamX::IPE;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodeInstanceId = 0;
                m_pipelineMultiInputBuffer[inputBufferIndex].pNodePort->nodePortId = CSLIPEInputPortId::CSLIPEInput2PortIdDSX;

                if (streamIdMap.find(StreamUsecases::IPEInputPort2DSX) == streamIdMap.end())
                {
                    NT_LOG_ERROR("Cannot find chistream for IPEInputPort2DSX");
                    return CDKResultENoSuch;
                }

                m_pipelineMultiInputBuffer[inputBufferIndex].pStream = streamIdMap[StreamUsecases::IPEInputPort2DSX];
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeId = CamX::IPE;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodeInstanceId = 0;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].nodePort.nodePortId =
                    CSLIPEInputPortId::CSLIPEInput2PortIdDSX;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.width = 3840;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.maxDimension.height = 2160;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.width = ICAMinWidthPixels;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.minDimension.height = ICAMinWidthPixels;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.width = 1920;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.optimalDimension.height = 1080;
                m_pipelineMultiInputBufferRequirements[inputBufferIndex].bufferOptions.size = sizeof(CHIBUFFEROPTIONS);
            }

            break;
        }
        default:
        {
            NT_LOG_ERROR("Invalid MFHDR pipeline type : %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
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
        ChiUsecase UsecaseE2EPreviewSnapshot = Usecases8Target[UsecaseE2EPreviewSnapshotId];
        INT pipelineId = -1;
        switch (type)
        {
        case PipelineType::PreviewJpegSnapshot:
            pipelineId = fetchPipelineID(UsecaseE2EPreviewSnapshot, "PreviewJpegSnapshot");
            break;
        case PipelineType::PreviewYuvSnapshot:
            pipelineId = fetchPipelineID(UsecaseE2EPreviewSnapshot, "PreviewYuvSnapshot");
            break;
        case PipelineType::PreviewJpegSnapshot3AStats:
            pipelineId = fetchPipelineID(UsecaseE2EPreviewSnapshot, "PreviewJpegSnapshot3AStats");
            break;
        case PipelineType::PreviewYuvSnapshot3AStats:
            pipelineId = fetchPipelineID(UsecaseE2EPreviewSnapshot, "PreviewYuvSnapshot3AStats");
            break;
        default:
            NT_LOG_ERROR("Invalid UsecaseE2EPreviewSnapshot pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        if (pipelineId < 0)
        {
            NT_LOG_ERROR("Invalid pipeline name. Please check topology XML.");
            return CDKResultENoSuch;
        }

        m_pipelineDescriptor     = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
        m_pipelineName           = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].pPipelineName;
        m_pipelineSrcTgtDescInfo = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].sourceTarget;
        m_pipelineSnkTgtDescInfo = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].sinkTarget;

        InitPortBufferDescriptor();

        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
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
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
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
    *   PipelineUtils::SetupE2EPreviewVideoCallback
    *
    *   @brief
    *       Setup Pipeline parameters for realtime preview callback snapshot & video
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map containing stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupE2EPreviewVideoCallback(std::map<StreamUsecases, ChiStream*> streamIdMap,
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
        ChiUsecase UsecaseE2EPreviewSnapshot = Usecases8Target[UsecaseE2EPreviewSnapshotId];
        INT pipelineId = -1;
        UINT32 outputBufferIndex = 0;

        switch (type)
        {
        case PipelineType::LiveVideoSnapshot:
            pipelineId = fetchPipelineID(UsecaseE2EPreviewSnapshot, "LiveVideoSnapshot");
            break;
        case PipelineType::PreviewCallbackSnapshotThumbnail:
            pipelineId = fetchPipelineID(UsecaseE2EPreviewSnapshot, "PreviewCallbackSnapshotThumbnail");
            break;
        default:
            NT_LOG_ERROR("Invalid UsecaseE2EPreviewSnapshot pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

        if (pipelineId < 0)
        {
            NT_LOG_ERROR("Invalid pipeline name. Please check topology XML.");
            return CDKResultENoSuch;
        }

        m_pipelineDescriptor     = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].pipelineCreateDesc;
        m_pipelineName           = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].pPipelineName;
        m_pipelineSrcTgtDescInfo = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].sourceTarget;
        m_pipelineSnkTgtDescInfo = UsecaseE2EPreviewSnapshot.pPipelineTargetCreateDesc[pipelineId].sinkTarget;

        InitPortBufferDescriptor();

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEW];
        outputBufferIndex++;

        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;

        if (type == PipelineType::LiveVideoSnapshot)
        {
            if (streamIdMap.find(StreamUsecases::VIDEO) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for VIDEO");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::VIDEO];
        }
        else if (type == PipelineType::PreviewCallbackSnapshotThumbnail)
        {
            if (streamIdMap.find(StreamUsecases::PREVIEWCALLBACK) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for PREVIEWCALLBACK");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::PREVIEWCALLBACK];
            outputBufferIndex++;

            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 1;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            if (streamIdMap.find(StreamUsecases::THUMBNAIL) == streamIdMap.end())
            {
                NT_LOG_ERROR("Cannot find chistream for THUMBNAIL");
                return CDKResultENoSuch;
            }
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::THUMBNAIL];
        }
        else
        {
            NT_LOG_ERROR("Invalid UsecaseE2EPreviewSnapshot pipeline type given: %d", static_cast<int>(type));
            return CDKResultEInvalidArg;
        }

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupE2EHDRPreviewVideo
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult PipelineUtils::SetupE2EHDRPreviewVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;
        UINT32 CONFIG_INDEX = 0;

        if (streamIdMap.find(StreamUsecases::IFEOutputPortRAW0) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortRAW0");
            return CDKResultENoSuch;
        }

        if(CmdLineParser::GetNumExposures() >= 2) // 2-exposure and 3-exposure
        {
            if (streamIdMap.find(StreamUsecases::IFEOutputPortRAW1) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortRAW1");
                return CDKResultENoSuch;
            }
            if(CmdLineParser::GetNumExposures() == 3) // 3-exposure
            {
                if (streamIdMap.find(StreamUsecases::IFEOutputPortRAW2) == streamIdMap.end())
                {
                    NT_LOG_ERROR( "Cannot find chistream for IFEOutputPortRAW2");
                    return CDKResultENoSuch;
                }
            }
        }

        switch (type)
        {
        case PipelineType::HDRPreviewVideo:
            if (streamIdMap.find(StreamUsecases::PREVIEW) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for PREVIEW");
                return CDKResultENoSuch;
            }
            if (streamIdMap.find(StreamUsecases::VIDEO) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for VIDEO");
                return CDKResultENoSuch;
            }
            selectUsecase = Usecases8Target[UsecaseE2EPreviewSnapshotId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[HDRPreviewVideo];
            break;
        case PipelineType::RealtimeIFEHDR:
            if (streamIdMap.find(StreamUsecases::CONFIG_OUTPUT) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for CONFIG_OUTPUT");
                return CDKResultENoSuch;
            }
            selectUsecase = Usecases4Target[UsecaseIFEHDRId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFEHDR];
            break;
        default:
            NT_LOG_ERROR("Unknown pipeline ID given!");
            return CDKResultEFailed;
        }

        m_pipelineDescriptor     = selectTargetCreateDescriptor.pipelineCreateDesc;
        m_pipelineName           = selectTargetCreateDescriptor.pPipelineName;
        m_pipelineSrcTgtDescInfo = selectTargetCreateDescriptor.sourceTarget;
        m_pipelineSnkTgtDescInfo = selectTargetCreateDescriptor.sinkTarget;

        ChangeLinksForHDR();
        InitPortBufferDescriptor();

        UINT32 outputBufferIndex = 0;

        if(type == PipelineType::RealtimeIFEHDR)
        {
            m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = ifeportid_t::IFEOutputPortFull;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::CONFIG_OUTPUT];
            outputBufferIndex++;
        }

        m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = ifeportid_t::IFEOutputPortRAW0;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::IFEOutputPortRAW0];

        if(CmdLineParser::GetNumExposures() >= 2) // 2-exposure and 3-exposure
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = ifeportid_t::IFEOutputPortRAW1;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::IFEOutputPortRAW1];

            if(CmdLineParser::GetNumExposures() == 3) // 3-exposure
            {
                outputBufferIndex++;
                m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IFE;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
                m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = ifeportid_t::IFEOutputPortRAW2;
                m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::IFEOutputPortRAW2];
            }
        }

        if(type == PipelineType::HDRPreviewVideo)
        {
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::PREVIEW];

            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size                      = sizeof(m_pPipelineOutputBuffer);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId         = CamX::IPE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId     = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream                   = streamIdMap[StreamUsecases::VIDEO];
        }
        return CDKResultSuccess;
    }

    /**************************************************************************************************
    *   PipelineUtils::SetupRealtimePipelineIFELite
    *
    *   @brief
    *       Setup pipeline parameters for  IFE Lite tests
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *       [in]  Pipeline                                type            differentiate between pipelines
    *   @return
    *       CDKResult result
    **************************************************************************************************/
    CDKResult PipelineUtils::SetupRealtimePipelineIFELite(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        uint32_t CONFIG_INDEX = 0;
        // Select usecase and target create descriptor based on pipeline type
        switch (type)
        {
        case PipelineType::RealtimeIFELiteTLBGStatsOut:
            selectUsecase = Usecases4Target[UsecaseGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFELiteTLBGStatsOut];
            CONFIG_INDEX = 1;
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime IFE Lite Generic pipeline type given : %d", static_cast<int>(type));
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

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
            CONFIG_INDEX = 0;
        }

        IfePortId selectIfePort;
        CHISTREAMFORMAT selectFormat = streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format;

        // Set the default output port based on format
        switch (selectFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
        case ChiStreamFormatNV12HEIF:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            break;
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatRaw12:
            selectIfePort = ifeportid_t::IFELITEOutputPortPreProcessRaw;
            break;
        case ChiStreamFormatPD10:
            selectIfePort = ifeportid_t::IFEOutputPortDS4;
            break;
        case ChiStreamFormatBlob:
            selectIfePort = ifeportid_t::IFEOutputPortStatsRS;
            break;
        // Formats which we don't know the output port on IFE
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Output port is unknown for CHISTREAMFORMAT %d on IFE Lite generic pipeline. "
                "Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        default:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Unknown format %d. Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        }
        NT_LOG_INFO( "Default output port for format %d is [%d]", selectFormat, selectIfePort);

        // If user provided an output port, override the default from format
        IfePortId overrideIfePort = static_cast<IfePortId>(CmdLineParser::GetOutputPort());
        if (IfePortId::IFEPortInvalid != overrideIfePort)
        {
            selectIfePort = overrideIfePort;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideIfePort);
        }

        // Edit the pipeline descriptor with selected output port
        // TODO intelligently find CONFIG_INDEX by going through links, find where source is IFE (what we want based on port
        // and instance)
        // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
        // buffers). Here it is the first output (0) buffer in all cases.
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectIfePort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId = selectIfePort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIfePort;

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        if (type == PipelineType::RealtimeIFELiteTLBGStatsOut)
        {
            // Setup static full output buffer
            outputBufferIndex++;
            m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
            m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFELITEOutputPortStatsBG;
            m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFELITEOutputPortStatsBG];
            if (streamIdMap.find(StreamUsecases::IFELITEOutputPortStatsBG) == streamIdMap.end())
            {
                NT_LOG_ERROR( "Cannot find chistream for IFELITEOutputPortStatsTLBG");
                return CDKResultENoSuch;
            }
        }

        return CDKResultSuccess;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupRealtimePipelineIFELiteRaw
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult PipelineUtils::SetupRealtimePipelineIFELiteRaw(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type)
    {
        ChiUsecase selectUsecase;
        ChiPipelineTargetCreateDescriptor selectTargetCreateDescriptor;

        uint32_t CONFIG_INDEX = 0;
        // Select usecase and target create descriptor based on pipeline type
        switch (type)
        {
        case PipelineType::RealtimeIFELiteRawTLBGStatsOut:
            selectUsecase = Usecases4Target[UsecaseGeneric2StreamsId];
            selectTargetCreateDescriptor = selectUsecase.pPipelineTargetCreateDesc[IFELiteRawTLBGStatsOut];
            CONFIG_INDEX = 1;
            break;
        default:
            NT_LOG_ERROR( "Invalid Realtime IFE Lite Generic pipeline type given : %d", static_cast<int>(type));
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

        if (CmdLineParser::RemoveSensor())
        {
            ConvertSensorPipelineToTestgenPipeline();
            CONFIG_INDEX = 0;
        }

        IfePortId selectIfePort;
        CHISTREAMFORMAT selectFormat = streamIdMap[StreamUsecases::CONFIG_OUTPUT]->format;

        // Set the default output port based on format
        switch (selectFormat)
        {
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatP010:
        case ChiStreamFormatNV12HEIF:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            break;
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw64:
        case ChiStreamFormatRaw12:
            selectIfePort = ifeportid_t::IFEOutputPortRDI0;
            break;
        case ChiStreamFormatPD10:
            selectIfePort = ifeportid_t::IFEOutputPortDS4;
            break;
        case ChiStreamFormatBlob:
            selectIfePort = ifeportid_t::IFEOutputPortStatsRS;
            break;
        // Formats which we don't know the output port on IFE
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Output port is unknown for CHISTREAMFORMAT %d on IFE Lite generic pipeline. "
                "Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        default:
            selectIfePort = ifeportid_t::IFEOutputPortFull;
            NT_LOG_WARN( "Unknown format %d. Setting to default output port %d...", selectFormat, selectIfePort);
            break;
        }
        NT_LOG_INFO( "Default output port for format %d is [%d]", selectFormat, selectIfePort);

        // If user provided an output port, override the default from format
        IfePortId overrideIfePort = static_cast<IfePortId>(CmdLineParser::GetOutputPort());
        if (IfePortId::IFEPortInvalid != overrideIfePort)
        {
            selectIfePort = overrideIfePort;
            NT_LOG_INFO( "Overriding default output port with [%d]", overrideIfePort);
        }

        // Edit the pipeline descriptor with selected output port
        // TODO intelligently find CONFIG_INDEX by going through links, find where source is IFE (what we want based on port
        // and instance)
        // TODO intelligently find index of the target buffer whichever we want to override (in case of multiple output
        // buffers). Here it is the first output (0) buffer in all cases.
        m_pipelineDescriptor.pLinks[CONFIG_INDEX].srcNode.nodePortId = selectIfePort;
        m_pipelineDescriptor.pNodes[CONFIG_INDEX].nodeAllPorts.pOutputPorts[0].portId = selectIfePort;
        selectTargetCreateDescriptor.sinkTarget.pTargetPortDesc[0].pNodePort->nodePortId = selectIfePort;

        // Setup configurable output buffer
        UINT32 outputBufferIndex = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = selectIfePort;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::CONFIG_OUTPUT];

        // Setup static full output buffer
        outputBufferIndex++;
        if (streamIdMap.find(StreamUsecases::IFELITEOutputPortPreProcessRaw) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFELITEOutputPortPreProcessRaw");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFELITEOutputPortPreProcessRaw;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFELITEOutputPortPreProcessRaw];

        //Setup stats TLBG Buffer
        outputBufferIndex++;
        if (streamIdMap.find(StreamUsecases::IFELITEOutputPortStatsBG) == streamIdMap.end())
        {
            NT_LOG_ERROR( "Cannot find chistream for IFELITEOutputPortStatsTLBG");
            return CDKResultENoSuch;
        }
        m_pPipelineOutputBuffer[outputBufferIndex].size = sizeof(CHIPORTBUFFERDESCRIPTOR);
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeId = CamX::IFE;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodeInstanceId = 0;
        m_pPipelineOutputBuffer[outputBufferIndex].pNodePort->nodePortId = ifeportid_t::IFELITEOutputPortStatsBG;
        m_pPipelineOutputBuffer[outputBufferIndex].pStream = streamIdMap[StreamUsecases::IFELITEOutputPortStatsBG];

        return CDKResultSuccess;
    }

}
