//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

/*
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "extended_camera_characteristics_test.h"
#include <sstream>
#include <algorithm>

namespace haltests
{

    ExtendedCameraCharacteristicsTest::ExtendedCameraCharacteristicsTest()
    {}

    ExtendedCameraCharacteristicsTest::~ExtendedCameraCharacteristicsTest()
    {}

    void ExtendedCameraCharacteristicsTest::Setup()
    {
        Camera3Device::Setup();
        ConstructCharacteristicsKeyMap();
        maxYuvSize = 0;
    }

    void ExtendedCameraCharacteristicsTest::Teardown()
    {
    }

   /**
    * Queries the available request capabilities by the given camera static info
    * Saves all the results into the actualCapabilities vector
    */
    void ExtendedCameraCharacteristicsTest::GetActualCapabilities(camera_info info)
    {
        actualCapabilities.clear();

        camera_metadata_entry_t actualCapabilitiesEntry;
        NT_ASSERT(0 == find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(
                info.static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES, &actualCapabilitiesEntry),"ANDROID_REQUEST_AVAILABLE_CAPABILITIES not found.");

        for (size_t i = 0; i < actualCapabilitiesEntry.count; ++i)
        {
            uint8_t capability = actualCapabilitiesEntry.data.u8[i];
            actualCapabilities.push_back(capability);
        }

        NT_ASSERT(!actualCapabilities.empty(),"No camera capabilities found!");
    }

    /** A CTS test to verify all the static camera keys */
    void ExtendedCameraCharacteristicsTest::TestKeys()
    {

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info,"get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0 == GetCamModule()->get_camera_info(cameraId, &info), "Can't get camera info for camera id = %d" , cameraId);

            GetActualCapabilities(info);
           /*
            * List of keys that must be present in camera characteristics (not null).
            *
            * Keys for LIMITED, FULL devices might be available despite lacking either
            * the hardware level or the capability. This is *OK*. This only lists the
            * *minimal* requirements for a key to be listed.
            *
            * LEGACY devices are a bit special since they map to api1 devices, so we know
            * for a fact most keys are going to be illegal there so they should never be
            * available.
            *
            * For LIMITED-level keys, if the level is >= LIMITED, then the capabilities are used to
            * do the actual checking.
            */
            camera_metadata_entry_t actualHwLevelEntry;
            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info.static_camera_characteristics),
                ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, &actualHwLevelEntry),"ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL not found.");

            int actualHwLevel = actualHwLevelEntry.data.u8[0];

            {
                //                        (HW Level)         (Key Name)                 (Capabilities <Var-Arg>)
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AVAILABLE_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AE_AVAILABLE_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AE_COMPENSATION_RANGE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AE_COMPENSATION_STEP, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AE_LOCK_AVAILABLE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AF_AVAILABLE_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AVAILABLE_EFFECTS, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AVAILABLE_SCENE_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AWB_AVAILABLE_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_AWB_LOCK_AVAILABLE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_MAX_REGIONS, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_EDGE_AVAILABLE_EDGE_MODES, FULL, NONE);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_FLASH_INFO_AVAILABLE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_JPEG_AVAILABLE_THUMBNAIL_SIZES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_FACING, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_AVAILABLE_APERTURES, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_AVAILABLE_FILTER_DENSITIES, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_AVAILABLE_FOCAL_LENGTHS, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION, LIMITED, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION, LIMITED, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_HYPERFOCAL_DISTANCE, LIMITED, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE, LIMITED, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_REQUEST_AVAILABLE_CAPABILITIES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS, OPT, YUV_REPROCESS);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS, OPT, OPAQUE_REPROCESS);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, OPT, CONSTRAINED_HIGH_SPEED);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_REQUEST_MAX_NUM_OUTPUT_STREAMS, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_REQUEST_PARTIAL_RESULT_COUNT, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_REQUEST_PIPELINE_MAX_DEPTH, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SCALER_CROPPING_TYPE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_AVAILABLE_TEST_PATTERN_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_BLACK_LEVEL_PATTERN, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_BLACK_LEVEL_PATTERN, FULL, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_CALIBRATION_TRANSFORM1, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_COLOR_TRANSFORM1, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_FORWARD_MATRIX1, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT, FULL, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_MAX_FRAME_DURATION, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_PHYSICAL_SIZE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_SENSITIVITY_RANGE, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_WHITE_LEVEL, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_MAX_ANALOG_SENSITIVITY, FULL, MANUAL_SENSOR);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_ORIENTATION, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_REFERENCE_ILLUMINANT1, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SHADING_AVAILABLE_MODES, LIMITED, MANUAL_POSTPROC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SHADING_AVAILABLE_MODES, LIMITED, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_STATISTICS_INFO_AVAILABLE_HOT_PIXEL_MAP_MODES, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES, LIMITED, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_STATISTICS_INFO_MAX_FACE_COUNT, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SYNC_MAX_LATENCY, OPT, BC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES, FULL, MANUAL_POSTPROC);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_TONEMAP_MAX_CURVE_POINTS, FULL, MANUAL_POSTPROC);
            }

            // Only check for these if the second reference illuminant is included
            camera_metadata_entry_t rererenceIlluminant2;

            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SENSOR_REFERENCE_ILLUMINANT2,
                &rererenceIlluminant2))
            {
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_REFERENCE_ILLUMINANT2, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_COLOR_TRANSFORM2, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_CALIBRATION_TRANSFORM2, OPT, RAW);
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_SENSOR_FORWARD_MATRIX2, OPT, RAW);

            }

            // Required key if any of RAW format output is supported
            bool supportAnyRaw = false;
            camera_metadata_entry_t streamConfigs;
            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
                &streamConfigs), "ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS not found");

            for (size_t i = 0; i < streamConfigs.count; i += 4)
            {
                int32_t format = streamConfigs.data.i32[i];
                if (format == HAL_PIXEL_FORMAT_RAW16 ||
                    format == HAL_PIXEL_FORMAT_RAW10 ||
                    format == HAL_PIXEL_FORMAT_RAW12 ||
                    format == HAL_PIXEL_FORMAT_RAW_OPAQUE)
                {

                    supportAnyRaw = true;
                    break;
                }
            }
            if (supportAnyRaw) // only for HAL3.4, may need to check HAL version later
            {
#ifdef _LINUX
                ExpectKeyAvailable(info, actualHwLevel, ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST_RANGE, OPT, BC);
#endif //_LINUX
            }

        }
    }

    /** Gets all the available keys from query of ANDROID_REQUEST_AVAILABLE_RESULT_KEYS */
    void ExtendedCameraCharacteristicsTest::GetCaptureResultKeys(camera_info info)
    {
        availableResultKeys.clear();

        camera_metadata_entry_t entry;
        NT_ASSERT(0== find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_REQUEST_KEYS,
            &entry),"ANDROID_REQUEST_AVAILABLE_REQUEST_KEYS not found");

        for (size_t i = 0; i < entry.count; i++)
        {
            availableResultKeys.push_back(entry.data.i32[i]);
        }
        NT_ASSERT(!availableResultKeys.empty(), "No available capture result key found!");
    }


   /**
    * Sanity check of optical black regions.
    */
    void ExtendedCameraCharacteristicsTest::TestOpticalBlackRegions()
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info, "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0== GetCamModule()->get_camera_info(cameraId, &info), "Can't get camera info for camera id = %d",cameraId);

            GetCaptureResultKeys(info);
#ifdef _LINUX
            bool hasDynamicBlackLevel = (std::find(availableResultKeys.begin(), availableResultKeys.end(),
                ANDROID_SENSOR_DYNAMIC_BLACK_LEVEL) != availableResultKeys.end());

            bool hasDynamicWhiteLevel = (std::find(availableResultKeys.begin(), availableResultKeys.end(),
                ANDROID_SENSOR_DYNAMIC_WHITE_LEVEL) != availableResultKeys.end());
#else
            bool hasDynamicBlackLevel = false;
            bool hasDynamicWhiteLevel = false;
#endif //_LINUX

            camera_metadata_entry_t entry;

            bool hasFixedBlackLevel = false;
            bool hasFixedWhiteLevel = false;
            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SENSOR_BLACK_LEVEL_PATTERN, &entry))
            {
                hasFixedBlackLevel = true;
            }
            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SENSOR_INFO_WHITE_LEVEL, &entry))
            {
                hasFixedWhiteLevel = true;
            }

            // The black and white levels should be either all supported or none of them is
            // supported.
            NT_ASSERT(hasDynamicWhiteLevel == hasDynamicBlackLevel
                   , "Dynamic black and white level should be all or none of them be supported");
            NT_ASSERT(hasFixedBlackLevel == hasFixedWhiteLevel
                   , "Fixed black and white level should be all or none of them be supported");
            NT_ASSERT(!hasDynamicBlackLevel || hasFixedBlackLevel
                   , "Fixed black level should be supported if dynamic black level is supported");

#ifdef _LINUX
            std::vector<Region> blackRegions;

            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SENSOR_OPTICAL_BLACK_REGIONS, &entry))
            {
                blackRegions.clear();
                for (size_t j = 0; j < entry.count; j += 4)
                {
                    uint32_t left   = entry.data.i32[j];
                    uint32_t top    = entry.data.i32[j + 1];
                    uint32_t width  = entry.data.i32[j + 2];
                    uint32_t height = entry.data.i32[j + 3];

                    NT_ASSERT(left >= 0, "Optical black region left must be >=0!");
                    NT_ASSERT(top  >= 0, "Optical black region top must be >=0!");
                    NT_ASSERT(left % 2 == 0 && top % 2 == 0 && width % 2 == 0 && height % 2 == 0
                       , "Optical black region left/right/width/height must be even number, otherwise, the bayer CFA pattern in this region will be messed up!");

                    blackRegions.push_back(Region(left,top,width,height));
                }
                NT_ASSERT(!blackRegions.empty(), "Optical back region arrays must not be empty!");
            }
#endif //_LINUX
        }



    }

    /**
    * Test that the available stream configurations contain a few required formats and sizes.
    */
    void ExtendedCameraCharacteristicsTest::TestAvailableStreamConfigs()
    {

        bool supportYUV = false;
        bool supportJPEG = false;
        bool supportPrivate = false;

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info
                   , "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0== GetCamModule()->get_camera_info(cameraId, &info)
                   , "Can't get camera info for camera id =   %d ", cameraId);

            camera_metadata_entry_t actualHwLevelEntry;
            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(
                    info.static_camera_characteristics),
                ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, &actualHwLevelEntry)
                   , "ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL not found.");

            //Find the actual HW level
            int actualHwLevel = actualHwLevelEntry.data.u8[0];

            // Find the actual capbilities
            GetActualCapabilities(info);

            // Check required formats exist (JPEG, and YUV_420_888).
            if (std::find(actualCapabilities.begin(), actualCapabilities.end(), ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE)
                == actualCapabilities.end())
            {
                NT_LOG_UNSUPP("BACKWARD_COMPATIBLE capability not supported for camera: %d, skipping test", cameraId);
                continue;
            }

            yuvSizes.clear();
            jpegSizes.clear();
            privateSizes.clear();
            supportYUV = IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_YCbCr_420_888, yuvSizes);
            supportJPEG = IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_BLOB, jpegSizes);
            supportPrivate = IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, privateSizes);

            NT_ASSERT(supportYUV, "No valid YUV_420_888 preview formats found for camera Id: %d " , cameraId);
            NT_ASSERT(supportJPEG, "No JPEG image format found for camera Id:%d " , cameraId);
            NT_ASSERT(!yuvSizes.empty(), "No sizes for preview format YUV_420_888 camera Id: %d " , cameraId);

            camera_metadata_entry_t activeRectEntry;
            NT_ASSERT(0==find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
                &activeRectEntry)
                   , "No ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE found");

            Size activeRectSize = Size(activeRectEntry.data.i32[0], activeRectEntry.data.i32[1]);

            VerifyRectSize(activeRectSize, cameraId, jpegSizes);
            //TODO: video size cases, get video max size from HAL?

            // Check all sizes other than FullHD
            if (actualHwLevel >= LEVEL_3 || actualHwLevel == FULL || actualHwLevel == LIMITED)
            {
                NT_ASSERT(ContainSizes(yuvSizes, jpegSizes), "JEPG size not found in YUV sizes!");
            }


        }

    }


    /** Checks if a given HAL format is supported in the stream config, and added the supported size into a vector */
    bool ExtendedCameraCharacteristicsTest::IsFormatSupportedInConfig(
        camera_info info,
        int reqFormat,
        std::vector<Size> &sizeVector)
    {
        camera_metadata_entry_t streamConfigs;
        if (0 != find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
            &streamConfigs))
        {
            NT_LOG_ERROR(stringify(ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS) "map not found");
            return false;
        }

        bool isSupported = false;
        for (size_t j = 0; j < streamConfigs.count; j += 4)
        {
            int32_t format = streamConfigs.data.i32[j];
            int32_t w;
            int32_t h;
            if (format == reqFormat)
            {
                w = streamConfigs.data.i32[j + 1];
                h = streamConfigs.data.i32[j + 2];
                //enable for debugging
                //LogMsg(eDebug, TESTCASE, "Width is: %d, Height is: %d", w, h);
                if (w <= 0 || h <= 0)
                {
                    NT_LOG_ERROR("Width and Height must be positive!");
                    isSupported = false;
                }
                sizeVector.push_back(Size(w, h));
                isSupported = true;
            }
        }
        return isSupported;
    }

    bool ExtendedCameraCharacteristicsTest::IsFormatSupportedInConfig(camera_info info,int reqFormat)
    {
        camera_metadata_entry_t streamConfigs;
        if (0 != find_camera_metadata_entry(
            const_cast<camera_metadata_t*>(info.static_camera_characteristics),
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
            &streamConfigs))
        {
            NT_LOG_ERROR(stringify(ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS) "map not found");
            return false;
        }

        bool isSupported = false;
        for (size_t j = 0; j < streamConfigs.count; j += 4)
        {
            int32_t format = streamConfigs.data.i32[j];
            int32_t w;
            int32_t h;
            if (format == reqFormat)
            {
                w = streamConfigs.data.i32[j + 1];
                h = streamConfigs.data.i32[j + 2];
                isSupported = true;
                if (reqFormat == HAL_PIXEL_FORMAT_YCbCr_420_888)
                {

                    if (w * h > maxYuvSize)
                    {
                        maxYuvSize = w * h;
                    }
                }
            }
        }
        return isSupported;
    }

   /**
    * Checks if size vector 1 contains all the sizes from size vector 2
    */
   bool ExtendedCameraCharacteristicsTest::ContainSizes(std::vector<Size> sizeVec1, std::vector<Size> sizeVec2)
   {
       if (sizeVec1.empty() || sizeVec2.empty())
       {
           NT_LOG_ERROR("One of the size vector is empty!");
           return false;
       }

       bool isFound = false;

       std::vector<Size>::iterator it1 = sizeVec1.begin();
       std::vector<Size>::iterator it2 = sizeVec2.begin();
       for (; it2 != sizeVec2.end(); it2++)
       {
           for (; it1 != sizeVec1.end(); it1++)
           {
               if (it2->width == it1->width && it2->height == it1->height)
               {
                   isFound = true;
                   break;
               }
           }
       }

       return isFound;
   }

  /**
   * Verifies that if the required resolution sizes FULLHD, FULLHD_ALT, VGA and QVGA are supported
   */
    void ExtendedCameraCharacteristicsTest::VerifyRectSize(
        Size activeRectSize,
        int cameraId,
        std::vector<Size> sizesOfFormat)
    {
        std::vector<Size>::iterator it = sizesOfFormat.begin();

        bool findFULLHD = false;
        bool findFULLHD_ALT = false;
        bool findVGA = false;
        bool findQVGA = false;

        if (activeRectSize.width >= FULLHD.width && activeRectSize.height >= FULLHD.height)
        {
            for (; it != sizesOfFormat.end(); it++)
            {
                if (it->width == FULLHD.width && it->height == FULLHD.height)
                {
                    findFULLHD = true;
                }
                else if (it->width == FULLHD_ALT.width && it->height == FULLHD_ALT.height)
                {
                    findFULLHD_ALT = true;
                }
            }

            NT_ASSERT(findFULLHD, "Required FULLHD size not found for camera ID =   %d ", cameraId);
            NT_ASSERT(findFULLHD_ALT, "Required FULLHD_ALT size not found for camera ID =   %d ", cameraId);
        }

        if (activeRectSize.width >= VGA.width && activeRectSize.height >= VGA.height)
        {
            for (; it != sizesOfFormat.end(); ++it)
            {
                if (it->width == VGA.width && it->height == VGA.height)
                {
                    findVGA = true;
                }
            }

            NT_ASSERT(findVGA, "Required VGA size not found for camera ID =   %d ", cameraId);
        }

        if (activeRectSize.width >= QVGA.width && activeRectSize.height >= QVGA.height)
        {
            for (; it != sizesOfFormat.end(); ++it)
            {
                if (it->width == QVGA.width && it->height == QVGA.height)
                {
                    findQVGA = true;
                }
            }

            NT_ASSERT(findQVGA, "Required QVGA size not found for camera ID =   %d ", cameraId);
        }
    }

   /**
    * Check reprocessing capabilities.
    */
    void ExtendedCameraCharacteristicsTest::TestReprocessingCharacteristics()
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info
                   , "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0== GetCamModule()->get_camera_info(cameraId, &info)
                   , "Can't get camera info for camera id =   %d ", cameraId);

            GetActualCapabilities(info);

            bool supportYUV = (std::find(actualCapabilities.begin(), actualCapabilities.end(),
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_YUV_REPROCESSING) != actualCapabilities.end());

            bool supportOpaque = (std::find(actualCapabilities.begin(), actualCapabilities.end(),
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_PRIVATE_REPROCESSING) != actualCapabilities.end());

            camera_metadata_entry_t entry;

            std::vector<uint8_t> availableEdgeModes;
            std::vector<uint8_t> availableNRModes;

            //available edge modes
            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_EDGE_AVAILABLE_EDGE_MODES, &entry), "ANDROID_EDGE_AVAILABLE_EDGE_MODES not found");

            for (size_t j = 0; j < entry.count; j++)
            {
                uint8_t edgeMode = entry.data.u8[j];
                availableEdgeModes.push_back(edgeMode);
            }

            //available noise reduction modes
            NT_ASSERT(0== find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES, &entry)
                   , "ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES not found");
            for (size_t j = 0; j < entry.count; j++)
            {
                uint8_t nrMode = entry.data.u8[j];
                availableNRModes.push_back(nrMode);
            }

            bool supportZslEdgeMode = false;
            bool supportZslNoiseReductionMode = false;
            bool supportHiQNoiseReductionMode = false;
            bool supportHiQEdgeMode = false;

            supportZslEdgeMode = IsKeySupported(availableEdgeModes, ANDROID_EDGE_MODE_ZERO_SHUTTER_LAG);
            supportZslNoiseReductionMode = IsKeySupported(availableNRModes, ANDROID_NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG);
            supportHiQNoiseReductionMode = IsKeySupported(availableNRModes, ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY);
            supportHiQEdgeMode = IsKeySupported(availableEdgeModes, ANDROID_EDGE_MODE_HIGH_QUALITY);

            if (supportYUV || supportOpaque)
            {
                NT_ASSERT(0== find_camera_metadata_entry(
                    const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                    ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS, &entry), "ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS not found");

                int32_t maxNumInputStreams = entry.data.i32[0];
                NT_ASSERT(maxNumInputStreams > 0, "Support reprocessing but max number of input stream is 0");
                NT_ASSERT(supportZslEdgeMode, "Support reprocessing but EDGE_MODE_ZERO_SHUTTER_LAG is not supported!");
                NT_ASSERT(supportZslNoiseReductionMode, "Support reprocessing but NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG is not supported!");

                // For reprocessing, if we only require OFF and ZSL mode, it will be just like jpeg
                // encoding. We implicitly require FAST to make reprocessing meaningful, which means
                // that we also require HIGH_QUALITY.
                NT_ASSERT(supportHiQEdgeMode, "Support reprocessing but EDGE_MODE_HIGH_QUALITY is not supported!");
                NT_ASSERT(supportHiQNoiseReductionMode, "Support reprocessing but NOISE_REDUCTION_MODE_HIGH_QUALITY is not supported!");

                // Verify mandatory input formats are supported
                NT_ASSERT(!supportYUV || IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_YCbCr_420_888)
                       , "YUV_420_888 input must be supported for YUV reprocessing");

                NT_ASSERT(!supportOpaque || IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED)
                       , "YUV_420_888 input must be supported for OPAQUE  reprocessing");
            }

        }
    }


    /**
    * Test values for static metadata used by the BURST capability.
    */
    void ExtendedCameraCharacteristicsTest::TestStaticBurstCharacteristics()
    {
        //float SIZE_ERROR_MARGIN = 0.03f;
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info
                   , "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0== GetCamModule()->get_camera_info(cameraId, &info)
                   , "Can't get camera info for camera id =   %d ", cameraId);

            GetActualCapabilities(info);

            // Check if the burst capability is defined
            bool haveBurstCapability = (std::find(actualCapabilities.begin(), actualCapabilities.end(),
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BURST_CAPTURE) != actualCapabilities.end());

            bool  haveBC = (std::find(actualCapabilities.begin(), actualCapabilities.end(),
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE) != actualCapabilities.end());

            if (haveBurstCapability && !haveBC)
            {
                NT_ASSERT(false, "Must have BACKWARD_COMPATIBLE capability if BURST_CAPTURE capability is defined");
            }

            if (!haveBC) continue;

            camera_metadata_entry_t entry;
            NT_ASSERT(0==find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
                &entry), "No stream configuration map found for camera id:  %d ", cameraId);

            NT_ASSERT(0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
                &entry), "No ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE found for camera id:  %d ", cameraId);

            Region sensorSize;
            for (size_t j = 0; j < entry.count; j += 4)
            {
                sensorSize.left = entry.data.i32[j];
                sensorSize.top = entry.data.i32[j + 1];
                sensorSize.width = entry.data.i32[j + 2];
                sensorSize.height = entry.data.i32[j + 3];
            }

            //Get the max YUV size
            //bool isYUVSupprted = IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_YCbCr_420_888);
            //TODO: Haven't included everything from CTS
        }

    }

    /**
    * Check depth output capability
    */
    void ExtendedCameraCharacteristicsTest::TestDepthOutputCharacteristics()
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            NT_ASSERT(nullptr != Camera3Module::GetCamModule()->get_camera_info
                   , "get_camera_info is not implemented");

            camera_info info;
            NT_ASSERT(0 == GetCamModule()->get_camera_info(cameraId, &info)
                   , "Can't get camera info for camera id =   %d ", cameraId);

            GetActualCapabilities(info); //get REQUEST_AVAILABLE_CAPABILITIES

            bool supportDepth = (std::find(actualCapabilities.begin(), actualCapabilities.end(),
                ANDROID_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT) != actualCapabilities.end());

            std::vector<Size> depth16Sizes;
            std::vector<Size> depthCloudSizes;

            depth16Sizes.clear();
            depthCloudSizes.clear();
            bool hasDepth16 = IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_Y16, depth16Sizes);
            bool hasDepthCloud = IsFormatSupportedInConfig(info, HAL_PIXEL_FORMAT_BLOB, depthCloudSizes);

            bool depthIsExclusive = false;

            camera_metadata_entry_t entry;
            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_DEPTH_DEPTH_IS_EXCLUSIVE,
                &entry))
            {
                depthIsExclusive = true;  //Found ANDROID_DEPTH_DEPTH_IS_EXCLUSIVE
            }

            std::vector<float> poseRotation;
            float normSq;
            std::vector<float> poseTranslation;
            std::vector<float> cameraIntrinsics;
            std::vector<float> radialDistortion;

            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_LENS_POSE_ROTATION, &entry))
            {
                for (size_t j = 0; j < entry.count; j++)
                {
                    poseRotation.push_back(entry.data.f[j]);
                }
            }

            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_LENS_POSE_TRANSLATION, &entry))
            {
                for (size_t j = 0; j < entry.count; j++)
                {
                    poseTranslation.push_back(entry.data.f[j]);
                }
            }

            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_LENS_INTRINSIC_CALIBRATION, &entry))
            {
                for (size_t j = 0; j < entry.count; j++)
                {
                    cameraIntrinsics.push_back(entry.data.f[j]);
                }
            }

            if (0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                ANDROID_LENS_RADIAL_DISTORTION, &entry))
            {
                for (size_t j = 0; j < entry.count; j++)
                {
                    radialDistortion.push_back(entry.data.f[j]);
                }
            }

            if (supportDepth)
            {
                NT_EXPECT(hasDepth16, "Supports DEPTH_OUTPUT but does not support DEPTH16");

                if (hasDepth16)
                {
                    NT_EXPECT(!depth16Sizes.empty(),"Supports DEPTH_OUTPUT but no sizes for DEPTH16 supported!");
                }
                if (hasDepthCloud)
                {
                    NT_EXPECT(!depthCloudSizes.empty(),"Supports DEPTH_POINT_CLOUD but but no sizes for DEPTH_POINT_CLOUD supported!");
                }

                NT_EXPECT(depthIsExclusive,"Supports DEPTH_OUTPUT but DEPTH_IS_EXCLUSIVE is not defined");

                NT_EXPECT(!poseRotation.empty() && poseRotation.size() == 4,"Supports DEPTH_OUTPUT but LENS_POSE_ROTATION not right size");

                NT_EXPECT(!poseTranslation.empty() && poseTranslation.size() == 3,"Supports DEPTH_OUTPUT but LENS_POSE_TRANSLATION not right size");

                NT_EXPECT(!cameraIntrinsics.empty() && cameraIntrinsics.size() == 5,"Supports DEPTH_OUTPUT but LENS_INTRINSIC_CALIBRATION not right size");

                NT_EXPECT(!radialDistortion.empty() && radialDistortion.size() == 6,"Supports DEPTH_OUTPUT but LENS_RADIAL_DISTORTION not right size");

                if (!poseRotation.empty() && poseRotation.size() == 4)
                {
                    normSq = poseRotation.at(0) * poseRotation.at(0) +
                        poseRotation.at(1) * poseRotation.at(1) +
                        poseRotation.at(2) * poseRotation.at(2) +
                        poseRotation.at(3) * poseRotation.at(3);
                    NT_EXPECT(0.9999f < normSq && normSq < 1.0001f,"LENS_POSE_ROTATION quarternion must be unit-length");
                }

                if (!poseTranslation.empty() && poseTranslation.size() == 3)
                {
                    normSq = poseTranslation.at(0) * poseTranslation.at(0) +
                        poseTranslation.at(1) * poseTranslation.at(1) +
                        poseTranslation.at(2) * poseTranslation.at(2);
                    NT_EXPECT(normSq < 1.f,"Pose translation is larger than 1 m");
                }

                NT_ASSERT(0 == find_camera_metadata_entry(
                    const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                    ANDROID_SENSOR_INFO_PRE_CORRECTION_ACTIVE_ARRAY_SIZE,
                    &entry),"Supports DEPTH_OUTPUT but does not have precorrection active array defined for camera id:  %d ", cameraId);

            }
            else
            {
                bool hasFields =
                    hasDepth16 && (!poseTranslation.empty()) &&
                    (!poseRotation.empty()) && (!cameraIntrinsics.empty()) &&
                    (!radialDistortion.empty()) && (depthIsExclusive);

                NT_EXPECT(!hasFields,"All necessary depth fields defined, but DEPTH_OUTPUT capability is not listed");

            }
        }

    }


    bool ExtendedCameraCharacteristicsTest::IsKeySupported(std::vector<uint8_t> keyVector, uint8_t key)
    {
        if (!keyVector.empty())
        {
            if (std::find(keyVector.begin(), keyVector.end(), key) != keyVector.end())
            {
                return true;
            }
        }
        return false;
    }

       /**
        * Check key is present in characteristics if the hardware level is at least {@code hwLevel};
        * check that the key is present if the actual capabilities are one of {@code capabilities}.
        *
        * @return
        */
    void ExtendedCameraCharacteristicsTest::ExpectKeyAvailable(
        camera_info info,
        int actualHwLevel, //HW level got from driver
        camera_metadata_tag_t tag,
        int hwLevel,      //HW level we are testing against
        int capability)   //Capability we are checking against
    {
        // For LIMITED-level targeted keys, rely on capability check, not level
        camera_metadata_entry_t entry;
        std::string tagName = ToStringCharacteristicsTag(tag);

        if ((CompareHardwareLevel(actualHwLevel, hwLevel) >= 0) && (hwLevel != LIMITED))
        {
            std::string errorMsg =") must be in characteristics for this hardware level (required minimal HW level  ";

            NT_ASSERT(0 == find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                tag, &entry), "Key (%s , %s , actual HW Level: %s %s )", tagName.c_str(), errorMsg.c_str(), ToStringHardwareLevel(hwLevel).c_str(), ToStringHardwareLevel(actualHwLevel).c_str());
        }
        else if (std::find(actualCapabilities.begin(), actualCapabilities.end(), capability)
            != actualCapabilities.end())
        {
            if (!(hwLevel == LIMITED && CompareHardwareLevel(actualHwLevel, hwLevel) < 0))
            {
                // Don't enforce LIMITED-starting keys on LEGACY level, even if cap is defined
                std::string errorMsg = ") must be in characteristics for the required capabilities.";

                NT_ASSERT(0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics), tag, &entry), "Key (%s %s " , tagName.c_str(), errorMsg.c_str());

            }
            else if (actualHwLevel == LEGACY && hwLevel != OPT)
            {
                NT_LOG_WARN("%s is not required for LEGACY devices but still appears.", tagName.c_str());
            }
        }

    }


    /**
     * Return a positive int if left > right, 0 if left==right, negative int if left < right
     */
    int ExtendedCameraCharacteristicsTest::CompareHardwareLevel(int left, int right)
    {
        return RemapHardwareLevel(left) - RemapHardwareLevel(right);

    }

    /**
     * Remap HW levels worst<->best, 0 = LEGACY, 1 = LIMITED, 2 = FULL, ..., N = LEVEL_N
     */
    int ExtendedCameraCharacteristicsTest::RemapHardwareLevel(int level)
    {
        switch (level)
        {
            case OPT:
                return MAX_INTEGER;
            case LEGACY:
                return 0; // lowest
            case LIMITED:
                return 1; // second lowest
            case FULL:
                return 2; // good
            default:
                if (level >= LEVEL_3)
                {
                    return level; // higher levels map directly
                }
        }
        NT_LOG_ERROR("Unknown HW level: %d", level);
        return -1;
    }

    std::string ExtendedCameraCharacteristicsTest::ToStringHardwareLevel(int level)
    {
        switch (level)
        {
            case LEGACY:
                return "LEGACY";
            case LIMITED:
                return "LIMITED";
            case FULL:
                return "FULL";
            default:
                if (level >= LEVEL_3)
                {
                    char outName[64];
                    std::string levelName;
                    snprintf(outName, sizeof(outName), "LEVEL_%d", level);
                    levelName = std::string(outName);
                    return levelName;
                }
                return "UNSUPPORTED";
        }

    }

    std::string ExtendedCameraCharacteristicsTest::ToStringCharacteristicsTag(camera_metadata_tag_t tag)
    {
        std::map<camera_metadata_tag_t, std::string>::iterator iter = camCharacteristicsKeys.find(tag);

        if (iter != camCharacteristicsKeys.end())
        {
            //element found;
            return(iter->second);
        }
        NT_LOG_ERROR("cannot find %s in the key map", ToStringCharacteristicsTag(tag).c_str());
        return "";
    }

    /** Creates a camera characteristics map to have mapping between name and value */
    void  ExtendedCameraCharacteristicsTest::ConstructCharacteristicsKeyMap()
    {
        camCharacteristicsKeys.clear();

        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES, "ANDROID_COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AVAILABLE_MODES, "ANDROID_CONTROL_AVAILABLE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES, "ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AE_AVAILABLE_MODES, "ANDROID_CONTROL_AE_AVAILABLE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, "ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AE_COMPENSATION_RANGE, "ANDROID_CONTROL_AE_COMPENSATION_RANGE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AE_COMPENSATION_STEP, "ANDROID_CONTROL_AE_COMPENSATION_STEP"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AE_LOCK_AVAILABLE, "ANDROID_CONTROL_AE_LOCK_AVAILABLE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AF_AVAILABLE_MODES, "ANDROID_CONTROL_AF_AVAILABLE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AVAILABLE_EFFECTS, "ANDROID_CONTROL_AVAILABLE_EFFECTS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AVAILABLE_SCENE_MODES, "ANDROID_CONTROL_AVAILABLE_SCENE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES, "ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AWB_AVAILABLE_MODES, "ANDROID_CONTROL_AWB_AVAILABLE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_AWB_LOCK_AVAILABLE, "ANDROID_CONTROL_AWB_LOCK_AVAILABLE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_MAX_REGIONS, "ANDROID_CONTROL_MAX_REGIONS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_EDGE_AVAILABLE_EDGE_MODES, "ANDROID_EDGE_AVAILABLE_EDGE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_FLASH_INFO_AVAILABLE, "ANDROID_FLASH_INFO_AVAILABLE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES, "ANDROID_HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, "ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_JPEG_AVAILABLE_THUMBNAIL_SIZES, "ANDROID_JPEG_AVAILABLE_THUMBNAIL_SIZES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_FACING, "ANDROID_LENS_FACING"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_AVAILABLE_APERTURES, "ANDROID_LENS_INFO_AVAILABLE_APERTURES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_AVAILABLE_FILTER_DENSITIES, "ANDROID_LENS_INFO_AVAILABLE_FILTER_DENSITIES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_AVAILABLE_FOCAL_LENGTHS, "ANDROID_LENS_INFO_AVAILABLE_FOCAL_LENGTHS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION, "ANDROID_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION, "ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_HYPERFOCAL_DISTANCE, "ANDROID_LENS_INFO_HYPERFOCAL_DISTANCE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE, "ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES, "ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES, "ANDROID_REQUEST_AVAILABLE_CAPABILITIES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS, "ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, "ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_REQUEST_MAX_NUM_OUTPUT_STREAMS, "ANDROID_REQUEST_MAX_NUM_OUTPUT_STREAMS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_REQUEST_PARTIAL_RESULT_COUNT, "ANDROID_REQUEST_PARTIAL_RESULT_COUNT"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_REQUEST_PIPELINE_MAX_DEPTH, "ANDROID_REQUEST_PIPELINE_MAX_DEPTH"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM, "ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SCALER_CROPPING_TYPE, "ANDROID_SCALER_CROPPING_TYPE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_AVAILABLE_TEST_PATTERN_MODES, "ANDROID_SENSOR_AVAILABLE_TEST_PATTERN_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_BLACK_LEVEL_PATTERN, "ANDROID_SENSOR_BLACK_LEVEL_PATTERN"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_CALIBRATION_TRANSFORM1, "ANDROID_SENSOR_CALIBRATION_TRANSFORM1"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_COLOR_TRANSFORM1, "ANDROID_SENSOR_COLOR_TRANSFORM1"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_FORWARD_MATRIX1, "ANDROID_SENSOR_FORWARD_MATRIX1"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE, "ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT, "ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE, "ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_MAX_FRAME_DURATION, "ANDROID_SENSOR_INFO_MAX_FRAME_DURATION"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_PHYSICAL_SIZE, "ANDROID_SENSOR_INFO_PHYSICAL_SIZE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE, "ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_SENSITIVITY_RANGE, "ANDROID_SENSOR_INFO_SENSITIVITY_RANGE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_WHITE_LEVEL, "ANDROID_SENSOR_INFO_WHITE_LEVEL"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE, "ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_MAX_ANALOG_SENSITIVITY, "ANDROID_SENSOR_MAX_ANALOG_SENSITIVITY"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_ORIENTATION, "ANDROID_SENSOR_ORIENTATION"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_REFERENCE_ILLUMINANT1, "ANDROID_SENSOR_REFERENCE_ILLUMINANT1"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SHADING_AVAILABLE_MODES, "ANDROID_SHADING_AVAILABLE_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES, "ANDROID_STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_STATISTICS_INFO_AVAILABLE_HOT_PIXEL_MAP_MODES, "ANDROID_STATISTICS_INFO_AVAILABLE_HOT_PIXEL_MAP_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES, "ANDROID_STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_STATISTICS_INFO_MAX_FACE_COUNT, "ANDROID_STATISTICS_INFO_MAX_FACE_COUNT"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SYNC_MAX_LATENCY, "ANDROID_SYNC_MAX_LATENCY"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES, "ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_TONEMAP_MAX_CURVE_POINTS, "ANDROID_TONEMAP_MAX_CURVE_POINTS"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_REFERENCE_ILLUMINANT2, "ANDROID_SENSOR_REFERENCE_ILLUMINANT2"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_COLOR_TRANSFORM2, "ANDROID_SENSOR_COLOR_TRANSFORM2"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_CALIBRATION_TRANSFORM2, "ANDROID_SENSOR_CALIBRATION_TRANSFORM2"));
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_SENSOR_FORWARD_MATRIX2, "ANDROID_SENSOR_FORWARD_MATRIX2"));
#ifdef _LINUX
        camCharacteristicsKeys.insert(std::make_pair(
            ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST_RANGE, "ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST_RANGE"));
#endif //_LINUX

    }

}