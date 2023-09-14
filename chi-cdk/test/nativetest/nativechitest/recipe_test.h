////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  recipe_test.h
/// @brief Declarations for the Recipe test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __RECIPE_TEST__
#define __RECIPE_TEST__

#include "chitestcase.h"

namespace chitests
{

class RecipeTest : public ChiTestCase
{

    public:

        void TestRealtimePipeline(TestId testId);
        void TestOfflinePipeline(TestId testId);

        RecipeTest() = default;
        ~RecipeTest() = default;

    protected:

        CDKResult SetupStreams() override;
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override;
        CDKResult CreateSessions() override;
        void      DestroyResources() override;

    private:

        ChiBufferManager::NativeChiBuffer  m_offlineOutputBuffers[MAX_STREAMS];    // offline output buffers
        ChiBufferManager::NativeChiBuffer* m_pInputBuffer;                         // input buffer

        CDKResult                       CreateSessionHelper(RecipeTest::SessionId sessionId);
        void                            DestroyResourcesHelper(SessionId sessionId);
};

} // namespace chitests

#endif // __RECIPE_TEST__
