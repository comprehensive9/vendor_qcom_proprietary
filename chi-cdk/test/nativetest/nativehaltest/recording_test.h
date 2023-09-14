//*****************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*****************************************************************************

#ifndef __RECORDING_TEST__
#define __RECORDING_TEST__

#include "camera3stream.h"
#include <cstdint>

namespace haltests
{
    class RecordingTest : public Camera3Stream
    {
    public:
        RecordingTest();
        ~RecordingTest();

        struct HighSpeedConfig
        {
            Size resolution;
            int fps;
            HighSpeedConfig(Size r, int f) : resolution(r), fps(f) {}
        };
        void TestBasicVideoRecording(int previewFormat, int videoFormat,
            Size previewRes, Size videoRes, int frameCount, int fps, const char* testName);
        void TestConstrainedHighSpeedRecording(Size resolution, int32_t fpsMin, int32_t fpsMax, int captureCount);
        void TestConstrainedHighSpeedRecordingHelper(int cameraId, Size resolution, int32_t fpsMin, int32_t fpsMax,
            int32_t batchSize, int captureCount);
        void TestBasicVideoLiveSnapshot(int previewFormat, int videoFormat, int snapFormat,
            Size previewRes, Size videoRes, Size snapshotRes, int captureCount, const char* testName);
        void TestVideoStabilization();
        void TestVideoZoom();
        void TestConstrainedHighSpeedRecordingSweep(int captureCount);

        static const int64_t CAPTURE_RESULT_TIMEOUT = 1;
        static const int PROCESS_CAPTURE_TIMEOUT = 10;
        static const int MAX_WAIT_RETRIES = 10;

        /*
        * availableHighSpeedVideoConfigurations format:
        * one dimensional array where each config consists of 5 int32_t's
        * [width, height, fpsMin, fpsMax, batchsize]
        * AHSVC_CONFIG_SIZE defined below ensures we go to the start of next
        * configuration
        */
        const static int AHSVC_OFFSET_WIDTH = 0;
        const static int AHSVC_OFFSET_HEIGHT = 1;
        const static int AHSVC_OFFSET_FPS_MIN = 2;
        const static int AHSVC_OFFSET_FPS_MAX = 3;
        const static int AHSVC_OFFSET_BATCH_SIZE = 4;
        const static int AHSVC_CONFIG_SIZE = 5;

        /*
        * CustomHFRFpsTable format:
        * one dimensional array where each config consists of 3 int32_t's
        * [width, height, fps]
        */
        const static int CUSTOM_HFR_OFFSET_WIDTH = 0;
        const static int CUSTOM_HFR_OFFSET_HEIGHT = 1;
        const static int CUSTOM_HFR_OFFSET_FPS = 2;
        const static int CUSTOM_HFR_CONFIG_SIZE = 3;
        const static int CUSTOM_HFR_RANGE_LOW = 30;
        const static int CUSTOM_HFR_RANGE_HIGH = 90;

        /*
        * aeAvailableTargetFpsRanges format:
        * one dimensional array where each config consists of 2 int32_t's
        * [fpsMin, fpsMax]
        */
        const static int TARGET_FPS_OFFSET_FPS_MIN = 0;
        const static int TARGET_FPS_OFFSET_FPS_MAX = 1;
        const static int TARGET_FPS_CONFIG_SIZE = 2;

    protected:
        virtual void Setup();

    private:

        void StartHighSpeedStreaming(int cameraId, int batchSize, int& frameStart, std::string testName);
        void VerifyFinalTargetFPS(int32_t* fpsRange, int cameraId);
        int FindMatchingConfig(Size resolution, int32_t fpsMin, int32_t fpsMax, camera_metadata_entry configs);
        bool IsHighSpeedSupported(int cameraId, camera_info info);

        // point struct for zoom test
        struct PointF
        {
            float x;
            float y;
        };

        // rect struct for zoom test
        struct Rect
        {
            int left;
            int top;
            int right;
            int bottom;
        };
        // 5 percent error margin for resulting crop regions
        const float CROP_REGION_ERROR_PERCENT_DELTA = 0.05f;
        // 1 percent error margin for centering the crop region
        const float CROP_REGION_ERROR_PERCENT_CENTERED = 0.01f;

        Rect GetCropRegionForZoom(float zoomFactor, PointF center,
            float maxZoom, Rect activeArray) const;

    };
}

#endif  //#ifndef __RECORDING_TEST__
