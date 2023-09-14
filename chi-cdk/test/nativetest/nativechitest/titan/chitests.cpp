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
#include "multi_camera_test.h"
#include "offline_pipeline_test.h"
#include "pip_camera_test.h"
#include "realtime_pipeline_test.h"
#include "recipe_test.h"

namespace chitests
{
    /* Register Testcases */
    //NATIVETEST_TEST(NativeCHIX, TestDIYPipeline)    { TestDIYPipeline();}

    // Basic testcase for number of cameras on device
    NATIVETEST_TEST(CameraModuleTest,  TestNumberOfCamera)                { TestNumberOfCamera(); }
    NATIVETEST_TEST(CameraModuleTest,  TestStaticCameraInfoAndMetadata)   { TestStaticCameraInfoAndMetadata(); }

    NATIVETEST_TEST(CameraManagerTest, TestOpenInvalidCamera)             { TestOpenInvalidCamera(); }
    NATIVETEST_TEST(CameraManagerTest, TestOpenCloseContext)              { TestOpenCloseContextSerially(); }

    // Custom node tests
    NATIVETEST_TEST(CustomNodeTest, TestVendorTagWrite)                   { TestCustomNode(TestVendorTagWrite); }

    // Binary Compatability Tests : To check integrity of chi.h structures
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIVENDORTAGSOPS)                 { TestCHIVENDORTAGSOPS(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHICAMERAINFO)                    { TestCHICAMERAINFO(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHISENSORMODEINFO)                { TestCHISENSORMODEINFO(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPIPELINECREATEDESCRIPTOR)      { TestCHIPIPELINECREATEDESCRIPTOR(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPORTBUFFERDESCRIPTOR)          { TestCHIPORTBUFFERDESCRIPTOR(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIBUFFEROPTIONS)                 { TestCHIBUFFEROPTIONS(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPIPELINEINFO)                  { TestCHIPIPELINEINFO(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHICALLBACKS)                     { TestCHICALLBACKS(); }
    NATIVETEST_TEST(BinaryCompatibilityTest, TestCHIPIPELINEREQUEST)               { TestCHIPIPELINEREQUEST(); }

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

    // Realtime pipeline test cases
    NATIVETEST_TEST(RealtimePipelineTest, TestIPEFull)                    { TestRealtimePipeline(TestIPEFull); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEFull)                    { TestRealtimePipeline(TestIFEFull); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEFullDualFoV)             { TestRealtimePipeline(TestIFEFullDualFoV); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEFull3A)                  { TestRealtimePipeline(TestIFEFull3A); }
    NATIVETEST_TEST(RealtimePipelineTest, TestPreviewHVX)                 { TestRealtimePipeline(TestPreviewHVX); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFERDI0RawOpaque)           { TestRealtimePipeline(TestIFERDI0RawOpaque); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFERDI0RawPlain16)          { TestRealtimePipeline(TestIFERDI0RawPlain16); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFERawCamif)                { TestRealtimePipeline(TestIFERawCamif); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFERawLsc)                  { TestRealtimePipeline(TestIFERawLsc); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFERDI0Raw10)               { TestRealtimePipeline(TestIFERDI0Raw10); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEFullStats)               { TestRealtimePipeline(TestIFEFullStats); }
    NATIVETEST_TEST(RealtimePipelineTest, TestRealtimePrevWith3AWrappers) { TestRealtimePipeline(TestRealtimePrevWith3AWrappers); }
    NATIVETEST_TEST(RealtimePipelineTest, TestRealtimePrevWithCallback)   { TestRealtimePipeline(TestRealtimePrevWithCallback); }
    NATIVETEST_TEST(RealtimePipelineTest, TestPreviewWithMemcpy)          { TestRealtimePipeline(TestPreviewWithMemcpy); }
    NATIVETEST_TEST(RealtimePipelineTest, TestPreviewFromMemcpy)          { TestRealtimePipeline(TestPreviewFromMemcpy); }
    NATIVETEST_TEST(RealtimePipelineTest, TestBypassNode)                 { TestRealtimePipeline(TestBypassNodeMemcpy); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIPE3A)                      { TestRealtimePipeline(TestIPE3A); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEIPEJpeg)                 { TestRealtimePipeline(TestIFEIPEJpeg); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEBPSIPEJpeg)              { TestRealtimePipeline(TestIFEBPSIPEJpeg); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEFD)                      { TestRealtimePipeline(TestIFEFD); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEChiDepthNode)            { TestRealtimePipeline(TestIFEChiDepthNode); }
    NATIVETEST_TEST(RealtimePipelineTest, TestIFEInplaceNode)             { TestRealtimePipeline(TestIFEInplaceNode); }

    // CVP related test cases
    NATIVETEST_TEST(RealtimePipelineTest, TestCVPDME)                     { TestRealtimePipeline(TestCVPDME, true); }
    NATIVETEST_TEST(RealtimePipelineTest, TestCVPDMEDisplayPort)          { TestRealtimePipeline(TestCVPDMEDisplayPort, true); }
    NATIVETEST_TEST(RealtimePipelineTest, TestCVPDMEICA)                  { TestRealtimePipeline(TestCVPDMEICA, true); }

    // Offline pipeline test cases
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayNV12)              { TestOfflinePipeline(TestIPEInputNV12DisplayNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayUBWCTP10)          { TestOfflinePipeline(TestIPEInputNV12DisplayUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayUBWCNV12)          { TestOfflinePipeline(TestIPEInputNV12DisplayUBWCNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputYUVOutputJPEG)                { TestOfflinePipeline(TestIPEInputYUVOutputJPEG); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12VideoNV12)                { TestOfflinePipeline(TestIPEInputNV12VideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12VideoUBWCNV12)            { TestOfflinePipeline(TestIPEInputNV12VideoUBWCNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12VideoUBWCTP10)            { TestOfflinePipeline(TestIPEInputNV12VideoUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayVideoNV12)         { TestOfflinePipeline(TestIPEInputNV12DisplayVideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayVideoUBWCNV12)     { TestOfflinePipeline(TestIPEInputNV12DisplayVideoUBWCNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayVideoUBWCTP10)     { TestOfflinePipeline(TestIPEInputNV12DisplayVideoUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayNV12VideoUBWCNV12) { TestOfflinePipeline(TestIPEInputNV12DisplayNV12VideoUBWCNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayUBWCNV12VideoNV12) { TestOfflinePipeline(TestIPEInputNV12DisplayUBWCNV12VideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayNV12VideoUBWCTP10) { TestOfflinePipeline(TestIPEInputNV12DisplayNV12VideoUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEInputNV12DisplayUBWCTP10VideoNV12) { TestOfflinePipeline(TestIPEInputNV12DisplayUBWCTP10VideoNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16OutFullUBWCTP10)         { TestOfflinePipeline(TestBPSInputRaw16OutFullUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw10OutFullUBWCTP10)         { TestOfflinePipeline(TestBPSInputRaw10OutFullUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16IPEDispNV12)             { TestOfflinePipeline(TestBPSInputRaw16IPEDispNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16IPEJpeg)                 { TestOfflinePipeline(TestBPSInputRaw16IPEJpeg); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16IPEDispUBWCNV12)         { TestOfflinePipeline(TestBPSInputRaw16IPEDispUBWCNV12); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16IPEDispUBWCTP10)         { TestOfflinePipeline(TestBPSInputRaw16IPEDispUBWCTP10); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16StatsOut)                { TestOfflinePipeline(TestBPSInputRaw16StatsOut); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputFHDRaw16StatsOut)             { TestOfflinePipeline(TestBPSInputFHDRaw16StatsOut); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputUHDRaw16StatsOutWithIPE)      { TestOfflinePipeline(TestBPSInputUHDRaw16StatsOutWithIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputFHDRaw16StatsOutWithIPE)      { TestOfflinePipeline(TestBPSInputFHDRaw16StatsOutWithIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputFHDRaw16BGStatsWithIPE)       { TestOfflinePipeline(TestBPSInputFHDRaw16BGStatsWithIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputUHDRaw16BGStatsWithIPE)       { TestOfflinePipeline(TestBPSInputUHDRaw16BGStatsWithIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputFHDRaw16HistStatsWithIPE)     { TestOfflinePipeline(TestBPSInputFHDRaw16HistStatsWithIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputUHDRaw16HistStatsWithIPE)     { TestOfflinePipeline(TestBPSInputUHDRaw16HistStatsWithIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestJPEGInputNV12VGAOutputBlob)           { TestOfflinePipeline(TestJPEGInputNV12VGAOutputBlob); }
    NATIVETEST_TEST(OfflinePipelineTest, TestJPEGInputNV124KOutputBlob)            { TestOfflinePipeline(TestJPEGInputNV124KOutputBlob); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16OutputGPUPorts)          { TestOfflinePipeline(TestBPSInputRaw16OutputGPUPorts); }
    NATIVETEST_TEST(OfflinePipelineTest, TestFDInputNV12VGAOutputBlob)             { TestOfflinePipeline(TestFDInputNV12VGAOutputBlob); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw16WithIPEAndDS)            { TestOfflinePipeline(TestBPSInputRaw16WithIPEAndDS); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSInputRaw10WithIPEAndDS)            { TestOfflinePipeline(TestBPSInputRaw10WithIPEAndDS); }

    NATIVETEST_TEST(OfflinePipelineTest, TestIPEMFHDRSnapshot)                     { TestOfflinePipeline(TestIPEMFHDRSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEMFHDRPreview)                      { TestOfflinePipeline(TestIPEMFHDRPreview); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEMFHDRSnapshotE2E)                  { TestOfflinePipeline(TestIPEMFHDRSnapshotE2E); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEMFHDRPreviewE2E)                   { TestOfflinePipeline(TestIPEMFHDRPreviewE2E); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIdealRaw)                          { TestOfflinePipeline(TestBPSIdealRaw); }
    NATIVETEST_TEST(OfflinePipelineTest, TestChiExternalNodeBPSIPE)                { TestOfflinePipeline(TestChiExternalNodeBPSIPE); }
    NATIVETEST_TEST(OfflinePipelineTest, TestChiExternalNodeBPSIPEJpeg)            { TestOfflinePipeline(TestChiExternalNodeBPSIPEJpeg); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPE2Streams)                       { TestOfflinePipeline(TestBPSIPE2Streams); }
    NATIVETEST_TEST(OfflinePipelineTest, TestChiDepthNode)                         { TestOfflinePipeline(TestChiDepthNode); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEBlurModule)                        { TestOfflinePipeline(TestIPEBlurModule); }

    // Generic Offline Pipeline Tests ( runtime configurable for resolution, format and port )
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEDisplay)                           { TestOfflinePipeline(TestIPEDisplay); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEVideo)                             { TestOfflinePipeline(TestIPEVideo); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEDisplayVideo)                      { TestOfflinePipeline(TestIPEDisplayVideo); }
    NATIVETEST_TEST(OfflinePipelineTest, TestIPEJPEGSnapshot)                      { TestOfflinePipeline(TestIPEJPEGSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSSnapshot)                          { TestOfflinePipeline(TestBPSSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPEYuvSnapshot)                    { TestOfflinePipeline(TestBPSIPEYuvSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPEJPEGSnapshot)                   { TestOfflinePipeline(TestBPSIPEJPEGSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSStats)                             { TestOfflinePipeline(TestBPSStats); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPEYuvStats)                       { TestOfflinePipeline(TestBPSIPEYuvStats); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPEYuvStatsBG)                     { TestOfflinePipeline(TestBPSIPEYuvStatsBG); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPEYuvStatsBHist)                  { TestOfflinePipeline(TestBPSIPEYuvStatsBHist); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSIPEWithDS)                         { TestOfflinePipeline(TestBPSIPEWithDS); }
    NATIVETEST_TEST(OfflinePipelineTest, TestBPSGPUWithDS)                         { TestOfflinePipeline(TestBPSGPUWithDS); }
    NATIVETEST_TEST(OfflinePipelineTest, TestJPEGNodeJPEGSnapshot)                 { TestOfflinePipeline(TestJPEGNodeJPEGSnapshot); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOfflineFD)                            { TestOfflinePipeline(TestOfflineFD); }
    NATIVETEST_TEST(OfflinePipelineTest, TestOfflineIPEFD)                         { TestOfflinePipeline(TestOfflineIPEFD); }

    /* End-to-end Pipeline Test Suite */
    NATIVETEST_TEST(E2ESuite, TestPreviewJpegSnapshot)                             { TestE2EPipeline(TestPreviewJpegSnapshot); }
    NATIVETEST_TEST(E2ESuite, TestPreviewYuvSnapshot)                              { TestE2EPipeline(TestPreviewYuvSnapshot); }
    NATIVETEST_TEST(E2ESuite, TestPreviewJpegSnapshot3AStats)                      { TestE2EPipeline(TestPreviewJpegSnapshot3AStats); }
    NATIVETEST_TEST(E2ESuite, TestPreviewYuvSnapshot3AStats)                       { TestE2EPipeline(TestPreviewYuvSnapshot3AStats); }
    NATIVETEST_TEST(E2ESuite, TestLiveVideoSnapshot)                               { TestE2EPipeline(TestLiveVideoSnapshot); }
    NATIVETEST_TEST(E2ESuite, TestPreviewCallbackSnapshotThumbnail)                { TestE2EPipeline(TestPreviewCallbackSnapshotThumbnail); }
    NATIVETEST_TEST(E2ESuite, TestHDRPreviewVideo)                                 { TestE2EPipeline(TestHDRPreviewVideo); }

    // Picture-in-picture tests
    NATIVETEST_TEST(PIPCameraTest, PIPCameraIFEFull)                               { TestPIPCamera(PIPCameraIFEFull); }
    NATIVETEST_TEST(PIPCameraTest, PIPCameraIFEGeneric)                            { TestPIPCamera(PIPCameraIFEGeneric); }

    /* Recipe Test Suite*/
    // Single output stream tests with configurable resolution, output format and output port
    NATIVETEST_TEST(RecipeTest, TestIFEGeneric)                                    { TestRealtimePipeline(TestIFEGeneric); }
    NATIVETEST_TEST(RecipeTest, TestIFEHDR)                                        { TestRealtimePipeline(TestIFEHDR); }
    NATIVETEST_TEST(RecipeTest, TestIFEGenericOffline)                             { TestOfflinePipeline(TestIFEGenericOffline); }
    NATIVETEST_TEST(RecipeTest, TestIPEGeneric)                                    { TestOfflinePipeline(TestIPEGeneric); }
    NATIVETEST_TEST(RecipeTest, TestBPSGeneric)                                    { TestOfflinePipeline(TestBPSGeneric); }
    NATIVETEST_TEST(RecipeTest, TestJPEGGeneric)                                   { TestOfflinePipeline(TestJPEGGeneric); }
    NATIVETEST_TEST(RecipeTest, TestChiGPUNode)                                    { TestOfflinePipeline(TestChiGPUNode); }

    // Single output stream tests with configurable resolution, output format and fixed output Downscale ports
    NATIVETEST_TEST(RecipeTest, TestBPSDownscaleGeneric)                           { TestOfflinePipeline(TestBPSDownscaleGeneric); }
    NATIVETEST_TEST(RecipeTest, TestIFEDownscaleGeneric)                           { TestRealtimePipeline(TestIFEDownscaleGeneric); }

    // Double output stream tests (static + configurable); configurable resolution, output format, output port
    NATIVETEST_TEST(RecipeTest, TestIFEGeneric2Stream)                             { TestRealtimePipeline(TestIFEGeneric2Stream); }
    NATIVETEST_TEST(RecipeTest, TestIFEGeneric2StreamDisplay)                      { TestRealtimePipeline(TestIFEGeneric2StreamDisplay); }
    NATIVETEST_TEST(RecipeTest, TestIPEGeneric2StreamDisplay)                      { TestOfflinePipeline(TestIPEGeneric2StreamDisplay); }
    NATIVETEST_TEST(RecipeTest, TestBPSGeneric2Stream)                             { TestOfflinePipeline(TestBPSGeneric2Stream); }
    NATIVETEST_TEST(RecipeTest, TestIFELiteTLBGStatsOut)                           { TestRealtimePipeline(TestIFELiteTLBGStatsOut); }

    //triple output stream tests
    NATIVETEST_TEST(RecipeTest, TestIFELiteRawTLBGStatsOut)                        { TestRealtimePipeline(TestIFELiteRawTLBGStatsOut); }

    // CVP stand alone
    NATIVETEST_TEST(RecipeTest, TestCVPDenseMotion)                             { TestOfflinePipeline(TestCVPDenseMotion); }

    /* MultiCameraTest suite */
    NATIVETEST_TEST(MultiCameraTest, MultiCameraIFEFull)                           { TestMultiCamera(MultiCameraIFEFull); }
    NATIVETEST_TEST(MultiCameraTest, MultiCameraIFEFullFlushSinglePipeline)        { TestMultiCamera(MultiCameraIFEFullFlushSinglePipeline); }

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
