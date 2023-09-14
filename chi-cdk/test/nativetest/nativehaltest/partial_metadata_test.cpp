//*****************************************************************************
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*****************************************************************************

#include "partial_metadata_test.h"

namespace haltests
{

    void PartialMetadataTest::Setup()
    {
        Camera3Stream::Setup();

        m_prevFrameNumber = 0;
        m_receivedPartialIdsSet.clear();

        for (int i = 0; i < MAX_PARTIAL_RESULTS; i++)
        {
            m_pPartialMetadatas[i] = NULL;
        }
    }

    void PartialMetadataTest::Teardown()
    {
        // Clean up any cloned metadata
        for (int i = 0; i < MAX_PARTIAL_RESULTS; i++)
        {
            if (NULL != m_pPartialMetadatas[i])
            {
                free_camera_metadata(m_pPartialMetadatas[i]);
                m_pPartialMetadatas[i] = NULL;
            }
        }

        Camera3Stream::Teardown();
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::TestPartialStaticSupport
    *
    * @brief
    *   Reads the partial result count from static metadata and verifies it is as expected (camxoverridesettings control what
    *   will be set)
    * @param
    *   [in]    PartialMode     mode    partial metadata mode
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::TestPartialStaticSupport(PartialMode mode)
    {
        NT_ASSERT(0 <= mode && mode < PartialNumOfModes,"Invalid partial metadata mode! (%u)",mode);
        uint32_t expectCount = m_modeCount[mode];

        std::vector<int> camList = Camera3Module::GetCameraList();
        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            OpenCamera(cameraId);
            uint32_t obtainCount = GetPartialResultCount(cameraId);
            NT_EXPECT(expectCount == obtainCount, "Static metadata partial result count (%u) for camera %d  not as expected! (%u) ",
                obtainCount,cameraId, expectCount);
            CloseCamera(cameraId);
        }
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::TestPartialOrdering
    *
    * @brief
    *   Takes 20 captures, ensuring that the ordering of partial results is monotonic. A partial_result value of 0 means
    *   buffers only and is allowed to appear at any time.
    * @param
    *   [in]    PartialMode     mode    partial metadata mode
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::TestPartialOrdering(PartialMode mode)
    {
        std::ostringstream testName;
        char modeLetter = 'A' + mode;
        testName << "TestPartialOrdering" << modeLetter;
        TakeCaptureAndValidate(mode, PartialOrdering, testName.str().c_str());
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::TestPartialDisjoint
    *
    * @brief
    *   Takes 20 captures, ensuring that the metadata in all non-zero partial results is disjoint
    * @param
    *   [in]    PartialMode     mode    partial metadata mode
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::TestPartialDisjoint(PartialMode mode)
    {
        std::ostringstream testName;
        char modeLetter = 'A' + mode;
        testName << "TestPartialDisjoint" << modeLetter;
        TakeCaptureAndValidate(mode, PartialDisjoint, testName.str().c_str());
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::TestPartialEnableZSL
    *
    * @brief
    *
    * @param
    *   [in]    PartialMode     mode    partial metadata mode
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::TestPartialEnableZSL(PartialMode mode)
    {
        std::ostringstream testName;
        char modeLetter = 'A' + mode;
        testName << "TestPartialEnableZSL" << modeLetter;
        m_foundZSLTagCount = 0;
        TakeCaptureAndValidate(mode, PartialEnableZSL, testName.str().c_str());
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::TestPartialFlush
    *
    * @brief
    *
    * @param
    *   [in]    PartialMode     mode    partial metadata mode
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::TestPartialFlush(PartialMode mode)
    {
        std::ostringstream testName;
        char modeLetter = 'A' + mode;
        testName << "TestPartialFlush" << modeLetter;
        m_partialFlushCount = 0;
        TakeCaptureAndValidate(mode, PartialFlush, testName.str().c_str());
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::TakeCaptureAndValidate
    *
    * @brief
    *   Take 20 captures and perform some type of partial metadata validation after each capture, according to the test type
    * @param
    *   [in]    PartialMode     mode            partial metadata mode
    *   [in]    PartilTestType  testType        partial metadata test type
    *   [in]    const char*     testName        test name
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::TakeCaptureAndValidate(PartialMode mode, PartialTestType testType, const char* testName)
    {
        int format = HAL_PIXEL_FORMAT_YCbCr_420_888;
        Size resolution = UHD;
        int captureCount = HalJsonParser::sFrames;

        // If stream is MAX_RES resolution or RAW format, need to query each camera for its max resolution
        bool shouldQueryMaxRes = false;
        if ((resolution.width == MAX_RES.width && resolution.height == MAX_RES.height) || format == HAL_PIXEL_FORMAT_RAW16 ||
            format == HAL_PIXEL_FORMAT_RAW10)
        {
            shouldQueryMaxRes = true;
        }

        std::vector<int> camList = Camera3Module::GetCameraList();
        for (size_t camCounter = 0; camCounter < camList.size(); camCounter++)
        {
            // Get the actual camera ID to test
            int cameraId = camList[camCounter];

            m_prevPartialResult = 0;

            // Query and overwrite with queried maximum resolution
            if (shouldQueryMaxRes)
            {
                camera_info* pCamInfo = GetCameraInfoWithId(cameraId);
                NT_ASSERT(nullptr != pCamInfo, "Unable to retrieve camera info for camera Id: %d", cameraId);
                resolution = GetMaxSize(format, pCamInfo);
                if (resolution.width == 0 && resolution.height == 0)
                {
                    NT_LOG_UNSUPP("Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                        format, ConvertResToString(resolution).c_str(), cameraId);
                    continue;
                }
                NT_LOG_DEBUG( "Raw resolution width %d height %d", resolution.width, resolution.height);
            }

            // Check for format/resolution support
            if (!CheckSupportedResolution(cameraId, format, resolution))
            {
               NT_LOG_UNSUPP( "Given format: [%d] and resolution: [%s] not supported for cameraId: [%d]",
                    format, ConvertResToString(resolution).c_str(), cameraId);
                continue;
            }

            OpenCamera(cameraId);

            // Record partial result count supported by camera
            m_partialResultCount[cameraId] = GetPartialResultCount(cameraId);

            // Validate given partial metadata count (helps detects if wrong camxoverride mode applied)
            uint32_t obtainCount = m_partialResultCount[cameraId];
            uint32_t expectCount = m_modeCount[mode];
            NT_ASSERT(expectCount == obtainCount,"Static metadata partial result count (%u) for camera %d not as expected! ( %u)",
                 obtainCount, cameraId, expectCount);

            // Setup and configure stream
            CreateStream(CAMERA3_STREAM_OUTPUT, resolution, 0, format, CAMERA3_STREAM_ROTATION_0);
            NT_ASSERT(ConfigStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, cameraId) == 0,"configure streams failed");

            int frameNumber = 0;
#ifdef ENABLE3A
            // Perform 3A algorithm
            frameNumber = Camera3Stream::ProcessCaptureRequestWith3AConverged(cameraId, frameNumber, CAMERA3_TEMPLATE_PREVIEW,
                0, false, "");
            NT_ASSERT(frameNumber != -1,"3A cannot converge!");
#endif
            // Create long-lived hal request
            CreateCaptureRequestWithMultBuffers(cameraId, 0, CAMERA3_TEMPLATE_STILL_CAPTURE, 1, ++frameNumber);

            // Main capture loop
            for (int frameIndex = 1; frameIndex <= captureCount; frameIndex++)
            {
                // Perform any special setup, according to test type
                switch (testType)
                {
                case PartialEnableZSL:
                    SetEnableZsl(cameraId, IsZSLFrame(frameNumber));
                    break;
                case PartialFlush:
                {
                    ClearErrorNotifyFlag(frameNumber);
                    bool isFlushFrame = (frameIndex % FLUSH_FRAME_INTERVAL) == 0;   // Flush every 7 frames
                    if (isFlushFrame)
                    {
                        SetFlushTestFrame();
                    }
                    else
                    {
                        ClearFlushTestFrame();
                    }
                    break;
                }
                default:
                    if (PartialNumOfTestTypes <= testType)
                    {
                        NT_LOG_ERROR("Invalid partial metadata test type (%d)!", testType)
                    }
                    break;
                }

                // Send hal request and dump image
                std::string captureImageName = ConstructOutputFileName(cameraId, format, frameNumber, testName);
                NT_ASSERT(CDKResultSuccess == StreamStarter(cameraId, frameNumber, true, 0, captureImageName.c_str()), "Start Streaming Failed");
                // Get capture result and then do validation according to test type
                PartialMetadataSet* pPartialSet = GetLastPartialMetadata();
                while (nullptr != pPartialSet)
                {
                    CDKResult validateRes;
                    switch (testType)
                    {
                    case PartialOrdering:
                        validateRes = ValidateOrdering(cameraId, mode, pPartialSet);
                        break;
                    case PartialDisjoint:
                        validateRes = ValidateDisjoint(cameraId, mode, pPartialSet);
                        break;
                    case PartialEnableZSL:
                        validateRes = ValidateEnableZSL(cameraId, mode, pPartialSet);
                        break;
                    case PartialFlush:
                        validateRes = ValidateFlush(cameraId, mode, pPartialSet);
                        break;
                    default:
                        NT_LOG_ERROR("Partial metadata test type (%d) has no validation function!", testType);
                        validateRes = CDKResultEFailed;
                        break;
                    }
                    NT_ASSERT(CDKResultSuccess == validateRes,"Partial metadata validation failure. Expected: %d Actual: %d!",
                        CDKResultSuccess, validateRes);

                    // Clean up and get next result
                    pPartialSet->metadata.ClearMetadata();
                    delete pPartialSet;
                    pPartialSet = GetLastPartialMetadata();
                }

                // Get new buffer for next frame
                GetHalRequest(cameraId)->output_buffers = GetBufferForRequest(0, ++frameNumber);
                NT_ASSERT(GetHalRequest(cameraId)->output_buffers != nullptr, "Failed to get output buffer for frame %d",
                    frameNumber);
            }

            // Teardown
            DeleteHalRequest(cameraId);
            FlushStream(cameraId);
            ClearOutStreams();
            CloseCamera(cameraId);
        }
    }

    /***************************************************************************************************************************
    * PartialMetadataTest::SetEnableZsl()
    *
    * @brief
    *   Prepares the existing HAL request to test ENABLE_ZSL tag. CAPTURE_INTENT must be set to STILL_CAPTURE mode, and then
    *   the ENABLE_ZSL key should be set.
    * @param
    *   [in]    int     cameraId    camera id
    *   [in]    bool    setVal      value to set for ENABLE_ZSL
    * @return
    *   void
    **************************************************************************************************************************/
    void PartialMetadataTest::SetEnableZsl(int cameraId, bool enable)
    {
        camera_metadata_entry_t entry;
        uint8_t setVal;

        // Ensure that capture intent is set to STILL_CAPTURE
        int ret = GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(GetHalRequest(cameraId)->settings),
            ANDROID_CONTROL_CAPTURE_INTENT, &entry);
        NT_ASSERT(ret == 0,"GetCameraMetadataEntryByTag ANDROID_CONTROL_CAPTURE_INTENT failed");

        if (entry.count == 1 && entry.data.u8[0] == ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE)
        {
            NT_LOG_INFO("CAPTURE_INTENT is STILL_CAPTURE (%u), ENABLE_ZSL is effective", entry.data.u8[0]);
        }
        else
        {
            NT_LOG_WARN("CAPTURE_INTENT is %u, setting to STILL_CAPTURE to ensure that ENABLE_ZSL is effective",
                entry.data.u8[0]);
            setVal = ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE;
            SetKey(cameraId, ANDROID_CONTROL_CAPTURE_INTENT, &setVal, 1);
        }

        // Set ENABLE_ZSL to desired value
        setVal = (enable) ? ANDROID_CONTROL_ENABLE_ZSL_TRUE : ANDROID_CONTROL_ENABLE_ZSL_FALSE;
        NT_LOG_INFO("Setting ENABLE_ZSL on frame [%u] to value [%u]", GetHalRequest(cameraId)->frame_number,
            setVal);
        SetKey(cameraId, ANDROID_CONTROL_ENABLE_ZSL, &setVal, 1);
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::ValidateOrdering
    *
    * @brief
    *   Validate that partial_result ordering: the maximum supported partial result should arrive last, except for 0, which
    *   may arrive after the max partial_result.
    * @param
    *   [in]    int                         cameraId        camera id
    *   [in]    PartialMode                 mode            partial metadata mode
    *   [in]    PartialMetadataSet*         pPartialSet     capture result to validate
    * @return
    *   CDKResult
    **************************************************************************************************************************/
    CDKResult PartialMetadataTest::ValidateOrdering(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet)
    {
        NATIVETEST_UNUSED_PARAM(mode);  // Logic is the same regardless of partial metadata mode

        CDKResult res = CDKResultSuccess;
        uint32_t currFrameNumber = pPartialSet->frame_number;
        uint32_t currPartialResult = pPartialSet->partial_result;
        NT_LOG_INFO("Validate partial metadata ordering on frame [%u], partial result [%u]...", currFrameNumber,
            currPartialResult);

        // Clear out received partial ids between different frames
        if (m_prevFrameNumber != currFrameNumber)
        {
            m_prevPartialResult = 0;
            m_receivedPartialIdsSet.clear();
        }

        // If there is metadata to validate...
        if (0 != currPartialResult)
        {
            // Keep track of received partial result Ids in a set
            m_receivedPartialIdsSet.insert(currPartialResult);
            if (currPartialResult <= m_prevPartialResult)
            {
                NT_LOG_ERROR("Partial result Id %u arrived before expected: %u", currPartialResult, m_prevPartialResult);
                res |= CDKResultEFailed;
            }

            // Check for too many partial results
            if (currPartialResult > m_partialResultCount[cameraId] ||
                m_receivedPartialIdsSet.size() > m_partialResultCount[cameraId])
            {
                NT_LOG_ERROR("Too many partial results were returned! (expect: %u, receive: %u)", m_partialResultCount[cameraId], m_receivedPartialIdsSet.size());
                res |= CDKResultEFailed;
            }
        }

        // Overwrite for non-zero partial results (0 does not count for monotonicity)
        m_prevPartialResult = currPartialResult;
        m_prevFrameNumber = currFrameNumber;
        return res;
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::ValidateDisjoint
    *
    * @brief
    *   Validate that each partial result has a disjoint set of tags, compared to the set of partial results from the same
    *   frame number.
    * @param
    *   [in]    int                         cameraId        camera id
    *   [in]    PartialMode                 mode            partial metadata mode
    *   [in]    PartialMetadataSet*         pPartialSet     capture result to validate
    * @return
    *   CDKResult
    **************************************************************************************************************************/
    CDKResult PartialMetadataTest::ValidateDisjoint(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet)
    {
        NATIVETEST_UNUSED_PARAM(cameraId);
        NATIVETEST_UNUSED_PARAM(mode);      // Logic is the same regardless of partial metadata mode

        CDKResult res = CDKResultSuccess;
        uint32_t currFrameNumber = pPartialSet->frame_number;
        uint32_t currPartialResult = pPartialSet->partial_result;
        const camera_metadata_t* currPartialMetadata = pPartialSet->metadata.GetCamMetadata();

        NT_LOG_INFO("Validate partial metadata disjoint on frame [%u], partial result [%u]...", currFrameNumber,
            currPartialResult);

        // Only validate if prev and curr partial result have the same frame number
        if (m_prevFrameNumber == currFrameNumber)
        {
            // If there is metadata to validate...
            if (0 != currPartialResult)
            {
                // Validate that partial results are disjoint in their metadata entries
                if (currPartialResult > 1)
                {
                    int entryCount = get_camera_metadata_entry_count(currPartialMetadata);
                    camera_metadata_entry_t currEntry;
                    camera_metadata_entry_t foundEntry;

                    // For each entry in the current partial metadata...
                    for (int entryIndex = 0; entryIndex < entryCount; entryIndex++)
                    {
                        if (0 != get_camera_metadata_entry(const_cast<camera_metadata_t*>(currPartialMetadata), entryIndex,
                            &currEntry))
                        {
                            NT_LOG_ERROR("Failed to get metadata entry from partial metadata!");
                            return CDKResultEFailed;
                        }

                        // Move backward through each PM...
                        for (int pmIndex = currPartialResult-1; pmIndex >= 0; pmIndex--)
                        {
                            // Verify that each entry/tag in the current PM is not in the previous PMs
                            if (0 == GetCameraMetadataEntryByTag(m_pPartialMetadatas[pmIndex],
                                static_cast<camera_metadata_tag_t>(currEntry.tag), &foundEntry))
                            {
                                NT_LOG_ERROR("Partial results must be disjoint from all other partial results of "
                                    "the same frame number! Found tag [%u] in multiple partial results for frame [%u]",
                                    currEntry.tag, currFrameNumber);
                                res |= CDKResultEFailed;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // Clean up the last frame's cloned partial metadatas
            for (int i = 0; i < MAX_PARTIAL_RESULTS; i++)
            {
                if (nullptr != m_pPartialMetadatas[i])
                {
                    free_camera_metadata(m_pPartialMetadatas[i]);
                    m_pPartialMetadatas[i] = NULL;
                }
            }
        }

        // Copy partial metadata to compare to future partial results
        m_pPartialMetadatas[currPartialResult] = clone_camera_metadata(currPartialMetadata);
        if (nullptr == m_pPartialMetadatas[currPartialResult] && nullptr != currPartialMetadata)
        {
            NT_LOG_ERROR("Failed to copy metadata from partial result!");
            return CDKResultEFailed;
        }

        m_prevFrameNumber = currFrameNumber;
        return res;
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::ValidateEnableZSL
    *
    * @brief
    *   Validate that ZSL tag is set as expected (true even frames, false odd frames), and is only contained in one of the
    *   partial results.
    * @param
    *   [in]    int                         cameraId        camera id
    *   [in]    PartialMode                 mode            partial metadata mode
    *   [in]    PartialMetadataSet*         pPartialSet     capture result to validate
    * @return
    *   CDKResult
    **************************************************************************************************************************/
    CDKResult PartialMetadataTest::ValidateEnableZSL(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet)
    {
        NATIVETEST_UNUSED_PARAM(cameraId);
        NATIVETEST_UNUSED_PARAM(mode);      // Logic is the same regardless of partial metadata mode

        CDKResult res = CDKResultSuccess;
        uint32_t currFrameNumber = pPartialSet->frame_number;
        uint32_t currPartialResult = pPartialSet->partial_result;
        const camera_metadata_t* currPartialMetadata = pPartialSet->metadata.GetCamMetadata();

        NT_LOG_INFO("Validate partial metadata enable ZSL on frame [%u], partial result [%u]...", currFrameNumber,
            currPartialResult);

        // If frame changed, then verify that enable ZSL tag was found exactly once among all partial metadata
        if (currFrameNumber != m_prevFrameNumber)
        {
            // Skip if this is the first frame (and there was no previous frame)
            if (0 != m_prevFrameNumber)
            {
                NT_EXPECT(1 == m_foundZSLTagCount,"Frame %u had incorrect amount of entries with ENABLE_ZSL tag! (%u )",
                    m_prevFrameNumber, m_foundZSLTagCount);
                if (1 != m_foundZSLTagCount)
                {
                    res |= CDKResultEFailed;
                }
            }
            // Reset the found count for this new frame
            m_foundZSLTagCount = 0;
        }

        // If there is metadata to validate...
        if (0 != currPartialResult)
        {
            // Check partial metadata for enable ZSL tag
            camera_metadata_entry_t foundEntry;
            uint8_t expectVal = (IsZSLFrame(currFrameNumber)) ? ANDROID_CONTROL_ENABLE_ZSL_TRUE :
                ANDROID_CONTROL_ENABLE_ZSL_FALSE;
            if (0 == GetCameraMetadataEntryByTag(const_cast<camera_metadata_t*>(currPartialMetadata),
                ANDROID_CONTROL_ENABLE_ZSL, &foundEntry))
            {
                NT_LOG_INFO("Found EnableZSL tag for frame [%u], partial result [%u], value [%u]", currFrameNumber,
                    currPartialResult, *foundEntry.data.u8);

                // Validate value is as expected
                NT_EXPECT(*foundEntry.data.u8 == expectVal,"EnableZSL tag found value [ %u ] for frame [%u] does not match the expect value [%u]",
                       *foundEntry.data.u8,currFrameNumber, expectVal);
                if (*foundEntry.data.u8 != expectVal)
                {
                    res |= CDKResultEFailed;
                }
                m_foundZSLTagCount++;
            }
        }

        m_prevFrameNumber = currFrameNumber;
        return res;
    }

    /**************************************************************************************************************************
    * PartialMetadataTest::ValidateFlush
    *
    * @brief
    *   Verify that the number of partial metadata received for the last frame was as expected. This function does not check
    *   for an error notify, as this is done in camera3stream.cpp.
    * @param
    *   [in]    int                         cameraId        camera id
    *   [in]    PartialMode                 mode            partial metadata mode
    *   [in]    PartialMetadataSet*         pPartialSet     capture result to validate
    * @return
    *   CDKResult
    **************************************************************************************************************************/
    CDKResult PartialMetadataTest::ValidateFlush(int cameraId, PartialMode mode, PartialMetadataSet* pPartialSet)
    {
        NATIVETEST_UNUSED_PARAM(mode);      // Logic is the same regardless of partial metadata mode

        CDKResult res = CDKResultSuccess;
        uint32_t currFrameNumber = pPartialSet->frame_number;
        uint32_t currPartialResult = pPartialSet->partial_result;

        NT_LOG_INFO("Validate partial metadata flush on frame [%u], partial result [%u]...", currFrameNumber,
            currPartialResult);

        // If frame has changed and partial result flush count is non-negative, then assume previous frame was flushed
        if (currFrameNumber != m_prevFrameNumber && 0 <= m_partialFlushCount)
        {
            // Partial result count was correct
            if (m_partialFlushCount == static_cast<int>(m_partialResultCount[cameraId]))
            {
                NT_LOG_INFO("Number of partial results received for flushed frame %u: [%d/%u]",
                    m_prevFrameNumber, m_partialFlushCount, m_partialResultCount[cameraId]);
            }
            else  // Partial result count was off
            {
                // Only issue warning, no hard requirement: flushed frames may not send back all partial results
                // Buffer status and error notify checks are done in camera3device and camera3stream
                NT_LOG_WARN("Number of partial results received for flushed frame %u: [%d/%u]",
                    m_prevFrameNumber, m_partialFlushCount, m_partialResultCount[cameraId]);
            }
        }

        // If this frame was flushed...
        if (IsFlushTestFrame())
        {
            // And this is a new frame....
            if (currFrameNumber != m_prevFrameNumber)
            {
                // Reset partial flush count for newly flushed frame
                m_partialFlushCount = 0;
            }

            // Count partial results
            if (0 != currPartialResult)
            {
                m_partialFlushCount++;
            }
        }
        else
        {
            // Invalidate/reset the count (-1 means frame not flushed)
            m_partialFlushCount = -1;
        }

        m_prevFrameNumber = currFrameNumber;
        return res;
    }

}