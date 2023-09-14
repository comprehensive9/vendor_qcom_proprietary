//******************************************************************************
// Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __FLUSH_TEST__
#define __FLUSH_TEST__

#define FLAGS_VIDEO_ENCODER 0x00010000

#include "camera3stream.h"

namespace haltests
{
    class FlushTest : public Camera3Stream
    {

    public:
        FlushTest() {};
        ~FlushTest() {};

        enum CaptureType
        {
            CapturePreview  = CAMERA3_TEMPLATE_PREVIEW,
            CaptureSnapshot = CAMERA3_TEMPLATE_STILL_CAPTURE
        };

        enum SpecialSetting
        {
            SettingZSL,
            SettingSWMF,
            SettingMNFR,
            SettingSAT,
            SettingBokeh,
            SettingBurstshot,
            NumOfSpecialSetting
        };

        struct FlushItem : StreamItem
        {
            FlushItem(int f, Size r) : StreamItem(f, r) {}
        };

        void TestFlushSnapshot(
            int captureFormat,        //[in] capture format
            Size captureResolution,   //[in] capture resolution
            const char* testName,     //[in] test name
            int captureCount,         //[in] number of frames to capture
            SpecialSetting setting,   //[in] special setting to apply
            CaptureType  captureType);//[in] type of capture to dump - snapshot/preview

        void TestFlushRecordingHFR(
            Size resolution,        // image resolution
            int32_t fpsMin,         // FPS lower bound
            int32_t fpsMax,         // FPS upper bound
            int captureCount);      // number of captures to take

        void TestFlushRecordingHDR(
            int previewFormat,     // preview format
            int videoFormat,       // video format
            Size previewRes,       // preview resolution
            Size videoRes,         // video resolution
            int frameCount,        // number of frames to capture
            int fps,               // fps to be used
            const char* testName); // filename to be used

        void TestFlushRecordingEIS(
            FlushItem preview,     // preview StreamItem
            FlushItem video,       // video StreamItem
            FlushItem snapshot,    // snapshot StreamItem
            const char* eisMode,   // EIS2 / EIS3 mode
            int captureCount,      // number of frames to capture
            bool isVideoOn);       // whether video recording is on

    private:
        void SetEnableZsl(int cameraId);

        const int FLUSH_FRAME_INTERVAL = 7;

        void StartHighSpeedStreaming(int cameraId, int batchSize, int& frameStart, std::string testName);
        void VerifyFinalTargetFPS(int32_t* fpsRange, int cameraId);
        int  FindMatchingConfig(Size resolution, int32_t fpsMin, int32_t fpsMax, camera_metadata_entry configs);
        bool IsHighSpeedSupported(int cameraId, camera_info info);

        void PrepareEISSnapshotRequest(
            int cameraId,           // camera Id to test
            FlushItem preview,      // preview StreamItem
            FlushItem snapshot,     // snapshot StreamItem
            int &frameNumber,       // frame number to process
            uint32_t opmode);       // operation mode for configuring streams

        void PrepareEISVideoRequest(
            int cameraId,           // camera Id to test
            FlushItem preview,      // preview StreamItem
            FlushItem video,        // video StreamItem
            FlushItem snapshot,     // snapshot StreamItem
            int &frameNumber,       // frame number to process
            uint32_t opmode);       // operation mode for configuring streams

        void VerifyEISResult(int cameraId, int &frameNumber, uint32_t tagId, const uint8_t *value, bool isVideoOn);

    };

}

#endif  //#ifndef __FLUSH_TEST__