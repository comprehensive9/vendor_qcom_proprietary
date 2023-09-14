//*************************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __CHI_PIPELINE_UTILS__
#define __CHI_PIPELINE_UTILS__

#include "commonchipipelineutils.h"
#include "camera3stream.h"

namespace chitests {

    class PipelineUtils : public CommonPipelineUtils
    {
        public:
            PipelineUtils() = default;
            ~PipelineUtils() = default;

            CDKResult SetupPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineCreateData* pPipelineCreate,
                chitests::PipelineType type);

        private:
            // realtime pipeline
            CDKResult SetupRealtimePipelineIPE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineIFE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineIFERDIO(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupRealtimePipelineIFEStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineIFERawCamif(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupRealtimePipelineIFERawLsc(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupRealtimePipelineExternal(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePrevRDI(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupPreviewCallbackSnapshotWithThumbnail(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimeIPE3A(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePIPCam(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimeFD(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimeJpeg(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimeIFEDownscaleGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimeIFEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupCustomVendorWritePipeline(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupRealtimePipelineIFELite(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineIFELiteRaw(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);

            // offline pipeline
            CDKResult SetupOfflinePipelineIPEJPEG(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineIPEDisplay(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineBPS(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineBPSDS4(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineIPEVideo(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineIPEDispVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflinePipelineBPSIPE(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineBPSIPEJpeg(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineBPSIPEDispVideo(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineBPSStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineJpeg(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineBPSGPUPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflineFDPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineDepthNode(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineBPSWithIPEAndDSPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflineChiGPUNode(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflineChiExternalNode(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineIPEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineBPSGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineBPSDownscaleGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineIFEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);

            // common for RTP, OP and MP
            CDKResult SetupPipelineMFHDR(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupCVPPipelines(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);

            // Titan E2E pipeline setup
            CDKResult SetupE2EZSLPreviewSnapshot(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupE2EPreviewVideoCallback(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupE2EHDRPreviewVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
    };
}

#endif  //#ifndef CHI_PIPELINE_UTILS
