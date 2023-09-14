//******************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __PREVIEW_TEST__
#define __PREVIEW_TEST__

#include "haltestcase.h"

namespace haltests
{
    class PreviewTest : public HalTestCase
    {

    public:
        PreviewTest();
        ~PreviewTest();

        void TestPreviewWithDualCapture(
            int            snapFormat,
            Size           previewRes,
            Size           snapshotRes,
            const char*    testName,
            int            nSnaps,
            bool           dumpAll,
            SpecialSetting setting = NoSetting);
        void TestBasicPreview();
        void TestPreviewWithMaxSnapshot();

    protected:
        virtual void Setup();

    };


}

#endif  //#ifndef __DUAL_PREVIEW_TEST__