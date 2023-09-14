//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA3_MODULE__
#define __CAMERA3_MODULE__

#include "camera3supermodule.h"

namespace haltests {

    class Camera3Module : public Camera3SuperModule,
                          public NativeTest
{
    public:
        Camera3Module();
        virtual ~Camera3Module();
    protected:
        virtual void Setup();
        virtual void Teardown();
};

}  // namespace haltests


#endif  //#ifndef __CAMERA3_MODULE__
