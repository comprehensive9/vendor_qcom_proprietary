//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "nativehalx.h"

namespace haltests
{
    NativeHalX::NativeHalX()
    {}

    NativeHalX::~NativeHalX()
    {}

    void NativeHalX::Setup()
    {
        HalTestCase::Setup();
        // session check
        NT_ASSERT((HalJsonParser::sSessionMode >= CAMERA3_VENDOR_STREAM_CONFIGURATION_MODE_START) ||
            inRange(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, CAMERA3_STREAM_CONFIGURATION_CONSTRAINED_HIGH_SPEED_MODE,
            HalJsonParser::sSessionMode), "Invalid Session Mode");

        // Request Type check
        NT_ASSERT(static_cast<int>(HalJsonParser::sRequestTypes.size()) > 0, "Request Type input not provided");
        NT_ASSERT(inRange(0, CAMERA3_TEMPLATE_MANUAL, HalJsonParser::sRequestTypes[0]) ||
            HalJsonParser::sRequestTypes[0] >= CAMERA3_VENDOR_TEMPLATE_START, "Request Type not in valid range");

        // number of streams check
        NT_ASSERT(HalJsonParser::sNumStreams >= 1, "Number of streams should be atleast 1");

        // Formats check
        NT_ASSERT(static_cast<int>(HalJsonParser::sFormats.size()) == HalJsonParser::sNumStreams,
            "Output format required for %d streams. Given for %d streams", HalJsonParser::sNumStreams,
            HalJsonParser::sFormats.size());

        // Resolutions check
        NT_ASSERT(static_cast<int>(HalJsonParser::sResolutions.size()) == HalJsonParser::sNumStreams,
            "Output resolutions required for %d streams. Provided: %d", HalJsonParser::sNumStreams,
            HalJsonParser::sResolutions.size());

        // Directions check
        NT_ASSERT(static_cast<int>(HalJsonParser::sDirections.size()) == HalJsonParser::sNumStreams,
            "Output directions required for %d streams. Provided: %d", HalJsonParser::sNumStreams,
            HalJsonParser::sDirections.size());

        // Stream Interval check
        NT_ASSERT(static_cast<int>(HalJsonParser::sStreamIntervals.size()) == HalJsonParser::sNumStreams,
            "Stream Interval required for %d streams. Provided: %d", HalJsonParser::sNumStreams,
            HalJsonParser::sStreamIntervals.size());

        // atlease one of the streamInterval should be equal to numFrames
        bool isAtleastOneStreamPresentAcrossAllFrames = false;
        for (int numSnap : HalJsonParser::sStreamIntervals)
        {
            NT_ASSERT(numSnap <= HalJsonParser::sFrames,
                "Invalid stream interval %d. It should be less than or equal to %d", numSnap, HalJsonParser::sFrames);
            if (numSnap == HalJsonParser::sFrames)
            {
                isAtleastOneStreamPresentAcrossAllFrames = true;
            }
        }
        NT_ASSERT(isAtleastOneStreamPresentAcrossAllFrames,
            "Atleast one of the stream interval should be equal to %d", HalJsonParser::sFrames);

        if (HalJsonParser::sZSL)
        {
            NT_ASSERT(std::find(HalJsonParser::sRequestTypes.begin(), HalJsonParser::sRequestTypes.end(),
                CAMERA3_TEMPLATE_STILL_CAPTURE) != HalJsonParser::sRequestTypes.end(),
                "ZSL is enabled, but STILL CAPTURE is not set in request types");
        }

        if (HalJsonParser::sExpPattern.size() > 0 || HalJsonParser::sRandExpPattern)
        {
            NT_ASSERT(HalJsonParser::sSHDR || HalJsonParser::sAutoHDR || HalJsonParser::sMFHDR || HalJsonParser::sQHDR,
                "Exposure Pattern provided without HDR enablement");
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// NativeHalX::OmniTest
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void NativeHalX::OmniTest()
    {
        std::vector<Size> resolutionList;
        //regex for matching width and height
        std::regex resRegex("^(\\d+)x(\\d+)$");
        for (auto& iter: HalJsonParser::sResolutions) {
            std::smatch match;
            regex_search(iter, match, resRegex);
            NT_ASSERT(match.size() == 3,
                "Invalid resolution %s. Width and height should be seperated by x. Example: 1920x1080", iter.c_str());
            resolutionList.push_back(Size(stoi(match[1]), stoi(match[2])));
        }

        int numStreams = HalJsonParser::sNumStreams;
        std::vector<int> templateType  = HalJsonParser::sRequestTypes;
        std::vector<int> usageFlagList = HalJsonParser::sUsageFlags;
        std::vector<int> dataspaceList = HalJsonParser::sDataspaces;
        std::vector<int> formatList    = HalJsonParser::sFormats;
        std::vector<camera3_stream_type_t> directions = HalJsonParser::sDirections;

        for (int camId : HalTestCase::GetCameraList())
        {
            //Get the actual camera ID to test
            mCurrentTestCamId = camId;
            NT_LOG_INFO("Testing Camera %d", mCurrentTestCamId);

            camera_info info = mCamInfoMap[mCurrentTestCamId];
            bool matched = true;

            for (int index = 0; index < numStreams; index++)
            {
                if (!CheckSupportedResolution(mCurrentTestCamId, &info, formatList[index], resolutionList[index]))
                {
                    NT_LOG_UNSUPP("Given format: %d and resolution: %dx%d not supported for cameraId : [%d]",
                        formatList[index], resolutionList[index].width, resolutionList[index].height, mCurrentTestCamId);
                    matched = false;
                    break;
                }
            }

            if (!matched)
            {
                continue;
            }

            //Request for templateType stream on non interval frames
            std::vector<Cam3Device::streamRequestType> configStreams{{}};

            //Request for templateType stream on interval frames
            //| requestType|format|skipFrame|sendFrame|intervalType|numStreams|fileName|enableZSL|dumpBuffer|
            configStreams.at(0).numStreams     = numStreams;
            configStreams.at(0).requestType    = templateType[0];
            configStreams.at(0).formatList     = formatList;
            configStreams.at(0).skipFrame      = -1;
            configStreams.at(0).sendFrame      = -1;
            configStreams.at(0).intervalType   = NHX;
            configStreams.at(0).enableZSL      = HalJsonParser::sZSL;

            InitCam3DeviceForCamera(mCurrentTestCamId);

            UpdateMetadataPerFrame(configStreams.at(0).setMap);

            OpenCamera(mCurrentTestCamId);

            //Create a stream for every output
            for (int index = 0; index < numStreams; index++)
            {
                int usageFlag = static_cast<int>(usageFlagList.size()) >= numStreams ? usageFlagList[index] : 0;
                int dataspace = static_cast<int>(dataspaceList.size()) >= numStreams ? dataspaceList[index] : -1;

                configStreams.at(0).dumpBufferList.push_back(true);
                configStreams.at(0).fileNameList.push_back(ConstructOutputFileName(index+1, formatList[index],
                    usageFlag, dataspace, resolutionList[index], "OmniTest"));

                //TODO: stream rotation configurable
                HalTestCase::CreateStream(mCurrentTestCamId, directions[index], resolutionList[index], usageFlag,
                    formatList[index], CAMERA3_STREAM_ROTATION_0, dataspace);
            }

            // TODO: this is actually session creation, need to change the name once NHX is complete
            NT_ASSERT(HalTestCase::ConfigStream(mCurrentTestCamId, HalJsonParser::sSessionMode) == 0, "config streams failed");

            SendContinuousPCR(mCurrentTestCamId, configStreams, 0);

            HalTestCase::FlushStream(mCurrentTestCamId);
            HalTestCase::ClearOutStreams(mCurrentTestCamId);
            HalTestCase::CloseCamera(mCurrentTestCamId);
        }
    }

}
