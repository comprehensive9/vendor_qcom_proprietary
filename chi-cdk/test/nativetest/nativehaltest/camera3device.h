//******************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA3_DEVICE__
#define __CAMERA3_DEVICE__

#include "camera3module.h"
#include "camera3metadata.h"
#include <mutex>
#include <thread>
#include <map>
#include <queue>

#define FRAME_REQUEST_MOD(frame) (((frame) - 1) % MAX_REQUESTS)

namespace haltests {

    static inline callbacks_process_capture_result_t aprocess_capture_result;
    static inline callbacks_notify_t anotify;
    static const uint32_t MAX_REQUESTS = 8;

class Camera3Device : public Camera3Module,
                      public Camera3Metadata,
                      public camera3_callback_ops_t
{
    public:
        Camera3Device();
        virtual ~Camera3Device();

        //Error codes found in Linux system
        enum ErrorStatusCodes
        {
            ERRNODEV=19,
            ERRPERM=1,
            ERRINVAL=22,
            ERRFAULT=14,
            ERRNOMEM=12,
            ERRTIME=62,
            ERRNODATA=61,
            ERREXIST=17,
            ERRACCESS=13,
            ERRINPROGRESS=115,
            ERRAGAIN=11,
            ERRBUSY=16,
            ERROVERFLOW=75,
            ERRDOM=33,
            ERRINTR=4,
            ERRNOSYS=38,
            ERRUSERS=87
        };

        //Status codes found from camx/camx/build/built/android64/Debug/libs/inc/camx/camxutils.h
        enum CameraModuleStatusCodes
        {
            CamxResultSuccess = 0,
            CamxResultEBusy = -ERRBUSY,
            CamxResultEFailed = -ERRNODEV,
            CamxResultEInvalidArg = -ERRINVAL,
            CamxResultENoSuch = -ERRINVAL,
            CamxResultEOutOfBounds = -ERRINVAL,
            CamxResultETooManyUsers = -ERRUSERS,
            CamxResultEUnsupported = -ERRPERM,
            CamxResultEInvalidState = -ERRPERM,
            CamxResultEInvalidPointer = -ERRFAULT,
            CamxResultENoMemory = -ERRNOMEM,
            CamxResultETimeout = -ERRTIME,
            CamxResultENoMore = -ERRNODATA,
            CamxResultENeedMore = -ERRPERM,
            CamxResultEExists = -ERREXIST,
            CamxResultEPrivLevel = -ERRACCESS,
            CamxResultEResource = -ERRPERM,
            CamxResultEUnableToLoad = -1,
            CamxResultEInProgress = -ERRINPROGRESS,
            CamxResultETryAgain = -ERRAGAIN,
            CamxResultEReentered = -ERRPERM,
            CamxResultEReadOnly = -ERRPERM,
            CamxResultEOverflow = -ERROVERFLOW,
            CamxResultEOutOfDomain = -ERRDOM,
            CamxResultEInterrupted = -ERRINTR,
            CamxResultEWouldBlock = -ERRPERM,
            CamxResultENotImplemented = -ERRNOSYS
        };

        struct CamDevice
        {
            camera3_device_t * camDevice;
            int                camId;
            CamDevice(camera3_device_t * d = nullptr, int id = 0) : camDevice(d), camId(id) {}
        };

        struct PartialMetadataSet
        {
            uint32_t frame_number;
            uint32_t partial_result;
            Camera3Metadata metadata;
            PartialMetadataSet(uint32_t f, uint32_t p, Camera3Metadata m) : frame_number(f), partial_result(p), metadata(m) {}
        };
        void partialResultsUpdate(int camId, int batchSize);
        void partialResultsReset(int camId, int batchSize);
        PartialMetadataSet* GetLastPartialMetadata();
        // minimal jpeg buffer size: 256KB + blob header
        static const size_t       kMinJpegBufferSize = 256 * 1024 + sizeof(camera3_jpeg_blob);
        static const int          MAX_NUMBER_CAMERAS = 9;
        void ProcessCaptureResult(const camera3_capture_result *result);
        void sNotify(const camera3_notify_msg *msg);

    protected:
        virtual void Setup();
        virtual void Teardown();

        void OpenCamera(int cameraId);
        void CloseCamera(int cameraId);
        Size GetMaxResolution(camera_metadata_t* metadata, int format) const;
        void SetQueriedMaxResolution(int cameraId, StreamItem streams[], int numStreams);
        static Size GetPixelArraySize(camera_metadata_t* metadata);
        bool CheckAvailableCapability(
            int cameraId, camera_metadata_enum_android_request_available_capabilities capability) const;
        bool CheckSupportedInputOutputFormat(int cameraId, int inputFormat, int outputFormat) const;
        bool CheckSupportedResolution(int cameraId, int format, Size resolution,
            int direction = CAMERA3_STREAM_OUTPUT) const;
        Size GetMaxSize(int format, camera_info* info, bool usePixelArraySize = false) const;
        size_t getJpegBufferSize(uint32_t width, uint32_t height, camera_info info) const;

        static int FindCameraMetadataFromTemplate(
            camera_metadata_t* templateRequest,
            camera_metadata_tag_t tag);

        static int GetCameraMetadataEntryByTag(
            camera_metadata_t* metadata,
            camera_metadata_tag_t tag,
            camera_metadata_entry_t *entry);

        const camera_metadata_t* GetCameraMetadataByFrame(uint32_t frameNumber);

        camera3_device_t* GetCamDevice(int index);

        void ClearMetadataMap(int cameraId);

        void IfAEConverged(camera_metadata_t* metadata);
        void IfAFConverged(camera_metadata_t* metadata);
        void IfAWBConverged(camera_metadata_t* metadata);

        uint32_t GetPartialResultCount(int camId) const;
        int GetPipelineDepth() const;

        std::mutex              halRequestMutex;
        std::mutex              resultReadyMutexArray[MAX_NUMBER_CAMERAS];
        std::condition_variable resultReadyCondArray[MAX_NUMBER_CAMERAS];
        bool                    isCaptureResultsReadyArray[MAX_NUMBER_CAMERAS];
        bool                    isAEConverged;
        bool                    isAFConverged;
        bool                    isAWBConverged;
        int                     nOutputBuffersRemaining[MAX_NUMBER_CAMERAS];
        int                     nInputBuffersRemaining[MAX_NUMBER_CAMERAS];
        int                     nPartialResultsRemaining[MAX_NUMBER_CAMERAS];
        std::map<uint32_t, Camera3Metadata> mMetadata[MAX_NUMBER_CAMERAS];

        /* Flush test helpers */
        int mOutputBufferStatus[MAX_REQUESTS];
        bool IsFlushTestFrame() const;
        void SetFlushTestFrame();
        void ClearFlushTestFrame();
        void ClearErrorNotifyFlag(uint32_t frameNum);
        int CheckErrorNotifyFlag(uint32_t frameNum, uint32_t pos) const;
        void SetDepthTest(bool bDepthTest);

        int GetCamIdFromFrameNumber(uint32_t frameNumber);

    private:
        void AddMetadatatoMap(const camera3_capture_result* resultMeta);
        void SetNumPartialResults(int camId);
        void SetPipelineDepth(int camId);
        std::vector<CamDevice> mCamDevices; // a list of opened cameras
        uint32_t        mDeviceVersion;
        bool            mHasInputBufferInRequest;
        uint32_t        mPartialResultCount[MAX_NUMBER_CAMERAS];
        bool            mHasResult;
        uint8_t         mPipelineDepth = 1;
        bool            isFlushTestFrame = false;
        bool            mbDepthTest = false;

        /* Partial metadata helpers */
        std::queue<PartialMetadataSet*> m_partialMetadataQueue;

        /* Flush test helpers */
        uint32_t mErrorNotifyBitFlag[MAX_REQUESTS];                 // bits to encode which notify errors were received
        void SetErrorNotifyFlag(uint32_t frameNum, uint32_t pos);   // set a bit of the error notify flag

        const char* mErrorNotifyStrings[5] =
        {
            "ERROR_UNKNOWN",
            "ERROR_DEVICE",
            "ERROR_REQUEST",
            "ERROR_RESULT",
            "ERROR_BUFFER"
        };
};


}  // namespace haltests


#endif  //#ifndef __CAMERA3_DEVICE__
