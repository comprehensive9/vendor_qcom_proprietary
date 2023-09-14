//*************************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************
#ifndef __CAMERA3_STREAM__
#define __CAMERA3_STREAM__


#include "camera3common.h"
#include <map>


namespace chitests
{
    static const int MAXIMUM_STREAMS = 11;
    /*
     * Enums defining various names for the streams. Instead of hardcoding stream index
     * we can use these enums to identify the streams
     */
    enum StreamUsecases
    {
        PREVIEW = 0,
        SNAPSHOT,
        VIDEO,
        REPROCESS,
        PREVIEWCALLBACK,
        THUMBNAIL,
        CONFIG_INPUT,
        CONFIG_OUTPUT,
        IFEOutputPortFull,
        IFEOutputPortDS4,
        IFEOutputPortDS16,
        IFEOutputPortCAMIFRaw,
        IFEOutputPortLSCRaw,
        IFEOutputPortGTMRaw,
        IFEOutputPortFD,
        IFEOutputPortPDAF,
        IFEOutputPortRDI0,
        IFEOutputPortRDI1,
        IFEOutputPortRDI2,
        IFEOutputPortRDI3,
        IFEOutputPortStatsRS,
        IFEOutputPortStatsCS,
        IFEOutputPortStatsIHIST,
        IFEOutputPortStatsBHIST,
        IFEOutputPortStatsHDRBE,
        IFEOutputPortStatsHDRBHIST,
        IFEOutputPortStatsTLBG,
        IFEOutputPortStatsBF,
        IFEOutputPortStatsAWBBG,
        IFEOutputPortDisplayFull,
        IFEOutputPortDisplayDS4,
        IFEOutputPortDisplayDS16,
        IFEOutputPortStatsDualPD,
        IFEOutputPortRAW0,
        IFEOutputPortRAW1,
        IFEOutputPortRAW2,
        IPEInputDS4BlurMapBlur,
        IPEInputDS16BlurMapBlur,
        BPSInputPort,
        BPSOutputPortFull,
        BPSOutputPortDS4,
        BPSOutputPortDS16,
        BPSOutputPortDS64,
        BPSOutputPortStatsBG,
        BPSOutputPortStatsBhist,
        BPSOutputPortREG1,
        BPSOutputPortREG2,
        IPEInputPortFull,
        IPEInputPortDS4,
        IPEInputPortDS16,
        IPEInputPortDS64,
        IPEInputPortFullRef,
        IPEInputPortDS4Ref,
        IPEInputPortDS16Ref,
        IPEInputPortDS64Ref,
        IPEOutputPortDisplay,
        IPEOutputPortVideo,
        IPEOutputPortFullRef,
        IPEOutputPortDS4Ref,
        IPEOutputPortDS16Ref,
        IPEOutputPortDS64Ref,
        IPEInputPort2Full,
        IPEInputPort2DSX,
        JPEGInputPort,
        JPEGOutputPort,
        JPEGSWInputPortMain,
        JPEGSWOutputPortMain,
        JPEGAggregatorInputPort0,
        JPEGAggregatorInputPort1,
        JPEGAggregatorOutputPort,
        JPEGAggregatorOutputPort0,
        GPUInputPort,
        GPUOutputPortFull,
        GPUOutputPortDS4,
        GPUOutputPortDS16,
        GPUOutputPortDS64,
        FDHwInputPortImage,
        FDManagerInputPort,
        FDManagerOutputPort,
        ChiNodeInputPort0,
        ChiNodeOutputPort0,
        ChiNodeOutputPort1,
        TFEOutputPortFull,
        TFEOutputPortIdealRaw,
        TFEOutputPortPDAF,
        TFEOutputPortRDI0,
        TFEOutputPortRDI1,
        TFEOutputPortRDI2,
        TFEOutputPortStatsAECBG,
        TFEOutputPortStatsBHIST,
        TFEOutputPortStatsTLBG,
        TFEOutputPortStatsBAF,
        TFEOutputPortStatsAWBBG,
        OPEInputPortFull,
        OPEOutputPortVideo,
        OPEOutputPortDisplay,
        OPEOutputPortARGB,
        OPEOutputPortStatsRS,
        OPEOutputPortStatsIHIST,
        OPEOutputPortStatsLTMDC,
        CVPInputPortTARIFEDS4,
        CVPInputPortREFIFEDS4,
        CVPInputPortIFEDS4DELAY,
        IFELITEOutputPortStatsBG,
        IFELITEOutputPortPreProcessRaw,
        IFEOutputPortStatsBFW,
        IFEOutputPortStatsAECBE,
        IFEOutputPortStatsAECBHIST,
        IFEOutputPortStatsGTMBHIST,
        InvalidPort
    };

    struct StreamCreateData
    {
        StreamCreateData() : num_streams(0), num_ipstreams(0), usageFlags(nullptr){}
        int                        num_streams;                                ///< number of streams
        int                        num_ipstreams;                              ///< number of streams
        android_pixel_format_t*    formats;                                    ///< format of streams
        camera3_stream_type_t*     directions;                                 ///< direction of streams
        Size*                      resolutions;                                ///< resolution of streams
        StreamUsecases*            streamIds;                                  ///< names for streams
        uint32_t*                  usageFlags;                                 ///< usage flags for streams
        const char*                inputImages[MAXIMUM_STREAMS] = { nullptr }; ///< filename to be read for input stream
        bool                       isJpeg;                                     ///< toggle between creating jpeg or blob buffer
        bool*                      isRealtime;                                 ///< specifies if stream is intended for RT
        Size                       maxRes;                                     ///< highest resolution out of all the streams
        UINT32                     delayedFrame[MAXIMUM_STREAMS] = { 0 };      ///<delayed frame inputs
    };

    camera3_stream_t* CreateStreams(StreamCreateData& streamInfo);
    Size GetGrallocSize(uint32_t width, uint32_t height, int format, int dataspace, unsigned char* resultBuffer = nullptr);
    uint32_t GetExactJpegBufferSize(unsigned char* resultBuffer);

    //map linking ports to strings
    extern std::map<StreamUsecases, std::string> mapStreamStr;

}
#endif  // __CAMERA3_STREAM__
