 //******************************************************************************
 // Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
 // All Rights Reserved.
 // Confidential and Proprietary - Qualcomm Technologies, Inc.
 //*******************************************************************************

#include "nativetest.h"
#include "camera_module_test.h"
#include "camera_device_test.h"
#include "camera_manager_test.h"
#include "still_capture_test.h"
#include "extended_camera_characteristics_test.h"
#include "capture_request_test.h"
#include "robustness_test.h"
#include "vendor_tags_test.h"
#include "recording_test.h"
#include "concurrent_capture_test.h"
#include "ife_unit_test.h"
#include "preview_capture_test.h"
#include "partial_metadata_test.h"
#include "flush_test.h"
#include "multi_frame_test.h"
#include "nativehalx.h"

namespace haltests {

    /* NativeHalX test */
    NATIVETEST_TEST(NativeHalX, OmniTest) { OmniTest(); }

    NATIVETEST_TEST(CameraModuleTest, TestNumberOfCamera) { TestNumberOfCamera(); }
    NATIVETEST_TEST(CameraModuleTest, TestStaticCameraInfoAndMetadata) { TestStaticCameraInfoAndMetadata(); }

    NATIVETEST_TEST(CameraDeviceTest, TestCaptureTemplateByCamera) { TestCaptureTemplateByCamera(); }

    NATIVETEST_TEST(CameraManagerTest, TestOpenInvalidCamera) { TestOpenInvalidCamera(); }
    NATIVETEST_TEST(CameraManagerTest, TestOpenCamerasSerially) { TestOpenCamerasSerially(); }
    NATIVETEST_TEST(CameraManagerTest, TestOpenAllCamerasFrontFirst) { TestOpenAllCamerasFrontFirst(); }
    NATIVETEST_TEST(CameraManagerTest, TestOpenAllCamerasBackFirst) { TestOpenAllCamerasBackFirst(); }

    NATIVETEST_TEST(ExtendedCameraCharacteristicsTest, TestKeys) { TestKeys(); }
    NATIVETEST_TEST(ExtendedCameraCharacteristicsTest, TestAvailableStreamConfigs) { TestAvailableStreamConfigs(); }
    NATIVETEST_TEST(ExtendedCameraCharacteristicsTest, TestReprocessingCharacteristics) { TestReprocessingCharacteristics(); }
    NATIVETEST_TEST(ExtendedCameraCharacteristicsTest, TestOpticalBlackRegions) { TestOpticalBlackRegions(); }
    NATIVETEST_TEST(ExtendedCameraCharacteristicsTest, TestStaticBurstCharacteristics) { TestStaticBurstCharacteristics(); }
    NATIVETEST_TEST(ExtendedCameraCharacteristicsTest, TestDepthOutputCharacteristics) { TestDepthOutputCharacteristics(); }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, "TestBasicYUVCaptureVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureqVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, QVGA, "TestBasicYUVCaptureqVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, HD, "TestBasicYUVCaptureHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureFHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, FULLHD, "TestBasicYUVCaptureFHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVP010CaptureFHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCBCR_P010, FULLHD, "TestBasicYUVCaptureFHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureUHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, UHD, "TestBasicYUVCaptureUHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCapture4K)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, HD4K, "TestBasicYUVCapture4K", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMax)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MAX_RES, "TestBasicYUVCaptureMax", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicRaw16CaptureMax)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_RAW16, RDI4608_2592, "TestBasicRaw16CaptureMax", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicRaw10CaptureMax)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_RAW10, RDI4608_2592, "TestBasicRaw10CaptureMax", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureqVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, QVGA, "TestBasicJpegCaptureqVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, VGA, "TestBasicJpegCaptureVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, HD, "TestBasicJpegCaptureHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureFHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, FULLHD, "TestBasicJpegCaptureFHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureUHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, UHD, "TestBasicJpegCaptureUHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureQVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, QVGA, "TestBasicPrivateCaptureQVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, VGA, "TestBasicPrivateCaptureVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HD, "TestBasicPrivateCaptureHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureFHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD, "TestBasicPrivateCaptureFHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureUHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestBasicPrivateCaptureUHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMax)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MAX_RES, "TestBasicPrivateCaptureMax", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureNQCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NQCIF, "TestBasicPrivateCaptureNQCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCapturePQCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, PQCIF, "TestBasicPrivateCapturePQCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, CIF, "TestBasicPrivateCaptureCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCapturePVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, PVGA, "TestBasicPrivateCapturePVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureSize720_480)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, Size720_480, "TestBasicPrivateCaptureSize720_480", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureFWVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FWVGA, "TestBasicPrivateCaptureFWVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureSize640_360)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, Size640_360, "TestBasicPrivateCaptureSize640_360", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureWVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, WVGA, "TestBasicPrivateCaptureWVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureWXGA1)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, WXGA1, "TestBasicPrivateCaptureWXGA1", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureWXGA2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, WXGA2, "TestBasicPrivateCaptureWXGA2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP1_3)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP1_3, "TestBasicPrivateCaptureMP1_3", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP1)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP1, "TestBasicPrivateCaptureMP1", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP2, "TestBasicPrivateCaptureMP2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP3)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP3, "TestBasicPrivateCaptureMP3", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureNQCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, NQCIF, "TestBasicYUVCaptureNQCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCapturePQCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, PQCIF, "TestBasicYUVCapturePQCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, CIF, "TestBasicYUVCaptureCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCapturePVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, PVGA, "TestBasicYUVCapturePVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureSize720_480)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, Size720_480, "TestBasicYUVCaptureSize720_480", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureFWVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, FWVGA, "TestBasicYUVCaptureFWVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureSize640_360)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, Size640_360, "TestBasicYUVCaptureSize640_360", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureWVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, WVGA, "TestBasicYUVCaptureWVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureWXGA1)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, WXGA1, "TestBasicYUVCaptureWXGA1", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureWXGA2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, WXGA2, "TestBasicYUVCaptureWXGA2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP1_3)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP1_3, "TestBasicYUVCaptureMP1_3", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP1)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP1, "TestBasicYUVCaptureMP1", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP2, "TestBasicYUVCaptureMP2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP3)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP3, "TestBasicYUVCaptureMP3", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureNQCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, NQCIF, "TestBasicJpegCaptureNQCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCapturePQCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, PQCIF, "TestBasicJpegCapturePQCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, CIF, "TestBasicJpegCaptureCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCapturePVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, PVGA, "TestBasicJpegCapturePVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureSize720_480)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, Size720_480, "TestBasicJpegCaptureSize720_480", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureFWVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, FWVGA, "TestBasicJpegCaptureFWVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureSize640_360)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, Size640_360, "TestBasicJpegCaptureSize640_360", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureWVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, WVGA, "TestBasicJpegCaptureWVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureWXGA1)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, WXGA1, "TestBasicJpegCaptureWXGA1", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureWXGA2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, WXGA2, "TestBasicJpegCaptureWXGA2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP1_3)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP1_3, "TestBasicJpegCaptureMP1_3", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP1)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP1, "TestBasicJpegCaptureMP1", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP2, "TestBasicJpegCaptureMP2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP3)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP3, "TestBasicJpegCaptureMP3", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP4)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP4, "SingleCameraBasicPrivateCaptureMP4", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP5)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP5, "SingleCameraBasicPrivateCaptureMP5", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureFULLHD_43)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD_43, "SingleCameraBasicPrivateCaptureFULLHD_43", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureSVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, SVGA, "SingleCameraBasicPrivateCaptureSVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCapturePHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, PHD, "SingleCameraBasicPrivateCapturePHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCapturePCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, PCIF, "SingleCameraBasicPrivateCapturePCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCapturePQVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, PQVGA, "SingleCameraBasicPrivateCapturePQVGA", HalJsonParser::sFrames, false);
    }

    /*NATIVETEST_TEST(StillCaptureTest, DISABLED_SingleCameraBasicPrivateCaptureRAW4208_3120)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, RAW4208_3120, "SingleCameraBasicPrivateCaptureRAW4208_3120", HalJsonParser::sFrames, false);
    }*/

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP13)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP13, "SingleCameraBasicPrivateCaptureMP13", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP12_2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP12_2, "SingleCameraBasicPrivateCaptureMP12_2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP8_Wide)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP8_Wide, "SingleCameraBasicPrivateCaptureMP8_Wide", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureMP8)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, MP8, "SingleCameraBasicPrivateCaptureMP8", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureSQUARE)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, SQUARE, "SingleCameraBasicPrivateCaptureSQUARE", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicPrivateCaptureSize160_120)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, Size160_120, "SingleCameraBasicPrivateCaptureSize160_120", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP4)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP4, "SingleCameraBasicYUVCaptureMP4", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP5)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP5, "SingleCameraBasicYUVCaptureMP5", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureFULLHD_43)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, FULLHD_43, "SingleCameraBasicYUVCaptureFULLHD_43", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureSVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, SVGA, "SingleCameraBasicYUVCaptureSVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCapturePHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, PHD, "SingleCameraBasicYUVCapturePHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCapturePCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, PCIF, "SingleCameraBasicYUVCapturePCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCapturePQVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, PQVGA, "SingleCameraBasicYUVCapturePQVGA", HalJsonParser::sFrames, false);
    }

    /*NATIVETEST_TEST(StillCaptureTest, DISABLED_SingleCameraBasicYUVCaptureRAW4208_3120)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, RAW4208_3120, "SingleCameraBasicYUVCaptureRAW4208_3120", HalJsonParser::sFrames, false);
    }*/

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP13)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP13, "SingleCameraBasicYUVCaptureMP13", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP12_2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP12_2, "SingleCameraBasicYUVCaptureMP12_2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP8_Wide)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP8_Wide, "SingleCameraBasicYUVCaptureMP8_Wide", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureMP8)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, MP8, "SingleCameraBasicYUVCaptureMP8", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureSQUARE)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, SQUARE, "SingleCameraBasicYUVCaptureSQUARE", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicYUVCaptureSize160_120)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, Size160_120, "SingleCameraBasicYUVCaptureSize160_120", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP4)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP4, "SingleCameraBasicJpegCaptureMP4", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP5)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP5, "SingleCameraBasicJpegCaptureMP5", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureFULLHD_43)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, FULLHD_43, "SingleCameraBasicJpegCaptureFULLHD_43", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureSVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, SVGA, "SingleCameraBasicJpegCaptureSVGA", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCapturePHD)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, PHD, "SingleCameraBasicJpegCapturePHD", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCapturePCIF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, PCIF, "SingleCameraBasicJpegCapturePCIF", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCapturePQVGA)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, PQVGA, "SingleCameraBasicJpegCapturePQVGA", HalJsonParser::sFrames, false);
    }

    /*NATIVETEST_TEST(StillCaptureTest, DISABLED_SingleCameraBasicJpegCaptureRAW4208_3120)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, RAW4208_3120, "SingleCameraBasicJpegCaptureRAW4208_3120", HalJsonParser::sFrames, false);
    }*/

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP13)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP13, "SingleCameraBasicJpegCaptureMP13", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP12_2)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP12_2, "SingleCameraBasicJpegCaptureMP12_2", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP8_Wide)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP8_Wide, "SingleCameraBasicJpegCaptureMP8_Wide", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureMP8)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, MP8, "SingleCameraBasicJpegCaptureMP8", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureSQUARE)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, SQUARE, "SingleCameraBasicJpegCaptureSQUARE", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, SingleCameraBasicJpegCaptureSize160_120)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_BLOB, Size160_120, "SingleCameraBasicJpegCaptureSize160_120", HalJsonParser::sFrames, false);
    }

    // Tests which require override settings only
    NATIVETEST_TEST(StillCaptureTest, TestPreviewBokeh)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestPreviewBokeh", HalJsonParser::sFrames, false, SettingBokeh);
    }
    NATIVETEST_TEST(StillCaptureTest, TestPreviewSAT)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestPreviewSAT", HalJsonParser::sFrames, false, SettingSAT);
    }
    NATIVETEST_TEST(StillCaptureTest, TestSnapshotSWMF)
    {
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, UHD, "TestSnapshotSWMF", HalJsonParser::sFrames, false, SettingSWMF);
    }


    NATIVETEST_TEST(StillCaptureTest, PrivPrevVGAYUVSnapVGA)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, NOT_APPLICABLE, VGA, VGA, UHD, "PrivPrevVGAYUVSnapVGA", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevVGAYUVSnapHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, NOT_APPLICABLE, VGA, HD, UHD, "PrivPrevVGAYUVSnapHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevVGAYUVSnapFHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, NOT_APPLICABLE, VGA, FULLHD, UHD, "PrivPrevVGAYUVSnapFHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevVGAYUVSnapUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, NOT_APPLICABLE, VGA, UHD, UHD, "PrivPrevVGAYUVSnapUHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevVGAYUVSnap4k)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, NOT_APPLICABLE, VGA, HD4K, UHD, "PrivPrevVGAYUVSnap4k", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDJpegSnapUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_BLOB, NOT_APPLICABLE, FULLHD, UHD, UHD, "PrivPrevFHDJpegSnapUHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDJpegSnapUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, NOT_APPLICABLE, FULLHD, UHD, UHD, "YUVPrevFHDJpegSnapUHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDRawSnapMax)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_RAW16, NOT_APPLICABLE, FULLHD, RDI4608_2592, UHD, "PrivPrevFHDRawSnapMax", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDRawSnapMax)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_RAW16, NOT_APPLICABLE, FULLHD, RDI4608_2592, UHD, "YUVPrevFHDRawSnapMax", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDPrivSnapQVGA)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, QVGA, UHD, "PrivPrevFHDPrivSnapQVGA", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDPrivSnapVGA)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, VGA, UHD, "PrivPrevFHDPrivSnapVGA", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDPrivSnapHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, HD, UHD, "PrivPrevFHDPrivSnapHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDPrivSnapFHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, FULLHD, UHD, "PrivPrevFHDPrivSnapFHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDPrivSnapUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, UHD, UHD, "PrivPrevFHDPrivSnapUHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDPrivSnapMax)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, MAX_RES, UHD, "PrivPrevFHDPrivSnapMax", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDYUVSnapQVGA)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, QVGA, UHD, "YUVPrevFHDYUVSnapQVGA", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDYUVSnapVGA)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, VGA, UHD, "YUVPrevFHDYUVSnapVGA", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDYUVSnapHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, HD, UHD, "YUVPrevFHDYUVSnapHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDYUVSnapFHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, FULLHD, UHD, "YUVPrevFHDYUVSnapFHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDYUVSnapUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, UHD, UHD, "YUVPrevFHDYUVSnapUHD", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDYUVSnapMax)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, NOT_APPLICABLE, FULLHD, MAX_RES, UHD, "YUVPrevFHDYUVSnapMax", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, PrivPrevFHDJpegSnapUHDRawSnapMax)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_BLOB, HAL_PIXEL_FORMAT_RAW16, FULLHD, UHD, RDI4608_2592, "PrivPrevFHDJpegSnapUHDRawSnapMax", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, YUVPrevFHDJpegSnapUHDRawSnapMax)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, HAL_PIXEL_FORMAT_RAW16, FULLHD, UHD, RDI4608_2592, "YUVPrevFHDJpegSnapUHDRawSnapMax", HalJsonParser::sSnaps, true, false);
    }

    NATIVETEST_TEST(StillCaptureTest, ZSLPrivPrevJpegUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_BLOB, NOT_APPLICABLE, FULLHD, UHD, UHD, "ZSLPrivPrevJpegUHD", HalJsonParser::sSnaps, false, true);
    }

    NATIVETEST_TEST(StillCaptureTest, ZSLYUVPrevJpegUHD)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, NOT_APPLICABLE, FULLHD, UHD, UHD, "ZSLYUVPrevJpegUHD", HalJsonParser::sSnaps, false, true);
    }

    NATIVETEST_TEST(StillCaptureTest, TestHEIF)
    {
        TestSnapshotWithPreview(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, FULLHD, UHD, UHD, "TestHEIF", HalJsonParser::sSnaps, false, true);
    }

    /*NATIVETEST_TEST(StillCaptureTest, DISABLED_PrivPrevFHDYUVPrevFHDJpegSnapMP4YUVSnapMP4YUVSnapQVGA)
    {
        StillCaptureItem previews[] = { StillCaptureItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD), StillCaptureItem(HAL_PIXEL_FORMAT_YCbCr_420_888, FULLHD) };
        StillCaptureItem snapshots[] = { StillCaptureItem(HAL_PIXEL_FORMAT_BLOB, MP4), StillCaptureItem(HAL_PIXEL_FORMAT_YCbCr_420_888, MP4), StillCaptureItem(HAL_PIXEL_FORMAT_YCbCr_420_888, QVGA) };
        TestSnapshotGeneric(previews, 2, snapshots, 3, "PrivPrevFHDYUVPrevFHDJpegSnapMP4YUVSnapMP4YUVSnapQVGA", HalJsonParser::sFrames);
    }*/

    NATIVETEST_TEST(StillCaptureTest, TestHVXPreview)
    {
        NT_LOG_WARN("This test requires \"enableHVXStreaming=1\" in camxoverridesettings.txt");
        TestSingleCameraCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, HD, "TestHVXPreview", HalJsonParser::sFrames, true);
    }

    /* Depth camera tests */
    NATIVETEST_TEST(StillCaptureTest, TestDepth16)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_Y16, DEPTH_RES), "TestDepth16", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepth16Secure)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_Y16, DEPTH_RES), "TestDepth16Secure", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthRaw)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_RAW16, DEPTH_RES), "TestDepthRaw", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthRawSecure)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_RAW16, DEPTH_RES), "TestDepthRawSecure", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthBlob)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_BLOB, DEPTH_RES), "TestDepthBlob", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthBlobSecure)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_BLOB, DEPTH_RES), "TestDepthBlobSecure", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthPrivate)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, DEPTH_RES), "TestDepthPrivate", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthPrivateSecure)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, DEPTH_RES), "TestDepthPrivateSecure", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(StillCaptureTest, TestDepthLensIntrinsics)
    {
        TestDepthCamera(StillCaptureItem(HAL_PIXEL_FORMAT_Y16, DEPTH_RES), "TestDepthLensIntrinsics", HalJsonParser::sFrames, false, true);
    }


    NATIVETEST_TEST(PreviewTest, PrivPrevVGAYUVVideoVGAYUVSnapVGA)
    {
        TestPreviewWithDualCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, VGA, "PrivPrevVGAYUVVideoVGAYUVSnapVGA", HalJsonParser::sSnaps, true);
    }

    NATIVETEST_TEST(PreviewTest, PrivPrevVGAYUVVideoVGAYUVSnapHD)
    {
        TestPreviewWithDualCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD, "PrivPrevVGAYUVVideoVGAYUVSnapHD", HalJsonParser::sSnaps, true);
    }

    NATIVETEST_TEST(PreviewTest, PrivPrevVGAYUVVideoVGAYUVSnapFHD)
    {
        TestPreviewWithDualCapture(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, FULLHD, "PrivPrevVGAYUVVideoVGAYUVSnapFHD", HalJsonParser::sSnaps, true);
    }

    NATIVETEST_TEST(PreviewTest, TestBasicPreview) { TestBasicPreview(); }
    NATIVETEST_TEST(PreviewTest, TestPreviewWithMaxSnapshot) { TestPreviewWithMaxSnapshot(); }

    NATIVETEST_TEST(CaptureRequestTest, TestAfModes) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAfModes); }
    NATIVETEST_TEST(CaptureRequestTest, TestAeModeAndLock) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAeModeAndLock); }
    NATIVETEST_TEST(CaptureRequestTest, TestAwbModeAndLock) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAwbModeAndLock); }
    NATIVETEST_TEST(CaptureRequestTest, TestLensShadingMap) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestLensShadingMap); }
    NATIVETEST_TEST(CaptureRequestTest, TestEffectModes) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestEffectModes); }
    NATIVETEST_TEST(CaptureRequestTest, TestSceneModes) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestSceneModes); }
    NATIVETEST_TEST(CaptureRequestTest, TestAntiBandingModes) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAntiBandingModes); }
    NATIVETEST_TEST(CaptureRequestTest, TestFlashControl) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestFlashControl); }
    NATIVETEST_TEST(CaptureRequestTest, TestToneMapControl) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestToneMapControl); }
    NATIVETEST_TEST(CaptureRequestTest, TestEdgeModes) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestEdgeModes); }
    NATIVETEST_TEST(CaptureRequestTest, TestFocusDistanceControl) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestFocusDistanceControl); }
    NATIVETEST_TEST(CaptureRequestTest, TestZoom) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestZoom); }
    NATIVETEST_TEST(CaptureRequestTest, TestNoiseReductionModeControl) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestNoiseReductionModeControl); }
    NATIVETEST_TEST(CaptureRequestTest, TestCameraStabilization) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestCameraStabilization); }
    NATIVETEST_TEST(CaptureRequestTest, TestFaceDetection) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestFaceDetection); }
    NATIVETEST_TEST(CaptureRequestTest, TestColorCorrectionControl) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestColorCorrectionControl); }
    NATIVETEST_TEST(CaptureRequestTest, TestDynamicBlackWhiteLevel) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestDynamicBlackWhiteLevel); }
    NATIVETEST_TEST(CaptureRequestTest, Test3ARegions) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::Test3ARegions); }
    NATIVETEST_TEST(CaptureRequestTest, TestAeCompensation) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAeCompensation); }
    NATIVETEST_TEST(CaptureRequestTest, TestAePrecaptureTrigger) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAePrecaptureTrigger); }
    NATIVETEST_TEST(CaptureRequestTest, TestAfTrigger) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestAfTrigger); }
    NATIVETEST_TEST(CaptureRequestTest, TestEnableZSL) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestEnableZSL); }
    NATIVETEST_TEST(CaptureRequestTest, TestCaptureIntent) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestCaptureIntent); }
    NATIVETEST_TEST(CaptureRequestTest, TestControlMode) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestControlMode); }
    NATIVETEST_TEST(CaptureRequestTest, TestPostRawSensitivityBoost) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestPostRawSensitivityBoost); }
    NATIVETEST_TEST(CaptureRequestTest, TestTestPattern) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestTestPattern); }
    NATIVETEST_TEST(CaptureRequestTest, TestColorCorrectionAberration) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestColorCorrectionAberration); }
    NATIVETEST_TEST(CaptureRequestTest, TestValidateEnableZSL) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestValidateEnableZSL); }
    NATIVETEST_TEST(CaptureRequestTest, TestHDR) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestHDR); }
    NATIVETEST_TEST(CaptureRequestTest, TestMFNR) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestMFNR); }
    NATIVETEST_TEST(CaptureRequestTest, TestHDRWithMFNR) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestHDRWithMFNR); }
    NATIVETEST_TEST(CaptureRequestTest, TestHDRWithFlash) { TestExecute(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, true, &CaptureRequestTest::TestHDRWithFlash); }

    NATIVETEST_TEST(RobustnessTest, TestBadSurfaceDimension) { TestBadSurfaceDimension(); }
    NATIVETEST_TEST(RobustnessTest, TestBasicTriggerSequence) { TestBasicTriggerSequence(); }
    NATIVETEST_TEST(RobustnessTest, TestSimultaneousTriggers) { TestSimultaneousTriggers(); }
    NATIVETEST_TEST(RobustnessTest, TestResolutionSweep) { TestResolutionSweep(); }

    NATIVETEST_TEST(VendorTagsTest, TestSaturation) { TestSaturation(HAL_PIXEL_FORMAT_YCbCr_420_888, HD); }
    NATIVETEST_TEST(VendorTagsTest, TestExposurePriority) { TestExposurePriority(HAL_PIXEL_FORMAT_YCbCr_420_888, HD); }
    NATIVETEST_TEST(VendorTagsTest, TestIsoPriority) { TestIsoPriority(HAL_PIXEL_FORMAT_YCbCr_420_888, HD); }
    /*NATIVETEST_TEST(VendorTagsTest, DISABLED_TestOverrideTagId) { TestOverrideTagId(); }
    NATIVETEST_TEST(VendorTagsTest, DISABLED_TestOverrideAppRead) { TestOverrideTagAppRead(); }
    NATIVETEST_TEST(VendorTagsTest, DISABLED_TestOverrideReadWrite) { TestOverrideTagReadWrite(); }
    NATIVETEST_TEST(VendorTagsTest, DISABLED_TestOverrideTagType) { TestOverrideTagType(); }
    NATIVETEST_TEST(VendorTagsTest, DISABLED_TestOverrideTagCount) { TestOverrideTagCount(); }*/
    NATIVETEST_TEST(VendorTagsTest, TestEISV2)
    {
        VendorTagItem preview = VendorTagItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD);
        VendorTagItem video = VendorTagItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD);
        VendorTagItem snapshot = VendorTagItem(HAL_PIXEL_FORMAT_BLOB, UHD);
        TestEIS(preview, video, snapshot, "EISV2", HalJsonParser::sFrames, true);
    }
    NATIVETEST_TEST(VendorTagsTest, TestEISV3)
    {
        VendorTagItem preview = VendorTagItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD);
        VendorTagItem video = VendorTagItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD);
        VendorTagItem snapshot = VendorTagItem(HAL_PIXEL_FORMAT_BLOB, UHD);
        TestEIS(preview, video, snapshot, "EISV3", HalJsonParser::sFrames, true);
    }
    NATIVETEST_TEST(VendorTagsTest, TestEISV2NoVideo)
    {
        VendorTagItem preview = VendorTagItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD);
        VendorTagItem video = VendorTagItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD);
        VendorTagItem snapshot = VendorTagItem(HAL_PIXEL_FORMAT_BLOB, UHD);
        TestEIS(preview, video, snapshot, "EISV2NoVideo", HalJsonParser::sFrames, false);
    }

    NATIVETEST_TEST(RecordingTest, TestqVGAVideoRecording)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, QVGA, HalJsonParser::sFrames, FPS_30, "PrivPrevVGAYUVVideoQVGA");
    }
    NATIVETEST_TEST(RecordingTest, TestVGAVideoRecording)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, VGA, HalJsonParser::sFrames, FPS_30, "PrivPrevVGAYUVVideoVGA");
    }
    NATIVETEST_TEST(RecordingTest, TestHDVideoRecording)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD, HalJsonParser::sFrames, FPS_30, "PrivPrevVGAYUVVideoHD");
    }
    NATIVETEST_TEST(RecordingTest, TestFHDVideoRecording)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, FULLHD, HalJsonParser::sFrames, FPS_30, "PrivPrevVGAYUVVideoFHD");
    }
    NATIVETEST_TEST(RecordingTest, TestUHDVideoRecording)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, UHD, HalJsonParser::sFrames, FPS_30, "PrivPrevVGAYUVVideoUHD");
    }
    NATIVETEST_TEST(RecordingTest, Test4KVideoRecording)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD4K, HalJsonParser::sFrames, FPS_30, "PrivPrevVGAYUVVideo4K");
    }
    NATIVETEST_TEST(RecordingTest, TestHDVideoRecording60fps)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD, HalJsonParser::sFrames, FPS_60, "PrivPrevVGAYUVVideoHD60fps");
    }
    NATIVETEST_TEST(RecordingTest, Test4KVideoRecording60fps)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, UHD, HalJsonParser::sFrames, FPS_60, "PrivPrevVGAYUVVideoUHD60fps");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDPrivVideoFHD)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD, FULLHD, HalJsonParser::sFrames, FPS_30, "PrivPrevFHDPrivVideoFHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDPrivVideoUHD)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD, UHD, HalJsonParser::sFrames, FPS_30, "PrivPrevFHDPrivVideoUHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDYUVVideoFHD)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, FULLHD, FULLHD, HalJsonParser::sFrames, FPS_30, "PrivPrevFHDYUVVideoFHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDYUVVideoUHD)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, FULLHD, UHD, HalJsonParser::sFrames, FPS_30, "PrivPrevFHDYUVVideoUHD");
    }
    NATIVETEST_TEST(RecordingTest, YUVPrevFHDYUVVideoUHD)
    {
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, FULLHD, UHD, HalJsonParser::sFrames, FPS_30, "YUVPrevFHDYUVVideoUHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDPrivVideoUHDJpegSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_BLOB, FULLHD, UHD, UHD, HalJsonParser::sFrames, "PrivPrevFHDPrivVideoUHDJpegSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDPrivVideoFHDJpegSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_BLOB, FULLHD, FULLHD, UHD, HalJsonParser::sFrames, "PrivPrevFHDPrivVideoFHDJpegSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDYUVVideoUHDJpegSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, FULLHD, UHD, UHD, HalJsonParser::sFrames, "PrivPrevFHDYUVVideoUHDJpegSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, YUVPrevFHDYUVVideoFHDJpegSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, FULLHD, FULLHD, UHD, HalJsonParser::sFrames, "YUVPrevFHDYUVVideoFHDJpegSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, YUVPrevVGAPrivVideoFHDYUVSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, FULLHD, UHD, HalJsonParser::sFrames, "YUVPrevVGAPrivVideoFHDYUVSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, YUVPrevVGAYUVVideoFHDYUVSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, FULLHD, UHD, HalJsonParser::sFrames, "YUVPrevVGAYUVVideoFHDYUVSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDPrivVideoFHDRawSnapMax)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_RAW16, FULLHD, FULLHD, RDI4608_2592, HalJsonParser::sFrames, "PrivPrevFHDPrivVideoFHDRawSnapMax");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDYUVVideoFHDJpegSnapUHD)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_BLOB, FULLHD, FULLHD, UHD, HalJsonParser::sFrames, "PrivPrevFHDYUVVideoFHDJpegSnapUHD");
    }
    NATIVETEST_TEST(RecordingTest, PrivPrevFHDYUVVideoFHDRawSnapMax)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_RAW16, FULLHD, FULLHD, RDI4608_2592, HalJsonParser::sFrames, "PrivPrevFHDYUVVideoFHDRawSnapMax");
    }
    NATIVETEST_TEST(RecordingTest, YUVPrevFHDYUVVideoFHDRawSnapMax)
    {
        TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_RAW16, FULLHD, FULLHD, RDI4608_2592, HalJsonParser::sFrames, "YUVPrevFHDYUVVideoFHDRawSnapMax");
    }

    NATIVETEST_TEST(RecordingTest, TestVideoStabilization) { TestVideoStabilization(); }
    NATIVETEST_TEST(RecordingTest, TestVideoZoom) { TestVideoZoom(); }
    NATIVETEST_TEST(RecordingTest, TestVideoLiveSnapshotUHD) { TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD4K, UHD, HalJsonParser::sFrames, "PrivPrevVGAYUVVideo4KYUVSnapUHD"); }
    NATIVETEST_TEST(RecordingTest, TestVideoLiveSnapshotFHD) { TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD4K, FULLHD, HalJsonParser::sFrames, "PrivPrevVGAYUVVideo4KYUVSnapFHD"); }
    NATIVETEST_TEST(RecordingTest, TestVideoLiveSnapshotHD) { TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD4K, HD, HalJsonParser::sFrames, "PrivPrevVGAYUVVideo4KYUVSnapHD"); }
    NATIVETEST_TEST(RecordingTest, TestVideoLiveSnapshotVGA) { TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD4K, VGA, HalJsonParser::sFrames, "PrivPrevVGAYUVVideo4KYUVSnapVGA"); }
    NATIVETEST_TEST(RecordingTest, TestVideoLiveSnapshotqVGA) { TestBasicVideoLiveSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD4K, QVGA, HalJsonParser::sFrames, "PrivPrevVGAYUVVideo4KYUVSnapQVGA"); }
    NATIVETEST_TEST(ConcurrentCaptureTest, Test2ConcurrentYuvCapture) { TestConcurrentCapture(2, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HalJsonParser::sFrames, "Test2ConcurrentYUVCapture"); }
    NATIVETEST_TEST(ConcurrentCaptureTest, Test4ConcurrentRawCapture) { TestConcurrentCapture(4, HAL_PIXEL_FORMAT_RAW16, VGA, HalJsonParser::sFrames, "Test4RawCapture"); }
    /*NATIVETEST_TEST(MultiFrameTest, DISABLED_TestOpenAllCameras) { TestOpenAllCameras(Camera3SuperModule::GetCameraList(), HAL_PIXEL_FORMAT_YCbCr_420_888, VGA); }
    NATIVETEST_TEST(MultiFrameTest, DISABLED_TestOpenMultipleConcurrentCameras) { TestOpenMultipleCameras({ Camera3SuperModule::GetCameraList() }, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, 1); }
    NATIVETEST_TEST(MultiFrameTest, DISABLED_TestStabilityOpenMultipleConcurrentCameras) { TestOpenMultipleCameras({ Camera3SuperModule::GetCameraList() }, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, 5); }
    NATIVETEST_TEST(MultiFrameTest, DISABLED_TestOpenRandomConcurrentCameras) { TestOpenRandomMultipleCameras(HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, 5); }*/

    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedFULLHDRecording120FPS)
    {
        TestConstrainedHighSpeedRecording(FULLHD, 120, 120, HalJsonParser::sFrames);
    }
    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedHDRecording120FPS)
    {
        TestConstrainedHighSpeedRecording(HD, 120, 120, HalJsonParser::sFrames);
    }
    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedHDRecording240FPS)
    {
        TestConstrainedHighSpeedRecording(HD, 240, 240, HalJsonParser::sFrames);
    }
    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedHDRecording480FPS)
    {
        TestConstrainedHighSpeedRecording(HD, 480, 480, HalJsonParser::sFrames);
    }
    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedVGARecording120FPS)
    {
        TestConstrainedHighSpeedRecording(VGA, 120, 120, HalJsonParser::sFrames);
    }
    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedVGARecording240FPS)
    {
        TestConstrainedHighSpeedRecording(VGA, 240, 240, HalJsonParser::sFrames);
    }
    NATIVETEST_TEST(RecordingTest, TestConstrainedHighSpeedRecordingSweep)
    {
        TestConstrainedHighSpeedRecordingSweep(HalJsonParser::sFrames);
    }

    NATIVETEST_TEST(RecordingTest, TestHVXPreviewVideo)
    {
        NT_LOG_WARN("This test requires \"enableHVXStreaming=1\" in camxoverridesettings.txt")
        TestBasicVideoRecording(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, HD, HalJsonParser::sFrames, FPS_30, "TestHVXPreviewVideo");
    }

    /*NATIVETEST_TEST(IfeUnitTest, DISABLED_IfeTestFullFdVGA)
    {
        Size resolutions[] = { VGA, //full
            VGA, //fd
        };
        std::string ports[] = { "full","fd" };
        TestIFEOutputPorts(resolutions, HalJsonParser::sFrames, ports, 2);
    }

    NATIVETEST_TEST(IfeUnitTest, DISABLED_IfeRawTestCamifMax)
    {
        Size resolutions[] = { VGA };
        std::string ports[] = { "camif" };
        TestIFEOutputPorts(resolutions, HalJsonParser::sFrames, ports, 1);
    }

    NATIVETEST_TEST(IfeUnitTest, DISABLED_IfeRawTestLscMax)
    {
        Size resolutions[] = { VGA };
        std::string ports[] = { "lsc" };
        TestIFEOutputPorts(resolutions, HalJsonParser::sFrames, ports, 1);
    }*/

    NATIVETEST_TEST(IfeUnitTest, IfeTestRDI0OnlyMAX)
    {
        Size resolutions[] = { RDI4608_2592 }; //rdi;
        std::string ports[] = { "rdi0" };
        TestIFEOutputPorts(resolutions, HalJsonParser::sFrames, ports, 1);
    }

    /* Partial Metadata tests */
    NATIVETEST_TEST(PartialMetadataTest, TestPartialStaticSupportA) { TestPartialStaticSupport(PartialModeA); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialStaticSupportB) { TestPartialStaticSupport(PartialModeB); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialStaticSupportC) { TestPartialStaticSupport(PartialModeC); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialStaticSupportD) { TestPartialStaticSupport(PartialModeD); }
    // NATIVETEST_TEST(PartialMetadataTest, TestPartialStaticSupportE) { TestPartialStaticSupport(PartialModeE); }

    NATIVETEST_TEST(PartialMetadataTest, TestPartialOrderingB) { TestPartialOrdering(PartialModeB); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialOrderingC) { TestPartialOrdering(PartialModeC); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialOrderingD) { TestPartialOrdering(PartialModeD); }
    // NATIVETEST_TEST(PartialMetadataTest, TestPartialOrderingE) { TestPartialOrdering(PartialModeE); }

    NATIVETEST_TEST(PartialMetadataTest, TestPartialDisjointB) { TestPartialDisjoint(PartialModeB); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialDisjointC) { TestPartialDisjoint(PartialModeC); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialDisjointD) { TestPartialDisjoint(PartialModeD); }
    // NATIVETEST_TEST(PartialMetadataTest, TestPartialDisjointE) { TestPartialDisjoint(PartialModeE); }

    NATIVETEST_TEST(PartialMetadataTest, TestPartialEnableZSLB) { TestPartialEnableZSL(PartialModeB); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialEnableZSLC) { TestPartialEnableZSL(PartialModeC); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialEnableZSLD) { TestPartialEnableZSL(PartialModeD); }
    // NATIVETEST_TEST(PartialMetadataTest, TestPartialEnableZSLE) { TestPartialEnableZSL(PartialModeE); }

    NATIVETEST_TEST(PartialMetadataTest, TestPartialFlushB) { TestPartialFlush(PartialModeB); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialFlushC) { TestPartialFlush(PartialModeC); }
    NATIVETEST_TEST(PartialMetadataTest, TestPartialFlushD) { TestPartialFlush(PartialModeD); }
    // NATIVETEST_TEST(PartialMetadataTest, TestPartialFlushE) { TestPartialFlush(PartialModeE); }

    /* Flush tests */
    NATIVETEST_TEST(FlushTest, TestFlushSnapshotZSL)
    {
        TestFlushSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestFlushSnapshotZSL", HalJsonParser::sFrames, SettingZSL, CaptureSnapshot);
    }

    NATIVETEST_TEST(FlushTest, TestFlushSnapshotSWMF)
    {
        TestFlushSnapshot(HAL_PIXEL_FORMAT_YCbCr_420_888, HD, "TestFlushSnapshotSWMF", HalJsonParser::sFrames, SettingSWMF, CaptureSnapshot);
    }

    NATIVETEST_TEST(FlushTest, TestFlushSnapshotMFNR)
    {
        TestFlushSnapshot(HAL_PIXEL_FORMAT_YCbCr_420_888, HD, "TestFlushSnapshotMFNR", HalJsonParser::sFrames, SettingMNFR, CaptureSnapshot);
    }

    NATIVETEST_TEST(FlushTest, TestFlushPreviewSAT)
    {
        TestFlushSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestFlushPreviewSAT", HalJsonParser::sFrames, SettingSAT, CapturePreview);
    }

    NATIVETEST_TEST(FlushTest, TestFlushPreviewBokeh)
    {
        TestFlushSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestFlushPreviewBokeh", HalJsonParser::sFrames, SettingBokeh, CapturePreview);
    }

    NATIVETEST_TEST(FlushTest, TestFlushBurstshot)
    {
        TestFlushSnapshot(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, UHD, "TestFlushBurstshot", HalJsonParser::sFrames, SettingBurstshot, CaptureSnapshot);
    }

    // TODO : Fix issues - [a] use standard flush framework, [b] Error - buffer timeout
    /*NATIVETEST_TEST(FlushTest, TestFlushRecordingHFR120)
    {
        TestFlushRecordingHFR(HD, 120, 120, HalJsonParser::sFrames);
    }*/

    NATIVETEST_TEST(FlushTest, TestFlushRecordingEIS)
    {
        NT_LOG_WARN("This test requires \"logVerboseMask=0x200\", \"EISV3Enable=2\" and \"enableNCSService=TRUE\" in camxoverridesettings.txt");
        FlushItem preview = FlushItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD);
        FlushItem video = FlushItem(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, FULLHD);
        FlushItem snapshot = FlushItem(HAL_PIXEL_FORMAT_BLOB, FULLHD);
        TestFlushRecordingEIS(preview, video, snapshot, "EISV3", HalJsonParser::sFrames, true);
    }

    NATIVETEST_TEST(FlushTest, TestFlushRecordingHDR)
    {
        NT_LOG_WARN("This test requires \"overrideForceSensorMode=7\" and \"feature1=1\" in camxoverridesettings.txt");
        TestFlushRecordingHDR(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL_PIXEL_FORMAT_YCbCr_420_888, VGA, FULLHD, HalJsonParser::sFrames, FPS_30, "TestFlushRecordingHDR");
    }

    int RunNativeHalTest(int argc, char** argv)
    {
        // parse all command line options, print help menu if illegal option provided
        int cmdCheck = HalCmdLineParser::ParseCommandLine(argc, argv);
        // set the verbose level
        verboseSeverity eSev = static_cast<verboseSeverity>(HalCmdLineParser::GetLogLevel());
        if (-1 == NTLog.SetLogLevel(eSev))
        {
            NT_LOG_ERROR("Invalid log level %d", HalCmdLineParser::GetLogLevel());
            return 1;
        }
        else
        {
            NT_LOG_DEBUG("LOG Level set to %d", NTLog.m_eLogLevel);
        }

        #ifdef ENVIRONMENT64
            NT_LOG_INFO("==== NativeHalTest 5.0 (64 bit) =====");
        #else
            NT_LOG_INFO("==== NativeHalTest 5.0 (32 bit) =====");
        #endif

        switch(cmdCheck)
        {
            case 0:
            {
                cmdCheck = catchRunErrorSignal(&RunTests);
                ShutDown();
                break;
            }
            case 1:
                cmdCheck = HalJsonParser::jsonParsing(HalCmdLineParser::m_testcaseOrJson);
                break;
            case 2:
                NT_LOG_ERROR("\"--gtest_filter\" cannot be used along with \"--test\". Mutually exclusive !");
                HalCmdLineParser::PrintCommandLineUsage();
                return 1;
                break;
            case -1:
            default:
                HalCmdLineParser::PrintCommandLineUsage();
                cmdCheck = 1;
                break;
        }

        return cmdCheck;
    }

}  // namespace haltests

    int main(int argc, char** argv)
    {
        return haltests::RunNativeHalTest(argc, argv);
    }

