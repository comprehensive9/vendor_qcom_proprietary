//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __STILL_CAPTURE_TEST__
#define __STILL_CAPTURE_TEST__

#include "haltestcase.h"

namespace haltests
{
    class StillCaptureTest : public HalTestCase
    {
        public:
            StillCaptureTest();
            ~StillCaptureTest();

            enum SpecialSetting
            {
                NoSetting,
                SettingZSL,
                SettingSWMF,
                SettingMNFR,
                SettingSAT,
                SettingBokeh,
                SettingBurstshot,
                NumOfSpecialSetting
            };

            struct StillCaptureItem : StreamItem
            {
                StillCaptureItem(int f, Size r) : StreamItem(f, r) {}
            };

            void TestSingleCameraCapture(int format, Size resolution, const char* testName, int captureCount,
                bool mustSupportRes, SpecialSetting setting = NoSetting);

            void TestSnapshotWithPreview(int previewFormat, int snap1Format, int snap2Format, Size previewRes, Size snap1Res,
                Size snap2Res, const char* testName, int captureCount, bool dumpPreview, bool enableZsl);

            void TestSnapshotGeneric(StillCaptureItem previews[], int numPreviews, StillCaptureItem snapshots[],
                int numSnapshots, const char* testName, int captureCount);

            void TestDepthCamera(StillCaptureItem preview, const char* testName, int captureCount, bool bEnableSecureStreaming,
                bool bVerifyIntrinsics = false);

        protected:
            virtual void Setup();
            virtual void Teardown();

        private:
            // Test basic still capture sequence.
            void TestCapture(int cameraId, int format, Size resolution, const char* testName, int captureCount, int setting);

            bool SupportedResolutionHelper(int cameraId, camera_info* info, const StillCaptureItem streams[], int numStreams) const;

            void CreateStreamHelper(int cameraId, const StillCaptureItem previews[], int numPreviews,
                const StillCaptureItem snapshots[], int numSnapshots);

            CDKResult VerifyDepthLensIntrinsics(int cameraId, const camera_metadata_t * metadata);
    };

}
#endif  //#ifndef __STILL_CAPTURE_TEST__
