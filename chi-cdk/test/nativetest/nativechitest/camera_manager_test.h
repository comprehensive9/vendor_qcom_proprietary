//******************************************************************************
// Copyright (c) 2016 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA_MANAGER_TEST__
#define __CAMERA_MANAGER_TEST__

#include "chitestcase.h"
#include "chimodule.h"

namespace chitests
{
    class CameraManagerTest : public ChiTestCase
    {

    public:
        virtual VOID  Setup();
        virtual VOID  Teardown();

        CameraManagerTest()=default;
        ~CameraManagerTest()=default;

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
        void TestOpenCloseContextSerially();
        void TestOpenInvalidCamera();

    private:
        ChiModule*       m_pChiModule;
        static const int NUM_CAMERA_REOPENS = 10;
    };

}

#endif  //#ifndef __CAMERA_MANAGER_TEST__