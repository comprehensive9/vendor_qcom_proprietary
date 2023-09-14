//*************************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __CAMERA3_COMMON__
#define __CAMERA3_COMMON__


#include "nativetestlog.h"
#include <fcntl.h>
#include <memory>
#include <cinttypes>
#include <sstream>
#include <hardware/camera3.h>
#ifdef _LINUX
#undef LOG_INFO
#include <dlfcn.h>
#include <system/graphics.h>
#include <ui/GraphicBuffer.h>
#define SAFE_NEW() new
using namespace android;
#else
#include <graphics.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>

#ifdef _DEBUG
#define SAFE_NEW() new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define SAFE_NEW() new
#endif //_DEBUG
#endif //_LINUX

#define NATIVETEST_UNUSED_PARAM(expr) (void)(expr)
#define NATIVETEST_ELEMENTS(arr) (sizeof(arr) / sizeof(arr[0]))


namespace chitests {
// To deal with padding added by driver
#define PAD_64(x) ((x + 63) & 0xFFFFFFC0)

static const uint16_t   kVersion3_0 = HARDWARE_MODULE_API_VERSION(3, 0);

    // ICA Min width and Height requirements
    // Refer to camxipenode.h
    static const uint32_t   ICAMinWidthPixels = 30;
    static const uint32_t   ICAMinHeightPixels = 26;

#ifdef _LINUX
    static const std::string ImageUpperPath = "/data/vendor/camera/";
    static const std::string ImageRootPath = "/data/vendor/camera/nativechitest/";
#else
static const std::string ImageRootPath = "./nativechitest/";
static const std::string OverrideSettings = "camxoverridesettings.txt";
#endif
struct Size
{
    uint32_t width;
    uint32_t height;
    Size(uint32_t w = 0, uint32_t h = 0) : width(w), height(h) { }

        inline Size operator / (int ds) const
        {
            // using this operator only if it passes isInvalidSize check
            // CamX always ceils it, but c++ integer division always floors it
            // 1920 x 1080 DS16 = 1080/16 results in 67.5, we cannot directly do integer division
            // 1920 x 1080 DS64 = 1080/64 results in 16.875, we cannot directly do integer division
            Size dsSize = Size(1 + ((width - 1) / ds), 1 + ((height -1) / ds));

            // even ceiling
            if ((dsSize.width & 0x00000001) != 0) {
                dsSize.width = dsSize.width + 0x00000001;
            }

            // even ceiling
            if ((dsSize.height & 0x00000001) != 0) {
                dsSize.height = dsSize.height + 0x00000001;
            }

            return dsSize;
        }

        // checking for ICA limits
        inline bool isICALimitHit(int ds)
        {
            Size dsSize = Size(width, height) / ds;
            if ((dsSize.width < ICAMinWidthPixels) || (dsSize.height < ICAMinHeightPixels)) {
                return true;
            }
            return false;
        }

    inline bool operator < (Size res) const
    {
        if ((width*height) < (res.width*res.height))
        {
            return true;
        }
        return false;
    }

    inline bool operator != (Size res) const
    {
        if ((width != res.width) || (height != res.height))
        {
            return true;
        }
        return false;
    }

    inline bool isInvalidSize()
    {
        if ((width*height) == 0)
        {
            return true;
        }
        return false;
    }
};

const int FPS_30 = 30;
const int FPS_60 = 60;

const int TIMEOUT_RETRIES = 10;

    ///Resolution definitions
    const Size RAW5344_4016   = Size(5344, 4016);
    const Size RAW4208_3120   = Size(4208, 3120);
    const Size RAW_12MP       = Size(4000, 3000);
    const Size RDI4608_2592   = Size(4608, 2592);
    const Size FULLHD         = Size(1920, 1080);
    const Size FULLHD_ALT     = Size(1920, 1088);
    const Size FULLHD_1440    = Size(1920, 1440);
    const Size HD4K           = Size(4096, 2160);
    const Size HD4K_ALT       = Size(4656, 3496);
    const Size UHD            = Size(3840, 2160);
    const Size HD             = Size(1280, 720);
    const Size VGA            = Size(640, 480);
    const Size QVGA           = Size(320, 240);
    const Size MP21           = Size(5344, 4008);
    const Size Size160_120    = Size(160, 120);
    const Size Size40_30      = Size(40, 30);
    const Size Size1008_568   = Size(1008, 568);

typedef enum ChiGrallocPrivateUsageFlags
{
    PRIVATE_GRALLOC_USAGE_TP10 = 0x08000000,   ///< UBWC TP10 private usage flag.
    PRIVATE_GRALLOC_USAGE_P010 = 0x40000000,   ///< UBWC P010 private usage flag.
} CHIGRALLOCPRIVATEUSAGEFLAGS;

/***************************************************************************************************
*   ConvertResToString
*
*   @brief
*       Helper function to convert resolution as a string which is used to append to
*       the filename.
*
*   @return
*       string
****************************************************************************************************/

static inline std::string ConvertResToString(
    Size resolution)  // [in] resolution to be converted as string
{
    std::ostringstream resolutionName;
    resolutionName << "Res" << resolution.width << "x" << resolution.height;
    return resolutionName.str();
}


static inline std::string GetFileExt(int format)
{
    switch (format)
    {
        case ChiStreamFormatBlob:
            return ".jpg";
        case ChiStreamFormatRaw10:
        case ChiStreamFormatRaw16:
        case ChiStreamFormatRaw12:
        case ChiStreamFormatRawOpaque:
        case ChiStreamFormatRaw64:
            return ".raw";
        case ChiStreamFormatYCbCr420_888:
        case ChiStreamFormatYCrCb420_SP:
        case ChiStreamFormatY8:
        case ChiStreamFormatY16:
            return ".yuv";
        case ChiStreamFormatImplDefined:
        case ChiStreamFormatUBWCTP10:
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatP010:
        case ChiStreamFormatUBWCNV124R:
        case ChiStreamFormatNV12UBWCFLEX:
            return ".ubwc";
        case ChiStreamFormatNV12HEIF:
            return ".heif";
            // Unverified formats
        case ChiStreamFormatPD10:
            return ".pd";
        default:
            NT_LOG_ERROR("Unknown capture format: %d", format);
            return ".unknown";
    }
}

static inline std::string ConstructOutputFileName(int cameraId)
{

    std::ostringstream outputName;

    outputName << ImageRootPath << currentTestCaseName;

    if (cameraId == -1)
    {
        outputName << "_";
    }
    else
    {
        outputName << "_Cam" << cameraId << "_";
    }

    return outputName.str();
}

}  // namespace chitests

#endif  // __CAMERA3_COMMON__
