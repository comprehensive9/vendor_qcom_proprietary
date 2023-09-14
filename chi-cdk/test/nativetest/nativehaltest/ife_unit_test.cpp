//******************************************************************************
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "ife_unit_test.h"

/*
* Use Case:
*     This suite contains unit tests for IFE.
*
* Desc:
*     This test verifies IFE functionality by directing IFE output ports to sink buffers
*     when appropriate topology is used. Currently the following port combinations are tested for IFE
*     IFEOutputPortFull + IFEOutputPortDS4
*     IFEOutputPortFull + IFEOutputPortDS16
*     IFEOutputPortFull + IFEOutputPortFD
*     IFEOutputPortCAMIFRaw
*     IFEOutputPortLSCRaw
*     IFEOutputPortGTMRaw
*
* Testcase:
*     Test                                  Ports                                 Resolution
*     IfeTestFullDs4VGA         IFEOutputPortFull + IFEOutputPortDS4                 VGA
*     IfeTestFullDs16VGA        IFEOutputPortFull + IFEOutputPortDS16                VGA
*     IfeTestFullFdVGA          IFEOutputPortFull + IFEOutputPortFD                  VGA
*     IfeRawTestGtmVGA          IFEOutputPortGTMRaw                                  VGA
*     IfeRawTestCamifVGA        IFEOutputPortCAMIFRaw                                VGA
*     IfeRawTestLscVGA          IFEOutputPortLSCRaw                                  VGA
*     IfeTestStatsHDRBEVGA      IFEOutputPortFull + IFEOutputPortStatsHDRBE          VGA
*     IfeTestStatsAWBBGVGA      IFEOutputPortFull + IFEOutputPortStatsAWBBG          VGA
*     IfeTestRDI0VGA            IFEOutputPortFull + IFEOutputPortRDI0                VGA
*
* Expected Output:
*     Requested number of captures from each port in topology
*
* Error Output:
*     Test fails if the callback does not provide the image buffers
*/
namespace haltests
{

    IfeUnitTest::IfeUnitTest()
    {}

    IfeUnitTest::~IfeUnitTest()
    {}

    /**************************************************************************************************************************
    * IfeUnitTest::TestIFEOutputPorts
    *
    * @brief
    *   Test the ife ouput ports
    * @return
    *   None
    **************************************************************************************************************************/
    void IfeUnitTest::TestIFEOutputPorts(
        Size resolutions[],  //[in] resolution for ife ports
        int captureCount,    //[in] number of images to capture
        std::string port[],  //[in] port ids
        int numPorts)        //[in] number of ports
    {
        std::vector<int> camList = Camera3Module::GetCameraList();

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);

            bool unsupported = false;

            for (int portIndex = 0; portIndex < numPorts; portIndex++)
            {
                int portFormat = ResolveOutputPortFormat(port[portIndex]);
                v_formats.push_back(portFormat);

                //overwrite resolution for RDI port to max sensor size
                if (port[portIndex].compare("rdi0")  == 0  ||
                    port[portIndex].compare("camif") == 0  ||
                    port[portIndex].compare("lsc")   == 0 )
                {
                    resolutions[portIndex] = Camera3Device::GetMaxSize(portFormat, pCamInfo);
                    if(resolutions[portIndex].width == 0 && resolutions[portIndex].height == 0)
                    {
                        NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]", portFormat, ConvertResToString(resolutions[portIndex]).c_str(), cameraId);
                        unsupported = true;
                        break;
                    }
                    NT_LOG_DEBUG("Raw resolution width %d height %d", resolutions[portIndex].width, resolutions[portIndex].height)
                }
                v_usageFlags.push_back(0);
                v_streamTypes.push_back(CAMERA3_STREAM_OUTPUT);
                v_fileNames.push_back("ife-" + port[portIndex]);
            }

            if(unsupported)
            {
                continue;
            }

            // open camera
            Camera3Device::OpenCamera(cameraId);

            TestMultiStreamCapture(cameraId,
                v_formats,
                v_usageFlags,
                v_streamTypes,
                resolutions,
                captureCount
            );
            //Cleanup
            v_formats.clear();
            v_usageFlags.clear();
            v_streamTypes.clear();
            v_fileNames.clear();

            Camera3Stream::FlushStream(cameraId);
            // close streams
            Camera3Stream::ClearOutStreams();
            // close camera
            Camera3Device::CloseCamera(cameraId);
        }

    }

    void IfeUnitTest::Setup()
    {
        Camera3Stream::Setup();
    }

    /**************************************************************************************************************************
    * IfeUnitTest::ResolveOutputPortFormat
    *
    * @brief
    *   Helper function to resolve port to output format
    * @return
    *   format int HAL_PIXEL_FORMAT for output port
    **************************************************************************************************************************/
    int IfeUnitTest::ResolveOutputPortFormat(
        std::string port)  //[in] portid to be resolved
    {
        int format=-1;
        switch (mapStrPort[port])
        {
            case IFEOutputPortFull:
            case IFEOutputPortFD:
                format = HAL_PIXEL_FORMAT_YCbCr_420_888;
                break;
            case IFEOutputPortDS4:
            case IFEOutputPortDS16:
                format = HAL_PIXEL_FORMAT_BLOB;
                break;
            case IFEOutputPortCAMIFRaw:
            case IFEOutputPortGTMRaw:
            case IFEOutputPortLSCRaw:
            case IFEOutputPortStatsHDRBE:
            case IFEOutputPortStatsAWBBG:
            case IFEOutputPortPDAF:
            case IFEOutputPortStatsBF:
                format = HAL_PIXEL_FORMAT_RAW16;
                break;
            case IFEOutputPortRDI0:
            case IFEOutputPortRDI1:
            case IFEOutputPortRDI2:
                format = HAL_PIXEL_FORMAT_RAW10;
                break;
            default:
                NT_LOG_ERROR("Unable to resolve output format for port %s",port.c_str())
                break;
        }
        return format;
    }

    /**************************************************************************************************************************
    * IfeUnitTest::TestMultiStreamCapture
    *
    * @brief
    *   Helper function to execute the IFE unit test for required ports
    * @return
    *   None
    **************************************************************************************************************************/
    void IfeUnitTest::TestMultiStreamCapture(
        int cameraId,                       //[in] camera Id
        std::vector<int>& formats,          //[in] vector of formats for each port
        std::vector<uint32_t>& usageFlags,  //[in] vector of usage flags for each port
        std::vector<int>& streamTypes,      //[in] vector of stream directions for each port
        Size resolutions[],                 //[in] array of stream resolutions for each port
        int captureCount)                   //[in] capture count
    {

        int frameNumber = 0;

        Camera3Stream::CreateMultiStreams(
            resolutions,
            formats.data(),
            static_cast<int>(formats.size()), //number of streams
            usageFlags.data(),
            streamTypes.data());

        NT_ASSERT(Camera3Stream::ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) == 0
            ,"configure streams failed");

        for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
        {
            Camera3Stream::CreateCaptureRequestWithMultBuffers(
                cameraId,
                0,
                CAMERA3_TEMPLATE_STILL_CAPTURE,
                static_cast<int>(formats.size()),
                ++frameNumber); //non fatal
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, false, 0, nullptr),
                "Start Streaming Failed ! ");
            DumpAllBuffers(cameraId, frameNumber);

            Camera3Stream::DeleteHalRequest(cameraId);

        }

    }
    /**************************************************************************************************************************
    * IfeUnitTest::DumpAllBuffers
    *
    * @brief
    *   Helper function to dump all buffers for configured streams
    * @return
    *   None
    **************************************************************************************************************************/
    void IfeUnitTest::DumpAllBuffers(
        int cameraId,     //[in] camera Id
        int frameNumber)  //[in] frameNumber
    {
        for (size_t streamIndex = 0; streamIndex < GetConfigedStreams().size(); streamIndex++)
        {
            int curFormat = GetConfigedStreams().at(streamIndex)->format;
            std::string fileName = ConstructOutputFileName(cameraId, curFormat, frameNumber,
                v_fileNames.at(streamIndex).c_str());
            Camera3Stream::DumpBuffer(streamIndex, frameNumber, fileName.c_str());
        }
    }
}
