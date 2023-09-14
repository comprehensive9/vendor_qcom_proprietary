//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************
#ifndef __HAL_TEST_CASE__
#define __HAL_TEST_CASE__

#include "camera3common.h"
#include "camera3supermodule.h"
#include "cam3device.h"
#include "staticmetadata.h"

namespace haltests
{
    class HalTestCase : public Camera3SuperModule,
                        public NativeTest
    {

    public:
        HalTestCase();
        ~HalTestCase();

        enum StreamConfiguration
        {
            FORMAT_OFFSET,
            RESOLUTION_WIDTH_OFFSET,
            RESOLUTION_HEIGHT_OFFSET,
            DIRECTION_OFFSET
        };

        int mCurrentTestCamId;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// inRange
        ///
        /// @brief  check if var is in within the range specified
        ///
        /// @return bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool inRange(int lowerLimit, int upperLimit, int var)
        {
            return ((var-upperLimit)*(var-lowerLimit) <= 0);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetPCRFrameNumbers
        ///
        /// @brief  Generates vector of linearly spaced frame numbers
        ///
        /// @return std::vector<int>
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> GetPCRFrameNumbers(int streamIndex);

    protected:
        virtual void Setup();
        virtual void Teardown();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// InitCam3DeviceForCamera()
        ///
        /// @brief  Initializes Cam3Device for Camera
        ///
        /// @param  camId   CameraID
        ///
        /// @return void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void InitCam3DeviceForCamera(uint32_t camId);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// AreCameraStreamsEqual()
        ///
        /// @brief  returns if the camera streams are equal
        ///
        /// @return bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool AreCameraStreamsEqual(uint32_t camId, camera3_stream_t &lhs, camera3_stream_t &rhs);
        bool CheckSupportedResolution(uint32_t camId, camera_info* info,
            int format,             //[in] format to be checked
            Size resolution,        //[in] resolution to be checked
            int direction = CAMERA3_STREAM_OUTPUT) const;   //[in] direction of the stream
        void ClearOutStreams(uint32_t camId);
        void CloseCamera(uint32_t camId);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ConfigStream
        ///
        /// @brief  Configuration of streams and session with session paramters
        ///
        /// @param  camId               Camera ID
        /// @param  opmode              operation mode of the session
        /// @param  setResourceCheck    flag to indicate bypass resource check session parameter
        /// @param  enableRTB           flag to indicate RTB feature enablement
        ///
        /// @return result code for configuration status
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int ConfigStream(uint32_t camId, uint32_t opmode, bool setResourceCheck = false, bool enableRTB = false);

        void CreateStream(
            uint32_t camId,
            int streamType,                         //[in] stream type specifies if stream is O/P, I/P or bidirectional type
            Size resolution,                        //[in] resolution of the stream
            uint32_t usage,                         //[in] usage flag required by gralloc
            int format,                             //[in] format of the stream
            camera3_stream_rotation_t rotation,     //[in] rotation of camera
            int dataspace = -1,                    //[in] dataspace, defaults to -1 if none provided
            std::string physicalCameraId = ""
        );
        void CreateMultiStreams(
            uint32_t camId,
            Size outputResolutions[],   //[in] Array of resolutions corresponding to each stream
            int outputFormats[],     //[in] Array of formats corresponding to each stream
            int numStreams,          //[in] Number of streams to be configured
            uint32_t usageFlags[],   //[in] Array of usage flags corresponding to each stream
            int streamTypes[]);      //[in] Array of stream type corresponding to each stream
        int FindCameraMetadataFromTemplate(
            uint32_t camId,
            camera_metadata_t* templateRequest,
            camera_metadata_tag_t tag);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// HalTestCase::IsHardwareAtleast()
        ///
        /// @brief
        ///   returns true if device supports the given hardware level
        ///
        /// @return
        ///   bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool IsHardwareAtleast(int cameraId, int level, camera_info *info);
        camera3_device_t* GetCamDevice(uint32_t camId) const;
        camera_metadata_t * GetCameraMetadataByFrame(
            uint32_t camId,
            uint32_t frameNumber) const; //[in] framenumber for getting corresponding metadata
        int GetCameraMetadataEntryByTag(
            uint32_t camId,
            camera_metadata_t* metadata,
            camera_metadata_tag_t tag,
            camera_metadata_entry_t *entry);
        std::vector<std::shared_ptr<camera3_stream_t>> GetConfigedStreams(uint32_t camId) const;
        camera3_capture_request_t * GetHalRequest(uint32_t camId) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetIntervalForSnapshot
        ///
        /// @brief  Get snapshot interval based on number of snaps, total number of frames/duration of the test
        ///
        /// @param  nSnaps  number of Snapshots requested
        ///
        /// @return interval between the snapshots
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int GetIntervalForSnapshot(int nSnaps);

        int GetLastProcessdFrameNumber(uint32_t camId);
        std::string GetMasterCameraOfLogical(uint32_t camId) const;
        Size GetMaxSize(uint32_t camId, int format, camera_info* camInfo, bool usePixelArraySize = false) const;
        Size GetMaxResolution( uint32_t camId, int format, camera_info* info) const;

        std::string GetSlaveCameraOfLogical(uint32_t camId) const;

        void FlushStream(uint32_t camId);
        void OpenCamera(uint32_t camId);
        void SendContinuousPCR(
            uint32_t camId, std::vector<Cam3Device::streamRequestType> streamRequestType, int outputStreamIndex,
            int totalFrames = HalJsonParser::sFrames, int frameNumber = 1);
        void SetQueriedMaxResolution(
            uint32_t camId,               //[in] camera for which to query max resolution for
            StreamItem streams[],    //[in] streams that will have their resolution checked
            int numStreams);         //[in] number of streams to in stream array
        void SetKey(uint32_t camId, uint32_t tag, const int32_t * value, size_t dataCount);
        void SetKey(uint32_t camId, uint32_t tag, const uint8_t * value, size_t dataCount);
        void SetKey(uint32_t camId, uint32_t tag, const float * value, size_t dataCount);
        void SetKey(uint32_t camId, uint32_t tag, const int64_t * value, size_t dataCount);
        void SetKey(uint32_t camId, uint32_t tag, const double * value, size_t dataCount);
        void SetKey(uint32_t camId, uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount);
        void SetMFNROnStreamRequest(int camId, Cam3Device::streamRequestType &streamReq, int startFrame, int endFrame,
            int skipFrame = -1, int sendFrame = -1, bool setPhysCamSettings = false);
        void SetDepthTest(uint32_t camId, bool bDepthTest);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// UpdateReconfigureTriggerFrames
        ///
        /// @brief  Inserts given framenumber to mReconfigureFramesMap
        ///
        /// @param  camId       Camera ID
        /// @param  frameNum    frame Number
        ///
        /// @return None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void UpdateReconfigureTriggerFrames(UINT32 camId, UINT32 frameNum);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// UpdateMetadataPerRequest()
        ///
        /// @brief  Updates Metadata per request
        ///
        /// @param  metadataMap     Metadata map to be updated
        ///
        /// @return void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void UpdateMetadataPerFrame(std::map<int, std::vector<Cam3Device::setMetadataTag>> &metadataMap);

    private:
        std::map<int, std::unique_ptr<Cam3Device>> c3dMap;
        bool mbDepthTest = false;
    };
}

#endif  //#ifndef __HAL_BASE_TEST__
