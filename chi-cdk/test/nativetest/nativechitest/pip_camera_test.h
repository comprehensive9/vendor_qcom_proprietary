////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  pip_camera_test.h
/// @brief Declarations for the Picture-in-Picture test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __PIP_CAMERA_TEST__
#define __PIP_CAMERA_TEST__

#include "chitestcase.h"

namespace chitests
{
    class PIPCameraTest : public ChiTestCase
    {
    public:

        PIPCameraTest() = default;
        ~PIPCameraTest() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Setup
        ///
        /// @brief  Virtual function for setup of this class
        /// @param  None
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual VOID  Setup();

        void TestPIPCamera(TestId testType);

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
        void      DestroyResources() override {};
        // END dummy functions

        CDKResult       SetupStreamsForCamera(int camIter);
        CDKResult       SetupPIPCameraPipelines(int cameraId);
        CDKResult       CreateSessions(std::vector <UINT32> camList);
        CDKResult       InitializeBufferManagers(int cameraId);

        void    CommonProcessCaptureResult(ChiCaptureResult* pCaptureResult, SessionPrivateData* pSessionPrivateData) override;
        void    DestroyResources(std::vector <UINT32> camList);

    private:
        ChiCallBacks                            m_callbacks[MAX_CAMERAS];
        CHICAPTUREREQUEST                       m_pipCamRequest[MAX_CAMERAS][MAX_REQUESTS];   // array of capture requests
        // TODO move to this chitestcase and make it 2D. It has to be camera and pipeline based.
        // TODO we should not use same pipeline data for all camera pipelines
        CHIPIPELINEMETADATAINFO                 m_pCameraPipelineMetadataInfo;                // pipeline metadata information
        // TODO move this to chitestcase and make it 2D
        ChiPrivateData                          m_cameraRequestPvtData[MAX_CAMERAS];                // request private data
        ChiStream*                              m_pRequiredStreams[MAX_CAMERAS];              // pointer to created stream objects for camera A
        // TODO move this to chitestcase and make it 2D
        ChiPipeline*                            m_pCameraChiPipeline[MAX_CAMERAS];                  // pipeline instance for realtime tests
        std::map<StreamUsecases, ChiStream*>    m_streamIdMap[MAX_CAMERAS];                   // map storing unique streamId and stream object for camera A
        std::map<ChiStream*, ChiBufferManager*> m_streamBufferMap[MAX_CAMERAS];               // map storing stream and it's associated buffermanager
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

        CDKResult                               GeneratePIPCaptureRequest(std::vector <UINT32> camList, UINT32 request);

    };
} // namespace chitests

#endif  // __PIP_CAMERA_TEST__
