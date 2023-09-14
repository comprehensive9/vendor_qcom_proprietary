//******************************************************************************
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __IFE_UNIT_TEST__
#define __IFE_UNIT_TEST__

#include "camera3stream.h"

namespace haltests
{
    class IfeUnitTest : public Camera3Stream
    {

    public:
        IfeUnitTest();
        ~IfeUnitTest();

        void TestIFEOutputPorts(Size resolutions[],
            int captureCount, std::string port[], int numPorts);

    protected:
        virtual void Setup();

    private:
        //Test input configuration
        std::vector<int> v_formats;
        std::vector<int>v_streamTypes;
        std::vector<std::string> v_fileNames;
        std::vector<uint32_t> v_usageFlags;

        enum ifeOutputPorts {
        IFEOutputPortFull = 0,
        IFEOutputPortDS4 = 1,
        IFEOutputPortDS16 = 2,
        IFEOutputPortCAMIFRaw = 3,
        IFEOutputPortLSCRaw = 4,
        IFEOutputPortGTMRaw = 5,
        IFEOutputPortFD = 6,
        IFEOutputPortPDAF = 7,
        IFEOutputPortRDI0 = 8,
        IFEOutputPortRDI1 = 9,
        IFEOutputPortRDI2 = 10,
        IFEOutputPortRDI3 = 11,
        IFEOutputPortStatsRS = 12,
        IFEOutputPortStatsCS = 13,
        IFEOutputPortStatsIHIST = 15,
        IFEOutputPortStatsBHIST = 16,
        IFEOutputPortStatsHDRBE = 17,
        IFEOutputPortStatsHDRBHIST = 18,
        IFEOutputPortStatsTLBG = 19,
        IFEOutputPortStatsBF = 20,
        IFEOutputPortStatsAWBBG = 21,
        };

        //map linking input string ports to ifeoutputport
        std::map<std::string, int> mapStrPort =
        {
            {"full", IFEOutputPortFull},
            {"ds4", IFEOutputPortDS4},
            {"ds16", IFEOutputPortDS16},
            {"fd", IFEOutputPortFD},
            {"lsc", IFEOutputPortLSCRaw},
            {"camif", IFEOutputPortCAMIFRaw},
            {"gtm", IFEOutputPortGTMRaw},
            {"statshdrbe", IFEOutputPortStatsHDRBE},
            {"statsawbbg", IFEOutputPortStatsAWBBG},
            {"statsbf", IFEOutputPortStatsBF},
            {"rdi0", IFEOutputPortRDI0},
            {"rdi1", IFEOutputPortRDI1},
            {"rdi2", IFEOutputPortRDI2},
            {"pdaf", IFEOutputPortPDAF},
        };

        int ResolveOutputPortFormat(std::string port);
        // Test basic capture with multiple buffers
        void TestMultiStreamCapture(int cameraId,
                                    std::vector<int>& formats,
                                    std::vector<uint32_t>& usageFlags,
                                    std::vector<int>& streamTypes,
                                    Size resolutions[],
                                    int captureCount);
        void DumpAllBuffers(int cameraId, int frameNumber);
        //std::vector<std::string> filePrefix;
    };


}




#endif  //#ifndef __IFE_UNIT_TEST__