////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  recipe_test.h
/// @brief Declarations for the Recipe test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NATIVECHIX_TEST__
#define __NATIVECHIX_TEST__

#include "chitestcase.h"

namespace chitests
{

class NativeCHIX : public ChiTestCase
{

    public:
        ChiUsecase mUsecase; // the ChiUsecase for this test filled per user requirements

        void TestDIYPipeline();

        // TODO: delete these debug functions after development
        void CreateSampleE2EUsecase();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// PrintUsecase
        /// @brief Helper function to print entire usecase for debugging purposes
        /// @param  None
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void PrintUsecase();

        NativeCHIX() = default;
        ~NativeCHIX() = default;

    protected:

        // TODO move it to chitestcase after final implementation
        const std::map<std::string, UINT> NODES = {
            {"Sensor", CamX::Sensor},
            {"StatsProcessing", CamX::StatsProcessing},
            {"SinkBuffer", CamX::SinkBuffer},
            {"SinkNoBuffer", CamX::SinkNoBuffer},
            {"SourceBuffer", CamX::SourceBuffer},
            {"AutoFocus", CamX::AutoFocus},
            {"JPEGAggregator", CamX::JPEGAggregator},
            {"FDSw", CamX::FDSw},
            {"FDManager", CamX::FDManager},
            {"StatsParse", CamX::StatsParse},
            {"OfflineStats", CamX::OfflineStats},
            {"Torch", CamX::Torch},
            {"AutoWhiteBalance", CamX::AutoWhiteBalance},
            {"HistogramProcess", CamX::HistogramProcess},
            {"Tracker", CamX::Tracker},
            {"IFE", CamX::IFE},
            {"JPEG", CamX::JPEG},
            {"IPE", CamX::IPE},
            {"BPS", CamX::BPS},
            {"FDHw", CamX::FDHw},
            {"LRME", CamX::LRME},
            {"RANSAC", CamX::RANSAC},
            {"CVP", CamX::CVP},
        };

        UINT64 mFrameNumber = 1;

        void      Setup() override;
        void      Teardown() override;
        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      DestroyResources() override;

    private:

        static std::vector<ChiNode> msDiyNodes;
        static std::vector<ChiNodeLink> msDiyPipelineLinks;
        static std::vector<ChiPipelineTargetCreateDescriptor> msDiyPipelines;

        CHICAPTUREREQUEST               mCaptureRequests[MAX_REQUESTS];        // array of realtime capture requests

        ChiBufferManager::NativeChiBuffer  mRealtimeOutputBuffers[MAX_STREAMS];   // realtime output buffers
        ChiBufferManager::NativeChiBuffer  mOfflineOutputBuffers[MAX_STREAMS];    // offline output buffers
        ChiBufferManager::NativeChiBuffer* mpInputBuffer;                         // input buffer

        CDKResult                       GenerateCaptureRequest(uint64_t frameNumber);
        CDKResult                       CreateSessionHelper();

        static NativeCHIX*       m_pTestObj;
};

} // namespace chitests

#endif // __RECIPE_TEST__
