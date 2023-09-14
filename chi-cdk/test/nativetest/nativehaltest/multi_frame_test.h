//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __MULTI_FRAME_TEST__
#define __MULTI_FRAME_TEST__

#include "haltestcase.h"

namespace haltests
{
    class MultiFrameTest : public HalTestCase
    {

    public:
        MultiFrameTest();
        ~MultiFrameTest();

        void OpenCamera(int cameraId);
        int ConfigStream(
            int cameraId,
            int format,
            Size resolution,
            bool setResourceCheck = false
        );
        void CloseCamera(int cameraId);

        void TestSingleCameraCaptureAsync(
            uint32_t cameraId,
            std::vector<Cam3Device::streamRequestType> streamReqType);

        void TestOpenMultipleCameras(
            std::vector<int> camIDList, int format, Size resolution, int num_iterations);

        void TestOpenAllCameras(std::vector<int> camIDList, int format, Size resolution);

        void TestOpenRandomMultipleCameras(
            int format, Size resolution, int num_iterations);

        std::vector<int> FilterCamListOnIFE(std::vector<int> camList, int nIFEs, std::map<int, CameraMapConfiguration> map);

    protected:
        virtual void Setup();

    private:
        std::mutex camMutexLock;
    };

}

#endif  //#ifndef __MULTI_FRAME_TEST__