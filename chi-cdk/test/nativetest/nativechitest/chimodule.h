//*************************************************************************************************
// Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __CHI_MODULE__
#define __CHI_MODULE__

#include "nchicommon.h"

//TODO: Remove this once chi.h has this typedef
typedef VOID(*PFNCHIENTRY)(CHICONTEXTOPS* pContextOps);

namespace chitests {

class ChiModule
{
public:

    bool                     m_bFullSizeQCFAExposed = false; //Override Settings
    vendor_tag_ops_t         m_halVendortagOps;      // function pointers for finding/enumerating vendor tags using HAL

    static ChiModule*       GetInstance();
    static void             DestroyInstance();
    int                     GetNumberCameras() const;
    const CHICAMERAINFO*    GetCameraInfo(UINT32 cameraId) const;
    CHISENSORMODEINFO*      GetCameraSensorModeInfo(UINT32 cameraId, UINT32 modeId) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetClosestSensorMode
    ///
    /// @brief Get closest sensor mode based on resolution and framerates
    ///
    /// @param  cameraId        Camera ID
    /// @param  resolution      Resolution to pick sensor mode
    /// @param  frameRate       frame rate to pick sensor mode
    /// @param  fpsExactMatch   flag to request exact match of fps
    ///
    /// @return CHISENSORMODE info
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHISENSORMODEINFO* GetClosestSensorMode(UINT32 cameraId, Size resolution, UINT32 frameRate=0,
        bool fpsExactMatch=false) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetMatchingSensorModes
    ///
    /// @brief  Query the available sensor capabilities for all modes for given camera
    ///
    /// @param  cameraId          Camera ID
    /// @param  userCapList       User requested sensor capability list
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<UINT32> GetMatchingSensorModes(uint32_t cameraId, std::vector<CmdLineParser::SensorCaps> userCapList) const;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiModule::GetContext
    ///
    /// @brief  Gets the chi entry function pointer
    ///
    /// @return PFNCHIENTRY  chi entry function pointer
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PFNCHIENTRY GetChiEntryFunctionPointer() const;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetClosestSensorMode
    ///
    //// @brief Gets pointer to buffer manager ops
    ///
    /// @return CHIBUFFERMANAGEROPS*    pointer to chi buffer manager APIs
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const CHIBUFFERMANAGEROPS*  GetChiBufferManagerOps() const;

    CDKResult               SubmitPipelineRequest(CHIPIPELINEREQUEST* pRequest) const;
    CHIHANDLE               GetContext() const;
    const CHICONTEXTOPS*    GetChiOps() const;
    const CHIFENCEOPS*      GetFenceOps() const;
    VOID*                   GetLibrary() const;
    std::vector <int>       GetCameraList() const;
    CHIMODIFYSETTINGS*      m_pCamXOrSettings = nullptr;
    CHIEXTENDSETTINGS*      m_pCamXExSettings = nullptr;
    CHISENSORMODEINFO**     m_pSelectedSensorInfo;  // pointer to selected sensor mode info

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetChiVendorTagOps
    ///
    /// @brief  vendor tag related functions from CHI API
    /// @param  None
    /// @return CHITAGSOPS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHITAGSOPS GetChiVendorTagOps() const
    {
        return m_chiVendortagOps;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetStaticVendorTagID
    ///
    /// @brief  Query the static capability through given vendor tag and get the tag ID
    /// @param  None
    /// @return UINT32 TagID
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 GetStaticVendorTagID(const char* tagName, const char* sectionName) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetStaticMetadataEntry
    ///
    /// @brief  Get the static vendor tag entry for given camera, tag and section
    /// @param  UINT32                   cameraID
    /// @param  const  char*             tagName
    /// @param  const  char*             sectionName
    /// @return CDKResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult GetStaticMetadataEntry(UINT32 cameraID, const char* tagName, const char* sectionName, camera_metadata_entry_t* tagEntry) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetStaticMetadata
    ///
    /// @brief  Set the ssession level parameter for given camera, tag and section
    /// @param  UINT32                   cameraID
    /// @param  const  char*             tagName
    /// @param  const  char*             sectionName
    /// @param  const  void*             value
    /// @param  const  size_t            dataCount
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult SetStaticMetadata(UINT32 cameraID, const char* tagName, const char* sectionName,
                                        const void* value, size_t dataCount);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetCameraStaticCharacteristics
    ///
    /// @brief  Get the static camera characteristics for given camera ID
    /// @param  UINT32 cameraID
    /// @return camera_metadata_t*
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    camera_metadata_t* GetCameraStaticCharacteristics(UINT32 cameraID) const;

    camera_info* GetLegacyCameraInfo(UINT32 cameraId) const
    {
        return &m_pLegacyCameraInfo[cameraId];
    }

    private:
        std::vector <int>   m_camList;              // list of cameras to test
        static ChiModule*   m_pModuleInstance;      // Singleton instance of ChiModule
        CHICAMERAINFO*      m_pCameraInfo;          // pointer to camera info
        camera_info*        m_pLegacyCameraInfo;    // pointer to legacy camera info
        CHISENSORMODEINFO** m_pSensorInfo;          // pointer to sensor mode info
        int                 m_numOfCameras;         // number of cameras present
        CHIHANDLE           m_hContext;             // Handle to the context
        CHITAGSOPS          m_chiVendortagOps;      // vendor tag related functions from CHI API
        CHIFENCEOPS         m_fenceOps;             // chi fence operations
        CHIBUFFERMANAGEROPS m_chiBufferMgrOps;      // chi buffer manager operations

        void*               hLibrary;               // pointer to the loaded driver dll/.so library
        CHICONTEXTOPS       m_chiOps;               // function pointers to all chi related APIs
        vendor_tag_ops_t    m_vendoTagOps;          // function pointers for finding/enumerating vendor tags
        camera_module_t*    m_pCameraModule;        // pointer to camera module functions
        PFNCHIENTRY         m_pChiHalOpen;          // Chi Entry Pointer

#ifdef ENVIRONMENT64
        const char*         m_libPath = "/vendor/lib64/hw/camera.qcom.so";
#else
        const char*         m_libPath = "/vendor/lib/hw/camera.qcom.so";
#endif
        struct SensorMode
        {
            UINT32 mode;
            UINT32 width;
            UINT32 height;
            UINT32 fps;

            SensorMode(UINT32 mode, UINT32 width, UINT32 height)
            {
                this->mode   = mode;
                this->width  = width;
                this->height = height;
            }
        };

        CDKResult           Initialize();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ChiModule::InitChiEntry
        ///
        /// @brief  Initializes Chi Entry Function pointer
        ///
        /// @return CDKResult result code
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CDKResult                InitChiEntry();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ChiModule::LoadLibraries
        ///
        /// @brief  Load chi and vendor tag operation libraries
        ///
        /// @return CDKResult result code
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CDKResult           LoadLibraries();

        CDKResult           OpenContext();
        CDKResult           CloseContext();

    ChiModule();
    ~ChiModule();
    /// Do not allow the copy constructor or assignment operator
    ChiModule(const ChiModule&) = delete;
    ChiModule& operator= (const ChiModule&) = delete;

};

} //namespace chitests

#endif  //#ifndef __CHI_MODULE__
