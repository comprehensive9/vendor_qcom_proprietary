//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAM3_DEVICE__
#define __CAM3_DEVICE__

#include "camera3supermodule.h"
#include "camera3metadata.h"
#include "staticmetadata.h"
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <queue>
#include <set>

#define REQUEST_FRAME_MOD(frame) (((frame) - 1) % MAX_FRAME_REQUESTS)

enum IntervalType
{
    MULTIPLE,
    NOINTERVAL,
    NHX
};
namespace haltests {

    static const uint32_t MAX_FRAME_REQUESTS = 8;
    static const int      MAX_NUMBER_CAMERAS = 9;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// aprocess_capture_result
    /// @brief
    ///      Static callback forwarding method from HAL to instance which is called
    ///      when a capture result is ready
    /// @return
    ///    None
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline callbacks_process_capture_result_t aprocess_capture_result;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anotify
    /// @brief
    ///      Static callback forwarding method from HAL to instance which provides
    ///      message notification on events
    /// @return
    ///    None
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline callbacks_notify_t anotify;

    class Cam3Device : public Camera3SuperModule, public Camera3Metadata, public camera3_callback_ops_t
    {
    public:
        Cam3Device(uint32_t cameraId);
        virtual ~Cam3Device();

        //Error codes found in Linux system
        enum ErrorStatusCodes
        {
            ERRNODEV = 19,
            ERRPERM = 1,
            ERRINVAL = 22,
            ERRFAULT = 14,
            ERRNOMEM = 12,
            ERRTIME = 62,
            ERRNODATA = 61,
            ERREXIST = 17,
            ERRACCESS = 13,
            ERRINPROGRESS = 115,
            ERRAGAIN = 11,
            ERRBUSY = 16,
            ERROVERFLOW = 75,
            ERRDOM = 33,
            ERRINTR = 4,
            ERRNOSYS = 38,
            ERRUSERS = 87
        };

        // TODO: Need to directly use camxutils enums rather than duplicating enums
        // Status codes found from camx/camx/build/built/android64/Debug/libs/inc/camx/camxutils.h
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

        enum PCRStatus {
            DEFAULT,
            REQUEST_SENT,
            RESULTS_RECEIVED,
            FLUSHED
        };

        enum FeatureVendorTags {
            VENDOR_NOISE_REDUCTION,
            VENDOR_STATS_EXPOSURECOUNT
        };

        // flush related
        std::map<UINT32, PCRStatus> mReconfigureFramesMap;
        std::set<UINT32> mFlushFramesSet = {};

        struct outputBufferInfo
        {
#ifdef _LINUX
            sp<GraphicBuffer>* outNativeBuffer;
#else
            std::shared_ptr <CamxBuffer>* outNativeBuffer;
#endif
        };

        struct PartialMetadataSet
        {
            uint32_t partial_result;
            Camera3Metadata metadata;
            PartialMetadataSet(uint32_t p, Camera3Metadata m) : partial_result(p), metadata(m) {}
        };

        typedef struct RequestTracker {
            std::queue<PartialMetadataSet *> metadataQueue; //Used to keep track of partial metadata returned from driver
            int nOutputBuffers;
            int nInputBuffersRemaining;
            int nOutputBuffersRemaining;
            int nPartialResultsRemaining;
            bool isAEConverged;
            bool isAFConverged;
            bool isAWBConverged;
            std::string masterCameraOfLogical = "";
            std::map<std::string, int> phyCamBufferStatusMap;
            bool zoom = false;
            std::vector<bool> dumpBufferList;
            PCRStatus resultStatus = DEFAULT;
            std::vector<std::string> fileNameList;
            RequestTracker(int nob,
                int ib,
                int ob,
                int pr) : nOutputBuffers(nob),
                nInputBuffersRemaining(ib),
                nOutputBuffersRemaining(ob),
                nPartialResultsRemaining(pr) {}
        }RequestTracker_t;

        struct setMetadataTag
        {
            uint32_t tag;
            void *data;
            size_t dataCount;
            std::string metaFileName = "";
            bool setPhysCamSettings = false;
            int physCam = -1;
        };

        struct verifyMetadataTag
        {
            uint32_t tag;
            void *data;
            size_t dataCount;
            bool stopPcr = false;
            std::string scenario = "";
            float errorMarginRate = 0.0;
        };

        typedef struct streamRequestType
        {
            int requestType;
            std::vector<int> formatList;
            int skipFrame;
            int sendFrame;
            IntervalType intervalType = NOINTERVAL;
            int numStreams;
            std::vector<std::string> fileNameList;
            bool enableZSL = FALSE;
            std::vector<bool> dumpBufferList;
            std::vector<std::string> physCamIdsList;
            std::map<int, std::vector<setMetadataTag>> setMap;
            std::map<int, std::vector<verifyMetadataTag>> verifyMap;
            std::vector<int> streamIndicesToRequest;
            bool zoom = false;
        }streamRequestType_t;

        // map to track send intervals per stream
        std::map<int, std::vector<int>> streamToPCRSendMap;

        // minimal jpeg buffer size: 256KB + blob header
        static const size_t kMinJpegBufferSize = 256 * 1024 + sizeof(camera3_jpeg_blob);
        std::mutex                outputBufferMutex;
#ifdef _LINUX
        int64_t             CAPTURE_RESULT_TIMEOUT = 1;
#else
        int64_t             CAPTURE_RESULT_TIMEOUT = 50; //Need higher timeout on presil
#endif //_LINUX
        int                 MAX_WAIT_RETRIES = 10;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetLastPartialMetadata
        /// @brief
        ///    Gets the oldest partial metadata not yet analyzed. Caller is
        ///    responsible for deleting metadata and struct once done.
        /// @return
        ///    PartialMetadataSet*
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        PartialMetadataSet* GetLastPartialMetadata();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  Cam3Device::getJpegBufferSize
        ///  @brief
        ///     Gets the jpeg buffer size based on max jpeg resolution
        ///  @return
        ///     jpeg buffer size
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        size_t getJpegBufferSize(uint32_t width, uint32_t height) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetCameraMetadataByFrame
        ///  @brief
        ///     Gets metadata of the requested frame from metadata map
        ///  @return
        ///     copy of camera_metadata_t* of desired frame, nullptr if metadata
        ///     not found
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        camera_metadata_t* GetCameraMetadataByFrame(uint32_t frameNumber) const;

        virtual void Setup();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// OpenCamera()
        /// @brief
        ///     Opens the camera device for a given camerId
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void OpenCamera();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// CloseCamera()
        ///
        /// @brief
        ///     close the camera device for a given camerId
        ///
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void CloseCamera();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ProcessCaptureResult()
        ///
        /// @brief
        ///     process results metadata/output buffers returned and invoked by the driver
        ///
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void ProcessCaptureResult(const camera3_capture_result *result);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// sNotify()
        ///
        /// @brief
        ///     Notification callback invoked by the driver to specify
        ///     any device sepcific message
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void sNotify(const camera3_notify_msg *msg);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetCameraId()
        ///
        /// @brief
        ///     returns camera ID
        ///
        /// @return
        ///     uint32_t for camera ID
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint32_t GetCameraId() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// AreCameraStreamsEqual()
        ///
        /// @brief
        ///   returns if the camera streams are equal
        ///
        /// @return
        ///   bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool AreCameraStreamsEqual(camera3_stream_t &lhs, camera3_stream_t &rhs);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// AreFramesProcessed()
        ///
        /// @brief
        ///     returns false if frames are not processed for all frame numbers
        ///
        /// @return
        ///     bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool AreFramesProcessed();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetMasterCameraOfLogical()
        ///
        /// @brief
        ///     returns master camera of logical camera fetched from ANDROID_LOGICAL_MULTI_CAMERA_ACTIVE_PHYSICAL_ID tag
        ///
        /// @return
        ///     string for master camera ID
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string GetMasterCameraOfLogical() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetSlaveCameraOfLogical()
        ///
        /// @brief
        ///     returns slave camera of logical camera fetched from com.qti.chi.multicamerainfo.ActiveCameraInfo tag
        ///
        /// @return
        ///     string for slave camera ID
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string GetSlaveCameraOfLogical() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetCamDevice
        ///  @brief
        ///      Gets the camera device object based on cameraId
        ///  @return
        ///      camera3_device_t object for given cameraId
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        camera3_device_t* GetCamDevice() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetPartialResultCount
        ///  @brief
        ///      Get the partial result count based on cameraId
        ///  @return
        ///      uint32_t partial result count
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint32_t GetPartialResultCount() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetPipelineDepth
        ///  @brief
        ///      Get the pipeline depth
        ///  @return
        ///      int pipeline depth
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int GetPipelineDepth() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetRequestByFrameNumber
        ///  @brief
        ///      Get the request from PCRTracker by frame number
        ///  @return
        ///      RequestTracker_t pointer to request in PCRTracker
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        RequestTracker_t* GetRequestByFrameNumber(int frameNumber) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetIfAWBConverged
        ///  @brief
        ///      Get isAWBConverged value from PCRTracker
        ///  @return
        ///      bool isAWBConverged
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool GetIfAWBConverged(int frameNumber) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetIfAFConverged
        ///  @brief
        ///      Get isAFConverged value from PCRTracker
        ///  @return
        ///      bool isAFConverged
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool GetIfAFConverged(int frameNumber) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetIfAEConverged
        ///  @brief
        ///      Get isAEConverged value from PCRTracker
        ///  @return
        ///      bool isAEConverged
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool GetIfAEConverged(int frameNumber) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetUnprocessedFramenumbers
        ///  @brief
        ///      Returns a list of frame numbers that don't have all results received from driver
        ///  @return
        ///      std::vector<int>
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> GetUnprocessedFramenumbers();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   FindCameraMetadatafromTemplate
        ///
        ///   @brief
        ///     Finds a specific medadata based on the given tag and
        ///     template
        ///   @return
        ///      None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int FindCameraMetadataFromTemplate(camera_metadata_t* templateRequest, camera_metadata_tag_t tag) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   GetCameraMetadataEntryByTag
        ///
        ///   @brief
        ///     Gets a specific medadata entry based on the given tag
        ///   @return
        ///     zero if found successfully, non zero otherwise
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int GetCameraMetadataEntryByTag(camera_metadata_t* metadata, camera_metadata_tag_t tag,
            camera_metadata_entry_t *entry) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  GetMaxSize
        ///  @brief
        ///     Gets the max resolution for a given image format
        ///  @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Size GetMaxSize(int format, camera_info* camInfo, bool usePixelArraySize = false) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// CheckSupportedResolution
        /// @brief
        ///     Check if a given format and resolution is supported
        /// @return
        ///    True if supported else false
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool CheckSupportedResolution(int format, Size resolution, int direction = CAMERA3_STREAM_OUTPUT) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   SetQueriedMaxResolution()
        ///
        ///   @brief
        ///       When stream resolution is MAX_RES/DEPTH_RES or the format is RAW, this helper function will
        ///       query for the maximum resolution and replace the resolution values stored in the
        ///       ReprocessItems.
        ///
        ///   @return
        ///        void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetQueriedMaxResolution(int cameraId, StreamItem streams[], int numStreams);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// CheckAvailableCapability
        /// @brief
        ///     Checks if given android capability is supported
        /// @return
        ///    True if supported else false
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool CheckAvailableCapability(int cameraId, camera_metadata_enum_android_request_available_capabilities capability) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   Cam3Device::SetDepthTest()
        ///
        ///   @brief
        ///       Sets the depth test flag
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetDepthTest(bool bDepthTest);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// SetActivePhysCamCheck
        ///
        /// @brief  sets flag to fetch active physical camera for logical multicamera
        ///
        /// @param  camInfo     Camera Information pointer
        ///
        /// @return vector containing physical cameras
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetActivePhysCamCheck(bool getActivePhysCam);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   Cam3Device::SetStopPcrFlag()
        ///
        ///   @brief
        ///       set stop PCR flag for PCR Async loop to read
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetStopPcrFlag(bool stopPcr);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   Cam3Device::CreateStream()
        ///
        ///   @brief
        ///       Helper function to create camera3_stream_t object which is used to configure the stream
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void CreateStream(int streamType, Size resolution, uint32_t usage, int format, camera3_stream_rotation_t rotation,
            int dataspace = -1, std::string physicalCameraId = "");

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ConfigStream
        ///
        /// @brief  Configures the camera streams for a given use case
        ///
        /// @param  opmode              operation mode of the session
        /// @param  mc                  pointer to corresponding multicamera device
        /// @param  setResourceCheck    flag to indicate bypass resource check session parameter
        /// @param  enableRTB           flag to indicate RTB enablement
        ///
        /// @return nt return code of config_stream function call
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int ConfigStream(uint32_t opmode, bool setResourceCostCheck = false, bool enableRTB = false);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ConfigStreamSingle
        ///
        /// @brief  Configures the single stream
        ///
        /// @param  isConstrainedHighSpeedConfiguration     flag for HFR session
        /// @param  streamIndex                             stream index
        /// @param  mc                                      pointer to corresponding multicamera device
        /// @param  generateBuffers                         flag to generate buffers
        /// @param  setResourceCheck                        flag to indicate bypass resource check session parameter
        /// @param  enableRTB                               flag to indicate RTB
        ///
        /// @return nt return code of config_stream function call
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int ConfigStreamSingle(bool isConstrainedHighSpeedConfiguration, int streamIndex, bool generateBuffers,
            bool setResourceCostCheck = false, bool enableRTB = false);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   CreateMultiStreams()
        ///
        ///   @brief
        ///       Helper function to create multiple stream objects
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void CreateMultiStreams(Size outResolutions[], int outputFormats[], int numStreams, uint32_t usageFlags[],
            int streamTypes[]);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   GetConfigedStreams()
        ///
        ///   @brief
        ///       Returns a vector of camera3_stream_t objects
        ///
        ///   @return
        ///       vector of camera3_stream_t objects
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::shared_ptr<camera3_stream_t>> GetConfigedStreams() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   GetLastProcessedFrameNumber()
        ///
        ///   @brief
        ///       Returns last processed frame number from PCRTracker
        ///
        ///   @return
        ///       int last processed frame number
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int GetLastProcessedFrameNumber();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   GetHalRequest()
        ///
        ///   @brief
        ///       Returns hal request object
        ///
        ///   @return
        ///       camera3_capture_request_t object
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        camera3_capture_request_t * GetHalRequest() const { return halRequest; };

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   ProcessCaptureRequestAsync()
        ///
        ///   @brief
        ///       create and send multiple capture requests to the driver
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void ProcessCaptureRequestAsync(std::vector<streamRequestType> streamRequestTypeList, int outputStreamIndex,
            int totalFrames = HalJsonParser::sFrames, int frameNumber = 1);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   SetKey()
        ///
        ///   @brief
        ///       Helper function to set data for a given tag and camera. A HAL request must exist for this function to succeed.
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetKey(uint32_t tag, const int32_t * value, size_t dataCount);
        void SetKey(uint32_t tag, const uint8_t * value, size_t dataCount);
        void SetKey(uint32_t tag, const float * value, size_t dataCount);
        void SetKey(uint32_t tag, const int64_t * value, size_t dataCount);
        void SetKey(uint32_t tag, const double * value, size_t dataCount);
        void SetKey(uint32_t tag, const camera_metadata_rational_t * value, size_t dataCount);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   ClearOutStreams()
        ///
        ///   @brief
        ///       clear out streams that were created as a part of CreateStream()
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void ClearOutStreams();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   Cam3Device::FlushStream()
        ///
        ///   @brief
        ///       Flushes the pending requests for given camera id
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void FlushStream();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetTagID
        ///
        /// @brief  Gets the Vendor Tag ID based feature Vendor Tag
        ///
        /// @param  featVendorTag   Feature Vendor Tag
        ///
        /// @return Tag ID
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 GetTagID(UINT32 featVendorTag);

    protected:
        bool isAEConverged;
        bool isAFConverged;
        bool isAWBConverged;
        int  nOutputBuffersRemaining[MAX_NUMBER_CAMERAS];
        int  nInputBuffersRemaining[MAX_NUMBER_CAMERAS];
        int  nPartialResultsRemaining[MAX_NUMBER_CAMERAS];
        /* Flush test helpers */
        int   mOutputBufferStatus[MAX_FRAME_REQUESTS];
        bool  stopPcrFlag = FALSE;
        bool  PcrSuccess = TRUE;
        bool  isLastPCRKeyFound = false;
        std::map<uint32_t, Camera3Metadata> mMetadata;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ClearErrorNotifyFlag()
        ///
        /// @brief
        ///     Clears the error notify flag bits for given frame number
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void ClearErrorNotifyFlag(uint32_t frameNum);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// CheckErrorNotifyFlag()
        ///
        /// @brief
        ///     Gets the desired bit from the error notify bit flag
        /// @return
        ///     int bit of the position requested, otherwise -1
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int CheckErrorNotifyFlag(uint32_t frameNum, uint32_t pos) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   TestFlush()
        ///
        ///   @brief
        ///       Flushes the pending requests for given camera id and measures the flush time
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int TestFlush();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   CreateCaptureRequestWithMultBuffers()
        ///
        ///   @brief
        ///       Helper function to create capture request with multiple buffers
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void CreateCaptureRequestWithMultBuffers(int streamStartIndex, int requestType, int num_streams, int frameNumber,
            std::vector<int> streamIndicesToRequest);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   SetHighSpeedSession()
        ///
        ///   @brief
        ///       Sets the boolean to indicate high speed capture session
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetHighSpeedSession();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   SetPreviewFPSforHFR()
        ///
        ///   @brief
        ///       Sets the FPS range for preview only mode in High speed session
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetPreviewFPSforHFR(int32_t fpsMin, int32_t fpsMax);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   IsHighSpeedSession()
        ///
        ///   @brief
        ///       Returns the boolean to indicate high speed capture session
        ///
        ///   @return
        ///       bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool IsHighSpeedSession() const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   ClearHighSpeedSession()
        ///
        ///   @brief
        ///       Clears the boolean to indicate end of high speed capture session
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void ClearHighSpeedSession();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   FillNV12Buffer()
        ///
        ///   @brief
        ///       Fill an input buffer with a four color test image
        ///
        ///   @return
        ///        void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void FillNV12Buffer(buffer_handle_t *bufferHandle, camera3_stream_t *stream);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   DumpBuffer()
        ///
        ///   @brief
        ///       Dump content from output buffer to an image file
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void DumpBuffer(camera3_stream_t *stream, buffer_handle_t *bufferHandle, int frameNumber, const char* fileName) const;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   GetJpegTrimmedBufferSize()
        ///
        ///   @brief
        ///       Returns the actual size of data in buffer after finding end of JPEG data (0xFF,0xD9)
        ///
        ///   @return
        ///        void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static int GetJpegTrimmedBufferSize(char* outputBufferPtr, int maxSize);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   CreateOutputStreamBuffer()
        ///
        ///   @brief
        ///       Creates an output stream buffer
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _LINUX
        void CreateOutputStreamBuffer(camera3_stream_buffer_t &outputBuffer, camera3_stream_t* outputStream,
            Size outputResolution, int outputFormat, sp<GraphicBuffer> *outputNativeBuffer);
#else
        void CreateOutputStreamBuffer(camera3_stream_buffer_t &outputBuffer, camera3_stream_t* outputStream,
            Size outputResolution, int outputFormat, std::shared_ptr<CamxBuffer>* outputNativeBuffer);
#endif
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   LoadImageToBuffer()
        ///
        ///   @brief
        ///       Loads specified image file to native buffer
        ///
        ///   @return
        ///        0 on success
        ///       -1 on failure
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int LoadImageToBuffer(const char * imageFile, buffer_handle_t *bufferHandle, camera3_stream_t *stream);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   InitHalRequest()
        ///
        ///   @brief
        ///       Sets up HAL request object for submitting to process
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void InitHalRequest(int frameNumber, camera3_stream_buffer_t * inputBuffer, camera3_stream_buffer_t * outputBuffer,
            int num_output_buffers, int templateType);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   DeleteHalRequest()
        ///
        ///   @brief
        ///       Deletes an existing HAL request for the given camera
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void DeleteHalRequest();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   UpdateSessionParameters()
        ///
        ///   @brief
        ///       Update metadata in camera3_stream_configuration.session_parameters
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void UpdateSessionParameters(uint32_t tag, const void* value, size_t dataCount);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   CheckKeySetInResult()
        ///
        ///   @brief
        ///       Helper function to check a result frame for an expected tag value.
        ///
        ///   @return
        ///       bool true if result data matches the expected value, false otherwise
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool CheckKeySetInResult(uint32_t tag, const uint8_t * value, uint32_t frameNum);
        bool CheckKeySetInResult(uint32_t tag, const int32_t * value, float errorMarginRate, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const float * value, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const int64_t * value, float errorMarginRate, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const double * value, int frameNum);
        bool CheckKeySetInResult(uint32_t tag, const camera_metadata_rational_t * value, int frameNum);

    private:
        const char* mErrorNotifyStrings[5] =
        {
            "ERROR_UNKNOWN",
            "ERROR_DEVICE",
            "ERROR_REQUEST",
            "ERROR_RESULT",
            "ERROR_BUFFER"
        };

        // basic camera related
        bool                            isHighspeedSession    = false;
        camera_info*                    mCamInfo;
        camera3_capture_request_t*      halRequest;
        camera3_device_t*               mCamDevice;
        camera3_stream_configuration    config;
        int32_t                         previewFpsRange[2];
        std::string                     masterCameraOfLogical;
        std::string                     slaveCameraOfPhysical;
        uint32_t                        mCamId;
        uint32_t                        mDeviceVersion;
        uint32_t                        mPartialResultCount;
        uint8_t                         mPipelineDepth = 1;

        // depth camera related
        bool                            mbDepthTest = false;

        // multi camera related
        bool                            mGetActivePhysCam = false;
        bool                            mGetSlavePhysCam = false;

        // framework related
        int               lastProcessedFrameNumber;
        std::mutex        dumpThreadMutex;
        std::mutex        halRequestMutex;
        std::mutex        nativeBufferMutex;
        std::mutex        resThreadMutex;

        // The only address returned in the process_capture_result that matches with the request buffers
        // is found to be the buffer handle. The buffer handle is used for dumping buffer to an image
        std::map<camera3_stream_t*, std::map<buffer_handle_t*, outputBufferInfo>> nativeBuffers;
        std::map<int, std::unique_ptr<RequestTracker_t>> PCRTracker;
        std::vector<std::shared_ptr<camera3_stream_t>> mStreams;
        std::queue<camera3_capture_request_t *>        PCRQueue;
        /* Partial metadata helpers */
        std::queue<PartialMetadataSet*>                m_partialMetadataQueue;

        /* Flush test helpers */
        uint32_t  mErrorNotifyBitFlag[MAX_FRAME_REQUESTS]; // bits to encode which notify errors were received
        const int DEFAULT_WAIT_TIME = 1000; //in milliseconds

        // Vendor Tags Related
        std::map<UINT32, UINT32>mTagIdMap;

        /*
        * Allocated streams is required to hold the address of all camera3_stream
        * objects created in order to free the memory later, In HFR when a previous stream is
        * deleted and a new stream is configured for next resolution and fps range,
        * process_capture_result fails hence preventing from freeing the memory space.
        * This vector will hold all the allocated memory which is cleared before camera is closed.
        */
        std::vector<std::shared_ptr<camera3_stream_t>> allocatedStreamsforHFR;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  AddMetadatatoMap
        ///  @brief
        ///     Adds metadata from the given frame into the map
        ///  @param
        ///     Result metadata for the received frame
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void AddMetadatatoMap(const camera3_capture_result* resultMeta);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  SetNumPartialResults
        ///  @brief
        ///      Set the partial result count for given cameraId
        ///  @return
        ///      None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetNumPartialResults();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  SetPipelineDepth
        ///  @brief
        ///      Set the pipeline depth count for given cameraId
        ///  @return
        ///      None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetPipelineDepth();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// SetErrorNotifyFlag()
        ///
        /// @brief
        ///     Gets the desired bit from the error notify bit flag
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetErrorNotifyFlag(uint32_t frameNum, uint32_t pos);   // set a bit of the error notify flag

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ClearMetadataQueue()
        ///
        /// @brief
        ///     clear metadataQueue which contains partial metadata objects
        /// @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void ClearMetadataQueue(std::queue<PartialMetadataSet *> metadataQueue);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// DoesBufferSizeExceedPipelineDepth()
        ///
        /// @brief
        ///     returns false if nativeBuffers size for any stream exceeds pipeline depth
        /// @return
        ///     bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool DoesBufferSizeExceedPipelineDepth(int frameNumber);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  IFAWBConverged
        ///  @brief
        ///     Check if 3A state are converged, which means we can do
        ///     still capture or reprocessing
        ///  @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void IfAWBConverged(camera_metadata_t* metadata);  //[in] metadata of current frame

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  IFAFConverged
        ///  @brief
        ///     Check if AF state are converged, which means we can do
        ///     still capture or reprocessing
        ///  @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void IfAFConverged(camera_metadata_t* metadata);  //[in] metadata of current frame

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  IF3AConverged
        ///  @brief
        ///     Check if 3A state are converged, which means we can do
        ///     still capture or reprocessing
        ///  @return
        ///     None
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void IfAEConverged(camera_metadata_t* metadata);  //[in] metadata of current frame

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   GetGrallocSize()
        ///
        ///   @brief
        ///       Determines the gralloc resolution for a given format. Gralloc dimensions may be different due to requirement of the
        ///       format.
        ///
        ///   @return
        ///       Size struct containing the gralloc resolution
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Size GetGrallocSize(Size resolution, int format);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   CheckType()
        ///
        ///   @brief
        ///       Helper function to check that a tag returns metadata of the expected type
        ///
        ///   @return
        ///       bool true if type was as expected, false otherwise
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool CheckType(uint32_t tag, uint8_t expectedType);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   UpdateKey()
        ///
        ///   @brief
        ///       Update key value in the camera metadata. If key is not yet present in the metadata object, add the key and the
        ///       corresponding value.
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void UpdateKey(uint32_t tag, const void* data, size_t dataCount);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   CreateOutputBuffer()
        ///
        ///   @brief
        ///       Helper function to generate an array of buffers for each camera stream
        ///
        ///   @return
        ///       int 0 if successful, -1 otherwise
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        camera3_stream_buffer_t* CreateOutputBuffer(camera3_stream_t* stream);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   DumpOutputBuffers()
        ///
        ///   @brief
        ///       Dumps output buffer returned from the result by the driver and saves it into a file
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void DumpOutputBuffers(buffer_handle_t* bufferHandle, int frameNumber, camera3_stream_t *camStream);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   VerifyResults()
        ///
        ///   @brief
        ///       Verify if all results are returned by the driver for a frame number and saves it into a file
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void VerifyResults(int frameNumber);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   SendProcessCaptureRequest()
        ///
        ///   @brief
        ///       Sends request to the driver through HAL API
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SendProcessCaptureRequest(int frameNumber, int templateType, int numStreams,
            int outputStreamIndex, std::map<int, std::vector<setMetadataTag>> setMap, bool enableZSL,
            std::vector<std::string> fileNameList, std::vector<std::string> physCamIdsList,
            std::vector<int> streamIndicesToRequest, bool zoom, std::vector<bool> dumpBufferList);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   SendProcessCaptureRequest()
        ///
        ///   @brief
        ///       Initiates request threads by frame number
        ///
        ///   @return
        ///       void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SendPCRByRequestType(int frameNumber, std::vector<streamRequestType> requestType,
            int outputStreamIndex);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   PCRStopCondition()
        ///
        ///   @brief
        ///       returns true if stop condition is reached
        ///
        ///   @return
        ///       bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool PCRStopCondition(std::chrono::time_point<std::chrono::steady_clock> endTime, int frameNumber, int totalFrames);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// SetEnableZsl()
        ///
        /// @brief  Prepares the existing HAL request to test ENABLE_ZSL tag. CAPTURE_INTENT must be set to STILL_CAPTURE mode,
        ///         and then the ENABLE_ZSL key should be set.
        ///
        /// @return void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetEnableZsl();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// UpdateFlushFramesSet
        ///
        /// @brief  UpdateFlushFramesSet
        ///
        /// @param  frameNumber frameNumber to start with
        ///
        /// @return void
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void UpdateFlushFramesSet(UINT32 frameNumber);
    };

}  // namespace haltests

#endif  //#ifndef __CAM3_DEVICE__
