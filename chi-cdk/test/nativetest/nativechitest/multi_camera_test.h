////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  multi_camera_test.h
/// @brief Declarations for the Multi Camera test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __MULTI_CAMERA_TEST__
#define __MULTI_CAMERA_TEST__

#include "chitestcase.h"

namespace chitests
{
    class MultiCameraTest : public ChiTestCase
    {
    public:
        // Process capture request tracker for multi camera scenario
        struct MultiCameraPCRequest
        {
            UINT32     camId;
            UINT32     frameNumber;
            CHISTREAM* pStream;     // to uniquely match with PC result later
        };

        static std::vector<MultiCameraPCRequest> m_PCReqTracker; // complete tracker for PCRequests in test

        MultiCameraTest() = default;
        ~MultiCameraTest() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Setup
        ///
        /// @brief  Virtual function for setup of this class
        ///
        /// @param  None
        ///
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual VOID  Setup();

        void TestMultiCamera(TestId testType);

    protected:

        // START dummy functions to satisfy pure virtual defs of the parent class (DO NOT USE)
        CDKResult SetupStreams() override { return CDKResultEFailed; };
        CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) override
        {
            NATIVETEST_UNUSED_PARAM(cameraId);
            NATIVETEST_UNUSED_PARAM(sensorMode);
            return CDKResultEFailed;
        };
        CDKResult CreateSessions() override { return CDKResultEFailed; };

        void    CommonProcessCaptureResult(ChiCaptureResult* pCaptureResult, SessionPrivateData* pSessionPrivateData) override;
        void    DestroyResources() override {};
        // END dummy functions

        CDKResult SetupStreamsForCamera(int camIter);
        CDKResult SetupMultiCameraPipelines(int cameraId);
        CDKResult CreateSession(std::vector <UINT32> camList);
        CDKResult InitializeBufferManagers(int cameraId);
        void      DestroyResources(std::vector <UINT32> camList);

    private:
        CHICAPTUREREQUEST                       m_multiCamRequest[MAX_CAMERAS][MAX_REQUESTS]; // array of capture requests
        // TODO move to this chitestcase and make it 2D. It has to be camera and pipeline based.
        // TODO we should not use same pipeline data for all camera pipelines
        CHIPIPELINEMETADATAINFO                 m_pCameraPipelineMetadataInfo;                // pipeline metadata information
        // TODO move this to chitestcase and make it 2D
        ChiPrivateData                          m_cameraRequestPvtData[MAX_CAMERAS];                // request private data
        ChiStream*                              m_pRequiredStreams[MAX_CAMERAS];              // pointer to created stream objects for camera A
        // TODO move this to chitestcase and make it 2D
        std::map<StreamUsecases, ChiStream*>    m_streamIdMap[MAX_CAMERAS];                   // map storing unique streamId and stream object for camera A
        std::map<ChiStream*,ChiBufferManager*>  m_streamBufferMap[MAX_CAMERAS];               // map storing stream and it's associated buffermanager
        ChiBufferManager*                       m_pBufferManagers[MAX_CAMERAS][MAX_STREAMS];  // pointer to buffermanagers for each stream

        /* StreamCreateData structure and memory allocated for its variables (overriding parent class variables) */
        StreamCreateData                        m_streamInfo[MAX_CAMERAS];                    // stream info required for creating stream objects
        int                                     m_numStreams[MAX_CAMERAS];                    // total number of streams for given testcase
        CHISTREAMFORMAT                         m_format[MAX_CAMERAS][MAX_STREAMS];           // format for the streams
        CHISTREAMTYPE                           m_direction[MAX_CAMERAS][MAX_STREAMS];        // stream direction
        Size                                    m_resolution[MAX_CAMERAS][MAX_STREAMS];       // resolution for the streams
        StreamUsecases                          m_streamId[MAX_CAMERAS][MAX_STREAMS];         // unique identifier for the streams
        uint32_t                                m_usageFlag[MAX_CAMERAS][MAX_STREAMS];        // gralloc usage flags for the streams
        bool                                    m_isRealtime[MAX_CAMERAS][MAX_STREAMS];       // bool to indicate if this stream is part of realtime session

        int GetCamIdFromStream(CHISTREAM* pStream);
        CDKResult GenerateMultiCameraCaptureRequest(std::vector <UINT32> camList, UINT32 request);

    };
} // namespace chitests

#endif  // __MULTI_CAMERA_TEST__
