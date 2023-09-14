//******************************************************************************
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA_MODULE_TEST__
#define __CAMERA_MODULE_TEST__

#include "chitestcase.h"
#include "chimodule.h"

namespace chitests
{
    class CameraModuleTest : public ChiTestCase
    {

    public:

        virtual VOID  Setup();
        virtual VOID  Teardown();

        CameraModuleTest() = default;
        ~CameraModuleTest() = default;

        // Unused functions
        virtual CDKResult SetupStreams() { return CDKResultSuccess; }
        virtual CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode)
        {
            NT_UNUSED_PARAM(cameraId);
            NT_UNUSED_PARAM(sensorMode);
            return CDKResultSuccess;
        }
        virtual CDKResult CreateSessions() { return CDKResultSuccess; }
        virtual void      DestroyResources() {};

        // Main test functions
        void TestNumberOfCamera();
        void TestStaticCameraInfoAndMetadata();
        void TestGetCameraInfo();

    private:
        ChiModule* m_pChiModule;
    };


}
#endif  //#ifndef __CAMERA_MODULE_TEST__