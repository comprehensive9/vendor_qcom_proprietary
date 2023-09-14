////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  recipe_test.cpp
/// @brief Definitions for the Recipe test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "diy.h"

namespace chitests
{

    // Initialize static variables
    NativeCHIX*        NativeCHIX::m_pTestObj;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TestDIYPipeline
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void NativeCHIX::TestDIYPipeline()
    {
        Setup();
        mUsecase.numPipelines = JsonParser::GetNumPipelines();
        mUsecase.pUsecaseName = JsonParser::GetUseCaseName().c_str();

        CreateSampleE2EUsecase();

        PrintUsecase();
    }

    // todo: delete this debug function after development
    void NativeCHIX::CreateSampleE2EUsecase()
    {
        mUsecase.pUsecaseName     = "UsecaseE2EPreviewSnapshot";
        mUsecase.streamConfigMode = 0; // hardcoded to 0 always

        // targets
        mUsecase.numTargets     = 2;
        ChiTarget** ppChiTargets = new ChiTarget*[mUsecase.numTargets];

        ppChiTargets[0] = new ChiTarget[1];
        ppChiTargets[0]->direction = ChiStreamTypeOutput;
        ppChiTargets[0]->dimension = { 0, 0, 4096, 2448 };
        ppChiTargets[0]->numFormats = 1;
        ChiBufferFormat* pBufferFormats1 = new ChiBufferFormat[1];
        pBufferFormats1[0] = ChiFormatYUV420NV12;
        ppChiTargets[0]->pBufferFormats = pBufferFormats1;
        ppChiTargets[0]->pChiStream = NULL;

        ppChiTargets[1] = new ChiTarget[1];
        ppChiTargets[1]->direction = ChiStreamTypeOutput;
        ppChiTargets[1]->dimension = { 0, 0, 4096, 2448 };
        ppChiTargets[1]->numFormats = 1;
        ChiBufferFormat* pBufferFormats2 = new ChiBufferFormat[1];
        pBufferFormats2[0] = ChiFormatYUV420NV12;
        ppChiTargets[1]->pBufferFormats = pBufferFormats2;
        ppChiTargets[1]->pChiStream = NULL;

        mUsecase.ppChiTargets = ppChiTargets;

        // pipelines
        mUsecase.numPipelines = 1;
        ChiPipelineTargetCreateDescriptor* pPipelineTargetCreateDesc = new ChiPipelineTargetCreateDescriptor[mUsecase.numPipelines];
        pPipelineTargetCreateDesc[0].pPipelineName = "PreviewYuvSnapshot";
        // ChiPipelineCreateDescriptor pipelineCreateDesc;
        pPipelineTargetCreateDesc[0].pipelineCreateDesc.size     = 0; // always hardcoded to 0
        pPipelineTargetCreateDesc[0].pipelineCreateDesc.numNodes = 5;
        CHINODE*      pNodes = new CHINODE[pPipelineTargetCreateDesc[0].pipelineCreateDesc.numNodes];
        CHINODEPORTS nodeAllPorts[pPipelineTargetCreateDesc[0].pipelineCreateDesc.numNodes];

        // node0
        pNodes[0].pNodeProperties = NULL;
        pNodes[0].nodeId          = 0;
        pNodes[0].nodeInstanceId  = 0;
        nodeAllPorts[0].numInputPorts = 0;
        nodeAllPorts[0].pInputPorts   = NULL; // ChiInputPortDescriptor
        nodeAllPorts[0].numOutputPorts= 1;
        CHIOUTPUTPORTDESCRIPTOR* pOutputPorts0 = new CHIOUTPUTPORTDESCRIPTOR[nodeAllPorts[0].numOutputPorts];
        pOutputPorts0[0] = {0, 0, 0, 0, 0, NULL};
        nodeAllPorts[0].pOutputPorts  = pOutputPorts0;
        pNodes[0].nodeAllPorts = nodeAllPorts[0];
        pNodes[0].numProperties = 0;           // always hardcoded
        pNodes[0].pruneProperties = {0, NULL}; // always hardcoded

        // node1
        pNodes[1].pNodeProperties = NULL;
        pNodes[1].nodeId          = 65536;
        pNodes[1].nodeInstanceId  = 0;
        nodeAllPorts[1].numInputPorts = 1;
        CHIINPUTPORTDESCRIPTOR*  pInputPorts1 = new CHIINPUTPORTDESCRIPTOR[nodeAllPorts[1].numInputPorts];
        pInputPorts1[0] = {2, 0, 0};
        nodeAllPorts[1].pInputPorts   = pInputPorts1;
        nodeAllPorts[1].numOutputPorts= 4;
        CHIOUTPUTPORTDESCRIPTOR* pOutputPorts1 = new CHIOUTPUTPORTDESCRIPTOR[nodeAllPorts[0].numOutputPorts];
        pOutputPorts1[0] ={0, 0, 0, 0, 0, NULL}; // IFEOutputPortFull
        pOutputPorts1[1] ={1, 0, 0, 0, 0, NULL}; // IFEOutputPortDS4
        pOutputPorts1[2] ={2, 0, 0, 0, 0, NULL}; // IFEOutputPortDS16
        pOutputPorts1[3] ={9, 0, 0, 0, 0, NULL}; // IFEOutputPortRDI1
        nodeAllPorts[1].pOutputPorts  = pOutputPorts1;
        pNodes[1].nodeAllPorts = nodeAllPorts[1];
        pNodes[1].numProperties = 0;           // always hardcoded
        pNodes[1].pruneProperties = {0, NULL}; // always hardcoded

        // node2
        pNodes[2].pNodeProperties = NULL;
        pNodes[2].nodeId          = 65538;
        pNodes[2].nodeInstanceId  = 0;
        nodeAllPorts[2].numInputPorts = 3;
        CHIINPUTPORTDESCRIPTOR* pInputPorts2 = new CHIINPUTPORTDESCRIPTOR[nodeAllPorts[2].numInputPorts];
        pInputPorts2[0] ={0, 0, 0};// IPEInputPortFull
        pInputPorts2[1] ={1, 0, 0};// IPEInputPortDS4
        pInputPorts2[2] ={2, 0, 0};// IPEInputPortDS16
        nodeAllPorts[2].pInputPorts   = pInputPorts2;
        nodeAllPorts[2].numOutputPorts= 1;
        CHIOUTPUTPORTDESCRIPTOR* pOutputPorts2 = new  CHIOUTPUTPORTDESCRIPTOR[nodeAllPorts[2].numOutputPorts];
        pOutputPorts2[0] = {8, 1, 1, 0, 0, NULL};
        nodeAllPorts[2].pOutputPorts  = pOutputPorts2;
        pNodes[2].nodeAllPorts = nodeAllPorts[2];
        pNodes[2].numProperties = 0;           // always hardcoded
        pNodes[2].pruneProperties = {0, NULL}; // always hardcoded

        // node3
        pNodes[3].pNodeProperties = NULL;
        pNodes[3].nodeId          = 65538;
        pNodes[3].nodeInstanceId  = 1;
        nodeAllPorts[3].numInputPorts = 4;
        CHIINPUTPORTDESCRIPTOR* pInputPorts3  = new CHIINPUTPORTDESCRIPTOR[nodeAllPorts[3].numInputPorts];
        pInputPorts3[0] = {1, 0, 0};// IPEInputPortDS4
        pInputPorts3[1] = {2, 0, 0};// IPEInputPortDS16
        pInputPorts3[2] = {3, 0, 0};// IPEInputPortDS64
        pInputPorts3[3] = {0, 0, 0};// IPEInputPortFull
        nodeAllPorts[3].pInputPorts   = pInputPorts3;
        nodeAllPorts[3].numOutputPorts= 1;
        CHIOUTPUTPORTDESCRIPTOR* pOutputPorts3 = new CHIOUTPUTPORTDESCRIPTOR[nodeAllPorts[3].numOutputPorts];
        pOutputPorts3[0] = {8, 1, 1, 0, 0, NULL};
        nodeAllPorts[3].pOutputPorts  = pOutputPorts3;
        pNodes[3].nodeAllPorts = nodeAllPorts[3];
        pNodes[3].numProperties = 0;           // always hardcoded
        pNodes[3].pruneProperties = {0, NULL}; // always hardcoded

        // node4
        pNodes[4].pNodeProperties = NULL;
        pNodes[4].nodeId          = 65539;
        pNodes[4].nodeInstanceId  = 0;
        nodeAllPorts[4].numInputPorts = 1;
        CHIINPUTPORTDESCRIPTOR*  pInputPorts4 = new CHIINPUTPORTDESCRIPTOR[nodeAllPorts[4].numInputPorts];
        pInputPorts4[0] = {0, 0, 0};
        nodeAllPorts[4].pInputPorts   = pInputPorts4;
        nodeAllPorts[4].numOutputPorts= 4;
        CHIOUTPUTPORTDESCRIPTOR* pOutputPorts4 = new CHIOUTPUTPORTDESCRIPTOR[nodeAllPorts[4].numOutputPorts];
        pOutputPorts4[0] = {2, 0, 0, 0, 0, NULL}; // BPSOutputPortDS4
        pOutputPorts4[1] = {3, 0, 0, 0, 0, NULL}; // BPSOutputPortDS16
        pOutputPorts4[2] = {4, 0, 0, 0, 0, NULL}; // BPSOutputPortDS64
        pOutputPorts4[3] = {1, 0, 0, 0, 0, NULL}; // BPSOutputPortFull
        nodeAllPorts[4].pOutputPorts  = pOutputPorts4;
        pNodes[4].nodeAllPorts = nodeAllPorts[4];
        pNodes[4].numProperties = 0;           // always hardcoded
        pNodes[4].pruneProperties = {0, NULL}; // always hardcoded

        pPipelineTargetCreateDesc[0].pipelineCreateDesc.pNodes = pNodes;

        // Links
        pPipelineTargetCreateDesc[0].pipelineCreateDesc.numLinks = 11;
        CHINODELINK* pLinks = new CHINODELINK[pPipelineTargetCreateDesc[0].pipelineCreateDesc.numLinks];

        // link0
        pLinks[0].srcNode = {0, 0, 0, 0};
        pLinks[0].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes0 = new CHILINKNODEDESCRIPTOR[pLinks[0].numDestNodes];
        pDestNodes0[0] = {65536, 0, 2, 0, {0, NULL}};
        pLinks[0].pDestNodes = pDestNodes0;
        pLinks[0].bufferProperties = {0};
        pLinks[0].linkProperties = {0, 0};

        // link1
        pLinks[1].srcNode = {65536, 0, 0, 0};
        pLinks[1].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes1 = new CHILINKNODEDESCRIPTOR[pLinks[1].numDestNodes];
        pDestNodes1[0] = {65538, 0, 0, 0, {0, NULL}};
        pLinks[1].pDestNodes = pDestNodes1;
        pLinks[1].bufferProperties = {ChiFormatUBWCTP10, 0, 8, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[1].linkProperties = {0, 0};

        // link2
        pLinks[2].srcNode = {65536, 0, 1, 0};
        pLinks[2].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes2 = new CHILINKNODEDESCRIPTOR[pLinks[2].numDestNodes];
        pDestNodes2[0] = {65538, 0, 0, 0, {0, NULL}};
        pLinks[2].pDestNodes = pDestNodes2;
        pLinks[2].bufferProperties = {ChiFormatPD10, 0, 6, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[2].linkProperties = {0, 0};

        // link3
        pLinks[3].srcNode = {65536, 0, 2, 0};
        pLinks[3].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes3 = new CHILINKNODEDESCRIPTOR[pLinks[3].numDestNodes];
        pDestNodes3[0] = {65538, 0, 2, 0, {0, NULL}};
        pLinks[3].pDestNodes = pDestNodes3;
        pLinks[3].bufferProperties = {ChiFormatPD10, 0, 6, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[3].linkProperties = {0, 0};

        // link4
        pLinks[4].srcNode = {65538, 0, 8, 0};
        pLinks[4].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes4 = new CHILINKNODEDESCRIPTOR[pLinks[4].numDestNodes];
        pDestNodes4[0] = {2, 0, 0, 0, {0, NULL}};
        pLinks[4].pDestNodes = pDestNodes4;
        pLinks[4].bufferProperties = {0};
        pLinks[4].linkProperties = {0, 0};

        // link5
        pLinks[5].srcNode = {65536, 0, 9, 0};
        pLinks[5].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes5 = new CHILINKNODEDESCRIPTOR[pLinks[5].numDestNodes];
        pDestNodes5[0] = {65539, 0, 0, 0, {0, NULL}};
        pLinks[5].pDestNodes = pDestNodes5;
        pLinks[5].bufferProperties = {ChiFormatRawMIPI, 0, 8, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[5].linkProperties = {0, 0};

        // link6
        pLinks[6].srcNode = {65539, 0, 2, 0};
        pLinks[6].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes6 = new CHILINKNODEDESCRIPTOR[pLinks[6].numDestNodes];
        pDestNodes6[0] = {65538, 1, 1, 0, {0, NULL}};
        pLinks[6].pDestNodes = pDestNodes6;
        pLinks[6].bufferProperties = {ChiFormatPD10, 0, 8, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[6].linkProperties = {0, 0};

        // link7
        pLinks[7].srcNode = {65539, 0, 3, 0};
        pLinks[7].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR* pDestNodes7 = new CHILINKNODEDESCRIPTOR[pLinks[7].numDestNodes];
        pDestNodes7[0] = {65538, 1, 2, 0, {0, NULL}};
        pLinks[7].pDestNodes = pDestNodes7;
        pLinks[7].bufferProperties = {ChiFormatPD10, 0, 8, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[7].linkProperties = {0, 0};

        // link8
        pLinks[8].srcNode = {65539, 0, 4, 0};
        pLinks[8].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR*  pDestNodes8 = new CHILINKNODEDESCRIPTOR[pLinks[8].numDestNodes];
        pDestNodes8[0] = {65538, 1, 3, 0, {0, NULL}};
        pLinks[8].pDestNodes = pDestNodes8;
        pLinks[8].bufferProperties = {ChiFormatPD10, 0, 8, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[8].linkProperties = {0, 0};

        // link9
        pLinks[9].srcNode = {65539, 0, 1, 0};
        pLinks[9].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR*  pDestNodes9 = new CHILINKNODEDESCRIPTOR[pLinks[9].numDestNodes];
        pDestNodes9[0] = {65538, 1, 0, 0, {0, NULL}};
        pLinks[9].pDestNodes = pDestNodes9;
        pLinks[9].bufferProperties = {ChiFormatUBWCTP10, 0, 8, 8, BufferHeapIon, BufferMemFlagHw};
        pLinks[9].linkProperties = {0, 0};

        // link10
        pLinks[10].srcNode = {65538, 1, 8, 0};
        pLinks[10].numDestNodes = 1;
        CHILINKNODEDESCRIPTOR*  pDestNodes10 = new CHILINKNODEDESCRIPTOR[pLinks[10].numDestNodes];
        pDestNodes10[0] = {2, 0, 0, 0, {0, NULL}};
        pLinks[10].pDestNodes = pDestNodes10;
        pLinks[10].bufferProperties = {0};
        pLinks[10].linkProperties = {0, 0};

        pPipelineTargetCreateDesc[0].pipelineCreateDesc.pLinks = pLinks;

        ChiTargetPortDescriptorInfo sinkTarget;
        sinkTarget.numTargets = 2;
        ChiTargetPortDescriptor* pTargetPortDesc = new ChiTargetPortDescriptor[sinkTarget.numTargets];

        // sinkTarget0
        pTargetPortDesc[0].pTargetName = "TARGET_CONFIG_BUFFER_PREVIEW";
        ChiBufferFormat formatSink0 = ChiFormatYUV420NV12;
        ChiTarget* pTarget0 = new ChiTarget[1];
        pTarget0[0] = {
                        ChiStreamTypeOutput,
                        {   // (MinW, MinH, MaxW, MaxH)
                            0, 0, 4096, 2448
                        },
                        1,
                        &formatSink0,
                        NULL
                    };
        pTargetPortDesc[0].pTarget = pTarget0;
        pTargetPortDesc[0].numNodePorts = 1;
        ChiLinkNodeDescriptor* pNodePort0 = new ChiLinkNodeDescriptor[pTargetPortDesc[0].numNodePorts];
        pNodePort0[0] = {65538, 0, 8 };
        pTargetPortDesc[0].pNodePort = pNodePort0;

        // sinkTarget1
        pTargetPortDesc[1].pTargetName = "TARGET_CONFIG_BUFFER_SNAPSHOT";
        ChiBufferFormat formatSink1 = ChiFormatYUV420NV12;
        ChiTarget* pTarget1 = new ChiTarget[1];
        pTarget1[0] = {
                        ChiStreamTypeOutput,
                        {   // (MinW, MinH, MaxW, MaxH)
                            0, 0, 4096, 2448
                        },
                        1,
                        &formatSink1,
                        NULL
                    };
        pTargetPortDesc[1].pTarget = pTarget1;
        pTargetPortDesc[1].numNodePorts = 1;
        ChiLinkNodeDescriptor* pNodePort1 = new ChiLinkNodeDescriptor[pTargetPortDesc[1].numNodePorts];
        pNodePort1[0] = {65538, 1, 8 };
        pTargetPortDesc[1].pNodePort = pNodePort1;

        sinkTarget.pTargetPortDesc = pTargetPortDesc;

        pPipelineTargetCreateDesc[0].sinkTarget = sinkTarget;
        pPipelineTargetCreateDesc[0].sourceTarget = {0, NULL};

        mUsecase.pPipelineTargetCreateDesc = pPipelineTargetCreateDesc;

        mUsecase.pTargetPruneSettings = NULL;
        mUsecase.isOriginalDescriptor = TRUE;
    }

    void NativeCHIX::Setup()
    {
        // Initialize parent class
        ChiTestCase::Setup();
        // Save instance so static functions can access later
        m_pTestObj = this;
        m_pChiMetadataUtil = nullptr;
        m_pRequiredStreams = nullptr;

        m_numStreams = 0;
    }

    void NativeCHIX::Teardown()
    {
        // Tear down parent class
        ChiTestCase::Teardown();
    }

    CDKResult NativeCHIX::SetupStreams()
    {
        return CDKResultSuccess;
    }

    CDKResult NativeCHIX::SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode)
    {
        NT_UNUSED_PARAM(cameraId);
        NT_UNUSED_PARAM(sensorMode);

        return CDKResultSuccess;
    }

    CDKResult NativeCHIX::CreateSessions()
    {
        return CDKResultSuccess;
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
    CDKResult NativeCHIX::CreateSessionHelper()
    {
        CDKResult result = CDKResultSuccess;
        return result;
    }

    /***************************************************************************************************************************
    *   NativeCHIX::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources / objects
    *   @param
    *       None
    *   @return
    *       None
    ***************************************************************************************************************************/
    void NativeCHIX::DestroyResources()
    {
    }


    /***************************************************************************************************************************
    *   RecipeTest::GenerateRealtimeCaptureRequest()
    *
    *   @brief
    *       Generate a realtime pipeline request
    *   @param
    *       [in] uint64_t   frameNumber     frame number associated with the request
    *   @return
    *       CDKResult success if request could be submitted or failure
    ***************************************************************************************************************************/
    CDKResult NativeCHIX::GenerateCaptureRequest(uint64_t frameNumber)
    {
        NT_UNUSED_PARAM(frameNumber);
        CDKResult result = CDKResultSuccess;

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PrintUsecase
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // todo: delete this debug function after development
    void NativeCHIX::PrintUsecase()
    {
        NT_LOG_DEBUG("---------- ChiUsecase Structure Data ----------");
        NT_LOG_DEBUG("pUsecaseName          : %s", mUsecase.pUsecaseName);
        NT_LOG_DEBUG("streamConfigMode      : %u", mUsecase.streamConfigMode);
        NT_LOG_DEBUG("numTargets            : %u", mUsecase.numTargets);
        for(UINT32 target = 0; target < mUsecase.numTargets; target++)
        {
            NT_LOG_DEBUG("[ChiTarget] %u, direction     : %d", target, mUsecase.ppChiTargets[target]->direction);
            NT_LOG_DEBUG("               BufferDimension: (%u, %u, %u, %u)",
                                                mUsecase.ppChiTargets[target]->dimension.minWidth,
                                                mUsecase.ppChiTargets[target]->dimension.minHeight,
                                                mUsecase.ppChiTargets[target]->dimension.maxWidth,
                                                mUsecase.ppChiTargets[target]->dimension.maxHeight);
            NT_LOG_DEBUG("               numFormats     : %u", mUsecase.ppChiTargets[target]->numFormats);
            for(UINT32 format = 0; format < mUsecase.ppChiTargets[target]->numFormats; format++)
            {
                NT_LOG_DEBUG("               [ChiBufferFormat] pBufferFormats : %u", mUsecase.ppChiTargets[target]->pBufferFormats[format]);
            }
            if(NULL != mUsecase.ppChiTargets[target]->pChiStream)
            {
                NT_LOG_DEBUG("               [ChiStream] streamType          : %d", mUsecase.ppChiTargets[target]->pChiStream->streamType);
                NT_LOG_DEBUG("                           width               : %u", mUsecase.ppChiTargets[target]->pChiStream->width);
                NT_LOG_DEBUG("                           height              : %u", mUsecase.ppChiTargets[target]->pChiStream->height);
                NT_LOG_DEBUG("                           format              : %d", mUsecase.ppChiTargets[target]->pChiStream->format);
                NT_LOG_DEBUG("                           grallocUsage        : %u", mUsecase.ppChiTargets[target]->pChiStream->grallocUsage);
                NT_LOG_DEBUG("                           maxNumBuffers       : %u", mUsecase.ppChiTargets[target]->pChiStream->maxNumBuffers);
                NT_LOG_DEBUG("                           pPrivateInfo        : %d", mUsecase.ppChiTargets[target]->pChiStream->pPrivateInfo);
                NT_LOG_DEBUG("                           dataspace           : %d", mUsecase.ppChiTargets[target]->pChiStream->dataspace);
                NT_LOG_DEBUG("                           rotation            : %d", mUsecase.ppChiTargets[target]->pChiStream->rotation);
                NT_LOG_DEBUG("                           pPrivateFrameworkId : %s", mUsecase.ppChiTargets[target]->pChiStream->pPrivateFrameworkId);

                NT_LOG_DEBUG("                           [CHIHALSTREAM]  id              : %u", mUsecase.ppChiTargets[target]->pChiStream->pHalStream->id);
                NT_LOG_DEBUG("                                           overrideFormat  : %u", mUsecase.ppChiTargets[target]->pChiStream->pHalStream->overrideFormat);
                NT_LOG_DEBUG("                                           producerUsage   : %d", mUsecase.ppChiTargets[target]->pChiStream->pHalStream->producerUsage);
                NT_LOG_DEBUG("                                           consumerUsage   : %d", mUsecase.ppChiTargets[target]->pChiStream->pHalStream->consumerUsage);
                NT_LOG_DEBUG("                                           maxBuffers      : %u", mUsecase.ppChiTargets[target]->pChiStream->pHalStream->maxBuffers);

                NT_LOG_DEBUG("                           [CHISTREAMPARAMS] planeStride   : %u", mUsecase.ppChiTargets[target]->pChiStream->streamParams.planeStride);
                NT_LOG_DEBUG("                                             sliceHeight   : %u", mUsecase.ppChiTargets[target]->pChiStream->streamParams.sliceHeight);
                NT_LOG_DEBUG("                                             streamFPS     : %u", mUsecase.ppChiTargets[target]->pChiStream->streamParams.streamFPS);
                NT_LOG_DEBUG("                                             bpp           : %u", mUsecase.ppChiTargets[target]->pChiStream->streamParams.bpp);

                NT_LOG_DEBUG("                           pPhysicalCameraId   : %s", mUsecase.ppChiTargets[target]->pChiStream->pPhysicalCameraId);
                NT_LOG_DEBUG("                           streamIntent        : %d", mUsecase.ppChiTargets[target]->pChiStream->streamIntent);
                NT_LOG_DEBUG("                           reserved            : %p", mUsecase.ppChiTargets[target]->pChiStream->reserved);
            }
        }
        NT_LOG_DEBUG("numPipelines          : %u", mUsecase.numPipelines);
        for(UINT32 pipelineId = 0; pipelineId < mUsecase.numPipelines; pipelineId++)
        {
            ChiPipelineTargetCreateDescriptor pipeline = mUsecase.pPipelineTargetCreateDesc[pipelineId];
            NT_LOG_DEBUG("[ChiPipelineTargetCreateDescriptor] %u pPipelineName: %s", pipelineId, pipeline.pPipelineName);
            NT_LOG_DEBUG("              [ChiPipelineCreateDescriptor]  size    : %u", pipeline.pipelineCreateDesc.size);
            NT_LOG_DEBUG("                                             numNodes: %u", pipeline.pipelineCreateDesc.numNodes);
            for(UINT32 nodeId = 0; nodeId < pipeline.pipelineCreateDesc.numNodes; nodeId++)
            {
                CHINODE node = pipeline.pipelineCreateDesc.pNodes[nodeId];
                NT_LOG_DEBUG("                                             [CHINODE] %u, pNodeProperties: %p", nodeId, node.pNodeProperties);
                NT_LOG_DEBUG("                                                           nodeId         : %u", node.nodeId);
                NT_LOG_DEBUG("                                                           nodeInstanceId : %u", node.nodeInstanceId);
                NT_LOG_DEBUG("                                                           [CHINODEPORTS] numInputPorts: %u", node.nodeAllPorts.numInputPorts);
                for(UINT32 inPortId = 0; inPortId < node.nodeAllPorts.numInputPorts; inPortId++)
                {
                    CHIINPUTPORTDESCRIPTOR inPort = node.nodeAllPorts.pInputPorts[inPortId];
                    NT_LOG_DEBUG("                                                                         [CHIINPUTPORTDESCRIPTOR] %u, portId: %u",
                                                                                    inPortId, inPort.portId);
                    NT_LOG_DEBUG("                                                                                                      isInputStreamBuffer: %u",
                                                                                    inPort.isInputStreamBuffer);
                    NT_LOG_DEBUG("                                                                                                      portSourceTypeId   : %u",
                                                                                    inPort.portSourceTypeId);
                }
                NT_LOG_DEBUG("                                                           [CHINODEPORTS] numOutputPorts: %u", node.nodeAllPorts.numOutputPorts);
                for(UINT32 outPortId = 0; outPortId < node.nodeAllPorts.numOutputPorts; outPortId++)
                {
                    CHIOUTPUTPORTDESCRIPTOR outPort = node.nodeAllPorts.pOutputPorts[outPortId];
                    NT_LOG_DEBUG("                                                                         [CHIOUTPUTPORTDESCRIPTOR] %u, portId: %u",
                                                                                    outPortId, outPort.portId);
                    NT_LOG_DEBUG("                                                                                                      isSinkPort: %u",
                                                                                    outPort.isSinkPort);
                    NT_LOG_DEBUG("                                                                                                      isOutputStreamBuffer: %u",
                                                                                    outPort.isOutputStreamBuffer);
                    NT_LOG_DEBUG("                                                                                                      portSourceTypeId   : %u",
                                                                                    outPort.portSourceTypeId);
                    NT_LOG_DEBUG("                                                                                                      numSourceIdsMapped   : %u",
                                                                                    outPort.numSourceIdsMapped);
                    NT_LOG_DEBUG("                                                                                                      pMappedSourceIds   : %p",
                                                                                    outPort.pMappedSourceIds);
                }
                NT_LOG_DEBUG("                                                           numProperties  : %u", node.numProperties);
                NT_LOG_DEBUG("                                                           pruneProperties: %p", node.pruneProperties);
            }
            NT_LOG_DEBUG("                                             numLinks: %u", pipeline.pipelineCreateDesc.numLinks);
            for(UINT32 linkId = 0; linkId < pipeline.pipelineCreateDesc.numLinks; linkId++)
            {
                CHINODELINK link = pipeline.pipelineCreateDesc.pLinks[linkId];
                NT_LOG_DEBUG("                                               [CHINODELINK] %u srcNode.nodeId: %u", linkId, link.srcNode.nodeId);
                NT_LOG_DEBUG("                                                                     nodeInstanceId   : %u", link.srcNode.nodeInstanceId);
                NT_LOG_DEBUG("                                                                     nodePortId       : %u", link.srcNode.nodePortId);
                NT_LOG_DEBUG("                                                                     portSourceTypeId : %u", link.srcNode.portSourceTypeId);
                NT_LOG_DEBUG("                                                                     pruneProperties  : %p", link.srcNode.pruneProperties);
                NT_LOG_DEBUG("                                                                numDestNodes: %u", link.numDestNodes);
                for(UINT32 nodeId = 0; nodeId < link.numDestNodes; nodeId++)
                {
                    CHILINKNODEDESCRIPTOR node = link.pDestNodes[nodeId];
                    NT_LOG_DEBUG("                                                            pDestNodes[%u] nodeId           : %u", nodeId, node.nodeId);
                    NT_LOG_DEBUG("                                                                           nodeInstanceId   : %u", node.nodeInstanceId);
                    NT_LOG_DEBUG("                                                                           nodePortId       : %u", node.nodePortId);
                    NT_LOG_DEBUG("                                                                           portSourceTypeId : %u", node.portSourceTypeId);
                    NT_LOG_DEBUG("                                                                           pruneProperties  : %p", node.pruneProperties);
                }
                NT_LOG_DEBUG("                                                               [CHILINKBUFFERPROPERTIES] bufferFormat: %u", link.bufferProperties.bufferFormat);
                NT_LOG_DEBUG("                                                                                         bufferSize: %u", link.bufferProperties.bufferSize);
                NT_LOG_DEBUG("                                                                                         bufferImmediateAllocCount: %u", link.bufferProperties.bufferImmediateAllocCount);
                NT_LOG_DEBUG("                                                                                         bufferQueueDepth: %u", link.bufferProperties.bufferQueueDepth);
                NT_LOG_DEBUG("                                                                                         bufferHeap: %u", link.bufferProperties.bufferHeap);
                NT_LOG_DEBUG("                                                                                         bufferFlags: %u", link.bufferProperties.bufferFlags);
                NT_LOG_DEBUG("                                                               [CHILINKPROPERTIES] isBatchedMode: %u", link.linkProperties.isBatchedMode);
                NT_LOG_DEBUG("                                                               [CHILINKPROPERTIES] linkFlags: %u", link.linkProperties.linkFlags);
            }
            NT_LOG_DEBUG("                                             isRealTime             : %u", pipeline.pipelineCreateDesc.isRealTime);
            NT_LOG_DEBUG("                                             numBatchedFrames       : %u", pipeline.pipelineCreateDesc.numBatchedFrames);
            NT_LOG_DEBUG("                                             maxFPSValue            : %u", pipeline.pipelineCreateDesc.maxFPSValue);
            NT_LOG_DEBUG("                                             cameraId               : %u", pipeline.pipelineCreateDesc.cameraId);
            NT_LOG_DEBUG("                                             hPipelineMetadata      : %p", pipeline.pipelineCreateDesc.hPipelineMetadata);
            NT_LOG_DEBUG("                                             HALOutputBufferCombined: %d", pipeline.pipelineCreateDesc.HALOutputBufferCombined);
            NT_LOG_DEBUG("                                             logicalCameraId        : %u", pipeline.pipelineCreateDesc.logicalCameraId);

            ChiTargetPortDescriptorInfo sinkTarget = pipeline.sinkTarget;
            for(UINT32 target = 0; target < sinkTarget.numTargets; target++)
            {
                ChiTargetPortDescriptor pTargetPortDesc = sinkTarget.pTargetPortDesc[target];
                NT_LOG_DEBUG("           [ChiTargetPortDescriptor] %u pTargetName: %s", target, pTargetPortDesc.pTargetName);
                NT_LOG_DEBUG("                                        [ChiTarget] direction: %u", pTargetPortDesc.pTarget->direction);
                NT_LOG_DEBUG("                                                    BufferDimension: (%u, %u, %u, %u)",
                                                                        pTargetPortDesc.pTarget->dimension.minWidth,
                                                                        pTargetPortDesc.pTarget->dimension.minHeight,
                                                                        pTargetPortDesc.pTarget->dimension.maxWidth,
                                                                        pTargetPortDesc.pTarget->dimension.maxHeight);
                NT_LOG_DEBUG("                                                     numFormats     : %u", pTargetPortDesc.pTarget->numFormats);
                for(UINT32 format = 0; format < pTargetPortDesc.pTarget->numFormats; format++)
                {
                    NT_LOG_DEBUG("                                                 pBufferFormats : %u", pTargetPortDesc.pTarget->pBufferFormats[format]);
                }
                if(NULL != pTargetPortDesc.pTarget->pChiStream)
                {
                    NT_LOG_DEBUG("               [ChiStream] streamType          : %d", pTargetPortDesc.pTarget->pChiStream->streamType);
                    NT_LOG_DEBUG("                           width               : %u", pTargetPortDesc.pTarget->pChiStream->width);
                    NT_LOG_DEBUG("                           height              : %u", pTargetPortDesc.pTarget->pChiStream->height);
                    NT_LOG_DEBUG("                           format              : %d", pTargetPortDesc.pTarget->pChiStream->format);
                    NT_LOG_DEBUG("                           grallocUsage        : %u", pTargetPortDesc.pTarget->pChiStream->grallocUsage);
                    NT_LOG_DEBUG("                           maxNumBuffers       : %u", pTargetPortDesc.pTarget->pChiStream->maxNumBuffers);
                    NT_LOG_DEBUG("                           pPrivateInfo        : %d", pTargetPortDesc.pTarget->pChiStream->pPrivateInfo);
                    NT_LOG_DEBUG("                           dataspace           : %d", pTargetPortDesc.pTarget->pChiStream->dataspace);
                    NT_LOG_DEBUG("                           rotation            : %d", pTargetPortDesc.pTarget->pChiStream->rotation);
                    NT_LOG_DEBUG("                           pPrivateFrameworkId : %s", pTargetPortDesc.pTarget->pChiStream->pPrivateFrameworkId);

                    NT_LOG_DEBUG("                           [CHIHALSTREAM]  id              : %u", pTargetPortDesc.pTarget->pChiStream->pHalStream->id);
                    NT_LOG_DEBUG("                                           overrideFormat  : %u", pTargetPortDesc.pTarget->pChiStream->pHalStream->overrideFormat);
                    NT_LOG_DEBUG("                                           producerUsage   : %d", pTargetPortDesc.pTarget->pChiStream->pHalStream->producerUsage);
                    NT_LOG_DEBUG("                                           consumerUsage   : %d", pTargetPortDesc.pTarget->pChiStream->pHalStream->consumerUsage);
                    NT_LOG_DEBUG("                                           maxBuffers      : %u", pTargetPortDesc.pTarget->pChiStream->pHalStream->maxBuffers);

                    NT_LOG_DEBUG("                           [CHISTREAMPARAMS] planeStride   : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.planeStride);
                    NT_LOG_DEBUG("                                             sliceHeight   : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.sliceHeight);
                    NT_LOG_DEBUG("                                             streamFPS     : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.streamFPS);
                    NT_LOG_DEBUG("                                             bpp           : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.bpp);

                    NT_LOG_DEBUG("                           pPhysicalCameraId   : %s", pTargetPortDesc.pTarget->pChiStream->pPhysicalCameraId);
                    NT_LOG_DEBUG("                           streamIntent        : %d", pTargetPortDesc.pTarget->pChiStream->streamIntent);
                    NT_LOG_DEBUG("                           reserved            : %p", pTargetPortDesc.pTarget->pChiStream->reserved);
                }

                NT_LOG_DEBUG("                                        numNodePorts: %u", pTargetPortDesc.numNodePorts);
                NT_LOG_DEBUG("                                        [ChiLinkNodeDescriptor] nodeId: %u", pTargetPortDesc.pNodePort->nodeId);
                NT_LOG_DEBUG("                                                                nodeInstanceId: %u", pTargetPortDesc.pNodePort->nodeInstanceId);
                NT_LOG_DEBUG("                                                                nodePortId: %u", pTargetPortDesc.pNodePort->nodePortId);
                NT_LOG_DEBUG("                                                                portSourceTypeId: %u", pTargetPortDesc.pNodePort->portSourceTypeId);
                NT_LOG_DEBUG("                                                                pruneProperties: %p", pTargetPortDesc.pNodePort->pruneProperties);
            }

            ChiTargetPortDescriptorInfo sourceTarget = pipeline.sourceTarget;
            for(UINT32 target = 0; target < sourceTarget.numTargets; target++)
            {
                ChiTargetPortDescriptor pTargetPortDesc = sourceTarget.pTargetPortDesc[target];
                NT_LOG_DEBUG("           [ChiTargetPortDescriptor] %u pTargetName: %s", target, pTargetPortDesc.pTargetName);
                NT_LOG_DEBUG("                                        [ChiTarget] direction: %u", pTargetPortDesc.pTarget->direction);
                NT_LOG_DEBUG("                                                    BufferDimension: (%u, %u, %u, %u)",
                                                                        pTargetPortDesc.pTarget->dimension.minWidth,
                                                                        pTargetPortDesc.pTarget->dimension.minHeight,
                                                                        pTargetPortDesc.pTarget->dimension.maxWidth,
                                                                        pTargetPortDesc.pTarget->dimension.maxHeight);
                NT_LOG_DEBUG("                                                     numFormats     : %u", pTargetPortDesc.pTarget->numFormats);
                for(UINT32 format = 0; format < pTargetPortDesc.pTarget->numFormats; format++)
                {
                    NT_LOG_DEBUG("                                                 pBufferFormats : %u", pTargetPortDesc.pTarget->pBufferFormats[format]);
                }
                if(NULL != pTargetPortDesc.pTarget->pChiStream)
                {
                    NT_LOG_DEBUG("               [ChiStream] streamType          : %d", pTargetPortDesc.pTarget->pChiStream->streamType);
                    NT_LOG_DEBUG("                           width               : %u", pTargetPortDesc.pTarget->pChiStream->width);
                    NT_LOG_DEBUG("                           height              : %u", pTargetPortDesc.pTarget->pChiStream->height);
                    NT_LOG_DEBUG("                           format              : %d", pTargetPortDesc.pTarget->pChiStream->format);
                    NT_LOG_DEBUG("                           grallocUsage        : %u", pTargetPortDesc.pTarget->pChiStream->grallocUsage);
                    NT_LOG_DEBUG("                           maxNumBuffers       : %u", pTargetPortDesc.pTarget->pChiStream->maxNumBuffers);
                    NT_LOG_DEBUG("                           pPrivateInfo        : %d", pTargetPortDesc.pTarget->pChiStream->pPrivateInfo);
                    NT_LOG_DEBUG("                           dataspace           : %d", pTargetPortDesc.pTarget->pChiStream->dataspace);
                    NT_LOG_DEBUG("                           rotation            : %d", pTargetPortDesc.pTarget->pChiStream->rotation);
                    NT_LOG_DEBUG("                           pPrivateFrameworkId : %s", pTargetPortDesc.pTarget->pChiStream->pPrivateFrameworkId);

                    NT_LOG_DEBUG("                           [CHIHALSTREAM]  id              : %u", pTargetPortDesc.pTarget->pChiStream->pHalStream->id);
                    NT_LOG_DEBUG("                                           overrideFormat  : %u", pTargetPortDesc.pTarget->pChiStream->pHalStream->overrideFormat);
                    NT_LOG_DEBUG("                                           producerUsage   : %d", pTargetPortDesc.pTarget->pChiStream->pHalStream->producerUsage);
                    NT_LOG_DEBUG("                                           consumerUsage   : %d", pTargetPortDesc.pTarget->pChiStream->pHalStream->consumerUsage);
                    NT_LOG_DEBUG("                                           maxBuffers      : %u", pTargetPortDesc.pTarget->pChiStream->pHalStream->maxBuffers);

                    NT_LOG_DEBUG("                           [CHISTREAMPARAMS] planeStride   : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.planeStride);
                    NT_LOG_DEBUG("                                             sliceHeight   : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.sliceHeight);
                    NT_LOG_DEBUG("                                             streamFPS     : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.streamFPS);
                    NT_LOG_DEBUG("                                             bpp           : %u", pTargetPortDesc.pTarget->pChiStream->streamParams.bpp);

                    NT_LOG_DEBUG("                           pPhysicalCameraId   : %s", pTargetPortDesc.pTarget->pChiStream->pPhysicalCameraId);
                    NT_LOG_DEBUG("                           streamIntent        : %d", pTargetPortDesc.pTarget->pChiStream->streamIntent);
                    NT_LOG_DEBUG("                           reserved            : %p", pTargetPortDesc.pTarget->pChiStream->reserved);
                }

                NT_LOG_DEBUG("                                        numNodePorts: %u", pTargetPortDesc.numNodePorts);
                NT_LOG_DEBUG("                                        [ChiLinkNodeDescriptor] nodeId: %u", pTargetPortDesc.pNodePort->nodeId);
                NT_LOG_DEBUG("                                                                nodeInstanceId: %u", pTargetPortDesc.pNodePort->nodeInstanceId);
                NT_LOG_DEBUG("                                                                nodePortId: %u", pTargetPortDesc.pNodePort->nodePortId);
                NT_LOG_DEBUG("                                                                portSourceTypeId: %u", pTargetPortDesc.pNodePort->portSourceTypeId);
                NT_LOG_DEBUG("                                                                pruneProperties: %p", pTargetPortDesc.pNodePort->pruneProperties);
            }
        }

        if(mUsecase.pTargetPruneSettings != nullptr)
        {
            NT_LOG_DEBUG("[PruneSettings]  numSettings : %u", mUsecase.pTargetPruneSettings->numSettings);
            NT_LOG_DEBUG("                 [PruneVariant] group : %u", mUsecase.pTargetPruneSettings->pVariants->group);
            NT_LOG_DEBUG("                                type  : %u", mUsecase.pTargetPruneSettings->pVariants->type);
        }
        NT_LOG_DEBUG("isOriginalDescriptor  : %u", mUsecase.isOriginalDescriptor);
    }
}