/********************************************************************
  ---------------------------------------------------------------------
  Copyright (c) 2015-2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ----------------------------------------------------------------------
 *********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR FILE

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/27/20   yb      Add listener for tvm solution.
  12/24/17   ac      modified dataLen size check received from listener for DYN_FONT_INDEX
  11/27/16   gs      Added validation for dataLen size received from listener
  04/11/16   sn      Added option to pass NULL gdFont in order to get new dynamic font
  01/13/16   dr      Fixes for listener buffer security
  01/04/16   dr      Add heatmap support
  12/23/15   danyr   Performance improvements
  07/29/15   ng      Add 64 arch support
  04/26/15   ng      Initial Version
  ===========================================================================*/

#include <linux/ion.h>
#include <linux/mem-buf.h>
#include <linux/msm_ion.h>
#include <linux/msm_ion_ids.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "font_manager.h"
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif
#include <time.h>
#include "TUIHeap.h"
#include "TUILog.h"
#include "font_manager_common.h"
#include "secure_display_renderer.h"
#include "memscpy.h"

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/evp.h>


#ifdef ENABLE_PROFILING
#undef ENABLE_PROFILING
#define ENABLE_PROFILING 0
#endif

#ifdef PROFILING
#if (ENABLE_PROFILING)
#define TIME_START                   \
    unsigned long long stop;         \
    unsigned long long start;        \
    struct timeval start_time;       \
    gettimeofday(&start_time, NULL); \
    start = start_time.tv_sec

#define TIME_STOP                   \
    struct timeval stop_time;       \
    gettimeofday(&stop_time, NULL); \
    stop = stop_time.tv_sec;        \
    delay = stop - start
#endif
#else
#define TIME_START \
    do {           \
    } while (0)
#define TIME_STOP \
    do {          \
    } while (0)
#endif

#define ENTER                                   \
    TUILOGD( "%s+", __func__); \
    TIME_START
#define EXIT                                                         \
    {                                                                \
        unsigned long long delay = 0;                                \
        TIME_STOP;                                                   \
        TUILOGD( "%s- (%llu ms)", __func__, delay); \
    }                                                                \
    return;

#define EXITV(x)                                                           \
    {                                                                      \
        unsigned long long delay = 0;                                      \
        TIME_STOP;                                                         \
        TUILOGD( "%s- (0x%08X) (%llu ms)", __func__, (x), \
               delay);                                                     \
    }                                                                      \
    return (x);

#define BYTES_TO_UINT32(x) ((x) / sizeof(uint32_t))
#define UINT32_TO_BYTES(x) ((x) * sizeof(uint32_t))

/* Constants */
#define FONT_SEGMENT_SIGNATURE_SIZE (256)
#define FONT_SEGMENT_DATA_OFFSET \
    (sizeof(uint32_t) + FONT_SEGMENT_SIGNATURE_SIZE)

#define NUM_BIG_INTS_RSA_PUB_KEY (2)
#define EXPONENT_SIZE (3)
#define SHA256_HASH_LEN (32)

#define MAX_NUM_FONTS_IN_CACHE 20
#define INITIAL_FONT_DATA_SIZE_BYTES (20000)
#define MAX_FONT_DATA_SIZE_BYTES \
    (640000)  // based on simsun: 100pt x 140 chars -> ~500K

#define MAX_TXT_LEN (1000)

/* Macros */
#define LOCATION_IN_INDEX_BUF(i) \
    (sizeof(dynamic_index_prefix_t) + i * sizeof(dynamic_font_index_entry_t))

/* Typedef */
typedef struct dynamic_font_cache_entry {
    char fontPath[MAX_FILENAME_LEN]; /* Path to the font file */
    dynamic_font_index_t fontIndex;  /* The index of the font */
    gdFontExt gdFont;                /* Ptr to the font itself */
    uint32_t gdFontOffset; /* Offset to the first free byte in the font
                              data array. The offset is in uint32_t units */
    uint32_t gdFontFreeSpace; /* Free space in the font data array, uint32_t units */
    uint32_t supportedChars[MAX_SUPPORTED_CHARS]; /* Characters that are currently
                                                supported by the font */
} dynamic_font_cache_entry_t;

/* The font cache is an array of font entries */
typedef struct dynamic_font_cache {
    uint8_t numEntries;
    dynamic_font_cache_entry_t fontEntries[MAX_NUM_FONTS_IN_CACHE];
} dynamic_font_cache_t;


/* local buffer for copying the font data received from HLOS
 * Note that FONT_BUFF_SIZE is shared with HLOS and if changed should be updated
 * in both headers */
static uint8_t gfontDataBuffer[FONT_BUFF_SIZE];

static dynamic_font_cache_t gDynFontCache;

/* Font configuration states - updated via configuration set functions in the
 * header */

/* if set to true, font manager will copy static font data into dynamic font */
static uint32_t gConfig_enableStaticToDynFontDataCopy = 0;

static uint32_t gTmpTxtBuff[MAX_TXT_LEN];
static uint32_t gFixedSizeCharsBuff[MAX_TXT_LEN];
static uint32_t gSegmentsBuff[MAX_SUPPORTED_CHARS];

/* Pointers of external arrays that contain the font public key info */
static uint8_t *gPublicRSAExponent = NULL;
static uint8_t *gPublicRSAModulus = NULL;

static font_cb_t gFontMgrCallcback = NULL;

static bool is_file_available(const char* filePath)
{
    struct stat st;

    return (stat(filePath, &st) == 0);
}

/*****************************************************************************************************************/

/**
    @brief
    This function provide a central point for accessing HLOS listener code from font manager
    It supports any type of valid listener request (needs to be part of listener_request_t union),
    receives and validates HLOS listener response, and copies the received buffer into secured memory.
    The listener (HLOS) shared buffer is reused between calls and is registered on the first invocation.
    The HLOS shared buffer pointer is stored into gRegisteredBuf so it can be freed during font manager cleanup.

    NOTE: * No other function should directly access gRegisteredBuf as it might compromise font manager security

    NOTE: ** The function promises that the response data size (respDataSize) is less or equal to the
    provided secured buffer size (respBuffSize), otherwise an error will be returned

    @param[in]      req            pointer to a valid listener request structure
    @param[in]      req_size       size of the provided listener structure
    @param[in]      respBuff       pointer to a secure memory buffer. HLOS NS data will be copied into this buffer
    @param[in]      respBuffSize   size of the secure memory buffer
    @param[out]     respDataSize   size of the data actually received from HLOS listener and copied into respBuff

    @return
    See #font_mgr_err_t for descriptions of return codes.
*/
static font_mgr_err_t font_mgr_request_service(
        listener_request_t *req, uint32_t reqSize,
        uint8_t *respBuff, uint32_t respBuffSize,
        uint32_t *respDataSize)
{
    listener_response_t listenerResponse = {0};
    font_mgr_err_t ret = FONT_MGR_SUCCESS;
    /* note that this is a font manager buffer, and should not be mistaken for a TUI core app buffer */
    uint8_t* appBuffer = NULL;
    uint32_t appBufferLen = 0;
    uint32_t dataLen = 0;
    int32_t qseeRes = 0;
    size_t copied = 0;
    size_t rsp_out = 0;
    int32_t memHandle = 0;
    int memBufFd;
    struct mem_buf_import_ioctl_arg importArgs = {};
    struct acl_entry mAclEntries;

    ENTER;

    if (NULL == req || NULL == respBuff || NULL == respDataSize)
    {
        TUILOGE("%s: invalid input parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (NULL == gFontMgrCallcback) {
        TUILOGE("%s: callback is not registered.", __func__);
        EXITV(FONT_MGR_LISTENER_ERROR);
    }

    /* send request to listener */
    qseeRes = gFontMgrCallcback(req, reqSize,
                    &listenerResponse, sizeof(listener_response_t), &rsp_out);
    if (0 != qseeRes)
    {
        TUILOGE("%s: request listener generic error.", __func__);
        EXITV(FONT_MGR_LISTENER_ERROR);
    }

    if (listenerResponse.dynamicFontRes.n32Status < 0)
    {
        TUILOGE("%s: FONT_MGR_LISTENER_ERROR %d", __func__, listenerResponse.dynamicFontRes.n32Status);
        EXITV(FONT_MGR_LISTENER_ERROR);
    }

    memHandle = listenerResponse.dynamicFontRes.bufferHandle;
    appBufferLen = listenerResponse.dynamicFontRes.u32BufLen;
    dataLen = listenerResponse.dynamicFontRes.u32DataLen;

    /* check if any data was provided back by the listener as some calls are allowed to return no data
     * in that case we don't need any further data validation and/or copy and will return success */
    if (0 == dataLen) {
        TUILOGE("%s: listener returned no data for this call", __func__);
        *respDataSize = 0; /* make sure client will not read anything from the buffer */
        EXITV(FONT_MGR_SUCCESS);
    }

    /* once here dataLen is > 0 so we should validate the returned buffer and the data size are valid */
    if (dataLen > appBufferLen)
    {
        TUILOGE("%s: listener returned invalid buffer", __func__);
        ret = FONT_MGR_BUFFER_ERROR;
        goto cleanup;
    }

    memBufFd = open("/dev/membuf", O_RDONLY);
    if (memBufFd < 0) {
        TUILOGE("%s: Failed to open membuf driver", __func__);
        ret = FONT_MGR_BUFFER_ERROR;
        goto cleanup;

    }

    mAclEntries.vmid = MEM_BUF_VMID_TRUSTED_VM;
    mAclEntries.perms = MEM_BUF_PERM_FLAG_READ;
    importArgs.memparcel_hdl = memHandle;
    importArgs.nr_acl_entries = 1;
    importArgs.acl_list = (__u64)&mAclEntries;
    ret = ioctl(memBufFd, MEM_BUF_IOC_IMPORT, &importArgs);
    if (ret < 0) {
        TUILOGE("%s: import failed! rc = [%d] (%s)", __func__, ret, strerror(-ret));
        ret = FONT_MGR_BUFFER_ERROR;
        goto cleanup;
    }

    appBuffer = (uint8_t*)mmap(
            NULL, appBufferLen, PROT_READ, MAP_SHARED, importArgs.dma_buf_import_fd, 0);
    if (NULL == appBuffer)
    {
        TUILOGE("%s: Filed to import app buffer.", __func__);
        ret = FONT_MGR_BUFFER_ERROR;
        goto cleanup;
    }

    /* copy the shared buffer into local (secure) memory */
    copied = memscpy(respBuff,respBuffSize,appBuffer,dataLen);
    if (copied != dataLen)
    {
        TUILOGE("%s: error while copying listener buffer", __func__);
        ret = FONT_MGR_BUFFER_ERROR;
        goto cleanup;
    }

    /* report to the caller the data size read from the listener */
    *respDataSize = dataLen;

cleanup:
    if (memBufFd > 0) {
        close(memBufFd);
    }

    if (importArgs.dma_buf_import_fd > 0) {
        close(importArgs.dma_buf_import_fd);
    }
    if (appBuffer != NULL) {
        munmap((void *)appBuffer, appBufferLen);
    }

    EXITV(ret);
}
/************************************************************************************/

/**
  @brief
  Internal function for sorting unsigned 32 bit numbers.
  */
static void sort_chars(uint32_t* chars, uint32_t len)
{
    uint32_t i, j, tmp;

    for (i = 0; i < len; i++) {
        for (j = 0; j < len - i - 1; j++) {
            if (chars[j] > chars[j + 1]) {
                tmp = chars[j];
                chars[j] = chars[j + 1];
                chars[j + 1] = tmp;
            }
        }
    }
}

/************************************************************************************/

/**
  @brief
  Internal function for converting utf8 text to fixed size uint32_t numbers.
  Please note that the utf-8 specification covers numbers up to 31 bits long.
  The function also sorts the fixed sized chars
  */
static font_mgr_err_t convert_utf8_to_fixed_size_sorted_chars(
    const uint8_t* text, uint32_t txtLenChars, uint32_t* fixedSizeCharsTxt)
{
    uint32_t i, len, curr_char_len = 0;

    if (NULL == text || 0 == txtLenChars || NULL == fixedSizeCharsTxt) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    /* convert utf8 characters to 32bit numbers */
    for (i = 0, len = 0; i < txtLenChars; i++) {
        fixedSizeCharsTxt[i] = getUnicode(text + len);
        get_utf8_len(text + len, 1, &curr_char_len, NULL);
        len += curr_char_len;
    }

    /* sort the fixed size string */
    sort_chars(fixedSizeCharsTxt, txtLenChars);

    EXITV(FONT_MGR_SUCCESS);
}

/**************************************************************************************/

/**
  @brief
  Internal function for checking whether a font supports a given char
  */
static bool gdfont_supports_text(gdFontExt* gdFont, uint32_t* text,
                                 uint32_t txtLenChars)
{
    uint32_t i;
    int index;
    ENTER;

    if (NULL == text || NULL == gdFont || NULL == gdFont->data ||
        0 == gdFont->nchars) {
        EXITV(false);
    }

    for (i = 0; i < txtLenChars; i++) {
        index = gdFontFindIndex(gdFont->data, gdFont->nchars, (int)text[i]);
        if (-1 == index) {
            TUILOGD( "%s: %d, char %d was not found", __func__, __LINE__,
                   (int)text[i]);
            EXITV(false);
        }
    }

    EXITV(true);
}

/******************************************************************************************/

/**
  @brief
  Internal function for determining the needed font segments given a text to
  display.
  For best performance, the text characters are sorted and both the text and the
  segments index
  are traversed only once.
  */
static font_mgr_err_t determine_needed_segments(
    dynamic_font_cache_entry_t* fontEntry, uint32_t* sortedTxt, uint32_t txtLen,
    uint32_t* segments, uint32_t* numSegments)
{
    uint32_t currCharIndex, currSegIndex;
    uint32_t numOfSegmentsInFont;
    dynamic_font_index_entry_t* indexEntry;
    ENTER;

    if (NULL == fontEntry || NULL == sortedTxt || NULL == segments ||
        NULL == numSegments) {
        TUILOGE("%s: %d - NULL parameters", __func__, __LINE__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (0 == txtLen) {
        TUILOGD( "%s: %d - text len is 0", __func__, __LINE__);

        *numSegments = 0;
        EXITV(FONT_MGR_SUCCESS);
    }

    numOfSegmentsInFont = fontEntry->fontIndex.indexPrefix.numSegments;
    if (0 == numOfSegmentsInFont) {
        TUILOGE("%s: %d corrupted font, num of segments == 0",
               __func__, __LINE__);
        EXITV(FONT_MGR_FONT_CACHE_ERROR);
    }

    /* initialize variables for traversing through segments and characters */
    currCharIndex = 0;
    currSegIndex = 0;
    *numSegments = 0;
    indexEntry = &fontEntry->fontIndex.indexData[currSegIndex];

    /* walk through the segments and the characters */
    while (currCharIndex < txtLen && currSegIndex < numOfSegmentsInFont - 1) {
        /* move the segment pointers until relevant segment is found for current
         * char*/
        while (indexEntry->lastCharIndex < sortedTxt[currCharIndex]) {
            currSegIndex++;

            if (currSegIndex >= numOfSegmentsInFont - 1) {
                TUILOGE("%s: %d -  no font entry for a character %d", __func__, __LINE__,
                       sortedTxt[currCharIndex]);
                EXITV(FONT_MGR_NO_FONT_ENTRY_FOR_CHAR);
            }

            indexEntry = &fontEntry->fontIndex.indexData[currSegIndex];
        }

        if (indexEntry->firstCharIndex > sortedTxt[currCharIndex]) {
            TUILOGE("%s: %d - no font entry for a character %d",
                   __func__, __LINE__, sortedTxt[currCharIndex]);
            EXITV(FONT_MGR_NO_FONT_ENTRY_FOR_CHAR);
        }

        /* store the relevant segment */
        segments[*numSegments] = currSegIndex;
        (*numSegments)++;

        /* skip all additional chars that are included in this segment */
        while (sortedTxt[currCharIndex] >= indexEntry->firstCharIndex &&
               sortedTxt[currCharIndex] <= indexEntry->lastCharIndex) {
            currCharIndex++;
        }
    }

    if (currCharIndex < txtLen) {
        TUILOGE("%s: no font entry for some characters",
               __func__);
        EXITV(FONT_MGR_NO_FONT_ENTRY_FOR_CHAR);
    }

    if (0 == *numSegments) {
        TUILOGE("%s: found no relevant font segments",
               __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    EXITV(FONT_MGR_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for verifying a font segment signature.
*/
static font_mgr_err_t verify_font_segment_signature(uint8_t* segment, uint32_t dataLen)
{
    RSA* r = NULL;
    BIGNUM* e = NULL;
    BIGNUM* n = NULL;
    EVP_PKEY* evpKey = NULL;
    EVP_MD_CTX* mdctx = NULL;
    font_mgr_err_t ret = FONT_MGR_SUCCESS;

    ENTER;
    if (segment == NULL
        || dataLen < FONT_SEGMENT_DATA_OFFSET) {
        TUILOGE("%s: Invalid input.", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    if (gPublicRSAExponent == NULL
        || gPublicRSAModulus == NULL) {
        TUILOGE("%s: Public Key isn't set, font_mgr_config_rsa() should be called,",
              __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    r = RSA_new();
    if (r == NULL) {
        TUILOGE("%s: Failed to create a RSA, r = NULL", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    e = BN_new();
    if (e == NULL) {
        TUILOGE("%s: Failed to create a BN, e = NULL", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    n = BN_new();
    if (n == NULL) {
        TUILOGE("%s: Failed to create a BN, n = NULL", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    BN_bin2bn(gPublicRSAExponent, EXPONENT_SIZE, e);
    BN_bin2bn(gPublicRSAModulus, FONT_SEGMENT_SIGNATURE_SIZE, n);
    if (RSA_set0_key(r, n, e, NULL) != 1) {
        TUILOGE("%s: Failed to set RSA key", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    /* Add all digest algorithms and encryption algorithms to an internal table. */
    OpenSSL_add_all_algorithms();

    evpKey = EVP_PKEY_new();
    if(evpKey == NULL) {
        TUILOGE("%s: Failed to create an EVP_PKEY", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    if(EVP_PKEY_set1_RSA(evpKey, r) != 1) {
        TUILOGE("%s: Failed to set RSA with EVP_PKEY", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    mdctx = EVP_MD_CTX_new();
    if(!EVP_VerifyInit_ex(mdctx, EVP_sha256(), NULL)) {
        TUILOGE("%s: Failed to init EVP verification", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

    if(!EVP_VerifyUpdate(mdctx, segment + FONT_SEGMENT_DATA_OFFSET, dataLen - FONT_SEGMENT_DATA_OFFSET)) {
        TUILOGE("%s: Failed to update EVP verification", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;

    }

    /* if the signature of segment is not right, this step will fail.*/
    if(!EVP_VerifyFinal(mdctx, segment, FONT_SEGMENT_SIGNATURE_SIZE, evpKey)) {
        TUILOGE("%s: Failed to complete EVP verification", __func__);
        ret = FONT_MGR_RSA_ERROR;
        goto cleanup;
    }

cleanup:
    /* release the EVP Key and BIGNUM */
    if (evpKey != NULL)
        EVP_PKEY_free(evpKey);
    else {
        if (e != NULL)
            BN_free(e);
        if (n != NULL)
            BN_free(n);
    }

    if (r != NULL)
        RSA_free(r);

    if (mdctx != NULL)
        EVP_MD_CTX_free(mdctx);

    EXITV(ret);
}

/*******************************************************************************************/

/**
  @brief
  Internal function for storing an index of a font in the font cache.
  */
static font_mgr_err_t font_cache_store_font_index(
    dynamic_font_cache_entry_t* fontEntry, uint8_t* indexBuf, uint32_t dataLen)
{
    uint32_t numSegments;
    size_t indexPrefixSize = sizeof(dynamic_index_prefix_t);
    size_t copied = 0;
    ENTER;

    if (NULL == fontEntry || NULL == indexBuf || 0 == dataLen) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (dataLen < sizeof(dynamic_index_prefix_t)) {
        TUILOGE("%s: invalid font segment size received",
               __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (NULL != fontEntry->fontIndex.indexData) {
        TUILOGE("%s:, corrupt font entry", __func__);
        EXITV(FONT_MGR_FONT_CACHE_ERROR);
    }

    /* store index prefix and make sure all struct bytes were copied */
    copied = memscpy(&(fontEntry->fontIndex.indexPrefix), indexPrefixSize,
                     indexBuf, dataLen);
    if (copied != indexPrefixSize) {
        TUILOGE("%s:, corrupt font index prefix", __func__);
        EXITV(FONT_MGR_FONT_CACHE_ERROR);
    }

    numSegments = fontEntry->fontIndex.indexPrefix.numSegments;

    /* init font general params with data from index: w, h */
    fontEntry->gdFont.w = fontEntry->fontIndex.indexPrefix.w;
    fontEntry->gdFont.h = fontEntry->fontIndex.indexPrefix.h;

    /* allocate and store index data */
    fontEntry->fontIndex.indexData =
        tui_malloc(numSegments * sizeof(dynamic_font_index_entry_t));
    if (NULL == fontEntry->fontIndex.indexData) {
        TUILOGE("%s: index memory allocation error",
               __func__);
        EXITV(FONT_MGR_MEM_ALLOCATION_ERROR);
    }

    /* Copy the segments data */
    memscpy(fontEntry->fontIndex.indexData,
            numSegments * sizeof(dynamic_font_index_entry_t),
            indexBuf + sizeof(dynamic_index_prefix_t),
            dataLen - sizeof(dynamic_index_prefix_t));

    EXITV(FONT_MGR_SUCCESS);
}

/*****************************************************************************************/
/**
  @brief
  Internal function for getting a font index from the file.
  The index data read from the file is copied into indexDataBuffer provided to
  this function
  */
static font_mgr_err_t get_font_index(const char* fontPath,
                                     uint8_t* indexDataBuffer,
                                     uint32_t indexBufferSize,
                                     uint32_t* indexDataLen)
{
    FILE* fontFile = NULL;
    uint32_t numSegments, neededBytesInBuff;
    dynamic_font_index_t indexFont = {0};
    font_mgr_err_t ret = FONT_MGR_SUCCESS;
    ENTER;
    if (0 == strlen(fontPath)) {
        TUILOGE("Error: Invalid fontPath passed");
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }
    /* open the font file using the path parameter */
    fontFile = fopen(fontPath, "rb");
    if (NULL == fontFile) {
        TUILOGE("get_font_index failed to open the font file %s", fontPath);
        EXITV(FONT_MGR_GENERAL_ERROR);
    }
    do {
        /* read the index prefix, contains info about num of segments */
        if (fread(&indexFont.indexPrefix, sizeof(dynamic_index_prefix_t), 1,
                  fontFile) != 1) {
            TUILOGE("get_font_index failed to read index prefix from the font "
                   "file");
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }

        /* store the index */
        numSegments = indexFont.indexPrefix.numSegments;
        if (numSegments > MAX_SUPPORTED_CHARS) {
            TUILOGE("get_font_index failed - error in parameters");
            ret = FONT_MGR_INPUT_PARAMS_ERROR;
            break;
        }

        indexFont.indexData = (dynamic_font_index_entry_t*)
        tui_malloc(numSegments * sizeof(dynamic_font_index_entry_t));
        if (NULL == indexFont.indexData) {
            TUILOGE("get_font_index Failed to allocate memory for font index");
            ret = FONT_MGR_BUFFER_ERROR;
            break;
        }

        /* read the entire index */
        if (fread(indexFont.indexData, sizeof(dynamic_font_index_entry_t),
                  numSegments, fontFile) != numSegments) {
            TUILOGE("get_font_index failed to read the index from the font file");
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }

        numSegments = indexFont.indexPrefix.numSegments;
        /* copy data to the buffer */
        neededBytesInBuff = sizeof(dynamic_index_prefix_t) +
                            numSegments * sizeof(dynamic_font_index_entry_t);
        if (neededBytesInBuff > indexBufferSize) {
            TUILOGE("get_font_index Failed - not enough room for index in the "
                   "font buffer");
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }
        memscpy(indexDataBuffer, indexBufferSize, &(indexFont.indexPrefix),
                sizeof(dynamic_index_prefix_t));
        memscpy(indexDataBuffer + sizeof(dynamic_index_prefix_t),
                indexBufferSize - sizeof(dynamic_index_prefix_t),
                indexFont.indexData,
                numSegments * sizeof(dynamic_font_index_entry_t));

        *indexDataLen = sizeof(dynamic_index_prefix_t) +
                        numSegments * sizeof(dynamic_font_index_entry_t);
    } while (0);

    if (NULL != fontFile) {
        fclose(fontFile);
    }

    if (NULL != indexFont.indexData) {
        tui_free(indexFont.indexData);
    }

    EXITV(ret);
}

/*****************************************************************************************/

/**
  @brief
  Internal function for getting a font index from the file.
  The index data read from the file is copied into indexDataBuffer provided to
  this function
  */
static font_mgr_err_t font_cache_get_font_index(const char* fontPath,
                                                uint8_t* indexDataBuffer,
                                                uint32_t indexBufferSize,
                                                uint32_t* dataLen)
{
    struct dyn_font_req_index req_index;
    font_mgr_err_t ret;
    ENTER;

    if (NULL == fontPath || NULL == dataLen || NULL == indexDataBuffer) {
        TUILOGE("%s: NULL parameter", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (is_file_available(fontPath)) {
        ret = get_font_index(fontPath, indexDataBuffer, indexBufferSize, dataLen);
    } else {
        /* request index */
        req_index.u32CmdId = FONT_MGR_CMD_GET_DYN_FONT_INDEX;
        strlcpy(req_index.fontPath, fontPath, MAX_FILENAME_LEN);

        ret = font_mgr_request_service((listener_request_t *)&req_index, sizeof(req_index),
                indexDataBuffer, indexBufferSize, dataLen);
    }

    if (FONT_MGR_SUCCESS != ret || *dataLen == 0) {
        TUILOGE("%s: error getting font index %d", __func__,
               ret);
        EXITV(ret);
    }

    if (*dataLen < sizeof(dynamic_index_prefix_t)) {
        TUILOGE("%s: invalid font segment size received from the listener %d",
               __func__, ret);
        EXITV(FONT_MGR_LISTENER_ERROR);
    }

    EXITV(FONT_MGR_SUCCESS);
}

/*****************************************************************************************/

/**
  @brief
  Internal function for finding a font cache entry by font path.
  */
static dynamic_font_cache_entry_t* font_cache_find_font_entry(
    const char* fontPath)
{
    uint8_t i;
    ENTER;

    for (i = 0; i < gDynFontCache.numEntries; i++) {
        if (0 == strcmp(gDynFontCache.fontEntries[i].fontPath, fontPath)) {
            EXITV(&gDynFontCache.fontEntries[i]);
        }
    }

    EXITV(NULL);
}

/*************************************************************************************/

/**
  @brief
  Internal function for creating a new font cache entry.
  */
static font_mgr_err_t font_cache_create_new_font_entry(
    const char* fontPath, dynamic_font_cache_entry_t** entry,
    uint8_t* entryIndex)
{
    dynamic_font_cache_entry_t* currEntry;
    ENTER;

    if (NULL == fontPath || NULL == entry || NULL == entryIndex) {
        TUILOGE("%s: NULL params", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    *entry = NULL;

    if (gDynFontCache.numEntries >= MAX_NUM_FONTS_IN_CACHE) {
        TUILOGE("%s: cache is full", __func__);
        EXITV(FONT_MGR_FONT_CACHE_ERROR);
    }

    currEntry = &gDynFontCache.fontEntries[gDynFontCache.numEntries];

    /* gdFont */
    currEntry->gdFont.data = tui_malloc(INITIAL_FONT_DATA_SIZE_BYTES);
    if (NULL == currEntry->gdFont.data) {
        TUILOGE("%s: font data memory allocation error",
               __func__);
        EXITV(FONT_MGR_MEM_ALLOCATION_ERROR);
    }

    /* fontPath */
    strlcpy(currEntry->fontPath, fontPath, MAX_FILENAME_LEN);

    /* index, memory will be allocated when getting the index from the listener
     */
    currEntry->fontIndex.indexData = NULL;

    currEntry->gdFont.nchars = 0;
    currEntry->gdFont.offset = 0;

    /* font data offset, free bytes */
    currEntry->gdFontOffset = 0;
    currEntry->gdFontFreeSpace = BYTES_TO_UINT32(INITIAL_FONT_DATA_SIZE_BYTES);

    /* supported chars */
    memset(currEntry->supportedChars, 0, sizeof(currEntry->supportedChars));

    *entry = currEntry;
    *entryIndex = gDynFontCache.numEntries;

    /* update the font cache entries num */
    gDynFontCache.numEntries++;

    EXITV(FONT_MGR_SUCCESS);
}

/**************************************************************************************/

/**
  @brief
  Internal function for deleting an existing font cache entry.
  */
static font_mgr_err_t font_cache_delete_font_entry(uint8_t entryIndex)
{
    uint8_t i;
    ENTER;

    if (entryIndex > (MAX_NUM_FONTS_IN_CACHE - 1)) {
        TUILOGE("font_cache_delete_font_entry failed, bad param");
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    /* free dynamic memory allocations */
    tui_free(gDynFontCache.fontEntries[entryIndex].gdFont.data);

    /* shift font cache entries left to fill the deleted slot */
    for (i = entryIndex + 1; i < MAX_NUM_FONTS_IN_CACHE; i++) {
        /* copy static data */
        gDynFontCache.fontEntries[i - 1] = gDynFontCache.fontEntries[i];

        /* copy arrays */
        strlcpy(gDynFontCache.fontEntries[i - 1].fontPath,
                gDynFontCache.fontEntries[i].fontPath, MAX_FILENAME_LEN);
        memscpy(gDynFontCache.fontEntries[i - 1].supportedChars,
                MAX_SUPPORTED_CHARS,
                gDynFontCache.fontEntries[i].supportedChars,
                MAX_SUPPORTED_CHARS);
    }

    gDynFontCache.numEntries--;

    EXITV(FONT_MGR_SUCCESS);
}

/************************************************************************************/

/**
  @brief
  Internal function for adding a new font cache entry.
  This function creates the entry and fills in the index data.
  */
static font_mgr_err_t font_cache_add_new_font_entry(
    const char* fontPath, dynamic_font_cache_entry_t** fontEntry)
{
    dynamic_font_cache_entry_t* currEntry;
    font_mgr_err_t ret;
    uint8_t entryIndex;
    uint32_t dataLen;
    ENTER;

    *fontEntry = NULL;

    /* get the index data from listener into the global buffer */
    ret = font_cache_get_font_index(fontPath, gfontDataBuffer,
                                    sizeof(gfontDataBuffer), &dataLen);
    if (FONT_MGR_SUCCESS != ret) {
        TUILOGE("%s: font_cache_get_font_index returned %d",
               __func__, ret);
        EXITV(ret);
    }

    /* create the cache entry for this font */
    ret = font_cache_create_new_font_entry(fontPath, &currEntry, &entryIndex);
    if (FONT_MGR_SUCCESS != ret) {
        TUILOGE("%s: new font entry was not created %d",
               __func__, ret);
        EXITV(ret);
    }

    /* store the received index in the font cache */
    ret = font_cache_store_font_index(currEntry, gfontDataBuffer, dataLen);
    if (FONT_MGR_SUCCESS != ret) {
        TUILOGE("%s: font_mgr_store_font_index returned %d",
               __func__, ret);
        font_cache_delete_font_entry(entryIndex);
        EXITV(ret);
    }

    *fontEntry = currEntry;
    EXITV(FONT_MGR_SUCCESS);
}

/*********************************************************************************************/

/**
  @brief
  Internal function for cleaning up the font cache.
  The function frees all allocated memory and resets the font cache data
  structure.
  */
static font_mgr_err_t font_cache_cleanup()
{
    uint8_t i;
    ENTER;

    for (i = 0; i < gDynFontCache.numEntries; i++) {
        tui_free(gDynFontCache.fontEntries[i].gdFont.data);
        tui_free(gDynFontCache.fontEntries[i].fontIndex.indexData);
    }

    gDynFontCache.numEntries = 0;

    EXITV(FONT_MGR_SUCCESS);
}

/***********************************************************************************************/

/**
  @brief
  Internal function that receives a sorted list of characters and filters out
  characters that are already supported by the font.
  */
static font_mgr_err_t font_cache_filter_missing_chars(
    dynamic_font_cache_entry_t* fontEntry, uint32_t* fixedSizeCharsTxt,
    uint32_t* txtLenInChars)
{
    uint32_t currfontChar, currTxtChar, currTmpChar;
    uint32_t* tmpTxt;
    ENTER;

    if (NULL == fontEntry || NULL == fixedSizeCharsTxt ||
        NULL == txtLenInChars) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    /* temporary buffer for storing characters that are not supported
     * by the font */
    tmpTxt = gTmpTxtBuff;

    currfontChar = 0;
    currTxtChar = 0;
    currTmpChar = 0;

    /* find chars that are not supported by the font */
    while (currfontChar < fontEntry->gdFont.nchars &&
           currTxtChar < *txtLenInChars) {
        /* these characters are not supported */
        while (currTxtChar < *txtLenInChars &&
               fixedSizeCharsTxt[currTxtChar] <
                   fontEntry->supportedChars[currfontChar]) {
            tmpTxt[currTmpChar] = fixedSizeCharsTxt[currTxtChar];
            currTxtChar++;
            currTmpChar++;
        }

        if (currTxtChar >= *txtLenInChars) break;

        while (currfontChar < fontEntry->gdFont.nchars &&
               fontEntry->supportedChars[currfontChar] <
                   fixedSizeCharsTxt[currTxtChar]) {
            currfontChar++;
        }

        while (currfontChar < fontEntry->gdFont.nchars &&
               currTxtChar < *txtLenInChars &&
               fontEntry->supportedChars[currfontChar] ==
                   fixedSizeCharsTxt[currTxtChar]) {
            /* account for possible repeating characters in the sorted text */
            while (currTxtChar + 1 < *txtLenInChars &&
                   fixedSizeCharsTxt[currTxtChar] ==
                       fixedSizeCharsTxt[currTxtChar + 1])
                currTxtChar++;

            currfontChar++;
            currTxtChar++;
        }
    }

    /* copy what is left */
    if (currTxtChar < *txtLenInChars) {
        while (currTxtChar < *txtLenInChars) {
            tmpTxt[currTmpChar] = fixedSizeCharsTxt[currTxtChar];
            currTxtChar++;
            currTmpChar++;
        }
    }

    /* update the text array and its length */
    memscpy(fixedSizeCharsTxt, UINT32_TO_BYTES((*txtLenInChars)), tmpTxt,
            UINT32_TO_BYTES(currTmpChar));
    *txtLenInChars = currTmpChar;

    EXITV(FONT_MGR_SUCCESS);
}

/***************************************************************************************/

/**
  @brief
  Internal function for reallocating the font data array.
  Each allocation is twice as big as previous, until max.
  */
static font_mgr_err_t font_cache_realloc_font_data(
    dynamic_font_cache_entry_t* fontEntry)
{
    uint32_t newAllocSizeBytes;
    int* newData;
    ENTER;

    if (NULL == fontEntry) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (UINT32_TO_BYTES(fontEntry->gdFontOffset + fontEntry->gdFontFreeSpace) ==
        MAX_FONT_DATA_SIZE_BYTES) {
        TUILOGE("%s: maximum space is already allocated",
               __func__);
        EXITV(FONT_MGR_FONT_SIZE_LIMIT_ERROR);
    }

    newAllocSizeBytes = 2 * (UINT32_TO_BYTES(fontEntry->gdFontOffset + fontEntry->gdFontFreeSpace));
    if (newAllocSizeBytes > MAX_FONT_DATA_SIZE_BYTES)
        newAllocSizeBytes = MAX_FONT_DATA_SIZE_BYTES;

    newData = tui_malloc(newAllocSizeBytes);
    if (NULL == newData) {
        TUILOGE("%s: memory allocation error", __func__);
        EXITV(FONT_MGR_MEM_ALLOCATION_ERROR);
    }

    /* copy the data to the new location */
    memscpy(newData, newAllocSizeBytes, fontEntry->gdFont.data,
            UINT32_TO_BYTES(fontEntry->gdFontOffset));

    /* update the font entry */
    tui_free(fontEntry->gdFont.data);

    fontEntry->gdFont.data = newData;
    fontEntry->gdFontFreeSpace =
        BYTES_TO_UINT32(newAllocSizeBytes) - fontEntry->gdFontOffset;

    EXITV(FONT_MGR_SUCCESS);
}

/******************************************************************************************/

/**
  @brief
  Internal function for adding a new character to a font.
  The function find the character inside a font segment and copies only
  that character data to the font
  */
static font_mgr_err_t font_cache_update_font_with_char(
    dynamic_font_cache_entry_t* fontEntry, uint32_t ch, uint32_t** segData,
    uint32_t* dataLen)
{
    uint32_t offset, end;
    uint32_t charDataSize, charToSeek;
    uint32_t* dataPtr;
    font_mgr_err_t ret;
    ENTER;

    if (NULL == fontEntry || NULL == segData || NULL == *segData ||
        NULL == dataLen || 0 == *dataLen) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    /* find the offset to the character*/
    dataPtr = *segData;
    charToSeek = (uint32_t)(-1 * (int)ch);

    for (offset = 0; offset < BYTES_TO_UINT32(*dataLen); offset++) {
        if (dataPtr[offset] == charToSeek) break;
    }

    if (UINT32_TO_BYTES(offset) == *dataLen) {
        TUILOGE("%s: no char data in segment", __func__);
        EXITV(FONT_MGR_NO_FONT_ENTRY_FOR_CHAR);
    }

    /* find the next character in the font and calculate this char size */
    end = offset + 1;
    charDataSize = 0;
    while ((int)dataPtr[end] >= 0 && end < BYTES_TO_UINT32(*dataLen)) end++;

    charDataSize = end - offset;

    /* check if there is enough free memory in the font,
     * if not, try to allocate more */
    if (charDataSize > fontEntry->gdFontFreeSpace) {
        ret = font_cache_realloc_font_data(fontEntry);
        if (FONT_MGR_SUCCESS != ret) {
            TUILOGE("%s font_cache_realloc_font_data returned %d", __func__,
                   ret);
            EXITV(ret);
        }
    }

    /* copy the character data */
    memscpy(fontEntry->gdFont.data + fontEntry->gdFontOffset,
            UINT32_TO_BYTES(fontEntry->gdFontFreeSpace), (*segData) + offset,
            UINT32_TO_BYTES(charDataSize));

    fontEntry->gdFontOffset += charDataSize;
    fontEntry->gdFontFreeSpace -= charDataSize;

    fontEntry->supportedChars[fontEntry->gdFont.nchars] = ch;

    /* update the first char of the font, if relevant */
    if ((0 == fontEntry->gdFont.nchars) ||
        (0 != fontEntry->gdFont.nchars && ch < fontEntry->gdFont.offset))
        fontEntry->gdFont.offset = ch;

    fontEntry->gdFont.nchars++;
    sort_chars(fontEntry->supportedChars, fontEntry->gdFont.nchars);

    /* advance the pointer to the segment data in order
     * to shorten the next search */
    *segData = dataPtr + end;
    *dataLen -= UINT32_TO_BYTES(end);

    EXITV(FONT_MGR_SUCCESS);
}

/********************************************************************************************/

/**
  @brief
  Internal function for storing a set of characters in a font.
  The function stores the character that are relevant for the current segment
  only.
  Characters are copied one by one
  */
static font_mgr_err_t font_cache_store_chars_in_font(
    dynamic_font_cache_entry_t* fontEntry, uint32_t segIndex, uint32_t* segData,
    uint32_t dataLen, uint32_t** text, uint32_t* txtLenInChars)
{
    uint32_t segFirstChar, segLastChar;
    uint32_t* txtPtr;
    font_mgr_err_t ret;
    ENTER;

    if (NULL == fontEntry || NULL == segData || 0 == dataLen || NULL == text ||
        NULL == *text || NULL == txtLenInChars || 0 == *txtLenInChars) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    txtPtr = *text;

    if (segIndex > fontEntry->fontIndex.indexPrefix.numSegments) {
        TUILOGE("%s: segIndex %d out of range", __func__,
               segIndex);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    segFirstChar = fontEntry->fontIndex.indexData[segIndex].firstCharIndex;
    segLastChar = fontEntry->fontIndex.indexData[segIndex].lastCharIndex;

    if (txtPtr[0] < segFirstChar || txtPtr[0] > segLastChar) {
        TUILOGE("%s: text is not as expected", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    /* continue while current character is within the segment */
    while (*txtPtr >= segFirstChar && *txtPtr <= segLastChar &&
           (txtPtr - *text) < *txtLenInChars) {
        /* add repeating characters only once */
        while ((txtPtr + 1 - *text) < *txtLenInChars &&
               *txtPtr == *(txtPtr + 1))
            txtPtr++;

        ret = font_cache_update_font_with_char(fontEntry, *txtPtr, &segData,
                                               &dataLen);
        if (FONT_MGR_SUCCESS != ret) {
            TUILOGE("%s: copy_char_line_to_gdfont returned %d", __func__, ret);

            /* advance the text pointers in any case */
            *txtLenInChars -= (txtPtr - *text);
            *text = txtPtr;

            EXITV(FONT_MGR_NO_FONT_ENTRY_FOR_CHAR);
        }

        txtPtr++;
    }

    /* shorten the text to handle */
    *txtLenInChars -= (txtPtr - *text);
    *text = txtPtr;

    EXITV(FONT_MGR_SUCCESS);
}
/********************************************************************************************/
/**
  @brief
  Internal function for getting dynamic font segment
  */
static font_mgr_err_t get_dynamic_font_segment(const char* fontPath,
                                               uint32_t u32SegIndex,
                                               uint8_t* segDataBuffer,
                                               uint32_t segBufferSize,
                                               uint32_t* dataLen)
{
    FILE* fontFile = NULL;
    uint32_t seekOffset, segmentSize;
    dynamic_font_cache_entry_t* indexCacheEntry;
    font_mgr_err_t ret = FONT_MGR_SUCCESS;
    ENTER;

    if (0 == strlen(fontPath)) {
        TUILOGE("error in input parameters");
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (NULL == (indexCacheEntry = font_cache_find_font_entry(fontPath))) {
        TUILOGE("get_dynamic_font_segment failed to find the index in cache");
        EXITV(FONT_MGR_GENERAL_ERROR);
    }

    do {
        /* open the font file using the path parameter */
        fontFile = fopen(fontPath, "rb");
        if (NULL == fontFile) {
            TUILOGE("get_dynamic_font_index failed to open the font file %s",
                   fontPath);
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }

        /* segment indexes are zero based, last segment is dummy segment */
        if (u32SegIndex >= indexCacheEntry->fontIndex.indexPrefix.numSegments - 1) {
            TUILOGE("get_dynamic_font_segment illegal segment index %d",
                   u32SegIndex);
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }

        /* read segment data */
        seekOffset = indexCacheEntry->fontIndex.indexData[u32SegIndex].offset;
        if (0 != fseek(fontFile, seekOffset, SEEK_SET)) {
            TUILOGE("get_dynamic_font_segment failed to seek into the font file");
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }

        segmentSize = indexCacheEntry->fontIndex.indexData[u32SegIndex + 1].offset - seekOffset;
        if (segmentSize > segBufferSize) {
            TUILOGE("get_dynamic_font_index failed - not enough room for font "
                   "segment in the font buffer");
            ret = FONT_MGR_BUFFER_ERROR;
            break;
        }

        if (fread(segDataBuffer, segmentSize, 1, fontFile) != 1) {
            TUILOGE("get_dynamic_font_segment failed to read font segment %d "
                   "from the font file",
                   u32SegIndex);
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }

        *dataLen = segmentSize;
    } while (0);

    if (NULL != fontFile) fclose(fontFile);

    EXITV(ret);
}
/*******************************************************************************************/

/**
  @brief
  Internal function for appending a single segment to a font
  */
static font_mgr_err_t font_cache_append_segment_to_font(
    dynamic_font_cache_entry_t* fontEntry, uint32_t segIndex, uint32_t** text,
    uint32_t* txtLenInChars)
{
    struct dyn_font_req_segment req_segment;
    font_mgr_err_t ret;
    uint8_t* data;
    uint32_t dataLen;
    bool signature_verification = false;

    ENTER;

    if (NULL == fontEntry || NULL == text || NULL == txtLenInChars) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (is_file_available(fontEntry->fontPath)) {
        ret = get_dynamic_font_segment(fontEntry->fontPath, segIndex, gfontDataBuffer,
                                     sizeof(gfontDataBuffer), &dataLen);
    } else {
        /* get the segment from the listener */
        req_segment.u32CmdId = FONT_MGR_CMD_GET_DYN_FONT_SEGMENT;
        strlcpy(req_segment.fontPath, fontEntry->fontPath, MAX_FILENAME_LEN);
        req_segment.u32SegIndex = segIndex;

        ret = font_mgr_request_service((listener_request_t*)&req_segment, sizeof(req_segment),
                gfontDataBuffer, sizeof(gfontDataBuffer), &dataLen);
        signature_verification = true;
    }

    if (FONT_MGR_SUCCESS != ret) {
        TUILOGE("%s: error getting font segment %d", __func__,
               ret);
        EXITV(ret);
    }

    if (dataLen < FONT_SEGMENT_DATA_OFFSET) {
        TUILOGE("%s: invalid font segment size received from the listener %d",
               __func__, ret);
        EXITV(FONT_MGR_LISTENER_ERROR);
    }

    data = gfontDataBuffer;

    /* verify font segment signature first */
    if (signature_verification) {
        ret = verify_font_segment_signature(data, dataLen);
        if (FONT_MGR_SUCCESS != ret)
        {
            TUILOGE("%s: signature verification returned %d", __func__, ret);
            EXITV(ret);
        }
    }
    /* advance beyond the signature */
    data += FONT_SEGMENT_DATA_OFFSET;
    dataLen -= FONT_SEGMENT_DATA_OFFSET;

    ret = font_cache_store_chars_in_font(fontEntry, segIndex, (uint32_t*)data,
                                         dataLen, text, txtLenInChars);
    if (FONT_MGR_SUCCESS != ret) {
       TUILOGE("%s: font_cache_store_chars_in_font returned %d", __func__, ret);
        EXITV(ret);
    }

    EXITV(FONT_MGR_SUCCESS);
}

/*******************************************************************************************/
/**
  @brief
  Internal function for setting the file offset for font warm up segments
  */

static font_mgr_err_t set_offset_font_warm_up_segments(const char* fontPath,
                                                       uint32_t* segments,
                                                       uint32_t numSegments)
{
    dynamic_font_cache_entry_t* indexCacheEntry;
    uint32_t seekOffset, segmentSize;
    FILE* fontFile = NULL;
    font_mgr_err_t ret = FONT_MGR_SUCCESS;
    uint32_t i = 0;

    ENTER;

    if (0 == strlen(fontPath) || numSegments > MAX_SUPPORTED_CHARS) {
        TUILOGE("Error in input parameters");
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    TUILOGD("font_warm_up_segments: received warmup struct for font: %s, with "
           "%d segments to load",
           fontPath, numSegments);
    indexCacheEntry = font_cache_find_font_entry(fontPath);
    if (NULL == indexCacheEntry) {
        TUILOGE("set_offset_font_warm_up_segments failed to get font entry");
        EXITV(FONT_MGR_GENERAL_ERROR);
    }

    do {
        fontFile = fopen(fontPath, "rb");
        if (NULL == fontFile) {
            TUILOGE("set_offset_font_warm_up_segments failed to open the font "
                   "file %s",
                   fontPath);
            ret = FONT_MGR_GENERAL_ERROR;
            break;
        }
        uint32_t u32SegIndexes[MAX_SUPPORTED_CHARS] = {0};
        memscpy(u32SegIndexes, MAX_SUPPORTED_CHARS, segments,
                numSegments * sizeof(uint32_t));
        /* for each segment calculate size/offest and dispatch a hint to the OS
         * using posix_fadvise */
        for (i = 0; i < numSegments; i++) {
            /* check that current segment index is valid */
            if (u32SegIndexes[i] >=
                indexCacheEntry->fontIndex.indexPrefix.numSegments - 1) {
                TUILOGD("set_offset_font_warm_up_segments invalid segment "
                       "passed for font %s",
                       fontPath);
                continue;
            }
            /* prefetch the segment into OS file cache */
            seekOffset = indexCacheEntry->fontIndex.indexData[u32SegIndexes[i]].offset;
            segmentSize = indexCacheEntry->fontIndex.indexData[u32SegIndexes[i] + 1].offset -
                seekOffset;
            if (0 != posix_fadvise(fileno(fontFile), seekOffset, segmentSize,
                                   POSIX_FADV_WILLNEED)) {
                TUILOGE("NOTE: set_offset_font_warm_up_segments failed to hint "
                       "OS cache for font file %s, segment %d",
                       fontPath, i);
            }
        }

    } while (0);

    if (NULL != fontFile) {
        fclose(fontFile);
    }
    EXITV(ret);
}

/***************************************************************************************/

/**
  @brief
  This function is called once per font file to warmup HLOS file cache.
  Will prefetch the required file segments into the OS file cache and improve
  the
  performance of subsequent calls to the font_cache_append_segment_to_font
  function.

  Note, that this function is for performance improvement only and there should
  be no
  other side-effects.


  @param[in]      fontPath      Path to the dynamic font file
  @param[in]      segments      array filled with segment indexes
  @param[in]      numSegments   number of entries in the 'segments' array

  @return
  See #font_mgr_err_t for descriptions of return codes.
  */

static font_mgr_err_t font_warm_up_segments(const char* fontPath,
                                            uint32_t* segments,
                                            uint32_t numSegments)
{
    struct dyn_font_warmup_segments req_segment_warmup;
    int ret = 0;
    uint32_t dataLen = 0;

    ENTER;
    if (NULL == fontPath || NULL == segments ||
        numSegments > MAX_SUPPORTED_CHARS) {
        TUILOGE("%s: invalid parameter", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (strlen(fontPath) > MAX_FILENAME_LEN) {
        TUILOGE("%s: fontPath is too long (%d vs. %d)",
               __func__, strlen(fontPath), MAX_FILENAME_LEN);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (0 == numSegments) {
        TUILOGD("%s: skipping cache warmup since no real segment was needed",
               __func__);
        EXITV(FONT_MGR_SUCCESS);
    }

    if (is_file_available(fontPath)) {
        ret = set_offset_font_warm_up_segments(fontPath, segments, numSegments);
    } else {
        //send a warmup request to listener
        req_segment_warmup.u32CmdId = FONT_MGR_CMD_WARM_UP_DYN_FONT_SEGMENTS;
        strlcpy(req_segment_warmup.fontPath, fontPath, MAX_FILENAME_LEN);
        memscpy(req_segment_warmup.u32SegIndexes,MAX_SUPPORTED_CHARS,segments,numSegments*sizeof(uint32_t));
        req_segment_warmup.numSegments = numSegments;

        ret = font_mgr_request_service((listener_request_t *)&req_segment_warmup, sizeof(req_segment_warmup),
                gfontDataBuffer, sizeof(gfontDataBuffer), &dataLen);
    }
    if (FONT_MGR_SUCCESS != ret) {
        TUILOGE("%s: set offset font warm up segments returned error: %d",
               __func__, ret);
        EXITV(FONT_MGR_GENERAL_ERROR);
    }

    EXITV(FONT_MGR_SUCCESS);
}

/*********************************************************************************************/

/**
  @brief
  Calculates the size (in bytes) of the gdFontExt->data

  Performs actual parsing of the font to calculate the size of the data array.
  Main assumption is that the data array provided has correct format. Otherwise
the function
  can continue to read beyond the data buffer and return incorrect data size or
crash / hang.
  Performance impact up to 2-3ms (for a 500Kb font data)

Note:
Currently this is the only way to get the font data length.
In the future layout should provide such data and this function can be
removed / replaced by direct read from the layout structure

@param[in]  gdFont    gdFontExt structure
@param[out]  length    returns the calculated length

@return
See #font_mgr_err_t for descriptions of return codes.
*/
static font_mgr_err_t gdfont_calculate_data_size(gdFontExt* gdStaticFont,
                                                 uint32_t* length)
{
    uint64_t charCalculatedLength = 0; /* calculated number of actual pixels in a character */
    uint32_t charDecodedLength = 0; /* accumulator for decoded number of a character pixels */
    uint32_t fontDataSize = 0; /* accumulator for the total font data size in bytes */
    int* dataPtr = NULL;
    int i = 0;

    ENTER;
    if (NULL == gdStaticFont || NULL == gdStaticFont->data || NULL == length) {
        TUILOGE("%s: invalid parameter", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }
    fontDataSize = 0;
    dataPtr = gdStaticFont->data;
    /* loop over font characters */
    for (i = 0; i < gdStaticFont->nchars; i++) {
        /* calculate expected decoded length as first three data values are
         * (char, width, height) */
        charCalculatedLength = dataPtr[1] * dataPtr[2];

        /* we always expect a character (negative value) as we begin an
         * iteration
         * also check that this character dimensions and data size are correct*/
        if (dataPtr[0] >= 0 || dataPtr[1] < 0 || dataPtr[1] > gdStaticFont->w ||
            dataPtr[2] < 0 || dataPtr[2] > gdStaticFont->h ||
            charCalculatedLength >= UINT32_MAX) {
            TUILOGE("%s: invalid font data format", __func__);
            EXITV(FONT_MGR_GENERAL_ERROR);
        }

        /* advance to the start of the compressed pixel data */
        fontDataSize += 3 * sizeof(int);
        dataPtr += 3;

        /* loop over character compressed data and count actual pixels
         * each iteration handles a pair (value, count) - according to data
         * compression format */
        charDecodedLength = 0;
        while (dataPtr[0] >= 0 && charDecodedLength < charCalculatedLength) {
            /* we only count number of pixels (dataPtr[1]) and disregard the
             * pixel value (dataPtr[0]) */
            charDecodedLength += dataPtr[1];
            dataPtr += 2;
            fontDataSize += 2 * sizeof(int); /* font size is counted in bytes */
        }
    }
    *length = fontDataSize;
    EXITV(FONT_MGR_SUCCESS);
}

/*****************************************************************************************/

/**
  @brief
  Copy all the characters data from the gdfont into dynamic font cache.
  In case of limited memory this function will perform partial copy and return
  success result

  @param[in]  toFontEntry    dynamic font that will be updated
  @param[in]  fromGdFont    static font (gdfont) which characters will be copied

  @return
  See #font_mgr_err_t for descriptions of return codes.
  */
static font_mgr_err_t font_cache_copy_gdfont_characters(
    dynamic_font_cache_entry_t* toFontEntry, gdFontExt* fromGdFont)
{
    uint32_t* dataPtr = NULL;
    uint32_t dataLen = 0;
    uint32_t ch;
    font_mgr_err_t res = FONT_MGR_SUCCESS;
    int i = 0;

    ENTER;
    if (NULL == toFontEntry || NULL == fromGdFont || NULL == fromGdFont->data) {
        TUILOGE("%s: invalid parameter", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    dataPtr = (uint32_t*)fromGdFont->data;
    /* calculate gdfont data length since it is not available from layout
     * structs */
    res = gdfont_calculate_data_size(fromGdFont, &dataLen);
    if (res != FONT_MGR_SUCCESS) {
        TUILOGE("%s: cannot calculate gdfont data length, error: %d", __func__,
               res);
        EXITV(res);
    }
    TUILOGD("%s: gdfont_calculate_data_size returned %d",
           __func__, dataLen);

    /* iterate over gdfont characters and copy them into dynamic font cache */
    for (i = 0; i < fromGdFont->nchars; i++) {
        ch = (uint32_t)(-((int)dataPtr[0])); /* negate to get the character value */
        /* copy the character into dynamic font cache */
        res = font_cache_update_font_with_char(toFontEntry, ch, &dataPtr,
                                               &dataLen);
        if (res != FONT_MGR_SUCCESS) {
            TUILOGE("%s: character (%d) copy failed with error: %d", __func__,
                   ch, res);
            break; /* even if we copied only part of the symbols - report
                      completion as success */
        }
    }
    TUILOGD("%s: successfully copied %d gdfont characters to dynamic font",
           __func__, i);
    EXITV(FONT_MGR_SUCCESS);
}

/**************************************************************************************/

/**
  @brief
  External function that prepares a gdFont for a layout object.
  If the existing font is static and supports the needed text, nothing is
  changed.
  If not, a dynamic font is prepared for the object.

  @param[in]      fontPath      Path to the dynamic font file
  @param[in]      text          The text that needs to be supported by the font.
  Text is in UTF-8 format
  @param[in]      txtLen        Length of the text in characters
  @param[in, out] gdFont        As input, this is the current font.
  As output, this is the updated font that is
  able to support the given text

  @return
  See #font_mgr_err_t for descriptions of return codes.
  */
font_mgr_err_t font_mgr_prepare_gdfont(const char* fontPath,
                                       const uint8_t* text,
                                       uint32_t txtLenChars, gdFontExt** gdFont)
{
    uint32_t numSegments = 0, i;
    uint32_t *txtPtr, *fixedSizeCharsTxt = NULL;
    uint32_t* segments = NULL;
    dynamic_font_cache_entry_t* fontEntry;
    font_mgr_err_t ret;
    ENTER;

    if (NULL == fontPath || NULL == text || NULL == gdFont) {
        TUILOGE("%s: NULL parameters", __func__);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (0 == txtLenChars) {
        TUILOGD( "%s: got 0 length text, exiting", __func__);
        EXITV(FONT_MGR_SUCCESS);
    }

    if (strlen(fontPath) > MAX_FILENAME_LEN) {
        TUILOGE("%s: fontPath is too long (%d vs. %d)",
               __func__, strlen(fontPath), MAX_FILENAME_LEN);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    if (txtLenChars > MAX_TXT_LEN) {
        TUILOGE("%s: text len in chars is too long (%d vs. %d)", __func__,
               txtLenChars, MAX_TXT_LEN);
        EXITV(FONT_MGR_INPUT_PARAMS_ERROR);
    }

    /* use one of the static buffers for the temporary sorted text */
    fixedSizeCharsTxt = gFixedSizeCharsBuff;
    ret = convert_utf8_to_fixed_size_sorted_chars(text, txtLenChars,
                                                  fixedSizeCharsTxt);
    if (ret != FONT_MGR_SUCCESS) {
        TUILOGE("%s: char conversion returned %d", __func__,
               ret);
        EXITV(FONT_MGR_INVALID_UTF8_STR);
    }

    do {
        fontEntry = font_cache_find_font_entry(fontPath);

        /* if no entry in the font cache matches the fontPath
         * or font cache entry is not the same as the given gdFont pointer
         * then this is a static font pointer.
         * check if it can support the given text */
        if (NULL == fontEntry || (&(fontEntry->gdFont) != *gdFont)) {
            if (gdfont_supports_text(*gdFont, fixedSizeCharsTxt, txtLenChars)) {
                TUILOGD("%s: all chars are supported by static font", __func__);
                EXITV(FONT_MGR_SUCCESS);
            }

            TUILOGD( "%s: working with dynamic font",
                   __func__);

            /* static font pointer does not support the text
             * switch to working dynamically
             * create new font entry, if needed */
            if (NULL == fontEntry) {
                ret = font_cache_add_new_font_entry(fontPath, &fontEntry);
                if (ret != FONT_MGR_SUCCESS) {
                    TUILOGE("%s: font_cache_get_gdfont returned %d", __func__,
                           ret);
                    break;
                }

                if (gConfig_enableStaticToDynFontDataCopy) {
                    /* heatmap support - initialize the dynamic font data with
                     * all
                     * the available static font characters  */
                    ret = font_cache_copy_gdfont_characters(fontEntry, *gdFont);
                    if (ret != FONT_MGR_SUCCESS) {
                        TUILOGE("%s: font_cache_copy_gdfont_characters returned "
                               "%d. Heatmap support unavailable",
                               __func__, ret);
                        ret = FONT_MGR_SUCCESS; /* continue even if no font
                                                   heat-map support available */
                    }
                } else {
                    TUILOGD("%s: heatmap support is disabled via configuration "
                           "parameter", __func__);
                }
            }
        }

        /* leave only missing chars in the array, update txt len */
        ret = font_cache_filter_missing_chars(fontEntry, fixedSizeCharsTxt,
                                              &txtLenChars);
        if (ret != FONT_MGR_SUCCESS) {
            TUILOGE("%s: font_cache_filter_missing_chars returned %d", __func__,
                   ret);
            break;
        }

        TUILOGD( "%s: missing %d chars", __func__,
               txtLenChars);

        /* determine needed segments for the text*/
        segments = gSegmentsBuff;
        ret = determine_needed_segments(fontEntry, fixedSizeCharsTxt,
                                        txtLenChars, segments, &numSegments);
        if (ret != FONT_MGR_SUCCESS) {
            TUILOGE("%s: determine_needed_font_segments returned %d", __func__,
                   ret);
            break;
        }

        /* warmup the listener caches */
        ret = font_warm_up_segments(fontPath, segments, numSegments);
        if (ret != FONT_MGR_SUCCESS) {
            TUILOGH("%s: font_warm_up_segments returned %d. Continuing without "
                   "warmup", __func__, ret);
            ret = FONT_MGR_SUCCESS;  // the rest of the code should run even if
                                     // warmup fails for some reason
        }

        /* get needed segments */
        txtPtr = fixedSizeCharsTxt;
        for (i = 0; i < numSegments; i++) {
            /* the text contains only chars that have corresponding segments
             * and text characters are sorted.
             * txtPtr is expected to advance to the chars of the next segment
             * each time */
            ret = font_cache_append_segment_to_font(fontEntry, segments[i],
                                                    &txtPtr, &txtLenChars);
            if (ret != FONT_MGR_SUCCESS) {
               TUILOGE("%s: font_cache_append_segment_to_font returned %d",
                       __func__, ret);
                break;
            }
        }
    } while (0);

    /* if the process succeeded, update the font pointer */
    if (FONT_MGR_SUCCESS == ret) *gdFont = &(fontEntry->gdFont);

    EXITV(ret);
}

/**************************************************************************************/

/**
  @brief
  External function for cleanup on application shutdown
  */
void font_mgr_cleanup()
{
    font_cache_cleanup();
}

/**************************************************************************************/

/**
  @brief
  External function for setting font manager configuration (see header)
  */
font_mgr_err_t font_mgr_cfg_enable_static_to_dynamic_font_copy(uint8_t enabled)
{
    gConfig_enableStaticToDynFontDataCopy = enabled;
    return FONT_MGR_SUCCESS;
}

/**************************************************************************************/

/**
  @brief
  External function for registering a callback for font manager (see header)
  @param[in]      cb   callback to be registered.
  @return              result of this function.
  */
font_mgr_err_t font_mgr_register_callback(font_cb_t cb)
{
    font_mgr_err_t ret = FONT_MGR_INPUT_PARAMS_ERROR;

    if (cb) {
        gFontMgrCallcback = cb;
        ret = FONT_MGR_SUCCESS;
    }
    return ret;
}

/**************************************************************************************/

/**
  @brief
  External function for Setting RSA parameters (see header)
  */
void font_mgr_config_rsa(uint8_t *rsa_exponent, uint8_t *rsa_modules)
{
    if (rsa_exponent && rsa_modules) {
        gPublicRSAExponent = rsa_exponent;
        gPublicRSAModulus = rsa_modules;
    }
}


