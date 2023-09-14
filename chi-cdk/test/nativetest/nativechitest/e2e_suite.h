////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  e2e_suite.h
/// @brief Declarations for the end-to-end test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __E2E_SUITE__
#define __E2E_SUITE__

#include "chitestcase.h"

namespace chitests
{

    class E2ESuite : public ChiTestCase
    {

    public:

        E2ESuite() = default;
        ~E2ESuite() = default;

        void TestE2EPipeline(TestId testType);

    protected:
        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      DestroyResources() override;

    private:

        ChiBufferManager::NativeChiBuffer  m_offlineOutputBuffers[MAX_STREAMS];    // offline output buffers
        ChiBufferManager::NativeChiBuffer* m_pInputBuffer;                         // input buffer
        ChiSensorModeInfo*              m_testSensorMode;                       // sensor mode for the given camera under test
        // Do not allow the copy constructor or assignment operator
        E2ESuite(const E2ESuite&) = delete;
        E2ESuite& operator = (const E2ESuite&) = delete;
    };
}

#endif //  __E2E_SUITE__
