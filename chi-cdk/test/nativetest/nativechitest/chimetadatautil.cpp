//******************************************************************************************************************************
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************
//==============================================================================================================================
// IMPLEMENTATION of ChiMetadataUtil class
//==============================================================================================================================

#include "chimetadatautil.h"
#include "chituningmodeparam.h"

namespace chitests
{
ChiMetadataUtil* ChiMetadataUtil::m_pMetadataUtilInstance = nullptr;
/**************************************************************************************************************************
* ChiMetadataUtil::GetInstance
*
* @brief
*     Gets the singleton instance for ChiMetadataUtil
* @param
*     None
* @return
*     ChiModule singleton
**************************************************************************************************************************/
ChiMetadataUtil* ChiMetadataUtil::GetInstance()
{
    if (m_pMetadataUtilInstance == nullptr)
    {
        m_pMetadataUtilInstance = SAFE_NEW() ChiMetadataUtil();

    }
    if (m_pMetadataUtilInstance != nullptr)
    {
        if (m_pMetadataUtilInstance->ChiModuleInitialize() != CDKResultSuccess)
        {
            NT_LOG_ERROR( "Failed to initialize ChiModule!");
            return nullptr;
        }

    }

    return m_pMetadataUtilInstance;
}

/**************************************************************************************************************************
* ChiMetadataUtil::DestroyInstance
*
* @brief
*     Destroy the singleton instance of the ChiMetadataUtil class
* @param
*     None
* @return
*     void
**************************************************************************************************************************/
void ChiMetadataUtil::DestroyInstance()
{
    if (m_pMetadataUtilInstance != nullptr)
    {
        delete m_pMetadataUtilInstance;
        m_pMetadataUtilInstance = nullptr;
    }
}
/**************************************************************************************************************************
* ChiMetadataUtil::ChiModuleInitialize
*
* @brief
*     Gets the ChiModule Instance and the MetadataOps struct.
* @param
*     None
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::ChiModuleInitialize()
{
    if (m_pChiModule == nullptr)
    {
        m_pChiModule = ChiModule::GetInstance();
    }
    if (m_pChiModule != nullptr)
    {

        m_pMetadataUtilInstance->m_pChiModule->GetChiOps()->pMetadataOps(&m_pMetadataOps);
    }

    return (m_pChiModule != nullptr) ? CDKResultSuccess : CDKResultEFailed;
}
/**************************************************************************************************************************
* ChiMetadataUtil::GetMetadataOps
*
* @brief
*     Gets the MetadataOps.
* @param
*     None
* @return
*     CHIMETADATAOPS
**************************************************************************************************************************/
CHIMETADATAOPS ChiMetadataUtil::GetMetadataOps()
{
    return m_pMetadataOps;

}

    /**************************************************************************************************************************
    * ChiMetadataUtil::CreateInputMetabufferPool
    *
    * @brief
    *    Creates a circular buffer pool of Input metabuffers
    * @param
    *     [in] int cameraId                            camera Id for which circular buffer pool has to be created
    *     [in] int poolSize                            number of buffers in the pool
    * @return
    *     CDKResult
    **************************************************************************************************************************/
    CDKResult ChiMetadataUtil::CreateInputMetabufferPool(int cameraId, int poolSize)
    {
        CDKResult result = CDKResultSuccess;
        if (m_pInputMetabufferPool == nullptr)
        {
            m_bufferPoolSize = poolSize;
            m_pInputMetabufferPool = SAFE_NEW() CHIMETAHANDLE[poolSize];
            for (int poolIndex = 0; poolIndex < poolSize; poolIndex++)
            {
                result |= CreateDefaultMetadata(cameraId, &m_pInputMetabufferPool[poolIndex]);
            }
        }
        return result;
    }

/**************************************************************************************************************************
* ChiMetadataUtil::GetInputMetabufferFromPool
*
* @brief
*    Returns a buffer from pool of Input metabuffers
* @param
*     [in] int index                            number of buffers in the pool
* @return
*     CHIMETAHANDLE
**************************************************************************************************************************/
CHIMETAHANDLE ChiMetadataUtil::GetInputMetabufferFromPool(int index)
{
    if (m_pInputMetabufferPool != nullptr)
    {
        return m_pInputMetabufferPool[index % m_bufferPoolSize];
    }
    return nullptr;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateOutputMetabufferPool
*
* @brief
*    Creates a circular buffer pool of Output metabuffers
* @param
*     [in] int poolSize                            number of buffers in the pool
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateOutputMetabufferPool(int poolSize)
{
    CDKResult result = CDKResultSuccess;
    if (m_pOutputMetabufferPool == nullptr)
    {
        m_bufferPoolSize = poolSize;
        m_pOutputMetabufferPool = SAFE_NEW() CHIMETAHANDLE[poolSize];
        for (int poolIndex = 0; poolIndex < poolSize; poolIndex++)
        {
            m_pOutputMetabufferPool[poolIndex] = nullptr;
        }
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetOutputMetabufferFromPool
*
* @brief
*    Returns a buffer from pool of Output metabuffers. Invalidates an existing metabuffer or creates a new one if no
*    existing is found.
* @param
*     [in] int index                            number of buffers in the pool
*     [in] UINT32* tagArray                     list of tags queried from the pipeline
*     [in] UINT32 tagCount                      number of tags
* @return
*     CHIMETAHANDLE
**************************************************************************************************************************/
CHIMETAHANDLE ChiMetadataUtil::GetOutputMetabufferFromPool(int index, UINT32* tagArray, UINT32 tagCount)
{
    if (m_pOutputMetabufferPool != nullptr)
    {
        if (m_pOutputMetabufferPool[index % m_bufferPoolSize] == nullptr)
        {
            CreateMetabufferWithTagList(tagArray, tagCount, &m_pOutputMetabufferPool[index % m_bufferPoolSize]);
        }
        else
        {
            m_pMetadataUtilInstance->InvalidateTags(m_pOutputMetabufferPool[index % m_bufferPoolSize]);
        }
        return m_pOutputMetabufferPool[index % m_bufferPoolSize];
    }
    return nullptr;
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetExistingOutputMetabufferFromPool
*
* @brief
*    Returns an existing buffer from pool of Output metabuffers (does not try to create a new one)
* @param
*     [in] int index                            number of buffers in the pool
*     [in] bool invalidate                      invalidates metabuffer before returning it
* @return
*     CHIMETAHANDLE
**************************************************************************************************************************/
CHIMETAHANDLE ChiMetadataUtil::GetExistingOutputMetabufferFromPool(int index, bool invalidate)
{
    if (m_pOutputMetabufferPool != nullptr && m_pOutputMetabufferPool[index % m_bufferPoolSize] != nullptr)
    {
        if (invalidate)
        {
            m_pMetadataUtilInstance->InvalidateTags(m_pOutputMetabufferPool[index % m_bufferPoolSize]);
        }
        return m_pOutputMetabufferPool[index % m_bufferPoolSize];
    }
    return nullptr;
}

/**************************************************************************************************************************
* ChiMetadataUtil::DestroyMetabufferPools
*
* @brief
*    Destroys the buffer pools of Input and Output metabuffers
* @param
*    None
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::DestroyMetabufferPools()
{
    CDKResult result = CDKResultSuccess;
    if (m_pInputMetabufferPool != nullptr)
    {
        for (int index = 0; index < m_bufferPoolSize; index++)
        {
            result |= DestroyMetabuffer(m_pInputMetabufferPool[index]);
        }

        delete[] m_pInputMetabufferPool;
        m_pInputMetabufferPool = nullptr;
    }
    if (m_pOutputMetabufferPool != nullptr)
    {
        for (int index = 0; index < m_bufferPoolSize; index++)
        {
            if (m_pOutputMetabufferPool[index] != nullptr)
                result |= DestroyMetabuffer(m_pOutputMetabufferPool[index]);
        }

        delete[] m_pOutputMetabufferPool;
        m_pOutputMetabufferPool = nullptr;
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateMetabuffer
*
* @brief
*     Creates the metabuffer using the pCreate function pointer in MetadataOps.
* @param
*     [out] CHIMETAHANDLE* pChiMetaHandle          pointer to the metabuffer that has to be created
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateMetabuffer(CHIMETAHANDLE* pChiMetaHandle)
{
    CDKResult result = CDKResultEFailed;

    if (m_pMetadataUtilInstance != nullptr)
    {
        if (pChiMetaHandle != nullptr)
        {
            return m_pMetadataUtilInstance->GetMetadataOps().pCreate(pChiMetaHandle, NULL);
        }
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::DestroyMetabuffer
*
* @brief
*     Destroy the metabuffer using the pDestroy function pointer in MetadataOps.
* @param
*     [in] CHIMETAHANDLE pChiMetaHandle            metabuffer that has to be destroyed
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::DestroyMetabuffer(CHIMETAHANDLE pChiMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        result = m_pMetadataUtilInstance->m_pMetadataOps.pDestroy(pChiMetaHandle, true);

    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateMetabufferWithAndroidMetadata
*
* @brief
*     Create the metabuffer using the default android metadata
* @param
*     [out] CHIMETAHANDLE* pChiMetaHandle          pointer to the metabuffer that has to be created
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateMetabufferWithAndroidMetadata(int cameraId, CHIMETAHANDLE* pChiMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        CHIMETAHANDLE hAndroidDefaultMetadata;
        result = m_pMetadataUtilInstance->m_pMetadataOps.pGetDefaultAndroidMeta(cameraId, (const VOID**)&hAndroidDefaultMetadata);
        if (result == CDKResultSuccess)
        {
            result |= m_pMetadataUtilInstance->m_pMetadataOps.pCreateWithAndroidMetadata(hAndroidDefaultMetadata, pChiMetaHandle, NULL);
        }
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateMetabufferWithTagList
*
* @brief
*     Create the metabuffer using the default android metadata
* @param
*       [in] const UINT32*   pTagList               list of tags for creating the metabuffer
*       [in] UINT32          tagListCount           count of tags sent
*       [out] CHIMETAHANDLE* pMetaHandle         created metabuffer with the provided tags
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateMetabufferWithTagList(const UINT32*  pTagList,
    UINT32         tagListCount,
    CHIMETAHANDLE* pMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        result = m_pMetadataUtilInstance->m_pMetadataOps.pCreateWithTagArray(pTagList, tagListCount, pMetaHandle, NULL);
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::SetTag
*
* @brief
*     Sets the tag value in the meta buffer sent
* @param
*     [in] CHIMETAHANDLE pChiMetaHandle         metabuffer whose tags are to be set
    *     [in] UINT32        tagID                  tag ID to be set with some value
    *     [in] VOID*         tagValue               the value to set to the tag
    *     [in] int           count                  count of the tag
* @return
*     CDKResult
**************************************************************************************************************************/
    CDKResult ChiMetadataUtil::SetTag(CHIMETAHANDLE pChiMetaHandle, UINT32 tagID, VOID* tagValue, int count)
{
    CDKResult result = CDKResultEFailed;
    CHIMETAHANDLE hAndroidDefaultMetadata;
    NATIVETEST_UNUSED_PARAM(hAndroidDefaultMetadata);

    result = m_pMetadataUtilInstance->m_pMetadataOps.pSetTag(pChiMetaHandle, tagID, tagValue, count);
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetTag
*
* @brief
*     Gets the requested tag value
* @param
*     [in] CHIMETAHANDLE pChiMetaHandle              metabuffer whose tags are requested
*     [in] UINT32 tagId                              tag ID
*     [out] CHIMETAHANDLE* tagContent                Pointer to tag data requested
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetTag(CHIMETAHANDLE pChiMetaHandle, UINT32 tagId, VOID** tagContent)
{
    CDKResult result = CDKResultEFailed;
    result = m_pMetadataUtilInstance->m_pMetadataOps.pGetTag(pChiMetaHandle, tagId, tagContent);
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetPipelineinfo
*
* @brief
*     Queries the pipeline metadata info given the pipeline descriptor
* @param
*       [in] CHIHANDLE  hChiContext                                 chi context
*       [in] CHIHANDLE  hSession                                    session handle
*       [in] CHIPIPELINEDESCRIPTOR hPipelineDescriptor              pipeline handle
*       [out] CHIPIPELINEMETADATAINFO*    pPipelineMetadataInfo     the metadata info requested for the pipeline
*
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetPipelineinfo(CHIHANDLE hChiContext,
    CHIHANDLE                   hSession,
    CHIPIPELINEDESCRIPTOR       hPipelineDescriptor,
    CHIPIPELINEMETADATAINFO*    pPipelineMetadataInfo)
{
    return m_pMetadataUtilInstance->m_pChiModule->GetChiOps()->pQueryPipelineMetadataInfo(hChiContext, hSession,
        hPipelineDescriptor, pPipelineMetadataInfo);
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateDefaultMetadata
*
* @brief
*     Creates the default metadata using the pGetDefaultAndroidMeta in MetadataOps.
* @param
*     [in]  int cameraId                        camera Id
*     [out] CHIMETAHANDLE* pChiMetaHandle       pointer to the metabuffer which contains the requested default metadata
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateDefaultMetadata(int cameraId, CHIMETAHANDLE* pChiMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        if (pChiMetaHandle != nullptr)
        {
            result = m_pMetadataUtilInstance->m_pMetadataOps.pGetDefaultMetadata(cameraId, pChiMetaHandle);
            ChiTuningModeParameter tagValue = { 0 };  // default value
            result |= m_pMetadataUtilInstance->m_pMetadataOps.pSetVendorTag(*pChiMetaHandle,
                "org.quic.camera2.tuning.mode", "TuningMode", &tagValue, sizeof(ChiTuningModeParameter));

            // for offline tests or metadata tests, sensor mode is not required and initialized to nullptr
            if (m_pChiModule->m_pSelectedSensorInfo[cameraId] != nullptr)
            {
                if (m_pChiModule->m_pSelectedSensorInfo[cameraId]->sensorModeCaps.u.QuadCFA == 1 &&
                    m_pChiModule->m_bFullSizeQCFAExposed == false)
                {
                    CHIMETADATAENTRY cropRegion;
                    UINT32 scalerCropRegion[4];

                    m_pMetadataUtilInstance->m_pMetadataOps.pGetTagEntry(*pChiMetaHandle, ANDROID_SCALER_CROP_REGION,
                        &cropRegion);

                    scalerCropRegion[0] = m_pChiModule->m_pSelectedSensorInfo[cameraId]->activeArrayCropWindow.left;
                    scalerCropRegion[1] = m_pChiModule->m_pSelectedSensorInfo[cameraId]->activeArrayCropWindow.top;
                    scalerCropRegion[2] = m_pChiModule->m_pSelectedSensorInfo[cameraId]->activeArrayCropWindow.width;
                    scalerCropRegion[3] = m_pChiModule->m_pSelectedSensorInfo[cameraId]->activeArrayCropWindow.height;

                    // in case of QCFA, driver publishes only Quarter size to framework layer, but internally pipeline wants to
                    // run in full mode. So reset the Android metadata to active array pixel size when QCFA sensor mode is
                    // selected and if fullsizeQCFAexposed is set to false

                    // TODO: add crop size matching logic in case of zoom, as it is not a direct mapping,
                    // TODO: zoom logic is width = active_array_width - active_array_width/zoom_crop_factor
                    // TODO: as our metadata is remaining constant, we should avoid creating default metadata multiple times,
                    // TODO: instead generate once, copy and re-use
                    cropRegion.pTagData = &scalerCropRegion[0];

                    result |= m_pMetadataUtilInstance->m_pMetadataOps.pSetTag(*pChiMetaHandle, ANDROID_SCALER_CROP_REGION,
                        cropRegion.pTagData, cropRegion.count);
                }
            }
        }
    }

    if (result != CDKResultSuccess)
    {
        NT_LOG_ERROR( "Failed to create default metadata!");
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CopyMetaBuffer
*
* @brief
*     Creates the copy of metadata in the pSourceMetaHandle sent.
* @param
*     [in] CHIMETAHANDLE& pSourceMetaHandle           source metabuffer whose data is copied
*     [out] CHIMETAHANDLE& pDestinationMetaHandle     destination metabuffer which contains the copied metadata
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CopyMetaBuffer(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        result = m_pMetadataUtilInstance->m_pMetadataOps.pCopy(pDestinationMetaHandle, pSourceMetaHandle, false);

    }
    return result;

}
/**************************************************************************************************************************
* ChiMetadataUtil::CopyMetaBufferDisjoint
*
* @brief
*     Creates the disjoint copy of metadata in the pSourceMetaHandle sent.
* @param
*    [in] CHIMETAHANDLE& pSourceMetaHandle           source metabuffer whose data is copied
*    [out] CHIMETAHANDLE& pDestinationMetaHandle     destination metabuffer which contains the copied metadata
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CopyMetaBufferDisjoint(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        return m_pMetadataUtilInstance->m_pMetadataOps.pCopy(pDestinationMetaHandle, pSourceMetaHandle, true);

    }
    return result;

}

/**************************************************************************************************************************
* ChiMetadataUtil::DeleteTagFromMetaBuffer
*
* @brief
*    Deletes a tag from the metadata buffer sent.
* @param
*     [in] CHIMETAHANDLE& pSourceMetaHandle         metabuffer whose tag needs to be deleted
*     [in] UINT32 tagId                             tag id for the tag to be deleted
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::DeleteTagFromMetaBuffer(CHIMETAHANDLE& pSourceMetaHandle, UINT32 tagId)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        result = m_pMetadataUtilInstance->m_pMetadataOps.pDeleteTag(pSourceMetaHandle, tagId);
    }
    return result;

}
/**************************************************************************************************************************
* ChiMetadataUtil::MergeMetaBuffer
*
* @brief
*     Performs a merge of the source and destination metadata handles.
* @param
*    [in] CHIMETAHANDLE& pSourceMetaHandle           source metabuffer whose data is merged
*    [out] CHIMETAHANDLE& pDestinationMetaHandle     destination metabuffer which contains the merged metadata
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::MergeMetaBuffer(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        return m_pMetadataUtilInstance->m_pMetadataOps.pMerge(pDestinationMetaHandle, pSourceMetaHandle, false);

    }
    return result;

}
/**************************************************************************************************************************
* ChiMetadataUtil::MergeMetaBufferDisjoint
*
* @brief
*     Performs a dijoint merge of the source and destination metadata handles.
* @param
*      [in] CHIMETAHANDLE& pSourceMetaHandle           source metabuffer whose data is merged
*      [out] CHIMETAHANDLE& pDestinationMetaHandle     destination metabuffer which contains the merged metadata
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::MergeMetaBufferDisjoint(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        result = m_pMetadataUtilInstance->m_pMetadataOps.pMerge(pDestinationMetaHandle, pSourceMetaHandle, true);

    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CloneMetaBuffer
*
* @brief
*     Creates the clone of metadata in the pSourceMetaHandle sent.
* @param
*      [in] CHIMETAHANDLE& pSourceMetaHandle           source metabuffer whose data is to be cloned
*      [out] CHIMETAHANDLE* pDestinationMetaHandle     pointer to destination metabuffer which contains the cloned metadata
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CloneMetaBuffer(CHIMETAHANDLE pSourceMetaHandle, CHIMETAHANDLE* pDestinationMetaHandle)
{
    CDKResult result = CDKResultEFailed;
    if (m_pMetadataUtilInstance != nullptr)
    {
        result = m_pMetadataUtilInstance->m_pMetadataOps.pClone(pSourceMetaHandle, pDestinationMetaHandle);

    }
    return result;

}

/**************************************************************************************************************************
* ChiMetadataUtil::CompareSizeOfTagList
*
* @brief
*     Compares the tag count in source and destination handles and returns true if equal.
* @param
*     [in] CHIMETAHANDLE& pSourceMetaHandle             source metadata
*     [in] CHIMETAHANDLE& pDestinationMetaHandle        destination metadata
* @return
*     bool
**************************************************************************************************************************/
bool ChiMetadataUtil::CompareSizeOfTagList(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle)
{
    if (m_pMetadataUtilInstance != nullptr)
    {
        CDKResult result;
        UINT32 countOfTagsInDestinationHandle = 0;
        result = m_pMetadataUtilInstance->m_pMetadataOps.pCount(pDestinationMetaHandle, &countOfTagsInDestinationHandle);
        UINT32 countOfTagsInSourceHandle = 0;
        result |= m_pMetadataUtilInstance->m_pMetadataOps.pCount(pSourceMetaHandle, &countOfTagsInSourceHandle);
        if (result == CDKResultSuccess)
        {
            return countOfTagsInDestinationHandle == countOfTagsInSourceHandle;
        }
    }
    return false;

}

/**************************************************************************************************************************
* ChiMetadataUtil::VerifyCaptureResultMetadata
*
* @brief
*     Verifies the metadata in the captured result
* @param
*     [in] CHIMETAHANDLE& pResultMetadata                   result metadata buffer
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::VerifyCaptureResultMetadata(CHIMETAHANDLE pResultMetadata)

{
    CDKResult result = CDKResultSuccess;
    if (m_pMetadataUtilInstance != nullptr)
    {
        std::string sfileName = "ResultMetadata.txt";
        result = m_pMetadataUtilInstance->DumpMetaHandle(pResultMetadata, sfileName.c_str());
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateChiRationalIdentityMatrix
*
* @brief
*     Creates an identity Matrix which can be used for setting any data of type ChiRational
* @param
*     [out] CHIRATIONAL* matrix             pointer to the matrix that had be filled (1X9 mmatrix whose memory needs to be
allocated by the callee)
* @return
*     void
**************************************************************************************************************************/
void ChiMetadataUtil::CreateChiRationalIdentityMatrix(CHIRATIONAL* matrix)
{
    CHIRATIONAL zero = { 0, 1 };
    CHIRATIONAL one = { 1, 1 };
    for (int i = 0; i < 9; i++)
    {

        matrix[i] = ((i == 0) || (i == 4) || (i == 8)) ? one : zero;

    }
}

/**************************************************************************************************************************
* ChiMetadataUtil::CheckMetadataOps
*
* @brief
*     Checks if the metadataops struct has no null function pointers
* @param
*     [in] CHIMETADATAOPS* metadataOps          pointer to the metadata ops structure
* @return
*     bool
**************************************************************************************************************************/
bool ChiMetadataUtil::CheckMetadataOps(CHIMETADATAOPS* metadataOps)
{
    bool result = false;
    if (metadataOps->pAddReference != nullptr &&
        metadataOps->pCapacity != nullptr &&
        metadataOps->pClone != nullptr &&
        metadataOps->pCopy != nullptr &&
        metadataOps->pCount != nullptr &&
        metadataOps->pCreate != nullptr &&
        metadataOps->pCreateWithAndroidMetadata != nullptr &&
        metadataOps->pCreateWithTagArray != nullptr &&
        metadataOps->pDeleteTag != nullptr &&
        metadataOps->pDestroy != nullptr &&
        metadataOps->pDump != nullptr &&
        metadataOps->pFilter != nullptr &&
        metadataOps->pGetDefaultAndroidMeta != nullptr &&
        metadataOps->pGetDefaultMetadata != nullptr &&
        metadataOps->pGetMetadataEntryCount != nullptr &&
        metadataOps->pGetMetadataTable != nullptr &&
        metadataOps->pGetPrivateData != nullptr &&
        metadataOps->pGetTag != nullptr &&
        metadataOps->pGetTagEntry != nullptr &&
        metadataOps->pGetVendorTag != nullptr &&
        metadataOps->pGetVendorTagEntry != nullptr &&
        metadataOps->pInvalidate != nullptr &&
        metadataOps->pIteratorBegin != nullptr &&
        metadataOps->pIteratorCreate != nullptr &&
        metadataOps->pIteratorDestroy != nullptr &&
        metadataOps->pIteratorGetEntry != nullptr &&
        metadataOps->pIteratorNext != nullptr &&
        metadataOps->pMerge != nullptr &&
        metadataOps->pPrint != nullptr &&
        metadataOps->pReferenceCount != nullptr &&
        metadataOps->pReleaseReference != nullptr &&
        metadataOps->pSetAndroidMetadata != nullptr &&
        metadataOps->pSetVendorTag != nullptr &&
        metadataOps->pSetTag != nullptr &&
        metadataOps->pReleaseAllReferences != nullptr
        )
    {
        result = true;
    }
    return result;
}

/**************************************************************************************************************************
* ChiMetadataUtil::DumpMetaHandle
*
* @brief
*    Dumps the handle data into a file
* @param
*     [in] CHIMETAHANDLE metadata               metadata buffer to be dumped
*     [in] const CHAR* fileName                 file name for the dump
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::DumpMetaHandle(CHIMETAHANDLE metadata, const CHAR* fileName)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pDump(metadata, fileName);
}
/**************************************************************************************************************************
* ChiMetadataUtil::PrintMetaHandle
*
* @brief
*    Dumps the handle data into a file
* @param
*     [in] CHIMETAHANDLE metadata               metadata buffer to be printed
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::PrintMetaHandle(CHIMETAHANDLE metadata)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pPrint(metadata);
}
/**************************************************************************************************************************
* ChiMetadataUtil::GetTagCount
*
* @brief
*    Gets the count of tags in a metahandle
* @param
*     [in]  CHIMETAHANDLE metadata                  input metadata buffer
*     [out] UINT32* count                           pointer to the variable that holds count
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetTagCount(CHIMETAHANDLE hMetaHandle, UINT32* count)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pCount(hMetaHandle, count);
}
/**************************************************************************************************************************
* ChiMetadataUtil::GetTagEntry
*
* @brief
*    Gets the count of tags in a metahandle
* @param
*     [in]  CHIMETAHANDLE metadata                  input metadata buffer
*     [in]  UINT32 tagID                            tag ID
*     [out] CHIMETADATAENTRY* tagEntry              pointer to the tag Entry
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetTagEntry(CHIMETAHANDLE hMetaHandle, UINT32 tagID, CHIMETADATAENTRY* tagEntry)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pGetTagEntry(hMetaHandle, tagID, tagEntry);
}
/**************************************************************************************************************************
* ChiMetadataUtil::GetTagCapacity
*
* @brief
*    Gets the capacity of a metahandle
* @param
*     [in] CHIMETAHANDLE metadata                   input metadata buffer
*     [out] UINT32* pCapacity                       pointer to the variable that holds capacity
* @return
*     CDKResult

**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetTagCapacity(CHIMETAHANDLE hMetaHandle, UINT32* pCapacity)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pCapacity(hMetaHandle, pCapacity);
}

    /**************************************************************************************************************************
    * ChiMetadataUtil::CheckAvailableCapability
    * @brief
    *     Checks if given android capability is supported
    * @return
    *    True if supported else false
    ***************************************************************************************************************************/
    bool ChiMetadataUtil::CheckAvailableCapability(
        int cameraId,  //[in] camera id
        camera_metadata_enum_android_request_available_capabilities capability) const  //[in] capability to check
    {
        CDKResult result = CDKResultEFailed;
        camera_metadata_tag_t tagID = ANDROID_REQUEST_AVAILABLE_CAPABILITIES;
        camera_metadata_entry_t tagEntry;

        camera_metadata_t* chiMetadata = const_cast<camera_metadata_t*>((m_pChiModule->GetLegacyCameraInfo(cameraId)->static_camera_characteristics));

        result = Camera3Metadata::GetCameraMetadataEntryByTag(chiMetadata, tagID, &tagEntry);

        if (CDKResultSuccess != result)
        {
            NT_LOG_UNSUPP("Can't find the metadata entry for ANDROID_REQUEST_AVAILABLE_CAPABILITIES.");
            return false;
        }

        for (size_t i = 0; i < tagEntry.count; i++)
        {
            if (tagEntry.data.u8[i] == capability)
            {
                return true;
            }

        }
        return false;
    }

/**************************************************************************************************************************
* ChiMetadataUtil::CreateIterator
*
* @brief
*    Creates the iterator for a metahandle
* @param
*     [in]  CHIMETAHANDLE metadata                  input metadata buffer
*     [out] CHIMETADATAITERATOR*  iterator          pointer to the iterator variable which is created
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateIterator(CHIMETAHANDLE hMetaHandle, CHIMETADATAITERATOR* iterator)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pIteratorCreate(hMetaHandle, iterator);
}

/**************************************************************************************************************************
* ChiMetadataUtil::BeginIterator
*
* @brief
*    Begins the iterator for a metahandle
* @param
*     [in] CHIMETADATAITERATOR  iterator            iterator
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::BeginIterator(CHIMETADATAITERATOR iterator)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pIteratorBegin(iterator);
}

/**************************************************************************************************************************
* ChiMetadataUtil::IteratorGetEntry
*
* @brief
*    Gets the metadata entry the iterator points to
* @param
*      [in]  CHIMETADATAITERATOR iterator           iterator
*      [out] CHIMETADATAENTRY* metadataEntry        pointer to the metadataentry structure that needs to be filled.
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::IteratorGetEntry(CHIMETADATAITERATOR iterator, CHIMETADATAENTRY* metadataEntry)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pIteratorGetEntry(iterator, metadataEntry);
}

/**************************************************************************************************************************
* ChiMetadataUtil::NextIterator
*
* @brief
*    Points to the next entry in the iterator
* @param
*     [in]  CHIMETADATAITERATOR iterator            iterator
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::NextIterator(CHIMETADATAITERATOR iterator)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pIteratorNext(iterator);
}

/**************************************************************************************************************************
* ChiMetadataUtil::DestroyIterator
*
* @brief
*    Destroys the iterator
* @param
*     [in]  CHIMETADATAITERATOR iterator             iterator
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::DestroyIterator(CHIMETADATAITERATOR iterator)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pIteratorDestroy(iterator);
}

/**************************************************************************************************************************
* ChiMetadataUtil::AddReference
*
* @brief
*    Adds the reference to the metabuffer
* @param
*     [in]  CHIMETAHANDLE hMetahandle               input metadata buffer
*     [in]  CHIMETADATACLIENTID clientId            client Id associated with it
*     [out] UINT32* pCount                          pointer to the variable for reference count
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::AddReference(CHIMETAHANDLE hMetahandle, CHIMETADATACLIENTID clientId, UINT32* pCount)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pAddReference(hMetahandle, clientId, pCount);

}

/**************************************************************************************************************************
* ChiMetadataUtil::ReleaseReference
*
* @brief
*    Releases the reference to the metabuffer
* @param
*     [in]  CHIMETAHANDLE hMetahandle               input metadata buffer
*     [in]  CHIMETADATACLIENTID clientId            client Id associated with it
*     [out] UINT32* pCount                          pointer to the variable for reference count
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::ReleaseReference(CHIMETAHANDLE hMetahandle, CHIMETADATACLIENTID clientId, UINT32* pCount)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pReleaseReference(hMetahandle, clientId, pCount);
}

/**************************************************************************************************************************
* ChiMetadataUtil::ReleaseAllReferences
*
* @brief
*    Releases all the references from the metabuffer
* @param
*     [in]  CHIMETAHANDLE hMetahandle               input metadata buffer
*     [in]  bool          bCHIAndCAMXReferences     Flag to indicate whether to remove only the CHI references or both CAMX and CHI
*                                                   references. This flag should be set to TRUE only for Flush or Teardown cases
* @return
*     CDKResult if successful CDK error values otherwise
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::ReleaseAllReferences(CHIMETAHANDLE hMetahandle, bool bCHIAndCAMXReferences)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pReleaseAllReferences(hMetahandle, bCHIAndCAMXReferences);
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetReferenceCount
*
* @brief
*    Gets the reference count for the metabuffer
* @param
*     [in]  CHIMETAHANDLE hMetahandle               input metadata buffer
*     [out] UINT32*	pCount                          pointer to the variable for reference count
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetReferenceCount(CHIMETAHANDLE hMetahandle, UINT32* pCount)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pReferenceCount(hMetahandle, pCount);
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetMetadataTable
*
* @brief
*    Gets the metadata Table
* @param
*   [out] CHIMETADATAENTRY* hMetadataTable            pointer to the array of metadata entry structure that's filled.
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetMetadataTable(CHIMETADATAENTRY* hMetadataTable)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pGetMetadataTable(hMetadataTable);
}

/**************************************************************************************************************************
* ChiMetadataUtil::InvalidateTags
*
* @brief
*    Invalidates tags for the metadata
* @param
*    [in] CHIMETAHANDLE hMetaHandle                    input metadata buffer
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::InvalidateTags(CHIMETAHANDLE hMetaHandle)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pInvalidate(hMetaHandle);
}

/**************************************************************************************************************************
* ChiMetadataUtil::CreateRandomMetadata
*
* @brief
*    Creates random values for the metadata
* @param
*    [out] CHIMETAHANDLE*   hMetaHandle                 pointer to the metadata buffer that has to be created
* @return
*     void
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::CreateRandomMetadata(CHIMETAHANDLE* hMetaHandle)
{
    CHIMETADATAENTRY* metadataEntry = nullptr;
    UINT32 pCount = 0;
    CDKResult result = m_pMetadataUtilInstance->GetMetadataOps().pGetMetadataEntryCount(&pCount);

    metadataEntry = SAFE_NEW() CHIMETADATAENTRY[pCount];

    result = m_pMetadataUtilInstance->GetMetadataTable(metadataEntry);
    if (CDKResultSuccess != result)
    {
        NT_LOG_ERROR("GetMetadataTable failed");
        return CDKResultEFailed;
    }

    UINT32* tagValues = SAFE_NEW() UINT32[pCount];
    for (UINT32 i = 0; i < pCount; i++)
    {
        tagValues[i] = metadataEntry[i].tagID;
    }
    m_pMetadataUtilInstance->CreateMetabufferWithTagList(tagValues, pCount, hMetaHandle);

    UINT8* tagData = nullptr;
    for (UINT32 i = 0; i < pCount; i++)
    {
        CHIMETADATAENTRY tag = metadataEntry[i];
        tagData = static_cast<UINT8*>(malloc(tag.size / sizeof(UINT8)));
        if (nullptr == tagData) {
            NT_LOG_ERROR("memory allocation failed during metadata creation");
            return CDKResultEFailed;
        }
        for (UINT32 index = 0; index < tag.size / sizeof(UINT8); index++)
        {
            tagData[index] = static_cast<UINT8>(rand() % 256);
        }
        result = m_pMetadataUtilInstance->GetMetadataOps().pSetTag(*hMetaHandle, tag.tagID, tagData, tag.count);
        free(tagData);
        tagData = nullptr;
    }
    delete[] tagValues;
    delete[] metadataEntry;

    tagValues = nullptr;
    metadataEntry = nullptr;

    return CDKResultSuccess;
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetVendorTag
*
* @brief
*    Gets the vendor tag requested
* @param
*   [in] CHIMETAHANDLE hMetaHandle                       input metadata buffer
*   [in] const CHAR* sectionName                         section name of the vendor tag
*   [in] const CHAR* pTagName                            vendor tag name
*   [out] VOID** ppData                                  pointer to the data that is filled with vendor tag details
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetVendorTag(CHIMETAHANDLE hMetaHandle, const CHAR* sectionName, const CHAR* pTagName, VOID** ppData)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pGetVendorTag(hMetaHandle, sectionName, pTagName, ppData);
}

/**************************************************************************************************************************
* ChiMetadataUtil::SetVendorTag
*
* @brief
*    Sets the vendor tag value specified.
* @param
*   [in] CHIMETAHANDLE* hMetaHandle                      input metadata buffer
*   [in] const CHAR* pSectionName                        section name of the vendor tag
*   [in] const CHAR* pTagName                            vendor tag name
*   [in] const void* pData                               pointer to the data
*   [in] UINT32 count                                    tag count (see camxtitan17xcontext.cpp for what to use)
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::SetVendorTag(CHIMETAHANDLE hMetaHandle, const CHAR* pSectionName, const CHAR* pTagName, const void* pData, UINT32 count)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pSetVendorTag(hMetaHandle, pSectionName, pTagName, pData, count);
}

/**************************************************************************************************************************
* ChiMetadataUtil::GetVendorTagEntry
*
* @brief
*    Sets the vendor tag value specified.
* @param
*   [in] CHIMETAHANDLE hMetaHandle                      input metadata buffer
*   [in] const CHAR* pSectionName                       section name of the vendor tag
*   [in] const CHAR* pTagName                           vendor tag name
*   [out] CHIMETADATAENTRY* pMetadataEntry              pointer to the metadata entry structure that has requested tag details
* @return
*     CDKResult
**************************************************************************************************************************/
CDKResult ChiMetadataUtil::GetVendorTagEntry(CHIMETAHANDLE hMetaHandle, const CHAR* pSectionName, const CHAR*   pTagName, CHIMETADATAENTRY* pMetadataEntry)
{
    return m_pMetadataUtilInstance->GetMetadataOps().pGetVendorTagEntry(hMetaHandle, pSectionName, pTagName, pMetadataEntry);
}

/**************************************************************************************************************************
* ChiMetadataUtil::CompareMetaBuffers
*
* @brief
*   Compares one metadata buffer with another metadata.
* @param
*   [in] CHIMETAHANDLE hMetaHandle                      destination metadata buffer
*   [in] CHIMETAHANDLE hDefaultMetaHandle               source metadata buffer
* @return
*     bool
**************************************************************************************************************************/
bool ChiMetadataUtil::CompareMetaBuffers(CHIMETAHANDLE hDefaultMetaHandle, CHIMETAHANDLE hMetaHandle)
{
    CHIMETADATAITERATOR iterator;

    CDKResult result = CreateIterator(hDefaultMetaHandle, &iterator);

    result |= BeginIterator(iterator);

    while (result == CDKResultSuccess)
    {
        CHIMETADATAENTRY metadataEntryA;
        CHIMETADATAENTRY metadataEntryB;
        result = IteratorGetEntry(iterator, &metadataEntryA);
        result |= GetMetadataOps().pGetTagEntry(hMetaHandle, metadataEntryA.tagID, &metadataEntryB);
        if (result == CDKResultSuccess)
        {
            if ((metadataEntryA.tagID == metadataEntryB.tagID) && (0 != memcmp(metadataEntryA.pTagData, metadataEntryB.pTagData, metadataEntryA.size)))
            {
                return false;
            }
        }
        result = NextIterator(iterator);
    }
    result = DestroyIterator(iterator);
    return result == CDKResultSuccess;
}
}
