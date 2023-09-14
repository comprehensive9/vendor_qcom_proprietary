//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __NATIVEHALX_TEST__
#define __NATIVEHALX_TEST__

#include "haltestcase.h"
#include <regex>

namespace haltests
{
    class NativeHalX : public HalTestCase
    {

    public:
        NativeHalX();
        ~NativeHalX();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// NativeHalX::OmniTest()
        ///
        /// @brief  Test for configuring streams as per the command line inputs
        ///
        /// @return None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void OmniTest();

    protected:
        virtual void Setup();

    };
}

#endif  //#ifndef __NATIVEHALX_TEST__