////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chibuffermanager.h
/// @brief Declarations for the ChiBufferManager class in NativeCHI
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CHI_BUFFER_MANAGER__
#define __CHI_BUFFER_MANAGER__

#include "nativetestlog.h"
#include <queue>
#include "chistatspropertydefines.h"
#include "camxpresilmem.h"
#include "camera3stream.h"
#include "commonutils.h"
#include "chi.h"
#include "chimodule.h"
#include "chxutils.h"

namespace chitests
{

// FD related metrics
#define IOU_LOWER_LIMIT           0.45 // Minimum required IOU for each face is 45%
#define LOW_IOU_FRAMES_TOLERANCE  10   // 10% of result frames allowed to have low IOU
#define FD_MIN_FRAME_INTERVAL     34
#define IOUShift(newIOU, oldIOU)  (std::abs(newIOU - oldIOU))
#define FD_FLUCTUATION_TOLERANCE  0.5

class ChiBufferManager
{

public:

    typedef CHISTREAM               NativeChiStream;
    typedef CHISTREAMBUFFER         NativeChiBuffer;
    typedef CHIBUFFERHANDLE         NativeChiBufferHandle;

    static std::string      m_lastImageRead[chitests::MAXIMUM_STREAMS]; // The latest image read in 8th input buffer
    struct GenericStream
    {
        union
        {
            NativeChiStream* pChiStream;
        };
        GenericStream() {};
        GenericStream(NativeChiStream* pStream) : pChiStream(pStream) {};
    };

    struct GenericBuffer
    {
        union
        {
            NativeChiBuffer* pChiBuffer;
        };
        GenericBuffer() {};
        GenericBuffer(NativeChiBuffer* pBuffer) : pChiBuffer(pBuffer) {};
    };

    static int LoadBufferLibs(void* pLib);

    ChiBufferManager(StreamUsecases streamId);
    ~ChiBufferManager();

    // per face FD data from driver / ground truth
    struct FaceDataFD
    {
        UINT32               faceId;
        UINT32               confidence;
        float                faceIOU;
        bool                 hasLowIOU;
        RectangleCoordinate  faceROI;
    };

    // Per frame face count status
    enum FaceCountStatus
    {
        LessThanExpected = 0, // Frame detected less faces than expected in ground truth
        ExactFaceCount,       // Frame detected exact number of faces as expected in ground truth}
        FalsePositive,        // Frame detected more faces than expected in ground truth
        maxFaceCountStatus
    };

    static const char* m_faceCountStatusTag[maxFaceCountStatus];

    // per frame FD results from driver / ground truth
    struct FrameDataFD
    {
        UINT64          frameID;          // current frame ID
        UINT32          faceCount;        // number of faces detected in this frame
        FaceDataFD*     faceData;         // per face data in this frame for faceCount
        FaceCountStatus faceCountStatus;  // status of face count
    };

    // per face FD metrics
    struct FaceMetrics
    {
        UINT8  faceId;               // current face ID
        float  bestIOU;              // best IOU reached
        UINT64 bestFrame;            // request ID with best IOU
        float  worstIOU;             // worst IOU reported
        UINT64 worstFrame;           // request ID with worst IOU
        UINT64 sumConfidence;        // sum of all IOUs for given face
        UINT32 meanConfidence;       // mean IOU for current face
        float  sumIOU;               // sum of all IOUs for given face
        float  meanIOU;              // mean IOU for current face
        UINT64 numLowIOUFrames;      // counter for frames with IOU < lower limit
        UINT64 firstGoodFrame;       // first frame which detected the given face
        UINT64 numFluctuations;      // number of times detected face had huge shift in IOU over several frames
        float  firstDetectionTime;   // time taken to first detect given face
    };

    // overall frame FD metrics
    struct OverallFDMetrics
    {
        UINT64 numFalsePositives;    // number of frames that detected more than expected faces
        UINT64 numFramesDroppedFace; // number of frames that detected less than expected faces
        UINT64 firstGoodFrameOverall;// first frame which detected all the faces
    };

    static long int                         m_startTimeFD;             // time when first request sent for FD test
    static bool                             m_isFDDatabaseTest;        // flag to tell if FD snapshot database being tested
    static std::vector<FrameDataFD>         mTargetFaceData;           // FD data from GT per frame
    static std::vector<FrameDataFD>         mTestFaceData;             // FD data from driver per frame
    static std::vector<float>               m_firstDetectionTimeList;  // time taken for first detection result
    static std::vector<UINT64>              m_firstDetectionFrameList; // toggles when face detected for first time
    static UINT64                           m_firstGoodFrameOverall;   // first frame which detected all the faces

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::Initialize
    ///
    /// @brief      Initialize new instance of ChiBufferManager
    ///
    /// @param  cameraId            camera ID for generating buffers
    /// @param  configuredStream    stream for generating buffers
    /// @param  streamId            stream ID for generating buffers (Native CHI specific ID)
    /// @param  filename            filename for generating buffers (nullptr for non-input streams)
    /// @param  inStreamIdx         input stream index (counted only by input direction)
    /// @param  delayDifferentInput Delay reading different frame by x number of frames (applies to input only)
    ///
    /// @return     Pointer to initialized BufferManager object or nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiBufferManager* Initialize(int cameraId, GenericStream* configuredStream, StreamUsecases streamId, const char* filename,
        int inStreamIdx = -1, UINT32 delayDifferentInput=0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetOutputBufferForRequest
    ///
    /// @brief      Creates output buffer for frame request
    ///
    /// @return     Void Pointer to Native Buffer or nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void* GetOutputBufferForRequest();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetInputBufferForRequest
    ///
    /// @brief      Creates Input buffer for frame request
    ///
    ///@param  bGetGenericBuffer      Get Generic Buffer instead of Native Buffer if required (for FD Testing)
    ///
    /// @return     Void Pointer to Native Buffer or nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void* GetInputBufferForRequest(bool bGetGenericBuffer = false);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetTimeElapsed
    ///
    /// @brief      Returns the time elapsed for first detection in FD
    ///
    ///@param startTime             start time for FD
    ///
    /// @return     Detection time in milliseconds
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static long int         GetTimeElapsed(long int startTime);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetNextImageName
    ///
    /// @brief      Reads next image in the location to be fed into new buffer for input stream
    ///
    /// @param      streamIdx   Stream Index
    ///
    /// @return     File name in std::string type
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::string GetNextImageName(int streamIdx);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ProcessBufferFromResult
    ///
    /// @brief  Function to process the buffer returned as part of the result
    ///
    /// @param  frameNumber     frame number associated with buffer
    /// @param  resultBuffer    result buffer returned from the driver
    /// @param  dump            dump flag to save to memory or not
    ///
    /// @return -1 in case of any error or else 0
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ProcessBufferFromResult(const int frameNumber, GenericBuffer* resultBuffer, bool dump);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GenerateInputBuffer
    ///
    /// @brief  Generate Input Buffer for offline tests
    ///
    /// @param  buffer      generic buffer
    /// @param  stream      stream that buffer to be associated with
    /// @param  filename    filename to that needs to be filled into the intput buffer
    ///
    /// @return -1 in case of any error or else 0
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GenerateInputBuffer(GenericBuffer* buffer, GenericStream* stream, const char* filename);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetStatusFromBuffer
    ///
    /// @brief  Get buffer status from generic buffer
    ///
    /// @param  buffer  buffer to fetch the buffer status from
    ///
    /// @return buffer status
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GetStatusFromBuffer(GenericBuffer* buffer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ReadFaceROIFromBuffer
    ///
    /// @brief    Function to get face ROI information from the driver for the given result buffer
    ///
    /// @param  resultBuffer     result buffer to process
    ///
    /// @return CDKResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ReadFaceROIFromBuffer(GenericBuffer* resultBuffer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ReadFDDatabase
    ///
    /// @brief  Function to extract face ROI information from the Ground_Truth file
    ///
    /// @param  FDTestImage     input scene name to be tested
    ///
    /// @return CDKResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult ReadFDDatabase(std::string FDTestImage);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::CalculateIOUForResultBuffer
    ///
    /// @brief  Function to calculate the Intersection of Union of the received and target face ROIs in video
    ///
    /// @param  receivedBufferROI   ROI data from result buffer
    ///
    /// @return CDKResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult CalculateIOUForResultBuffer(FaceROIInformation* receivedBufferROI);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::PrintFDMetrics
    ///
    /// @brief  Function to print the overall and per face FD metrics
    /// @param  None
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void PrintFDMetrics();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::DestroyBuffers
    ///
    /// @brief  Cleanup function to free all the memory allocated
    ///
    /// @return none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void DestroyBuffers();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BufferManager::ReleaseReferenceToBuffer
    ///
    /// @brief  release the reference to buffer, so that camx chi buffer manager moves to free list
    ///
    /// @param  pHandle  chi buffer handle
    ///
    /// @return CDKResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ReleaseReferenceToBuffer(CHIBUFFERHANDLE pHandle);

protected:

    // chi related
    CHIBUFFERMANAGERHANDLE         m_pCamXChiBufferMgr;
    std::string                    m_OutFileName;
    ChiModule*                     m_pChiModule;

    //memory Related variables
    static CamxMemAllocFunc        m_pCamxMemAlloc;
    static CamxMemReleaseFunc      m_pCamxMemRelease;
    static CamxMemGetImageSizeFunc m_pCamxMemGetImageSize;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GenerateBuffers
    ///
    /// @brief  Generates output buffers for given stream
    ///
    /// @param  cameraId            Camera ID for the file name
    /// @param  configuredStream    configured stream for a given session
    /// @param  streamId            streamId (Native CHI specific)
    /// @param  filename            filename for input stream to be read
    /// @param  inStreamIdx         input Stream Index
    /// @param  delayDifferentInput Delay reading different frame by x number of frames (applies to input only)
    ///
    /// @return 0 on success or -1 on failure
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GenerateBuffers(int cameraId, GenericStream* configuredStream, StreamUsecases streamId, const char* filename,
        int inStreamIdx = 0, UINT32 delayDifferentInput=0);

private:

    //Buffer Queue variables
    std::deque<GenericBuffer*>*  m_inputImageQueue;
    std::queue<GenericBuffer*>*  m_emptyQueue;
    std::queue<GenericBuffer*>*  m_filledQueue;

    Mutex*              m_pQueueMutex;
    Condition*          m_pQueueCond;
    bool                m_bBufferAvailable;
    int                 m_streamType;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::initCamXChiBufferManager
    ///
    /// @brief  initialize {m_pCamXChiBufferMgr} by creating new camx chi buffer manager
    ///
    /// @param  pStream     stream that buffer to be associated with
    ///
    /// @return none
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void initCamXChiBufferManager(GenericStream* pStream, std::string fName);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GenerateOutputBuffer
    ///
    /// @brief  Overloaded helper function to generate a Chi native buffer and NativeChiBuffer for a given input stream
    ///
    /// @param  pRequiredBuffer generic buffer
    /// @param  pOutStream      stream that buffer to be associated with
    ///
    /// @return 0 on success or -1 on failure
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GenerateOutputBuffer(GenericBuffer* pRequiredBuffer, GenericStream* pOutStream);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::SaveImageToFile
    ///
    /// @brief  Helper function to save an image to the file
    ///
    /// @param  buffer  Generic buffer pointer
    /// @param  stream  Generic stream pointer
    ///
    /// @return 0 on success or -1 on failure
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int SaveImageToFile(GenericBuffer* resultbuffer, std::string filename);

    /* Buffer Allocation Helpers */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::CreateGenericBuffer
    ///
    /// @brief  Helper function to initialize a generic buffer with a chi buffer
    ///
    /// @return GenericBuffer pointer on success, nullptr otherwise
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    GenericBuffer* CreateGenericBuffer();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::DestroyGenericBuffer
    ///
    /// @brief  Helper function to destroy a generic buffer along with its associated chi buffer
    ///
    /// @param  buffer  Generic buffer pointer
    ///
    /// @return 0 on success, otherwise -1
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void DestroyGenericBuffer(GenericBuffer* buffer);

    /* Fence Helpers */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::CreateFence
    ///
    /// @brief  Creates a fence handle using the driver and assembles a CHIFENCEINFO object out of it
    ///
    /// @param  isValid     if set false, the returned fence will not have any memory allocated for its handle
    ///
    /// @return CHIFENCEINFO which may have its valid flag true or false
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHIFENCEINFO CreateFence(bool isValid = true);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::DestroyFence
    ///
    /// @brief  Destroys a fence by releasing it on driver side and deallocating its fence handle. No action if fence not valid.
    ///
    /// @return None
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void DestroyFence(CHIFENCEINFO fenceInfo);

    /* Stream Getters */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   ChiBufferManager::GetTypeFromStream
    ///
    /// @brief  Get stream type from generic stream
    ///
    /// @param stream   Generic stream pointer
    ///
    /// @return stream type
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GetTypeFromStream(GenericStream* stream);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetMaxBuffersFromStream
    ///
    /// @brief  Get max number of buffers from generic stream
    ///
    /// @param  stream  Generic Stream pointer
    ///
    /// @return max number of buffers
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t GetMaxBuffersFromStream(GenericStream* stream);

    /* Buffer Getters */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetNativeBufferFromGeneric
    ///
    /// @brief  Get void pointer to a NativeChiBuffer from a GenericBuffer
    ///
    /// @param buffer   buffer to get the actual native buffer
    ///
    /// @return void pointer to the associated NativeChiBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void* GetNativeBufferFromGeneric(GenericBuffer* buffer);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetHandleFromBuffer
    ///
    /// @brief  Get buffer handle from generic buffer
    ///
    /// @param  buffer  buffer to get handle on
    ///
    /// @return NativeChiBufferHandle
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NativeChiBufferHandle GetHandleFromBuffer(GenericBuffer* buffer);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetReleaseFenceFromBuffer
    ///
    /// @brief  Get release fence from generic buffer
    ///
    /// @param  buffer  Generic buffer pointer
    ///
    /// @return release fence status, where -1 means no waiting
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GetReleaseFenceFromBuffer(GenericBuffer* buffer);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::SetAcquireFenceInBuffer
    ///
    /// @brief  Set the acquire fence in generic buffer
    ///
    /// @param  buffer  buffer to set fence
    /// @param  setVal  value
    ///
    /// @return void
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAcquireFenceInBuffer(GenericBuffer* buffer, int setVal);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetMaxBuffersFromBuffer
    ///
    /// @brief  Get the max number of buffers from generic buffer's associated stream
    ///
    /// @param  buffer generic buffer
    ///
    /// @return uint32_t    max number of buffers
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t GetMaxBuffersFromBuffer(GenericBuffer* buffer);

    /// Do not allow the copy constructor or assignment operator
    ChiBufferManager(const ChiBufferManager&) = delete;
    ChiBufferManager& operator= (const ChiBufferManager&) = delete;
};
}

#endif //__CHI_BUFFER_MANAGER__
