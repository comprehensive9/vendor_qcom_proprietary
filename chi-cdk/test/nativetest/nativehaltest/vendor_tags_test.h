//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __VENDOR_TAGS_TEST__
#define __VENDOR_TAGS_TEST__


#include "camera3stream.h"
#ifdef _LINUX
#include "camera_metadata_hidden.h"
#include <string.h>
#include <cmath>
#else
#include "system/camera_metadata_hidden.h"
#endif

namespace haltests
{
    class VendorTagsTest : public Camera3Stream
    {
    public:
        VendorTagsTest();
        ~VendorTagsTest();

        struct VendorTagItem : StreamItem
        {
            VendorTagItem(int f, Size r) : StreamItem(f, r) {}
        };

        static uint32_t ResolveAvailableVendorTag(vendor_tag_ops_t *vendorTagOps, const char *tagName, const char *sectionName);
        static int InitializeVendorTags(vendor_tag_ops_t *vendorTagOps);

        void TestSaturation(int format, Size resolution);
        void TestExposurePriority(int format, Size resolution);
        void TestIsoPriority(int format, Size resolution);
        void TestOverrideTagId();
        void TestOverrideTagAppRead();
        void TestOverrideTagReadWrite() const;
        void TestOverrideTagType();
        void TestOverrideTagCount();
        void TestEIS(
            VendorTagItem preview,
            VendorTagItem video,
            VendorTagItem snapshot,
            const char* eisMode,
            int captureCount,
            bool isVideoOn);

        /**
        * Operation modes required for EISv2 and EISv3 usecase selection
        */
        static const uint32_t CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV2 = 0xF004;
        static const uint32_t CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3 = 0xF008;

        /**
        * Gralloc flags required for EIS testcases
        */
        static const uint32_t GrallocUsageHwTexture = 0x00000100;
        static const uint32_t GrallocUsageHwVideoEncoder = 0x00010000;
        static const uint32_t GrallocUsageHwCameraWrite = 0x00020000;

        static const int PROCESS_CAPTURE_TIMEOUT = 30;

        static uint32_t *availableTags;
        static int tagCount;

    protected:
        virtual void Setup();
    private:
        void prepare3AConvergedRequest(
            int cameraId,
            int format,
            Size resolution,
            bool dumpBuffer,
            int& frameNumber);
        void prepareEISSnapshotRequest(
            int cameraId,
            VendorTagItem preview,
            VendorTagItem snapshot,
            int &frameNumber,
            uint32_t opmode);
        void prepareEISVideoRequest(
            int cameraId,
            VendorTagItem preview,
            VendorTagItem video,
            VendorTagItem snapshot,
            int &frameNumber,
            uint32_t opmode);

        void verifyPriorityResult(int cameraId, int &frameNumber, const int64_t *value, int32_t priority);
        void verifyEISResult(int cameraId, int &frameNumber, uint32_t tagId, const uint8_t *value, bool isVideoOn);
        const double TOLERANCE = 0.2;
        const int64_t ONE_SECOND = 1000000000;

        ///@brief struct for grouping vendortaginfo
        struct tagInfo
        {
            const char*  sectionName;    ///< sectionName
            const char*  tagName;        ///< tagName
            int          tagType;        ///< tagType
            int          count;          ///< number of elements in tag
        };

        ///@brief number of Tags defined in override
        static const int NUM_CUSTOM_TAGS = 4;

        ///@brief customTag info defined in chi override
        tagInfo customTags[NUM_CUSTOM_TAGS] =
        {
            { "com.bots.chi.override","bots_override_tag1", TYPE_INT32, 1 },
            { "com.bots.chi.override","bots_override_tag2", TYPE_BYTE,  1 },
            { "com.bots.chi.override","bots_override_tag3", TYPE_INT64, 1 },
            { "com.bots.chi.override","bots_override_tag4", TYPE_INT64, 2 }
        };

    };
}

#endif  //#ifndef __VENDOR_TAGS_TEST__
