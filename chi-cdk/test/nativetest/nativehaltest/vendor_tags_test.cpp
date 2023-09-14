//*************************************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************


#include "vendor_tags_test.h"
namespace haltests
{

    // Initialize static members
    uint32_t * VendorTagsTest::availableTags = nullptr;
    int VendorTagsTest::tagCount = 0;

    VendorTagsTest::VendorTagsTest()
    {
    }

    VendorTagsTest::~VendorTagsTest()
    {
        delete[] availableTags;
        tagCount = 0;
    }

    /***************************************************************************************************
    *   VendorTagsTest::SetUp()
    *
    *   @brief
    *       Sets up the camera module and populates all the vendor tags information
    *       prior to running the tests
    *
    *   @return
    *       void
    ****************************************************************************************************/

    void VendorTagsTest::Setup()
    {
        Camera3Stream::Setup();
        NT_ASSERT(InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");
    }
    /***************************************************************************************************
    *   VendorTagsTest::InitializeVendorTags()
    *
    *   @brief
    *       Populates all the vendor tags information prior to running the tests. The vendorTagOps
    *       parameter allows this function to be static.
    *
    *   @return
    *       int 0 if successful, -1 otherwise
    ****************************************************************************************************/
    int VendorTagsTest::InitializeVendorTags(vendor_tag_ops_t *vendorTagOps)
    {
        if (nullptr == vendorTagOps)
        {
            NT_LOG_ERROR("Vendor tag ops are null!");
            return -1;
        }

        tagCount = vendorTagOps->get_tag_count(vendorTagOps);
        if (-1 == tagCount)
        {
            NT_LOG_ERROR("Vendor tags are not available!");
            return -1;
        }

        NT_LOG_DEBUG("Vendor tag count: %d", VendorTagsTest::tagCount);
        uint32_t* availableTags = NULL;
        availableTags = new uint32_t[VendorTagsTest::tagCount];
        vendorTagOps->get_all_tags(vendorTagOps, availableTags);

        // Keep for debugging purposes
        //for (int k = 0; k < tagCount; k++)
        //{
        //    NT_LOG_DEBUG("Tag number: %u Section name: %s Tag name: %s Tag type: %d",
        //        availableTags[k], vendorTagOps->get_section_name(vendorTagOps, availableTags[k]),
        //        vendorTagOps->get_tag_name(vendorTagOps, availableTags[k]), vendorTagOps->get_tag_type(vendorTagOps, availableTags[k]));
        //}
        return 0;
    }

    /***************************************************************************************************
    *   VendorTagsTest::ResolveAvailableVendorTag()
    *
    *   @brief
    *       Helper function to find the matching vendor tag number given
    *       a vendor tag name and section
    *
    *   @return
    *       uint32_t tag number for a given vendor tag if found
    ****************************************************************************************************/

    uint32_t VendorTagsTest::ResolveAvailableVendorTag(vendor_tag_ops_t *vendorTagOps,
        const char *tagName,
        const char *sectionName)
    {
        if (nullptr == vendorTagOps)
        {
            NT_LOG_ERROR("Vendor tag ops are null!");
            return UINT32_MAX;
        }

        tagCount = vendorTagOps->get_tag_count(vendorTagOps);
        if (-1 == tagCount)
        {
            NT_LOG_ERROR("Vendor tags are not available!");
            return UINT32_MAX;
        }

        NT_LOG_DEBUG("Vendor tag count: %d", VendorTagsTest::tagCount);
        uint32_t* availableTags = NULL;
        availableTags = new uint32_t[VendorTagsTest::tagCount];
        vendorTagOps->get_all_tags(vendorTagOps, availableTags);

        uint32_t tagNumber = UINT32_MAX;

        for (int k = 0; k < tagCount; k++)
        {
            if (strcmp(tagName, vendorTagOps->get_tag_name(vendorTagOps, availableTags[k])) == 0 &&
                strcmp(sectionName, vendorTagOps->get_section_name(vendorTagOps, availableTags[k])) == 0)
            {
                tagNumber = availableTags[k];
                break;
            }
        }
        return tagNumber;
    }

    /***************************************************************************************************
    *   VendorTagsTest::prepare3AConvergedRequest()
    *
    *   @brief
    *       Helper function to perform 3a convergence and obtain the frame number
    *       of the converged frame. The function also creates HAL request with
    *       necessary information
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::prepare3AConvergedRequest(
        int cameraId,
        int format,
        Size resolution,
        bool dumpBuffer,
        int &frameNumber)
    {
        // Create one output stream
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            resolution,
            0,
            format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(Camera3Stream::ConfigStream(false, cameraId)==0, "configure streams failed");

#ifdef ENABLE3A
        frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
            cameraId,
            frameNumber,
            CAMERA3_TEMPLATE_PREVIEW, // requestType,
            0, // output stream index
            false,
            "");

        NT_ASSERT(frameNumber != -1, "3A cannot converge!");
#endif
        camera3_stream_buffer_t* outputBuffer = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(outputBuffer != nullptr, "Failed to get output buffer");

        // create hal request
        if (dumpBuffer)
        {
            InitHalRequest(cameraId, frameNumber++, nullptr, outputBuffer, 1, CAMERA3_TEMPLATE_STILL_CAPTURE);
        }
        else
        {
            InitHalRequest(cameraId, frameNumber++, nullptr, outputBuffer, 1, CAMERA3_TEMPLATE_PREVIEW);
        }

    }

    /***************************************************************************************************
    *   VendorTagsTest::prepareEISSnapshotRequest()
    *
    *   @brief
    *       Helper function to perform 3a convergence and obtain the frame number
    *       of the converged frame. The function also creates HAL request for
    *       an EIS test using the STILL_CAPTURE template
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::prepareEISSnapshotRequest(
        int cameraId,           //[in] camera Id
        VendorTagItem preview,  //[in] preview format and resolution
        VendorTagItem snapshot, //[in] snapshot format and resolution
        int &frameNumber,       //[in/out] frame number
        uint32_t opmode)        //[in] operation mode for configuring streams
    {
        // Create preview and snapshot streams
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            preview.resolution,
            0,
            preview.format,
            CAMERA3_STREAM_ROTATION_0);

        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            snapshot.resolution,
            0,
            snapshot.format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(Camera3Stream::ConfigStream(opmode, cameraId) == 0, "configure streams failed");

#ifdef ENABLE3A
        frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
            cameraId,
            frameNumber,
            CAMERA3_TEMPLATE_PREVIEW, // requestType,
            0, // output stream index
            false,
            "");

        NT_ASSERT(frameNumber != -1, "3A cannot converge!");
#endif
        camera3_stream_buffer_t* outputBuffer = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(outputBuffer != nullptr, "Failed to get output buffer for request!");

        // create hal request
        InitHalRequest(cameraId, frameNumber++, nullptr, outputBuffer, 1, CAMERA3_TEMPLATE_STILL_CAPTURE);

    }

    /***************************************************************************************************
    *   VendorTagsTest::prepareEISVideoRequest()
    *
    *   @brief
    *       Helper function to perform 3a convergence and obtain the frame number
    *       of the converged frame. The function also creates HAL request for
    *       an EIS test using the VIDEO_SNAPSHOT template
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::prepareEISVideoRequest(
        int cameraId,           //[in] camera Id
        VendorTagItem preview,  //[in] preview format and resolution
        VendorTagItem video,    //[in] video format and resolution
        VendorTagItem snapshot, //[in] snapshot format and resolution
        int &frameNumber,       //[in/out] frame number
        uint32_t opmode)        //[in] operation mode for configuring streams
    {
        // Create preview, video, and snapshot streams
        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            preview.resolution,
            GrallocUsageHwCameraWrite | GrallocUsageHwTexture,
            preview.format,
            CAMERA3_STREAM_ROTATION_0);

        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            video.resolution,
            GrallocUsageHwVideoEncoder,
            video.format,
            CAMERA3_STREAM_ROTATION_0);

        Camera3Stream::CreateStream(
            CAMERA3_STREAM_OUTPUT,
            snapshot.resolution,
            0,
            snapshot.format,
            CAMERA3_STREAM_ROTATION_0);

        NT_ASSERT(Camera3Stream::ConfigStream(opmode, cameraId) == 0, "configure streams failed");

#ifdef ENABLE3A
        frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(
            cameraId,
            frameNumber,
            CAMERA3_TEMPLATE_PREVIEW, // requestType,
            0, // output stream index
            false,
            "");

        NT_ASSERT(frameNumber != -1, "3A cannot converge!");
#endif
        camera3_stream_buffer_t* outputBuffer = GetBufferForRequest(0, frameNumber);
        NT_ASSERT(outputBuffer != nullptr, "Failed to get output buffer for request!");

        // create hal request
        InitHalRequest(cameraId, frameNumber++, nullptr, outputBuffer, 1, CAMERA3_TEMPLATE_VIDEO_SNAPSHOT);

    }

    /***************************************************************************************************
    *   VendorTagsTest::verifyPriorityResult()
    *
    *   @brief
    *       This function takes a snapshot and checks if the result metadata has the requested
    *       ISO_PRIORITY/EXPOSURE_PRIORITY value set. It repeats untill the result is correct
    *       or it times out whichever occurs first.
    *
    *   @return
    *       void
    ****************************************************************************************************/

    void VendorTagsTest::verifyPriorityResult(
        int cameraId,
        int &frameNumber,
        const int64_t *value,
        int32_t priority)
    {

        int i = 0;
        const camera_metadata_t* metaFromFile;
        int success = 0;

        while (i <= PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr), "Start Streaming Failed");
            metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);

            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");

            camera_metadata_entry_t entry;
            int ret;
            if (priority)
            {
                ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
                    ANDROID_SENSOR_EXPOSURE_TIME, &entry);

                if (ret == 0 && entry.count > 0)
                {
                    if (std::abs(entry.data.i64[0] - *value) <= static_cast<int64_t>(TOLERANCE * *value))
                    {
                        NT_LOG_INFO("Key in capture result is within the tolerance of the requested value");
                        NT_LOG_INFO("Exposure time set to: %d Expected value is: %d", *entry.data.i64, *value);
                        success = 1;
                        break;
                    }
                }
            }
            else
            {
                ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile),
                    ANDROID_SENSOR_SENSITIVITY, &entry);

                if (ret == 0 && entry.count > 0)
                {
                    if (std::abs(entry.data.i32[0] - *value) <= static_cast<int32_t>(TOLERANCE * *value))
                    {
                        NT_LOG_INFO("Key in capture result is within the tolerance of the requested value");
                        NT_LOG_INFO("ISO set to: %d Expected value is: %d", *entry.data.i32, *value);
                        success = 1;
                        break;
                    }
                }
            }

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
            i++;
        }

        NT_ASSERT(success == 1, "Capture Results does not contain the value set");

    }

    /***************************************************************************************************
    *   VendorTagsTest::TestSaturation()
    *
    *   @brief
    *       Tests the saturation vendor tag by iterating over few possible values
    *       and takes a snapshot for every value set.
    *
    *   @return
    *       void
    ****************************************************************************************************/

    void VendorTagsTest::TestSaturation(
        int format,       //[in] format of the image
        Size resolution)  //[in] resolution of the image
    {
        char tagName[] = "use_saturation";
        char sectionName[] = "org.codeaurora.qcamera3.saturation";

        uint32_t tagId = ResolveAvailableVendorTag(&vTag, tagName, sectionName);

        if (tagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("Vendor tag not available");
            NT_FAIL("");
        }

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        const int32_t satValues[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            if (!CheckSupportedResolution(cameraId, format, resolution))
            {
                NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    format, ConvertResToString(resolution).c_str(), cameraId);
                continue;
            }

            // open camera
            Camera3Device::OpenCamera(cameraId);

            int frameNumber = 0;

            prepare3AConvergedRequest(cameraId, format, resolution, true, frameNumber); // non fatal

            for (int32_t j = 0; j < static_cast<int>(sizeof satValues / sizeof satValues[0]); j++)
            {
                Camera3Stream::SetKey(cameraId, tagId, satValues + j, 1);

                std::ostringstream outputName;
                outputName << sImageRootPath << "Saturation_Cam" << cameraId << "_SatValue" << satValues[j] << ".yuv";

                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");

                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, true, 0, outputName.str().c_str()), "Start Streaming Failed");
            }

            Camera3Stream::FlushStream(cameraId);

            Camera3Stream::DeleteHalRequest(cameraId);

            // clear streams
            Camera3Stream::ClearOutStreams();

            // close camera
            Camera3Device::CloseCamera(cameraId);

        }
    }

    /***************************************************************************************************
    *   VendorTagsTest::TestExposurePriority()
    *
    *   @brief
    *       Tests the Exposure priority vendor tag by iterating over few possible values
    *       and takes a snapshot for every value set. The test also verifies if the result
    *       metadata has the requested exposure time set.
    *
    *   @return
    *       void
    ****************************************************************************************************/

    void VendorTagsTest::TestExposurePriority(
        int format,       //[in] format of the image
        Size resolution)  //[in] resolution of the image
    {
        char priorityTagName[] = "select_priority";
        char tagName[] = "use_iso_exp_priority";
        char sectionName[] = "org.codeaurora.qcamera3.iso_exp_priority";

        uint32_t priorityTagId = ResolveAvailableVendorTag(&vTag, priorityTagName, sectionName);
        if (priorityTagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("select_priority tag not available");
            return;
        }

        uint32_t tagId = ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("Vendor tag not available");
            return;
        }

        int32_t priority = 1;

        const int64_t exposure_time[] = { ONE_SECOND / 10, ONE_SECOND / 20, ONE_SECOND / 30, ONE_SECOND / 50, ONE_SECOND / 60 };

        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            if (!CheckSupportedResolution(cameraId, format, resolution))
            {
                NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    format, ConvertResToString(resolution).c_str(), cameraId);
                continue;
            }

            // open camera
            Camera3Device::OpenCamera(cameraId);


            int frameNumber = 0;

            prepare3AConvergedRequest(cameraId, format, resolution, false, frameNumber); //non fatal

            Camera3Stream::SetKey(cameraId, priorityTagId, &priority, 1);

            for (int j = 0; j < static_cast<int>(sizeof exposure_time / sizeof exposure_time[0]); j++)
            {
                Camera3Stream::SetKey(cameraId, tagId, exposure_time + j, 1);

                verifyPriorityResult(cameraId, frameNumber, exposure_time + j, priority);//non fatal

                std::ostringstream outputName;
                outputName << sImageRootPath << "ExposurePriority_Cam" << cameraId << "_ExposureTime" << ONE_SECOND / exposure_time[j] << ".yuv";

                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, true, 0, outputName.str().c_str()), "Start Streaming Failed");
            }


            Camera3Stream::FlushStream(cameraId);

            Camera3Stream::DeleteHalRequest(cameraId);

            // clear streams
            Camera3Stream::ClearOutStreams();

            // close camera
            Camera3Device::CloseCamera(cameraId);

        }
    }
    /***************************************************************************************************
    *   VendorTagsTest::TestIsoPriority()
    *
    *   @brief
    *       Tests the ISO priority vendor tag by iterating over few possible values
    *       and takes a snapshot for every value set. The test also verifies if the result
    *       metadata has the requested ISO value set.
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestIsoPriority(
        int format,       //[in] format of the image
        Size resolution)  //[in] resolution of the image
    {
        char priorityTagName[] = "select_priority";
        char tagName[] = "use_iso_exp_priority";
        char sectionName[] = "org.codeaurora.qcamera3.iso_exp_priority";

        uint32_t priorityTagId = ResolveAvailableVendorTag(&vTag, priorityTagName, sectionName);
        if (priorityTagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("select_priority tag not available");
            return;
        }

        uint32_t tagId = ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("Vendor tag not available");
            return;
        }

        int32_t priority = 0;
        const int64_t isoValues[] = { 100, 200, 400, 800 };

        //Set corresponding modes for isopriority

        const int64_t isoModes[] = { 2, 3, 4, 5 };
        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            if (!CheckSupportedResolution(cameraId, format, resolution))
            {
                NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    format, ConvertResToString(resolution).c_str(), cameraId);
                continue;
            }

            // open camera
            Camera3Device::OpenCamera(cameraId);

            int frameNumber = 0;

            prepare3AConvergedRequest(cameraId, format, resolution, false, frameNumber);//nonfatal

            Camera3Stream::SetKey(cameraId, priorityTagId, &priority, 1);

            for (int64_t j = 0; j < static_cast<int64_t>(sizeof isoValues / sizeof isoValues[0]); j++)
            {
                Camera3Stream::SetKey(cameraId, tagId, isoModes + j, 1);

                verifyPriorityResult(cameraId, frameNumber, isoValues + j, priority);//non fatal

                std::ostringstream outputName;
                outputName << sImageRootPath << "IsoPriority_Cam" << cameraId << "_IsoValue" << isoValues[j] << ".yuv";

                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, true, 0, outputName.str().c_str()), "Start Streaming Failed");
            }


            Camera3Stream::FlushStream(cameraId);

            Camera3Stream::DeleteHalRequest(cameraId);

            // clear streams
            Camera3Stream::ClearOutStreams();

            // close camera
            Camera3Device::CloseCamera(cameraId);

        }
    }

    /***************************************************************************************************
    *   VendorTagsTest::TestCustomTagId()
    *
    *   @brief
    *       Tests getting tagId for tags defined in Chi override
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestOverrideTagId()
    {
        int result = 0;
        for (size_t i = 0; i < NUM_CUSTOM_TAGS; i++)
        {
            uint32_t tagId = ResolveAvailableVendorTag(&vTag, customTags[i].tagName, customTags[i].sectionName);
            if (tagId == UINT32_MAX)
            {
                NT_LOG_ERROR("Could not get tagId for tagname %s ", customTags[i].tagName);
                result = -1;
            }
        }
        NT_ASSERT(result == 0, "Could not get tagId for all customTags");
    }

    /***************************************************************************************************
    *   VendorTagsTest::TestOverrideTagAppRead()
    *
    *   @brief
    *       Tests chi override module can read/write metadata and compares values written from app
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestOverrideTagAppRead()
    {
        // The logic for get_camera_info in custom Chi override libcom.bots.chi.override.so is written so that it is able to
        // write data into the tag. After successful read of the written data, it returns success
        // or if there is an error in read/write, get_camera_info call will return failure

        camera_info info;
        int32_t  expectData0 = 0xFFFF0000;                        // value expected to be written by override module for tag1
        uint8_t  expectData1 = 0xFF;                              // value expected to be written by override module for tag2
        uint64_t expectData2 = 0xFFFFFFFF00000000;                // value expected to be written by oveeride module for tag3
        uint64_t expectData3[2] = { expectData2, expectData2 };   // array of values expected to be written by override module
                                                                  // for tag4

        int val = GetCamModule()->get_camera_info(0, &info);

        NT_ASSERT(val == 0, "Read/write operation failed in override module");

        //If write was successful from chi override, we should be able to read the written data here

        uint32_t tagId = ResolveAvailableVendorTag(&vTag, customTags[0].tagName, customTags[0].sectionName);
        NT_ASSERT(tagId != UINT32_MAX, "Could not resolve tagId for tagname %s", customTags[0].tagName);

        camera_metadata_entry_t entry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics),tagId, &entry);
        NT_ASSERT(ret == 0, "Could not find metadata entry %d", tagId);
        NT_ASSERT(entry.data.i32[0] == expectData0
        ,"Read value does not match expected for tagname %s", customTags[0].tagName);

        tagId = ResolveAvailableVendorTag(&vTag, customTags[1].tagName, customTags[1].sectionName);
        NT_ASSERT(tagId != UINT32_MAX, "Could not resolve tagId for tagname %s", customTags[1].tagName);

        ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics),tagId, &entry);
        NT_ASSERT(ret == 0, "Could not find metadata entry %d", tagId);
        NT_ASSERT(entry.data.u8[0] == expectData1
        ,"Read value does not match expected for tagname %s", customTags[1].tagName);

        tagId = ResolveAvailableVendorTag(&vTag, customTags[2].tagName, customTags[2].sectionName);
        NT_ASSERT(tagId != UINT32_MAX, "Could not resolve tagId for tagname %s", customTags[2].tagName);

        ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics),tagId, &entry);
        NT_ASSERT(ret == 0, "Could not find metadata entry %d", tagId);
        NT_ASSERT(static_cast<uint64_t>(entry.data.i64[0]) == expectData2
        ,"Read value does not match expected for tagname %s",customTags[2].tagName);

        tagId = ResolveAvailableVendorTag(&vTag, customTags[3].tagName, customTags[3].sectionName);
        NT_ASSERT(tagId != UINT32_MAX, "Could not resolve tagId for tagname %s", customTags[3].tagName);
        ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics), tagId, &entry);
        NT_ASSERT(ret == 0, "Could not find metadata entry %d", tagId);

        NT_ASSERT(static_cast<uint64_t>(entry.data.i64[0]) == expectData3[0] &&
                    static_cast<uint64_t>(entry.data.i64[1]) == expectData3[1]
        , "Read value does not match expected for tagname %s", customTags[3].tagName);

    }

    /***************************************************************************************************
    *   VendorTagsTest::TestOverrideTagReadWrite()
    *
    *   @brief
    *       Tests chi override module can read/write metadata
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestOverrideTagReadWrite() const
    {
        // The logic for get_camera_info in custom Chi override libcom.bots.chi.override.so is written so that
        // it is able to write data into the tag. After successful read of the written data, it returns success,
        // if there is an error in read/write, get_camera_info call will return failure

        camera_info info;
        int val = GetCamModule()->get_camera_info(0, &info);

        NT_ASSERT(val == 0, "Read/write operation failed in override module");
    }

    /***************************************************************************************************
    *   VendorTagsTest::TestOverrideTagType()
    *
    *   @brief
    *       Tests tag types defined in override matches expected values
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestOverrideTagType()
    {
        camera_info info;
        int val = GetCamModule()->get_camera_info(0, &info);

        NT_ASSERT(val == 0, "Read/write operation failed in override module");

        for (size_t tagIndex = 0; tagIndex < NUM_CUSTOM_TAGS; tagIndex++)
        {
            uint32_t tagId = ResolveAvailableVendorTag(&vTag, customTags[tagIndex].tagName, customTags[tagIndex].sectionName);
            NT_ASSERT(tagId != UINT32_MAX, "Could not resolve tagId for tagname %s", customTags[0].tagName);

            camera_metadata_entry_t entry;
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                tagId, &entry);
            NT_ASSERT(ret == 0, "Could not find metadata entry %d", tagId);

            NT_ASSERT(entry.type == customTags[tagIndex].tagType
            ,"Type does not match expectation for tagname %s", customTags[tagIndex].tagName);
        }
    }

    /***************************************************************************************************
    *   VendorTagsTest::TestOverrideTagCount()
    *
    *   @brief
    *       Tests tag count defined in override matches expected values
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestOverrideTagCount()
    {
        camera_info info;
        int val = GetCamModule()->get_camera_info(0, &info);

        NT_ASSERT(val == 0, "Read/write operation failed in override module");

        for (size_t tagIndex = 0; tagIndex < NUM_CUSTOM_TAGS; tagIndex++)
        {
            uint32_t tagId = ResolveAvailableVendorTag(&vTag, customTags[tagIndex].tagName, customTags[tagIndex].sectionName);
            NT_ASSERT(tagId != UINT32_MAX, "Could not resolve tagId for tagname %d",customTags[0].tagName);
            camera_metadata_entry_t entry;
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info.static_camera_characteristics),
                tagId, &entry);
            NT_ASSERT(ret == 0, "Could not find metadata entry %d",tagId);

            NT_ASSERT(static_cast<int>(entry.count) == customTags[tagIndex].count
            ,"Count does not match expectation for tagname %s", customTags[tagIndex].tagName);
        }

    }

    /***************************************************************************************************
    *   VendorTagsTest::TestEIS()
    *
    *   @brief
    *       Tests electronic image stabilization (EIS) vendor tag with three streams: preview, video,
    *       and snapshot for given number of frames. A vendor tag is also used to stop the recording.
    *
    *       eisMode values and their respective usecases:
    *           "EISV2" - EISV2 on preview stream, EISV2 on video stream
    *           "EISV3" - EISV2 on preview stream, EISV3 on video stream
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::TestEIS(
        VendorTagItem preview,  //[in] preview format and resolution
        VendorTagItem video,    //[in] video format and resolution
        VendorTagItem snapshot, //[in] snapshot format and resolution
        const char* eisMode,    //[in] determines which EIS usecase to select
        int captureCount,       //[in] number of frames to run the test
        bool isVideoOn)         //[in] turns video stream on or off
    {
        uint32_t eisTag = strncmp(eisMode, "EISV2", 5) == 0 ?
            CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV2 :
            CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3;

        char eisv3TagName[] = "EISV3Enable";
        char eisv3SectionName[] = "org.quic.camera.eis3enable";
        char stopRecordTagName[] = "endOfStream";
        char stopRecordSectionName[] = "org.quic.camera.recording";

        const uint8_t EISV3_OFF = 0;
        const uint8_t EISV3_ON = 1;
        const uint8_t STOP_RECORD_OFF = 0;
        const uint8_t STOP_RECORD_ON = 1;
        // Setup and resolve vendor tag
        bool vendorTags = false;
        camera_metadata_tag_t liveShotSupp;
        char tagName[] = "isLiveshotSizeSameAsVideoSize";
        char sectionName[] = "org.quic.camera.LiveshotSize";
        camera_metadata_tag_t vidConfig;
        char vcTagName[] = "VideoConfigurationsTable";
        char vcSectionName[] = "org.quic.camera2.VideoConfigurations.info";

        // Initialize vendor tags if needed
        if (!vendorTags)
        {
            NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0,"Vendor tags could not be initialized!");
            vendorTags = true;
        }

        uint32_t tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("isLiveshotSizeSameAsVideoSize vendor tag not available!");
            return;
        }
        liveShotSupp = static_cast<camera_metadata_tag_t>(tagNum);

        tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, vcTagName, vcSectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("VideoConfigurations vendor tag not available!");
            return;
        }
        vidConfig = static_cast<camera_metadata_tag_t>(tagNum);
        NATIVETEST_UNUSED_PARAM(EISV3_OFF);
        NATIVETEST_UNUSED_PARAM(STOP_RECORD_OFF);

        uint32_t eisv3TagId = UINT32_MAX;
        if (eisTag == CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3)
        {
            // Resolve EISV3 vendor tag
            eisv3TagId = ResolveAvailableVendorTag(&vTag, eisv3TagName, eisv3SectionName);
            if (eisv3TagId == UINT32_MAX)
            {
                NT_LOG_UNSUPP("EISV3 vendor tag not available!");
                return;
            }
        }

        // Resolve stop recording vendor tag
        uint32_t stopRecordTagId = ResolveAvailableVendorTag(&vTag, stopRecordTagName, stopRecordSectionName);
        if (stopRecordTagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("endOfStream vendor tag not available!");
            return;
        }

         // Setup and resolve maxPreviewSize vendor tag
        camera_metadata_tag_t maxPreviewTag;
        char prTagName[] = "MaxPreviewSize";
        char prSectionName[] = "org.quic.camera.MaxPreviewSize";

        tagNum = VendorTagsTest::ResolveAvailableVendorTag(&vTag, prTagName, prSectionName);
        if (tagNum == UINT32_MAX)
        {
            NT_LOG_UNSUPP("MaxPreviewSize vendor tag not available!");
            return;
        }
        maxPreviewTag = static_cast<camera_metadata_tag_t>(tagNum);


        std::vector<int> camList = Camera3Module::GetCameraList();

        NT_ASSERT(0< static_cast<int>(camList.size()), "No cameras found");

        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            //MaxPreviewSize vendorTag
            camera_metadata_entry_t tagEntry;
            camera_info* pCamInfoMx = GetCameraInfoWithId(cameraId);
            NT_ASSERT(nullptr != pCamInfoMx, "Unable to retrieve camera info for camera Id: %d", cameraId);
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoMx->static_camera_characteristics),
                maxPreviewTag, &tagEntry);
            NT_ASSERT(ret == 0, "find_camera_metadata_entry max_preview_size failed");
            if (preview.resolution.width*preview.resolution.height > static_cast<uint32_t>(tagEntry.data.i32[0] * tagEntry.data.i32[1]))
            {
                preview.resolution.width = tagEntry.data.i32[0];
                preview.resolution.height = tagEntry.data.i32[1];
                NT_LOG_INFO("OVERRIDE: Reseting preview to max preview size supported %d x %d",
                    preview.resolution.width, preview.resolution.height);
            }

            //VideoConfig Vendor Tags
            ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoMx->static_camera_characteristics), vidConfig, &tagEntry);
            NT_ASSERT(ret == 0, "find_camera_metadata_entry max_preview_size failed");
            int index = -1;
            for (int i = 0; i<static_cast<int>(tagEntry.count); i++)
            {
                if (i % 6 == 0 && video.resolution.width == static_cast<uint32_t>(tagEntry.data.i32[i]))
                {
                    index = i;
                    break;
                }
            }
            if (index != -1)//Check if resolution match
            {
                if (tagEntry.data.i32[index + 4] == 1)//isLiveShot Check
                {
                    //LiveShotSameVideo
                    camera_metadata_entry_t tagEntryLive;
                    ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfoMx->static_camera_characteristics), liveShotSupp, &tagEntryLive);
                    NT_ASSERT(ret == 0, "find_camera_metadata_entry LiveshotSizeSameAsVideoSize failed");
                    if (tagEntryLive.data.i32[0] == 1)
                    {
                        snapshot = video;
                        NT_LOG_INFO("Live shot set to video resolution!");
                    }
                }
                else
                {
                    NT_LOG_UNSUPP("LiveShot is not supported");
                    return;
                }
                if (tagEntry.data.i32[index + 5] == 0) //isEIS Check
                {
                    NT_LOG_UNSUPP("EIS is not supported");
                    return;
                }
            }
            else
            {
                NT_LOG_UNSUPP("Video Resolution (%d x %d) not supported", video.resolution.width, video.resolution.height);
                return;
            }

            // Check preview stream parameters are supported
            if (!CheckSupportedResolution(cameraId, preview.format, preview.resolution))
            {
                NT_LOG_UNSUPP("Preview stream format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    preview.format, ConvertResToString(preview.resolution).c_str(), cameraId);
                continue;
            }
            // Check video stream parameters are supported
            if (isVideoOn && !CheckSupportedResolution(cameraId, video.format, video.resolution))
            {
                NT_LOG_UNSUPP("Video stream format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    video.format, ConvertResToString(video.resolution).c_str(), cameraId);
                continue;
            }
            // Check snapshot stream parameters are supported
            if (!CheckSupportedResolution(cameraId, snapshot.format, snapshot.resolution))
            {
                NT_LOG_UNSUPP("Snapshot stream format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    snapshot.format, ConvertResToString(snapshot.resolution).c_str(), cameraId);
                continue;
            }

            // Open camera
            Camera3Device::OpenCamera(cameraId);
            int frameNumber = 0;
            if (isVideoOn)
            {
                prepareEISVideoRequest(cameraId, preview, video, snapshot, frameNumber, eisTag); //non fatal
            }
            else
            {
                prepareEISSnapshotRequest(cameraId, preview, snapshot, frameNumber, eisTag); //non fatal
            }

            // Set EIS metadata tags
            uint8_t eisv2Val = ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_ON;
            Camera3Stream::SetKey(cameraId, ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &eisv2Val, 1);
            if (eisTag == CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3)
            {
                Camera3Stream::SetKey(cameraId, eisv3TagId, &EISV3_ON, 1);
            }

            // Start recording and verify the tags are correct in the capture result
            verifyEISResult(cameraId, frameNumber, ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &eisv2Val, isVideoOn);

            if (eisTag == CAMERA3_VENDOR_STREAM_CONFIGURATION_EISV3)
            {
                verifyEISResult(cameraId, frameNumber, eisv3TagId, &EISV3_ON, isVideoOn); //non fatal
            }
            NT_LOG_DEBUG("EIS tags were found correctly in the capture result");

            // Continue to run test for number of requested frames
            for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
            {
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(1, frameNumber);
                if (isVideoOn)
                {
                    GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(2, frameNumber);
                }
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "GetHal request failed");
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr), "Start Streaming Failed");
            }

            // Dump buffers
            std::ostringstream outputNamePreview;
            std::ostringstream outputNameVideo;
            std::ostringstream outputNameSnapshot;
            outputNamePreview << "EIS_Preview";
            outputNameVideo << "EIS_Video";
            outputNameSnapshot << "EIS_Snapshot";
            DumpBuffer(0, frameNumber, ConstructOutputFileName(cameraId, preview.format, -1, outputNamePreview.str().c_str()).c_str());
            if (isVideoOn)
            {
                DumpBuffer(1, frameNumber, ConstructOutputFileName(cameraId, video.format, -1, outputNameVideo.str().c_str()).c_str());
                DumpBuffer(2, frameNumber, ConstructOutputFileName(cameraId, snapshot.format, -1, outputNameSnapshot.str().c_str()).c_str());
            }
            else
            {
                DumpBuffer(1, frameNumber, ConstructOutputFileName(cameraId, snapshot.format, -1, outputNameSnapshot.str().c_str()).c_str());
            }

            // Stop recording (send stop recording vendor tag)
            Camera3Stream::SetKey(cameraId, stopRecordTagId, &STOP_RECORD_ON, 1);
            verifyEISResult(cameraId, frameNumber, stopRecordTagId, &STOP_RECORD_ON, isVideoOn);

            NT_LOG_DEBUG("endOfStream tag found in capture result");

            Camera3Stream::FlushStream(cameraId);
            Camera3Stream::DeleteHalRequest(cameraId);

            // clear streams
            Camera3Stream::ClearOutStreams();

            // close camera
            Camera3Device::CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************
    *   VendorTagsTest::verifyEISResult()
    *
    *   @brief
    *       Verifies that electronic image stabilization (EIS) vendor tag took in capture result
    *
    *   @return
    *       void
    ****************************************************************************************************/
    void VendorTagsTest::verifyEISResult(
        int cameraId,           //[in] camera Id
        int &frameNumber,       //[in/out] frame number
        uint32_t tagId,         //[in] EIS tag Id
        const uint8_t *value,   //[in] value of the EIS tag to verify
        bool isVideoOn)         //[in] is video stream included in verification
    {
        int i = 0;
        int success = 0;
        const camera_metadata_t* metaFromFile;

        while (i <= PROCESS_CAPTURE_TIMEOUT)
        {
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, frameNumber);
            GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(1, frameNumber);
            if (isVideoOn)
            {
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(2, frameNumber);
            }
            NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for request!");
            NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber++, false, 0, nullptr), "Start Streaming Failed");
            metaFromFile = GetCameraMetadataByFrame(GetHalRequest(cameraId)->frame_number);
            NT_ASSERT(metaFromFile != nullptr, "Meta read from file is null \n");

            camera_metadata_entry_t entry;
            int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(metaFromFile), tagId, &entry);

            if (ret == 0 && entry.count > 0 && entry.data.u8[0] == *value)
            {
                NT_LOG_INFO("Key in capture result is matches requested value");
                success = 1;
                break;
            }

            if (metaFromFile != nullptr)
            {
                free(const_cast<camera_metadata_t*>(metaFromFile));
            }
            i++;
        }
        NT_ASSERT(success == 1, "Capture Results does not contain the value set");
    }

}
