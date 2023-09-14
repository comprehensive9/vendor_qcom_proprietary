////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chibuffermanager.cpp
/// @brief Definitions for the ChiBufferManager class  in NativeCHI
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include "chibuffermanager.h"

namespace chitests
{
    CamxMemAllocFunc                            ChiBufferManager::m_pCamxMemAlloc;
    CamxMemReleaseFunc                          ChiBufferManager::m_pCamxMemRelease;
    CamxMemGetImageSizeFunc                     ChiBufferManager::m_pCamxMemGetImageSize;


    std::vector <ChiBufferManager::FrameDataFD>     ChiBufferManager::mTargetFaceData;
    std::vector <ChiBufferManager::FrameDataFD>     ChiBufferManager::mTestFaceData;
    bool                                            ChiBufferManager::m_isFDDatabaseTest = false;
    std::string                                     ChiBufferManager::m_lastImageRead[chitests::MAXIMUM_STREAMS];
    std::vector<UINT64>                             ChiBufferManager::m_firstDetectionFrameList;
    std::vector<float>                              ChiBufferManager::m_firstDetectionTimeList;
    UINT64                                          ChiBufferManager::m_firstGoodFrameOverall = 0;
    long int                                        ChiBufferManager::m_startTimeFD;
    const char* ChiBufferManager::m_faceCountStatusTag[maxFaceCountStatus] = {"LessThanExpected", "ExactFaceCount", "FalsePositive"};


    //Constructor for the ChiBufferManager class
    ChiBufferManager::ChiBufferManager(StreamUsecases streamId) :
        m_OutFileName(""),
        m_inputImageQueue(nullptr),
        m_emptyQueue(nullptr),
        m_filledQueue(nullptr),
        m_bBufferAvailable(false),
        m_streamType(-1)
    {
        // Initialize mutex and condition member variables
        m_pChiModule = ChiModule::GetInstance();
        m_pQueueMutex = Mutex::Create((mapStreamStr[streamId]).c_str());
        m_pQueueCond = Condition::Create((mapStreamStr[streamId]).c_str());
    }

    //Destructor for the ChiBufferManager class
    ChiBufferManager::~ChiBufferManager()
    {
        m_inputImageQueue = nullptr;
        m_emptyQueue = nullptr;
        m_filledQueue = nullptr;
        m_bBufferAvailable = false;
        m_OutFileName = "";
        m_streamType = -1;

        // Destroy mutex and condition member variables
        if (nullptr != m_pQueueMutex)
        {
            m_pQueueMutex->Destroy();
            m_pQueueMutex = nullptr;
        }
        if (nullptr != m_pQueueCond)
        {
            m_pQueueCond->Destroy();
            m_pQueueCond = nullptr;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetTimeElapsed
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    long int ChiBufferManager::GetTimeElapsed(long int startTime)
    {
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        long int endTime = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
        long int timeElapsed = endTime - startTime;
        float timeMilli = float(timeElapsed) / float(1000);
        return timeMilli;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::LoadBufferLibs
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::LoadBufferLibs(
        void* pLib) //[in] library to be loaded
    {
        // CamxMemGetImageSizeFunc available on both Android and Windows
        m_pCamxMemGetImageSize = reinterpret_cast<CamxMemGetImageSizeFunc>(ChxUtils::LibGetAddr(pLib, "CamxMemGetImageSize"));
        if (m_pCamxMemGetImageSize == NULL)
        {
            NT_LOG_ERROR("CamxMemGetImageSize not available in the library");
            return -1;
        }

#ifndef _LINUX
        // CamxMemAllocFunc and CamxMemReleaseFunc not available on Android
        m_pCamxMemAlloc = reinterpret_cast<CamxMemAllocFunc>(ChxUtils::LibGetAddr(pLib, "CamxMemAlloc"));
        if (nullptr == m_pCamxMemAlloc)
        {
            NT_LOG_ERROR("CamxMemAllocFunc not available in the library");
            return -1;
        }

        m_pCamxMemRelease = reinterpret_cast<CamxMemReleaseFunc>(ChxUtils::LibGetAddr(pLib, "CamxMemRelease"));
        if (nullptr == m_pCamxMemRelease)
        {
            NT_LOG_ERROR("CamxMemRelease not available in the library");
            return -1;
        }
#endif // _LINUX
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::Initialize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiBufferManager* ChiBufferManager::Initialize(int cameraId, GenericStream* configuredStream, StreamUsecases streamId,
        const char* filename, int inStreamIdx, UINT32 delayDifferentInput)

    {
        int result = -1;

        if (filename == nullptr)
        {
            result = GenerateBuffers(cameraId, configuredStream, streamId, filename);
        }
        else
        {
            if (inStreamIdx < 0) {
                return nullptr;
            }
            result = GenerateBuffers(cameraId, configuredStream, streamId, filename, inStreamIdx, delayDifferentInput);
        }

        if (result != 0)
        {
            DestroyBuffers();
            return nullptr;
        }

        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GenerateBuffers
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::GenerateBuffers(int cameraId, GenericStream* configuredStream, StreamUsecases streamId,
        const char* filename, int inStreamIdx, UINT32 delayDifferentInput)
    {
        if (configuredStream == nullptr)
        {
            NT_LOG_ERROR("Cannot create buffers for NULL stream");
            return -1;
        }

        m_streamType = GetTypeFromStream(configuredStream);
        std::string readImageFile = ""; // declare a string to be fed later with valid filename in input stream
        if (m_streamType == CAMERA3_STREAM_INPUT)
        {
            m_inputImageQueue = SAFE_NEW() std::deque<GenericBuffer*>();
            if (nullptr == filename) // nullptr check to avoid CFI violation
            {
                NT_LOG_ERROR("Input filename invalid!");
                return -1;
            }
            readImageFile = filename;  // feed to std::string for further operations
        }
        else if (m_streamType == CAMERA3_STREAM_BIDIRECTIONAL)
        {
            m_inputImageQueue = SAFE_NEW() std::deque<GenericBuffer*>();
            m_emptyQueue      = SAFE_NEW() std::queue<GenericBuffer*>();
            m_filledQueue     = SAFE_NEW() std::queue<GenericBuffer*>();
        }
        else
        {
            m_emptyQueue  = SAFE_NEW() std::queue<GenericBuffer*>();
            m_filledQueue = SAFE_NEW() std::queue<GenericBuffer*>();
        }

        m_OutFileName = ConstructOutputFileName(cameraId).append(mapStreamStr[streamId]);

        uint32_t maxBuffers = GetMaxBuffersFromStream(configuredStream);
        if (0 == maxBuffers)
        {
            NT_LOG_ERROR("Stream %d cannot have maxBuffers of 0!", streamId);
            return -1;
        }

        for (uint32_t bufferIndex = 0; bufferIndex < maxBuffers; bufferIndex++)
        {
            GenericBuffer* streamBuffer = CreateGenericBuffer();
            if (streamBuffer == nullptr)
            {
                NT_LOG_ERROR("cannot create generic buffer for Stream %d!", streamId);
                return -1;
            }

            if (m_streamType == CAMERA3_STREAM_INPUT)
            {
                // Generate new input buffer with given input image
                if (0 != GenerateInputBuffer(streamBuffer, configuredStream, readImageFile.c_str()))
                {
                    NT_LOG_ERROR("cannot generate input buffer for Stream %d!", streamId);
                    return -1;
                }
                // Push the input buffer into the queue
                m_inputImageQueue->push_back(streamBuffer);
                // update the latest image read, to be used later for next batch of input buffers
                m_lastImageRead[inStreamIdx] = readImageFile; // @todo: remove this and update in GetNextImageName() instead
                // check if new input image available on device for next input buffer
                if (delayDifferentInput == 0) {
                    readImageFile = GetNextImageName(inStreamIdx);
                } else {
                    delayDifferentInput--;
                }
            }
            else
            {
                if (0 != GenerateOutputBuffer(streamBuffer, configuredStream))
                {
                    NT_LOG_ERROR("cannot generate output buffer for Stream %d!", streamId);
                    return -1;
                }
                m_emptyQueue->push(streamBuffer);
            }
        }

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetOutputBufferForRequest
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void* ChiBufferManager::GetOutputBufferForRequest()
    {
        m_pQueueMutex->Lock();

        GenericBuffer* pBuffer = !m_emptyQueue->empty() ? m_emptyQueue->front() : nullptr;
        int retries = 0;
        int bufferCount = 0;
        int statusCode = 0;
        int timeout = CmdLineParser::GetTimeoutOverride();

        while (nullptr == pBuffer && bufferCount < TIMEOUT_RETRIES)
        {
            while (false == m_bBufferAvailable && retries < TIMEOUT_RETRIES)
            {
                m_pQueueCond->TimedWait(m_pQueueMutex->GetNativeHandle(), timeout);
                retries++;
            }

            if (m_bBufferAvailable == false)
            {
                NT_LOG_ERROR("There is no buffer available to serve next request");
                statusCode = -1;
                break;
            }
            m_bBufferAvailable = false;
            pBuffer = !m_emptyQueue->empty() ? m_emptyQueue->front() : nullptr;
            bufferCount++;
        }

        if (pBuffer == nullptr && bufferCount >= TIMEOUT_RETRIES)
        {
            NT_LOG_ERROR("All buffers in queue are NULL");
            statusCode = -1;
        }

        if (statusCode == 0)
        {
            m_emptyQueue->pop();
            m_filledQueue->push(pBuffer);
        }

        m_pQueueMutex->Unlock();
        return (statusCode == 0 && pBuffer != nullptr) ? GetNativeBufferFromGeneric(pBuffer) : nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetInputBufferForRequest
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void* ChiBufferManager::GetInputBufferForRequest(bool bGetGenericBuffer)
    {
        m_pQueueMutex->Lock();
        GenericBuffer* pBuffer = nullptr;
        int retries = 0;
        int statusCode = 0;
        int timeout = CmdLineParser::GetTimeoutOverride();

        while (m_inputImageQueue->empty() && retries < TIMEOUT_RETRIES)
        {
            while (!m_bBufferAvailable && retries < TIMEOUT_RETRIES)
            {
                m_pQueueCond->TimedWait(m_pQueueMutex->GetNativeHandle(), timeout);
                retries++;
            }
            if (m_streamType == CAMERA3_STREAM_BIDIRECTIONAL)
            {
                m_bBufferAvailable = false;
            }
        }

        if (!m_inputImageQueue->empty())
        {
            pBuffer = m_inputImageQueue->front();
            retries = 0;
        }
        while (NULL == pBuffer && retries < TIMEOUT_RETRIES)
        {
            if (m_streamType == CAMERA3_STREAM_BIDIRECTIONAL)
            {
                while (!m_bBufferAvailable && retries < TIMEOUT_RETRIES)
                {
                    m_pQueueCond->TimedWait(m_pQueueMutex->GetNativeHandle(), timeout);
                    retries++;
                }
                m_bBufferAvailable = false;
            }
            else // m_streamType == CAMERA3_STREAM_INPUT
            {
                if (!m_inputImageQueue->empty())
                {
                    m_inputImageQueue->pop_front();
                }
                m_inputImageQueue->push_back(pBuffer);
            }

            pBuffer = !m_inputImageQueue->empty() ? m_inputImageQueue->front() : nullptr;
            retries++;
        }

        if (nullptr == pBuffer && retries >= TIMEOUT_RETRIES)
        {
            NT_LOG_ERROR("All buffers in queue are NULL");
            statusCode = -1;
        }

        if (NULL != pBuffer)
        {
            if (!m_inputImageQueue->empty())
            {
                m_inputImageQueue->pop_front();
            }
            m_inputImageQueue->push_back(pBuffer);
        }

        m_pQueueMutex->Unlock();

        if (bGetGenericBuffer == true)
        {
            return (statusCode == 0 && pBuffer != nullptr) ? static_cast<void*>(pBuffer) : nullptr; // returns GenericBuffer
        }
        return (statusCode == 0 && pBuffer != nullptr) ? GetNativeBufferFromGeneric(pBuffer) : nullptr; // returns NativeChiBuffer
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::initCamXChiBufferManager
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ChiBufferManager::initCamXChiBufferManager(GenericStream* pStream, std::string fName)
    {
        CHIBufferManagerCreateData createBuffer = { 0 };

        createBuffer.pChiStream = pStream->pChiStream;
        createBuffer.width      = pStream->pChiStream->width;
        createBuffer.height     = pStream->pChiStream->height;

        createBuffer.immediateBufferCount    = 0;
        createBuffer.minBufferCount          = 0;
        createBuffer.maxBufferCount          = pStream->pChiStream->maxNumBuffers;

        NT_LOG_INFO("Buffer type is %d", JsonParser::GetBufferType());
        createBuffer.bufferHeap         = JsonParser::GetBufferType();
        createBuffer.bEnableLateBinding = FALSE;

        createBuffer.consumerFlags = ChiGralloc1ProducerUsageCamera | ChiGralloc1ProducerUsageCpuRead |
            ChiGralloc1ProducerUsageCpuWrite;
        createBuffer.producerFlags = ChiGralloc1ConsumerUsageCamera | ChiGralloc1ConsumerUsageCpuRead;

        switch (pStream->pChiStream->format)
        {
            case ChiStreamFormatUBWCNV124R:
            {
                NT_LOG_WARN("Overriding format from ChiStreamFormatUBWCNV124R to ChiStreamFormatUBWCTP10 "
                    "as Gralloc does not support ChiStreamFormatUBWCNV124R");
                createBuffer.format = ChiStreamFormatUBWCTP10;
                break;
            }
            case ChiStreamFormatPD10:
            {
                NT_LOG_WARN("Overriding format from ChiStreamFormatPD10 to ChiStreamFormatRaw12 "
                    "as Gralloc does not support ChiStreamFormatPD10");
                createBuffer.format = ChiStreamFormatUBWCTP10;
                break;
            }
            case ChiStreamFormatBlob:
            {
                // CAMX has limitation that it needs to be aware of camera ID to allocate blob sizes correctly
                // for now, driver wants us to do the same way we do with Gralloc, x4 times to include header and 3A debug data
                createBuffer.width  = createBuffer.height * createBuffer.width * 4;
                createBuffer.height = 1;
                break;
            }
            default:
            {
                createBuffer.format = pStream->pChiStream->format;
                break;
            }
        }

        size_t i = fName.rfind('/', fName.length());

        std::string bufferMgrName = "";
        if (i != std::string::npos) {
            bufferMgrName = (fName.substr(i+1, fName.length() - i));
        }

        NT_LOG_INFO("Attempting to create CamX Chi Buffer Manager with name: %s", bufferMgrName.c_str());
        m_pCamXChiBufferMgr = m_pChiModule->GetChiBufferManagerOps()->pCreate(bufferMgrName.c_str(), &createBuffer);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GenerateOutputBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::GenerateOutputBuffer(GenericBuffer* pRequiredBuffer, GenericStream* pOutStream)
    {
        CDKResult opBufferResult = CDKResultSuccess;
        pRequiredBuffer->pChiBuffer->pStream = pOutStream->pChiStream;

        NT_LOG_INFO("Stream width: %d height: %d format: %d dataspace: %d", pOutStream->pChiStream->width,
            pOutStream->pChiStream->height, pOutStream->pChiStream->format, pOutStream->pChiStream->dataspace);

#ifdef _LINUX
        if (nullptr == m_pCamXChiBufferMgr)
        {
            NT_LOG_INFO("Attempting to create CamX Chi Buffer Manager for output stream");
            initCamXChiBufferManager(pOutStream, m_OutFileName);

            if (nullptr == m_pCamXChiBufferMgr)
            {
                NT_LOG_ERROR("Fail to create CHIBufferMananger for output stream");
                return -1;
            }

            NT_LOG_INFO("created CamX Chi Buffer Manager for output stream");
        }

        CHIBUFFERHANDLE pBufferHandle = m_pChiModule->GetChiBufferManagerOps()->pGetImageBuffer(m_pCamXChiBufferMgr);
        opBufferResult = m_pChiModule->GetChiBufferManagerOps()->pBindBuffer(pBufferHandle);
        if (opBufferResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to bind the image output buffer");
            return -1;
        }

        pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer   = pBufferHandle;
        pRequiredBuffer->pChiBuffer->bufferInfo.bufferType = CHIBUFFERTYPE::ChiNative;

#else
        // if PD10 format tests fail in Pre-Silicon, CamxMemAllocFunc in CamX needs implementation of PD10 memory allocation
        NT_LOG_DEBUG( "if PD10 format tests fail in Pre-Silicon,"
            "CamxMemAllocFunc in CamX needs implementation of PD10 memory allocation");
        CamxMemHandle bufferHandle;
        (*m_pCamxMemAlloc)(&bufferHandle, width, height, outputFormat, 0);
        pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer = reinterpret_cast<NativeChiBufferHandle*>(bufferHandle);
        pRequiredBuffer->pChiBuffer->bufferInfo.bufferType = CHIBUFFERTYPE::ChiNative;
#endif

        pRequiredBuffer->pChiBuffer->acquireFence = CreateFence(false);

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GenerateInputBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::GenerateInputBuffer(GenericBuffer* pRequiredBuffer, GenericStream* pInputStream,
        const char* filename)
    {
        CDKResult ipBufferResult = CDKResultSuccess;

        if (filename == nullptr)
        {
            NT_LOG_ERROR( "Filename is NULL");
            return -1;
        }
        void *pData;
        native_handle_t* pHandle;

        // Look for input image in the root location
        std::ostringstream completeFilePath;
        completeFilePath << RootPath << filename;

        FILE* pFile = OsUtils::FOpen(completeFilePath.str().c_str(), "rb");

        if (pFile == nullptr)
        {
            NT_LOG_ERROR( "Input file read failed");
            return -1;
        }

        OsUtils::FSeek(pFile, 0L, SEEK_END);    // Seek to end of file
        long long fileSize = OsUtils::FTell(pFile);
        NT_LOG_INFO("Input file %s: size is: %lld", completeFilePath.str().c_str(), fileSize);
        OsUtils::FSeek(pFile, 0L, SEEK_SET);    // Seek back to beginning of file

        int res = 0;

#ifdef _LINUX
        if (nullptr == m_pCamXChiBufferMgr)
        {
            NT_LOG_INFO("Attempting to create CamX Chi Buffer Manager for input stream");
            std::string s(filename);
            initCamXChiBufferManager(pInputStream, completeFilePath.str());

            if (nullptr == m_pCamXChiBufferMgr)
            {
                NT_LOG_ERROR("Fail to create CHIBufferMananger for input stream");
                return -1;
            }
            NT_LOG_INFO("created CamX Chi Buffer Manager for input stream");
        }

        CHIBUFFERHANDLE pBufferHandle = m_pChiModule->GetChiBufferManagerOps()->pGetImageBuffer(m_pCamXChiBufferMgr);
        ipBufferResult = m_pChiModule->GetChiBufferManagerOps()->pBindBuffer(pBufferHandle);
        if (ipBufferResult != CDKResultSuccess)
        {
            NT_LOG_ERROR("Unable to bind the image output buffer");
            return -1;
        }

        pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer   = pBufferHandle;
        pRequiredBuffer->pChiBuffer->bufferInfo.bufferType = CHIBUFFERTYPE::ChiNative;

        pData = const_cast<void *>(m_pChiModule->GetChiBufferManagerOps()->pGetCPUAddress(
            pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer));

#else
        CamxMemHandle bufferHandle;
        (*m_pCamxMemAlloc)(&bufferHandle, inputStream->pChiStream->width, inputStream->pChiStream->height, inputStream->pChiStream->format, 0);
        pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer = reinterpret_cast<buffer_handle_t*>(bufferHandle);
        pRequiredBuffer->pChiBuffer->bufferInfo.bufferType = CHIBUFFERTYPE::ChiNative;

        pHandle = *reinterpret_cast<native_handle_t**>(pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer);
        pData = *reinterpret_cast<void**>(pHandle->data);
#endif

        size_t read = 0;
        if (NULL != pData) {
            read = OsUtils::FRead(pData, /*unknown dest buffer size*/fileSize, 1, fileSize, pFile);
            res = OsUtils::FClose(pFile);
            if (static_cast<long>(read) != fileSize)
            {
                NT_LOG_ERROR("Failed to read file of size: %d, read: %d bytes.", fileSize, read);
                return -1;
            }
        }
        else
        {
            NT_LOG_ERROR("pData is null");
            return -1;
        }

    #ifdef _LINUX
        m_pChiModule->GetChiBufferManagerOps()->pCacheOps(&pRequiredBuffer->pChiBuffer->bufferInfo.phBuffer, TRUE, TRUE);
    #endif
        pRequiredBuffer->pChiBuffer->pStream = pInputStream->pChiStream;
        pRequiredBuffer->pChiBuffer->acquireFence = CreateFence(false);
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::SaveImageToFile
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::SaveImageToFile(GenericBuffer* resultBuffer, std::string filename)
    {
        int res;
        void* pData = nullptr;

        native_handle_t* phBuffer = *static_cast<native_handle_t**>(resultBuffer->pChiBuffer->bufferInfo.phBuffer);
        NativeChiStream* resultStream = resultBuffer->pChiBuffer->pStream;

        uint32_t output_width  = resultStream->width;
        uint32_t output_height = resultStream->height;
        uint32_t format        = resultStream->format;
        uint32_t usage         = resultStream->grallocUsage;

        size_t imageSize = 0;
        // Image formats we know how to calculate size for...
        // Though Raw Opaque is known to Gralloc, Driver is free to have its own Raw alignments in case of Raw Opaque, so
        // falling back to driver API to get the size for Opaque
        if (format == ChiStreamFormatBlob ||
            format == ChiStreamFormatRaw10 ||
            format == ChiStreamFormatY8 ||
            format == ChiStreamFormatY16)
        {
            imageSize = GetGrallocSize(output_width, output_height, format, resultStream->dataspace, reinterpret_cast<unsigned char *>(pData)).width;
        }
        else // Unknown image format size calculations: PD10, RAW64, NV12HEIF, P010
        {
            // now pre-silicon APIs are linked to image format mapper (post-silicon functions)
            NT_LOG_DEBUG( "CamxMemGetImageSize must have implementation for size calculation for format : %d", format);
            imageSize = (*m_pCamxMemGetImageSize)(output_width, output_height, format, usage);
        }

        if (0 == imageSize)
        {
            NT_LOG_ERROR("Calculated gralloc image size should not be 0!");
            return -1;
        }

        NT_LOG_DEBUG( "Image size to be saved is: %zu", imageSize);


#ifdef _LINUX

        pData = const_cast<void *>(m_pChiModule->GetChiBufferManagerOps()->pGetCPUAddress(GetHandleFromBuffer(resultBuffer)));

        // Ensure capture folder gets created or exists
        // Make upper camera folder first
        if (mkdir(ImageUpperPath.c_str(), 0777) != 0 && EEXIST != errno)
        {
            NT_LOG_ERROR( "Failed to create capture camera folder, Error: %d", errno);
            return -1;
        }

        // Make lower nativetest folder second
        if (mkdir(ImageRootPath.c_str(), 0777) != 0 && EEXIST != errno)
        {
            NT_LOG_ERROR( "Failed to create capture root folder, Error: %d", errno);
            return -1;
        }
#else

        pData = *reinterpret_cast<void**>(phBuffer->data);

        if (pData == nullptr)
        {
            NT_LOG_ERROR( "Buffer data is NULL");
            return -1;
        }

        // make sure capture folder gets created or exists
        if (_mkdir(ImageRootPath.c_str()) != 0)
        {
            DWORD error = GetLastError();
            if (ERROR_ALREADY_EXISTS != error)
            {
                NT_LOG_ERROR( "Failed to create capture folder, Error: %d", error);
                return -1;
            }
        }

#endif

        filename.append(GetFileExt(format));
        NT_LOG_DEBUG( "Saving image to file: %s", filename.c_str());

        char out_fname[256];
        OsUtils::SNPrintF(out_fname, sizeof(out_fname), "%s", filename.c_str());

        FILE* pFile = OsUtils::FOpen(out_fname, "wb");
        if (pFile == nullptr)
        {
            NT_LOG_ERROR( "Output file creation failed");
            return -1;
        }

        size_t written = 0;

        written = OsUtils::FWrite(reinterpret_cast<unsigned char *>(pData), 1, imageSize, pFile);

        if (written != imageSize)
        {
            NT_LOG_ERROR("failed to write: %s (written: %zu)", out_fname, written);
            return -1;
        }

    #ifdef _LINUX
        m_pChiModule->GetChiBufferManagerOps()->pCacheOps(GetHandleFromBuffer(resultBuffer), TRUE, TRUE);
    #endif //_LINUX
        res = OsUtils::FClose(pFile);
        if (res != 0)
        {
            NT_LOG_ERROR( "Close file failed");
            return -1;
        }
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetNextImageName
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string ChiBufferManager::GetNextImageName(int streamIdx)
    {
        std::string currentFile = m_lastImageRead[streamIdx]; // keep original copy in case new file not found
        std::string nextFile    = m_lastImageRead[streamIdx]; // to be modified to look for next image name

        if (m_lastImageRead[streamIdx].length() >= UINT_MAX)
        {
            NT_LOG_WARN("File name length cannot exceed %d, hence returning current file name", UINT_MAX);
            return currentFile;
        }

        // it has be int, because std::string::npos can be returned, which is -1
        auto const posDelimiter1 = nextFile.find_last_of("_");    // position of 1st delimiter's last ocurrence
        auto const posDelimiter2 = nextFile.find_last_of(".");    // position of 2nd delimiter's last ocurrence
        // if delimiters are not found, npos will be stored which is -1
        if (posDelimiter1 == std::string::npos || posDelimiter2 == std::string::npos)
        {
            NT_LOG_WARN("Could not read next input image. Using previous file : %s", currentFile.c_str());
            return currentFile;
        }

        // Get the substring (image number) between the two delimiters (if present)
        std::string imageNumberStr = nextFile.substr(posDelimiter1 + 1, posDelimiter2 - posDelimiter1 - 1);
        bool bHasDigitsOnly = imageNumberStr.find_first_not_of("0123456789") == std::string::npos;
        if (!bHasDigitsOnly) // if file name does not have digits to increment
        {
            NT_LOG_WARN("Could not read next input image. Using previous file : %s", currentFile.c_str());
            return currentFile;
        }

        int imageNumber = std::stoi(imageNumberStr);
        nextFile.replace(posDelimiter1 + 1, posDelimiter2 - posDelimiter1 - 1, std::to_string(++imageNumber));
        // Check if new file present, else use current file name
        std::ostringstream completeFilePath;
        completeFilePath << RootPath << nextFile;
        FILE* pFile = OsUtils::FOpen(completeFilePath.str().c_str(), "rb");

        if (pFile == nullptr)
        {
            NT_LOG_WARN("Could not read next input image. Using previous file : %s", currentFile.c_str());
            return currentFile;
        }
        // close the file and return its name
        OsUtils::FClose(pFile);

        /* @todo: Enable this back and fix image read logic in GenerateBuffers()
        m_lastImageRead[streamIdx] = nextFile;
        */

        return nextFile;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::CreateGenericBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiBufferManager::GenericBuffer* ChiBufferManager::CreateGenericBuffer()
    {
        GenericBuffer* newGenericBuffer = SAFE_NEW() GenericBuffer();
        if (newGenericBuffer != nullptr)
        {
            NativeChiBuffer* newChiBuffer = SAFE_NEW() NativeChiBuffer();
            if (newChiBuffer != nullptr)
            {
                newChiBuffer->size = sizeof(NativeChiBuffer);
                newGenericBuffer->pChiBuffer = newChiBuffer;
                return newGenericBuffer;
            }
        }
        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::DestroyGenericBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ChiBufferManager::DestroyGenericBuffer(GenericBuffer* buffer)
    {
        if (buffer != nullptr)
        {
            if (buffer->pChiBuffer != nullptr)
            {
                DestroyFence(buffer->pChiBuffer->acquireFence);
                DestroyFence(buffer->pChiBuffer->releaseFence);

                delete buffer->pChiBuffer;
            }
            delete buffer;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ReleaseReferenceToBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ChiBufferManager::ReleaseReferenceToBuffer(CHIBUFFERHANDLE pHandle)
    {
        return m_pChiModule->GetChiBufferManagerOps()->pReleaseReference(m_pCamXChiBufferMgr, pHandle);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   ChiBufferManager::GetTypeFromStream
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::GetTypeFromStream(GenericStream* stream)
    {
        return stream->pChiStream->streamType;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetMaxBuffersFromStream
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t ChiBufferManager::GetMaxBuffersFromStream(GenericStream* stream)
    {
        return stream->pChiStream->maxNumBuffers;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetNativeBufferFromGeneric
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void* ChiBufferManager::GetNativeBufferFromGeneric(GenericBuffer* buffer)
    {
        return static_cast<void*>(buffer->pChiBuffer);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetHandleFromBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiBufferManager::NativeChiBufferHandle ChiBufferManager::GetHandleFromBuffer(GenericBuffer* buffer)
    {
        return buffer->pChiBuffer->bufferInfo.phBuffer;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetStatusFromBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::GetStatusFromBuffer(GenericBuffer* buffer)
    {
        return buffer->pChiBuffer->bufferStatus;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetReleaseFenceFromBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::GetReleaseFenceFromBuffer(GenericBuffer* buffer)
    {
        if (buffer->pChiBuffer->releaseFence.valid == true)
        {
            CDKResult* pFenceResult = NULL;
            CDKResult res = m_pChiModule->GetFenceOps()->pGetFenceStatus(m_pChiModule->GetContext(),
                buffer->pChiBuffer->releaseFence.hChiFence, pFenceResult);
            if (CDKResultSuccess == res && NULL != pFenceResult)
            {
                if (CDKResultSuccess != *pFenceResult)
                {
                    return 0;   // TODO fence is busy?
                }
                else
                {
                    return -1;  // Fence not busy
                }
            }
            NT_LOG_ERROR( "Failure when getting release fence status!");
            return -1;  // TODO what to return for failure?
        }
        // If fence not valid, then assume no waiting
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::SetAcquireFenceInBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ChiBufferManager::SetAcquireFenceInBuffer(GenericBuffer* buffer, int setVal)
    {
        if (-1 == setVal)
        {
            DestroyFence(buffer->pChiBuffer->acquireFence);
        }
        else if (buffer->pChiBuffer->acquireFence.valid == true)
        {
            // TODO hold the fence?
            PFNCHIFENCECALLBACK dummyCallback = NULL;
            VOID* pData = NULL;
            m_pChiModule->GetFenceOps()->pWaitFenceAsync(m_pChiModule->GetContext(), dummyCallback,
                buffer->pChiBuffer->acquireFence.hChiFence, pData);
        }
        // If not valid, do nothing
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::GetMaxBuffersFromBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t ChiBufferManager::GetMaxBuffersFromBuffer(GenericBuffer* buffer)
    {
        return buffer->pChiBuffer->pStream->maxNumBuffers;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::CreateFence
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHIFENCEINFO ChiBufferManager::CreateFence(bool isValid)
    {
        CHIFENCEINFO fenceInfo;
        fenceInfo.valid = false;
        fenceInfo.type = ChiFenceTypeInternal;

        if (isValid)
        {
            CHIFENCEHANDLE* pFenceHandle = SAFE_NEW() CHIFENCEHANDLE;
            CDKResult res = m_pChiModule->GetFenceOps()->pCreateFence(m_pChiModule->GetContext(), NULL/*TODO*/, pFenceHandle);

            if (res == CDKResultSuccess)
            {
                fenceInfo.valid = true;
                fenceInfo.hChiFence = *pFenceHandle;
            }
        }

        return fenceInfo;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::DestroyFence
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ChiBufferManager::DestroyFence(CHIFENCEINFO fenceInfo)
    {
        if (fenceInfo.valid == true)
        {
            fenceInfo.valid = false;
            CHIFENCEHANDLE* pFenceHandle = &(fenceInfo.hChiFence);

            m_pChiModule->GetFenceOps()->pReleaseFence(m_pChiModule->GetContext(), fenceInfo.hChiFence);
            if (pFenceHandle != NULL)
            {
                delete pFenceHandle;
            }
        }
        // If not valid, then no action needed
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::DestroyBuffers
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ChiBufferManager::DestroyBuffers()
    {
        if (m_emptyQueue != nullptr)
        {
            while (!m_emptyQueue->empty())
            {
                GenericBuffer* currBuffer = static_cast<GenericBuffer*>(m_emptyQueue->front());
                if (currBuffer == nullptr || GetReleaseFenceFromBuffer(currBuffer) != -1)
                {
                    m_emptyQueue->pop();
                    continue;
                }

                native_handle_t* phandle = *static_cast<native_handle_t**>(GetHandleFromBuffer(currBuffer));
                if (phandle == nullptr)
                {
                    m_emptyQueue->pop();
                    DestroyGenericBuffer(currBuffer);
                    continue;
                }

#ifndef _LINUX
                (*m_pCamxMemRelease)(reinterpret_cast<CamxMemHandle>(phandle));
#endif
                DestroyGenericBuffer(currBuffer);
                m_emptyQueue->pop();
            }

            delete m_emptyQueue;
            m_emptyQueue = nullptr;
        }

        if (m_filledQueue != nullptr)
        {
            while (!m_filledQueue->empty())
            {
                GenericBuffer* currBuffer = reinterpret_cast<GenericBuffer*>(m_filledQueue->front());
                if (currBuffer == nullptr || GetReleaseFenceFromBuffer(currBuffer) != -1)
                {
                    m_filledQueue->pop();
                    continue;
                }

                native_handle_t* phandle = *static_cast<native_handle_t**>(GetHandleFromBuffer(currBuffer));
                if (phandle == nullptr)
                {
                    m_filledQueue->pop();
                    DestroyGenericBuffer(currBuffer);
                    continue;
                }

#ifndef _LINUX
                (*m_pCamxMemRelease)(reinterpret_cast<CamxMemHandle>(phandle));
#endif
                DestroyGenericBuffer(currBuffer);
                m_filledQueue->pop();
            }

            delete m_filledQueue;
            m_filledQueue = nullptr;
        }

        if (m_inputImageQueue != nullptr)
        {
            // No new buffers created for m_inputImageQueue for bidirectional streams, so do not deallocate
            if (m_streamType != CAMERA3_STREAM_BIDIRECTIONAL)
            {
                while (!m_inputImageQueue->empty())
                {
                    GenericBuffer* currBuffer = reinterpret_cast<GenericBuffer*>(m_inputImageQueue->front());
                    if (currBuffer == nullptr || GetReleaseFenceFromBuffer(currBuffer) != -1)
                    {
                        m_inputImageQueue->pop_front();
                        continue;
                    }

                    native_handle_t* phandle = *static_cast<native_handle_t**>(GetHandleFromBuffer(currBuffer));
                    if (phandle == nullptr)
                    {
                        m_inputImageQueue->pop_front();
                        DestroyGenericBuffer(currBuffer);
                        continue;
                    }

#ifndef _LINUX
                    (*m_pCamxMemRelease)(reinterpret_cast<CamxMemHandle>(phandle));
#endif
                    DestroyGenericBuffer(currBuffer);
                    m_inputImageQueue->pop_front();
                }
            }

            delete m_inputImageQueue;
            m_inputImageQueue = nullptr;
        }

        if (nullptr != m_pCamXChiBufferMgr)
        {
            m_pChiModule->GetChiBufferManagerOps()->pDestroy(m_pCamXChiBufferMgr);
            m_pCamXChiBufferMgr = nullptr;
        }

        delete this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ReadFDDatabase
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ChiBufferManager::ReadFDDatabase(std::string FDTestImage)
    {
        // Run database testing only if valid FD input first frame present on device
        std::ifstream  validInputFrame(FDFirstInputFrame);
        if (!validInputFrame)
        {
            NT_LOG_ERROR("Could not open/access valid FD input frame %s", FDFirstInputFrame.c_str());
            return CDKResultEFailed;
        }

        // Read the standard FD ground truth file on device
        std::ifstream validGroundTruth(FDStandardGroundTruthFile);
        if (!validGroundTruth)
        {
            NT_LOG_ERROR("Cannot open/access %s", FDStandardGroundTruthFile.c_str());
            return CDKResultEFailed;
        }

        // For snapshot usecase, check if FD test scene name provided at runtime
        if (FDTestImage == "" && !JsonParser::IsVideoFDTest())
        {
            NT_LOG_ERROR("FD test image scene name for snapshot not provided");
            return CDKResultEFailed;
        }
        // Snapshot : loop through file till scene name found; then parse once and use same data for all frames
        // Video    : loop through file and parse each line for per-frame face ROI data
        std::string readLine;
        while (std::getline(validGroundTruth, readLine))
        {
            // For snapshot usecase, check if image found in the list
            if (!JsonParser::IsVideoFDTest() && readLine.find(FDTestImage) == std::string::npos)
            {
                continue; // keep looking for given scene name
            }

            FrameDataFD targetData; // FaceROI data parsed per frame

            // get number of faces from next line
            std::getline(validGroundTruth, readLine);
            targetData.faceCount = std::stoi(readLine);
            FaceDataFD* faceData = new FaceDataFD[targetData.faceCount];

            // get ROI for each face
            for (UINT32 faceIndex = 0; faceIndex < targetData.faceCount; faceIndex++)
            {
                RectangleCoordinate faceROI;
                std::getline(validGroundTruth, readLine); // read the ROI data
                std::string delimiter = " "; // using space as delimiter as per standard ground truth file
                size_t delimPosition = 0;
                int coordIndex = 0; // iterate over all face ROI coordinates
                while ((delimPosition = readLine.find(delimiter)) != std::string::npos) // sequence: left, top, width and height
                {
                    int coordValue = std::stoi(readLine.substr(0, delimPosition));
                    switch (coordIndex)
                    {
                    case 0: faceROI.left = coordValue;
                        break;
                    case 1: faceROI.top = coordValue;
                        break;
                    case 2: faceROI.width = coordValue;
                        break;
                    default:
                        NT_LOG_ERROR("Invalid face rectangle coordinate index!");
                        break;
                    }
                    readLine.erase(0, delimPosition + delimiter.length());
                    ++coordIndex; // assign next face ROI value to next rectangle coordinate
                }
                // remaining part gives the last face ROI coordinate value
                int coordValue = std::stoi(readLine);
                faceROI.height = coordValue;
                faceData[faceIndex].faceROI = faceROI;
            }
            targetData.faceData = faceData;
            targetData.faceCountStatus = ExactFaceCount;
            targetData.frameID = mTargetFaceData.size();
            mTargetFaceData.push_back(targetData);
        }
        if(mTargetFaceData.size()) // enable FD database testing, if parsing successful
        {
            ChiBufferManager::m_isFDDatabaseTest = true;
            return CDKResultSuccess;
        }
        else // no data could be parsed
        {
            NT_LOG_WARN("Could not parse ground truth data. Please check test image input for the scene.");
            return CDKResultEFailed;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::CalculateIOUForResultBuffer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ChiBufferManager::CalculateIOUForResultBuffer(FaceROIInformation* receivedBufferROI)
    {
        // per frame FD data expected as per ground truth
        FrameDataFD targetData;
        if(JsonParser::IsVideoFDTest()) // use per-frame data (video)
        {
            if(receivedBufferROI->requestId > mTargetFaceData.size())
            {
                NT_LOG_ERROR("No GT data available for frame %u", receivedBufferROI->requestId);
                return CDKResultEFailed;
            }
            targetData = mTargetFaceData[receivedBufferROI->requestId - 1];
        }
        else // use same data for all frames (snapshot)
        {
            targetData = mTargetFaceData[0];
        }

        // per frame FD result sent by driver
        FrameDataFD bufferResult;
        bufferResult.frameID   = receivedBufferROI->requestId;
        bufferResult.faceCount = receivedBufferROI->ROICount;  // number of faces detected by driver

        // Check for false positive cases
        if (bufferResult.faceCount > targetData.faceCount)
        {
            bufferResult.faceCountStatus = FalsePositive;
        }
        else if (bufferResult.faceCount < targetData.faceCount)
        {
            bufferResult.faceCountStatus = LessThanExpected;
        }
        else
        {
            bufferResult.faceCountStatus = ExactFaceCount;
        }

        bufferResult.faceData = new FaceDataFD[targetData.faceCount]; // to store data for all faces in this result frame
        UINT64 goodFaceCount  = 0; // for snapshot usecase only

        // Calculate the IoU for each face expected
        for (UINT32 faceIter = 0; faceIter < targetData.faceCount; faceIter++)
        {
            bufferResult.faceData[faceIter].faceId = faceIter + 1;

            // Set IOU and confidence to 0 for any face expected by GT but not detected by driver
            if (faceIter > bufferResult.faceCount || bufferResult.faceCount == 0)
            {
                bufferResult.faceData[faceIter].faceIOU    = 0;
                bufferResult.faceData[faceIter].confidence = 0;
                bufferResult.faceData[faceIter].faceROI    = { 0 };
            }
            else // proceed with IOU calculation for actual detected face
            {
                RectangleCoordinate receivedRectangle = receivedBufferROI->stabilizedROI[faceIter].faceRect;
                // todo: set the order of faces as per best possible match; do not follow order sent by driver
                bufferResult.faceData[faceIter].faceROI  = receivedRectangle;

                // initialize
                float maxFaceIOU = 0; // initial IOU
                bufferResult.faceData[faceIter].faceIOU    = maxFaceIOU;
                bufferResult.faceData[faceIter].confidence = receivedBufferROI->stabilizedROI[faceIter].confidence;
                // Since driver's order of faces may not match with ground truth order, compare IOU with all faces, and use max
                // todo: Find better solution for this problem
                for (UINT32 gtFaceIter = 0; gtFaceIter < targetData.faceCount; gtFaceIter++)
                {
                    RectangleCoordinate targetRectangle = targetData.faceData[gtFaceIter].faceROI;
                    UINT32 uint32_1 = static_cast<UINT32>(1);
                    float currentFaceIOU = maxFaceIOU;

                    /* UBSan / Integer overflow check before calculation
                    Should not cross (UINT_MAX/2), to avoid going over UINT_MAX in later calculations */
                    if (receivedRectangle.left              >= UINT_MAX / 2 ||
                        receivedRectangle.top               >= UINT_MAX / 2 ||
                        receivedRectangle.width             >= UINT_MAX / 2 ||
                        receivedRectangle.height            >= UINT_MAX / 2 ||
                        targetRectangle.left                >= UINT_MAX / 2 ||
                        targetRectangle.top                 >= UINT_MAX / 2 ||
                        targetRectangle.width               >= UINT_MAX / 2 ||
                        targetRectangle.height              >= UINT_MAX / 2 ||
                        targetRectangle.width    + uint32_1 >= UINT_MAX / 2 ||
                        targetRectangle.height   + uint32_1 >= UINT_MAX / 2 ||
                        receivedRectangle.width  + uint32_1 >= UINT_MAX / 2 ||
                        receivedRectangle.height + uint32_1 >= UINT_MAX / 2)
                    {
                        // Non fatal error
                        NT_LOG_WARN("UBSan Check Failed: RectangleCoordinate values out of bounds. \
                                Cannot calculate IOU for request: %d, face: %d", receivedBufferROI->requestId, faceIter);
                    }
                    else
                    {
                        // Coordinates of intersection rectangle
                        int xA = std::max(receivedRectangle.left, targetRectangle.left);
                        int yA = std::max(receivedRectangle.top, targetRectangle.top);
                        int xB = std::min(receivedRectangle.left + receivedRectangle.width,
                        targetRectangle.left + targetRectangle.width);
                        int yB = std::min(receivedRectangle.top + receivedRectangle.height,
                        targetRectangle.top + targetRectangle.height);

                        // UBSan check
                        if (xA > xB || yA > yB)
                        {
                            // Check if rectangles intersect for IOU calculation; if not, IOU is not calculated and 0 is used
                            // this warning is printed several time while matching given face with other face ROIs for maxFaceIOU
                            NT_LOG_WARN("Invalid intersection rectangle. Cannot calculate IOU for request: %d, face: %d",
                            receivedBufferROI->requestId, faceIter + 1);
                        }
                        else  // calculate IOU
                        {
                            int intersectionArea = (xB - xA + 1) * (yB - yA + 1);

                            UINT64 areaBoxA = (targetRectangle.width + uint32_1)   * (targetRectangle.height + uint32_1);
                            UINT64 areaBoxB = (receivedRectangle.width + uint32_1) * (receivedRectangle.height + uint32_1);

                            currentFaceIOU = static_cast<float>(intersectionArea) /
                                            static_cast<float>(areaBoxA + areaBoxB - intersectionArea);
                        }
                    }
                    if (currentFaceIOU > maxFaceIOU)
                    {
                        maxFaceIOU = currentFaceIOU;
                    }
                }

                bufferResult.faceData[faceIter].faceIOU = maxFaceIOU;   // update
                // if current face detected for first time, add the frame number (IOU 0 to 0.45)
                if ((bufferResult.faceData[faceIter].faceIOU > 0) && (UINT32(m_firstDetectionFrameList.size()) == faceIter))
                {
                    m_firstDetectionFrameList.push_back(receivedBufferROI->requestId);
                    m_firstDetectionTimeList.push_back(GetTimeElapsed(m_startTimeFD));
                }
                if (bufferResult.faceData[faceIter].faceIOU > 0)
                {
                    goodFaceCount++;
                }
            }
            // Mark as low IOU face, if current face IOU < lower limit
            if (bufferResult.faceData[faceIter].faceIOU < IOU_LOWER_LIMIT)
            {
                bufferResult.faceData[faceIter].hasLowIOU = true;
            }
            else
            {
                bufferResult.faceData[faceIter].hasLowIOU  = false;
            }
        }

        // if all expected faces detected for first time (IOU can be < 0.45)
        if (goodFaceCount == targetData.faceCount && m_firstGoodFrameOverall == 0)
        {
            m_firstGoodFrameOverall = receivedBufferROI->requestId;
        }

        mTestFaceData.push_back(bufferResult);
        return CDKResultSuccess;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::PrintFDMetrics
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void ChiBufferManager::PrintFDMetrics()
    {
        // Initialize metric related variables
        int    totalResults = mTestFaceData.size();  // total results sent by driver
        std::vector<FrameDataFD>::iterator  pResultFD = mTestFaceData.begin();  // per frame result from driver
        std::vector<FrameDataFD>::iterator  pDataGT   = mTargetFaceData.begin();  // per frame result from GT file
        std::vector<FaceMetrics> FaceMetricsList;
        bool   bInitialized[MaxFaceROIs] = { false };                   // to check if each face metric set is initialized
        float  lastFrameIOU[MaxFaceROIs] = { 0 };                       // List of face IOUs in previous frame

        UINT64 initialPredetectionFrames = 0;
        // subtract first few frames before stabilized detection
        if (m_firstGoodFrameOverall > 0)
        {
            initialPredetectionFrames = m_firstGoodFrameOverall - 1;
            totalResults -= initialPredetectionFrames;
        }

        if(!JsonParser::IsVideoFDTest()) // generate these metrics for snapshot usecase only
        {
            // Initialize a list of per face FD metrics
            for (UINT8 faceIter = 0; faceIter < mTargetFaceData[0].faceCount; faceIter++)
            {
                FaceMetrics  currentFaceMetrics = { 0 };
                currentFaceMetrics.faceId = faceIter;
                // Initialize first good frame here to avoid unnecessary looping later
                if (faceIter < m_firstDetectionFrameList.size())
                {
                    currentFaceMetrics.firstGoodFrame = m_firstDetectionFrameList[faceIter];
                    currentFaceMetrics.firstDetectionTime = m_firstDetectionTimeList[faceIter];
                }
                FaceMetricsList.push_back(currentFaceMetrics);
            }
        }

        OverallFDMetrics  OverallMetrics = { 0 };

        // Print per frame metrics
        for (; pResultFD < mTestFaceData.end(); pResultFD++)
        {
            UINT64 frameId = pResultFD->frameID;
            if (pResultFD->faceCountStatus == FalsePositive)
            {
                OverallMetrics.numFalsePositives++;
            }
            else if (pResultFD->faceCountStatus == LessThanExpected)
            {
                OverallMetrics.numFramesDroppedFace++;
            }

            NT_LOG_PERF("[GroundTruth] Frame:%u, faceCount:%u", frameId, pDataGT->faceCount);
            NT_LOG_PERF("[   FDResult] Frame:%u, faceCount:%u", frameId, pResultFD->faceCount);
            for(UINT32 faceId = 0; faceId < pDataGT->faceCount; faceId++)
            {
                if(!JsonParser::IsVideoFDTest()) // generate these metrics for snapshot usecase only
                {
                    // skip the face result for initial bad frames
                    if (frameId < FaceMetricsList[faceId].firstGoodFrame)
                    {
                        continue;
                    }

                    float currentFaceIOU = pResultFD->faceData[faceId].faceIOU;

                    if (!bInitialized[faceId])  // one time initilization
                    {
                        FaceMetricsList[faceId].bestIOU = currentFaceIOU;
                        FaceMetricsList[faceId].bestFrame = frameId;
                        FaceMetricsList[faceId].worstIOU = currentFaceIOU;
                        FaceMetricsList[faceId].worstFrame = frameId;
                        bInitialized[faceId] = true;
                    }
                    else // if already initialized, update results
                    {
                        // update current face FD metric for worse IOU
                        if (currentFaceIOU < FaceMetricsList[faceId].worstIOU)
                        {
                            FaceMetricsList[faceId].worstIOU = currentFaceIOU;
                            FaceMetricsList[faceId].worstFrame = frameId;
                        }
                        // update current face FD metric for best IOU
                        if (currentFaceIOU > FaceMetricsList[faceId].bestIOU)
                        {
                            FaceMetricsList[faceId].bestIOU = currentFaceIOU;
                            FaceMetricsList[faceId].bestFrame = frameId;
                        }

                        // Fluctuation between consecutive frame IOUs should be < 50% for each face
                        if (IOUShift(currentFaceIOU, lastFrameIOU[faceId]) > FD_FLUCTUATION_TOLERANCE)
                        {
                            FaceMetricsList[faceId].numFluctuations++;
                        }
                    }

                    FaceMetricsList[faceId].sumIOU += currentFaceIOU;
                    FaceMetricsList[faceId].sumConfidence += pResultFD->faceData[faceId].confidence;
                    if (pResultFD->faceData[faceId].hasLowIOU)
                    {
                        FaceMetricsList[faceId].numLowIOUFrames++;
                    }

                    lastFrameIOU[faceId] = currentFaceIOU;
                }

                NT_LOG_PERF("[GroundTruth]  faceId:%u, left:%d, top:%d, width:%d, height:%d",
                                           faceId, pDataGT->faceData[faceId].faceROI.left,
                                                   pDataGT->faceData[faceId].faceROI.top,
                                                   pDataGT->faceData[faceId].faceROI.width,
                                                   pDataGT->faceData[faceId].faceROI.height);
                NT_LOG_PERF("[   FDResult]  faceId:%u, left:%d, top:%d, width:%d, height:%d, IOU: %.2f, Confidence: %u",
                                           faceId, pResultFD->faceData[faceId].faceROI.left,
                                                   pResultFD->faceData[faceId].faceROI.top,
                                                   pResultFD->faceData[faceId].faceROI.width,
                                                   pResultFD->faceData[faceId].faceROI.height,
                                                   pResultFD->faceData[faceId].faceIOU,
                                                   pResultFD->faceData[faceId].confidence);
            }
            if(JsonParser::IsVideoFDTest())
            {
                pDataGT++; // For video, move to next frame data in GT (use same data for all frames in snapshot usecase)
            }
        }

        // discard initial predetection frames
        if(initialPredetectionFrames <= OverallMetrics.numFramesDroppedFace)
        {
            OverallMetrics.numFramesDroppedFace -= initialPredetectionFrames;
        }

        // Print overall metrics common to video and snapshot usecases
        NT_LOG_PERF("--------------------- FD Performance Metrics --------------------- ");
        NT_LOG_PERF("Number of initial frames before stabilized detection : %u ", initialPredetectionFrames);
        NT_LOG_PERF("Number of frames with less than expected face count since first detection : %u / %d",
            OverallMetrics.numFramesDroppedFace, totalResults);
        NT_LOG_PERF("Number of false positive frames : %u / %d", OverallMetrics.numFalsePositives, totalResults);
        NT_LOG_PERF("First frame with all faces detected : %u ", m_firstGoodFrameOverall);

        // Print metrics for snapshot usecase
        if(!JsonParser::IsVideoFDTest())
        {
            // Calculate average IOU in separate loop to reduce number of calculations
            for (std::vector<FaceMetrics>::iterator pMetricIter = FaceMetricsList.begin();
                pMetricIter != FaceMetricsList.end(); ++pMetricIter)
            {
                pMetricIter->meanIOU = pMetricIter->sumIOU / float(totalResults);
                pMetricIter->meanConfidence = pMetricIter->sumConfidence / totalResults;
            }

            for (std::vector<FaceMetrics>::iterator pMetricIter = FaceMetricsList.begin();
                pMetricIter != FaceMetricsList.end(); ++pMetricIter)
            {
                NT_LOG_PERF("Face ID: %u [Best  IOU]: %.2f, frame: %u",
                    pMetricIter->faceId, pMetricIter->bestIOU, pMetricIter->bestFrame);
                NT_LOG_PERF("Face ID: %u [Worst IOU]: %.2f, frame: %u",
                    pMetricIter->faceId, pMetricIter->worstIOU, pMetricIter->worstFrame);
                NT_LOG_PERF("Face ID: %u [Mean  IOU]: %.2f", pMetricIter->faceId, pMetricIter->meanIOU);
                NT_LOG_PERF("Face ID: %u Mean Confidence: %u", pMetricIter->faceId, pMetricIter->meanConfidence);
                NT_LOG_PERF("Face ID: %u First Detected At Frame ID : %u", pMetricIter->faceId, pMetricIter->firstGoodFrame);
                NT_LOG_PERF("Face ID: %u Time taken for first detected result : %.2f milliseconds",
                    pMetricIter->faceId, pMetricIter->firstDetectionTime);
                NT_LOG_PERF("Face ID: %u Number of frames with low IOU (< 0.45) : %u / %d",
                    pMetricIter->faceId, pMetricIter->numLowIOUFrames, totalResults);
                NT_LOG_PERF("Face ID: %u Number of fluctuations in IOU (> 50%%)  : %u \n",
                    pMetricIter->faceId, pMetricIter->numFluctuations);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ReadFaceROIFromBuffer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ChiBufferManager::ReadFaceROIFromBuffer(GenericBuffer* resultBuffer)
    {
        int              res;
        void*            pData    = nullptr;

    #ifdef _LINUX
        pData = const_cast<void *>(m_pChiModule->GetChiBufferManagerOps()->pGetCPUAddress(GetHandleFromBuffer(resultBuffer)));
    #else
        native_handle_t* phBuffer = *static_cast<native_handle_t**>(GetHandleFromBuffer(resultBuffer));
        pData = *reinterpret_cast<void**>(phBuffer->data);
    #endif

        if (nullptr == pData)
        {
            NT_LOG_ERROR("Buffer data is NULL");
            return CDKResultEFailed;  //fatal error
        }
        // process the buffer for face ROI data and add to list
        FaceROIInformation receivedBufferROI;

        ChxUtils::Memcpy(&receivedBufferROI, reinterpret_cast<unsigned char *>(pData), sizeof(FaceROIInformation));

        return CalculateIOUForResultBuffer(&receivedBufferROI);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiBufferManager::ProcessBufferFromResult
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int ChiBufferManager::ProcessBufferFromResult(const int frameNumber, GenericBuffer* resultBuffer, bool dump)
    {
        int result = 0;

        if (nullptr != m_filledQueue)
        {
            m_pQueueMutex->Lock();

            result = 0;

            GenericBuffer* pQueuedStreamBuffer = static_cast<GenericBuffer*>(m_filledQueue->front());

            if (pQueuedStreamBuffer == nullptr)
            {
                NT_LOG_ERROR("Filled Queue has a NULL buffer");
                m_pQueueMutex->Unlock();
                return -1;
            }

            m_filledQueue->pop();

            if (GetHandleFromBuffer(pQueuedStreamBuffer) == GetHandleFromBuffer(resultBuffer))
            {
                if (CAMERA3_BUFFER_STATUS_OK == GetStatusFromBuffer(resultBuffer))
                {
                    if (GetReleaseFenceFromBuffer(resultBuffer) == -1)
                    {
                        if (CmdLineParser::NoImageDump())
                        {
                            NT_LOG_WARN("Result image dump is disabled, skipping...");
                        }
                        else if (dump)
                        {
                            std::ostringstream outputName;
                            outputName << m_OutFileName;
                            if (frameNumber != -1)
                            {
                                outputName << "_Frame" << frameNumber;
                            }

                            result = SaveImageToFile(resultBuffer, outputName.str());
                        }

                        if (m_streamType == CAMERA3_STREAM_BIDIRECTIONAL)
                        {
                            if (m_inputImageQueue != nullptr)
                            {
                                if (m_inputImageQueue->size() == GetMaxBuffersFromBuffer(resultBuffer))
                                {
                                    m_inputImageQueue->pop_back();
                                }

                                m_inputImageQueue->push_front(pQueuedStreamBuffer);
                            }
                            else
                            {
                                NT_LOG_ERROR("inputImage Queue is NULL");
                                result = -1;
                            }
                        }

                        SetAcquireFenceInBuffer(pQueuedStreamBuffer, -1);
                        m_emptyQueue->push(pQueuedStreamBuffer);
                    }
                    else
                    {
                        NT_LOG_ERROR("Release fence is not set for frame: %d", frameNumber);
                        result = -1;
                    }
                }
                else
                {
                    NT_LOG_ERROR("Received buffer in error for frame: %d", frameNumber);
                    result = -1;
                }
            }
            else
            {
                NT_LOG_ERROR("Received buffer in non-FIFO order for frame: %d!", frameNumber);
                result = -1;
            }

            m_pQueueMutex->Unlock();

            if (result == 0)
            {
                m_bBufferAvailable = true;
                m_pQueueCond->Signal();
            }
        }
        else
        {
            NT_LOG_ERROR("Filled buffer queue is NULL");
            result = -1;
        }

        if (result == 0) {
            // only for FD database test, get face ROI data from the output buffer
            if (m_isFDDatabaseTest)
            {
                if (CDKResultSuccess != ReadFaceROIFromBuffer(resultBuffer))
                {
                    result = -1;
                }
            }
        }

        return result;
    }
} // namespace chitests
