//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera3module.h"

namespace haltests {

    Camera3Module::Camera3Module()
    {}

    /**************************************************************************************************
    *   Camera3Module::~Camera3Module
    *
    *   @brief
    *       Destructor for Camera3Module
    **************************************************************************************************/
    Camera3Module::~Camera3Module()
    {}

    /***************************************************************
    * Gtest setup() invoked before starting the test
    ***************************************************************/
    void Camera3Module::Setup()
    {
        Camera3SuperModule::Setup();
    }

    void Camera3Module::Teardown()
    {
        Camera3SuperModule::Teardown();
    }

} //namespace tests

