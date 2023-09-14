//******************************************************************************
// Copyright (c) 2016 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __BINARY_COMP_TEST__
#define __BINARY_COMP_TEST__

#include "chitestcase.h"
#include "chimodule.h"

namespace chitests
{
    class BinaryCompatibilityTest : public ChiTestCase
    {
        //Together used to compare every 8 bytes of memory
        static const uint32_t TestValue3210 = 0x03020100; // UCHAR equivalent of 0,1,2,3
        static const uint32_t TestValue7654 = 0x07060504; // UCHAR equivalent of 4,5,6,7

    public:
        virtual VOID Setup();
        virtual VOID Teardown();

        BinaryCompatibilityTest() = default;
        ~BinaryCompatibilityTest() = default;

        void FillByteValue(UCHAR* , UINT32);
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

        // Main Test functions
        void TestCHIVENDORTAGSOPS();
        void TestCHICAMERAINFO();
        void TestCHISENSORMODEINFO();
        void TestCHIPIPELINECREATEDESCRIPTOR();
        void TestCHIPORTBUFFERDESCRIPTOR();
        void TestCHIBUFFEROPTIONS();
        void TestCHIPIPELINEINFO();
        void TestCHICALLBACKS();
        void TestCHIPIPELINEREQUEST();

    private:
        /* Returns a 64bit constant by using two 32bit constants */
        static inline int64_t Get64bitConstant(int32_t msb, int32_t lsb)
        {
            return (int64_t)msb << 32 | lsb;
        };
    };


}

#endif  //#ifndef __BINARY_COMP_TEST__
