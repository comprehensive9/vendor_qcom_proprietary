//*************************************************************************************************
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __CHI_PIPELINE_UTILS__
#define __CHI_PIPELINE_UTILS__

#include "commonchipipelineutils.h"

namespace chitests {

    class PipelineUtils : public CommonPipelineUtils
    {
        public:
            PipelineUtils() = default;
            ~PipelineUtils() = default;

            CDKResult SetupPipeline(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineCreateData* pPipelineCreate,
                chitests::PipelineType type);

        private:

            // MIMAS testcases
            CDKResult SetupRealtimeTFEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflineOPEGeneric(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineTFEStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineTFERDI(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineTFE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelinePrevRDITFEOPE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineOPE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflinePipelineJPEGSW(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineOPEVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflinePipelineOPERaw(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflinePipelineOPEDisplayVideo(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimeTFEDownscaleGenericTestgen(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflinePipelineOPEDisplay(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupRealtimePipelineExternalTFEOPE(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupOfflinePipelineOPEJPEGSW(std::map<StreamUsecases, ChiStream*> streamIdMap);
            CDKResult SetupOfflinePipelineOPEStats(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);

            // Mimas E2E pipeline setup
            CDKResult SetupE2EZSLPreviewSnapshot(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
            CDKResult SetupE2EPreviewLiveVideoSnapshot(std::map<StreamUsecases, ChiStream*> streamIdMap, PipelineType type);
    };
}

#endif  //#ifndef CHI_PIPELINE_UTILS
