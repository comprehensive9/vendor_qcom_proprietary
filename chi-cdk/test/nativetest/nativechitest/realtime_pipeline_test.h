////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  realtime_pipeline_test.h
/// @brief Declarations for the realtime test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __REALTIME_PIPELINETEST__
#define __REALTIME_PIPELINETEST__

#include "chitestcase.h"

namespace chitests
{

    class RealtimePipelineTest : public ChiTestCase
    {

    public:

        RealtimePipelineTest() = default;
        ~RealtimePipelineTest() = default;

        void TestRealtimePipeline(TestId testType, bool bUseExternalSensor = false);

    protected:

        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      DestroyResources() override;

    };
} // namespace chitests

#endif  //#ifndef __REALTIMEPIPELINETEST__
