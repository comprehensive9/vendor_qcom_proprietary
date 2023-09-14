 //*******************************************************************************
 // Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
 // All Rights Reserved.
 // Confidential and Proprietary - Qualcomm Technologies, Inc.
 //*******************************************************************************

#include "nativetest.h"
#include "cmdlineparser.h"
#include "nativetestlog.h"
#include "chitestcase.h"
#include "binary_comp_test.h"
#include "camera_module_test.h"
#include "camera_manager_test.h"
#include "chimetadata_test.h"
#include "custom_node_test.h"
#include "diy.h"
#include "e2e_suite.h"
#include "offline_pipeline_test.h"
#include "pip_camera_test.h"
#include "realtime_pipeline_test.h"
#include "recipe_test.h"
#include "multi_camera_test.h"
#include "e2e_suite.h"

namespace chitests
{

    /* Register Testcases */
    NATIVETEST_TEST(NativeCHIX, TestDIYPipeline)    {TestDIYPipeline();}

    // Basic testcase for number of cameras on device
    NATIVETEST_TEST(CameraModuleTest,  TestNumberOfCamera)                 { TestNumberOfCamera(); }
    NATIVETEST_TEST(CameraModuleTest,  TestStaticCameraInfoAndMetadata)    { TestStaticCameraInfoAndMetadata(); }

    NATIVETEST_TEST(CameraManagerTest, TestOpenInvalidCamera)              { TestOpenInvalidCamera(); }
    NATIVETEST_TEST(CameraManagerTest, TestOpenCloseContext)               { TestOpenCloseContextSerially(); }

    // Custom node tests
    NATIVETEST_TEST(CustomNodeTest, TestVendorTagWrite)                   { TestCustomNode(TestVendorTagWrite); }

    // Chimetadata tests
    NATIVETEST_TEST(ChiMetadataTest, TestCreateMetadata)                  { TestCreateMetadata(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCreateDefaultMetadata)           { TestCreateDefaultMetadata(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCreateWithAndroidMetadata)       { TestCreateWithAndroidMetadata(); }
    NATIVETEST_TEST(ChiMetadataTest, TestSetTag)                          { TestSetTag(); }
    NATIVETEST_TEST(ChiMetadataTest, TestGetTag)                          { TestGetTag(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCreateWithTagArray)              { TestCreateWithTagArray(); }
    NATIVETEST_TEST(ChiMetadataTest, TestDump)                            { TestDump(); }
    NATIVETEST_TEST(ChiMetadataTest, TestPrint)                           { TestPrint(); }
    NATIVETEST_TEST(ChiMetadataTest, TestGetMetadataOps)                  { TestGetMetadataOps(); }
    NATIVETEST_TEST(ChiMetadataTest, TestGetTagCount)                     { TestGetTagCount(); }
    NATIVETEST_TEST(ChiMetadataTest, TestGetMetadataEntry)                { TestGetMetadataEntry(); }
    NATIVETEST_TEST(ChiMetadataTest, TestDeleteTag)                       { TestDeleteTag(); }
    NATIVETEST_TEST(ChiMetadataTest, TestVendorTagOps)                    { TestVendorTagOps(); }
    NATIVETEST_TEST(ChiMetadataTest, TestMergeDisjoint)                   { TestMergeDisjoint(); }
    NATIVETEST_TEST(ChiMetadataTest, TestMerge)                           { TestMerge(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCopy)                            { TestCopy(); }
    NATIVETEST_TEST(ChiMetadataTest, TestClone)                           { TestClone(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCopyDisjoint)                    { TestCopyDisjoint(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCapacity)                        { TestCapacity(); }
    NATIVETEST_TEST(ChiMetadataTest, TestGetMetadataTable)                { TestGetMetadataTable(); }
    NATIVETEST_TEST(ChiMetadataTest, TestIterator)                        { TestIterator(); }
    NATIVETEST_TEST(ChiMetadataTest, TestMetadataReference)               { TestMetadataReference(); }
    NATIVETEST_TEST(ChiMetadataTest, TestInvalidate)                      { TestInvalidate(); }
    NATIVETEST_TEST(ChiMetadataTest, TestCopyPerformance)                 { TestCopyPerformance(); }
    NATIVETEST_TEST(ChiMetadataTest, TestMergePerformance)                { TestMergePerformance(); }
    NATIVETEST_TEST(ChiMetadataTest, TestDestroyMetadata)                 { TestDestroyMetadata(); }

    // binary compatability tests
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIVENDORTAGSOPS)                 { TestCHIVENDORTAGSOPS(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHICAMERAINFO)                    { TestCHICAMERAINFO(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHISENSORMODEINFO)                { TestCHISENSORMODEINFO(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPIPELINECREATEDESCRIPTOR)      { TestCHIPIPELINECREATEDESCRIPTOR(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPORTBUFFERDESCRIPTOR)          { TestCHIPORTBUFFERDESCRIPTOR(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIBUFFEROPTIONS)                 { TestCHIBUFFEROPTIONS(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPIPELINEINFO)                  { TestCHIPIPELINEINFO(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHICALLBACKS)                     { TestCHICALLBACKS(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPIPELINEREQUEST)               { TestCHIPIPELINEREQUEST(); }

    // Specific to Mimas Architecture (TFE->OPE vs IFE->BPS->IPE)
    // Mimas Recipe Tests
    NATIVETEST_TEST(RecipeTest, TestTFEGeneric)                      { TestRealtimePipeline(TestTFEGeneric); }
    NATIVETEST_TEST(RecipeTest, TestTFEGeneric2Stream)               { TestRealtimePipeline(TestTFEGeneric2Stream); }
    NATIVETEST_TEST(RecipeTest, TestOPEGeneric2StreamDisplay)        { TestOfflinePipeline(TestOPEGeneric2StreamDisplay); }
    NATIVETEST_TEST(RecipeTest, TestOPEGeneric)                      { TestOfflinePipeline(TestOPEGeneric); }
    NATIVETEST_TEST(RecipeTest, TestOPEGeneric3StreamDisplayStats)   { TestOfflinePipeline(TestOPEGeneric3StreamDisplayStats); }
    NATIVETEST_TEST(RecipeTest, TestInputConfigJpegSwOut)            { TestOfflinePipeline(TestInputConfigJpegSwOut); }
    NATIVETEST_TEST(RecipeTest, TestTFERDIOutMIPI12)                 { TestRealtimePipeline(TestTFERDIOutMIPI12); }

    // Mimas Realtime Tests
    NATIVETEST_TEST(RealtimePipelineTest, TestTFERDI0RawOpaque)           { TestRealtimePipeline(TestTFERDI0RawOpaque); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFERDI0Raw10)               { TestRealtimePipeline(TestTFERDI0Raw10); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFERDI0RawPlain16)          { TestRealtimePipeline(TestTFERDI0RawPlain16); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFERDI1RawPlain16)          { TestRealtimePipeline(TestTFERDI1RawPlain16); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFEFull)                    { TestRealtimePipeline(TestTFEFull); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFEFullStats)               { TestRealtimePipeline(TestTFEFullStats); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFERDI1RawMIPI)             { TestRealtimePipeline(TestTFERDI1RawMIPI); }
    NATIVETEST_TEST(RealtimePipelineTest, TestPrevRDITFEOPE)              { TestRealtimePipeline(TestPrevRDITFEOPE); }
    NATIVETEST_TEST(RealtimePipelineTest, TestOPEFull)                    { TestRealtimePipeline(TestOPEFull); }
    NATIVETEST_TEST(RealtimePipelineTest, TestPreviewWithMemcpyTFEOPE)    { TestRealtimePipeline(TestPreviewWithMemcpyTFEOPE); }
    NATIVETEST_TEST(RealtimePipelineTest, TestPreviewFromMemcpyTFEOPE)    { TestRealtimePipeline(TestPreviewFromMemcpyTFEOPE); }
    NATIVETEST_TEST(RealtimePipelineTest, TestBypassNodeTFEOPE)           { TestRealtimePipeline(TestBypassNodeMemcpyTFEOPE); }
    NATIVETEST_TEST(RealtimePipelineTest, TestTFEFull3A)                  { TestRealtimePipeline(TestTFEFull3A); }

    // Mimas Offline Tests
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12DisplayNV12)             { TestOfflinePipeline(TestOPEInputNV12DisplayNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12DisplayNV21)             { TestOfflinePipeline(TestOPEInputNV12DisplayNV21); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12VideoNV12)               { TestOfflinePipeline(TestOPEInputNV12VideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12VideoNV21)               { TestOfflinePipeline(TestOPEInputNV12VideoNV21); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputConfigOutFullNV12)            { TestOfflinePipeline(TestOPEInputConfigOutFullNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputConfigOutFullNV21)            { TestOfflinePipeline(TestOPEInputConfigOutFullNV21); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12DisplayNV21VideoNV12)    { TestOfflinePipeline(TestOPEInputNV12DisplayNV21VideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12DisplayNV12VideoNV21)    { TestOfflinePipeline(TestOPEInputNV12DisplayNV12VideoNV21); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12DisplayVideoNV12)        { TestOfflinePipeline(TestOPEInputNV12DisplayVideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputNV12DisplayVideoNV21)        { TestOfflinePipeline(TestOPEInputNV12DisplayVideoNV21); }
    NATIVETEST_TEST(OfflinePipelineTest,  TestOPEJPEGSWSnapshot)                  { TestOfflinePipeline(TestOPEJPEGSWSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOPEInputConfigStatsOut)              { TestOfflinePipeline(TestOPEInputConfigStatsOut); }

    // Picture-in-picture tests
    NATIVETEST_TEST(PIPCameraTest, PIPCameraTFEFull)                              { TestPIPCamera(PIPCameraTFEFull); }

    //Mimas MultiCamera Tests
    NATIVETEST_TEST(MultiCameraTest, MultiCameraTFEFull)                          { TestMultiCamera(MultiCameraTFEFull); }

    /* End-to-end Pipeline Test Suite */
    NATIVETEST_TEST(E2ESuite, TestPreviewJpegSnapshot)                            { TestE2EPipeline(TestPreviewJpegSnapshot); }
    NATIVETEST_TEST(E2ESuite, TestPreviewYuvSnapshot)                             { TestE2EPipeline(TestPreviewYuvSnapshot); }
    NATIVETEST_TEST(E2ESuite, TestPreviewJpegSnapshot3AStats)                     { TestE2EPipeline(TestPreviewJpegSnapshot3AStats); }
    NATIVETEST_TEST(E2ESuite, TestPreviewYuvSnapshot3AStats)                      { TestE2EPipeline(TestPreviewYuvSnapshot3AStats); }
    NATIVETEST_TEST(E2ESuite, TestLiveVideoSnapshot)                              { TestE2EPipeline(TestLiveVideoSnapshot); }

    // Convenience helper to run main code inside tests namespace
    int RunNativeChiTest(int argc, char** argv)
    {

        // print the main title with platform info
        NT_LOG_STD("===================| NativeChiTest 3.1 |===================");

        // parse all command line options, print help menu if illegal option provided
        int cmdCheck = CmdLineParser::ParseCommandLine(argc, argv);

        // set the verbose level
        verboseSeverity eSev = static_cast<verboseSeverity>(CmdLineParser::GetLogLevel());
        if (-1 == NTLog.SetLogLevel(eSev))
        {
            NT_LOG_ERROR("Invalid log level %d", CmdLineParser::GetLogLevel());
            return 1;
        }

        switch (cmdCheck)
        {
        case 0:
            //Run tests inputted at command line when Json not present
            cmdCheck = RunTests();
            ShutDown();
            break;
        case 1:
            //Parse Json file and run tests
            JsonParser::executeJson(CmdLineParser::GetJsonFileName());
            break;
        case -1: //Command line error input
        default:
            CmdLineParser::PrintCommandLineUsage();
            cmdCheck = 1;
            break;
        }

        return cmdCheck;
    }

} //namespace chitests

int main(int argc, char** argv)
{
    return chitests::RunNativeChiTest(argc, argv);
}
