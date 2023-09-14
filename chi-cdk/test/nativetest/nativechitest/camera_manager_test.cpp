//******************************************************************************
// Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera_manager_test.h"

namespace chitests
{
    /**************************************************************************************************
    *   CameramanagerTest::Setup
    *
    *   @brief
    *       Entry point to the test
    **************************************************************************************************/
    void CameraManagerTest::Setup()
    {
        m_pChiModule = ChiModule::GetInstance();
    }

    /**************************************************************************************************
    *   CameramanagerTest::Teardown
    *
    *   @brief
    *       Teardown point to the test
    **************************************************************************************************/
    void CameraManagerTest::Teardown()
    {
        m_pChiModule->DestroyInstance();
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestOpenInvalidCamera
    *
    *   @brief
    *       Negative test to expect opening camera with invalid ID
    **************************************************************************************************/
    void CameraManagerTest::TestOpenInvalidCamera()
    {
        NT_ASSERT(nullptr != m_pChiModule->GetContext(), "Camera open() is unimplemented");
        int invalidId = -1;
        const CHIHANDLE hContext = m_pChiModule->GetContext();
        CHICAMERAINFO info;
        NT_ASSERT(0 != m_pChiModule->GetChiOps()->pGetCameraInfo(hContext, invalidId, &info),
            "Shouldn't get camera info for camera id -1");
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestOpenCamerasSerially
    *
    *   @brief
    *       Tests that each camera device can be opened one at a time, several times.
    **************************************************************************************************/
    void CameraManagerTest::TestOpenCloseContextSerially()
    {
        PFNCHIENTRY pChiHalOpen = m_pChiModule->GetChiEntryFunctionPointer();
        m_pChiModule->DestroyInstance();

        CHICONTEXTOPS chiOps;

        (*pChiHalOpen)(&chiOps);

        for (int j = 0; j < NUM_CAMERA_REOPENS; j++)
        {
            NT_LOG_INFO("Opening Chi Context");
            CHIHANDLE hContext = chiOps.pOpenContext();
            NT_ASSERT(hContext != nullptr, "Chi Context failed to open");
            chiOps.pCloseContext(hContext);
        }
    }
}