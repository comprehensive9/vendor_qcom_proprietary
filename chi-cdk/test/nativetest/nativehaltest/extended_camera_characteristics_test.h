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

#ifndef __EXTENDED_CAMERA_CHARACTERISTICS_TEST__
#define __EXTENDED_CAMERA_CHARACTERISTICS_TEST__

#include "camera3device.h"
#include <map>

namespace haltests
{
    class ExtendedCameraCharacteristicsTest : public Camera3Device
    {

    public:
        ExtendedCameraCharacteristicsTest();
        ~ExtendedCameraCharacteristicsTest();

        struct Region
        {
            uint32_t left;
            uint32_t top;
            uint32_t width;
            uint32_t height;
            Region (uint32_t l = 0, uint32_t t = 0, uint32_t w = 0, uint32_t h = 0)
                : left(l), top(t), width(w), height(h) {}

        };

        void TestKeys();

       /**
        * Test that the available stream configurations contain a few required formats and sizes.
        */
        void TestAvailableStreamConfigs();

       /**
        * Test values for static metadata used by the RAW capability.
        */
        void TestStaticRawCharacteristics();

       /**
        * Test values for static metadata used by the BURST capability.
        */
        void TestStaticBurstCharacteristics();

       /**
        * Check reprocessing capabilities.
        */
        void TestReprocessingCharacteristics();

       /**
        * Check depth output capability
        */
        void TestDepthOutputCharacteristics();

       /**
        * Test high speed capability and cross-check the high speed sizes and fps ranges from
        * the StreamConfigurationMap.
        */
        void TestConstrainedHighSpeedCapability();

       /**
        * Sanity check of optical black regions.
        */
        void TestOpticalBlackRegions();

    protected:
        virtual void Setup();
        virtual void Teardown();
    private:
        std::vector<int> actualCapabilities;
        std::map<camera_metadata_tag_t, std::string> camCharacteristicsKeys;
        std::map<int, std::string> camCapabilities;
        std::vector<int32_t> availableResultKeys;
       /**
        * Check key is present in characteristics if the hardware level is at least {@code hwLevel};
        * check that the key is present if the actual capabilities are one of {@code capabilities}.
        *
        * @return value of the {@code key} from {@code c}
        */
        void ExpectKeyAvailable(camera_info info,
            int actualHwLevel,
            camera_metadata_tag_t tag,
            int hwLevel,
            int capability);

       /**
        * HW Levels shorthand
        */
        static const int MAX_INTEGER = 999999;
        static const int LEGACY = ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY;
        static const int LIMITED = ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED;
        static const int FULL = ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL;
#ifdef _LINUX
        static const int LEVEL_3 = ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_3;
#else
        static const int LEVEL_3 = 3;
#endif //_LINUX
        static const int OPT = MAX_INTEGER;  // For keys that are optional on all hardware levels.

       /**
        * Capabilities shorthand
        */
        static const int NONE = -1;
        static const int BC = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE;
        static const int MANUAL_SENSOR = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR;
        static const int MANUAL_POSTPROC = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_POST_PROCESSING;
        static const int RAW = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_RAW;
        static const int YUV_REPROCESS = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_YUV_REPROCESSING;
        static const int OPAQUE_REPROCESS = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_PRIVATE_REPROCESSING;
        static const int CONSTRAINED_HIGH_SPEED = ANDROID_REQUEST_AVAILABLE_CAPABILITIES_CONSTRAINED_HIGH_SPEED_VIDEO;
        static const int HIGH_SPEED_FPS_LOWER_MIN = 30;
        static const int HIGH_SPEED_FPS_UPPER_MIN = 120;

        int CompareHardwareLevel(int left, int right);
        int RemapHardwareLevel(int level);
        std::string ToStringHardwareLevel(int level);
        void ConstructCharacteristicsKeyMap();
        std::string ToStringCharacteristicsTag(camera_metadata_tag_t tag);
        void GetActualCapabilities(camera_info info);
        void GetCaptureResultKeys(camera_info info);
        void VerifyRectSize(Size activeRectSize, int cameraId, std::vector<Size> sizesOfFormat);
        std::vector<Size> yuvSizes;
        std::vector<Size> jpegSizes;
        std::vector<Size> privateSizes;
        bool ContainSizes(std::vector<Size> sizeVec1, std::vector<Size> sizeVec2);
        bool IsKeySupported(std::vector<uint8_t> keyVector, uint8_t key);
        bool IsFormatSupportedInConfig(camera_info info, int format, std::vector<Size> &sizeVector);
        bool IsFormatSupportedInConfig(camera_info info, int reqFormat);
        int32_t maxYuvSize;
    };


}




#endif  //#ifndef __EXTENDED_CAMERA_CHARACTERISTICS_TEST__