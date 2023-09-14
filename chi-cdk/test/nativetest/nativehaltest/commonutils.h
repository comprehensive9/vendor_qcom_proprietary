////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  commonutils.h
/// @brief OS specific utility implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#if defined (_LINUX)
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <camxcdktypes.h>
#endif // _LINUX

#if defined (_WIN32)
#include <windows.h>
#include "camxcdktypes.h"
#endif // _WIN32

namespace haltests
{
/// Thread entry function type
typedef VOID* (*OSThreadFunc)(VOID* pArg);

/// Dynamic library handle
typedef VOID* OSLIBRARYHANDLE;

#if defined (_LINUX)
/// Thread handle type
typedef pthread_t OSThreadHandle;
/// Mutex handle type
typedef pthread_mutex_t OSMutexHandle;
/// Native fence type
typedef INT NativeFence;
/// File system path separator character
static const CHAR PathSeparator[] = "/";
static const CHAR SharedLibraryExtension[] = "so";

#if defined(_LP64)
static const CHAR VendorLibPath[] = "/vendor/lib64";
#else
static const CHAR VendorLibPath[] = "/vendor/lib";
#endif // defined(_LP64)

#elif defined (_WIN32)
/// Thread handle type
typedef HANDLE OSThreadHandle;
/// Mutex handle type
typedef CRITICAL_SECTION OSMutexHandle;
/// Native fence type
typedef VOID* NativeFence;
/// File system path separator character
static const CHAR PathSeparator[] = "\\";
static const CHAR SharedLibraryExtension[] = "dll";
static const CHAR VendorLibPath[] = ".";

#else
#error Unsupported target defined
#endif // defined(_LINUX) || defined(_WIN32)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Time support structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Time structure
struct CDKTime
{
    UINT32  seconds;                    ///< Time in whole seconds
    UINT32  nanoSeconds;                ///< Fractional nanoseconds
};

/// @brief Date time structure
struct CDKDateTime
{
    UINT32  seconds;                    ///< Seconds after the minute 0-61*
    UINT32  milliseconds;               ///< Milliseconds 0-999
    UINT32  minutes;                    ///< Time in minutes 0-59
    UINT32  hours;                      ///< Time in hours 0-23
    UINT32  dayOfMonth;                 ///< Day of month 1-31
    UINT32  month;                      ///< Months since January 1-12, where 1 is January
    UINT32  year;                       ///< Year 1900-30827
    UINT32  weekday;                    ///< Day of week 0-6
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// fileNameToken
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class FileNameToken
{
    Com,       ///< com
    Vendor,    ///< vendor name
    Category,  ///< category name
    Module,    ///< module name
    Extension, ///< File Extension
    Max        ///< Max tokens for file name
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General OS specific utility class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OsUtils
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // File System Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_LINUX)
    typedef struct stat64 StatType;
#elif defined(_WIN32)
    typedef struct __stat64 StatType;
#else
    CAMX_STATIC_ASSERT_MESSAGE(FALSE, "Unsupported Target");
#endif // defined(_LINUX)

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Stat
    ///
    /// @brief  Gets status information on a file
    ///
    /// @param  pPath    File to obtain status information from
    /// @param  pBuffer  Pointer to results status structure
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult Stat(
        const CHAR* pPath,
        StatType*   pBuffer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FOpen
    ///
    /// @brief  Opens a file on the filesystem
    ///
    /// @param  pFilename File to open
    /// @param  pMode     File access mode string
    ///
    /// @return File handle or NULL on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static FILE* FOpen(
        const CHAR* pFilename,
        const CHAR* pMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FClose
    ///
    /// @brief  Closes the given file handle
    ///
    /// @param  pFile File handle to close
    ///
    /// @return CDKResultSuccess on success, CDKResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult FClose(
        FILE* pFile);

    static const INT SeekSet = SEEK_SET;    ///< Beginning of file origin for Fseek
    static const INT SeekEnd = SEEK_END;    ///< End of file origin for Fseek
    static const INT SeekCur = SEEK_CUR;    ///< Current position of the file pointer origin for Fseek

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FSeek
    ///
    /// @brief  Moves the file pointer to a specified location
    ///
    /// @param  pFile    File handle to seek
    /// @param  offset   Number of bytes from origin
    /// @param  origin   Initial position
    ///
    /// @return CDKResultSuccess on success, CDKResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult FSeek(
        FILE*   pFile,
        INT64   offset,
        INT     origin);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FTell
    ///
    /// @brief  Gets the current position of a file pointer
    ///
    /// @param  pFile File handle to tell
    ///
    /// @return Current file position or -1 on error
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT64 FTell(
        FILE* pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FGetS
    ///
    /// @brief  Reads characters from the specified stream and stores them as a C string into pDst until (numCharsToCopy-1)
    ///         characters have been read or either a newline or the end-of-file is reached, whichever happens first. A newline
    ///         character will cause the function to stop reading from the stream, but it is considered a valid character and
    ///         included in the string copied to pDst. A terminating null character is automatically appended after the
    ///         characters copied to pDst.
    ///
    /// @param  pDst            Destination string
    /// @param  numCharsToCopy  Number of characters to copy to pDst
    /// @param  pFile           File handle to read from
    ///
    /// @return If successful, pDst is returned. If 0 characters are read, NULL is returned.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline CHAR* FGetS(
        CHAR*   pDst,
        INT     numCharsToCopy,
        FILE*   pFile)
    {
        return fgets(pDst, numCharsToCopy, pFile);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FRead
    ///
    /// @brief  Reads data from a file handle
    ///
    /// @param  pDst     Destination memory
    /// @param  dstSize  Destination memory size
    /// @param  size     Size of objects to read
    /// @param  count    Number of objects to read
    /// @param  pFile    File handle to read from
    ///
    /// @return Number of objects read successfully
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T FRead(
        VOID*   pDst,
        SIZE_T  dstSize,
        SIZE_T  size,
        SIZE_T  count,
        FILE*   pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FWrite
    ///
    /// @brief  Writes data to a file handle
    ///
    /// @param  pSrc     Source memory
    /// @param  size     Size of objects to write
    /// @param  count    Number of objects to write
    /// @param  pFile    File handle to write to
    ///
    /// @return Number of objects written successfully
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T FWrite(
        const VOID* pSrc,
        SIZE_T      size,
        SIZE_T      count,
        FILE*       pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FFlush
    ///
    /// @brief  Flushes a file handle
    ///
    /// @param  pFile File handle to flush
    ///
    /// @return CDKResultSuccess on success, CDKResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult FFlush(
        FILE* pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFileName
    ///
    /// @brief  Extracts just the file name from a file's path plus name
    ///
    /// @param  pFilePath String to parse for the filename
    ///
    /// @return File name
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static const CHAR* GetFileName(
        const CHAR* pFilePath);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFileNameToken
    ///
    /// @brief  Extract the token based on token count from Filename - com.<vendor>.<category>.<algorithm>.so
    ///
    /// @param  pFileName         Name of the file
    /// @param  tokenInputNum     get token based on tokenInputNum
    /// @param  pOutToken         Pointer to store the output token
    /// @param  maxTokenLen       Max token len.
    ///
    /// @return BOOL              TRUE or FALSE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL GetFileNameToken(
        CHAR*  pFileName,
        UINT32 tokenInputNum,
        CHAR*  pOutToken,
        SIZE_T maxTokenLen);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFilesFromPath
    ///
    /// @brief  Reads the directory and returns the binary file names
    ///
    /// @param  pFileSearchPath   Path to the directory
    /// @param  maxFileNameLength Maximum length of file name
    /// @param  pFileNames        Pointer to the beginning of memory to store an array of file names at the return of function
    /// @param  pVendorName       Vendor name
    /// @param  pCategoryName     Category of binary file
    /// @param  pModuleName       Name of the module
    /// @param  pExtension        Pointer to file extension
    ///
    /// @return Number of binary files in the directory
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT16 GetFilesFromPath(
        const CHAR* pFileSearchPath,
        SIZE_T      maxFileNameLength,
        CHAR*       pFileNames,
        const CHAR* pVendorName,
        const CHAR* pCategoryName,
        const CHAR* pModuleName,
        const CHAR* pExtension);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFileSize
    ///
    /// @brief  Gets the size, in bytes, for a given filename. Temporarily opens the file in the process
    ///
    /// @param  pFilename File to open
    ///
    /// @return File size, if file exists
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT64 GetFileSize(
        const CHAR* pFilename);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Memory functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MallocAligned
    ///
    /// @brief  Allocates memory aligned to specified alignment. Alignment must be power of 2. Do not use this directly,
    ///         use CAMX_CALLOC_ALIGNED* instead.
    ///
    /// @param  size        size for allocation
    /// @param  alignment   Alignment of memory block
    ///
    /// @return Pointer to malloc'd memory with start address a multiple of alignment and as big as size.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* MallocAligned(
        SIZE_T size,
        SIZE_T alignment);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FreeAligned
    ///
    /// @brief  Frees memory that was allocated using MallocAligned. Do not use this directly, use CAMX_FREE_ALIGNED* instead.
    ///
    /// @param  pMem    A pointer to the memory to free
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID FreeAligned(
        VOID* pMem);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PageSize
    ///
    /// @brief  Gets the default allocation page size in bytes.
    ///
    /// @return The number of bytes in a memory page; -1 if page size could not be found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT PageSize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MemMap
    ///
    /// @brief  Maps file or device into memory
    ///
    /// @param  bufferFD        file or device to be mapped
    /// @param  bufferLength    length of the mapping
    /// @param  offset          offset to be mapped in memory of the fd
    ///
    /// @return non-NULL pointer if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* MemMap(
        INT      bufferFD,
        SIZE_T   bufferLength,
        SIZE_T   offset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MemUnmap
    ///
    /// @brief  Unmaps the memory which was previous mapped
    ///
    /// @param  pAddr   Address or memory to be unmapped
    /// @param  size    length of the memory to be unmapped
    ///
    /// @return 0 if successful and -1 for failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT MemUnmap(
        VOID*   pAddr,
        SIZE_T  size);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // String Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DPrintF
    ///
    /// @brief  Write formatted data from variable argument list to sized buffer
    ///
    /// @param  fd       File descriptor to print to
    /// @param  pFormat  Format string, printf style
    /// @param  ...      Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT DPrintF(
        INT         fd,
        const CHAR* pFormat,
        ...);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SNPrintF
    ///
    /// @brief Write formatted data from variable argument list to sized buffer
    ///
    /// @param pDst     Destination buffer
    /// @param sizeDst  Size of the destination buffer
    /// @param pFormat  Format string, printf style
    /// @param ...      Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline INT SNPrintF(
        CHAR*       pDst,
        SIZE_T      sizeDst,
        const CHAR* pFormat,
        ...)
    {
        INT     numCharWritten;
        va_list args;

        va_start(args, pFormat);
        numCharWritten = VSNPrintF(pDst, sizeDst, pFormat, args);
        va_end(args);

        return numCharWritten;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// VSNPrintF
    ///
    /// @brief  Write formatted data from variable argument list to sized buffer
    ///
    /// @param  pDst     Destination buffer
    /// @param  sizeDst  Size of the destination buffer
    /// @param  pFormat  Format string, printf style
    /// @param  argptr   Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT VSNPrintF(
        CHAR*       pDst,
        SIZE_T      sizeDst,
        const CHAR* pFormat,
        va_list     argptr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrStrip
    ///
    /// @brief  Strip all whitespace characters (space, tab, newline, carriage return, form feed, vertical tab) from a string.
    ///
    /// @param  pDst     The string to write to
    /// @param  pSrc     The string to read from
    /// @param  sizeDst  Size of the destination string
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline VOID StrStrip(
        CHAR* pDst,
        CHAR* pSrc,
        SIZE_T sizeDst)
    {
        if ((NULL != pSrc) && (NULL != pDst) && (sizeDst > 0))
        {
            CHAR* pTokenString;
            CHAR* pContext;
            pDst[0] = '\0';
            pTokenString = StrTokReentrant(pSrc, " \t\n\r\f\v", &pContext);
            while ((pTokenString != NULL) && (pTokenString[0] != '\0'))
            {
                StrLCat(pDst, pTokenString, sizeDst);
                pTokenString = StrTokReentrant(NULL, " \t\n\r\f\v", &pContext);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrCmp
    ///
    /// @brief  Compares two strings.
    ///
    /// @param  pString1 First string to compare
    /// @param  pString2 Second string to compare
    ///
    /// @return Returns a signed integer indicating the relationship between the input strings:
    ///         <0  the first character that does not match has a lower value in string1 than in string
    ///         0   the contents of both strings are equal
    ///         >0  the first character that does not match has a greater value in string1 than in string2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline INT StrCmp(
        const CHAR* pString1,
        const CHAR* pString2)
    {
        return strcmp(pString1, pString2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrNCmp
    ///
    /// @brief  Compares two strings looking at a maximum of N characters.
    ///
    /// @param  pString1 First string to compare
    /// @param  pString2 Second string to compare
    /// @param  maxCount Maximum number of characters to compare
    ///
    /// @return Returns a signed integer indicating the relationship between the input strings:
    ///         <0  the first character that does not match has a lower value in string1 than in string
    ///         0   the contents of both strings are equal
    ///         >0  the first character that does not match has a greater value in string1 than in string2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT StrNCmp(
        const CHAR* pString1,
        const CHAR* pString2,
        SIZE_T      maxCount);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrNICmp
    ///
    /// @brief  Compares two strings ignoring the case of the characters.
    ///
    /// @param  pString1 First string to compare
    /// @param  pString2 Second string to compare
    /// @param  maxCount Maximum number of characters to compare
    ///
    /// @return Returns a signed integer indicating the relationship between the input strings:
    ///         <0  the first character that does not match has a lower value in string1 than in string
    ///         0   the contents of both strings are equal
    ///         >0  the first character that does not match has a greater value in string1 than in string2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT StrNICmp(
        const CHAR* pString1,
        const CHAR* pString2,
        SIZE_T      maxCount);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrLCat
    ///
    /// @brief  Appends (sizeDst - StrLen(pDst) - 1) characters of pSrc to the end of pDst.
    ///
    /// @param  pDst     The string to write to
    /// @param  pSrc     The string to read from
    /// @param  sizeDst  Size of the destination string in bytes
    ///
    /// @return The total length of the new string.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T StrLCat(
        CHAR* pDst,
        const CHAR* pSrc,
        SIZE_T sizeDst);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrLCpy
    ///
    /// @brief  Static function to copy a string with a maximum destination string length. Will always terminate. Source and
    ///         destination must not overlap
    ///
    /// @param  pDst     Destination string
    /// @param  pSrc     Source string
    /// @param  sizeDst  Size of destination buffer, in bytes
    ///
    /// @return Length of the source string
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T StrLCpy(
        CHAR*       pDst,
        const CHAR* pSrc,
        SIZE_T      sizeDst);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrLen
    ///
    /// @brief  Static function to return the length of the null-terminated string.
    ///
    /// @param  pSrc The null-terminated string to determine the length of.
    ///
    /// @return Length of the null-terminated string
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline SIZE_T StrLen(
        const CHAR* pSrc)
    {
        return strlen(pSrc);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrRChr
    ///
    /// @brief  Static function to find the last occurrence of a character within a string
    ///
    /// @param  pSrc        String to search
    /// @param  character   Character to search for
    ///
    /// @return Pointer to the character or NULL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline const CHAR* StrRChr(
        const CHAR* pSrc,
        CHAR        character)
    {
        return strrchr(pSrc, static_cast<INT>(character));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrStr
    ///
    /// @brief  Returns a pointer to the first occurrence of pString2 in pString1, or a NULL pointer if pString2 is not part of
    ///         pString1. The matching process stops at the first terminating null-characters and it is not considered in the
    ///         comparison.
    ///
    /// @param  pString1    The string to search
    /// @param  pString2    The string containing the sequence of characters to match
    ///
    /// @return A pointer to the first occurrence in pString1 of the entire sequence of characters specified in pString2, or a
    ///         NULL pointer if the sequence is not present in pString1.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline const CHAR* StrStr(
        const CHAR* pString1,
        const CHAR* pString2)
    {
        return strstr(pString1, pString2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrCSpn
    ///
    /// @brief  Scans pString1 for the first occurrence of any of the characters that are part of pString2, returning the number
    ///         of characters of pString1 read before this first occurrence. The search includes the terminating
    ///         null-characters. Therefore, the function will return the length of pString1 if none of the characters of
    ///         pString2 are found in pString1.
    ///
    /// @param  pString1    The string to be scanned
    /// @param  pString2    The string containing the characters to match
    ///
    /// @return The length of the inital part of pString1 not containing any of the characters that are part of pString2. This
    ///         is the length of pString1 if none of the characters in pString2 are found in pString1.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline SIZE_T StrCSpn(
        const CHAR* pString1,
        const CHAR* pString2)
    {
        return strcspn(pString1, pString2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrTokReentrant
    ///
    /// @brief  Parses a string into a sequence of tokens. On the first call, the string to be parsed is specified in pSrc and
    ///         the value of pContext should be valid, but is ignored. In each subsequent call that should parse the same
    ///         string, pSrc should be NULL and pContext should be unchanged since the previous call. This function is
    ///         reentrant and different strings may be parsed concurrently using sequences of calls to strtok_r() that specify
    ///         different saveptr arguments.
    ///
    /// @param  pSrc        The string to search
    /// @param  pDelimiter  Specifies a set of bytes that delimit the tokens in the parsed string. The caller may specify
    ///                     different delimiter strings in successive calls that parse the same string.
    /// @param  ppContext   A pointer to a char* variable that is used internally in order to maintain context between
    ///                     successive calls that parse the same string.
    ///
    /// @return A pointer to a null-terminated string containing the next token, or NULL if there are no more tokens.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CHAR* StrTokReentrant(
        CHAR*       pSrc,
        const CHAR* pDelimiter,
        CHAR**      ppContext);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrToL
    ///
    /// @brief  Parses the string pSrc, interpreting its content as a signed integral number of the specified base, which is
    ///         returned as an value of type signed long int.
    ///
    /// @param  pSrc  String containing the representation of a signed integral number.
    /// @param  ppEnd Reference to an object of type char*, whose value is set by the function to the next character in pSrc
    ///               after the numerical value. This parameter can also be NULL, in which case it is not used.
    /// @param  base  Numerical base (radix) that determines the valid characters and their interpretation.
    ///
    /// @return The converted signed integral number as an INT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline INT StrToL(
        const CHAR* pSrc,
        CHAR**      ppEnd,
        INT         base)
    {
        return static_cast<INT>(strtol(pSrc, ppEnd, base));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrToUL
    ///
    /// @brief  Parses the string pSrc, interpreting its content as an unsigned integral number of the specified base, which is
    ///         returned as a value of type unsigned long int.
    ///
    /// @param  pSrc  String containing the representation of an unsigned integral number.
    /// @param  ppEnd Reference to an object of type char*, whose value is set by the function to the next character in pSrc
    ///               after the numerical value. This parameter can also be NULL, in which case it is not used.
    /// @param  base  Numerical base (radix) that determines the valid characters and their interpretation.
    ///
    /// @return The converted unsigned integral number as a UINT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline UINT StrToUL(
        const CHAR* pSrc,
        CHAR**      ppEnd,
        INT         base)
    {
        return static_cast<UINT>(strtoul(pSrc, ppEnd, base));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrToD
    ///
    /// @brief  Parses the string pSrc, interpreting its content as a double precision floating point number of the specified
    ///         base, which is returned as a value of type unsigned long int.
    ///
    /// @param  pSrc  String containing the representation of a double precision floating point number.
    /// @param  ppEnd Reference to an object of type char*, whose value is set by the function to the next character in pSrc
    ///               after the numerical value. This parameter can also be NULL, in which case it is not used.
    ///
    /// @return The converted double precision floating point number as a DOUBLE value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline DOUBLE StrToD(
        const CHAR* pSrc,
        CHAR**      ppEnd)
    {
        return strtod(pSrc, ppEnd);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ToLower
    ///
    /// @brief  Converts the input character to its lowercase equivalent.
    ///
    /// @param  character The character to make lowercase.
    ///
    /// @return The converted character value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline CHAR ToLower(
        const CHAR character)
    {
        return static_cast<CHAR>(tolower(static_cast<INT>(character)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Thread Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetThreadID
    ///
    /// @brief  return the thread ID
    ///
    /// @return Numerical ID of the current thread
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT GetThreadID();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetProcessID
    ///
    /// @brief  return the process ID
    ///
    /// @return Numerical ID of the current process
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT GetProcessID();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadCreate
    ///
    /// @brief  Create a OS thread
    ///
    /// @param  threadEntryFunction  Function to execute in new thread
    /// @param  pThreadData          Data passed to threadEntryFunction()
    /// @param  phThread             New thread handle or NULL on failure
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult ThreadCreate(
        OSThreadFunc    threadEntryFunction,
        VOID*           pThreadData,
        OSThreadHandle* phThread);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadSetName
    ///
    /// @brief  Sets the name of an OS thread
    ///
    /// @param  hThread             New thread handle or NULL on failure
    /// @param  pName               New name of the thread
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult ThreadSetName(
        OSThreadHandle  hThread,
        const CHAR*     pName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadWait
    ///
    /// @brief  Wait for the given thread to terminate
    ///
    /// @param  hThread Thread to wait on
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID ThreadWait(
        OSThreadHandle hThread);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SleepMilliseconds
    ///
    /// @brief  Put the current thread to sleep
    ///
    /// @param  milliseconds  Sleep duration in milliseconds
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline VOID SleepMilliseconds(
        UINT milliseconds)
    {
        SleepMicroseconds(/*convert to microseconds*/(milliseconds * 1000));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SleepMicroseconds
    ///
    /// @brief  Put the current thread to sleep
    ///
    /// @param  microseconds  Sleep duration in microseconds
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID SleepMicroseconds(
        UINT microseconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// System
    ///
    /// @brief  Invokes the command processor to execute a command
    ///
    /// @param  pCommand    Command string to execute
    ///
    /// @return status of execution or existence of command processor
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline INT System(
        const CHAR* pCommand)
    {
        return system(pCommand);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LibGetAddr
    ///
    /// @brief  Static function find an entry point in a library previously loaded by LibMap()
    ///
    /// @param  hLibrary  Handle to library from LibMap()
    /// @param  pProcName Name of entry point to find
    ///
    /// @return Pointer to named entry point in library, or NULL on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* LibGetAddr(
        OSLIBRARYHANDLE hLibrary,
        const CHAR*     pProcName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetTime
    ///
    /// @brief  Gets the current time
    ///
    /// @param  pTime Output time structure
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID GetTime(
        CDKTime* pTime);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetNanoSeconds
    ///
    /// @brief  Get the nanoseconds
    ///
    /// @return Nano seconds
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static inline UINT64 GetNanoSeconds()
    {
        const UINT64 NanoSecondsPerSecond = 1000000000ULL;
        CDKTime systemTime;
        GetTime(&systemTime);
        UINT64 nanoSeconds = (systemTime.seconds * NanoSecondsPerSecond) + systemTime.nanoSeconds;

        return nanoSeconds;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetDateTime
    ///
    /// @brief  Gets the date and time
    ///
    /// @param  pDateTime Output date and time structure
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID GetDateTime(
        CDKDateTime* pDateTime);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetGMTime
    ///
    /// @brief  Gets UTC/GMT time from local
    ///
    /// @param  pTime     Pointer time structure
    /// @param  pResult   Pointer to result
    ///
    /// @return Return 0 on success or errno on error
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT GetGMTime(
        const time_t*   pTime,
        struct tm*      pResult);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CamxTimeToMillis
    ///
    /// @brief  Translates CDKTime to time in milliseconds
    ///
    /// @param  pTime   Pointer to CDKTime structure
    ///
    /// @return Return milliseconds on success or 0 on error
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT32 CamxTimeToMillis(
        CDKTime*   pTime)
    {
        return (pTime == NULL) ? 0 : ((1000 * pTime->seconds) + ((pTime->nanoSeconds) / 1000000L));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Close
    ///
    /// @brief  Closes the given file descriptor if it is valid one
    ///
    /// @param  FD Filedescriptor to close
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CDKResult Close(
        INT FD);

private:
    OsUtils() = default;
};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief General portable mutex class implementation
    ///
    /// Basic wrapping of OS mutex to provide abstraction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class Mutex final
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Create
        ///
        /// @brief  Static method to create an instance of Mutex
        ///
        /// @param  pResourceName Human readable name of the resource being protected
        ///
        /// @return Pointer to newly created mutex object on success, NULL on failure.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Mutex* Create(
            const CHAR* pResourceName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Destroy
        ///
        /// @brief  Method to delete an instance of Mutex
        ///
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        VOID Destroy();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Lock
        ///
        /// @brief  Lock/acquire an Mutex
        ///
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        VOID Lock();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// TryLock
        ///
        /// @brief  Try to acquire a Mutex lock
        ///
        /// @return CDKResultSuccess on success, CDKResultEBusy if lock already taken and CDKResultEFailed on all other errors.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CDKResult TryLock();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Unlock
        ///
        /// @brief  Unlock/release an Mutex
        ///
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        VOID Unlock();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetNativeHandle
        ///
        /// @brief  Get the native mutex handle, needed for other system calls which takes native handle
        ///
        /// @return A handle to the native mutex handle
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        OSMutexHandle* GetNativeHandle();

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Mutex
        ///
        /// @brief  Default constructor for Mutex object.
        ///
        /// @return A pointer to the new Mutex object
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mutex() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ~Mutex
        ///
        /// @brief  Default destructor for Mutex object.
        ///
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~Mutex() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Initialize
        ///
        /// @brief  Initialize a newly created Mutex object
        ///
        /// @param  pResourceName Human readable name of the resource being protected
        ///
        /// @return None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CDKResult Initialize(
            const CHAR* pResourceName);

        Mutex(const Mutex&) = delete;                ///< Disallow the copy constructor
        Mutex& operator=(const Mutex&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
        pthread_mutex_t     m_mutex;                ///< (Linux) Mutex
        BOOL                m_validMutex;           ///< (Linux) Indicates if underlying mutex is valid
#else
        CRITICAL_SECTION    m_criticalSection;      ///< (Win32) Critical Section implementing Mutex
#endif // defined(_LINUX)
        const CHAR*         m_pResourceName;        ///< Name of resource protected. Needs to persist for object lifetime
    };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable semaphore class implementation
///
/// Basic wrapping of OS semaphore to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Semaphore final
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of Semaphore
    ///
    /// @return Pointer to newly created object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Semaphore* Create();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of Semaphore
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Wait
    ///
    /// @brief  Decrement/wait indefinitely on Semaphore (equivalent to sem P)
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Wait();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TimedWait
    ///
    /// @brief  Wait with timeout on the Semaphore
    ///
    /// @param  timeoutMilliseconds Timeout in milliseconds
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult TimedWait(
        UINT timeoutMilliseconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Signal
    ///
    /// @brief  Increment/signal a Semaphore (equivalent to sem V)
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Signal();

private:

    /// @brief Constructor for Semaphore object.
    Semaphore() = default;

    /// @brief Destructor for Semaphore object.
    ~Semaphore() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Semaphore object
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult Initialize();

    Semaphore(const Semaphore&) = delete;                ///< Disallow the copy constructor
    Semaphore& operator=(const Semaphore&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
    sem_t        m_semaphore;        ///< (Linux) Semaphore
    BOOL         m_validSemaphore;   ///< (Linux) Indicates if underlying semaphore is valid
#else
    HANDLE       m_hSemaphore;       ///< (Windows) Semaphore
#endif // defined(_LINUX)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable condition class implementation
///
/// Basic wrapping of OS condition variable to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Condition final
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of Condition
    ///
    /// @param  pResource pointer to description of resource being conditioned
    ///
    /// @return Pointer to newly created object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Condition* Create(
        const CHAR* pResource);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of Condition
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Wait
    ///
    /// @brief  Wait on the condition
    ///
    /// @param  phMutex OS mutex handle, caller is responsible for using the same handle as used to lock around the wait
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult Wait(
        OSMutexHandle* phMutex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TimedWait
    ///
    /// @brief  Wait with timeout on the Condition
    ///
    /// @param  phMutex                 OS mutex handle, caller is responsible for using the same handle as used to lock
    ///                                 around the wait
    /// @param  timeoutMilliseconds     Timeout in milliseconds
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult TimedWait(
        OSMutexHandle*  phMutex,
        UINT            timeoutMilliseconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Signal
    ///
    /// @brief  Signal the condition (notify one waiting thread)
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Signal();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Broadcast
    ///
    /// @brief  Broadcast the condition (notify all waiting threads)
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Broadcast();

private:
    Condition() = default;
    ~Condition() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Condition object
    ///
    /// @param  pResource pointer to description of resource being conditioned
    ///
    /// @return A result code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult Initialize(
        const CHAR* pResource);

    Condition(const Condition&) = delete;                ///< Disallow the copy constructor
    Condition& operator=(const Condition&) = delete;     ///< Disallow assignment operator

    const CHAR*         m_pResource;                     ///< Name of the resource being conditioned
#if defined(_LINUX)
    pthread_cond_t      m_conditionVar;                  ///< (Linux) Underlying conditional variable
    BOOL                m_validConditionVar;             ///< (Linux) Indicates if condVar is valid
#else
    CONDITION_VARIABLE  m_conditionVar;                  ///< (Windows) Underlying conditional variable
#endif // defined(_LINUX)
};

}

#endif // COMMONUTILS_H