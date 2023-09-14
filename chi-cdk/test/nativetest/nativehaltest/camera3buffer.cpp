//******************************************************************************
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "camera3buffer.h"

namespace haltests {

static __inline UINT32 AlignGeneric32(
    UINT32 operand,
    UINT   alignment)
{
    UINT remainder = (operand % alignment);

    return (0 == remainder) ? operand : operand - remainder + alignment;
}

static __inline BOOL IsPowerOfTwo(
    SIZE_T val)
{
    return ((val == 0) ? FALSE : (((val & (val - 1)) == 0) ? TRUE : FALSE));
}

/**********************************************************************
/// ByteAlign32
///
/// @brief  This function is used to align up any UINT32 value to a specific power of 2
///
/// @param  operand     value to be aligned
/// @param  alignment   desired alignment
///
/// @return Value aligned as specified
///
/// @remark Use The ByteAlignPtr() function for pointer calculations
**********************************************************************/
static __inline UINT32 ByteAlign32(
    UINT32 operand,
    UINT   alignment)
{
    // This function only works for a power-of-2 alignment
    if (!IsPowerOfTwo(alignment))
    {
        return operand;
    }

    return ((operand + (alignment - 1)) & ~(static_cast<UINT32>(alignment) - 1));
}

/**********************************************************************
/// PadToSize
///
/// @brief  Adjust size to be an even multiple of the padding requirement.
///
/// @param  size    The size to pad.
/// @param  padding The padding requirement.
///
/// @return The adjusted size.
**********************************************************************/
static __inline SIZE_T PadToSize(
    SIZE_T size,
    SIZE_T padding)
{
    return ((size + padding - 1) / padding * padding);
}

CamxBuffer::CamxBuffer(): bufferHandle(nullptr), bufferSize(0), mHeight(0), mWidth(0), mStride(0)
{
}

CamxBuffer::CamxBuffer(uint32_t width, uint32_t height, int format)
{

    mStride = width;
    mWidth = width;
    mHeight = height;
    // Add image info based on format, width and height requested
    // May add addtional test control later

    Format camxFormat = Format::RawPlain16;
    if (format == HAL_PIXEL_FORMAT_YCbCr_420_888 ||
        format == HAL_PIXEL_FORMAT_YCrCb_420_SP)
    {
        camxFormat = Format::YUV420NV12;
    }
    else if (format == HAL_PIXEL_FORMAT_RAW16)
    {
        camxFormat = Format::RawPlain16;
    }
    else if (format == HAL_PIXEL_FORMAT_RAW_OPAQUE ||
        format == HAL_PIXEL_FORMAT_RAW10)
    {
        camxFormat = Format::RawMIPI;
    }
    else if (format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED)
    {
        camxFormat = GetCamXFormatFromSettings();
    }

    const unsigned int ImageFormatParamsSizeBytes = sizeof(FormatParams);
    const unsigned int BufferAddressSizeBytes = 8;
    const unsigned int NativeHandleDataSizeBytes = ImageFormatParamsSizeBytes + BufferAddressSizeBytes;

    size_t nativeHandleSize = sizeof(native_handle_t) + NativeHandleDataSizeBytes;
    // creating a new instance of native_handle for this buffer
    bufferHandle = reinterpret_cast<native_handle_t*>(malloc(nativeHandleSize));

    NT_ASSERT(bufferHandle, "could not get buffer handle");

    bufferHandle->version = sizeof(native_handle_t);
    bufferHandle->numFds = 0;
    bufferHandle->numInts = 2;

    unsigned int * pImageBufferParams = reinterpret_cast<unsigned int*>(bufferHandle + 1);
    pImageBufferParams++; // Skip buffer address
    pImageBufferParams++; // Skip buffer address

    ImageFormat * pImageFormat = reinterpret_cast<ImageFormat*>(pImageBufferParams);

    pImageFormat->width = mWidth;
    pImageFormat->height = mHeight;
    pImageFormat->format = camxFormat;
    pImageFormat->colorSpace = ColorSpace::BT601Full;
    pImageFormat->rotation = Rotation::CW0Degrees;

    InitializeFormatParams(pImageFormat);

    bufferSize = CalculateBufferSize(pImageFormat);

    // allocating buffer and storing the address in data[]
    *reinterpret_cast<intptr_t*>(bufferHandle->data) = reinterpret_cast<intptr_t>(MallocAligned(bufferSize,
        pImageFormat->alignment));
}

CamxBuffer::~CamxBuffer()
{
}

buffer_handle_t * CamxBuffer::GetNativeHandle()
{
    return reinterpret_cast<buffer_handle_t*>(const_cast<const native_handle_t **>(&bufferHandle));
}

int CamxBuffer::FreeBuffer()
{
    if (bufferHandle)
    {
        if (bufferHandle->version != sizeof(native_handle_t) && static_cast<void *>(bufferHandle->data) == nullptr)
            return -EINVAL;
        void *bufferData  = (reinterpret_cast<void *>(*reinterpret_cast<intptr_t *>(bufferHandle->data)));
#ifdef _MSC_VER
        _aligned_free(bufferData);
#else
        free(bufferData);
#endif
        bufferSize = 0;
        free(bufferHandle);
    }
    return 0;
}

void* CamxBuffer::MallocAligned(
    SIZE_T size,
    SIZE_T alignment)
{
    VOID* pMem = NULL;

    if (TRUE == IsPowerOfTwo(alignment))
    {
#ifdef _MSC_VER
        pMem = _aligned_malloc(size, alignment);
#else
        memalign(alignment, size);
#endif
    }

    return pMem;
}

int CamxBuffer::getStride() const
{
    return mStride;
}

int CamxBuffer::getHeight() const
{
    return mHeight;
}

int CamxBuffer::getWidth() const
{
    return mWidth;
}

int CamxBuffer::FillBuffer(char *inputBufferPtr)
{
    char *bufferData = (reinterpret_cast<char *>(*reinterpret_cast<intptr_t *>(bufferHandle->data)));
    memcpy(bufferData, inputBufferPtr, bufferSize);

    return 0;
}

size_t CamxBuffer::getBufferSize() const
{
    return bufferSize;
}

// function to calculate buffer size required based on image format, image width and height

SIZE_T CamxBuffer::CalculateBufferSize(const ImageFormat* pFormat)
{
    SIZE_T bufferSize = 0;

    if (NULL != pFormat)
    {
        UINT numberOfPlanes = GetNumberOfPlanes(pFormat);

        for (UINT planeIndex = 0; planeIndex < numberOfPlanes; planeIndex++)
        {
            bufferSize += GetPlaneSize(pFormat, planeIndex);
        }
    }

    return bufferSize;
}

SIZE_T CamxBuffer::GetPlaneSize(const ImageFormat* pFormat,
    UINT planeIndex)
{
    SIZE_T planeSize = 0;

    if (NULL != pFormat)
    {
        if (TRUE == IsYUV(pFormat))
        {
            if (planeIndex < GetNumberOfPlanes(pFormat))
            {
                const YUVFormat* pYUVFormat = &(pFormat->formatParams.yuvFormat[planeIndex]);
                planeSize = pYUVFormat->planeStride * pYUVFormat->sliceHeight;
            }
        }
        else if (TRUE == IsUBWC(pFormat))
        {
            planeSize = pFormat->formatParams.yuvFormat[planeIndex].metadataSize +
                pFormat->formatParams.yuvFormat[planeIndex].pixelPlaneSize;
        }
        else if (TRUE == IsRAW(pFormat))
        {
            if (0 == planeIndex)
            {
                planeSize = GetRawSize(pFormat);
            }
        }
        else if (Format::Jpeg == pFormat->format)
        {
            // @todo (CAMX-308) Abstract JPEG buffer size assumptions
            // For JPEG make the assumption that worst case will be 12 bpp. In general data size will be much less.
            planeSize = 12 / 8 * pFormat->width * pFormat->height;
        }
        else if (Format::Blob == pFormat->format)
        {
            planeSize = pFormat->width * pFormat->height;
        }
        else if (Format::PD10 == pFormat->format)
        {
            planeSize = pFormat->formatParams.yuvFormat[0].planeStride * pFormat->formatParams.yuvFormat[0].sliceHeight;
        }
        else
        {
            NT_LOG_ERROR("Invalid format");
        }

        // Pad the plane size for the required alignment if not 0. 0 plane size means there was an error with the format
        // definition and we should return 0.
        if (0 != planeSize)
        {
            planeSize = PadToSize(planeSize, pFormat->alignment);
        }
    }

    return planeSize;
}

SIZE_T CamxBuffer::GetRawSize(const ImageFormat* pFormat)
{
    SIZE_T planeSize = 0;

    const RawFormat* pRawFormat = &(pFormat->formatParams.rawFormat);

    switch (pFormat->format)
    {
    case Format::RawYUV8BIT:
        planeSize = (pRawFormat->bitsPerPixel) / 8 * pRawFormat->stride * pRawFormat->sliceHeight;
        break;
    case Format::RawPrivate:
        if (8 == pRawFormat->bitsPerPixel)
        {
            // 1 byte per pixel (8 bytes per 8 pixels)
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight;
        }
        else if (10 == pRawFormat->bitsPerPixel)
        {
            // 8 bytes per 6 pixels
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight * 8 / 6;
        }
        else if (12 == pRawFormat->bitsPerPixel)
        {
            // 8 bytes per 5 pixels
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight * 8 / 5;
        }
        else if (14 == pRawFormat->bitsPerPixel)
        {
            // 8 bytes per 4 pixels
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight * 8 / 4;
        }
        else
        {
            NT_LOG_ERROR("Invalid bpp");
        }
        break;
    case Format::RawMIPI:
        if (8 == pRawFormat->bitsPerPixel)
        {
            // 1 byte per 1 pixel
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight;
        }
        else if (10 == pRawFormat->bitsPerPixel)
        {
            // 5 bytes per 4 pixels
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight * 5 / 4;
        }
        else if (12 == pRawFormat->bitsPerPixel)
        {
            // 3 bytes per 2 pixels
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight * 3 / 2;
        }
        else if (14 == pRawFormat->bitsPerPixel)
        {
            // 7 bytes per 4 pixels
            planeSize = pRawFormat->stride * pRawFormat->sliceHeight * 7 / 4;
        }
        else
        {
            NT_LOG_ERROR("Invalid bpp");
        }
        break;
    case Format::RawPlain16:
        planeSize = pRawFormat->stride * pRawFormat->sliceHeight;
        break;
    case Format::RawMeta8BIT:
        planeSize = pRawFormat->stride * pRawFormat->sliceHeight;
        break;
    default:
        NT_LOG_ERROR("Invalid RAW format");
        break;
    }

    return planeSize;
}


void CamxBuffer::InitializeFormatParams(ImageFormat* pImageFormat)
{

    switch (pImageFormat->format)
    {
        case Format::RawPlain16:
        {
            pImageFormat->formatParams.rawFormat.bitsPerPixel = 16;
            pImageFormat->formatParams.rawFormat.stride = mWidth * 2;
            pImageFormat->formatParams.rawFormat.sliceHeight = mHeight;
            break;
        }
        case Format::RawMIPI:
            {
            pImageFormat->formatParams.rawFormat.bitsPerPixel = 10;
            // will cause dimension issue
            pImageFormat->formatParams.rawFormat.stride = pImageFormat->width * 5 / 4;
            pImageFormat->formatParams.rawFormat.sliceHeight = pImageFormat->height;
            pImageFormat->formatParams.rawFormat.colorFilterPattern = ColorFilterPattern::RGGB;
            pImageFormat->alignment = 4;
            break;
            }
        case Format::YUV420NV12:
        case Format::YUV420NV21:
        case Format::UBWCTP10:
        case Format::UBWCNV12:
        case Format::UBWCNV124R:
            {
            pImageFormat->formatParams.yuvFormat[0].width = mWidth;
            pImageFormat->formatParams.yuvFormat[0].height = mHeight;
            pImageFormat->formatParams.yuvFormat[0].planeStride = mWidth;
            pImageFormat->formatParams.yuvFormat[0].sliceHeight = mHeight;
            pImageFormat->formatParams.yuvFormat[1].width = mWidth;
            pImageFormat->formatParams.yuvFormat[1].height = mHeight >> 1;
            pImageFormat->formatParams.yuvFormat[1].planeStride = mWidth;
            pImageFormat->formatParams.yuvFormat[1].sliceHeight = mHeight >> 1;
            break;
            }

        case Format::Blob:
        {
            pImageFormat->formatParams.rawFormat.bitsPerPixel = 8;
            pImageFormat->formatParams.rawFormat.stride = pImageFormat->width;
            pImageFormat->formatParams.rawFormat.sliceHeight = pImageFormat->height;
            break;
        }
        default:
        {
            NT_LOG_ERROR("Unexpected buffer format :%d", pImageFormat->format);
            break;
        }
    }

    if (TRUE == IsUBWC(pImageFormat))
    {
        // 4kB alignment for UBWC formats
        pImageFormat->alignment = 4096;
        SetupUBWCPlanes(pImageFormat);
    }
    else
    {
        // 16B/128bits alignement for linear formats
        pImageFormat->alignment = 16;
    }
}

Format CamxBuffer::GetCamXFormatFromSettings()
{
    FILE* overrideSettingsTextFile = fopen(sOverrideSettings.c_str(), "r");
    char fileLine[KSTRING_LEN];
    Format bufferFormat = Format::YUV420NV12;
    const char* searchString = "outputFormat";
    char *key;
    char *value;
    char* val = NULL;
    if(overrideSettingsTextFile)
    {
        while (fgets(fileLine, KSTRING_LEN, overrideSettingsTextFile) != nullptr)
        {
#if defined(_MSC_VER)
            key = strtok_s(fileLine, "=", &val);
            value = strtok_s(NULL, "=", &val);
#else
            key = strtok_r(fileLine, "=", &val);
            value = strtok_r(NULL, "=", &val);
#endif
            if (key == NULL || value == NULL)
            {
                continue;
            }
            if (strcmp(key, searchString) == 0)
            {
                if (ImplementationDefinedTypes.find(value) != ImplementationDefinedTypes.end())
                {
                    NT_LOG_INFO("OVERRIDE: output buffer format :%d", ImplementationDefinedTypes[value]);
                    switch (ImplementationDefinedTypes[value])
                    {
                    case OutputFormatYUV420NV12:
                        bufferFormat = Format::YUV420NV12;
                        break;
                    case OutputFormatUBWCNV12:
                        bufferFormat = Format::UBWCNV12;
                        break;
                    case OutputFormatUBWCTP10:
                        bufferFormat = Format::UBWCTP10;
                        break;
                    default:
                        bufferFormat = Format::YUV420NV12;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        NT_LOG_WARN("Either overrides file does not exist or unable to open it");
    }
    return bufferFormat;
}

unsigned CamxBuffer::GetNumberOfPlanes(const ImageFormat* pFormat)
{
    UINT numberOfPlanes = 0;

    if (NULL != pFormat)
    {
        switch (pFormat->format)
        {
        case Format::RawYUV8BIT:
        case Format::RawPrivate:
        case Format::RawMIPI:
        case Format::RawPlain16:
        case Format::RawMeta8BIT:
        case Format::Jpeg:
        case Format::Blob:
        case Format::Y8:
        case Format::Y16:
        case Format::PD10:
            numberOfPlanes = 1;
            break;
        case Format::YUV420NV12:
        case Format::YUV420NV21:
        case Format::YUV422NV16:
        case Format::YUV420NV12TP10:
        case Format::YUV420NV21TP10:
        case Format::YUV422NV16TP10:
            numberOfPlanes = 2;
            break;
            // Listing UBWC formats separate, for clarity
        case Format::UBWCTP10:
        case Format::UBWCNV12:
        case Format::UBWCNV124R:
            numberOfPlanes = 2;
            break;
        default:
            NT_LOG_ERROR("Invalid Format");
            break;
        }
    }

    return numberOfPlanes;
}

void CamxBuffer::SetupUBWCPlanes(ImageFormat* pFormat)
{
    UINT   indexUBWC = 0;
    BOOL   validUBWCFormat = TRUE;
    Format UBWCFormat = pFormat->format;

    switch (UBWCFormat)
    {
    case Format::UBWCTP10:
        indexUBWC = 0;
        break;
    case Format::UBWCNV124R:
        indexUBWC = 1;
        break;
    case Format::UBWCNV12:
        indexUBWC = 2;
        break;
    default:
        validUBWCFormat = FALSE;
        NT_LOG_ERROR("Invalid UBWC format");
        break;
    }

    if (TRUE == validUBWCFormat)
    {
        UINT numberOfPlanes = GetNumberOfPlanes(pFormat);

        for (UINT planeIndex = 0; planeIndex < numberOfPlanes; planeIndex++)
        {
            YUVFormat* pPlane = &pFormat->formatParams.yuvFormat[planeIndex];

            UINT localWidth = AlignGeneric32(pPlane->width, SupportedUBWCTileInfo[indexUBWC].widthPixels);

            pPlane->width = (localWidth / SupportedUBWCTileInfo[indexUBWC].BPPDenominator) *
                SupportedUBWCTileInfo[indexUBWC].BPPNumerator;


            FLOAT local1 = static_cast<FLOAT>((static_cast<FLOAT>(pPlane->width) /
                SupportedUBWCTileInfo[indexUBWC].widthBytes)) / 64;
            pPlane->metadataStride = static_cast<UINT32>(ceil(local1)) * 1024;

            FLOAT local2 = static_cast<FLOAT>((static_cast<FLOAT>(pPlane->height) /
                SupportedUBWCTileInfo[indexUBWC].height)) / 16;
            UINT  localMetaSize = static_cast<UINT32>(ceil(local2)) * pPlane->metadataStride;
            pPlane->metadataSize = ByteAlign32(localMetaSize, 4096);
            pPlane->metadataHeight = pPlane->metadataSize / pPlane->metadataStride;

            pPlane->planeStride = ByteAlign32(pPlane->width, SupportedUBWCTileInfo[indexUBWC].widthMacroTile);

            pPlane->sliceHeight = ByteAlign32(pPlane->height, SupportedUBWCTileInfo[indexUBWC].heightMacroTile);

            pPlane->pixelPlaneSize = ByteAlign32((pPlane->planeStride * pPlane->sliceHeight), 4096);
        }
    }
}

bool CamxBuffer::IsYUV(const ImageFormat* pFormat)
{
    bool isYUV = false;

    if (NULL != pFormat)
    {
        switch (pFormat->format)
        {
        case Format::Y8:
        case Format::Y16:
        case Format::YUV420NV12:
        case Format::YUV420NV21:
        case Format::YUV422NV16:
        case Format::YUV420NV12TP10:
        case Format::YUV420NV21TP10:
        case Format::YUV422NV16TP10:
            isYUV = true;
            break;
        default:
            isYUV = false;
            break;
        }
    }

    return isYUV;
}

bool CamxBuffer::IsRAW(const ImageFormat* pFormat)
{
    bool isRAW = false;

    if (NULL != pFormat)
    {
        switch (pFormat->format)
        {
        case Format::RawYUV8BIT:
        case Format::RawPrivate:
        case Format::RawMIPI:
        case Format::RawPlain16:
        case Format::RawMeta8BIT:
            isRAW = true;
            break;
        default:
            isRAW = false;
            break;
        }
    }

    return isRAW;
}

bool CamxBuffer::IsUBWC(const ImageFormat* pFormat)
{
    bool isUBWC = false;

    if (NULL != pFormat)
    {
        switch (pFormat->format)
        {
        case Format::UBWCTP10:
        case Format::UBWCNV12:
        case Format::UBWCNV124R:
            isUBWC = true;
            break;
        default:
            isUBWC = false;
            break;
        }
    }

    return isUBWC;
}
}
