//******************************************************************************
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#include "chimetadata_test.h"
#include "commonutils.h"

namespace chitests
{
    /**************************************************************************************************
    *   ChiMetadataTest::Setup
    *
    *   @brief
    *       Entry point to the test
    **************************************************************************************************/
    void ChiMetadataTest::Setup()
    {
        m_pMetadataUtil = ChiMetadataUtil::GetInstance();
        m_pChiModule    = ChiModule::GetInstance();
    }

    /**************************************************************************************************
    *   ChiMetadataTest::Teardown
    *
    *   @brief
    *       Teardown point to the test
    **************************************************************************************************/
    void ChiMetadataTest::Teardown()
    {
        if (m_pMetadataUtil != nullptr)
        {

            m_pMetadataUtil->DestroyMetabuffer(GetDefaultMetahandle());
            m_pMetadataUtil->DestroyMetabuffer(GetMetaHandle());
            m_pMetadataUtil->DestroyInstance();
            m_pMetadataUtil = nullptr;
        }

        // Destroying the Instance of chi module instance as part of teardown
        if (m_pChiModule != nullptr)
        {
            m_pChiModule->DestroyInstance();
            m_pChiModule = nullptr;
        }
    }

    /**************************************************************************************************
    *   ChiMetadataTest::GetMetaHandle
    *
    *   @brief
    *       Gets the metahandle that is created once and used for the rest of the tests
    **************************************************************************************************/
    CHIMETAHANDLE ChiMetadataTest::GetMetaHandle()
    {
        m_pMetadataUtil->CreateMetabuffer(&m_pMetaHandle);
        return m_pMetaHandle;
    }

    /**************************************************************************************************
    *   ChiMetadataTest::GetDefaultMetahandle
    *
    *   @brief
    *       Gets the default metahandle that is created once and used for the rest of the tests
    **************************************************************************************************/
    CHIMETAHANDLE ChiMetadataTest::GetDefaultMetahandle()
    {
        m_pMetadataUtil->CreateDefaultMetadata(0, &m_pDefaultMetaHandle);
        return m_pDefaultMetaHandle;
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCreateMetadata
    *
    *   @brief
    *       Tests the PFN_CMB_CREATE of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestCreateMetadata()
    {
        CDKResult result;
        if (m_pMetadataUtil != nullptr)
        {
            result = m_pMetadataUtil->CreateMetabuffer(&m_pMetaHandle);
        }
        else
        {
            NT_LOG_ERROR("m_pMetadataUtil is NULL");
            result = CDKResultEFailed;
        }
        NT_ASSERT(CDKResultSuccess == result, "Can't create metadata buffer");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCreateDefaultMetadata
    *
    *   @brief
    *       Tests the PFN_CMB_GET_DEFAULT_METADATA of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestCreateDefaultMetadata()
    {
        CDKResult result;
        result = m_pMetadataUtil->CreateDefaultMetadata(0, &m_pDefaultMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Can't create default metadata buffer");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestDestroyMetadata
    *
    *   @brief
    *       Tests the PFN_CMB_DESTROY of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestDestroyMetadata()
    {
        if (m_pMetadataUtil != nullptr)
        {
            CHIMETAHANDLE hDefaultHandle = GetDefaultMetahandle();
            CDKResult result;
            result = m_pMetadataUtil->DestroyMetabuffer(hDefaultHandle);
            NT_ASSERT(CDKResultSuccess == result, "Failed to destroy the metadata buffer");
        }
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCreateWithTagArray
    *
    *   @brief
    *      Tests the PFN_CMB_CREATE_WITH_TAGARRAY of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestCreateWithTagArray()
    {
        CHIMETAHANDLE hMetaHandle;
        CHIMETAHANDLE hDefaultMetadata = GetDefaultMetahandle();
        NATIVETEST_UNUSED_PARAM(hDefaultMetadata);
        UINT32 countOfTags = 0;
        m_pMetadataUtil->GetMetadataOps().pGetMetadataEntryCount(&countOfTags);
        ChiMetadataEntry* metadataEntry = nullptr;
        metadataEntry = SAFE_NEW() ChiMetadataEntry[countOfTags];
        UINT32 *metadataTagArray = SAFE_NEW() UINT32[countOfTags];
        CDKResult result = m_pMetadataUtil->GetMetadataTable(metadataEntry);
        if (result == CDKResultSuccess)
        {
            for (UINT32 i = 0; i < countOfTags; i++)
            {
                metadataTagArray[i] = metadataEntry[i].tagID;

            }
        }
        result = m_pMetadataUtil->CreateMetabufferWithTagList(metadataTagArray, countOfTags, &hMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Failed to create the metadata buffer with tag list");

        delete[] metadataEntry;
        delete[] metadataTagArray;

        metadataEntry = nullptr;
        metadataTagArray = nullptr;
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCreateWithAndroidMetadata
    *
    *   @brief
    *       Tests the PFN_CMB_CREATE_WITH_ANDROIDMETADATA of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestCreateWithAndroidMetadata()
    {
        std::vector <int> camList = m_pChiModule->GetCameraList();

        for (size_t camIndex = 0; camIndex < camList.size(); camIndex++)
        {
            CHIMETAHANDLE hMetaHandle;
            CDKResult result;
            if (m_pMetadataUtil != nullptr)
            {
                result = m_pMetadataUtil->CreateMetabufferWithAndroidMetadata(camList[camIndex], &hMetaHandle);
            }
            else
            {
                NT_LOG_ERROR("m_pMetadataUtil is NULL");
                result = CDKResultEFailed;
            }
            NT_ASSERT(CDKResultSuccess == result, "Failed to create the metadata buffer with default android metadata");
        }
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestSetTag
    *
    *   @brief
    *       Tests the PFN_CMB_SET_TAG of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestSetTag()
    {
        CHIMETAHANDLE hDefaultMetaHandle = GetDefaultMetahandle();
        CHIRATIONAL colorTransformMatrix[9];
        uint32_t tagID    = ANDROID_COLOR_CORRECTION_TRANSFORM;
        CDKResult result = m_pMetadataUtil->SetTag(hDefaultMetaHandle, tagID, &colorTransformMatrix[9], 1);
        NT_ASSERT(CDKResultSuccess == result, "Failed to set the tag");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestGetTag
    *
    *   @brief
    *       Tests the PFN_CMB_GET_TAG of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestGetTag()
    {
        CHIRATIONAL *colorTransformMatrix = SAFE_NEW() CHIRATIONAL[9];
        CDKResult result = m_pMetadataUtil->GetTag(GetDefaultMetahandle(), ANDROID_COLOR_CORRECTION_TRANSFORM,
            reinterpret_cast<VOID**>(&colorTransformMatrix));
        NT_ASSERT(result == CDKResultSuccess, "Failed to get Tag ANDROID_COLOR_CORRECTION_TRANSFORM");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestDeleteTag
    *
    *   @brief
    *        Tests the PFN_CMB_DELETE_TAG of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestDeleteTag()
    {
        CHIMETAHANDLE hMetaHandle;
        m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);

        CHIMETAHANDLE hDefaultMetaHandle = (GetDefaultMetahandle());
        CDKResult result = m_pMetadataUtil->CopyMetaBuffer(hMetaHandle, hDefaultMetaHandle);

        result = m_pMetadataUtil->DeleteTagFromMetaBuffer(hMetaHandle, ANDROID_COLOR_CORRECTION_TRANSFORM);
        NT_ASSERT(CDKResultSuccess == result, "Failed to delete tag from the metadata");
        NT_ASSERT(false == m_pMetadataUtil->CompareSizeOfTagList(hMetaHandle, hDefaultMetaHandle),
             "Deleted buffer has same tag count as the source metadata");

        m_pMetadataUtil->DestroyMetabuffer(hMetaHandle);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestMergeDisjoint
    *
    *   @brief
    *        Tests the PFN_CMB_MERGE of ChiMetadataOps with disjoint flag set to true
    **************************************************************************************************/
    void ChiMetadataTest::TestMergeDisjoint()
    {
        CHIMETAHANDLE hMetaHandle;
        m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);

        CHIMETAHANDLE hDefaultMetaHandle = (GetDefaultMetahandle());
        CDKResult result = m_pMetadataUtil->CopyMetaBuffer(hMetaHandle, hDefaultMetaHandle);
        result = m_pMetadataUtil->DeleteTagFromMetaBuffer(hMetaHandle, ANDROID_COLOR_CORRECTION_TRANSFORM);
        NT_ASSERT(CDKResultSuccess == result, "Failed to delete tag from the metadata");

        result = m_pMetadataUtil->MergeMetaBufferDisjoint(hMetaHandle, hDefaultMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Failed to perform a disjoint merge of the metadata");

        CHIMETADATAENTRY tagInSource;
        CHIMETADATAENTRY tagInMergedBuffer;
        result = m_pMetadataUtil->GetMetadataOps().pGetTagEntry(hDefaultMetaHandle, ANDROID_COLOR_CORRECTION_TRANSFORM, &tagInSource);
        result |= m_pMetadataUtil->GetMetadataOps().pGetTagEntry(hDefaultMetaHandle, ANDROID_COLOR_CORRECTION_TRANSFORM, &tagInMergedBuffer);

        NT_EXPECT(0 == memcmp(tagInSource.pTagData, tagInMergedBuffer.pTagData, tagInSource.size), "");
        m_pMetadataUtil->DestroyMetabuffer(hMetaHandle);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCopy
    *
    *   @brief
    *       Tests the PFN_CMB_COPY of ChiMetadataOps with disjoint flag set to false
    **************************************************************************************************/
    void ChiMetadataTest::TestCopy()
    {
        CHIMETAHANDLE hDefaultMetaHandle = (GetDefaultMetahandle());
        CHIMETAHANDLE hMetaHandle;
        CDKResult result = m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);
        result = m_pMetadataUtil->CopyMetaBuffer(hMetaHandle, hDefaultMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Failed to create a copy of the metabuffer");
        NT_ASSERT(true == m_pMetadataUtil->CompareSizeOfTagList(hMetaHandle, hDefaultMetaHandle),
            "Copied buffer has different tag count than the source metadata");

        m_pMetadataUtil->DestroyMetabuffer(hMetaHandle);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestMerge
    *
    *   @brief
    *       Tests the PFN_CMB_MERGE of ChiMetadataOps with disjoint flag set to false
    **************************************************************************************************/
    void ChiMetadataTest::TestMerge()
    {
        CHIMETAHANDLE hMetaHandle;
        CDKResult result = m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Failed to create the metabuffer");

        CHIMETAHANDLE hDefaultMetaHandle = (GetDefaultMetahandle());
        result = m_pMetadataUtil->MergeMetaBuffer(hMetaHandle, hDefaultMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Failed to merge the metabuffer");

        NT_ASSERT(true == m_pMetadataUtil->CompareMetaBuffers(hDefaultMetaHandle, hMetaHandle),
            "Failed to compare merged buffers");
        m_pMetadataUtil->DestroyMetabuffer(hMetaHandle);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCopyDisjoint
    *
    *   @brief
    *       Tests the PFN_CMB_COPY of ChiMetadataOps with disjoint flag set to true
    **************************************************************************************************/
    void ChiMetadataTest::TestCopyDisjoint()
    {
        CHIMETAHANDLE hMetaHandle;
        CDKResult result;

        CHIMETAHANDLE hDefaultMetaHandle = (GetDefaultMetahandle());
        result = m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);
        NT_ASSERT(CDKResultSuccess == result, "Failed to create metabuffer");

        result = m_pMetadataUtil->CopyMetaBuffer(hMetaHandle, hDefaultMetaHandle);
        result = m_pMetadataUtil->DeleteTagFromMetaBuffer(hMetaHandle, ANDROID_COLOR_CORRECTION_TRANSFORM);
        result = m_pMetadataUtil->CopyMetaBufferDisjoint(hMetaHandle, hDefaultMetaHandle);

        NT_ASSERT(CDKResultSuccess == result, "Failed to create a disjoint copy of the metadata");
        NT_ASSERT(true == m_pMetadataUtil->CompareSizeOfTagList(hMetaHandle, hDefaultMetaHandle),
            "Copied buffer has different tag count than the source metadata");

        m_pMetadataUtil->DestroyMetabuffer(hMetaHandle);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCapacity
    *
    *   @brief
    *       Tests the PFN_CMB_CAPACITY of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestCapacity()
    {
        CDKResult result = CDKResultEFailed;
        CHIMETAHANDLE hDefaultMetaHandle = (GetDefaultMetahandle());

        UINT32 pCapacityOfDefaultMetadata = 0;
        result = m_pMetadataUtil->GetTagCapacity(hDefaultMetaHandle, &pCapacityOfDefaultMetadata);
        NT_ASSERT(CDKResultSuccess == result, "");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestGetTagCount
    *
    *   @brief
    *       Tests the PFN_CMB_TAG_COUNT of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestGetTagCount()
    {
        CDKResult result = CDKResultEFailed;
        UINT32 tagCount = 0;
        result = m_pMetadataUtil->GetTagCount(GetDefaultMetahandle(), &tagCount);

        NT_ASSERT(CDKResultSuccess == result, "");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestDump
    *
    *   @brief
    *      Tests the PFN_CMB_DUMP of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestDump()
    {
        CDKResult result = CDKResultEFailed;
        std::string fileName = "MetadataDump.txt";
        CHIMETAHANDLE hDefaultMetadata = (GetDefaultMetahandle());
        result = m_pMetadataUtil->DumpMetaHandle(hDefaultMetadata, fileName.c_str());
        NT_ASSERT(result == CDKResultSuccess, "Failed to dump the metadata");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestPrint
    *
    *   @brief
    *      Tests the PFN_CMB_PRINT of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestPrint()
    {
        CDKResult result = CDKResultEFailed;
        CHIMETAHANDLE hDefaultMetadata = (GetDefaultMetahandle());
        result = m_pMetadataUtil->PrintMetaHandle(hDefaultMetadata);
        NT_ASSERT(result == CDKResultSuccess,"Failed to print the metadata");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestGetMetadataOps
    *
    *   @brief
    *      Tests the metadata ops struct
    **************************************************************************************************/
    void ChiMetadataTest::TestGetMetadataOps()
    {
        CHIMETADATAOPS ops = m_pMetadataUtil->GetMetadataOps();
        NT_ASSERT(false != m_pMetadataUtil->CheckMetadataOps(&ops),"");
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCreateIterator
    *
    *   @brief
    *      Tests the iterator create for metadata
    **************************************************************************************************/
    void ChiMetadataTest::TestIterator()
    {
        CHIMETAHANDLE hDefaultMetaHandle = GetDefaultMetahandle();
        CHIMETADATAITERATOR iterator;
        UINT32 metadataEntryCount = 0;
        CDKResult result;

        result = m_pMetadataUtil->CreateIterator(hDefaultMetaHandle, &iterator);
        NT_ASSERT(CDKResultSuccess == result,"");

        result = m_pMetadataUtil->BeginIterator(iterator);
        NT_ASSERT(CDKResultSuccess == result,"");

        while (result == CDKResultSuccess)
        {
            metadataEntryCount += 1;
            CHIMETADATAENTRY metadataEntry;
            result = m_pMetadataUtil->IteratorGetEntry(iterator, &metadataEntry);
            NT_ASSERT(CDKResultSuccess == result,"");

            result = m_pMetadataUtil->NextIterator(iterator);
        }

        result = m_pMetadataUtil->NextIterator(iterator);
        NT_ASSERT(CDKResultENoMore == result,"");

        UINT32 tagCountOfDefaultMetadata = 0;
        m_pMetadataUtil->GetTagCount(GetDefaultMetahandle(), &tagCountOfDefaultMetadata);
        NT_ASSERT(tagCountOfDefaultMetadata == metadataEntryCount,"");

        result = m_pMetadataUtil->DestroyIterator(iterator);
        NT_ASSERT(CDKResultSuccess == result,"");

        CHIMETAHANDLE hMetaHandle;
        result = m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);

        if (result == CDKResultSuccess)
        {
            result = m_pMetadataUtil->CreateIterator(hMetaHandle, &iterator);
            NT_ASSERT(CDKResultSuccess == result,"");

            result = m_pMetadataUtil->BeginIterator(iterator);
            NT_ASSERT(CDKResultENoMore == result,"");

            result = m_pMetadataUtil->DestroyIterator(iterator);
            NT_ASSERT(CDKResultSuccess == result,"");
        }
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestMetadataReference
    *
    *   @brief
    *      Tests the reference related operations for metadata
    **************************************************************************************************/
    void ChiMetadataTest::TestMetadataReference()
    {
        CHIMETAHANDLE hDefaultMetaHandle = GetDefaultMetahandle();
        CDKResult result;
        CHIMETADATACLIENTID clientID;
        clientID.clientIndex = 1;
        clientID.frameNumber = 0;
        clientID.reserved    = 1;

        UINT32 referenceCount;

        result = m_pMetadataUtil->GetReferenceCount(hDefaultMetaHandle, &referenceCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT(static_cast<UINT32>(0) == referenceCount,"");

        result = m_pMetadataUtil->AddReference(hDefaultMetaHandle, clientID, &referenceCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT(static_cast<UINT32>(1) == referenceCount,"");

        result = m_pMetadataUtil->ReleaseReference(hDefaultMetaHandle, clientID, &referenceCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT(static_cast<UINT32>(0) == referenceCount,"");


        result = m_pMetadataUtil->AddReference(hDefaultMetaHandle, clientID, &referenceCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT(static_cast<UINT32>(1) == referenceCount,"");

        result = m_pMetadataUtil->AddReference(hDefaultMetaHandle, clientID, &referenceCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT(static_cast<UINT32>(2) == referenceCount,"");

        result = m_pMetadataUtil->ReleaseAllReferences(hDefaultMetaHandle, TRUE);
        NT_ASSERT(CDKResultSuccess == result,"");

        result = m_pMetadataUtil->GetReferenceCount(hDefaultMetaHandle, &referenceCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT(static_cast<UINT32>(0) == referenceCount,"");

    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestInvalidate
    *
    *   @brief
    *      Tests the invalidate operations for metadata
    **************************************************************************************************/
    void ChiMetadataTest::TestInvalidate()
    {
        CHIMETAHANDLE hDefaultMetaHandle = GetDefaultMetahandle();
        CDKResult result;

        CHIMETAHANDLE hCopiedMetaHandle;
        result = m_pMetadataUtil->CreateMetabuffer(&hCopiedMetaHandle);
        result = m_pMetadataUtil->CopyMetaBuffer(hDefaultMetaHandle, hCopiedMetaHandle);

        CHIMETAHANDLE hMergedMetaHandle;
        result = m_pMetadataUtil->CreateMetabuffer(&hMergedMetaHandle);
        result = m_pMetadataUtil->MergeMetaBuffer(hDefaultMetaHandle, hMergedMetaHandle);

        result = m_pMetadataUtil->InvalidateTags(hDefaultMetaHandle);
        NT_ASSERT(CDKResultSuccess == result,"");

        UINT32 tagCount;
        result = m_pMetadataUtil->GetTagCount(hDefaultMetaHandle, &tagCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT((UINT32)0== tagCount,"");

        result = m_pMetadataUtil->GetTagCount(hMergedMetaHandle, &tagCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT((UINT32)0== tagCount,"");

        result = m_pMetadataUtil->GetTagCount(hCopiedMetaHandle, &tagCount);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_ASSERT((UINT32)0== tagCount,"");

        m_pMetadataUtil->DestroyMetabuffer(hDefaultMetaHandle);
        m_pMetadataUtil->DestroyMetabuffer(hCopiedMetaHandle);
        m_pMetadataUtil->DestroyMetabuffer(hMergedMetaHandle);

    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestCopyPerformance
    *
    *   @brief
    *      Prints the stats of copy operation of metadata
    **************************************************************************************************/
    void ChiMetadataTest::TestCopyPerformance()
    {
        CDKResult result;
        double averageTime = 0;
        for (int operationCount = 0; operationCount < 100; operationCount++)
        {
            CHIMETAHANDLE hMetaHandleA;
            m_pMetadataUtil->CreateMetabuffer(&hMetaHandleA);
            m_pMetadataUtil->CreateRandomMetadata(&hMetaHandleA);

            CHIMETAHANDLE hMetaHandleB;
            m_pMetadataUtil->CreateMetabuffer(&hMetaHandleB);

            uint64_t start = OsUtils::GetNanoSeconds();
            result = m_pMetadataUtil->CopyMetaBuffer(hMetaHandleB, hMetaHandleA);
            NT_ASSERT(CDKResultSuccess == result,"");

            uint64_t end = OsUtils::GetNanoSeconds();
            double difference = (end - start) / 1000000000.0;

           NT_LOG_DEBUG( "KPI: time for metadata copy operation %d : %lf s", operationCount, difference);
            averageTime += difference;
            result = m_pMetadataUtil->DestroyMetabuffer(hMetaHandleA);
            NT_ASSERT(CDKResultSuccess == result,"");

            result = m_pMetadataUtil->DestroyMetabuffer(hMetaHandleB);
            NT_ASSERT(CDKResultSuccess == result,"");
        }

       NT_LOG_DEBUG( "KPI: average time for metadata copy operation : %lf s", averageTime / 100);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestMergePerformance
    *
    *   @brief
    *      Prints the stats of copy operation of metadata
    **************************************************************************************************/
    void ChiMetadataTest::TestMergePerformance()
    {
        CDKResult result;
        double averageTime = 0;
        for (int operationCount = 0; operationCount < 100; operationCount++)
        {
            CHIMETAHANDLE hMetaHandleA;
            m_pMetadataUtil->CreateRandomMetadata(&hMetaHandleA);

            CHIMETAHANDLE hMetaHandleB;
            result = m_pMetadataUtil->CreateMetabuffer(&hMetaHandleB);
            NT_ASSERT(CDKResultSuccess == result,"");

            uint64_t start = OsUtils::GetNanoSeconds();
            result = m_pMetadataUtil->MergeMetaBuffer(hMetaHandleB, hMetaHandleA);
            NT_ASSERT(CDKResultSuccess == result, "Failed to merge meta buffer");

            uint64_t end = OsUtils::GetNanoSeconds();
            double difference = (end - start) / 1000000000.0;

            NT_LOG_INFO("KPI: time for metadata merge operation %d : %lf s", operationCount, difference);
            averageTime += difference;
            result = m_pMetadataUtil->DestroyMetabuffer(hMetaHandleB);
            NT_ASSERT(CDKResultSuccess == result,"");
        }

       NT_LOG_DEBUG( "KPI: average time for metadata merge operation: %lf s", averageTime / 100);
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestClone
    *
    *   @brief
    *      Tests the PFN_CMB_CLONE of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestClone()
    {
        CHIMETAHANDLE hClonedMetaHandle;
        CDKResult result;

        CHIMETAHANDLE hDefaultMetaHandle = GetDefaultMetahandle();
        result = m_pMetadataUtil->CloneMetaBuffer(hDefaultMetaHandle, &hClonedMetaHandle);

        NT_ASSERT(CDKResultSuccess == result,"Failed to create a clone of the metabuffer");

        UINT32 tagCountInClonedBuffer = 0;
        m_pMetadataUtil->GetTagCount(hClonedMetaHandle, &tagCountInClonedBuffer);

        UINT32 tagCountInDefaultBuffer = 0;
        m_pMetadataUtil->GetTagCount(hDefaultMetaHandle, &tagCountInDefaultBuffer);

        NT_ASSERT(tagCountInDefaultBuffer == tagCountInClonedBuffer,"");

        NT_ASSERT(true == m_pMetadataUtil->CompareMetaBuffers(hDefaultMetaHandle, hClonedMetaHandle),
             "Failed to compare cloned buffers");

        m_pMetadataUtil->DestroyMetabuffer(hClonedMetaHandle);
    }
    /**************************************************************************************************
    *   ChiMetadataTest::TestGetMetadataEntry
    *
    *   @brief
    *      Tests the PFN_CMB_GET_TAG_ENTRY of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestGetMetadataEntry()
    {
        CHIMETAHANDLE hDefaultMetaHandle = GetDefaultMetahandle();
        CHIMETADATAENTRY tagEntry;
        CHIRATIONAL colorTransformMatrix[9];
        m_pMetadataUtil->CreateChiRationalIdentityMatrix(colorTransformMatrix);
        uint32_t tagID = ANDROID_COLOR_CORRECTION_TRANSFORM;

        CDKResult result = m_pMetadataUtil->SetTag(hDefaultMetaHandle, tagID, &colorTransformMatrix[0], 1);
        NT_ASSERT(CDKResultSuccess == result,"Failed to set the tag");

        result = m_pMetadataUtil->GetTagEntry(hDefaultMetaHandle, tagID, &tagEntry);
        NT_ASSERT(CDKResultSuccess == result,"");
        NT_EXPECT(memcmp(colorTransformMatrix, tagEntry.pTagData, sizeof(colorTransformMatrix)) == 0,"");
        CHIMETAHANDLE hMetaHandle;

        m_pMetadataUtil->CreateMetabuffer(&hMetaHandle);
        result = m_pMetadataUtil->GetTagEntry(hMetaHandle, tagID, &tagEntry);
        NT_ASSERT(CDKResultSuccess != result,"");
        m_pMetadataUtil->DestroyMetabuffer(hMetaHandle);
    }
    /**************************************************************************************************
    *   ChiMetadataTest::TestGetMetadataTable
    *
    *   @brief
    *      Tests the PFN_CMB_GET_METADATA_TABLE of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestGetMetadataTable()
    {
        CHIMETAHANDLE hDefaultMetadata = GetDefaultMetahandle();
        NATIVETEST_UNUSED_PARAM(hDefaultMetadata);

        UINT32 countOfTags = 0;
        m_pMetadataUtil->GetMetadataOps().pGetMetadataEntryCount(&countOfTags);
        ChiMetadataEntry* metadataEntry = nullptr;
        metadataEntry = SAFE_NEW() ChiMetadataEntry[countOfTags];

        CDKResult result = m_pMetadataUtil->GetMetadataTable(metadataEntry);
        NT_ASSERT(CDKResultSuccess == result,"");

        delete[] metadataEntry;
        metadataEntry = nullptr;
    }

    /**************************************************************************************************
    *   ChiMetadataTest::TestVendorTagOps
    *
    *   @brief
    *      Tests the vendor tag operations  of ChiMetadataOps
    **************************************************************************************************/
    void ChiMetadataTest::TestVendorTagOps()
    {
        CHIMETAHANDLE hDefaultmetaHandle = GetDefaultMetahandle();
        CDKResult result;
        CHIMETADATAENTRY hMetadataEntry;

        result = m_pMetadataUtil->GetVendorTagEntry(hDefaultmetaHandle, "org.quic.camera2.tuning.feature", "Feature1Mode", &hMetadataEntry);
        NT_ASSERT(CDKResultSuccess == result,"CDKResultError (%d) in GetVendorTagEntry!", result);

        UINT32 tagSize = 0, tagCount = 0;
        tagSize = hMetadataEntry.size;
        tagCount = hMetadataEntry.count;

        UINT8 *tagValue = SAFE_NEW() UINT8[tagSize];
        memset(tagValue, (UINT8)1, tagSize);

        result = m_pMetadataUtil->SetVendorTag(hDefaultmetaHandle, "org.quic.camera2.tuning.feature", "Feature1Mode", tagValue, tagCount);
        NT_ASSERT(CDKResultSuccess == result, "CDKResultError (%d) in SetVendorTag!", result);

        UINT8 *vendorTagValue = NULL;
        result = m_pMetadataUtil->GetVendorTag(hDefaultmetaHandle, "org.quic.camera2.tuning.feature", "Feature1Mode", reinterpret_cast<void**>(&vendorTagValue));
        NT_ASSERT(CDKResultSuccess == result, "CDKResultError (%d) in GetVendorTag!", result);

        NT_ASSERT(memcmp(vendorTagValue, tagValue, tagSize) == 0,"");

        delete[] tagValue;
        tagValue = NULL;
        vendorTagValue = NULL;
    }
}
