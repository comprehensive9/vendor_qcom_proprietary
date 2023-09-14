//*************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************
#ifndef __CAMERA3_STREAM__
#define __CAMERA3_STREAM__

#include "camera3device.h"
#include <csetjmp>
#include <csignal>
#ifndef _LINUX
#include "camera3buffer.h"
#endif

namespace haltests {

static const uint32_t   MAX_REPEAT_NUM = 50;

// Gralloc usage flags
enum GRALLOC_USAGE_FLAGS
{
    GRALLOC_USAGE_HW_IMAGE_ENCODER = 1ULL << 27
};

class Camera3Stream : public Camera3Device
{

    public:
        Camera3Stream();
        ~Camera3Stream();

#ifdef _LINUX
        int64_t CAPTURE_RESULT_TIMEOUT = 1;
#else
        int64_t CAPTURE_RESULT_TIMEOUT = 50; //Need higher timeout on presil
#endif //_LINUX
        int MAX_WAIT_RETRIES = 10;

    protected:

        virtual void Setup();
        virtual void Teardown();

        virtual void StartStreaming(
            int cameraId,
            uint32_t frameNumber,
            bool bDumpOutputBuffer = false,
            int outputStreamIndex = 0,
            const char * fileName = nullptr);

        CDKResult StreamStarter(int cameraId, uint32_t frameNumber, bool bDumpOutputBuffer,
            int outputStreamIndex, const char * fileName);

        void CreateStream(
            int streamType,
            Size resolution,
            uint32_t usage,
            int format,
            camera3_stream_rotation_t rotation,
            int dataspace = -1);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ConfigStream
        ///
        /// @brief  Configures the stream for camera session
        ///
        /// @param  opmode                  operation mode for the session
        /// @param  cameraId                camera Id
        /// @param  setResourceCostCheck    flag to indicate bypass resource check session parameter
        ///
        /// @return nt return code of config_stream function call
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int ConfigStream(uint32_t opmode, int cameraId, bool setResourceCostCheck = false);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ConfigStreamSingle
        ///
        /// @brief  Configures the single stream
        ///
        /// @param  isConstrainedHighSpeedConfiguration     flag for HFR session
        /// @param  streamIndex                             stream index
        /// @param  cameraId                                camera Id
        /// @param  generateBuffers                         flag to generate buffers
        /// @param  setResourceCheck                        flag to indicate bypass resource check session parameter
        ///
        /// @return nt return code of config_stream function call
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int ConfigStreamSingle(bool isConstrainedHighSpeedConfiguration, int streamIndex, int cameraId, bool generateBuffers,
            bool setResourceCostCheck = false);

        camera3_stream_buffer_t* GetBufferForRequest(
            int streamIndex,
            int frameNumber) const;

        bool ProcessCaptureRequest(
            int cameraId,
            uint32_t frameNumber,
            int requestType,
            int outputStreamIndex,
            bool bDumpOutputBuffer,
            const char* fileName,
            bool bStartStream);

        int ProcessCaptureRequestWith3AConverged(
            int cameraId,
            uint32_t frameNumber,
            int requestType,
            int outputStreamIndex,
            bool bDumpOutputBuffer,
            const char * fileName);

        void FlushStream(int cameraId);

        int TestFlush(int cameraId);

        void CreateMultiStreams(
            Size outResolutions[],
            int outputFormats[],
            int numStreams,
            uint32_t usageFlags[],
            int streamTypes[]);

        void CreateCaptureRequestWithMultBuffers(
            int cameraId,
            int streamStartIndex,
            int requestType,
            int num_streams,
            int frameNumber);

        void SetHighSpeedSession();
        void SetPreviewFPSforHFR(int32_t fpsMin, int32_t fpsMax);
        bool IsHighSpeedSession() const;
        void ClearHighSpeedSession();
        void FillNV12Buffer(int streamIndex, int frameNumber);
        void DumpBuffer(int streamIndex, int frameNumber, const char* fileName) const;
        static int GetJpegTrimmedBufferSize(char* outputBufferPtr, int maxSize);
        std::vector<std::shared_ptr<camera3_stream_t>> GetConfigedStreams() const;
        void ClearOutStreams();
        bool SetMFNRMode(int cameraId);

#ifdef _LINUX
        void CreateOutputStreamBuffer(camera3_stream_buffer_t &outputBuffer,
            int outputStreamIndex, // stream to use for the initial capture from sensor
            Size outputResolution,  // resolution of the initial capture from sensor
            int outputFormat,// format for the initial capture from sensor
            sp<GraphicBuffer> *outputNativeBuffer,
            int cameraId);

#else
        void CreateOutputStreamBuffer(
            camera3_stream_buffer_t &outputBuffer,
            int outputStreamIndex, // stream to use for the initial capture from sensor
            Size outputResolution,  // resolution of the initial capture from sensor
            int outputFormat, // format for the initial capture from sensor
            std::shared_ptr<CamxBuffer> *outputNativeBuffer,
            int cameraId);

#endif
        int LoadImageToBuffer(
            const char * imageFile,
            int streamIndex,
            int frameNumber);

        camera3_capture_request_t * GetHalRequest(int cameraId) const { return halRequest[cameraId]; };

        void InitHalRequest(
            int cameraId,
            int frameNumber,
            camera3_stream_buffer_t * inputBuffer,
            camera3_stream_buffer_t * outputBuffer,
            int num_output_buffers,
            int templateType);

        void DeleteHalRequest(int cameraId);

        void SetKey(int cameraId, uint32_t tag, const int32_t * value, size_t dataCount);
        void SetKey(int cameraId, uint32_t tag, const uint8_t * value, size_t dataCount);
        void SetKey(int cameraId, uint32_t tag, const float * value, size_t dataCount);
        void SetKey(int cameraId, uint32_t tag, const int64_t * value, size_t dataCount);
        void SetKey(int cameraId, uint32_t tag, const double * value, size_t dataCount);
        void SetKey(int cameraId, uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount);
        void UpdateSessionParameters(
            uint32_t tag,       //[in] tag to update data for
            const void* value,  //[in] pointer to data to write to tag
            size_t dataCount);  //[in] number of data pieces to write
        bool CheckKeySetInResult(uint32_t tag, const uint8_t * value, uint32_t frameNum);
        bool CheckKeySetInResult(uint32_t tag, const int32_t * value, float errorMarginRate, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const float * value, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const int64_t * value, float errorMarginRate, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const double * value, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const camera_metadata_rational_t * value, int frameNum);

    private:
        camera3_capture_request_t * halRequest[MAX_NUMBER_CAMERAS];
        camera3_stream_configuration config;
        std::vector<std::shared_ptr<camera3_stream_t>> mStreams;
        /*
        * Allocated streams is required to hold the address of all camera3_stream
        * objects created in order to free the memory later, In HFR when a previous stream is
        * deleted and a new stream is configured for next resolution and fps range,
        * process_capture_result fails hence preventing from freeing the memory space.
        * This vector will hold all the allocated memory which is cleared before camera is closed.
        */
        std::vector<std::shared_ptr<camera3_stream_t>> allocatedStreamsforHFR;
        Size GetGrallocSize (Size resolution, int format, int cameraId);
        bool CheckType(uint32_t tag, uint8_t expectedType);
        void UpdateKey(int cameraId, uint32_t tag, const void* data, size_t dataCount);
        int CreateOutputBuffers(int cameraId);
        void ClearOutputBuffers();

        bool isHighspeedSession = false;
        int32_t previewFpsRange[2];

#ifdef _LINUX
        sp<GraphicBuffer>** m_ppOutNativeBuffers;
#else
        std::shared_ptr <CamxBuffer>** m_ppOutNativeBuffers;
#endif
        camera3_stream_buffer_t**  m_ppOutBuffers;

        camera3_stream_buffer_t* m_pOutBuffersPerRequest;
};

}  // namespace haltests

#endif  // __CAMERA3_STREAM__
