//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA_DEVICE_TEST__
#define __CAMERA_DEVICE_TEST__

#include "haltestcase.h"

namespace haltests
{
    class CameraDeviceTest : public HalTestCase
    {

    public:
        CameraDeviceTest();
        ~CameraDeviceTest();

        void TestCaptureTemplateByCamera(); // CTS case


    protected:
        virtual void Setup();
        virtual void Teardown();

    private:
        void CheckRequestForTemplate(
            int cameraId,
            const camera_metadata_t *templateRequest,
            int reqTemplate,
            camera_info info);
        void CheckFPSRange(
            int cameraId,
            const camera_metadata_t *templateRequest,
            int reqTemplate,
            camera_info info);
        void CheckAFMode(
            int cameraId,
            const camera_metadata_t *templateRequest,
            int reqTemplate,
            camera_info info);
        void CheckAntiBandingMode(
            int cameraId,
            const camera_metadata_t *templateRequest,
            int reqTemplate,
            camera_info info);

        const int MIN_FPS_REQUIRED_FOR_STREAMING = 20;


    };


}




#endif  //#ifndef __CAMERA_DEVICE_TEST__