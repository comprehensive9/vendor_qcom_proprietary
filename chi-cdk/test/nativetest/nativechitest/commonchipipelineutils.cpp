//*******************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************************

#include "commonchipipelineutils.h"
#include "cmdlineparser.h"
#include <regex>

namespace chitests {

    /**************************************************************************************************
    *   CommonPipelineUtils::InitPortBufferDescriptor
    *
    *   @brief
    *       Initialize Port buffer descriptor based on numInputs and numOutputs
    *   @return
    *       None
    **************************************************************************************************/
    void CommonPipelineUtils::InitPortBufferDescriptor()
    {
        // update outputs and inputs
        m_pPipelineCreateData->numOutputs = m_pipelineSnkTgtDescInfo.numTargets;
        m_pPipelineCreateData->numInputs  = m_pipelineSrcTgtDescInfo.numTargets;
        if (m_pipelineDescriptor.isRealTime && m_pipelineDescriptor.pNodes[0].nodeId == CamX::Sensor
            && m_pPipelineCreateData->numInputs == 0)
        {
            m_pPipelineCreateData->numInputs = 1;
        }

        UINT32 numNodePorts = 1;
        if (m_pipelineSrcTgtDescInfo.pTargetPortDesc)
        {
            numNodePorts = m_pipelineSrcTgtDescInfo.pTargetPortDesc->numNodePorts;
        }

        for (uint32_t ipInd = 0;
            ipInd < (m_pPipelineCreateData->numInputs < numNodePorts ? numNodePorts : m_pPipelineCreateData->numInputs) &&
            ipInd < MAX_INPUT_BUFFERS;
            ipInd++)
        {
            m_pipelineMultiInputBuffer[ipInd].pNodePort = SAFE_NEW() CHILINKNODEDESCRIPTOR;
            m_pipelineMultiInputBuffer[ipInd].numNodePorts = numNodePorts;
            m_pipelineMultiInputBuffer[ipInd].flagValue = 0;
            m_pipelineMultiInputBuffer[ipInd].pBufferNegotiationsOptions = NULL;
        }

        for (uint32_t opInd = 0; opInd < m_pPipelineCreateData->numOutputs && opInd < MAX_OUTPUT_BUFFERS; opInd++)
        {
            m_pPipelineOutputBuffer[opInd].pNodePort = SAFE_NEW() CHILINKNODEDESCRIPTOR;
            m_pPipelineOutputBuffer[opInd].numNodePorts = 1;
            m_pPipelineOutputBuffer[opInd].flagValue = 0;
            m_pPipelineOutputBuffer[opInd].pBufferNegotiationsOptions = NULL;
        }
    }

    /**************************************************************************************************
    *   CommonPipelineUtils::DestroyNodePorts
    *
    *   @brief
    *       Free memory for pNodePort allocated while initializing
    *   @param
    *       [in]  PipelineCreateData*     pPipelineCreate     Pointer to PipelineCreateData
    *   @return
    *       None
    **************************************************************************************************/
    void CommonPipelineUtils::DestroyNodePorts(PipelineCreateData* pPipelineCreate)
    {
        if (m_pipelineInputBuffer.pNodePort != nullptr)
        {
            delete m_pipelineInputBuffer.pNodePort;
            m_pipelineInputBuffer.pNodePort = nullptr;
        }

        for (uint32_t outputIndex = 0; outputIndex <= pPipelineCreate->numOutputs; outputIndex++)
        {
            if (m_pPipelineOutputBuffer[outputIndex].pNodePort != nullptr)
            {
                delete m_pPipelineOutputBuffer[outputIndex].pNodePort;
                m_pPipelineOutputBuffer[outputIndex].pNodePort = nullptr;
            }
        }
    }

    /**************************************************************************************************
    *   CommonPipelineUtils::fetchPipelineID
    *
    *   @brief
    *       Fetch the pipeline ID based on the name
    *   @param
    *       [in]  ChiUseCase      chiUsecase      chi nodes in the pipeline
    *       [in]  const CHAR*     pipelineName    Node Property ID
    *   @return
    *       INT  PipelineID
    **************************************************************************************************/
    INT CommonPipelineUtils::fetchPipelineID(ChiUsecase chiUsecase, const CHAR* commonPipelineName)
    {

        INT commonPipelineId = -1;
        std::regex pipelineNamePattern(("soc"+std::to_string(m_socId)+"_.*"+ std::string (commonPipelineName)).c_str());
        for (UINT pipelineID = 0; pipelineID < chiUsecase.numPipelines; pipelineID++)
        {
            if (regex_search(chiUsecase.pPipelineTargetCreateDesc[pipelineID].pPipelineName, pipelineNamePattern))
            {
                NT_LOG_INFO("returning SOC %s specific pipeline",std::to_string(m_socId).c_str());
                return pipelineID;
            }

            if (strcmp(chiUsecase.pPipelineTargetCreateDesc[pipelineID].pPipelineName, commonPipelineName) == 0)
            {
                NT_LOG_INFO("Common pipeline found");
                commonPipelineId = pipelineID;
            }
        }

        return commonPipelineId;
    }

    /**************************************************************************************************
    *   PipelineUtils::SelectTfePortByOutputFormat
    *
    *   @brief
    *       selects TFE Port based on the output format
    *   @param
    *       [in]  CHISTREAMFORMAT     streamFormat     ChiStreamFormat enum
    *   @return
    *       CSLTFEOutputPortId
    ************************************************************************************************
    CSLTFEOutputPortId CommonPipelineUtils::SelectTfePortByOutputFormat(CHISTREAMFORMAT streamFormat)
    {
        CSLTFEOutputPortId selectedTfePort;

        // If user provided an output port, override the default from format
        int userPickedPort = CmdLineParser::GetOutputPort();
        if (INVALID_PORT != userPickedPort)
        {
            selectedTfePort = static_cast<CSLTFEOutputPortId>(userPickedPort);
            NT_LOG_INFO("Overriding default output port with [%d]", selectedTfePort);
        }
        else
        {
            // Set the default output port based on format
            switch (streamFormat)
            {
                case ChiStreamFormatYCrCb420_SP:
                case ChiStreamFormatYCbCr420_888:
                case ChiStreamFormatImplDefined:
                case ChiStreamFormatUBWCTP10:
                case ChiStreamFormatUBWCNV12:
                case ChiStreamFormatUBWCNV124R:
                case ChiStreamFormatP010:
                case ChiStreamFormatNV12HEIF:
                    selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdFull;
                    break;
                case ChiStreamFormatRaw16:
                case ChiStreamFormatRawOpaque:
                case ChiStreamFormatRaw10:
                case ChiStreamFormatRaw64:
                    selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdRDI0;
                    break;
                case ChiStreamFormatBlob:
                    selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdStatBF;
                    break;
                    // Formats which we don't know the output port on TFE
                case ChiStreamFormatPD10:
                case ChiStreamFormatY8:
                case ChiStreamFormatY16:
                    selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdFull;;
                    NT_LOG_WARN("Output port is unknown for CHISTREAMFORMAT %d on TFE generic pipeline. "
                        "Setting to default output port %d...", streamFormat, selectedTfePort);
                    break;
                default:
                    selectedTfePort = CSLTFEOutputPortId::CSLTFEPortIdFull;
                    NT_LOG_WARN("Unknown format %d. Setting to default output port %d...", streamFormat, selectedTfePort);
                    break;
            }
            NT_LOG_INFO("Default output port for format %d is [%d]", streamFormat, selectedTfePort);
        }
        return selectedTfePort;
    }

    *************************************************************************************************
    *   PipelineUtils::SelectOpePortByOutputFormat
    *
    *   @brief
    *       selects TFE Port based on the output format
    *   @param
    *       [in]  CHISTREAMFORMAT     streamFormat     ChiStreamFormat enum
    *   @return
    *       CSLTFEOutputPortId
    ************************************************************************************************
    CSLOPEOutputPortId CommonPipelineUtils::SelectOpePortByOutputFormat(CHISTREAMFORMAT streamFormat)
    {
        CSLOPEOutputPortId selectedOpePort;

        // If user provided an output port, override the default from format
        int userPickedPort = CmdLineParser::GetOutputPort();
        if (INVALID_PORT != userPickedPort)
        {
            selectedOpePort = static_cast<CSLOPEOutputPortId>(userPickedPort);
            NT_LOG_INFO("Overriding default output port with [%d]", selectedOpePort);
        }
        else
        {
            // Set the default output port based on format
            switch (streamFormat)
            {
                case ChiStreamFormatYCrCb420_SP:
                case ChiStreamFormatYCbCr420_888:
                case ChiStreamFormatImplDefined:
                case ChiStreamFormatUBWCTP10:
                case ChiStreamFormatUBWCNV12:
                case ChiStreamFormatUBWCNV124R:
                case ChiStreamFormatP010:
                    selectedOpePort = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
                    break;
                case ChiStreamFormatBlob:
                    selectedOpePort = CSLOPEOutputPortId::CSLOPEOutputPortIDStatRS;
                    break;
                    // Set output port to OPEOutputPortVideo for unknown formats
                case ChiStreamFormatNV12HEIF:
                case ChiStreamFormatRaw16:
                case ChiStreamFormatRawOpaque:
                case ChiStreamFormatRaw10:
                case ChiStreamFormatRaw64:
                case ChiStreamFormatPD10:
                case ChiStreamFormatY8:
                case ChiStreamFormatY16:
                    selectedOpePort = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
                    NT_LOG_WARN("Output port is unknown for CHISTREAMFORMAT %d on OPE generic pipeline. "
                        "Setting to default output port %d...", streamFormat, selectedOpePort);
                    break;
                default:
                    selectedOpePort = CSLOPEOutputPortId::CSLOPEOutputPortIDDisplay;
                    NT_LOG_WARN("Unknown format %d. Setting to default output port %d...", selectedOpePort, selectedOpePort);
                    break;
            }
            NT_LOG_INFO("Default output port for format %d is [%d]", streamFormat, selectedOpePort);
        }
        return selectedOpePort;
    }

    ***/

    /**************************************************************************************************
    *   CommonPipelineUtils::ChangeNodeProperty
    *
    *   @brief
    *       Change node property of given node
    *   @param
    *       [in]  CHINODE      chiNode      chi nodes in the pipeline
    *       [in]  UINT         nodeProdId    Node Property ID
    *   @return
    *       CDKResult
    **************************************************************************************************/
    CDKResult CommonPipelineUtils::ChangeNodeProperty(CHINODE chiNode, UINT nodePropId)
    {
        CDKResult res = CDKResultSuccess;

        if (nullptr == chiNode.pNodeProperties)
        {
            NT_LOG_INFO("no node properties available for this node (instance) id %d in pipeline", chiNode.nodeId);
            return res;
        }

        switch(chiNode.nodeId)
        {
            case CamX::FDHw:
            case CamX::FDManager:
                for (UINT32 propId = 0; propId < chiNode.numProperties; propId++)
                {
                    if (chiNode.pNodeProperties[propId].id == nodePropId)
                    {
                        int fdHwType = CmdLineParser::GetHwFdType();
                        if (INVALID_CMD_INPUT != fdHwType)
                        {
                            if (hwfdtype_t::CvpFdHw == fdHwType)
                            {
                                // we cannot use int directly
                                chiNode.pNodeProperties[propId].pValue = "1";
                            }
                            else if (hwfdtype_t::ExtFdHw == fdHwType)
                            {
                                // we cannot use int directly
                                chiNode.pNodeProperties[propId].pValue = "0";
                            }
                            else if (hwfdtype_t::MaxFdHwTypes <= fdHwType)
                            {
                                res = CDKResultENoSuch;
                                NT_LOG_ERROR(0,"No Such FD HW Type");
                            }
                        }
                    }
                }
                break;
            default:
                NT_LOG_INFO("no property change implemented for node id %d", chiNode.nodeId);
                break;
        }
        return res;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CommonPipelineUtils::SwitchPortByType
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult CommonPipelineUtils::SwitchPortByType(UINT nodeID, CmdLineParser::switchport_t switchPortType)
    {
        CDKResult res  = CDKResultSuccess;
        ChiNode* pNode = m_pipelineDescriptor.pNodes;
        bool bIFEPresent = false;
        bool bIPEPresent = false;

        switch (switchPortType)
        {
            case CmdLineParser::switchport_t::IfeFulltoDisp:
            case CmdLineParser::switchport_t::IfeDisptoFull:
                if (CamX::IFE != nodeID)
                {
                    NT_LOG_ERROR("Switch port not compatible ! %u <> %d", nodeID, switchPortType);
                    return CDKResultEFailed;
                }
                break;
            case CmdLineParser::switchport_t::IpeDisptoVideo:
            case CmdLineParser::switchport_t::IpeVideotoDisp:
                if (CamX::IPE != nodeID)
                {
                    NT_LOG_ERROR("Switch port not compatible ! %u <> %d", nodeID, switchPortType);
                    return CDKResultEFailed;
                }
                break;
            default:
                NT_LOG_DEBUG("Invalid switch type %d", switchPortType);
                return res;
        }

        for (unsigned int chiNodeIndex = 0; chiNodeIndex < m_pipelineDescriptor.numNodes; ++chiNodeIndex)
        {
            if (pNode[chiNodeIndex].nodeId == nodeID)
            {
                NT_LOG_INFO("Switching port type on Node Id :%u ", nodeID);
                for (unsigned int propId = 0; propId < pNode[chiNodeIndex].nodeAllPorts.numOutputPorts; ++propId)
                {
                    switch (switchPortType)
                    {
                        case CmdLineParser::switchport_t::IfeFulltoDisp:
                        {
                            if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == IfePortId::IFEOutputPortFull)
                            {
                                NT_LOG_INFO("Switching portId from IFEOutputPortFull to IFEOutputPortDisplayFull");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = IfePortId::IFEOutputPortDisplayFull;
                                bIFEPresent = true;
                            }
                            else if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == IfePortId::IFEOutputPortDS4)
                            {
                                NT_LOG_INFO("Switching portId from IFEOutputPortDS4 to IFEOutputPortDisplayDS4");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = IfePortId::IFEOutputPortDisplayDS4;
                            }
                            else if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == IfePortId::IFEOutputPortDS16)
                            {
                                NT_LOG_INFO("Switching portId from IFEOutputPortDS16 to IFEOutputPortDisplayDS16");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = IfePortId::IFEOutputPortDisplayDS16;
                            }
                            break;
                        }
                        case CmdLineParser::switchport_t::IfeDisptoFull:
                        {
                            if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == IfePortId::IFEOutputPortDisplayFull)
                            {
                                NT_LOG_INFO("Switching portId from IFEOutputPortDisplayFull to IFEOutputPortFull");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = IfePortId::IFEOutputPortFull;
                                bIFEPresent = true;
                            }
                            else if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == IfePortId::IFEOutputPortDisplayDS4)
                            {
                                NT_LOG_INFO("Switching portId from IFEOutputPortDisplayDS4 to IFEOutputPortDS4");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = IfePortId::IFEOutputPortDS4;
                            }
                            else if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == IfePortId::IFEOutputPortDisplayDS16)
                            {
                                NT_LOG_INFO("Switching portId from IFEOutputPortDisplayDS16 to IFEOutputPortDS4");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = IfePortId::IFEOutputPortDS16;
                            }
                            break;
                        }
                        case CmdLineParser::switchport_t::IpeVideotoDisp:
                        {
                            if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == CSLIPEOutputPortId::CSLIPEOutputPortIdVideo)
                            {
                                NT_LOG_INFO("Switching portId from IPEOutputPortIdVideo to IPEOutputPortIdDisplay");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
                                bIPEPresent = true;
                            }
                            break;
                        }
                        case CmdLineParser::switchport_t::IpeDisptoVideo:
                        {
                            if (pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId == CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay)
                            {
                                NT_LOG_INFO("Switching portId from IPEOutputPortIdDisplay to IPEOutputPortIdVideo");
                                pNode[chiNodeIndex].nodeAllPorts.pOutputPorts[propId].portId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
                                bIPEPresent = true;
                            }
                            break;
                        }
                        default:
                        {
                            res = CDKResultEInvalidArg;
                            break;
                        }
                    }
                }
                break; //Todo: switch for mutilple nodes
            }
        }

        NT_LOG_INFO("Switching port Id(s) in link");
        for (unsigned int chiNodeLinkIndex = 0; chiNodeLinkIndex < m_pipelineDescriptor.numLinks; chiNodeLinkIndex++)
        {
            if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodeId == nodeID)
            {
                 switch (switchPortType)
                 {
                     case CmdLineParser::switchport_t::IfeFulltoDisp:
                     {
                         if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == IfePortId::IFEOutputPortFull)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = IfePortId::IFEOutputPortDisplayFull;
                         }
                         else if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == IfePortId::IFEOutputPortDS4)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = IfePortId::IFEOutputPortDisplayDS4;
                         }
                         else if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == IfePortId::IFEOutputPortDS16)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = IfePortId::IFEOutputPortDisplayDS16;
                         }
                         break;
                     }
                     case CmdLineParser::switchport_t::IfeDisptoFull:
                     {
                         if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == IfePortId::IFEOutputPortDisplayFull)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = IfePortId::IFEOutputPortFull;
                         }
                         else if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == IfePortId::IFEOutputPortDisplayDS4)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = IfePortId::IFEOutputPortDS4;
                         }
                         else if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == IfePortId::IFEOutputPortDisplayDS16)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = IfePortId::IFEOutputPortDS16;
                         }
                         break;
                     }
                     case CmdLineParser::switchport_t::IpeVideotoDisp:
                     {
                         if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == CSLIPEOutputPortId::CSLIPEOutputPortIdVideo)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
                         }
                         break;
                     }
                     case CmdLineParser::switchport_t::IpeDisptoVideo:
                     {
                         if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId == CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay)
                         {
                             m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
                         }
                         break;
                     }
                     default:
                         res = CDKResultEInvalidArg;
                         break;
                 }
            }
        }

        NT_LOG_INFO("Updating portId(s) in sink target");
        for (unsigned int chiSinkTargetIndex = 0; chiSinkTargetIndex < m_pipelineSnkTgtDescInfo.numTargets; chiSinkTargetIndex++)
        {
            for (unsigned int propId = 0; propId < m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].numNodePorts; ++propId)
            {
                if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodeId == nodeID)
                {
                    switch (switchPortType)
                    {
                    case CmdLineParser::switchport_t::IfeFulltoDisp:
                    {
                        if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == IfePortId::IFEOutputPortFull)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = IfePortId::IFEOutputPortDisplayFull;
                        }
                        else if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == IfePortId::IFEOutputPortDS4)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = IfePortId::IFEOutputPortDisplayDS4;
                        }
                        else if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == IfePortId::IFEOutputPortDS16)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = IfePortId::IFEOutputPortDisplayDS16;
                        }
                        break;
                    }
                    case CmdLineParser::switchport_t::IfeDisptoFull:
                    {
                        if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == IfePortId::IFEOutputPortDisplayFull)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = IfePortId::IFEOutputPortFull;
                        }
                        else if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == IfePortId::IFEOutputPortDisplayDS4)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = IfePortId::IFEOutputPortDS4;
                        }
                        else if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == IfePortId::IFEOutputPortDisplayDS16)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = IfePortId::IFEOutputPortDS16;
                        }
                        break;
                    }
                    case CmdLineParser::switchport_t::IpeVideotoDisp:
                    {
                        if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == CSLIPEOutputPortId::CSLIPEOutputPortIdVideo)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay;
                        }
                        break;
                    }
                    case CmdLineParser::switchport_t::IpeDisptoVideo:
                    {
                        if (m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId == CSLIPEOutputPortId::CSLIPEOutputPortIdDisplay)
                        {
                            m_pipelineSnkTgtDescInfo.pTargetPortDesc[chiSinkTargetIndex].pNodePort->nodePortId = CSLIPEOutputPortId::CSLIPEOutputPortIdVideo;
                        }
                        break;
                    }
                    default:
                        res = CDKResultEInvalidArg;
                        break;
                    }
                }
            }
        }

        //print node incompatible w/ switch port type
        switch (switchPortType)
        {
            case CmdLineParser::switchport_t::IfeFulltoDisp:
            case CmdLineParser::switchport_t::IfeDisptoFull:
                if(!bIFEPresent)
                {
                    NT_LOG_ERROR("Unable to find CamX::IFE to switch port");
                }
                break;
            case CmdLineParser::switchport_t::IpeDisptoVideo:
            case CmdLineParser::switchport_t::IpeVideotoDisp:
                if (!bIPEPresent)
                {
                    NT_LOG_ERROR("Unable to find CamX::IPE node to switch port");
                }
                break;
            default:
                break;
        }

        return res;
    }


    /**************************************************************************************************
    *   CommonPipelineUtils::ConvertSensorPipelineToTestgenPipeline
    *
    *   @brief
    *       Change the Sensor based RT pipeline to Testgen based RT pipeline
    **************************************************************************************************/
    void CommonPipelineUtils::ConvertSensorPipelineToTestgenPipeline()
    {
        NT_LOG_INFO("Converting Sensor based RT pipeline to Testgen based RT pipeline");

        static ChiNode* nodesArray = new ChiNode[m_pipelineDescriptor.numNodes - 1];

        bool bSensorNodeIdentified = false;
        for (unsigned long long int chiNodeIndex = 0; chiNodeIndex < (m_pipelineDescriptor.numNodes - 1); chiNodeIndex++)
        {
            if (m_pipelineDescriptor.pNodes[chiNodeIndex].nodeId == CamX::Sensor)
            {
                bSensorNodeIdentified = true;
            }

            // skip the sensor node
            if (bSensorNodeIdentified)
            {
                nodesArray[chiNodeIndex] = m_pipelineDescriptor.pNodes[chiNodeIndex + 1];
            }
            else
            {
                nodesArray[chiNodeIndex] = m_pipelineDescriptor.pNodes[chiNodeIndex];
            }

            // disconnecting IFE node from sensor input
            if (nodesArray[chiNodeIndex].nodeId == CamX::IFE)
            {
                nodesArray[chiNodeIndex].nodeAllPorts.numInputPorts = 0;
                nodesArray[chiNodeIndex].nodeAllPorts.pInputPorts = NULL;
            }

        }

        m_pipelineDescriptor.numNodes = m_pipelineDescriptor.numNodes - 1;
        m_pipelineDescriptor.pNodes = nodesArray;

        // adjusting links
        static ChiNodeLink* nodeLinksArray = new ChiNodeLink[m_pipelineDescriptor.numLinks - 1];

        bSensorNodeIdentified = false;
        for (unsigned long long int chiNodeLinkIndex = 0; chiNodeLinkIndex < (m_pipelineDescriptor.numLinks - 1);
            chiNodeLinkIndex++)
        {
            if (m_pipelineDescriptor.pLinks[chiNodeLinkIndex].srcNode.nodeId == CamX::Sensor)
            {
                bSensorNodeIdentified = true;
            }

            // skip the sensor node
            if (bSensorNodeIdentified)
            {
                nodeLinksArray[chiNodeLinkIndex] = m_pipelineDescriptor.pLinks[chiNodeLinkIndex + 1];
            }
            else
            {
                nodeLinksArray[chiNodeLinkIndex] = m_pipelineDescriptor.pLinks[chiNodeLinkIndex];
            }
        }

        m_pipelineDescriptor.numLinks = m_pipelineDescriptor.numLinks - 1;
        m_pipelineDescriptor.pLinks = nodeLinksArray;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChangeLinksForHDR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void CommonPipelineUtils::ChangeLinksForHDR()
    {
        // TODO: make this function generic; currently tailored for HDR tests, based on XML topology
        NT_LOG_WARN("Adjusting links for HDR streams based on number of exposures.");
        UINT32 nExposures    = CmdLineParser::GetNumExposures();
        UINT32 newLinksCount = m_pipelineDescriptor.numLinks - (DEFAULT_EXPOSURE_COUNT - nExposures);
        UINT8  nRawOuts      = 0; // number of raw out streams

        // 1. Modify the link descriptor to point to new links
        static ChiNodeLink* pNewLinks = new ChiNodeLink[newLinksCount];
        UINT32 newLinkIndex = 0;
        for (UINT32 oldLinkIndex = 0; oldLinkIndex < m_pipelineDescriptor.numLinks; oldLinkIndex++)
        {
            // todo: restrict links from sensor to IFE based on exposure count
            // restrict SFE RAW out streams based on exposure count
            if( m_pipelineDescriptor.pLinks[oldLinkIndex].srcNode.nodePortId == ifeportid_t::IFEOutputPortRAW0 ||
                m_pipelineDescriptor.pLinks[oldLinkIndex].srcNode.nodePortId == ifeportid_t::IFEOutputPortRAW1 ||
                m_pipelineDescriptor.pLinks[oldLinkIndex].srcNode.nodePortId == ifeportid_t::IFEOutputPortRAW2 )
            {
                if(nRawOuts < nExposures)
                {
                    pNewLinks[newLinkIndex] = m_pipelineDescriptor.pLinks[oldLinkIndex];
                    nRawOuts++;
                    newLinkIndex++;
                }
            }
            else // include all other links
            {
                pNewLinks[newLinkIndex] = m_pipelineDescriptor.pLinks[oldLinkIndex];
                newLinkIndex++;
            }
        }
        m_pipelineDescriptor.numLinks = newLinksCount;
        m_pipelineDescriptor.pLinks = pNewLinks;

        // 2. Modify node ports
        for(UINT nodeId = 0; nodeId < m_pipelineDescriptor.numNodes; nodeId++)
        {
            // update IFE output ports
            if(m_pipelineDescriptor.pNodes[nodeId].nodeId == 65536)
            {
                nRawOuts = 0;
                UINT32 nOldIFEOutPorts = m_pipelineDescriptor.pNodes[nodeId].nodeAllPorts.numOutputPorts; // temporary local copy
                // update number of output ports based on exposure count
                UINT32 nNewIFEOutPorts = nOldIFEOutPorts - (DEFAULT_EXPOSURE_COUNT - nExposures);
                UINT newPortIndex = 0;
                static CHIOUTPUTPORTDESCRIPTOR* pNewOutputPorts = new CHIOUTPUTPORTDESCRIPTOR[nNewIFEOutPorts];
                for(UINT oldPortIndex = 0; oldPortIndex < nOldIFEOutPorts; oldPortIndex++)
                {
                    UINT32 portId = m_pipelineDescriptor.pNodes[nodeId].nodeAllPorts.pOutputPorts[oldPortIndex].portId;
                    // restrict SFE RAW out streams based on exposure count
                    if( portId == ifeportid_t::IFEOutputPortRAW0 ||
                        portId == ifeportid_t::IFEOutputPortRAW1 ||
                        portId == ifeportid_t::IFEOutputPortRAW2)
                    {
                        if(nRawOuts < nExposures)
                        {
                            pNewOutputPorts[newPortIndex] = m_pipelineDescriptor.pNodes[nodeId].nodeAllPorts.pOutputPorts[oldPortIndex];
                            nRawOuts++;
                            newPortIndex++;
                        }
                    }
                    else
                    {
                        pNewOutputPorts[newPortIndex] = m_pipelineDescriptor.pNodes[nodeId].nodeAllPorts.pOutputPorts[oldPortIndex];
                        newPortIndex++;
                    }
                }
                // Finally update the node ports
                m_pipelineDescriptor.pNodes[nodeId].nodeAllPorts.numOutputPorts = nNewIFEOutPorts;
            }
        }

        // 3. Modify the sink target descriptor
        UINT32 nOldTargets = m_pipelineSnkTgtDescInfo.numTargets;
        UINT32 nNewTargets = nOldTargets - (DEFAULT_EXPOSURE_COUNT - nExposures);
        static ChiTargetPortDescriptor* pNewTargetPortDesc = new ChiTargetPortDescriptor[nNewTargets];
        nRawOuts = 0;
        UINT32 newSinkPortIndex = 0;
        for (UINT32 oldSinkPortIndex = 0; oldSinkPortIndex < nOldTargets; oldSinkPortIndex++)
        {
            UINT32 targetSrcPortId = m_pipelineSnkTgtDescInfo.pTargetPortDesc[oldSinkPortIndex].pNodePort[0].nodePortId;
            // restrict SFE RAW out streams based on exposure count
            if( targetSrcPortId == ifeportid_t::IFEOutputPortRAW0 ||
                targetSrcPortId == ifeportid_t::IFEOutputPortRAW1 ||
                targetSrcPortId == ifeportid_t::IFEOutputPortRAW2)
            {
                if (nRawOuts < nExposures)
                {
                    pNewTargetPortDesc[newSinkPortIndex] = m_pipelineSnkTgtDescInfo.pTargetPortDesc[oldSinkPortIndex];
                    nRawOuts++;
                    newSinkPortIndex++;
                }
            }
            else
            {
                pNewTargetPortDesc[newSinkPortIndex] = m_pipelineSnkTgtDescInfo.pTargetPortDesc[oldSinkPortIndex];
                newSinkPortIndex++;
            }
        }
        // Finally update the sink target descriptor
        m_pipelineSnkTgtDescInfo.numTargets = nNewTargets;
        m_pipelineSnkTgtDescInfo.pTargetPortDesc = pNewTargetPortDesc;
    }

    /**************************************************************************************************
    *   CommonPipelineUtils::getTotalOutputStreams
    *
    *   @brief
    *       Get the total output stream count for a usecase
    *   @param
    *       [in]  std::map<StreamUsecases, ChiStream*>    streamIdMap     Map conatining stream objects
    *   @return
    *        INT  m_outtputStreamCount
    **************************************************************************************************/
    INT CommonPipelineUtils::getTotalOutputStreams(std::map<StreamUsecases, ChiStream*> m_streamIdMap)
    {
        INT outputStreamCount = 0;
        std::map<StreamUsecases, ChiStream*>::iterator it = m_streamIdMap.begin();
        while (it != m_streamIdMap.end())
        {
            if (it->second->streamType == ChiStreamTypeOutput)
            {
                outputStreamCount++;
            }
            it++;
        }

        return outputStreamCount;
    }
}
