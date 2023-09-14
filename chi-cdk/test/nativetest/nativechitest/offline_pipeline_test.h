////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  offline_pipeline_test.h
/// @brief Declarations for the offline test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __OFFLINE_PIPELINETEST__
#define __OFFLINE_PIPELINETEST__

#include "chitestcase.h"

namespace chitests
{

    class OfflinePipelineTest : public ChiTestCase
    {
    public:

        OfflinePipelineTest()  = default;
        ~OfflinePipelineTest() = default;

        void TestOfflinePipeline(TestId testType);

    protected:

        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      DestroyResources() override;
    };
}
#endif  //#ifndef __OFFLINE_PIPELINETEST__
