//*************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#include <math.h>
#include "camera3stream.h"

namespace chitests
{
    /*
     * Mapping of the different stream usecases with corresponding strings
     */
    std::map<StreamUsecases, std::string> mapStreamStr =
    {
        { PREVIEW,                      "Prv"               },
        { VIDEO,                        "Video"             },
        { REPROCESS,                    "RpInput"           },
        { SNAPSHOT,                     "Snap"              },
        { PREVIEWCALLBACK,              "PrvCallBack"       },
        { THUMBNAIL,                    "Thumbnail"         },
        { CONFIG_INPUT,                 "ConfigInput"       },
        { CONFIG_OUTPUT,                "ConfigOutput"      },
        { IFEOutputPortFull,            "ifefull"           },
        { IFEOutputPortDS4,             "ifeds4"            },
        { IFEOutputPortDS16,            "ifeds16"           },
        { IFEOutputPortFD,              "ifefd"             },
        { IFEOutputPortLSCRaw,          "ifelsc"            },
        { IFEOutputPortCAMIFRaw,        "ifecamif"          },
        { IFEOutputPortGTMRaw,          "ifegtm"            },
        { IFEOutputPortStatsHDRBE,      "ifestatshdrbe"     },
        { IFEOutputPortStatsAWBBG,      "ifestatsawbbg"     },
        { IFEOutputPortStatsBF,         "ifestatsbf"        },
        { IFEOutputPortStatsRS,         "ifestatsrs"        },
        { IFEOutputPortStatsCS,         "ifestatscs"        },
        { IFEOutputPortStatsIHIST,      "ifestatsihist"     },
        { IFEOutputPortStatsTLBG,       "ifestatstlbg"      },
        { IFEOutputPortStatsHDRBHIST,   "ifestatshdrbhist"  },
        { IFEOutputPortStatsBHIST,      "ifestatsbhist"     },
        { IFEOutputPortRDI0,            "iferdi0"           },
        { IFEOutputPortRDI1,            "iferdi1"           },
        { IFEOutputPortRDI2,            "iferdi2"           },
        { IFEOutputPortRAW0,            "iferaw0"           },
        { IFEOutputPortRAW1,            "iferaw1"           },
        { IFEOutputPortRAW2,            "iferaw2"           },
        { IFEOutputPortPDAF,            "ifepdaf"           },
        { IPEOutputPortDisplay,         "Prv"               },
        { IPEOutputPortVideo,           "Video"             },
        { BPSOutputPortFull,            "bpsfull"           },
        { BPSOutputPortDS4,             "bpsds4"            },
        { BPSOutputPortDS16,            "bpsds16"           },
        { BPSOutputPortDS64,            "bpsds64"           },
        { BPSOutputPortStatsBG,         "bpsBG"             },
        { BPSOutputPortStatsBhist,      "bpsHist"           },
        { JPEGInputPort,                "jpegInput"         },
        { JPEGAggregatorOutputPort,     "jpegAgg"           },
        { GPUInputPort,                 "gpuInput"          },
        { GPUOutputPortFull,            "gpufull"           },
        { GPUOutputPortDS4,             "gpuds4"            },
        { GPUOutputPortDS16,            "gpuds16"           },
        { GPUOutputPortDS64,            "gpuds64"           },
        { FDManagerInputPort,           "fdmgrinput"        },
        { FDManagerOutputPort,          "fdmgroutput"       },
        { IFEOutputPortStatsBFW,        "ifestatsbfw"       },
        { IFEOutputPortStatsAECBE,      "ifestatsaecbe"     },
        { IFEOutputPortStatsAECBHIST,   "ifestatsaecbhist"     },
        { IFEOutputPortStatsGTMBHIST,   "ifestatsgtmbhist"  }
    };

    /**************************************************************************************************
    *   CreateStreams
    *
    *   @brief
    *       Generates camera3_stream_t* based on stream info
    *   @return
    *       valid stream pointer on success or nullptr on failure
    **************************************************************************************************/
    camera3_stream_t* CreateStreams(
        StreamCreateData& streamInfo) //[in] info of the streams to be created
    {

    if (streamInfo.num_streams <= 0)
    {
        NT_LOG_ERROR("Number of streams should be greater than 0, given: %d", streamInfo.num_streams);
        return nullptr;
    }

    camera3_stream_t* requiredStreams = SAFE_NEW() camera3_stream_t[streamInfo.num_streams];

    for (int streamIndex = 0; streamIndex < streamInfo.num_streams; streamIndex++)
    {
        requiredStreams[streamIndex].format = streamInfo.formats[streamIndex];
        requiredStreams[streamIndex].width = streamInfo.resolutions[streamIndex].width;
        requiredStreams[streamIndex].height = streamInfo.resolutions[streamIndex].height;
        if (streamInfo.usageFlags == nullptr)
        {
            requiredStreams[streamIndex].usage = 0;
        }
        else
        {
            requiredStreams[streamIndex].usage = streamInfo.usageFlags[streamIndex];
        }
        /*
        * GRALLOC_USAGE_PRIVATE_0 is common only for UBWC related formats, additionally any specific
        * gralloc flags for a given format will already be set in the corresponding test.cpp file.
        * For private formats gralloc flag please clarify from developer.
        */
#ifdef _LINUX
        // Set gralloc flags for special formats
        switch (static_cast<int>(streamInfo.formats[streamIndex]))
        {
        case ChiStreamFormatImplDefined:
            requiredStreams[streamIndex].usage |= GRALLOC_USAGE_PRIVATE_0;
            break;
        case ChiStreamFormatUBWCTP10:
            requiredStreams[streamIndex].usage |= PRIVATE_GRALLOC_USAGE_TP10;
            break;
        case ChiStreamFormatUBWCNV12:
        case ChiStreamFormatUBWCNV124R:
            requiredStreams[streamIndex].usage |= GRALLOC_USAGE_PRIVATE_0;
            break;
        case ChiStreamFormatP010:
            requiredStreams[streamIndex].usage |= PRIVATE_GRALLOC_USAGE_P010;
            break;
        default:
            // No extra gralloc flags needed; do nothing
            break;
        }
        NT_LOG_DEBUG( "After applying gralloc flags for special formats, usage for stream %d is [0x%08x]",
            streamIndex, requiredStreams[streamIndex].usage);
#endif
            /*
             * The below gralloc flags are common for all stream formats but depend on stream direction.
             * To know what each flag corresponds to please refer gralloc.h
             */
#ifdef _LINUX
             // dummy instance of GraphicBuffer, only to get the mapper version
            sp<GraphicBuffer> graphicBufferInstance = SAFE_NEW() GraphicBuffer(
                640,
                480,
                0x00000023, // ChiStreamFormatYCbCr420_888
                0x00000100); // GrallocUsageHwTexture
            GraphicBufferMapper::Version grallocVersion = graphicBufferInstance->getBufferMapperVersion();
#endif
            if(streamInfo.directions[streamIndex] == CAMERA3_STREAM_OUTPUT)
            {
#ifdef _LINUX
                if (grallocVersion == GraphicBufferMapper::Version::GRALLOC_2)
                {
                    NT_LOG_DEBUG("Using gralloc version 2.0");
                    requiredStreams[streamIndex].usage |= GraphicBuffer::USAGE_SW_READ_OFTEN |
                        GraphicBuffer::USAGE_SW_WRITE_NEVER |
                        GRALLOC_USAGE_HW_CAMERA_WRITE |
                        GraphicBuffer::USAGE_HW_MASK;
                }
                else if (grallocVersion == GraphicBufferMapper::Version::GRALLOC_3)
                {
                    NT_LOG_DEBUG("Using gralloc version 3.0");
                    // remove this flag from mask for new gralloc3 changes
                    requiredStreams[streamIndex].usage &= ~GraphicBuffer::USAGE_HW_2D;
                }

#endif
            }
            else if(streamInfo.directions[streamIndex] == CAMERA3_STREAM_INPUT)
            {
#ifdef _LINUX
                if (grallocVersion == GraphicBufferMapper::Version::GRALLOC_2)
                {
                    NT_LOG_DEBUG("Using gralloc version 2.0");
                    requiredStreams[streamIndex].usage |= GraphicBuffer::USAGE_SW_READ_RARELY |
                        GraphicBuffer::USAGE_SW_WRITE_OFTEN |
                        GraphicBuffer::USAGE_HW_MASK;
                }
                else if (grallocVersion == GraphicBufferMapper::Version::GRALLOC_3)
                {
                    NT_LOG_DEBUG("Using gralloc version 3.0");
                    // remove this flag from mask for new gralloc3 changes
                    requiredStreams[streamIndex].usage &= ~GraphicBuffer::USAGE_HW_2D;
                }
#endif
            }
            else if(streamInfo.directions[streamIndex] == CAMERA3_STREAM_BIDIRECTIONAL)
            {
#ifdef _LINUX
                if (grallocVersion == GraphicBufferMapper::Version::GRALLOC_2)
                {
                    NT_LOG_DEBUG("Using gralloc version 2.0");
                    requiredStreams[streamIndex].usage |= GraphicBuffer::USAGE_SW_READ_RARELY |
                        GraphicBuffer::USAGE_SW_WRITE_RARELY |
                        GRALLOC_USAGE_HW_CAMERA_ZSL |
                        GraphicBuffer::USAGE_HW_MASK;
                }
                else if (grallocVersion == GraphicBufferMapper::Version::GRALLOC_3)
                {
                    NT_LOG_DEBUG("Using gralloc version 3.0");
                    // remove this flag from mask for new gralloc3 changes
                    requiredStreams[streamIndex].usage &= ~GraphicBuffer::USAGE_HW_2D;
                }
#endif
        }
        else
        {
            NT_LOG_ERROR("Invalid stream direction given: %d", streamInfo.directions[streamIndex]);
            return nullptr;
        }

        NT_LOG_DEBUG("After applying gralloc flags for stream direction, usage for stream %d is [0x%08x]",
            streamIndex, requiredStreams[streamIndex].usage);

        requiredStreams[streamIndex].stream_type = streamInfo.directions[streamIndex];
        requiredStreams[streamIndex].max_buffers = 0;
        requiredStreams[streamIndex].priv = nullptr;
        requiredStreams[streamIndex].rotation = CAMERA3_STREAM_ROTATION_0;

        switch (static_cast<int>(streamInfo.formats[streamIndex]))  // Cast to int to avoid warnings about diff enum types
        {
            case ChiStreamFormatRaw16:
            case ChiStreamFormatRaw12:
            case ChiStreamFormatRaw10:
            case ChiStreamFormatRawOpaque:
            case ChiStreamFormatRaw64:
            case ChiStreamFormatNV12HEIF:
                requiredStreams[streamIndex].data_space = HAL_DATASPACE_ARBITRARY;
                break;
            case ChiStreamFormatBlob:
                // Height of 1 means it must be a dumped stat --> use arbitrary dataspace instead
                // This will only be the case for tests that dump both jpegs and stats
                if (streamInfo.isJpeg && streamInfo.resolutions[streamIndex].height != 1)
                {
                    requiredStreams[streamIndex].data_space = HAL_DATASPACE_V0_JFIF;
                }
                else
                {
                    NT_LOG_INFO("Non jpeg test, use dataspace HAL_DATASPACE_ARBITRARY");
                    requiredStreams[streamIndex].data_space = HAL_DATASPACE_ARBITRARY;
                }
                break;
            case ChiStreamFormatImplDefined:
            case ChiStreamFormatUBWCTP10:
            case ChiStreamFormatUBWCNV12:
            case ChiStreamFormatUBWCNV124R:
            case ChiStreamFormatP010:
            case ChiStreamFormatPD10:
                requiredStreams[streamIndex].data_space = HAL_DATASPACE_UNKNOWN;
                break;
            case ChiStreamFormatYCbCr420_888:
            case ChiStreamFormatYCrCb420_SP:
                requiredStreams[streamIndex].data_space = HAL_DATASPACE_STANDARD_BT709;
                break;
            case ChiStreamFormatY8:
            case ChiStreamFormatY16:
                requiredStreams[streamIndex].data_space = HAL_DATASPACE_DEPTH;
                break;
            default:
                NT_LOG_ERROR("Invalid stream format: %d", streamInfo.formats[streamIndex]);
                return nullptr;
        }

        if (streamInfo.maxRes < streamInfo.resolutions[streamIndex])
        {
            streamInfo.maxRes = streamInfo.resolutions[streamIndex];
        }
    }

    return requiredStreams;
}

/*******************************************************************
* GetGrallocSize
*
* @brief
*     gralloc dimensions may be different due to requirement of the format
* @return
*     new size object based on the format
*
********************************************************************/
Size GetGrallocSize(
    uint32_t width,
    uint32_t height,
    int format,
    int dataspace,
    unsigned char* resultBuffer)
{
    //Gralloc implementation maps Raw Opaque to BLOB
    //Support upto 12 bpp

        Size newResolution;
        newResolution.height = 1;

    switch (format)
    {
        case ChiStreamFormatBlob:
            if (dataspace == HAL_DATASPACE_V0_JFIF ||
                dataspace == HAL_DATASPACE_JFIF)
            {
                newResolution.width = (nullptr == resultBuffer) ? (width * height * 4) : GetExactJpegBufferSize(resultBuffer);
            }
            else
            {
                newResolution.width = static_cast<uint32_t>(width * height * 2);
            }
            break;
        case ChiStreamFormatRawOpaque:
            newResolution.width = static_cast<uint32_t>(width * height * (3.0 / 2));
            break;
        case ChiStreamFormatRaw10:
            // 4 pixels packed into 5 bytes, width must be multiple of 4 pixels
            newResolution.width = static_cast<uint32_t>(ceil(width / 4.0) * 5 * height);
            break;
        case ChiStreamFormatY8:
            // 1 pixel packed into 1 byte (8 bits), width must be multiple of 2 pixels
            newResolution.width = static_cast<uint32_t>(ceil(width / 2.0) * 2 * height);
            break;
        case ChiStreamFormatY16:
            // Same as Y8, but double the bits per pixel (1 pixel packed into 2 bytes)
            newResolution.width = static_cast<uint32_t>(ceil(width / 2.0) * 4 * height);
            break;
        case ChiStreamFormatPD10:
        case ChiStreamFormatRaw12:
            // This cannot be added to Gralloc Graphic buffer as part of Display/Graphics HAL,
            // as this is our IP format, commiting this to display/GDX HAL will expose PD10 format to
            // open source (caf), so handling it internally. PD10 packing needs 4 times the unknown
            // format in Gralloc Graphics buffer. SW team is discussing internally if this needs to be
            // exposed to HAL, when moving to 3.0 framework, we need to move away from Graphic Buffer
            // to  Chi buffer through flags (Graphic, Native, CamXChi, Internal).
            // TODO: TDEV-2839 - Migrate from Graphics Buffers to CamxChi buffers
            newResolution.width = static_cast<uint32_t>(width * height * 4.0);
            break;
        default:
            newResolution.width = width;
            newResolution.height = height;
            break;
    }

    return newResolution;
}

/*******************************************************************
* GetExactJpegBufferSize
*
* @brief
*     The buffer size after trimming data after end of image
*     bytes 0xFF 0xD9
* @return
*     new size object after trimming
*
********************************************************************/
uint32_t GetExactJpegBufferSize(
    unsigned char* resultBuffer)
{
    uint32_t actualSize = 0;
    bool endOfImage = false;
    unsigned char* pCurrentBuffer = resultBuffer;
    unsigned char* pNextBuffer = resultBuffer;
    while (!endOfImage)
    {
        ++pNextBuffer;
        if (static_cast<uint32_t>(*pCurrentBuffer) == 255) //0xFF
        {
            if (static_cast<uint32_t>(*pNextBuffer) == 217) //0xD9
            {
                endOfImage = true;
            }
        }
        ++actualSize;
        pCurrentBuffer = pNextBuffer;
    }

    return ++actualSize;    // increment by 1 to include the last byte
    }
}
