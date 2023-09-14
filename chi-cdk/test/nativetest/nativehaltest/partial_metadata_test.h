//*****************************************************************************
// Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*****************************************************************************

#ifndef __PARTIAL_METADATA_TEST__
#define __PARTIAL_METADATA_TEST__

#include "camera3stream.h"
#include <set>
namespace haltests
{
    class PartialMetadataTest : public Camera3Stream
    {
    public:

        enum PartialMode {
            PartialModeA = 0,
            PartialModeB,
            PartialModeC,
            PartialModeD,
            PartialModeE,
            PartialNumOfModes
        };

        enum PartialTestType {
            PartialStaticSupport = 0,
            PartialOrdering,
            PartialDisjoint,
            PartialEnableZSL,
            PartialFlush,
            PartialNumOfTestTypes
        };

        PartialMetadataTest() {};
        ~PartialMetadataTest() {};

        void TestPartialStaticSupport(PartialMode mode);
        void TestPartialOrdering(PartialMode mode);
        void TestPartialDisjoint(PartialMode mode);
        void TestPartialEnableZSL(PartialMode mode);
        void TestPartialFlush(PartialMode mode);

    protected:

        virtual void Setup() override;
        virtual void Teardown() override;
        void TakeCaptureAndValidate(PartialMode mode, PartialTestType testType, const char* testName);

        CDKResult ValidateOrdering(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet);
        CDKResult ValidateDisjoint(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet);
        CDKResult ValidateEnableZSL(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet);
        CDKResult ValidateFlush(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet);

    private:

        const int m_modeCount[5] = { 1, 2, 2, 3, 4 };                   // Expected partial result count, to be used with PartialMode enums
        static const int MAX_PARTIAL_RESULTS = 5;                       // Max number of partial results the driver can support
        static const int FLUSH_FRAME_INTERVAL = 7;                      // Frame interval to perform flush
        uint32_t m_partialResultCount[MAX_NUMBER_CAMERAS];              // Partial result count reported by device per camera
        uint32_t m_prevPartialResult;                                   // Previous partial result received
        uint32_t m_prevFrameNumber;                                     // Frame number of the previous result received
        camera_metadata_t* m_pPartialMetadatas[MAX_PARTIAL_RESULTS];    // Partial results stored for the current frame
        int m_foundZSLTagCount;                                         // Count of ZSL tags found in partial results
        int m_partialFlushCount;                                        // Count of partial results from a flushed frame (-1 means frame not flushed)
        std::set<int> m_receivedPartialIdsSet;                          // partial result Ids received for the current frame (for ordering test)

        void SetEnableZsl(int cameraId, bool enable);
        inline bool IsZSLFrame(uint32_t frameNumber)
        {
            return frameNumber % 2 == 0;
        };
    };
}

#endif  //#ifndef __PARTIAL_METADATA_TEST__