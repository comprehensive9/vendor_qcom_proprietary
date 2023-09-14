//*************************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __COMMON_CHI_PIPELINE_UTILS__
#define __COMMON_CHI_PIPELINE_UTILS__

#include "g_testpipelines.h"
#include "camxcsl.h"
#include "camxcslresourcedefs.h"
#include "camxhwdefs.h"
#include "camxdefs.h"
#include "camera3stream.h"

namespace chitests {


    /// @brief PipelineType
    enum class PipelineType
    {
        // Titan realtime pipelines
        RealtimeIFE = 0,
        RealtimeIFEDualFoV,
        RealtimeIFE3A,
        RealTimeIPE,
        RealtimeIPE3A,
        RealtimeIFEGeneric,
        RealtimeIFEGeneric2Stream,
        RealtimeIFEDownscaleGeneric,
        RealtimeIFEGeneric2StreamDisplay,
        RealtimeIFEStats,
        RealtimeIFERDI0,
        RealtimeIFERawCamif,
        RealtimeIFERawLsc,
        RealtimeChiDepth,
        RealtimeBypassNode,
        RealtimeInplaceNode,
        RealtimePrevRDI,
        RealtimePrevRDIWithStats,
        RealtimePrevWithCallbackWithStats,
        RealtimePrevWith3AWrappers,
        RealtimePrevAndVideoWithStats,
        RealtimePrevWithMemcpy,
        RealtimePrevFromMemcpy,
        RealtimePrevHVX,
        RealtimeRDI1Only,
        RealtimeIPEJpeg,
        RealtimeBPSIPEJpeg,
        RealtimeFD,
        VendorTagWrite,
        RealtimeCVPDME,
        RealtimeCVPDMEDisplayPort,
        RealtimeCVPDMEICA,
        RealtimeIFEHDR,
        RealtimeIFELiteTLBGStatsOut,
        RealtimeIFELiteRawTLBGStatsOut,

        // Titan CVP Standalone
        OfflineCVPStandAlone,

        // Titan offline pipelines
        OfflineIFEGeneric,
        OfflineBPS,
        OfflineBPSGeneric,
        OfflineBPSGeneric2Stream,
        OfflineBPSDS4,
        OfflineIPEJPEG,
        OfflineIPEDisp,
        OfflineIPEVideo,
        OfflineIPEDispVideo,
        OfflineIPEBlurModule,
        OfflineIPEMFHDRSnapshot,
        OfflineIPEMFHDRPreview,
        OfflineIPEMFHDRSnapshotE2E,
        OfflineIPEMFHDRPreviewE2E,
        OfflineIPEGeneric,
        OfflineIPEGeneric2StreamDisplay,
        OfflineBPSIPE,
        OfflineBPSIPEJpeg,
        OfflineBPSIPEDispVideo,
        OfflineBPSStats,
        OfflineBPSStatsWithIPE,
        OfflineBPSBGStatsWithIPE,
        OfflineBPSHistStatsWithIPE,
        OfflineBPSDownscaleGeneric,
        OfflineJpegSnapshot,
        OfflineBPSGPUpipeline,
        OfflineFDPipeline,
        OfflineIPEFDPipeline,
        OfflineBPSWithIPEAndDS,
        OfflineExtBPSIPE,
        OfflineExtBPSIPEJPEG,
        OfflineDepthNode,
        OfflineChiGPUNode,

        // Titan E2E Pipelines
        PreviewJpegSnapshot,
        PreviewYuvSnapshot,
        PreviewJpegSnapshot3AStats,
        PreviewYuvSnapshot3AStats,
        LiveVideoSnapshot,
        PreviewCallbackSnapshotThumbnail,
        HDRPreviewVideo,

        // Mimas realtime pipelines
        RealtimeTFERDI0,
        RealtimeTFERDI1RawPlain16,
        RealtimeTFEStats,
        RealtimeTFE,
        RealtimeTFERDI1RawMIPI,
        RealtimeTFEGeneric,
        RealtimeTFEGeneric2Stream,
        RealtimePrevRDITFEOPE,
        RealtimeOPE,
        RealtimePrevWithMemcpyTFEOPE,
        RealtimePrevFromMemcpyTFEOPE,
        RealtimeBypassNodeTFEOPE,
        RealtimeTFERDIOutMIPI12,
        RealtimeTFE3A,

        // Mimas offline pipelines
        OfflineOPEGeneric,
        OfflineOPEGeneric2StreamDisplay,
        OfflineOPEGeneric3StreamDisplayStats,
        OfflineOPEJPEGSWSnapshot,
        OfflineOPEDisplayOnNV12,
        OfflineOPEDisplayOnNV21,
        OfflineOPEVideoNV12,
        OfflineOPEVideoNV21,
        OfflineOPEInputConfigOutNV12,
        OfflineOPEInputConfigOutNV21,
        OfflineOPERawNV12,
        OfflineOPERawNV21,
        OfflineOPEDisplayNV21VideoNV12,
        OfflineOPEDisplayNV12VideoNV21,
        OfflineOPEDispVideoNV12,
        OfflineOPEDispVideoNV21,
        OfflineJPEGSWSnapshot,
        OfflineOPEStats
    };

    class CommonPipelineUtils
    {
        public:
            CommonPipelineUtils() = default;
            ~CommonPipelineUtils() = default;

            INT32           m_socId = 0;

            UINT32 m_streamIdMapSize = 0;
            UINT32 m_totalOutputStreams = 0;
            void InitPortBufferDescriptor();
            void DestroyNodePorts(PipelineCreateData* pPipelineCreate);
            void ConvertSensorPipelineToTestgenPipeline();
            INT fetchPipelineID(ChiUsecase chiUsecase, const CHAR* commonPipelineName);
            INT getTotalOutputStreams(std::map<StreamUsecases, ChiStream*> m_streamIdMap);
            //static CSLTFEOutputPortId SelectTfePortByOutputFormat(CHISTREAMFORMAT streamFormat);
            //static CSLOPEOutputPortId SelectOpePortByOutputFormat(CHISTREAMFORMAT streamFormat);
            static CDKResult ChangeNodeProperty(CHINODE chiNode, UINT nodePropId);
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// CommonPipelineUtils::SwitchPortByType
            ///
            ///  @brief
            ///      Change node's port based on enum type
            ///  @param
            ///     [in]  UINT           NodeId                 change specific node's portId
            ///     [in]  switchport_t   switchPortType         port type to switch
            ///
            /// @return
            ///      CDKResult
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            CDKResult SwitchPortByType(UINT nodeId, CmdLineParser::switchport_t switchPortType);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// ChangeLinksForNumExposures
            ///
            /// @brief  Function to modify links in pipeline based on number of exposures
            /// @param  None
            /// @return None
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void ChangeLinksForHDR();

        protected:
            /// @brief Sensor ports
            typedef enum SensorPortId
            {
                SensorOutputPort0 = 0
            }sensorportid_t;

            /// @brief IfePorts
            typedef enum IfePortId
            {
                IFEPortInvalid                    = -1,
                IFEOutputPortFull                 = 0,
                IFEOutputPortDS4                  = 1,
                IFEOutputPortDS16                 = 2,
                IFEOutputPortCAMIFRaw             = 3,
                IFEOutputPortLSCRaw               = 4,
                IFEOutputPortGTMRaw               = 5,
                IFEOutputPortFD                   = 6,
                IFEOutputPortPDAF                 = 7,
                IFEOutputPortRDI0                 = 8,
                IFEOutputPortRDI1                 = 9,
                IFEOutputPortRDI2                 = 10,
                IFEOutputPortRDI3                 = 11,
                IFEOutputPortStatsRS              = 12,
                IFEOutputPortStatsCS              = 13,
                IFEOutputPortStatsIHIST           = 15,
                IFEOutputPortStatsBHIST           = 16,
                IFEOutputPortStatsHDRBE           = 17,
                IFEOutputPortStatsHDRBHIST        = 18,
                IFEOutputPortStatsTLBG            = 19,
                IFEOutputPortStatsBF              = 20,
                IFEOutputPortStatsAWBBG           = 21,
                IFEOutputPortDisplayFull          = 22,
                IFEOutputPortDisplayDS4           = 23,
                IFEOutputPortDisplayDS16          = 24,
                IFEOutputPortStatsDualPD          = 25,
                IFEOutputPortRDIRD                = 26,
                IFEOutputPortLCR                  = 27,
                IFEOutputPortPDHWPreProcessedPD   = 28,
                IFEOutputPortStatsBFW             = 29,
                IFEOutputPortStatsAECBE           = 30,
                IFEOutputPortStatsLTMDC           = 31,
                IFEOutputPortStatsGTMBHIST        = 32,
                IFEOutputPortSparsePDOut          = 33,
                IFELITEOutputPortStatsBG          = 34,
                IFELITEOutputPortPreProcessRaw    = 35,
                IFEOutputPortStatsAECBHIST        = 36,
                IFEOutputPortFullExposure1        = 100,
                IFEOutputPortFullExposure2        = 101,
                IFEOutputPortFullExposure3        = 102,
                IFEOutputPortDS4Exposure1         = 103,
                IFEOutputPortDS4Exposure2         = 104,
                IFEOutputPortDS4Exposure3         = 105,
                IFEOutputPortDS16Exposure1        = 106,
                IFEOutputPortDS16Exposure2        = 107,
                IFEOutputPortDS16Exposure3        = 108,
                IFEOutputPortDisplayFullExposure1 = 109,
                IFEOutputPortDisplayFullExposure2 = 110,
                IFEOutputPortDisplayFullExposure3 = 111,
                IFEOutputPortDisplayDS4Exposure1  = 112,
                IFEOutputPortDisplayDS4Exposure2  = 113,
                IFEOutputPortDisplayDS4Exposure3  = 114,
                IFEOutputPortDisplayDS16Exposure1 = 115,
                IFEOutputPortDisplayDS16Exposure2 = 116,
                IFEOutputPortDisplayDS16Exposure3 = 117,
                IFEOutputPortStatsBGExposure1     = 118,
                IFEOutputPortStatsBGExposure2     = 119,
                IFEOutputPortStatsBGExposure3     = 120,
                IFEOutputPortStatsBHISTExposure1  = 121,
                IFEOutputPortStatsBHISTExposure2  = 122,
                IFEOutputPortStatsBHISTExposure3  = 123,
                IFEOutputPortRAW0                 = 124,
                IFEOutputPortRAW1                 = 125,
                IFEOutputPortRAW2                 = 126,
                IFEOutputPortRAW3                 = 127,
                IFEOutputPortRAW4                 = 128,
                IFEOutputPortRAW5                 = 129,
                IFEOutputPortRAW6                 = 130,
                IFEOutputPortRAW7                 = 131,
                IFEOutputPortRAW8                 = 132,
                IFEOutputPortRAW9                 = 133,

                IFEInputPortCSIDTPG           = 0,
                IFEInputPortCAMIFTPG          = 1,
                IFEInputPortSensor            = 2,
                IFEInputPortRDI0              = 3,
                IFEInputPortRDI1              = 4,
                IFEInputPortRDI2              = 5,
                IFEInputPortDualPD            = 6,
                IFEInputPortCustomHWPrimary   = 7,
                IFEInputPortCustomHWAF        = 8,
                IFEInputPortCustomMETA        = 9,
                IFEInputPortCustomHWSecondary = 10

            }ifeportid_t;

            /// @brief BpsPorts
            typedef enum BpsPortId
            {
                BPSPortInvalid = -1,
                BPSInputPort = 0,
                BPSOutputPortFull,
                BPSOutputPortDS4,
                BPSOutputPortDS16,
                BPSOutputPortDS64,
                BPSOutputPortStatsBG,
                BPSOutputPortStatsBHist,
                BPSOutputPortReg1,
                BPSOutputPortReg2
            }bpsportid_t;

            /// @brief GpuPorts
            typedef enum GPUPortId
            {
                GPUInputPort = 0,
                GPUOutputPortFull = 0,
                GPUOutputPortDS4,
                GPUOutputPortDS16,
                GPUOutputPortDS64,
            }gpuportid_t;

            /// @brief CVPPorts
            typedef enum CVPPortId
            {
                CVPInstanceName0 = 0,
                CVPOutputPortVector = 0,
                CVPOutputPortData = 1,
                CVPOutputPortImage = 2,

                CVPInputPortTARIFEFull = 3,
                CVPInputPortREFIFEFull = 4,
                CVPInputPortTARIFEDS4 = 5,
                CVPInputPortREFIFEDS4 = 6,
                CVPInputPortTARIFEDS16 = 7,
                CVPInputPortREFIFEDS16 = 8,
                CVPInputPortData = 9,
                CVPInputPortData2 = 10,
                CVPInputPortData3 = 11,

                CVPOutputPortData2 = 16,
                CVPOutputPortData3 = 17,
                CVPOutputPortImage2 = 18,
                CVPOutputPortImage3 = 19,
                CVPMaxOutputPorts = 19,

                CVPInputPort2TARIFEFull = 20,
                CVPInputPort2REFIFEFull = 21,
                CVPInputPort2TARIFEDS4 = 22,
                CVPInputPort2REFIFEDS4 = 23,
                CVPInputPort2TARIFEDS16 = 24,
                CVPInputPort2REFIFEDS16 = 25,

                CVPInputPort3TARIFEFull = 26,
                CVPInputPort3REFIFEFull = 27,
                CVPInputPort3TARIFEDS4 = 28,
                CVPInputPort3REFIFEDS4 = 29,
                CVPInputPort3TARIFEDS16 = 30,
                CVPInputPort3REFIFEDS16 = 31,

                CVPInputPortDisplay = 32,
                CVPInputPortIFEDS4DELAY = 33,
                CVPMaxInputPorts = 33,
            } cvpportid_t;

            /// @brief JpegPorts
            typedef enum JpegAggPortId
            {
                JPEGAggregatorPortInvalid = -1,
                JPEGAggregatorInputPort0  =  0,
                JPEGAggregatorInputPort1  =  1,
                JPEGAggregatorOutputPort
            }jpegaggportid_t;

            /// @brief JpegPorts
            typedef enum JpegPortId
            {
                JPEGPortInvalid = -1,
                JPEGInputPort   =  0,
                JPEGOutputPort

            }jpegportid_t;

            /// @brief JpegswPorts
            typedef enum JpegswPortId
            {
                JPEGSWInputPortMain = 0,
                JPEGSWOutputPortMain = 2

            }jpegswportid_t;

            /// @brief FDHw
            typedef enum FDHwPortId
            {
                FDHwInputPortImage = 0,

            }fdhwportid_t;

            /// @brief FD manager
            typedef enum FDMgrPortId
            {
                FDManagerInputPort = 0,
                FDManagerOutputPort = 0

            }fdmgrportid_t;

            /// @brief ChiExternalNode
            typedef enum ExternalNodeId
            {
                ChiNodeInputPort0 = 0,
                ChiNodeOutputPort0 = 0,
                ChiNodeOutputPort1
            }externalportid_t;

            /// @brief HW FD Types
            typedef enum HwFdTypes
            {
                CvpFdHw = 0,
                ExtFdHw,
                MaxFdHwTypes
            }hwfdtype_t;

            static const UINT MAX_NODES = 20;
            static const UINT MAX_NODE_PORTS = 20;
            static const UINT MAX_LINKS = 20;
            static const UINT MAX_PORTS = 20;
            static const UINT MAX_OUTPUT_BUFFERS = 11;
            static const INT  INVALID_PORT = -1;
            static const INT  INVALID_CMD_INPUT = -1;
            static const UINT MAX_INPUT_BUFFERS = 6;

            CHIPIPELINECREATEDESCRIPTOR  m_pipelineDescriptor;
            ChiTargetPortDescriptorInfo  m_pipelineSrcTgtDescInfo;
            ChiTargetPortDescriptorInfo  m_pipelineSnkTgtDescInfo;
            CHINODE                      m_nodes[MAX_NODES];
            CHINODEPORTS                 m_ports[MAX_NODE_PORTS];
            CHINODELINK                  m_links[MAX_LINKS];
            CHIINPUTPORTDESCRIPTOR       m_inputPorts[MAX_PORTS];
            CHIOUTPUTPORTDESCRIPTOR      m_outputPorts[MAX_PORTS];
            CHILINKNODEDESCRIPTOR        m_linkNodeDescriptors[MAX_LINKS];
            CHIPORTBUFFERDESCRIPTOR      m_pPipelineOutputBuffer[MAX_OUTPUT_BUFFERS];
            CHIPORTBUFFERDESCRIPTOR      m_pipelineInputBuffer;
            CHIPIPELINEINPUTOPTIONS      m_pipelineInputBufferRequirements;
            CHIPORTBUFFERDESCRIPTOR      m_pipelineMultiInputBuffer[MAX_INPUT_BUFFERS];
            CHIPIPELINEINPUTOPTIONS      m_pipelineMultiInputBufferRequirements[MAX_INPUT_BUFFERS];
            const CHAR*                  m_pipelineName;
            PipelineCreateData*          m_pPipelineCreateData;
    };
}

#endif  //#ifndef COMMON_CHI_PIPELINE_UTILS
