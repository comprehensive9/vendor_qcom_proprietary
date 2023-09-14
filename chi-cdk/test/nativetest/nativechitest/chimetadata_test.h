//******************************************************************************************************************************
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************
#ifndef __CHIMETADATA_TEST__
#define __CHIMETADATA_TEST__

#include "chitestcase.h"
#include "chimodule.h"
#include "chimetadatautil.h"

namespace chitests
{
    class ChiMetadataTest : public ChiTestCase
    {

    public:
        virtual VOID  Setup();
        virtual VOID  Teardown();

        ChiMetadataTest() = default;
        ~ChiMetadataTest() = default;

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

        CHIMETAHANDLE GetDefaultMetahandle();
        CHIMETAHANDLE GetMetaHandle();

        void TestCreateMetadata();
        void TestDestroyMetadata();
        void TestCreateWithTagArray();
        void TestCreateWithAndroidMetadata();
        void TestGetTag();
        void TestSetTag();
        void TestDeleteTag();
        void TestMerge();
        void TestCopy();
        void TestMergeDisjoint();
        void TestCopyDisjoint();
        void TestCapacity();
        void TestGetTagCount();
        void TestDump();
        void TestGetMetadataOps();
        void TestCreateDefaultMetadata();
        void TestIterator();
        void TestMetadataReference();
        void TestInvalidate();
        void TestCopyPerformance();
        void TestMergePerformance();
        void TestClone();
        void TestGetMetadataTable();
        void TestVendorTagOps();
        void TestPrint();
        void TestGetMetadataEntry();

    private:
        ChiMetadataUtil* m_pMetadataUtil;
        ChiModule*       m_pChiModule;
        CHIMETAHANDLE m_pMetaHandle;
        CHIMETAHANDLE m_pDefaultMetaHandle;
    };

}

#endif  //#ifndef __CHIMETADATA_TEST__
