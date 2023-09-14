////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  commonutilslinux.cpp
/// @brief General OS specific utility class implementation for Linux
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (_LINUX)            // This file for Linux build only

#include <dlfcn.h>                  // dynamic linking
#include <errno.h>                  // errno
#include <stdarg.h>
#include <stdlib.h>                 // posix_memalign, free
#include <string.h>                 // strlcat
#include <sys/mman.h>               // memory management
#include <sys/time.h>
#include <unistd.h>                 // library functions
#include <time.h>
#include <dirent.h>

#include "commonutils.h"
#include "camera3common.h"
#include "nativetestlog.h"

namespace haltests
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File System
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::Stat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CDKResult OsUtils::Stat(
//    const CHAR* pPath,
//    StatType*   pBuffer)
//{
//    CDKResult result = CDKResultSuccess;
//    // Problem with stat64 on NDK 15: no constuctor for stat64
//    if (0 != StatType(pPath, pBuffer))
//    {
//        result = CDKResultEFailed;
//    }
//    return result;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FOpen
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FILE* OsUtils::FOpen(
    const CHAR* pFilename,
    const CHAR* pMode)
{
    return fopen(pFilename, pMode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FClose
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult OsUtils::FClose(
    FILE* pFile)
{
    CDKResult result = CDKResultSuccess;
    if (0 != fclose(pFile))
    {
        result = CDKResultEFailed;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FSeek
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult OsUtils::FSeek(
    FILE*   pFile,
    INT64   offset,
    INT     origin)
{
    CDKResult result = CDKResultSuccess;
    if (0 != fseeko(pFile, static_cast<off_t>(offset), origin))
    {
        result = CDKResultEFailed;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FTell
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT64 OsUtils::FTell(
    FILE* pFile)
{
    return static_cast<INT64>(ftello(pFile));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FRead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIZE_T OsUtils::FRead(
    VOID*   pDst,
    SIZE_T  dstSize,
    SIZE_T  size,
    SIZE_T  count,
    FILE*   pFile)
{
    NATIVETEST_UNUSED_PARAM(dstSize);

    return fread(pDst, size, count, pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FWrite
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIZE_T OsUtils::FWrite(
    const VOID* pSrc,
    SIZE_T      size,
    SIZE_T      count,
    FILE*       pFile)
{
    return fwrite(pSrc, size, count, pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::FFlush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult OsUtils::FFlush(
    FILE* pFile)
{
    CDKResult result = CDKResultSuccess;
    if (0 != fflush(pFile))
    {
        result = CDKResultEFailed;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logging functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::GetFileName
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const CHAR* OsUtils::GetFileName(
    const CHAR* pFilePath)
{
    const CHAR* pFileName = StrRChr(pFilePath, '/');
    if (NULL != pFileName)
    {
        // StrRChr will return a pointer to the /, advance one to the filename
        pFileName += 1;
    }
    else
    {
        pFileName = pFilePath;
    }
    return pFileName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::GetFileNameToken
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL OsUtils::GetFileNameToken(
    CHAR*  pFilePath,
    UINT32 tokenInputNum,
    CHAR*  pOutToken,
    SIZE_T maxTokenLen)
{
    UINT  tokenCount = 0;
    CHAR* pToken = NULL;
    INT   result = FALSE;
    CHAR* pContext = NULL;
    CHAR  outTokenString[FILENAME_MAX];

    OsUtils::StrLCpy(outTokenString, pFilePath, strlen(pFilePath));
    pToken = OsUtils::StrTokReentrant(outTokenString, ".", &pContext);

    // The binary name is of format com.<vendor>.<category>.<algorithm>.<extension>
    while (NULL != pToken)
    {
        tokenCount++;
        if (tokenInputNum == tokenCount)
        {
            OsUtils::StrLCpy(pOutToken, pToken, maxTokenLen);
            result = TRUE;
            break;
        }
        pToken = OsUtils::StrTokReentrant(NULL, ".", &pContext);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::GetFilesFromPath
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 OsUtils::GetFilesFromPath(
    const CHAR* pFileSearchPath,
    SIZE_T      maxFileNameLength,
    CHAR*       pFileNames,
    const CHAR* pVendorName,
    const CHAR* pCategoryName,
    const CHAR* pModuleName,
    const CHAR* pExtension)
{
    UINT16      fileCount = 0;
    DIR*        pDirectory = NULL;
    dirent*     pDirent = NULL;
    const CHAR* pToken = NULL;
    CHAR*       pContext = NULL;
    UINT        tokenCount = 0;
    BOOL        isValid = TRUE;
    const CHAR* pCom = static_cast<const CHAR*>("com");
    UINT8       maxFileNameToken = static_cast<UINT8>(FileNameToken::Max);
    const CHAR* pTokens[maxFileNameToken];
    CHAR        fileName[maxFileNameLength];

    pDirectory = opendir(pFileSearchPath);
    if (NULL != pDirectory)
    {
        while (NULL != (pDirent = readdir(pDirectory)))
        {
            OsUtils::StrLCpy(fileName, pDirent->d_name, maxFileNameLength);
            tokenCount = 0;
            pToken = OsUtils::StrTokReentrant(fileName, ".", &pContext);
            isValid = TRUE;
            for (UINT8 i = 0; i < maxFileNameToken; i++)
            {
                pTokens[i] = NULL;
            }

            // The binary name is of format com.<vendor>.<category>.<module>.<extension>
            // Read all the tokens
            while ((NULL != pToken) && (tokenCount < maxFileNameToken))
            {
                pTokens[tokenCount++] = pToken;
                pToken = OsUtils::StrTokReentrant(NULL, ".", &pContext);
            }
            // token validation
            if ((NULL == pToken) && (maxFileNameToken == tokenCount))
            {
                for (UINT8 i = 0; (i < maxFileNameToken) && (isValid == TRUE); i++)
                {
                    switch (static_cast<FileNameToken>(i))
                    {
                    case FileNameToken::Com:
                        pToken = pCom;
                        break;
                    case FileNameToken::Vendor:
                        pToken = pVendorName;
                        break;
                    case FileNameToken::Category:
                        pToken = pCategoryName;
                        break;
                    case FileNameToken::Module:
                        pToken = pModuleName;
                        break;
                    case FileNameToken::Extension:
                        pToken = pExtension;
                        break;
                    default:
                        break;
                    }

                    if (0 != OsUtils::StrCmp(pToken, "*"))
                    {
                        if (0 == OsUtils::StrCmp(pToken, pTokens[i]))
                        {
                            isValid = TRUE;
                        }
                        else
                        {
                            isValid = FALSE;
                        }
                    }
                }
                if (TRUE == isValid)
                {
                    OsUtils::SNPrintF(pFileNames + (fileCount * maxFileNameLength),
                        maxFileNameLength,
                        "%s%s%s",
                        pFileSearchPath,
                        PathSeparator,
                        pDirent->d_name);
                    fileCount++;

                }
            }
        }
        closedir(pDirectory);
    }

    return fileCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::GetFileSize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT64 OsUtils::GetFileSize(
    const CHAR* pFilename)
{
    UINT64      size = 0;
    struct stat statBuf;

    if (0 == stat(pFilename, &statBuf))
    {
        size = statBuf.st_size;
    }

    return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::MallocAligned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* OsUtils::MallocAligned(
    SIZE_T size,
    SIZE_T alignment)
{
    VOID* pMem = NULL;

    bool isPowerOfTwo = ((alignment == 0) ? FALSE : (((alignment & (alignment - 1)) == 0) ? TRUE : FALSE));
    if ((0 == alignment) || (TRUE == isPowerOfTwo))
    {
        if (alignment < sizeof(VOID*))
        {
            alignment = sizeof(VOID*);
        }

        if (posix_memalign(&pMem, alignment, size) != 0)
        {
            pMem = NULL;
        }
    }

    return pMem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::FreeAligned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OsUtils::FreeAligned(
    VOID* pMem)
{
    free(pMem);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::PageSize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::PageSize()
{
    return sysconf(_SC_PAGESIZE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::MemMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* OsUtils::MemMap(
    INT     bufferFD,
    SIZE_T  bufferLength,
    SIZE_T  offset)
{
    VOID* pMem = NULL;

    if (bufferFD <= 0)
    {
        NT_LOG_ERROR( "bufferFD <= 0!");
        return NULL;
    }
    if ((bufferLength > 0) && (bufferFD > 0))
    {
        pMem = mmap(NULL, bufferLength, (PROT_READ | PROT_WRITE), MAP_SHARED, bufferFD, offset);
        if (MAP_FAILED == pMem)
        {
            NT_LOG_ERROR( "mmap() failed! errno %d", errno);
            pMem = NULL;
        }
    }

    return pMem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::MemUnmap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::MemUnmap(
    VOID*   pAddr,
    SIZE_T  size)
{
    INT result = -1;

    if (NULL == pAddr)
    {
        NT_LOG_ERROR( "pAddr is NULL!");
        return -1;
    }
    else
    {
        result = munmap(pAddr, size);
        if (0 != result)
        {
            NT_LOG_ERROR( "munmap() failed! errno %d", errno);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::DPrintF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::DPrintF(
    INT         fd,
    const CHAR* pFormat,
    ...)
{
    INT     numCharWritten;
    va_list args;

    va_start(args, pFormat);
    numCharWritten = vdprintf(fd, pFormat, args);
    va_end(args);

    return numCharWritten;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Thread functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::ThreadCreate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult OsUtils::ThreadCreate(
    OSThreadFunc    threadEntryFunction,
    VOID*           pThreadData,
    OSThreadHandle* phThread)
{
    CDKResult result = CDKResultSuccess;

    if (pthread_create(phThread, 0, threadEntryFunction, pThreadData) != 0)
    {
        // Failed to create thread
        NT_LOG_ERROR( "Failed to create thread!");
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::ThreadSetName
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult OsUtils::ThreadSetName(
    OSThreadHandle  hThread,
    const CHAR*     pName)
{
    CDKResult result = CDKResultSuccess;

    if (pthread_setname_np(hThread, pName) != 0)
    {
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::ThreadWait
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OsUtils::ThreadWait(
    OSThreadHandle hThread)
{
    pthread_join(hThread, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::SleepMicroseconds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OsUtils::SleepMicroseconds(
    UINT microseconds)
{
    // usleep takes microsecond param
    usleep(microseconds);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::LibGetAddr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* OsUtils::LibGetAddr(
    OSLIBRARYHANDLE hLibrary,
    const CHAR*     pProcName)
{
    VOID* pProcAddr = NULL;

    if (hLibrary != NULL)
    {
        pProcAddr = dlsym(hLibrary, pProcName);
    }
    return pProcAddr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::GetThreadID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT OsUtils::GetThreadID()
{
    return gettid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::GetProcessID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT OsUtils::GetProcessID()
{
    return getpid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::VSNPrintF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::VSNPrintF(
    CHAR*       pDst,
    SIZE_T      sizeDst,
    const CHAR* pFormat,
    va_list     argptr)
{
    INT numCharWritten = 0;
    numCharWritten = vsnprintf(pDst, sizeDst, pFormat, argptr);
    if (numCharWritten < 0)
    {
        NT_LOG_ERROR( "numCharWritter < 0!");
        return -1;
    }
    if ((numCharWritten >= static_cast<INT>(sizeDst)) && (sizeDst > 0))
    {
        // Message length exceeds the buffer limit size
        NT_LOG_ERROR( "Message length exceeds the buffer limit size!");
        numCharWritten = -1;
    }
    return numCharWritten;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::StrNCmp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::StrNCmp(
    const CHAR* pString1,
    const CHAR* pString2,
    SIZE_T      maxCount)
{
    return strncmp(pString1, pString2, maxCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::StrNICmp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::StrNICmp(
    const CHAR* pString1,
    const CHAR* pString2,
    SIZE_T      maxCount)
{
    return strncasecmp(pString1, pString2, maxCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::StrLCat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIZE_T OsUtils::StrLCat(
    CHAR* pDst,
    const CHAR* pSrc,
    SIZE_T sizeDst)
{
#ifdef ANDROID
    return strlcat(pDst, pSrc, sizeDst);
#else // ANDROID
    return g_strlcat(pDst, pSrc, sizeDst);
#endif // ANDROID
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::StrLCpy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIZE_T OsUtils::StrLCpy(
    CHAR*       pDst,
    const CHAR* pSrc,
    SIZE_T      sizeDst)
{
#ifdef ANDROID
    return strlcpy(pDst, pSrc, sizeDst);
#else // ANDROID
    return g_strlcpy(pDst, pSrc, sizeDst);
#endif // ANDROID
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::StrTokReentrant
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHAR* OsUtils::StrTokReentrant(
    CHAR*       pSrc,
    const CHAR* pDelimiter,
    CHAR**      ppContext)
{
    return strtok_r(pSrc, pDelimiter, ppContext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::GetTime
///
/// @brief  Gets the current time
///
/// @param  pTime Output time structure
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OsUtils::GetTime(
    CDKTime* pTime)
{
    if (NULL == pTime)
    {
        NT_LOG_ERROR( "pTime is NULL!");
        return;
    }

    timespec time;
    clock_gettime(CLOCK_BOOTTIME, &time);

    pTime->seconds = static_cast<UINT32>(time.tv_sec);
    pTime->nanoSeconds = static_cast<UINT32>(time.tv_nsec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::GetDateTime
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OsUtils::GetDateTime(
    CDKDateTime* pDateTime)
{
    if (NULL == pDateTime)
    {
        NT_LOG_ERROR( "pDateTime is NULL!");
        return;
    }
    struct timeval  timeValue;
    struct tm*      pTimeInfo = NULL;
    INT             result = 0;

    result = gettimeofday(&timeValue, NULL);
    if (0 == result)
    {
        pTimeInfo = localtime(&timeValue.tv_sec);
        if (NULL != pTimeInfo)
        {
            pDateTime->seconds = static_cast<UINT32>(pTimeInfo->tm_sec);
            pDateTime->milliseconds = static_cast<UINT32>(timeValue.tv_usec / 1000);
            pDateTime->minutes = static_cast<UINT32>(pTimeInfo->tm_min);
            pDateTime->hours = static_cast<UINT32>(pTimeInfo->tm_hour);
            pDateTime->dayOfMonth = static_cast<UINT32>(pTimeInfo->tm_mday);
            pDateTime->month = static_cast<UINT32>(pTimeInfo->tm_mon + 1);
            pDateTime->year = static_cast<UINT32>(pTimeInfo->tm_year + 1900);
            pDateTime->weekday = static_cast<UINT32>(pTimeInfo->tm_wday);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OsUtils::GetGMTime
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT OsUtils::GetGMTime(
    const time_t*   pTime,
    struct tm*      pResult)
{
    if (NULL == pTime)
    {
        NT_LOG_ERROR( "pTime is NULL!");
        return -1;
    }
    if (NULL == pResult)
    {
        NT_LOG_ERROR( "pResult is NULL!");
        return -1;
    }
    INT        result = 0;
    struct tm* pReturnValue = NULL;

    pReturnValue = gmtime_r(pTime, pResult);
    if (NULL == pReturnValue)
    {
        result = -1;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OsUtils::Close
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult OsUtils::Close(
    INT FD)
{
    CDKResult result = CDKResultSuccess;
    if (-1 != FD)
    {
        if (0 != close(FD))
        {
            result = CDKResultEFailed;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mutex* Mutex::Create(
    const CHAR* pResourceName)
{
    Mutex* pMutex = NULL;

    pMutex = SAFE_NEW() Mutex();
    if (NULL != pMutex)
    {
        if (CDKResultSuccess != pMutex->Initialize(pResourceName))
        {
            delete pMutex;
            pMutex = NULL;
        }
    }

    return pMutex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Mutex::Initialize(
    const CHAR* pResourceName)
{
    CDKResult           result = CDKResultSuccess;
    pthread_mutexattr_t attr;
    BOOL                bValidAttr = FALSE;      // TRUE once attr has been initialized

    if (pResourceName == NULL)
    {
        NT_LOG_ERROR( "pResourceName is NULL!");
        return CDKResultEInvalidPointer;
    }

    m_pResourceName = pResourceName;

    if (pthread_mutexattr_init(&attr) == 0)
    {
        bValidAttr = TRUE;
    }
    else
    {
        result = CDKResultEFailed;
    }

    // Using re-entrant mutexes
    if ((CDKResultSuccess == result) &&
        (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP) != 0))
    {
        result = CDKResultEFailed;
    }

    if ((CDKResultSuccess == result) &&
        (pthread_mutex_init(&m_mutex, &attr) == 0))
    {
        m_validMutex = TRUE;
    }
    else
    {
        result = CDKResultEFailed;
    }

    if (TRUE == bValidAttr)
    {
        pthread_mutexattr_destroy(&attr);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Mutex::Destroy()
{
    if (TRUE == m_validMutex)
    {
        pthread_mutex_destroy(&m_mutex);
    }

    delete this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::Lock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Mutex::Lock()
{
    pthread_mutex_lock(&m_mutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::TryLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Mutex::TryLock()
{
    CDKResult result = CDKResultSuccess;
    INT        returnCode = 0;

    returnCode = pthread_mutex_trylock(&m_mutex);
    if (0 != returnCode)
    {
        if (EBUSY == returnCode)
        {
            result = CDKResultEBusy;
        }
        else
        {
            result = CDKResultEFailed;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::Unlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Mutex::Unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex::GetNativeHandle
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OSMutexHandle* Mutex::GetNativeHandle()
{
    return ((TRUE == m_validMutex) ? &m_mutex : NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Semaphore* Semaphore::Create()
{
    Semaphore* pSemaphore = NULL;

    pSemaphore = SAFE_NEW() Semaphore();

    if (NULL != pSemaphore)
    {
        if (CDKResultSuccess != pSemaphore->Initialize())
        {
            delete pSemaphore;
            pSemaphore = NULL;
        }
    }

    return pSemaphore;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Semaphore::Initialize()
{
    CDKResult result = CDKResultSuccess;

    m_validSemaphore = FALSE;

    if (sem_init(&m_semaphore, 0, 0) == 0)
    {
        m_validSemaphore = TRUE;
    }
    else
    {
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Semaphore::Destroy()
{
    if (TRUE == m_validSemaphore)
    {
        sem_destroy(&m_semaphore);
    }

    delete this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore::Wait
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Semaphore::Wait()
{
    if (TRUE == m_validSemaphore)
    {
        sem_wait(&m_semaphore);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore::Signal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Semaphore::Signal()
{
    if (TRUE == m_validSemaphore)
    {
        sem_post(&m_semaphore);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semaphore::TimedWait
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Semaphore::TimedWait(
    UINT timeoutMilliseconds)
{
    CDKResult      result = CDKResultSuccess;
    INT             waitResult = 0;
    UINT            timeoutSeconds = (timeoutMilliseconds / 1000UL);
    UINT            timeoutNanoseconds = (timeoutMilliseconds % 1000UL) * 1000000UL;
    struct timespec timeout = {};

    // Calculate the timeout time
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeoutSeconds += (static_cast<UINT>(timeout.tv_nsec) + timeoutNanoseconds) / 1000000000UL;
    timeoutNanoseconds = (static_cast<UINT>(timeout.tv_nsec) + timeoutNanoseconds) % 1000000000UL;
    timeout.tv_sec += static_cast<INT>(timeoutSeconds);
    timeout.tv_nsec = static_cast<INT>(timeoutNanoseconds);

    waitResult = sem_timedwait(&m_semaphore, &timeout);
    if (waitResult != 0)
    {
        // Check errno for reason for failure
        if (ETIMEDOUT == waitResult)
        {
            result = CDKResultETimeout;
        }
        else
        {
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Condition* Condition::Create(
    const CHAR* pResource)
{
    Condition* pCondition = NULL;

    pCondition = SAFE_NEW() Condition();

    if (NULL != pCondition)
    {
        if (CDKResultSuccess != pCondition->Initialize(pResource))
        {
            delete pCondition;
            pCondition = NULL;
        }
    }

    return pCondition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Condition::Destroy()
{
    if (TRUE == m_validConditionVar)
    {
        pthread_cond_destroy(&m_conditionVar);
    }

    delete this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Condition::Initialize(
    const CHAR* pResource)
{
    CDKResult result = CDKResultSuccess;

    m_pResource = pResource;

    if (pthread_cond_init(&m_conditionVar, NULL) == 0)
    {
        m_validConditionVar = TRUE;
    }
    else
    {
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::Wait
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Condition::Wait(
    OSMutexHandle* phMutex)
{
    if (NULL == phMutex)
    {
        NT_LOG_ERROR( "phMutex is NULL!");
        return CDKResultEInvalidPointer;
    }

    INT         rc = 0;
    CDKResult  result = CDKResultEFailed;

    rc = pthread_cond_wait(&m_conditionVar, phMutex);

    if (0 == rc)
    {
        result = CDKResultSuccess;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::TimedWait
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Condition::TimedWait(
    OSMutexHandle*  phMutex,
    UINT            timeoutMilliseconds)
{
    if (NULL == phMutex)
    {
        NT_LOG_ERROR( "phMutex is NULL!");
        return CDKResultEInvalidPointer;
    }

    CDKResult      result = CDKResultSuccess;
    INT             waitResult = 0;
    UINT            timeoutSeconds = (timeoutMilliseconds / 1000UL);
    UINT            timeoutNanoseconds = (timeoutMilliseconds % 1000UL) * 1000000UL;
    struct timespec timeout = {};

    // Calculate the timeout time
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeoutSeconds += (static_cast<UINT>(timeout.tv_nsec) + timeoutNanoseconds) / 1000000000UL;
    timeoutNanoseconds = (static_cast<UINT>(timeout.tv_nsec) + timeoutNanoseconds) % 1000000000UL;
    timeout.tv_sec += static_cast<INT>(timeoutSeconds);
    timeout.tv_nsec = static_cast<INT>(timeoutNanoseconds);

    waitResult = pthread_cond_timedwait(&m_conditionVar, phMutex, &timeout);
    if (waitResult != 0)
    {
        // Check errno for reason for failure
        if (ETIMEDOUT == waitResult)
        {
            result = CDKResultETimeout;
        }
        else
        {
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::Signal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Condition::Signal()
{
    //NT_LOG_INFO( "Condition::Signal %s", m_pResource);
    pthread_cond_signal(&m_conditionVar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Condition::Broadcast
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Condition::Broadcast()
{
    //NT_LOG_INFO( "Condition::Broadcast %s", m_pResource);
    pthread_cond_broadcast(&m_conditionVar);
}

} // namespace tests

#endif // defined (_LINUX)
