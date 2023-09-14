////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chitestcase.h
/// @brief Declarations for chitestcase manager.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CHI_TESTCASE__
#define __CHI_TESTCASE__

#include "nativetest.h"
#include "commonutils.h"
#include "chipipeline.h"
#include "chisession.h"
#include "chibuffermanager.h"
#include "chimetadatautil.h"
#include "chiifedefs.h"
#include <atomic>
#include <queue>
#include <list>

#define FRAME_REQUEST_MOD(frame) (((frame) - 1) % MAX_REQUESTS)
// todo: get max IFEs from driver instead
#define MAX_PHY_CAMERAS 3 // Max IFEs on Lahaina

namespace chitests
{
static const int    MAX_CAMERAS           = MAXIMUM_CAMERAS;
static const int    MAX_SESSIONS          = MAXIMUM_SESSIONS;
static const int    MAX_STREAMS           = MAXIMUM_STREAMS;
static const int    MAX_REQUESTS          = 8; //CamX supports 8 frames to be queued in a pipeline
static const int    START_FRAME           = 1;
static const int    ONE_FRAME_TIME        = 33000;
static const float  DEFAULT_ZOOM          = 1.0;
static const UINT32 DEFAULT_BIT_WIDTH     = 10;
static const UINT32 GAMMA_ENTRIES_PER_LUT = 65; //CamX requires 65 entries in the table as default
static const int    INVALID_CAMID_INT     = -1;
static const UINT32 FIXED_INDEX           = 0; // use this fixed index instead camera ID to force creating single session for all cameras

static const char* IPE_COLORBAR_VGA            = "ipe-unittest-input_full_vga_colorbar-1.yuv";
static const char* FD_FIRST_INPUT_FRAME        = "fd_1.yuv";
static const char* IPE_BARBARA_FHD_UBWCTP10    = "ipe-unittest-input_1_barbara_1920x1080_TP10.ubwc";
static const char* MFHDR_FHD_UBWCTP10_LONGEXP  = "MFHDR_LONGEXP_UBWCTP10_1080P.ubwc";
static const char* MFHDR_FHD_P010_MIDEXP       = "MFHDR_MIDEXP_P010_1080P.p010";
static const char* MFHDR_FHD_UBWCTP10_SHORTEXP = "MFHDR_SHORTEXP_UBWCTP10_1080P.ubwc";
static const char* MFHDR_FHD_DS4_PD10_LONGEXP  = "MFHDR_LONGEXP_PD10_1080P_DS4.PD10";
static const char* MFHDR_FHD_DS4_PD10_MIDEXP   = "MFHDR_MIDEXP_PD10_1080P_DS4.PD10";
static const char* MFHDR_FHD_DS4_PD10_SHORTEXP = "MFHDR_SHORTEXP_PD10_1080P_DS4.PD10";
static const char* BPS_COLORBAR_FHD            = "bps-unittest-input_1080p_colorbar.raw"; //8-bit RAW image
static const char* BPS_MIPI_RAW_10             = "MIPI_RAW_10.raw";                       //10-bit RAW HD image
static const char* BPS_MIPI_RAW_10_4K          = "MIPI_RAW_10_4656x3496.raw";             //10-bit RAW 4K image
static const char* BPS_IDEALRAW_UHD            = "bps-idealraw-input.raw";
static const char* CVP_INPUT_DS4               = "CVP_DS4_IN.PD10";
static const char* IPE_EMULATEDSENSOR_4K       = "emulatedsensor_nv12_4096_2160.yuv";
static const char* BLURMAPINPUT                = "INPUT_10_0_0.yuv";
static const char* BMAP_DS4_INPUT              = "BMAP_11_0_0.yuv";
static const char* BMAP_DS16_INPUT             = "BMAP_11_1_0.yuv";


static const std::string CAMXORFILE("/vendor/etc/camera/camxoverridesettings.txt");


// Node Map (for json parsing)
static const std::map<std::string, UINT> NODES = {
    { "Sensor", CamX::Sensor },
    { "StatsProcessing", CamX::StatsProcessing },
    { "SinkBuffer", CamX::SinkBuffer },
    { "SinkNoBuffer", CamX::SinkNoBuffer },
    { "SourceBuffer", CamX::SourceBuffer },
    { "AutoFocus", CamX::AutoFocus },
    { "JPEGAggregator", CamX::JPEGAggregator },
    { "FDSw", CamX::FDSw },
    { "FDManager", CamX::FDManager },
    { "StatsParse", CamX::StatsParse },
    { "OfflineStats", CamX::OfflineStats },
    { "Torch", CamX::Torch },
    { "AutoWhiteBalance", CamX::AutoWhiteBalance },
    { "HistogramProcess", CamX::HistogramProcess },
    { "Tracker", CamX::Tracker },
    { "IFE", CamX::IFE },
    { "JPEG", CamX::JPEG },
    { "IPE", CamX::IPE },
    { "BPS", CamX::BPS },
    { "FDHw", CamX::FDHw },
    { "LRME", CamX::LRME },
    { "RANSAC", CamX::RANSAC },
    { "CVP", CamX::CVP },
};

//  Buffer Format Map (for json parsing)
static const std::map<std::string, UINT32> BUFFER_FORMAT = {
    { "ChiFormatJpeg", ChiFormatJpeg },
    { "ChiFormatY8", ChiFormatY8 },
    { "ChiFormatY16", ChiFormatY16 },
    { "ChiFormatYUV420NV12", ChiFormatYUV420NV12 },
    { "ChiFormatYUV420NV21", ChiFormatYUV420NV21 },
    { "ChiFormatYUV420NV16", ChiFormatYUV420NV16 },
    { "ChiFormatBlob", ChiFormatBlob },
    { "ChiFormatRawYUV8BIT", ChiFormatRawYUV8BIT },
    { "ChiFormatRawPrivate", ChiFormatRawPrivate },
    { "ChiFormatRawMIPI", ChiFormatRawMIPI },
    { "ChiFormatRawPlain16", ChiFormatRawPlain16 },
    { "ChiFormatRawMeta8BIT", ChiFormatRawMeta8BIT },
    { "ChiFormatUBWCTP10", ChiFormatUBWCTP10 },
    { "ChiFormatUBWCNV12", ChiFormatUBWCNV12 },
    { "ChiFormatUBWCNV124R", ChiFormatUBWCNV124R },
    { "ChiFormatYUV420NV12TP10", ChiFormatYUV420NV12TP10 },
    { "ChiFormatYUV420NV21TP10", ChiFormatYUV420NV21TP10 },
    { "ChiFormatYUV422NV16TP10", ChiFormatYUV422NV16TP10 },
    { "ChiFormatPD10", ChiFormatPD10 },
    { "ChiFormatRawMIPI8", ChiFormatRawMIPI8 },
    { "ChiFormatP010", ChiFormatP010 },
    { "ChiFormatRawPlain64", ChiFormatRawPlain64 },
    { "ChiFormatUBWCP010", ChiFormatUBWCP010 },
    { "ChiFormatRawDepth", ChiFormatRawDepth },
};

// Buffer Heap Map (for json parsing)
static const std::map<std::string, UINT32> BUFFER_HEAP = {
    { "BufferHeapSystem", BufferHeapSystem },
    { "BufferHeapIon", BufferHeapIon },
    { "BufferHeapDSP", BufferHeapDSP },
    { "BufferHeapEGL", BufferHeapEGL },
};

// Buffer mem flags Map (for json parsing)
static const std::map<std::string, UINT32> BUFFER_FLAGS = {
    { "BufferMemFlagHw", BufferMemFlagHw },
    { "BufferMemFlagProtected", BufferMemFlagProtected },
    { "BufferMemFlagCache", BufferMemFlagCache },
    { "BufferMemFlagLockable", BufferMemFlagLockable },
};

class ChiTestCase : public NativeTest
{
public:

    enum TestId
    {
        // single out stream Recipe Tests Titan
        TestIFEGeneric,
        TestIFEGenericOffline,
        TestIPEGeneric,
        TestBPSGeneric,
        TestChiGPUNode,

        // single out stream Recipe Tests Mimas
        TestOPEGeneric,
        TestTFEGeneric,
        TestTFERDIOutMIPI12,

        // JPEG single stream
        TestJPEGGeneric,
        TestInputConfigJpegSwOut,

        // double out stream Recipe Tests Titan
        TestIFEGeneric2Stream,
        TestIFEGeneric2StreamDisplay,
        TestIPEGeneric2StreamDisplay,
        TestBPSGeneric2Stream,
        TestBPSDownscaleGeneric,
        TestIFEDownscaleGeneric,
        TestIFELiteTLBGStatsOut,

        // double out stream Recipe Tests Mimas
        TestOPEGeneric2StreamDisplay,
        TestTFEGeneric2Stream,

        //three out stream Recipe Tests Titan
        TestIFELiteRawTLBGStatsOut,

        // three out stream Recipe Tests Mimas
        TestOPEGeneric3StreamDisplayStats,

        //CVP StandAlone tests
        TestCVPDenseMotion,

        // Offline Tests Titan
        TestIPEInputNV12DisplayNV12,
        TestIPEInputNV12DisplayUBWCTP10,
        TestIPEInputNV12DisplayUBWCNV12,
        TestIPEInputUBWCTP10DisplayNV12,
        TestIPEInputYUVOutputJPEG,
        TestIPEInputNV12VideoNV12,
        TestIPEInputNV12VideoUBWCNV12,
        TestIPEInputNV12VideoUBWCTP10,
        TestIPEInputNV12DisplayVideoNV12,
        TestIPEInputNV12DisplayVideoUBWCNV12,
        TestIPEInputNV12DisplayVideoUBWCTP10,
        TestIPEInputNV12DisplayNV12VideoUBWCNV12,
        TestIPEInputNV12DisplayUBWCNV12VideoNV12,
        TestIPEInputNV12DisplayNV12VideoUBWCTP10,
        TestIPEInputNV12DisplayUBWCTP10VideoNV12,
        TestBPSInputRaw16OutFullUBWCTP10,
        TestBPSInputRaw10OutFullUBWCTP10,
        TestBPSInputRaw16IPEDispNV12,
        TestBPSInputRaw16IPEJpeg,
        TestBPSInputRaw16IPEDispUBWCNV12,
        TestBPSInputRaw16IPEDispUBWCTP10,
        TestBPSInputRaw16StatsOut,
        TestBPSInputFHDRaw16StatsOut,
        TestBPSInputUHDRaw16StatsOutWithIPE,
        TestBPSInputFHDRaw16StatsOutWithIPE,
        TestBPSInputFHDRaw16BGStatsWithIPE,
        TestBPSInputUHDRaw16BGStatsWithIPE,
        TestBPSInputFHDRaw16HistStatsWithIPE,
        TestBPSInputUHDRaw16HistStatsWithIPE,
        TestJPEGInputNV12VGAOutputBlob,
        TestJPEGInputNV124KOutputBlob,
        TestBPSInputRaw16OutputGPUPorts,
        TestFDInputNV12VGAOutputBlob,
        TestBPSInputRaw10WithIPEAndDS,
        TestBPSInputRaw16WithIPEAndDS,
        TestBPSIdealRaw,
        TestIPEMFHDRSnapshot,
        TestIPEMFHDRPreview,
        TestIPEMFHDRSnapshotE2E,
        TestIPEMFHDRPreviewE2E,
        TestChiExternalNodeBPSIPE,
        TestChiExternalNodeBPSIPEJpeg,
        TestBPSIPE2Streams,
        TestChiDepthNode,

        // Titan Generic Offline
        TestIPEDisplay,
        TestIPEVideo,
        TestIPEDisplayVideo,
        TestIPEJPEGSnapshot,
        TestBPSSnapshot,
        TestBPSIPEYuvSnapshot,
        TestBPSIPEJPEGSnapshot,
        TestBPSStats,
        TestBPSIPEYuvStats,
        TestBPSIPEYuvStatsBG,
        TestBPSIPEYuvStatsBHist,
        TestBPSIPEWithDS,
        TestBPSGPUWithDS,
        TestJPEGNodeJPEGSnapshot,
        TestOfflineFD,
        TestOfflineIPEFD,
        TestIPEBlurModule,

        // Mimas Offline
        TestOPEJPEGSWInputNV124KOutputBlob,
        TestOPEInputNV12DisplayNV12,
        TestOPEInputNV12DisplayNV21,
        TestOPEInputNV12VideoNV12,
        TestOPEInputNV12VideoNV21,
        TestOPEInputConfigOutFullNV12,
        TestOPEInputConfigOutFullNV21,
        TestOPEInputNV12DisplayNV21VideoNV12,
        TestOPEInputNV12DisplayNV12VideoNV21,
        TestOPEInputNV12DisplayVideoNV12,
        TestOPEInputNV12DisplayVideoNV21,
        TestOPEJPEGSWSnapshot,
        TestOPEInputConfigStatsOut,

        // Realtime Pipeline Tests Titan
        TestIPEFull,
        TestIFEFull,
        TestIFEFullDualFoV,
        TestIFEFull3A,
        TestIFERDI0RawOpaque,
        TestIFERDI0RawPlain16,
        TestIFERawCamif,
        TestIFERawLsc,
        TestIFERDI0Raw10,
        TestIFEFullStats,
        TestRealtimePrevRDI,
        TestRealtimePrevWithCallback,
        TestRealtimePrevWith3AWrappers,
        TestPreviewWithMemcpy,
        TestPreviewFromMemcpy,
        TestPreviewHVX,
        TestBypassNodeMemcpy,
        TestCVPDME,
        TestCVPDMEDisplayPort,
        TestCVPDMEICA,
        TestIPE3A,
        TestIFEHDR,
        TestIFEIPEJpeg,
        TestIFEBPSIPEJpeg,
        TestIFEFD,
        TestIFEChiDepthNode,
        TestIFEInplaceNode,

        // Realtime Pipeline Tests Mimas
        TestTFEFull,
        TestTFEFullStats,
        TestTFERDI0RawOpaque,
        TestTFERDI0RawPlain16,
        TestTFERDI0Raw10,
        TestTFERDI1RawMIPI,
        TestTFERDI1RawPlain16,
        TestPrevRDITFEOPE,
        TestOPEFull,
        TestPreviewWithMemcpyTFEOPE,
        TestPreviewFromMemcpyTFEOPE,
        TestBypassNodeMemcpyTFEOPE,
        TestTFEFull3A,

        // PIP Camera Tests Titan
        PIPCameraIFEFull,
        PIPCameraIFEGeneric,

        // PIP Camera Tests Mimas
        PIPCameraTFEFull,

        // MultiCamera Titan
        MultiCameraIFEFull,
        MultiCameraIFEFullFlushSinglePipeline,

        // MultiCamera Mimas
        MultiCameraTFEFull,

        // E2E Tests Titan
        TestPreviewJpegSnapshot,
        TestPreviewYuvSnapshot,
        TestPreviewJpegSnapshot3AStats,
        TestPreviewYuvSnapshot3AStats,
        TestLiveVideoSnapshot,
        TestPreviewCallbackSnapshotThumbnail,
        TestHDRPreviewVideo,

        // Custom Node Tests
        TestVendorTagWrite,

        // Max to invalidate tests
        MaxNumOfTests
    };

    enum MetadataUsecases {
        MFHDR = 0,
        FD, // face detection
        DualFov,
        Zoom,
        Testgen,
        DefaultOffline,
        MaxMetaDataUsecases // max number of usecaes to validate/invalidate the usecases supported
    };

    enum SessionId
    {
        RealtimeSession,
        OfflineSession,
        MaxSessions
    };

    enum PipelineIndex
    {
        Realtime = 0,
        Offline,
        E2E,
        MaxPipelines
    };

    enum PipelineConfig
    {
        RealtimeConfig,
        OfflineConfig,
        MixedConfig,
    };

    enum FlushSignal
    {
        SignalFlushReady,
        SignalFlushTrigger,
        SignalFlushDone
    };

    const char* m_flushSignalStrings[3] = { "SignalFlushReady", "SignalFlushTrigger", "SignalFlushDone" };

    const uint32_t m_flushQueueTriggerFrame = 5;   // result frame number of when to trigger the flush for flush queue tests
    UINT           m_usFlushDelay;                 // number of usec to delay flush in FlushNode and FlushQueue tests
    UINT64         m_flushStartTime;               // time that flush was triggered
    UINT64         m_flushEndTime;                 // time that flushed request was returned to app
    UINT32         m_flushFrame;                   // frame number to trigger flush after
    OSThreadHandle m_flushThread;                  // worker thread in charge of triggering flush when signaled by main thread
    FlushSignal    m_flushSignaler;                // enum to help signal/command the flush thread
    Mutex*         m_pFlushSignalMutex;            // mutex to protect the signaler
    Condition*     m_pFlushSignalCond;             // condition to help coordinate between main and flush thread
    UINT64         mCurrentResultFrame[MaximumPipelinesPerSession]; // result frame numbers most recently receivedfor pipeline flush

    // Camera related
    std::vector <UINT32>    m_cameraList;                               // list of camera IDs to test
    int                     m_currentTestCameraId;

    // capture request related
    CHICAPTUREREQUEST       m_offlineRequest[MAX_REQUESTS];     ///< array of offline capture requests
    CHICAPTUREREQUEST       m_realtimeRequest[MAX_REQUESTS];    ///< array of realtime capture requests
    ChiPrivateData          m_requestPvtData[MAX_SESSIONS];     ///< request private data

    // metadata and session parameters related
    /// @brief TestGen Capabilities related to the device
    typedef struct
    {
        UINT32 maxBayerWidth;                ///< Max Regular Bayer Width
        UINT32 maxBayerHeight;               ///< Max Regular Bayer Height
        UINT32 maxQCFAWidth;                 ///< Max QCFA Width
        UINT32 maxQCFAHeight;                ///< Max QCFA Height
        UINT32 maxQCFAHDRWidth;              ///< Max QCFA HDR Width
        UINT32 maxQCFAHDRHeight;             ///< Max QCFA HDR Height
        UINT32 maxSHDRWidth;                 ///< Max SHDR Width
        UINT32 maxSHDRHeight;                ///< Max SHDR height
        UINT32 maxPDAFWidth;                 ///< Max PDAF Width
        UINT32 maxPDAFHeight;                ///< Max PDAF Height
        BOOL   bISSHDRSupported;             ///< TRUE if SHDR mode supported by testgen
        BOOL   bIsYUVSupported;              ///< TRUE if YUV Output supported by testgen
        BOOL   bIsQCFASupported;             ///< TRUE if QCFA mode supported by testgen
        BOOL   bIsQCFAHDRSupported;          ///< TRUE if QCFA HDR mode supported by testgen
        BOOL   bIsFSSupported;               ///< TRUE if FS mode supported by testgen
        BOOL   bIsPDAFSupported;             ///< TRUE if PDAF mode supported by testgen
        UINT32 maxBitWidth;                  ///< Max Bit Width that can be configured
        UINT32 maxNumberOfExposuresinSHDR;   ///< Max number of exposures that can be supported by testgen
    } CHITESTGENCAPS;

    CHITESTGENCAPS*         m_pTestGenCaps; ///< pointer to testgen capabilities queried from driver at session level

    // buffer and buffer manager related
    ChiBufferManager::NativeChiBuffer  m_realtimeOutputBuffers[MAX_STREAMS];    ///< realtime output buffers

    // pipeline related
    // TODO: Make them per session
    CHIPIPELINEMETADATAINFO m_pPipelineMetadataInfo[MaximumPipelinesPerSession];    ///< pipeline metadata information
    ChiPipeline*            m_pChiPipeline[MaximumPipelinesPerSession];             ///< pipeline instances per session
    CHIPIPELINEREQUEST      m_submitRequest[MAX_CAMERAS][MAX_SESSIONS];             ///< pipeline request
    PipelineConfig          m_pipelineConfig;                                       ///< differentiate between pipeline config

    // infrastructure related
    INT32           m_socId = 0;            ///< Camera SOC ID
    static bool     ms_bEnableNewImageRead; ///< toggle for switching to new set of input images for input buffers
    static bool     sbIsCameraIDSessionID;  ///< set to true camera ID is used as session ID

    // HDR related
    UINT32         mNumberOfExposures = 0;  ///< number of exposures involved in the test

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setup
    ///
    /// @brief  Virtual function for setup of this class
    ///
    /// @param  None
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual VOID  Setup();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Teardown
    ///
    /// @brief  Virtual function for teardown of this class
    ///
    /// @param  None
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual VOID  Teardown();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// QueueCaptureResult
    ///
    /// @brief  Perform base class functionality for processing the capture result
    ///         Copy and queue up capture results returned from driver for later processing in the main thread
    ///
    /// @param  pResult                 Total Result the came through callback
    /// @param  pPrivateCallbackData    Callback data that came through the callback
    ///
    /// @return void
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void QueueCaptureResult(CHICAPTURERESULT* pResult, void* pPrivateCallbackData);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// QueuePartialCaptureResult
    ///
    /// @brief  Perform base class functionality for processing the partial capture result
    ///         Copy and queue up capture results returned from driver for later processing in the main thread
    ///
    /// @param  pPartialResult          Partial Result the came through callback
    /// @param  pPrivateCallbackData    Callback data that came through the callback
    ///
    /// @return void
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void QueuePartialCaptureResult(CHIPARTIALCAPTURERESULT* pPartialResult, void* pPrivateCallbackData);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ProcessMessage
    ///
    /// @brief  Perform processing of the message notification or Process Notify message from driver
    ///
    /// @param  pMessageDescriptor      Message Descriptor the came through callback
    /// @param  pPrivateCallbackData    Callback data that came through the callback
    ///
    /// @return void
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void ProcessMessage(const CHIMESSAGEDESCRIPTOR* pMessageDescriptor, void* pPrivateCallbackData);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// QueryStaticCapabilities
    ///
    /// @brief  Query static capabilities through vendor tags as per the usecase and camera ID
    ///
    /// @param  usecase    usecase type
    /// @param  camId      Camera ID
    ///
    /// @return CDKResult  result
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult QueryStaticCapabilities(MetadataUsecases usecase, UINT32 cameraID);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetSessionParameters
    ///
    /// @brief  Set session level parameters as per the usecase and camera ID
    ///
    /// @param  usecase    usecase type
    /// @param  camId      Camera ID
    /// @param  Size       maxStreamSize
    ///
    /// @return CDKResult  result
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //CDKResult SetSessionParameters(MetadataUsecases usecase, UINT32 cameraID, Size maxStreamSize);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetUsecaseMetadata
    ///
    /// @brief  Sets metadata as per the usecase on top of default metadata
    ///
    /// @param  usecase      usecase type
    /// @param  fNum        frame number
    /// @param  chiCapReq   CHICAPTUREREQUEST, where metadata needs to be set
    /// @param  resMax      Maximum Resolution of the streams
    /// @param  camId      Camera Id
    ///
    /// @return CDKResult   CDKResult type
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult SetUsecaseMetadata(MetadataUsecases usecase, UINT64 fNum, CHICAPTUREREQUEST chiCapReq, Size resMax, int camId);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TriggerFlush
    ///
    /// @brief  Flushes the session / pipeline, sets the flush trigger flag, and starts tracking the flush time
    ///
    /// @param  None
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void TriggerFlush(SessionId sessionId);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// WaitForFlushSignal
    ///
    /// @brief  Should be run in a worker thread, which sleeps until condition variables wakes it up to trigger a flush
    ///
    /// @param  [in]    VOID*   pArg    pointer to a ChiTestCase instance
    ///
    /// @return         VOID*   not used
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* WaitForFlushSignal(VOID * pArg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SignalFlushThread
    ///
    /// @brief  Changes the signaler and then notifies the flush thread
    ///
    /// @param  [in]    FlushSignal     signal      command to direct the flush thread
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SignalFlushThread(FlushSignal signal);

protected:

    // making constructor default since Setup() will take care of initializing
    ChiTestCase() = default;
    // making destructor default since Teardown() will take care of destroying
    ~ChiTestCase() = default;

    /// Session private data
    struct SessionPrivateData
    {
        ChiTestCase* pTestInstance; ///< Per usecase class
        uint32_t     sessionId;     ///< Session Id that is meaningful to the usecase in which the session belongs
        UINT32       cameraId;      ///< Camera ID that session belongs (or) fixed index in case of multicamera
        ChiSession*  pChiSession;   ///< ChiSession Pointer
        CHIHANDLE    sessionHandle; ///< ChiSessionHandle to reduce redundant calling of GetSessionHandle
    };

    struct PerFrameData
    {
        int                                           perFramePendingOpBuffers;
        std::map<CHIBUFFERHANDLE, ChiBufferManager*>     inputBufferHandlesManagersMap;
    };

    /* StreamCreateData structure and memory allocated for its variables */

    // flush related variables
    bool    m_bIsFlushTriggered;             // tells if session was flushed

    // test infrastructure related member variables
    bool        m_isCamIFTestGen;                // set true if the test is camif test gen based
    TestId      m_testId = MaxNumOfTests;        // Test ID of current test

    // command line inputs
    UINT32          m_captureCount;
    CHISTREAMFORMAT m_userInputFormat;
    CHISTREAMFORMAT m_userPrvFormat;
    CHISTREAMFORMAT m_userVidFormat;
    CHISTREAMFORMAT m_userSnapFormat;
    Size            m_userInputSize;
    Size            m_userPrvSize;
    Size            m_userRDISize;
    Size            m_userVidSize;
    Size            m_userSnapSize;
    Size            m_userDispSize;
    UINT32          m_nEarlyPCR;

    // stream related member variables
    bool                m_isRealtime[MAX_STREAMS];       // bool to indicate if this stream is part of realtime session
    ChiBufferManager*   m_pBufferManagers[MAX_STREAMS];  // pointer to buffermanagers for each stream
    ChiStream*          m_pRequiredStreams;              // pointer to created stream objects
    CHISTREAMFORMAT     m_format[MAX_STREAMS];           // format for the streams
    CHISTREAMTYPE       m_direction[MAX_STREAMS];        // stream direction
    int                 m_numStreams;                    // total number of streams for given test case
    int                 m_numInputStreams;               // total number of input streams for given test case
    Size                m_resolution[MAX_STREAMS];       // resolution for the streams
    StreamCreateData    m_streamInfo;                    // stream info required for creating stream objects
    StreamUsecases      m_streamId[MAX_STREAMS];         // unique identifier for the streams
    uint32_t            m_usageFlag[MAX_STREAMS];        // gralloc usage flags for the streams

    // stream related tracker member variables
    std::map<ChiStream*, ChiBufferManager*>             m_streamBufferMap;   // map storing stream and its buffermanager
    std::map<StreamUsecases, ChiStream*>                m_streamIdMap;       // map storing unique streamId and stream object
    static std::map<ChiStream*,std::vector<uint32_t>>   m_bufferErrorStreamMap; // stream pointer to buffer errored frames
    UINT32                                              mLastFrameReceived;

    // per frame trackers
    std::map<int, PerFrameData>     m_frameToIpBuffHandleMap;

    // chi pointers to driver Ops
    ChiModule*               m_pChiModule;                    // pointer to the ChiModule singleton
    ChiMetadataUtil*         m_pChiMetadataUtil;              // pointer to chi metadata util singleton

    // static chi pointers to driver ops
    static CHICONTEXTOPS*   spChiOps;   ///< ChiContextOps for using in static functions

    /* Process Capture Result Task Offloading */
    static std::queue<ChiCaptureResult*>    m_pResultsToProcess;               ///< queue of capture results that need to be processed
    static std::queue<SessionPrivateData*>  m_pPrivateDataToProcess;           ///< queue of private data associated with a capture result
    SessionPrivateData                      mPerSessionPvtData[MAX_SESSIONS];  ///< test specific private data per camera

    static VOID*      ProcessCaptureResults(VOID* pArg);
    virtual CDKResult WaitForResults();
    // @brief functions to be implemented by derived class
    virtual CDKResult SetupStreams() = 0;
    virtual CDKResult SetupPipelines(int cameraId, ChiSensorModeInfo* sensorMode) = 0;
    virtual CDKResult CreateSessions() = 0;
    virtual void      DestroyResources() = 0;

    CDKResult         InitializeBufferManagers(int cameraId);
    void              DestroyCommonResources();
    void              AtomicIncrement(int count = 1);
    void              AtomicDecrement(int count = 1);
    uint32_t          GetPendingBufferCount() const;
    static bool       IsRDIStream(CHISTREAMFORMAT format);
    static bool       IsSensorModeMaxResRawUnsupportedTFEPort(StreamUsecases tfePort);
    void              ConvertCamera3StreamToChiStream(camera3_stream_t* inCamStreams, CHISTREAM* outChiStreams, int numStreams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClearErrorNotifyCode
    ///
    /// @brief  Clear the error notify code for the given frame
    /// @param  UINT32   frame  Frame number to clear error code for
    /// @return void
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void  ClearErrorNotifyCode(UINT32 frame);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckErrorNotifyCode
    ///
    /// @brief  Check if given error notify code set for the given frame
    /// @param  UINT32               frame            Frame number to associate the error notify code with
    /// @param  CHIERRORMESSAGECODE  errorCode        Error notify code for the current frame
    /// @param  CHISTREAM*           pCurrentStream   Pointer to stream associated with the frame
    /// @return bool
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool CheckErrorNotifyCode(UINT32 frame, CHIERRORMESSAGECODE errorCode, CHISTREAM* pCurrentStream = NULL);

    /* Metadata getters */
    static inline const VOID* GetInputMetadata(CHICAPTUREREQUEST* pRequest)
    {
        return pRequest->pInputMetadata;
    }
    static inline const VOID* GetOutputMetadata(CHICAPTURERESULT* pResult)
    {
        return pResult->pOutputMetadata;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GenerateOfflineCaptureRequest
    ///
    /// @brief  Generate an offline pipeline request
    ///
    /// @param  frameNumber framenumber associated with the request
    ///
    /// @return CDKResult success if request could be submitted or failure
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult GenerateOfflineCaptureRequest(uint64_t frameNumber);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GenerateRealtimeCaptureRequest
    ///
    /// @brief  Generate a realtime pipeline request
    /// @param  frameNumber     framenumber associated with the request
    /// @param  bSnapshot       flag to enable snapshot request
    /// @param  bSkipE2EPreview flag to enable snapshot request only and skip preview in E2E
    /// @return CDKResult       success if request could be submitted or failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult GenerateRealtimeCaptureRequest(UINT64 frameNumber, bool bSnapshot = false, bool bSkipE2EPreview = false);


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CommonProcessCaptureResult()
    ///
    /// @brief  Overridden function implementation which processes the capture result obtained from driver
    ///
    /// @param  pCaptureResult          pointer to capture result
    /// @param  pPrivateCallbackData    pointer private callback data
    ///
    /// @return None
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void CommonProcessCaptureResult(ChiCaptureResult * pCaptureResult, SessionPrivateData * pSessionPrivateData);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ValidateCustomNodeResult
    ///
    /// @brief  Function implementation to populate vendortag with current processed frame, verify vendor tag frame matches
    ///         expected frame number
    ///
    /// @param  pCaptureResult  pointer to capture result
    ///
    /// @return CDKResult
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ValidateCustomNodeResult(ChiCaptureResult* pCaptureResult);

private:
    std::atomic<uint32_t>    m_buffersRemaining;                  // pending number of buffers
    Mutex*                   m_pBufferCountMutex;                 // mutex associated with buffer remaining
    static Condition*        m_pBufferCountCond;                  // cond variable to signal/wait on buffer remaining
    static Mutex*            m_pResultQueueMutex;                 // mutex for accessing capture result and private data queues
    int                      m_waitForResultsRetries;             // number of tries NativeChiTest has waited for result
    static UINT32            m_errorNotifyList[MAX_REQUESTS];     // per frame error notify list

    void                     ResetWaitForResultsRetryCount(); // resets the number of retries waiting for result

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetErrorNotifyCode
    ///
    /// @brief  Set the given error notify code for the given processed frame
    /// @param  UINT32               frame            Frame number to associate the error notify code with
    /// @param  CHIERRORMESSAGECODE  errorCode        Error notify code for the current frame
    /// @param  CHISTREAM*           pCurrentStream   Pointer to stream associated with the frame
    /// @return void
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetErrorNotifyCode(UINT32 frame, CHIERRORMESSAGECODE errorCode, CHISTREAM* pCurrentStream = NULL);

    /*Camx Overrides*/
    std::map<std::string, std::string> m_camXORMap = {};

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// EnableOverrides
    ///
    /// @brief  Enable overrides based on test
    ///
    /// @return CDKResult
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult EnableOverrides();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// WriteCamXORMapToFile
    ///
    /// @brief  Write provided map to a file (camxoverrides)
    ///
    /// @param  writeMap    Map to write
    ///
    /// @return CDKResult
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult WriteCamXORMapToFile(std::map<std::string, std::string> writeMap);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ReadCamXOR
    ///
    /// @brief  Read camxoverridesettings.txt and store into a map
    ///
    /// @param  preserve    flag to indicate whether to store it to Map or not
    ///
    /// @return CDKResult
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ReadCamXOR(bool preserve = false);

    /* Capture result copy and destroy operations */
    static ChiCaptureResult* DeepCopyCaptureResult(ChiCaptureResult* pCaptureResult);       // Copies all parts of the capture result
    static void              DeepDestroyCaptureResult(ChiCaptureResult* pCaptureResult);    // Destroys all parts of the capture result

    /* Processing capture results */
    enum ProcessSignal
    {
        SignalReady,
        SignalProcess,
        SignalDone
    };

    OSThreadHandle          m_processingThread;     // worker thread in charge of processing capture results when signaled by main thread
    static ProcessSignal    m_processingSignaler;   // enum to help signal/command the processing thread
    static Mutex*           m_pProcessingMutex;     // mutex to protect the signaler
    static Condition*       m_pProcessingCond;      // condition to help coordinate between main and processing thread
    static void             SignalProcessingThread(ProcessSignal signal);   // Signals processing thread to perform a task

    // Do not allow the copy constructor or assignment operator
    ChiTestCase(const ChiTestCase& rUsecase) = delete;
    ChiTestCase& operator= (const ChiTestCase& rUsecase) = delete;
};

} // namespace chitests

#endif  //#ifndef __CHI_TESTCASE__
