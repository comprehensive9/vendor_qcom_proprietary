//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA_MANAGER_TEST__
#define __CAMERA_MANAGER_TEST__

#include "haltestcase.h"

namespace haltests
{
    class CameraManagerTest : public HalTestCase
    {

    public:
        CameraManagerTest();
        ~CameraManagerTest();

        void TestOpenCamerasSerially();
        void TestOpenAllCamerasFrontFirst();
        void TestOpenAllCamerasBackFirst();
        void TestOpenInvalidCamera();


    protected:
        virtual void Setup();
        virtual void Teardown();


    private:
        static const int NUM_CAMERA_REOPENS = 10;


    };


}




#endif  //#ifndef __CAMERA_MANAGER_TEST__