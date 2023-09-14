//******************************************************************************
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CUSTOM_NODE_TEST__
#define __CUSTOM_NODE_TEST__

#include "chitestcase.h"

namespace chitests
{
    class CustomNodeTest : public ChiTestCase
    {

    public:

        CustomNodeTest() = default;
        ~CustomNodeTest() = default;

        void TestCustomNode(TestId testType);

    protected:

        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      DestroyResources() override;

    };
}

#endif  //#ifndef __CUSTOM_NODE_TEST__
