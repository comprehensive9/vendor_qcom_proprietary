//******************************************************************************
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA3_BUFFER__
#define __CAMERA3_BUFFER__

#include "camera3common.h"

namespace haltests {

/// Enumeration of the color filter pattern for RAW outputs
enum class ColorFilterPattern
{
    Y,      ///< Monochrome pixel pattern.
    YUYV,   ///< YUYV pixel pattern.
    YVYU,   ///< YVYU pixel pattern.
    UYVY,   ///< UYVY pixel pattern.
    VYUY,   ///< VYUY pixel pattern.
    RGGB,   ///< RGGB pixel pattern.
    GRBG,   ///< GRBG pixel pattern.
    GBRG,   ///< GBRG pixel pattern.
    BGGR,   ///< BGGR pixel pattern.
    RGB     ///< RGB pixel pattern.
};

struct RawFormat
{
    uint32_t              bitsPerPixel;       ///< Bits per pixel.
    uint32_t              stride;             ///< Stride in bytes.
    uint32_t              sliceHeight;        ///< The number of lines in the plane which can be equal to or larger than actual
                                              ///  frame height.
    ColorFilterPattern   colorFilterPattern;  ///< Color filter pattern of the RAW format.
};

struct YUVFormat
{
    uint32_t width;       ///< Width of the YUV plane in pixels.
                        ///  Tile aligned width in bytes for UBWC
    uint32_t height;      ///< Height of the YUV plane in pixels.
    uint32_t planeStride; ///< The number of bytes between the first byte of two sequential lines on plane 1. It may be
                        ///  greater than nWidth * nDepth / 8 if the line includes padding.
                        ///  Macro-tile width aligned for UBWC
    uint32_t sliceHeight; ///< The number of lines in the plane which can be equal to or larger than actual frame height.
                        ///  Tile height aligned for UBWC

    UINT32 metadataStride;  ///< Aligned meta data plane stride in bytes, used for UBWC formats
    UINT32 metadataHeight;  ///< Aligned meta data plane height in bytes, used for UBWC formats
    UINT32 metadataSize;    ///< Aligned metadata plane size in bytes, used for UBWC formats
    UINT32 pixelPlaneSize;  ///< Aligned pixel plane size in bytes, calculated once for UBWC formats
                            ///< and stored thereafter, since the calculations are expensive
};

/// Maximum number of planes for all formats.
static const size_t FormatsMaxPlanes = 3;

union FormatParams
{
    YUVFormat yuvFormat[FormatsMaxPlanes];  ///< YUV format specific properties.
    RawFormat rawFormat;                    ///< RAW format specific properties.
};

/// @brief This enumerates pixel formats.
enum class Format
{
    Jpeg,           ///< JPEG format.
    Y8,             ///< Luma only, 8 bits per pixel.
    Y16,            ///< Luma only, 16 bits per pixel.
    YUV420NV12,     ///< YUV 420 format as described by the NV12 fourcc.
    YUV420NV21,     ///< YUV 420 format as described by the NV21 fourcc.
    YUV422NV16,     ///< YUV 422 format as described by the NV16 fourcc
    Blob,           ///< Any non image data
    RawYUV8BIT,     ///< Packed YUV/YVU raw format. 16 bpp: 8 bits Y and 8 bits UV. U and V are interleaved as YUYV or YVYV.
    RawPrivate,     ///< Private RAW formats where data is packed into 64bit word.
                    ///  8BPP:  64-bit word contains 8 pixels p0-p7, p0 is stored at LSB.
                    ///  10BPP: 64-bit word contains 6 pixels p0-p5, most significant 4 bits are set to 0. P0 is stored at LSB.
                    ///  12BPP: 64-bit word contains 5 pixels p0-p4, most significant 4 bits are set to 0. P0 is stored at LSB.
                    ///  14BPP: 64-bit word contains 4 pixels p0-p3, most significant 8 bits are set to 0. P0 is stored at LSB.
    RawMIPI,        ///< MIPI RAW formats based on MIPI CSI-2 specification.
                    ///  8BPP: Each pixel occupies one bytes, starting at LSB. Output width of image has no restrictions.
                    ///  10BPP: 4 pixels are held in every 5 bytes. The output width of image must be a multiple of 4 pixels.
                    ///  12BPP: 2 pixels are held in every 3 bytes. The output width of image must be a multiple of 2 pixels.
                    ///  14BPP: 4 pixels are held in every 7 bytes. The output width of image must be a multiple of 4 pixels.
    RawPlain16,     ///< Plain16 RAW format. Single pixel is packed into two bytes, little endian format. Not all bits may be
                    ///  used as RAW data is generally 8, 10, or 12 bits per pixel. Lower order bits are filled first.
    RawMeta8BIT,    ///< Generic 8-bit raw meta data for internal camera usage.
    UBWCTP10,       ///< UBWC TP10 format (as per UBWC2.0 design specification)
    UBWCNV12,       ///< UBWC NV12 format (as per UBWC2.0 design specification)
    UBWCNV124R,     ///< UBWC NV12-4R format (as per UBWC2.0 design specification)
    PD10,
    YUV420NV12TP10,
    YUV420NV21TP10,
    YUV422NV16TP10
};

/// Enumerates implementation defined format types
enum OutputFormatType
{
    OutputFormatYUV420NV12,                 ///< Linear NV12 format
    OutputFormatUBWCNV12,                   ///< UBWC NV12 format
    OutputFormatUBWCTP10,                   ///< UBWC TP10 format
};

/// @brief UBWC tile info structure for a UBWC format
struct UBWCTileInfo
{
    UINT widthPixels;       ///< Tile width in pixels
    UINT widthBytes;        ///< Tile width in pixels
    UINT height;            ///< Tile height
    UINT widthMacroTile;    ///< Macro tile width
    UINT heightMacroTile;   ///< Macro tile height
    UINT BPPNumerator;      ///< Bytes per pixel (numerator)
    UINT BPPDenominator;    ///< Bytes per pixel (denominator)
};

// Tile info for supported UBWC formats (from HAS)
static const UBWCTileInfo SupportedUBWCTileInfo[] =
{
    { 48, 64, 4, 256, 16, 4, 3 }, // UBWC_TP10-Y

    { 64, 64, 4, 256, 16, 1, 1 }, // UBWC_NV12-4R-Y

    { 32, 32, 8, 128, 32, 1, 1 }, // UBWC_NV12-Y
};


/// @brief This enumerates degrees of rotation in a clockwise direction. The specific variable or struct member must declare the
/// semantics of the rotation (e.g. the image HAS BEEN rotated or MUST BE rotated).
enum class Rotation
{
    CW0Degrees,     ///< Zero degree rotation.
    CW90Degrees,    ///< 90 degree clockwise rotation.
    CW180Degrees,   ///< 180 degree clockwise rotation.
    CW270Degrees    ///< 270 degree clockwise rotation.
};

/// @brief This enumerates color space specifications
enum class ColorSpace
{
    Unknown,    ///< Default-assumption data space, when not explicitly specified.
    BT601Full,  ///< ITU-R Recommendation 601 (BT.601) - Full range.
    BT601625,   ///< ITU-R Recommendation 601 (BT.601) - 625-line  SDTV, 625 Lines (PAL).
    BT601525,   ///< ITU-R Recommendation 601 (BT.601) - 525-line SDTV, 525 Lines (NTSC).
    BT709,      ///< ITU-R Recommendation 709 (BT.709) HDTV.
    Depth       ///< The buffer contains depth ranging measurements from a depth camera per HAL definition.
};

struct ImageFormat
{
    uint32_t            width;         ///< Width of the image in pixels.
    uint32_t            height;        ///< Height of the image in pixels.
    Format              format;        ///< Format of the image.
    ColorSpace          colorSpace;    ///< Color space of the image.
    Rotation            rotation;      ///< Rotation applied to the image.
    FormatParams        formatParams;  ///< Format specific definitions.
    size_t              alignment;     ///< The required alignment of each plane specified in bytes.
};


class CamxBuffer
{

public:
    CamxBuffer();
    CamxBuffer(uint32_t width, uint32_t height, int format);
    ~CamxBuffer();
    buffer_handle_t* GetNativeHandle();
    int FreeBuffer();
    int getStride() const;
    int getHeight() const;
    int getWidth() const;
    int FillBuffer(char* inputBufferPtr);
    size_t getBufferSize() const;

private:
    SIZE_T CalculateBufferSize(const ImageFormat* pFormat);
    SIZE_T GetPlaneSize(const ImageFormat * pFormat, UINT planeIndex);
    static SIZE_T GetRawSize(const ImageFormat* pFormat);
    void InitializeFormatParams(ImageFormat * pImageFormat);
    Format GetCamXFormatFromSettings();
    static bool IsUBWC(const ImageFormat * pImageFormat);
    void SetupUBWCPlanes(ImageFormat * pImageFormat);
    static bool IsYUV(const ImageFormat* pFormat);
    static bool IsRAW(const ImageFormat* pFormat);
    static unsigned GetNumberOfPlanes(const ImageFormat* pFormat);
    static void * MallocAligned(SIZE_T size, SIZE_T alignment);
    native_handle_t* bufferHandle;
    size_t bufferSize;
    int mHeight;
    int mWidth;
    int mStride;
    std::map<std::string, OutputFormatType> ImplementationDefinedTypes =
    {
        { "OutputFormatUBWCTP10", OutputFormatUBWCTP10 },
        { "OutputFormatYUV420NV12", OutputFormatYUV420NV12 },
        { "OutputFormatUBWCNV12", OutputFormatUBWCNV12 }
    };

};

}
#endif  // __CAMERA3_BUFFER_
