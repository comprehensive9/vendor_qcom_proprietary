//******************************************************************************
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************


#include "binary_comp_test.h"

namespace chitests
{
    /**************************************************************************************************
    *   CameramanagerTest::Setup
    *
    *   @brief
    *       Entry point to the test
    **************************************************************************************************/
    VOID BinaryCompatibilityTest::Setup()
    {
    }

    /**************************************************************************************************
    *   CameramanagerTest::Teardown
    *
    *   @brief
    *       Teardown point to the test
    **************************************************************************************************/
    VOID BinaryCompatibilityTest::Teardown()
    {
    }

    /**************************************************************************************************
    *   FillByteValue(UCHAR* pIter, UINT32 structSize)
    *
    *   @brief
    *      Loop to fill each UCHAR byte with placeholder/dummy value
    **************************************************************************************************/
    void BinaryCompatibilityTest::FillByteValue(UCHAR* pIter, UINT32 structSize)
    {
        UINT32 byteData = 0;      //Byte data toggles from 00,01,02,03,04,05,06,07 back to 00,01,02,03 ...
        for (UINT32 i = 0; i < structSize; i++)
        {
           *pIter = byteData % 8;
           byteData++;
           pIter += sizeof(UCHAR);
        }
    }

#ifdef ENVIRONMENT64
    /**************************************************************************************************
    *   CameramanagerTest::TestCHIVENDORTAGSOPS
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIVENDORTAGSOPS()
    {
        UCHAR* pMem = NULL; // Pointer to start of the memory
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHITAGSOPS)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIVENDORTAGOPS: %u", sizeof(CHITAGSOPS));
        static const UINT32 structSize = sizeof(CHITAGSOPS);

        FillByteValue(pMem, structSize);

        CHITAGSOPS s = { 0 };
        s.pQueryVendorTagLocation =
            reinterpret_cast<PFNCHIQUERYVENDORTAGLOCATION>(Get64bitConstant(TestValue7654, TestValue3210));
        s.pSetMetaData = reinterpret_cast<PSETMETADATA>(Get64bitConstant(TestValue7654, TestValue3210));
        s.pGetMetaData = reinterpret_cast<PGETMETADATA>(Get64bitConstant(TestValue7654, TestValue3210));

        s.reserved[0] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[1] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[2] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[3] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[4] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[5] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[6] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.reserved[7] = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210));

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHICAMERAINFO
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHICAMERAINFO()
    {
        UCHAR* pMem = NULL;
        int float_converter;    // Used to correctly write int into a float variable
        INT64 struct_padding;   // Used to fill values to handle structure padding
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHICAMERAINFO)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHICAMERAINFO: %u", sizeof(CHICAMERAINFO));
        static const UINT32 structSize = sizeof(CHICAMERAINFO);

        FillByteValue(pMem, structSize);

        CHICAMERAINFO s = { 0 };

        s.size = TestValue3210; //4 bytes
        s.numSensorModes = TestValue7654; //4 bytes

        s.pLegacy = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210)); //8 bytes

        s.sensorCaps.size = TestValue3210; //4 bytes
        s.sensorCaps.sensorId = TestValue7654; //4 bytes

        s.sensorCaps.facing = static_cast<CHIDIRECTION>(TestValue3210); //4 bytes
        s.sensorCaps.positionType = static_cast<CHISENSORPOSITIONTYPE>(TestValue7654); //4 bytes

        float_converter = TestValue3210;
        s.sensorCaps.pixelSize = *reinterpret_cast<float*>(&float_converter); //4 bytes
        s.sensorCaps.activeArray.left = TestValue7654; //4 bytes

        s.sensorCaps.activeArray.top = TestValue3210; //4 bytes
        s.sensorCaps.activeArray.width = TestValue7654; //4 bytes

        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.sensorCaps.activeArray.height, &struct_padding, sizeof(INT64)); //8 bytes

        s.sensorCaps.pRawOTPData = reinterpret_cast<VOID*>(Get64bitConstant(TestValue7654, TestValue3210)); //8 bytes

        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.sensorCaps.rawOTPDataSize, &struct_padding, sizeof(INT64)); // 8 bytes

        s.sensorCaps.pSensorName = reinterpret_cast<CHAR*>(Get64bitConstant(TestValue7654, TestValue3210)); //8 bytes

        s.lensCaps.size = TestValue3210; //4 bytes
        float_converter = TestValue7654; //4 bytes
        s.lensCaps.focalLength = *reinterpret_cast<float*>(&float_converter);

        s.lensCaps.isFixedFocus = TestValue3210; //4 bytes
        float_converter = TestValue7654; //4 bytes
        s.lensCaps.horViewAngle = *reinterpret_cast<float*>(&float_converter);

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPIPELINECREATEDESCRIPTOR
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPIPELINECREATEDESCRIPTOR()
    {
        UCHAR* pMem = NULL;
        INT64 struct_padding;   // Used to fill values to handle structure padding
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPIPELINECREATEDESCRIPTOR)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPIPELINECREATEDESCRIPTOR: %u",
               sizeof(CHIPIPELINECREATEDESCRIPTOR));
        static const UINT32 structSize = sizeof(CHIPIPELINECREATEDESCRIPTOR);

        FillByteValue(pMem, structSize);

        CHIPIPELINECREATEDESCRIPTOR s = { 0 };
        s.size = TestValue3210;
        s.numNodes = TestValue7654;
        s.pNodes = reinterpret_cast<CHINODE*>(Get64bitConstant(TestValue7654, TestValue3210));
        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.numLinks, &struct_padding, sizeof(INT64));
        s.pLinks = reinterpret_cast<CHINODELINK*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.isRealTime = TestValue3210;
        s.numBatchedFrames = TestValue7654;
        s.maxFPSValue = TestValue3210;
        s.cameraId = TestValue7654;
        s.hPipelineMetadata = reinterpret_cast<CHIMETAHANDLE>(Get64bitConstant(TestValue7654, TestValue3210));
        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.HALOutputBufferCombined, &struct_padding, sizeof(INT64));
        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");
        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPORTBUFFERDESCRIPTOR
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPORTBUFFERDESCRIPTOR()
    {
        UCHAR* pMem = NULL;
        INT64 struct_padding;   // Used to fill values to handle structure padding
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPORTBUFFERDESCRIPTOR)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPORTBUFFERDESCRIPTOR: %u", sizeof(CHIPORTBUFFERDESCRIPTOR));
        static const UINT32 structSize = sizeof(CHIPORTBUFFERDESCRIPTOR);

        FillByteValue(pMem, structSize);

        CHIPORTBUFFERDESCRIPTOR s = { 0 };
        s.size = TestValue3210; // 4 bytes
        s.numNodePorts = TestValue7654; // 4 bytes
        s.pNodePort = reinterpret_cast<ChiLinkNodeDescriptor*>(Get64bitConstant(TestValue7654, TestValue3210)); // 8 bytes
        s.pStream = reinterpret_cast<CHISTREAM*>(Get64bitConstant(TestValue7654, TestValue3210)); // 8 bytes
        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.flagValue, &struct_padding, sizeof(INT64)); // 8bytes
        s.pBufferNegotiationsOptions = reinterpret_cast<CHIBUFFEROPTIONS*>(Get64bitConstant(TestValue7654, TestValue3210)); // 8 bytes
        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPIPELINEINFO
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPIPELINEINFO()
    {
        UCHAR* pMem = NULL;
        INT64 struct_padding;   // Used to fill values to handle structure padding
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPIPELINEINFO)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPIPELINEINFO: %u", sizeof(CHIPIPELINEINFO));
        static const UINT32 structSize = sizeof(CHIPIPELINEINFO);

        FillByteValue(pMem, structSize);

        CHIPIPELINEINFO s = { 0 };
        s.hPipelineDescriptor =
            reinterpret_cast<CHIPIPELINEDESCRIPTOR>(Get64bitConstant(TestValue7654, TestValue3210));
        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.pipelineInputInfo.isInputSensor, &struct_padding, sizeof(INT64));
        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.pipelineInputInfo.sensorInfo.cameraId, &struct_padding, sizeof(INT64));
        s.pipelineInputInfo.sensorInfo.pSensorModeInfo =
            reinterpret_cast<const CHISENSORMODEINFO*>(Get64bitConstant(TestValue7654, TestValue3210));
        s.pipelineOutputInfo.hPipelineHandle =
            reinterpret_cast<CHIPIPELINEHANDLE>(Get64bitConstant(TestValue7654, TestValue3210));
        s.pipelineResourcePolicy = TestValue3210;
        s.isDeferFinalizeNeeded = TestValue7654;

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHICALLBACKS
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHICALLBACKS()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHICALLBACKS)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHICALLBACKS: %u", sizeof(CHICALLBACKS));
        static const UINT32 structSize = sizeof(CHICALLBACKS);

        FillByteValue(pMem, structSize);

        CHICALLBACKS s = { 0 };
        s.ChiProcessCaptureResult =
            reinterpret_cast<void(*)(
                CHICAPTURERESULT*   pCaptureResult,
                VOID*               pPrivateCallbackDataconst)>(Get64bitConstant(TestValue7654, TestValue3210));
        s.ChiNotify =
            reinterpret_cast<void(*)(
                const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
                VOID*                       pPrivateCallbackDataconst)>(Get64bitConstant(TestValue7654, TestValue3210));

        s.ChiProcessPartialCaptureResult =
            reinterpret_cast<void(*)(
                CHIPARTIALCAPTURERESULT*   pCaptureResult,
                VOID*                      pPrivateCallbackDataconst)>(Get64bitConstant(TestValue7654, TestValue3210));

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPIPELINEREQUEST
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPIPELINEREQUEST()
    {
        UCHAR* pMem = NULL;
        INT64 struct_padding;   // Used to fill values to handle structure padding
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPIPELINEREQUEST)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPIPELINEREQUEST: %u", sizeof(CHIPIPELINEREQUEST));
        static const UINT32 structSize = sizeof(CHIPIPELINEREQUEST);

        FillByteValue(pMem, structSize);

        CHIPIPELINEREQUEST s = { 0 };
        s.pSessionHandle = reinterpret_cast<CHIHANDLE>(Get64bitConstant(TestValue7654, TestValue3210));
        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.numRequests, &struct_padding, sizeof(INT64));
        s.pCaptureRequests = reinterpret_cast<const CHICAPTUREREQUEST*>(Get64bitConstant(TestValue7654, TestValue3210));

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

#else // ENVIRONMENT32

#if defined (_LINUX)  //these cases require 32-bit environment on device
    /**************************************************************************************************
    *   CameramanagerTest::TestCHIVENDORTAGSOPS
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIVENDORTAGSOPS()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHITAGSOPS)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIVENDORTAGOPS: %u", sizeof(CHITAGSOPS));
        static const UINT32 structSize = sizeof(CHITAGSOPS);

        FillByteValue(pMem, structSize);

        CHITAGSOPS s = { 0 };
        s.pQueryVendorTagLocation = reinterpret_cast<PFNCHIQUERYVENDORTAGLOCATION>(TestValue3210);
        s.pSetMetaData = reinterpret_cast<PSETMETADATA>(TestValue3210);
        s.pGetMetaData = reinterpret_cast<PGETMETADATA>(TestValue3210);

        s.reserved[0] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[1] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[2] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[3] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[4] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[5] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[6] = reinterpret_cast<VOID*>(TestValue3210);
        s.reserved[7] = reinterpret_cast<VOID*>(TestValue3210);

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHICAMERAINFO
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHICAMERAINFO()
    {
        UCHAR* pMem = NULL;
        int float_converter;    // Used to correctly write int into a float variable
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHICAMERAINFO)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHICAMERAINFO: %u", sizeof(CHICAMERAINFO));
        static const UINT32 structSize = sizeof(CHICAMERAINFO);

        FillByteValue(pMem, structSize);

        CHICAMERAINFO s = { 0 };
        s.size = TestValue3210;
        s.numSensorModes = TestValue3210;
        s.pLegacy = reinterpret_cast<VOID*>(TestValue3210);
        s.sensorCaps.size = TestValue3210;
        s.sensorCaps.sensorId = TestValue3210;
        s.sensorCaps.facing = static_cast<CHIDIRECTION>(TestValue3210);
        s.sensorCaps.positionType = static_cast<CHISENSORPOSITIONTYPE>(TestValue3210);
        float_converter = TestValue3210;
        s.sensorCaps.pixelSize = *reinterpret_cast<float*>(&float_converter);
        s.sensorCaps.activeArray.left = TestValue3210;
        s.sensorCaps.activeArray.top = TestValue3210;
        s.sensorCaps.activeArray.width = TestValue3210;
        s.sensorCaps.activeArray.height = TestValue3210;
        s.sensorCaps.pRawOTPData = reinterpret_cast<VOID*>(TestValue3210);
        s.sensorCaps.rawOTPDataSize = TestValue3210;
        s.lensCaps.size = TestValue3210;
        float_converter = TestValue3210;
        s.lensCaps.focalLength = *reinterpret_cast<float*>(&float_converter);
        s.lensCaps.isFixedFocus = TestValue3210;
        float_converter = TestValue3210;
        s.lensCaps.horViewAngle = *reinterpret_cast<float*>(&float_converter);

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPIPELINECREATEDESCRIPTOR
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPIPELINECREATEDESCRIPTOR()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPIPELINECREATEDESCRIPTOR)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPIPELINECREATEDESCRIPTOR: %u",
            sizeof(CHIPIPELINECREATEDESCRIPTOR));
        static const UINT32 structSize = sizeof(CHIPIPELINECREATEDESCRIPTOR);

        FillByteValue(pMem, structSize);

        CHIPIPELINECREATEDESCRIPTOR s = { 0 };
        s.size = TestValue3210;
        s.numNodes = TestValue3210;
        s.pNodes = reinterpret_cast<CHINODE*>(TestValue3210);
        s.numLinks = TestValue3210;
        s.pLinks = reinterpret_cast<CHINODELINK*>(TestValue3210);
        s.isRealTime = TestValue3210;
        s.numBatchedFrames = TestValue3210;
        s.maxFPSValue = TestValue3210;
        s.cameraId = TestValue3210;
        s.hPipelineMetadata = reinterpret_cast<CHIMETAHANDLE>(TestValue3210);
        s.HALOutputBufferCombined = TestValue3210;
        s.logicalCameraId = TestValue3210;
        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPORTBUFFERDESCRIPTOR
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPORTBUFFERDESCRIPTOR()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPORTBUFFERDESCRIPTOR)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPORTBUFFERDESCRIPTOR: %u", sizeof(CHIPORTBUFFERDESCRIPTOR));
        static const UINT32 structSize = sizeof(CHIPORTBUFFERDESCRIPTOR);

        FillByteValue(pMem, structSize);

        CHIPORTBUFFERDESCRIPTOR s = { 0 };

        s.size = TestValue3210;
        s.numNodePorts = TestValue3210;
        s.pNodePort = reinterpret_cast<ChiLinkNodeDescriptor*>(TestValue3210);
        s.pStream = reinterpret_cast<CHISTREAM*>(TestValue3210);
        s.flagValue = TestValue3210;
        s.pBufferNegotiationsOptions = reinterpret_cast<CHIBUFFEROPTIONS*>(TestValue3210); // 8 bytes
        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPIPELINEINFO
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPIPELINEINFO()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPIPELINEINFO)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPIPELINEINFO: %u", sizeof(CHIPIPELINEINFO));
        static const UINT32 structSize = sizeof(CHIPIPELINEINFO);

        FillByteValue(pMem, structSize);

        CHIPIPELINEINFO s = { 0 };
        s.hPipelineDescriptor = reinterpret_cast<CHIPIPELINEDESCRIPTOR>(TestValue3210);
        s.pipelineInputInfo.isInputSensor = TestValue3210;
        s.pipelineInputInfo.sensorInfo.cameraId = TestValue3210;
        s.pipelineInputInfo.sensorInfo.pSensorModeInfo =
            reinterpret_cast<const CHISENSORMODEINFO*>(TestValue3210);
        s.pipelineOutputInfo.hPipelineHandle = reinterpret_cast<CHIPIPELINEHANDLE>(TestValue3210);
        s.pipelineResourcePolicy = TestValue3210;
        s.isDeferFinalizeNeeded = TestValue3210;

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHICALLBACKS
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHICALLBACKS()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHICALLBACKS)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHICALLBACKS: %u", sizeof(CHICALLBACKS));
        static const UINT32 structSize = sizeof(CHICALLBACKS);

        FillByteValue(pMem, structSize);

        CHICALLBACKS s = { 0 };
        s.ChiProcessCaptureResult =
            reinterpret_cast<void(*)(
                CHICAPTURERESULT*   pCaptureResult,
                VOID*               pPrivateCallbackDataconst)>(TestValue3210);
        s.ChiNotify =
            reinterpret_cast<void(*)(
                const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
                VOID*                       pPrivateCallbackDataconst)>(TestValue3210);

        s.ChiProcessPartialCaptureResult =
            reinterpret_cast<void(*)(
                CHIPARTIALCAPTURERESULT*   pCaptureResult,
                VOID*                      pPrivateCallbackDataconst)>(TestValue3210);

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIPIPELINEREQUEST
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIPIPELINEREQUEST()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIPIPELINEREQUEST)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIPIPELINEREQUEST: %u", sizeof(CHIPIPELINEREQUEST));
        static const UINT32 structSize = sizeof(CHIPIPELINEREQUEST);

        FillByteValue(pMem, structSize);

        CHIPIPELINEREQUEST s = { 0 };
        s.pSessionHandle = reinterpret_cast<CHIHANDLE>(TestValue3210);
        s.numRequests = TestValue3210;
        s.pCaptureRequests = reinterpret_cast<const CHICAPTUREREQUEST*>(TestValue3210);

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }
#endif
#endif

    /**************************************************************************************************
    *   CameramanagerTest::TestCHISENSORMODEINFO
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHISENSORMODEINFO()
    {
        UCHAR* pMem = NULL;
        UINT64 struct_padding;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHISENSORMODEINFO)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHISENSORMODEINFO: %u", sizeof(CHISENSORMODEINFO));
        static const UINT32 structSize = sizeof(CHISENSORMODEINFO);

        FillByteValue(pMem, structSize);

        CHISENSORMODEINFO s = { 0 };
        s.size = TestValue3210; //4 bytes
        s.modeIndex = TestValue7654;  //4 bytes

        s.arraySizeInMPix = TestValue3210; //4 bytes
        s.frameDimension.left = TestValue7654; //4 bytes

        s.frameDimension.top = TestValue3210; //4 bytes
        s.frameDimension.width = TestValue7654; //4 bytes

        s.frameDimension.height = TestValue3210; //4 bytes
        s.cropInfo.left = TestValue7654; //4 bytes

        s.cropInfo.top = TestValue3210; //4 bytes
        s.cropInfo.width = TestValue7654; //4 bytes

        s.cropInfo.height = TestValue3210; //4 bytes
        s.aspectRatio.numerator = TestValue7654; //4 bytes

        s.aspectRatio.denominator = TestValue3210; //4 bytes
        s.bpp = TestValue7654; //4 bytes

        s.frameRate = TestValue3210; //4 bytes
        s.batchedFrames = TestValue7654; //4 bytes

        s.sensorModeCaps.value = TestValue3210; //4 bytes
        s.streamtype = static_cast<CHISENSORSTREAMTYPE>(TestValue7654); //4 bytes

        s.remosaictype = static_cast<CHIREMOSAICTYPE>(TestValue3210); //4 bytes
        s.horizontalBinning = TestValue7654; //4 bytes

        s.verticalBinning = TestValue3210; //4 bytes
        s.HALOutputBufferCombined = TestValue7654; //4 bytes

        s.CSIPHYId = TestValue3210; //4 bytes
        s.is3Phase = TestValue7654; //4 bytes

        struct_padding = Get64bitConstant(TestValue7654, TestValue3210);
        memcpy(&s.laneCount, &struct_padding, sizeof(INT64)); // 8 bytes

        s.outPixelClock = Get64bitConstant(TestValue7654, TestValue3210); // UINT64 8 bytes

        s.streamConfigCount = Get64bitConstant(TestValue7654, TestValue3210); // UINT64 8 bytes

        s.laneCfg = TestValue3210; //4 bytes
        s.streamConfig[0].vc = TestValue7654; //4 bytes

        s.streamConfig[0].dt = TestValue3210; //4 bytes
        s.streamConfig[0].frameDimension.left = TestValue7654; //4 bytes

        s.streamConfig[0].frameDimension.top = TestValue3210; //4 bytes
        s.streamConfig[0].frameDimension.width = TestValue7654; //4 bytes

        s.streamConfig[0].frameDimension.height = TestValue3210; //4 bytes
        s.streamConfig[0].bpp = TestValue7654; //4 bytes

        s.streamConfig[0].streamtype = static_cast<CHISENSORSTREAMTYPE>(TestValue3210); //4 bytes
        s.streamConfig[1].vc = TestValue7654; //4 bytes

        s.streamConfig[1].dt = TestValue3210; //4 bytes
        s.streamConfig[1].frameDimension.left = TestValue7654; //4 bytes

        s.streamConfig[1].frameDimension.top = TestValue3210; //4 bytes
        s.streamConfig[1].frameDimension.width = TestValue7654; //4 bytes

        s.streamConfig[1].frameDimension.height = TestValue3210; //4 bytes
        s.streamConfig[1].bpp = TestValue7654; //4 bytes

        s.streamConfig[1].streamtype = static_cast<CHISENSORSTREAMTYPE>(TestValue3210); //4 bytes
        s.streamConfig[2].vc = TestValue7654; //4 bytes

        s.streamConfig[2].dt = TestValue3210; //4 bytes
        s.streamConfig[2].frameDimension.left = TestValue7654; //4 bytes

        s.streamConfig[2].frameDimension.top = TestValue3210; //4 bytes
        s.streamConfig[2].frameDimension.width = TestValue7654; //4 bytes

        s.streamConfig[2].frameDimension.height = TestValue3210; //4 bytes
        s.streamConfig[2].bpp = TestValue7654; //4 bytes

        s.streamConfig[2].streamtype = static_cast<CHISENSORSTREAMTYPE>(TestValue3210); //4 bytes
        s.streamConfig[3].vc = TestValue7654; //4 bytes

        s.streamConfig[3].dt = TestValue3210; //4 bytes
        s.streamConfig[3].frameDimension.left = TestValue7654; //4 bytes

        s.streamConfig[3].frameDimension.top = TestValue3210; //4 bytes
        s.streamConfig[3].frameDimension.width = TestValue7654; //4 bytes

        s.streamConfig[3].frameDimension.height = TestValue3210; //4 bytes
        s.streamConfig[3].bpp = TestValue7654; //4 bytes

        s.streamConfig[3].streamtype = static_cast<CHISENSORSTREAMTYPE>(TestValue3210); //4 bytes
        s.streamConfig[4].vc = TestValue7654; //4 bytes

        s.streamConfig[4].dt = TestValue3210; //4 bytes
        s.streamConfig[4].frameDimension.left = TestValue7654; //4 bytes

        s.streamConfig[4].frameDimension.top = TestValue3210; //4 bytes
        s.streamConfig[4].frameDimension.width = TestValue7654; //4 bytes

        s.streamConfig[4].frameDimension.height = TestValue3210; //4 bytes
        s.streamConfig[4].bpp = TestValue7654; //4 bytes

        s.streamConfig[4].streamtype = static_cast<CHISENSORSTREAMTYPE>(TestValue3210); //4 bytes
        s.activeArrayCropWindow.left = TestValue7654; //4 bytes

        s.activeArrayCropWindow.top = TestValue3210; //4 byte
        s.activeArrayCropWindow.width = TestValue7654; //4 bytes

        s.activeArrayCropWindow.height = TestValue3210; //4 bytes
        s.PDAFType = static_cast<CHIPDAFTYPE>(TestValue7654); //4 bytes

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }

    /**************************************************************************************************
    *   CameramanagerTest::TestCHIBUFFEROPTIONS
    *
    *   @brief
    *       Test to make sure struct memebers don't change
    **************************************************************************************************/
    void BinaryCompatibilityTest::TestCHIBUFFEROPTIONS()
    {
        UCHAR* pMem = NULL;
        pMem = reinterpret_cast<UCHAR*>(malloc(sizeof(CHIBUFFEROPTIONS)));
        NT_ASSERT(NULL != pMem, "Memory allocation failure (malloc)");
        NT_LOG_INFO("Size of CHIBUFFEROPTIONS: %u", sizeof(CHIBUFFEROPTIONS));
        static const UINT32 structSize = sizeof(CHIBUFFEROPTIONS);

        FillByteValue(pMem, structSize);

        CHIBUFFEROPTIONS s = { 0 };
        s.size = TestValue3210;
        s.minDimension.width = TestValue7654;
        s.minDimension.height = TestValue3210;
        s.maxDimension.width = TestValue7654;
        s.maxDimension.height = TestValue3210;
        s.optimalDimension.width = TestValue7654;
        s.optimalDimension.height = TestValue3210;

        NT_ASSERT(0 == memcmp(&s, pMem, structSize), "Struct Data Mismatch");

        free(pMem);
    }
}
