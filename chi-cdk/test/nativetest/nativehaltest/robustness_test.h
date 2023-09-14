//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __ROBUSTNESS_TEST__
#define __ROBUSTNESS_TEST__

#include "camera3stream.h"

namespace haltests
{
    class RobustnessTest : public Camera3Stream
    {
    public:
        RobustnessTest();
        ~RobustnessTest();

        /**
        * Test a snapshot with wrong supported size
        */
        void TestBadSurfaceDimension();
        void TestBasicTriggerSequence();
        void TestSimultaneousTriggers();
        void TestResolutionSweep();

        struct RobustnessItem : StreamItem
        {
            RobustnessItem(int f, Size r) : StreamItem(f, r) {}
            RobustnessItem() : StreamItem(0, Size(0,0)) {}
        };

    protected:
        virtual void Setup();

    private:
        uint8_t* afModes;
        uint8_t* aeModes;
        size_t afCount;
        size_t aeCount;
        RobustnessItem* supportedStreams;


        bool PrepareTriggerTestSession(int cameraId);
        void GetAvailableAfandAeModes(camera_info info);
        void GetSupportedResolutions(camera_info info, int requestedDirection, int& numSupportedStreams);
        void CancelTriggersAndWait(int cameraId, uint8_t afMode, int& frameNumber);
        void TriggerAfandVerify(int cameraId, uint8_t afMode, int& frameNumber);
        static void VerifyAfSequence(uint8_t afMode, uint8_t afState, bool& focusComplete);
        void TriggerAeandVerify(int cameraId, int& frameNumber);
        static void VerifyAeSequence(uint8_t aeState, bool& precaptureComplete);
        void TriggerAfAeandVerify(int cameraId, uint8_t afMode, int& frameNumber);
        uint8_t GetAeStateFromMeta(const camera_metadata_t* metaFromFile);
        uint8_t GetAfStateFromMeta(const camera_metadata_t* metaFromFile);
        void BasicTriggerSequenceHelper(int cameraId);
        void SimultaneousTriggersHelper(int cameraId);
        void ResolutionSweepHelper(int cameraId, int numSupportedStreams, int captureCount);
        const int MAX_RESULT_STATE_POSTCHANGE_WAIT_FRAMES = 10;
        const int PREVIEW_WARMUP_FRAMES = 60;
        const int MAX_TRIGGER_SEQUENCE_FRAMES = 180;

    };
}

#endif  //#ifndef __ROBUSTNESS_TEST__
