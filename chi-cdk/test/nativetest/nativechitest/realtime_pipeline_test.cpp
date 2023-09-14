////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  realtime_pipeline_test.cpp
/// @brief Definitions for the realtime test suite.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "realtime_pipeline_test.h"

namespace chitests
{
    /***************************************************************************************************************************
    *   RealtimePipelineTest::SetupStreams()
    *
    *   @brief
    *       Overridden function implementation which defines the stream information based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if stream objects could be created or failure
    ***************************************************************************************************************************/
    CDKResult RealtimePipelineTest::SetupStreams()
    {
        CDKResult result = CDKResultSuccess;
        int streamIndex = 0;

        switch (m_testId)
        {
            case TestIPEFull:
            case TestPreviewHVX:
                m_numStreams = 1;

                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD: m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = PREVIEW;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFEFull:
                m_numStreams = 1;

                //1. IFE full stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IFEOutputPortFull;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFEFullDualFoV:
                m_numStreams = 2;

                // IFE Full Stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD_1440 : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IFEOutputPortFull;

                streamIndex++;

                // IFE Full Display Streams
                m_resolution[streamIndex] = m_userDispSize.isInvalidSize() ? FULLHD_1440 : m_userDispSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IFEOutputPortDisplayFull;

                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFEFull3A:
                if (CmdLineParser::RemoveSensor())
                {
                    NT_LOG_UNSUPP("3A included pipelines cannot be run in testgen mode");
                    return CDKResultEUnsupported;
                }
                m_numStreams = 1;

                //1. IFE full stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD_1440 : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IFEOutputPortFull;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFERDI0RawOpaque:
            case TestIFERDI0RawPlain16:
            case TestIFERDI0Raw10:
                m_numStreams = 1;
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }

                //1. IFE RDI stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize()? RDI4608_2592: m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                if(m_testId == TestIFERDI0RawOpaque)
                {
                    m_format[streamIndex] = ChiStreamFormatRawOpaque;
                }
                else if(m_testId == TestIFERDI0RawPlain16)
                {
                    m_format[streamIndex] = ChiStreamFormatRaw16;
                }
                else if(m_testId == TestIFERDI0Raw10)
                {
                    m_format[streamIndex] = ChiStreamFormatRaw10;
                }

                m_streamId[streamIndex]   = IFEOutputPortRDI0;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFERawCamif:
                m_numStreams = 1;
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }

                //1. IFE CAMIF stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? RDI4608_2592 : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_streamId[streamIndex]   = IFEOutputPortCAMIFRaw;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFERawLsc:
                m_numStreams = 1;
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }

                //1. IFE LSC stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? RDI4608_2592 : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_streamId[streamIndex]   = IFEOutputPortLSCRaw;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIFEFullStats:
                m_numStreams = 10;

                // Stat node resolution sizes were taken from calculations in camxtitan17xdefs.h
                //1. IFE Full stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IFEOutputPortFull;
                streamIndex++;
                //2. IFE RS
                m_resolution[streamIndex] = Size(32768, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsRS;
                streamIndex++;

                if (m_socId == 457)
                {
                    m_resolution[streamIndex] = Size(1348, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsBFW;
                    streamIndex++;
                }
                else
                {
                    //3. IFE CS
                    m_resolution[streamIndex] = Size(12480, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsCS;
                    streamIndex++;
                }

                //4. IFE IHIST
                m_resolution[streamIndex] = Size(2048, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsIHIST;
                streamIndex++;

                if (m_socId == 457)
                {
                    //5. IFE BHIST
                    m_resolution[streamIndex] = Size(16384, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsAECBHIST;
                    streamIndex++;

                    //6. IFE AECBE
                    m_resolution[streamIndex] = Size(327680, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsAECBE;
                    streamIndex++;
                    //7. IFE GTMBHIST
                    m_resolution[streamIndex] = Size(16384, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsGTMBHIST;
                    streamIndex++;
                }
                else
                {

                    //5. IFE BHIST
                    m_resolution[streamIndex] = Size(16384, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsBHIST;
                    streamIndex++;

                    //6. IFE HDRBE
                    m_resolution[streamIndex] = Size(147456, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsHDRBE;
                    streamIndex++;
                    //7. IFE HDRBHIST
                    m_resolution[streamIndex] = Size(3072, 1);
                    m_direction[streamIndex]  = ChiStreamTypeOutput;
                    m_format[streamIndex]     = ChiStreamFormatBlob;
                    m_streamId[streamIndex]   = IFEOutputPortStatsHDRBHIST;
                    streamIndex++;
                }

                //8. IFE TLBG
                m_resolution[streamIndex] = Size(147456, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsTLBG;
                streamIndex++;
                //9. IFE BF
                m_resolution[streamIndex] = Size(5768, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsBF;
                streamIndex++;
                //10. IFE AWBBG
                m_resolution[streamIndex] = Size(691200, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = IFEOutputPortStatsAWBBG;

                m_streamInfo.isJpeg        = false;
                m_streamInfo.num_streams   = m_numStreams;
                break;

            case TestRealtimePrevRDI:
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }

                m_numStreams = 2;

                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                streamIndex++;

                //2. RDI stream
                if (m_isCamIFTestGen) {
                    m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? FULLHD : m_userRDISize;
                }
                else {
                    m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? UHD : m_userRDISize;
                }
                m_direction[streamIndex]  = ChiStreamTypeBidirectional;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_streamId[streamIndex]   = IFEOutputPortRDI0;

                m_streamInfo.num_streams = m_numStreams;
                break;
            case TestRealtimePrevWithCallback:
            case TestRealtimePrevWith3AWrappers:
                m_numStreams = 2;

                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCNV12;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                streamIndex++;

                //2. Preview callback stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = PREVIEWCALLBACK;

                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestPreviewWithMemcpy:
            case TestPreviewFromMemcpy:
            case TestBypassNodeMemcpy:
                m_numStreams = 1;

                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = PREVIEW;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestIPE3A:
                m_numStreams = 1;

                //1. preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatUBWCNV12;
                m_streamId[streamIndex]   = IPEOutputPortDisplay;
                streamIndex++;

                m_streamInfo.num_streams = m_numStreams;
                break;

            case TestTFEFullStats:
                m_numStreams = 6;
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }
                // Stat node resolution sizes were taken from calculations in camxtitan5xdefs.h
                //1. TFE Full stream
                m_resolution[streamIndex] = (m_userRDISize.isInvalidSize()) ? FULLHD : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatRaw16;
                m_streamId[streamIndex]   = TFEOutputPortFull;
                streamIndex++;

                //2. TFE AECBG
                m_resolution[streamIndex] = Size(73728, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = TFEOutputPortStatsAECBG;
                streamIndex++;

                //3. TFE BHIST
                m_resolution[streamIndex] = Size(3072, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = TFEOutputPortStatsBHIST;
                streamIndex++;

                //4. TFE TLBG
                m_resolution[streamIndex] = Size(16384, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = TFEOutputPortStatsTLBG;
                streamIndex++;

                //5. TFE BAF
                m_resolution[streamIndex] = Size(16384, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = TFEOutputPortStatsBAF;
                streamIndex++;

                //6. TFE AWBBG
                m_resolution[streamIndex] = Size(36864, 1);
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = TFEOutputPortStatsAWBBG;
                streamIndex++;

                m_streamInfo.isJpeg       = false;
                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestTFERDI0RawOpaque:
            case TestTFERDI0Raw10:
            case TestTFERDI0RawPlain16:
            case TestTFERDI1RawMIPI:
            case TestTFERDI1RawPlain16:

                m_numStreams = 1;
                //1. TFE RDI stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize()? RDI4608_2592: m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;

                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                    m_resolution[streamIndex] = m_userRDISize.isInvalidSize()? FULLHD : m_userRDISize;
                }

                if (m_testId == TestTFERDI0RawOpaque){
                    m_format[streamIndex]     = ChiStreamFormatRawOpaque;
                    m_streamId[streamIndex]   = TFEOutputPortRDI0;
                }
                else if (m_testId == TestTFERDI0RawPlain16 ){
                    m_format[streamIndex]     = ChiStreamFormatRaw16;
                    m_streamId[streamIndex]   = TFEOutputPortRDI0;
                }
                else if (m_testId == TestTFERDI1RawPlain16){
                    m_format[streamIndex]     = ChiStreamFormatRaw16;
                    m_streamId[streamIndex]   = TFEOutputPortRDI1;
                }
                else if (m_testId == TestTFERDI1RawMIPI){
                    m_format[streamIndex]     = ChiStreamFormatRawOpaque;
                    m_streamId[streamIndex]   = TFEOutputPortRDI1;
                }
                else if (m_testId == TestTFERDI0Raw10){
                    m_format[streamIndex]     = ChiStreamFormatRaw10;
                    m_streamId[streamIndex]   = TFEOutputPortRDI0;
                }
                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestTFEFull:
                m_numStreams = 1;
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }

                //1. TFE full stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? FULLHD : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatRawOpaque;
                m_streamId[streamIndex]   = TFEOutputPortFull;

                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestPreviewWithMemcpyTFEOPE:
            case TestPreviewFromMemcpyTFEOPE:
            case TestBypassNodeMemcpyTFEOPE:
                m_numStreams = 1;

                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = PREVIEW;
                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestCVPDME:
            case TestCVPDMEICA:
            case TestCVPDMEDisplayPort:
                m_numStreams = 1;
                m_resolution[streamIndex] = (m_userPrvSize.isInvalidSize()) ? HD4K : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = (m_testId == TestCVPDMEDisplayPort)? IFEOutputPortDisplayFull : IFEOutputPortFull;

                m_streamInfo.num_streams  = m_numStreams;
                break;

            case TestPrevRDITFEOPE:
                m_numStreams = 2;
                if (CmdLineParser::RemoveSensor())
                {
                    m_isCamIFTestGen = true;
                }

                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = OPEOutputPortDisplay;
                streamIndex++;

                //2. RDI stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? RDI4608_2592 : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatRawOpaque;
                m_streamId[streamIndex]   = TFEOutputPortRDI0;

                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestOPEFull:
                m_numStreams = 1;
                if (CmdLineParser::RemoveSensor())
                {
                    m_streamId[streamIndex]   = OPEOutputPortDisplay;
                }
                else {
                    m_streamId[streamIndex]   = PREVIEW;
                }
                //1. Preview stream
                m_resolution[streamIndex] = m_userPrvSize.isInvalidSize() ? FULLHD : m_userPrvSize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestTFEFull3A:
                if (CmdLineParser::RemoveSensor())
                {
                    NT_LOG_UNSUPP("3A included pipelines cannot be run in testgen mode");
                    return CDKResultEUnsupported;
                }
                m_numStreams = 1;

                 //1. TFE full stream
                m_resolution[streamIndex] = m_userRDISize.isInvalidSize() ? FULLHD : m_userRDISize;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatRawOpaque;
                m_streamId[streamIndex]   = TFEOutputPortFull;

                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestIFEIPEJpeg:
            case TestIFEBPSIPEJpeg:

                m_numStreams = 1;
                m_resolution[streamIndex] = UHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = SNAPSHOT;
                m_isRealtime[streamIndex] = true;

                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestIFEFD:
                m_numStreams = 1;

                m_resolution[streamIndex] = VGA;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatBlob;
                m_streamId[streamIndex]   = FDManagerOutputPort;
                m_isRealtime[streamIndex] = true;
                m_streamInfo.isJpeg       = false;

                m_streamInfo.num_streams  = m_numStreams;
                break;
            case TestIFEChiDepthNode:

                m_numStreams = 1;

                //1. Depth stream
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatY16;
                m_streamId[streamIndex]   = PREVIEW;
                m_isRealtime[streamIndex] = true;

                m_streamInfo.isJpeg = false;
                m_streamInfo.num_streams = m_numStreams;
                break;
            case TestIFEInplaceNode:

                m_numStreams = 1;

                //1. Preview stream
                m_resolution[streamIndex] = FULLHD;
                m_direction[streamIndex]  = ChiStreamTypeOutput;
                m_format[streamIndex]     = ChiStreamFormatYCbCr420_888;
                m_streamId[streamIndex]   = PREVIEW;
                m_isRealtime[streamIndex] = true;
                m_streamInfo.num_streams  = m_numStreams;
                break;

            default:
                NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
                result = CDKResultENoSuch;
                break;
        }

        if (result == CDKResultSuccess)
        {
            if (m_streamInfo.num_streams <= 0)
            {
                NT_LOG_ERROR("Number of streams should be greater than 0, given: %d", m_streamInfo.num_streams);
                result = CDKResultEFailed;
            }
            else
            {
                m_pRequiredStreams = SAFE_NEW() CHISTREAM[m_streamInfo.num_streams];
                if (NULL == m_pRequiredStreams)
                {
                    NT_LOG_ERROR("Failed to initialize CHI streams");
                    result = CDKResultEFailed;
                }
                else
                {
                    camera3_stream_t* pCamStreams = CreateStreams(m_streamInfo);
                    if (nullptr == pCamStreams)
                    {
                        NT_LOG_ERROR("Failed to created streams");
                        result = CDKResultEFailed;
                    }
                    else
                    {
                        ConvertCamera3StreamToChiStream(pCamStreams, m_pRequiredStreams, m_streamInfo.num_streams);
                        delete[] pCamStreams;   // No longer needed, converted to m_pRequiredStreams

                        if (m_pRequiredStreams == nullptr)
                        {
                            NT_LOG_ERROR("Provided streams could not be configured for testcase Id: [%d]", m_testId);
                            result = CDKResultEFailed;
                        }
                        else
                        {
                            for (int index = 0; index < m_numStreams; index++)
                            {
                                m_isRealtime[index] = true;
                                m_streamIdMap[m_streamInfo.streamIds[index]] = &m_pRequiredStreams[index];
                            }
                            if (m_streamIdMap.size() != static_cast<size_t>(m_numStreams))
                            {
                                result = CDKResultEFailed;
                            }
                        }
                    }
                }
            }
        }

        return result;
    }

    /***************************************************************************************************************************
    *   RealtimePipelineTest::SetupPipelines()
    *
    *   @brief
    *       Overridden function implementation which creates pipeline objects based on the test Id
    *   @param
    *       [in]  int                 cameraId    cameraId to be associated with the pipeline
    *       [in]  ChiSensorModeInfo*  sensorMode  sensor mode to be used for the pipeline
    *   @return
    *       CDKResult success if pipelines could be created or failure
    ***************************************************************************************************************************/
    CDKResult RealtimePipelineTest::SetupPipelines(int cameraId, ChiSensorModeInfo * sensorMode)
    {
        CDKResult result = CDKResultSuccess;
        switch (m_testId)
        {
            case TestIPEFull:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealTimeIPE);
                break;
            case TestIFEFull:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFE);
                break;
            case TestIFEFullDualFoV:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEDualFoV);
                break;
            case TestIFEFull3A:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFE3A);
                break;
            case TestIFERDI0RawOpaque:
            case TestIFERDI0RawPlain16:
            case TestIFERDI0Raw10:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFERDI0);
                break;
            case TestIFERawCamif:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFERawCamif);
                break;
            case TestIFERawLsc:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFERawLsc);
                break;
            case TestIFEFullStats:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIFEStats);
                break;
            case TestRealtimePrevRDI:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevRDI);
                break;
            case TestRealtimePrevWithCallback:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevWithCallbackWithStats);
                break;
            case TestRealtimePrevWith3AWrappers:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevWith3AWrappers);
                break;
            case TestPreviewWithMemcpy:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevWithMemcpy);
                break;
            case TestPreviewFromMemcpy:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevFromMemcpy);
                break;
            case TestBypassNodeMemcpy:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeBypassNode);
                break;
            case TestPreviewHVX:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevHVX);
                break;
            case TestIPE3A:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIPE3A);
                break;
            case TestTFEFullStats:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFEStats);
                break;
            case TestTFEFull:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFE);
                break;
            case TestTFEFull3A:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFE3A);
                break;
            case TestPreviewWithMemcpyTFEOPE:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevWithMemcpyTFEOPE);
                break;
            case TestPreviewFromMemcpyTFEOPE:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevFromMemcpyTFEOPE);
                break;
            case TestBypassNodeMemcpyTFEOPE:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeBypassNodeTFEOPE);
                break;
            case TestCVPDME:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeCVPDME);
                break;
            case TestCVPDMEDisplayPort:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeCVPDMEDisplayPort);
                break;
            case TestCVPDMEICA:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeCVPDMEICA);
                break;
            case TestTFERDI0RawOpaque:
            case TestTFERDI0Raw10:
            case TestTFERDI0RawPlain16:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFERDI0);
                break;
            case TestTFERDI1RawPlain16:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFERDI1RawPlain16);
                break;
            case TestTFERDI1RawMIPI:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeTFERDI1RawMIPI);
                break;
            case TestPrevRDITFEOPE:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimePrevRDITFEOPE);
                break;
            case TestOPEFull:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeOPE);
                break;
            case TestIFEIPEJpeg:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeIPEJpeg);
                break;
            case TestIFEBPSIPEJpeg:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeBPSIPEJpeg);
                break;
            case TestIFEFD:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeFD);
                break;
            case TestIFEChiDepthNode:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeChiDepth);
                break;
            case TestIFEInplaceNode:
                m_pChiPipeline[Realtime] = ChiPipeline::Create(cameraId, sensorMode, m_streamIdMap, PipelineType::RealtimeInplaceNode);
                break;
            default:
                NT_LOG_ERROR( "Unknown testcase Id provided: [%d]", m_testId);
                return CDKResultENoSuch;
        }

        result = (m_pChiPipeline[Realtime] != nullptr) ? m_pChiPipeline[Realtime]->CreatePipelineDesc() : CDKResultEInvalidPointer;
        return result;
    }

    /***************************************************************************************************************************
    *   RealtimePipelineTest::CreateSessions()
    *
    *   @brief
    *       Overridden function implementation which creates required number of sessions based on the test Id
    *   @param
    *       None
    *   @return
    *       CDKResult success if sessions could be created or failure
    ***************************************************************************************************************************/
    CDKResult RealtimePipelineTest::CreateSessions()
    {
        CDKResult result = CDKResultSuccess;
        ChiCallBacks callbacks = { 0 };

        callbacks.ChiNotify                      = ChiTestCase::ProcessMessage;
        callbacks.ChiProcessCaptureResult        = ChiTestCase::QueueCaptureResult;
        callbacks.ChiProcessPartialCaptureResult = ChiTestCase::QueuePartialCaptureResult;

        mPerSessionPvtData[RealtimeSession].pTestInstance = this;
        mPerSessionPvtData[RealtimeSession].sessionId     = RealtimeSession;
        mPerSessionPvtData[RealtimeSession].cameraId      = m_currentTestCameraId;

        mPerSessionPvtData[RealtimeSession].pChiSession   = ChiSession::Create(&m_pChiPipeline[Realtime], 1, &callbacks,
                                                            &mPerSessionPvtData[RealtimeSession]);
        mPerSessionPvtData[RealtimeSession].sessionHandle = mPerSessionPvtData[RealtimeSession].pChiSession->GetSessionHandle();

        if(mPerSessionPvtData[RealtimeSession].pChiSession == nullptr)
        {
            NT_LOG_ERROR( "Realtime session could not be created");
            result = CDKResultEFailed;
        }
        else
        {
            if(!m_nEarlyPCR) // if early PCR not enabled, activate pipeline here
            {
                result = m_pChiPipeline[Realtime]->ActivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
            }

            if (result == CDKResultSuccess)
            {
                result = m_pChiMetadataUtil->GetPipelineinfo(m_pChiModule->GetContext(),
                    mPerSessionPvtData[RealtimeSession].sessionHandle,
                    m_pChiPipeline[Realtime]->GetPipelineHandle(), &m_pPipelineMetadataInfo[Realtime]);
            }

        }
        return result;
    }

    /***************************************************************************************************************************
    *   RealtimePipelineTest::DestroyResources()
    *
    *   @brief
    *       Overridden function implementation which destroys all created resources / objects
    *   @param
    *       None
    *   @return
    *       None
        ***********************************************************************************************************************/
    void RealtimePipelineTest::DestroyResources()
    {
        if ((nullptr != mPerSessionPvtData[RealtimeSession].pChiSession) && (nullptr != m_pChiPipeline[Realtime]))
        {
            m_pChiPipeline[Realtime]->DeactivatePipeline(mPerSessionPvtData[RealtimeSession].sessionHandle);
        }

        if (mPerSessionPvtData[RealtimeSession].pChiSession != nullptr)
        {
            mPerSessionPvtData[RealtimeSession].pChiSession->DestroySession();
            mPerSessionPvtData[RealtimeSession].pChiSession = nullptr;
        }

        if (m_pChiPipeline[Realtime] != nullptr)
        {
            m_pChiPipeline[Realtime]->DestroyPipeline();
            m_pChiPipeline[Realtime] = nullptr;
        }

        DestroyCommonResources();
    }

    /**************************************************************************************************
    *   RealtimePipelineTest::TestRealtimePipeline
    *
    *   @brief
    *       Test a realtime pipeline involving Sensor+IFE or Sensor+IFE+IPE depending on pipelineType
    *   @param
    *       [in] TestType   testType            Test identifier
    *       [in] bool       bUseExternalSensor  Set if test uses external sensor (defaults to false)
    *   @return
    *       None
    **************************************************************************************************/
    void RealtimePipelineTest::TestRealtimePipeline(TestId testType, bool bUseExternalSensor)
    {
        m_testId = testType;
        Setup();

        // Get list of cameras to test
        std::vector <int> camList = m_pChiModule->GetCameraList();
        m_pipelineConfig = RealtimeConfig;

        // Only test camera 0 if using external sensor (camera 1 and 2 not supported with external sensor)
        if (bUseExternalSensor)
        {
            camList.clear();
            camList.push_back(0);
        }

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            m_currentTestCameraId = camList[camCounter];

            if(CmdLineParser::isTestGenMode())
            {
                // Query static capabilities for testgen
                NT_ASSERT(CDKResultSuccess == QueryStaticCapabilities(MetadataUsecases::Testgen, m_currentTestCameraId),
                                                "Could not query testgen capabilities for cameraID %d", m_currentTestCameraId);
            }

            // For tests with ChiDepthNode, check that device can support depth capability
            if (m_testId == TestIFEChiDepthNode)
            {
                bool bDepthSupported = m_pChiMetadataUtil->CheckAvailableCapability(m_currentTestCameraId, ANDROID_REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT);
                if (!bDepthSupported)
                {
                    NT_LOG_UNSUPP( "Depth not supported on camera %d, skipping...", m_currentTestCameraId);
                    continue;
                }
            }
            //1. Create chistream objects from streamcreatedata
            NT_ASSERT(SetupStreams() == CDKResultSuccess, "Fatal failure at SetupStreams() !");

            /* Set session level parameters
            if(CmdLineParser::isTestGenMode())
            {
                NT_ASSERT(CDKResultSuccess == SetSessionParameters(MetadataUsecases::Testgen, m_currentTestCameraId, m_streamInfo.maxRes),
                                                "Could not set session level parameters for cameraID %d", m_currentTestCameraId);
            }
            */

            ChiSensorModeInfo* testSensorMode = nullptr;
            testSensorMode = m_pChiModule->GetClosestSensorMode(m_currentTestCameraId, m_streamInfo.maxRes);

            NT_ASSERT(testSensorMode != nullptr, "Sensor mode is NULL for cameraId: %d", m_currentTestCameraId);

            NT_LOG_INFO("Camera %d chosen sensor mode: %u, width: %u, height: %u, framerate: %u", m_currentTestCameraId,
                testSensorMode->modeIndex, testSensorMode->frameDimension.width, testSensorMode->frameDimension.height, testSensorMode->frameRate);

            // TODO: Set number of batched frames based on usecase
            testSensorMode->batchedFrames = 1;

            //For RDI cases, we need to make sure stream resolution matches sensor mode
            for (int streamIndex = 0; streamIndex < m_numStreams; streamIndex++)
            {
                if ((IsRDIStream(m_pRequiredStreams[streamIndex].format)) &&
                    (!IsSensorModeMaxResRawUnsupportedTFEPort(m_streamId[streamIndex])))
                {
                    if (!m_userRDISize.isInvalidSize())
                    {
                        NT_LOG_INFO("OVERRIDE: RDI resolution set by User width: %d, height: %d ",
                            m_userRDISize.width, m_userRDISize.height);
                        m_pRequiredStreams[streamIndex].width = m_userRDISize.width;
                        m_pRequiredStreams[streamIndex].height = m_userRDISize.height;
                    }
                    else if (!m_isCamIFTestGen)
                    {
                        NT_LOG_INFO("Selected RDI resolution based on Sensor mode width: %d, height: %d ",
                            testSensorMode->frameDimension.width, testSensorMode->frameDimension.height);
                        m_pRequiredStreams[streamIndex].width = testSensorMode->frameDimension.width;
                        m_pRequiredStreams[streamIndex].height = testSensorMode->frameDimension.height;
                    }
                }
            }

            //2. Create pipelines
            NT_ASSERT(SetupPipelines(m_currentTestCameraId, testSensorMode) == CDKResultSuccess, "Pipelines Setup failed");

            //3. Create sessions
            NT_ASSERT(CreateSessions() == CDKResultSuccess, "Necessary sessions could not be created");

            //4. Initialize buffer manager and metabuffer pools
            NT_ASSERT(InitializeBufferManagers(m_currentTestCameraId) == CDKResultSuccess,
                "Buffer managers could not be initialized");

            NT_ASSERT(m_pChiMetadataUtil->CreateInputMetabufferPool(m_currentTestCameraId, MAX_REQUESTS) == CDKResultSuccess,
                "Input metabuffer pool could not be created!");
            NT_ASSERT(m_pChiMetadataUtil->CreateOutputMetabufferPool(MAX_REQUESTS) == CDKResultSuccess,
                "Output metabuffer pool could not be created!");

            //5. Create and submit capture requests
            for (UINT32 frameNumber = START_FRAME; frameNumber <= m_captureCount; frameNumber++)
            {
                NT_ASSERT(GenerateRealtimeCaptureRequest(frameNumber) == CDKResultSuccess,
                    "Submit pipeline request failed for frameNumber: %d", frameNumber);
            }

            //6. Wait for all results
            NT_ASSERT(WaitForResults() == CDKResultSuccess,
                "There are pending buffers not returned by driver after several retries");

            //7. Destroy all resources created
            DestroyResources();
        }
    }

}
