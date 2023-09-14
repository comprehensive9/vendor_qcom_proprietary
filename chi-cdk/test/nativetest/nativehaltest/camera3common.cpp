 //*************************************************************************************************
 // Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
 // All Rights Reserved.
 // Confidential and Proprietary - Qualcomm Technologies, Inc.
 //*************************************************************************************************
#include "camera3common.h"

namespace haltests {

    /***************************************************************************************************
    *   ConvertResToString
    *
    *   @brief
    *       Helper function to convert resolution as a string which is used to append to
    *       the filename.
    *
    *   @return
    *       resolution as string
    ****************************************************************************************************/

    std::string ConvertResToString(
        Size resolution)  // [in] resolution to be converted as string
    {
        std::ostringstream resolutionName;
        resolutionName << "Res"<< resolution.width << "x" << resolution.height;
        return resolutionName.str();
    }

    /***************************************************************************************************
    *   ConstructOutputFileName
    *
    *   @brief
    *       Helper function to construct filname for an image
    *
    *   @return
    *       final filename
    ****************************************************************************************************/
    std::string ConstructOutputFileName(
        int cameraId,           //[in] camera id to be appended
        int format,             //[in] format to determine file extension
        int frameIndex,         //[in] frame index to be appended
        const char * testName)  //[in] test name to be prefixed
    {
        std::ostringstream outputName;

        if (frameIndex == -1)
        {
            // frameIndex is not needed for this filename
            outputName << sImageRootPath << testName << "_Cam" << cameraId;
        }
        else
        {
            outputName << sImageRootPath << testName << "_Cam" << cameraId << "_Frame" << frameIndex;
        }

        switch (format)
        {
            case HAL_PIXEL_FORMAT_BLOB:
                outputName << ".jpg";
                break;
            case HAL_PIXEL_FORMAT_RAW10:
            case HAL_PIXEL_FORMAT_RAW16:
            case HAL_PIXEL_FORMAT_RAW_OPAQUE:
                outputName << ".raw";
                break;
            case HAL_PIXEL_FORMAT_YCbCr_420_888:
            case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
            case HAL_PIXEL_FORMAT_Y8:
            case HAL_PIXEL_FORMAT_Y16:
                outputName << ".yuv";
                break;
            default:
                NT_LOG_ERROR("Unsupported capture format: %d", format);
                break;
        }

        return outputName.str();
    }

    /***************************************************************************************************
    *   ConvertFormatToString
    *
    *   @brief
    *       Helper function to convert format to a string
    *
    *   @return
    *       format as string
    ****************************************************************************************************/
    std::string ConvertFormatToString(
        int format)  //[in] format to be converted
    {
        switch(format)
        {
            case HAL_PIXEL_FORMAT_BLOB:
                return "BLOB";
            case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
                return "PRIVATE";
            case HAL_PIXEL_FORMAT_RAW10:
                return "RAW10";
            case HAL_PIXEL_FORMAT_RAW12:
                return "RAW12";
            case HAL_PIXEL_FORMAT_RAW16:
                return "RAW16";
            case HAL_PIXEL_FORMAT_RAW_OPAQUE:
                return "RAW_OPAQUE";
            case HAL_PIXEL_FORMAT_YCbCr_420_888:
                return "YUV420";
            case HAL_PIXEL_FORMAT_Y8:
                return "Y8";
            case HAL_PIXEL_FORMAT_Y16:
                return "Y16";
            default:
                 NT_LOG_ERROR("Unknown format: [%d]", format);
                return "UNKNOWN";
        }
    }

    /***************************************************************************************************
    *   ConvertToLowerCase
    *
    *   @brief
    *       Helper function to convert string to lowercase string
    *
    *   @return
    *       format as string
    ****************************************************************************************************/
    char * ConvertToLowerCase(
        char *charArray)  //[in] a character array
    {
        char *copy;
        for (copy = charArray; *copy != '\0'; ++copy)
        {
            *copy = tolower(*copy);
        }
        return charArray;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ConstructOutputFileName
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string ConstructOutputFileName(int streamIndex, int format, int usageFlag, int dataspace,
        Size resolution, const char* testName)
    {
        std::ostringstream outputName;
        outputName << testName << "_Stream" << streamIndex << "_" << ConvertFormatToString(format).c_str()
            << "_" << ConvertResToString(resolution).c_str() << "_Dataspace" << dataspace << "_UsageFlag" << usageFlag;
        return outputName.str();
    }
}
