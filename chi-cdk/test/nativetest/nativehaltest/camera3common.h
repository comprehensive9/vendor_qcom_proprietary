//******************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA3_COMMON__
#define __CAMERA3_COMMON__

#include <nativetestlog.h>
#include <fcntl.h>
#include <memory>
#include "camxcdktypes.h"
#include <hardware/camera3.h>
#include <hardware/gralloc1.h>
#include <system/graphics.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#define NOMINMAX
#undef min
#undef max
#endif // WIN32/WIN64

#ifdef _LINUX
#undef LOG_INFO
#include <ui/GraphicBuffer.h>
#include <camera_metadata_hidden.h>
#define SAFE_NEW() new
using namespace android;
#else
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <csetjmp>
#include <csignal>
#ifdef _DEBUG
#define SAFE_NEW() new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define SAFE_NEW() new
#endif //_DEBUG
#endif //_LINUX

// Determine the compiler's environment
#if ((ULONG_MAX) == (UINT_MAX))
#define ENVIRONMENT32
#else
#define ENVIRONMENT64
#endif

extern "C" {
    typedef void (callbacks_process_capture_result_t)(
        const struct camera3_callback_ops *,
        const camera3_capture_result_t *);

    typedef void (callbacks_notify_t)(
        const struct camera3_callback_ops *,
        const camera3_notify_msg_t *);
}

namespace haltests {

    // To deal with padding added by driver
#define PAD_64(x) ((x + 63) & 0xFFFFFFC0)
#define NATIVETEST_UNUSED_PARAM(expr) (void)(expr)
#define NATIVETEST_ELEMENTS(arr) (sizeof(arr) / sizeof(arr[0]))

    static const uint16_t   kVersion3_0 = HARDWARE_MODULE_API_VERSION(3, 0);
    static const char* versionInfo = "Version 5.0";

    static const std::string sOverrideSettings = "camxoverridesettings.txt";
#ifdef _LINUX
    static const std::string sImageUpperPath = "/data/vendor/camera/";
    static const std::string sImageRootPath = "/data/vendor/camera/nativehaltest/";
#else
    static const std::string sImageRootPath = "./nativehaltest/";
#endif
    struct Size
    {
        uint32_t width;
        uint32_t height;
        Size(uint32_t w = 0, uint32_t h = 0) : width(w), height(h) {}

    };

    struct Sizef
    {
        float width;
        float height;
        Sizef(float w = 0, float h = 0) : width(w), height(h) {}
    };

    /**
    * Holds properties of an input or output to be used in a reprocess test case.
    */
    struct StreamItem
    {
        int format;      // Format of the stream
        Size resolution; // Resolution of the stream
    protected:
        StreamItem(int f, Size r) : format(f), resolution(r) {}
    };

    const int NOT_APPLICABLE = -1;

    const int NUM_IMAGES_PRESIL = 5;
    const int NUM_IMAGES_DEVICE = 30;
    const int FLUSH_DELAY_US    = 0;  // flush delay in microseconds
    const int FPS_30  = 30;
    const int FPS_60  = 60;
    const int FPS_120 = 120;


    const Size RAW5344_4016 = Size(5344, 4016);
    const Size RAW4208_3120 = Size(4208, 3120);
    const Size RDI4608_2592 = Size(4608, 2592);
    const Size Size5488_4112 = Size(5488, 4112);
    const Size Size5184_3880 = Size(5184, 3880);
    const Size Size4608_3456 = Size(4608, 3456);
    const Size FULLHD = Size(1920, 1080);
    const Size FULLHD_ALT = Size(1920, 1088);
    const Size FULLHD_43 = Size(1920, 1440);
    const Size HD4K = Size(4096, 2160);
    const Size UHD = Size(3840, 2160);
    const Size HD = Size(1280, 720);
    const Size PHD = Size(720, 1280);
    const Size VGA = Size(640, 480);
    const Size QVGA = Size(320, 240);
    const Size PQVGA = Size(240, 320);
    const Size MP21 = Size(5344, 4008);
    const Size MP16_Wide = Size(5312, 2988);
    const Size MP13 = Size(4160, 3120);
    const Size MP12 = Size(4032, 3024);
    const Size MP12_2 = Size(4000, 3000);
    const Size MP8 = Size(3200, 2400);
    const Size MP8_Wide = Size(3264, 2448);
    const Size SQUARE = Size(2976, 2976);
    const Size Size160_120 = Size(160, 120);
    const Size Size40_30 = Size(40, 30);
    const Size MP5 = Size(2592, 1944);
    const Size MP4 = Size(2688, 1512);
    const Size MP3 = Size(2048, 1536);
    const Size MP2 = Size(1600, 1200);
    const Size MP1 = Size(1280, 768);
    const Size MP1_3 = Size(1280, 960);
    const Size WXGA1 = Size(1024, 768);
    const Size WXGA2 = Size(1024, 738);
    const Size WVGA = Size(800, 600);
    const Size SVGA = Size(800, 480);
    const Size Size720_480 = Size(720, 480);
    const Size FWVGA = Size(864, 480);
    const Size Size640_360 = Size(640, 360);
    const Size PVGA = Size(480, 640);
    const Size CIF = Size(352, 288);
    const Size PCIF = Size(288, 352);
    const Size NQCIF = Size(176, 144);
    const Size PQCIF = Size(144, 176);
    const Size Size2744_2056 = Size(2744, 2056);
    const Size Size2744_1544 = Size(2744, 1544);
    const Size Size2100_1560 = Size(2100, 1560);
    const Size DEPTH_RES = Size(224, 1557);
    const Size Blob_458752_1 = Size(458752, 1);
    const Size MAX_RES = Size(UINT_MAX, UINT_MAX);

    std::string ConvertResToString(Size resolution);

    std::string ConstructOutputFileName(int cameraId,
        int format,
        int frameIndex,
        const char* testName);

    std::string ConvertFormatToString(int format);

    char * ConvertToLowerCase(char *charArray);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ConstructOutputFileName
    ///
    /// @brief  Helper function to construct filename for an image
    ///
    /// @param  streamIndex    stream index
    /// @param  format         format of the stream
    /// @param  usageFlag      usageFlag of the stream
    /// @param  dataspace      dataspace of the stream
    /// @param  resolution     resolution of the stream
    /// @param  testName       test name to be prefixed
    ///
    /// @return string
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string ConstructOutputFileName(int streamIndex, int format, int usageFlag, int dataspace,
        Size resolution, const char* testName);

}  // namespace haltests

#endif  // __CAMERA3_COMMON__
