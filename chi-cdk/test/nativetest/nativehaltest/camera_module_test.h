//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA_MODULE_TEST__
#define __CAMERA_MODULE_TEST__

#include "haltestcase.h"

namespace haltests
{
    class CameraModuleTest : public HalTestCase
    {

    public:
        CameraModuleTest();
        ~CameraModuleTest();

        void TestNumberOfCamera();
        void TestStaticCameraInfoAndMetadata();
        void TestGetCameraInfo();

    protected:
        virtual void Setup();
        virtual void Teardown();
    };


}




#endif  //#ifndef __CAMERA_MODULE_TEST__