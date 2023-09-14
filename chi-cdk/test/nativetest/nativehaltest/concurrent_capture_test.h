//******************************************************************************
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CONCURRENT_CAPTURE_TEST__
#define __CONCURRENT_CAPTURE_TEST__

#include "camera3stream.h"

namespace haltests
{
    class ConcurrentCaptureTest : public Camera3Stream
    {

    public:
        ConcurrentCaptureTest();
        ~ConcurrentCaptureTest();

        void TestConcurrentCapture(
            int numExpectCameras,
            int format,
            Size resolution,
            int numFrames,
            const char* testName);

    protected:
        virtual void Setup();
        virtual void Teardown();

    private:
        // Test normal still capture sequence.
        void TestCapture(
            int numCameras,
            int cameraId,
            int format,
            const char* filename,
            int captureCount);

    };

}

#endif  //#ifndef __CONCURRENT_CAPTURE_TEST__