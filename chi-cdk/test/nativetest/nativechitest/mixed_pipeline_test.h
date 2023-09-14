////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  mixed_pipeline_test.h
/// @brief Declarations for the offline test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MIXED_PIPELINE_TEST__
#define __MIXED_PIPELINE_TEST__

#include "chitestcase.h"

namespace chitests
{
    class MixedPipelineTest : public ChiTestCase
    {

    public:
        enum TestId
        {
            PreviewZSLJpegSnapshot = 0,
            PreviewZSLJpegSnapshotWithStats,
            PreviewZSLYuvSnapshot,
            PreviewZSLYuvSnapshotWithStats,
            PreviewNZSLJpegSnapshot,
            PreviewNZSLJpegSnapshotWithStats,
            PreviewNZSLYuvSnapshot,
            PreviewNZSLYuvSnapshotWithStats,
            TestPreviewCallbackSnapshotWithThumbnail,
            LiveVideoSnapShot,
            TestIPEABCStatsTestgenOfflineJPEG,
            TestIPEABCStatsTestgenOfflineYUV,
            TestIPEABCStatsTestgenOfflineRaw,
            TestIPEABCStatsOfflineJPEG,
            TestIPEABCStatsOfflineYUV,
            TestIPEABCStatsOfflineRaw,
        };

        MixedPipelineTest() = default;
        ~MixedPipelineTest() = default;

        void TestMixedPipelineTest(TestId testType, bool bUseExternalSensor = false);

    protected:
        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      CommonProcessCaptureResult(ChiCaptureResult* pCaptureResult, SessionPrivateData* pSessionPrivateData) override;
        void      DestroyResources() override;

    private:

        CDKResult GenerateRealtimeRequest(uint64_t frameNumber, bool includeRaw);
        CDKResult GenerateOfflineRequest(uint64_t frameNumber);

        // Do not allow the copy constructor or assignment operator
        MixedPipelineTest(const MixedPipelineTest&) = delete;
        MixedPipelineTest& operator = (const MixedPipelineTest&) = delete;

        Size                            m_selectedRdiOutputSize;                // user provided RDI output resolution at runtime
        TestId                          m_testId;                               // testId to differentiate between mixed pipeline tests
        ChiSession*                     m_pSession[MaxSessions];                // session instance for mixed usecase
        ChiPipeline*                    m_Pipelines[MaxPipelines];              // pipeline instance for mixed usecase
        SessionPrivateData              m_perSessionPvtData[MaxSessions];       // test specific private data
        ChiPrivateData                  m_requestPvtData[MaxSessions];          // request specific private data
        BufferManager::NativeChiBuffer  m_realtimeOutputBuffers[MAX_STREAMS];   // realtime output buffers
        BufferManager::NativeChiBuffer  m_offlineOutputBuffers[MAX_STREAMS];    // offline output buffers
        BufferManager::NativeChiBuffer* m_pInputBuffer;                         // input buffer
        CHICAPTUREREQUEST               m_realtimeRequest[MAX_REQUESTS];        // realtime capture requests
        CHICAPTUREREQUEST               m_snapshotRequest[MAX_REQUESTS];        // snapshot capture requests
        CHIPIPELINEREQUEST              m_submitRequest;                        // submit request
        CHIPIPELINEMETADATAINFO         m_pPipelineMetadataInfo[MaxPipelines];  // pipeline metadata information
        uint32_t                        m_lastRealtimeFrameReceived;            // tracks the last realtime frame received (for metadata passing purposes)

    };
}

#endif //  __MIXED_PIPELINE_TEST__

