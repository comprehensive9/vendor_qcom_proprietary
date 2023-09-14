//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAPTURE_REQUEST_TEST__
#define __CAPTURE_REQUEST_TEST__

#include "camera3stream.h"

namespace haltests
{

    class CaptureRequestTest : public Camera3Stream
    {

    public:
        CaptureRequestTest();
        ~CaptureRequestTest();

        void TestExecute(int format, Size resolution, bool bDumpOutputBuffer, void(CaptureRequestTest::*TestCaptureRequest)(int, int, int, camera_info*, bool));
        void TestAfModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestAeModeAndLock(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestAwbModeAndLock(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestLensShadingMap(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestEffectModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestSceneModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestAntiBandingModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestFlashControl(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestToneMapControl(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestColorCorrectionControl(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestEdgeModes(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestFocusDistanceControl(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestZoom(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestNoiseReductionModeControl(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestCameraStabilization(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestFaceDetection(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestDynamicBlackWhiteLevel(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void Test3ARegions(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestAeCompensation(int cameraId, int format, int frameNumber, camera_info * info, bool bDumpOutputBuffer);
        void TestAePrecaptureTrigger(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestAfTrigger(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestEnableZSL(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestCaptureIntent(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestControlMode(int cameraId, int format, int frameNumber, camera_info * info, bool bDumpOutputBuffer);
        void TestPostRawSensitivityBoost(int cameraId, int format, int frameNumber, camera_info * info, bool bDumpOutputBuffer);
        void TestTestPattern(int cameraId, int format, int frameNumber, camera_info * info, bool bDumpOutputBuffer);
        void TestColorCorrectionAberration(int cameraId, int format, int frameNumber, camera_info * info, bool bDumpOutputBuffer);
        void TestValidateEnableZSL(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestHDR(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestMFNR(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestHDRWithMFNR(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);
        void TestHDRWithFlash(int cameraId, int format, int frameNumber, camera_info *info, bool bDumpOutputBuffer);

        static bool hasFocuser(camera_info *info);

    protected:
        virtual void Setup();
        virtual void Teardown();

        void SetupPreview(
            int cameraId,
            int format,
            Size resolution,
            int& frameNumber);

        int ApplyAndVerifyKeySetting(int cameraId, int frameNumber, uint32_t tag, const uint8_t *value, size_t dataCount = 1, bool setKey = true);
        int ApplyAndVerifyKeySetting(int cameraId, int frameNumber, uint32_t tag, const int32_t *value, size_t dataCount = 1, float errorMargin = 0);
        int ApplyAndVerifyKeySetting(int cameraId, int frameNumber, uint32_t tag, const int64_t *value, size_t dataCount = 1, float errorMargin = 0);
        int ApplyAndVerifyKeySetting(int cameraId, int frameNumber, uint32_t tag, const float * value, size_t dataCount = 1, bool skipVerification = false);
        int ApplyAndVerifyKeySetting(int cameraId, int frameNumber, uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount = 1, bool skipVerification = false);
        int ApplyAndVerifyKeySettingWith3AConverged(int cameraId, int frameNumber, uint32_t tag, const uint8_t * value, size_t dataCount = 1);

        void TestAeManualControl(
            int cameraId,
            int frameNumber,
            camera_info * info,
            bool bDumpOutputBuffer,
            int format);

        static bool IsCaptureRequestKeySupported(camera_info * info, int32_t tag);
        static bool IsCaptureResultKeySupported(camera_info * info, int32_t tag);
        static bool IsCharacteristicKeySupported(camera_info * info, int32_t tag);

        void WaitForAeLocked(camera_info * info, int cameraId, int &frameNumber, int numResultsWait);
        void WaitForAeStable(camera_info * info, int cameraId, int & frameNumber, int numResultsWait);
        void WaitForAnyStateValue(int cameraId, int & frameNumber, int stateTag, uint8_t expectedValues[], int numExpectedValues, int numResultsWait);
        void WaitForSettingsApplied(int cameraId, int &frameNumber, int numResultsWait);

        int64_t GetExposureMaximumOrDefault(camera_info * info);
        int64_t GetExposureMinimumOrDefault(camera_info * info);
        int32_t GetSensitivityMaximumOrDefault(camera_info * info);
        int32_t GetSensitivityMinimumOrDefault(camera_info * info);
        int64_t GetMaxExposureValue(camera3_capture_request_t * request, int64_t maxExposureTimeUs, int64_t maxSensitivity);
        int64_t GetExposureValue(const camera_metadata_t * metadata);
        int32_t GetIntFromMetadata(const camera_metadata_t * metadata, camera_metadata_tag_t tag);
        int64_t GetLongFromMetadata(const camera_metadata_t * metadata, camera_metadata_tag_t tag);

    private:
        const int64_t DEFAULT_EXP_TIME_NS = 30000000L;
        const int32_t DEFAULT_SENSITIVITY = 100;
        const float EXPOSURE_TIME_ERROR_MARGIN_RATE = 0.03f; // 3%, Approximation.
        const float SENSITIVITY_ERROR_MARGIN_RATE = 0.03f; // 3%, Approximation.
        const int DEFAULT_NUM_EXPOSURE_TIME_STEPS = 3;
        const int DEFAULT_SENSITIVITY_STEP_SIZE = 100;
        const int DEFAULT_NUM_SENSITIVITY_STEPS = 16;
        const int NUM_RESULTS_WAIT_TIMEOUT = 100;
        const int NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY = 8;
        const int32_t SENSOR_INFO_SENSITIVITY_RANGE_MIN_AT_MOST = 100;
        const int32_t SENSOR_INFO_SENSITIVITY_RANGE_MAX_AT_LEAST = 800;
        const int64_t SENSOR_INFO_EXPOSURE_TIME_RANGE_MIN_AT_MOST = 100000L; //100us
        const int64_t SENSOR_INFO_EXPOSURE_TIME_RANGE_MAX_AT_LEAST = 100000000; //100ms
        const int32_t POST_RAW_SENSITIVITY_BOOST_STEP = 100; //100iso
        const double AE_COMPENSATION_ERROR_TOLERANCE = 0.2;
        const int FACE_ID_UNSUPPORTED = -1;
        const int FACE_SCORE_MIN = 1;
        const int FACE_SCORE_MAX = 100;

        // 5 percent error margin for calibrated device
        const float FOCUS_DISTANCE_ERROR_PERCENT_CALIBRATED = 0.05f;
        // 25 percent error margin for uncalibrated device
        const float FOCUS_DISTANCE_ERROR_PERCENT_UNCALIBRATED = 0.25f;
        // 10 percent error margin for approximate device
        const float FOCUS_DISTANCE_ERROR_PERCENT_APPROXIMATE = 0.10f;

        static bool isCapabilitySupported(camera_info *info, uint8_t capability);

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

        Rect getCropRegionForZoom(float zoomFactor, PointF center, float maxZoom, Rect activeArray) const;
    };

}


#endif  //#ifndef __CAPTURE_REQUEST_TEST__
