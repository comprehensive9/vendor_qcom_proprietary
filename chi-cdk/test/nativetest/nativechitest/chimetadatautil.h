//******************************************************************************************************************************
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************

#ifndef __CHI_METADATAUTIL__
#define __CHI_METADATAUTIL__


#include "chimodule.h"

namespace chitests
{
class ChiMetadataUtil
{
public:
    ChiMetadataUtil() = default;
    ~ChiMetadataUtil() = default;

    static ChiMetadataUtil*  GetInstance();
    static void  DestroyInstance();
    CDKResult ChiModuleInitialize();

    CHIMETADATAOPS GetMetadataOps();

    /*Methods to help test the metadataops*/
    CDKResult CreateMetabuffer(CHIMETAHANDLE* pChiMetaHandle);
    CDKResult DestroyMetabuffer(CHIMETAHANDLE pChiMetaHandle);
    CDKResult CreateMetabufferWithAndroidMetadata(int cameraId, CHIMETAHANDLE* pChiMetaHandle);
    CDKResult CreateMetabufferWithTagList(const UINT32*  pTagList,
        UINT32         tagListCount,
        CHIMETAHANDLE* pMetaHandle);
    void CreateChiRationalIdentityMatrix(CHIRATIONAL* matrix);
        CDKResult SetTag(CHIMETAHANDLE pChiMetaHandle, UINT32 tagID, VOID* tagValue, int count);
    CDKResult GetTag(CHIMETAHANDLE pChiMetaHandle, UINT32 tagId, VOID** tagContent);
    CDKResult GetPipelineinfo(CHIHANDLE hChiContext,
        CHIHANDLE                   hSession,
        CHIPIPELINEDESCRIPTOR       hPipelineDescriptor,
        CHIPIPELINEMETADATAINFO*    pPipelineMetadataInfo);

    CDKResult CreateDefaultMetadata(int cameraId, CHIMETAHANDLE* pChiMetaHandle);

    CDKResult CopyMetaBuffer(CHIMETAHANDLE& pSourceMetaHandle, CHIMETAHANDLE& pDestinationMetaHandle);
    CDKResult CopyMetaBufferDisjoint(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle);
    CDKResult DeleteTagFromMetaBuffer(CHIMETAHANDLE& pSourceMetaHandle, UINT32 tagId);
    CDKResult MergeMetaBuffer(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle);
    CDKResult MergeMetaBufferDisjoint(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle);
    CDKResult CloneMetaBuffer(CHIMETAHANDLE pSourceMetaHandle, CHIMETAHANDLE* pDestinationMetaHandle);
    bool CompareSizeOfTagList(CHIMETAHANDLE& pDestinationMetaHandle, CHIMETAHANDLE& pSourceMetaHandle);
    bool CheckMetadataOps(CHIMETADATAOPS* metadataOps);

    CDKResult VerifyCaptureResultMetadata(CHIMETAHANDLE pResultMetadata);
    CDKResult DumpMetaHandle(CHIMETAHANDLE metadata, const CHAR* fileName);
    int GetTagCount(CHIMETAHANDLE hMetaHandle, UINT32* count);
    CDKResult GetTagEntry(CHIMETAHANDLE hMetaHandle, UINT32 tagId, CHIMETADATAENTRY* tagEntry);
    CDKResult GetTagCapacity(CHIMETAHANDLE hMetaHandle, UINT32* pCapacity);

    CDKResult CreateIterator(CHIMETAHANDLE hMetaHandle, CHIMETADATAITERATOR* iterator);
    CDKResult BeginIterator(CHIMETADATAITERATOR iterator);
    CDKResult IteratorGetEntry(CHIMETADATAITERATOR iterator, CHIMETADATAENTRY* metadataEntry);
    CDKResult NextIterator(CHIMETADATAITERATOR iterator);
    CDKResult DestroyIterator(CHIMETADATAITERATOR iterator);
    CDKResult AddReference(CHIMETAHANDLE hMetahandle, CHIMETADATACLIENTID clientId, UINT32* pCount);
    CDKResult ReleaseReference(CHIMETAHANDLE hMetahandle, CHIMETADATACLIENTID clientId, UINT32* pCount);
    CDKResult ReleaseAllReferences(CHIMETAHANDLE hMetahandle, bool bCHIAndCAMXReferences);
    CDKResult GetReferenceCount(CHIMETAHANDLE hMetahandle, UINT32* pCount);
    CDKResult InvalidateTags(CHIMETAHANDLE hMetaHandle);
    CDKResult CreateRandomMetadata(CHIMETAHANDLE* hMetaHandle);
    CDKResult GetMetadataTable(CHIMETADATAENTRY* hMetadataTable);
    CDKResult GetVendorTag(CHIMETAHANDLE hMetaHandle, const CHAR* sectionName, const CHAR* pTagName, VOID** ppData);
    CDKResult SetVendorTag(CHIMETAHANDLE hMetaHandle, const CHAR* pSectionName, const CHAR* pTagName, const void* pData, UINT32 count);
    CDKResult GetVendorTagEntry(CHIMETAHANDLE hMetaHandle, const CHAR* pSectionName, const CHAR*   pTagName, CHIMETADATAENTRY* pMetadataEntry);
    bool CompareMetaBuffers(CHIMETAHANDLE hDefaultMetaHandle, CHIMETAHANDLE hMetaHandle);
    CDKResult PrintMetaHandle(CHIMETAHANDLE metadata);
    CDKResult CreateInputMetabufferPool(int cameraId, int poolSize);     // Circular buffer to hold input metabuffers
    CDKResult CreateOutputMetabufferPool(int poolSize);     // Circular buffer to hold input metabuffers
    CHIMETAHANDLE GetInputMetabufferFromPool(int index);
    CHIMETAHANDLE GetOutputMetabufferFromPool(int index, UINT32* tagArray, UINT32 tagCount);
    CHIMETAHANDLE GetExistingOutputMetabufferFromPool(int index, bool invalidate);
    CDKResult DestroyMetabufferPools();

        bool CheckAvailableCapability(
            int cameraId,  //[in] camera id
            camera_metadata_enum_android_request_available_capabilities capability) const;  //[in] capability to check
private:
    ChiModule*              m_pChiModule;
    CHIMETADATAOPS          m_pMetadataOps;
    static ChiMetadataUtil* m_pMetadataUtilInstance;
    CHIMETAHANDLE*           m_pInputMetabufferPool;
    CHIMETAHANDLE*           m_pOutputMetabufferPool;
    int                      m_bufferPoolSize;
};
}
#endif