//******************************************************************************************************************************
// Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//******************************************************************************************************************************
//==============================================================================================================================
// IMPLEMENTATION of ChiModule
//==============================================================================================================================

#include "chimodule.h"
#include "chxutils.h"

namespace chitests {

///@brief ChiModule singleton
ChiModule* ChiModule::m_pModuleInstance = nullptr;

/***************************************************************************************************************************
* ChiModule::GetInstance
*
* @brief
*     Gets the singleton instance for ChiModule
* @param
*     None
* @return
*     ChiModule singleton
***************************************************************************************************************************/
ChiModule* ChiModule::GetInstance()
{
    if (m_pModuleInstance == nullptr)
    {
        m_pModuleInstance = SAFE_NEW() ChiModule();
        if (m_pModuleInstance->Initialize() != CDKResultSuccess)
        {
            NT_LOG_ERROR("Failed to initialize ChiModule singleton!");
            return nullptr;
        }
    }

    return m_pModuleInstance;
}

/***************************************************************************************************************************
* ChiModule::DestroyInstance
*
* @brief
*     Destroy the singleton instance of the ChiModule class
* @param
*     None
* @return
*     void
***************************************************************************************************************************/
void ChiModule::DestroyInstance()
{
    if (m_pModuleInstance != nullptr)
    {
        delete m_pModuleInstance;
        m_pModuleInstance = nullptr;
    }
}

/***************************************************************************************************************************
* ChiModule::ChiModule
*
* @brief
*     Constructor for ChiModule
***************************************************************************************************************************/
ChiModule::ChiModule() :
    m_camList(),
    m_pCameraInfo(nullptr),
    m_pLegacyCameraInfo(nullptr),
    m_pSensorInfo(nullptr),
    m_numOfCameras(0),
    m_hContext(nullptr),
    m_chiVendortagOps{},
    m_fenceOps{},
    m_chiBufferMgrOps{},
    hLibrary(nullptr),
    m_chiOps{},
    m_pCameraModule(nullptr)
{
}

/***************************************************************************************************************************
* ChiModule::~ChiModule
*
* @brief
*     Destructor for ChiModule
***************************************************************************************************************************/
ChiModule::~ChiModule()
{
    if (m_hContext)
    {
        if (CloseContext() != CDKResultSuccess)
        {
            NT_LOG_ERROR("Failed to close camera context!");
        }
    }

#ifndef _LINUX
    // free the library
    if (nullptr != hLibrary)
    {
        if (!FreeLibrary(static_cast<HMODULE>(hLibrary)))
        {
            NT_LOG_ERROR("Failed to free library handle");
        }
    }
#endif

    for (int currCamera = 0; currCamera < m_numOfCameras; currCamera++)
    {
        if (m_pSensorInfo[currCamera])
        {
            delete[] m_pSensorInfo[currCamera];
            m_pSensorInfo[currCamera] = nullptr;
        }
    }

    if (m_pLegacyCameraInfo)
    {
        delete[] m_pLegacyCameraInfo;
        m_pLegacyCameraInfo = nullptr;
    }

    if (m_pSensorInfo)
    {
        delete[] m_pSensorInfo;
        m_pSensorInfo = nullptr;
    }

    if (m_pCameraInfo)
    {
        delete[] m_pCameraInfo;
        m_pCameraInfo = nullptr;
    }

    if (m_pCamXExSettings)
    {
        m_pCamXExSettings = nullptr;
    }

    if (m_pCamXOrSettings)
    {
        m_pCamXOrSettings = nullptr;
    }
}

/***************************************************************************************************************************
* ChiModule::Initialize
*
* @brief
*     Initialize member variables using driver calls
* @param
*     None
* @return
*     CDKResult result code
***************************************************************************************************************************/
CDKResult ChiModule::Initialize()
{
    CDKResult result = CDKResultSuccess;

    result = LoadLibraries();
    if (result != CDKResultSuccess)
    {
        return result;
    }
    result = OpenContext();
    if (result != CDKResultSuccess)
    {
        return result;
    }

    m_numOfCameras = m_chiOps.pGetNumCameras(m_hContext);

    NT_LOG_INFO("Number of cameras reported by device: %d", m_numOfCameras);

    m_pCameraInfo         = SAFE_NEW() CHICAMERAINFO[m_numOfCameras];
    m_pLegacyCameraInfo   = SAFE_NEW() camera_info[m_numOfCameras];
    m_pSensorInfo         = SAFE_NEW() CHISENSORMODEINFO*[m_numOfCameras];
    m_pSelectedSensorInfo = SAFE_NEW() CHISENSORMODEINFO*[m_numOfCameras];

    for (int currCamera = 0; currCamera < m_numOfCameras; currCamera++)
    {
        m_pCameraInfo[currCamera].pLegacy = &m_pLegacyCameraInfo[currCamera];

        m_chiOps.pGetCameraInfo(m_hContext, currCamera, &m_pCameraInfo[currCamera]);

        UINT32 numSensorModes = m_pCameraInfo[currCamera].numSensorModes;

        m_pSensorInfo[currCamera] = SAFE_NEW() CHISENSORMODEINFO[numSensorModes];

        m_chiOps.pEnumerateSensorModes(m_hContext, currCamera, numSensorModes, m_pSensorInfo[currCamera]);

        NT_LOG_DEBUG("Listing available sensor modes for camera %d...", currCamera)

        for (UINT32 sensorMode = 0; sensorMode < numSensorModes; sensorMode++)
        {
            NT_LOG_DEBUG("Camera %d sensormode %u: width %u, height %u, framerate %u, bpp %u",
                currCamera,
                sensorMode,
                m_pSensorInfo[currCamera][sensorMode].frameDimension.width,
                m_pSensorInfo[currCamera][sensorMode].frameDimension.height,
                m_pSensorInfo[currCamera][sensorMode].frameRate,
                m_pSensorInfo[currCamera][sensorMode].bpp,
                m_pSensorInfo[currCamera][sensorMode].sensorModeCaps.u.Normal,
                m_pSensorInfo[currCamera][sensorMode].sensorModeCaps.u.PDAF);
        }
    }

    // Check if --camera flag set
    UINT32 testCameraID = CmdLineParser::GetCamera();
    // If camera ID within range, add to test list
    if (testCameraID >= 0 && testCameraID < static_cast<UINT32>(m_numOfCameras))
    {
        m_camList.push_back(testCameraID);
    }
    else // Else add all available cameras for test
    {
        for (UINT32 currCamera = 0; currCamera < static_cast<UINT32>(m_numOfCameras); currCamera++)
        {
            m_camList.push_back(currCamera);
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiModule::InitChiEntry
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiModule::InitChiEntry()
{

    const CHAR* pDlErrorMsg;

#ifdef _LINUX
    hLibrary = dlopen(m_libPath, RTLD_NOW);
    if (hLibrary == NULL)
    {
        pDlErrorMsg = dlerror();
        NT_LOG_ERROR( "Failed to load android library %s", pDlErrorMsg ? pDlErrorMsg : "N/A");
        return CDKResultEUnableToLoad;
    }
    m_pChiHalOpen = reinterpret_cast<PFNCHIENTRY>(dlsym(hLibrary, "ChiEntry"));
    if (m_pChiHalOpen == NULL)
    {
        NT_LOG_ERROR( "ChiEntry missing in library");
        return CDKResultEUnableToLoad;
    }
    m_pCameraModule = reinterpret_cast<camera_module_t*>(dlsym(hLibrary, "HMI"));
    if (m_pCameraModule == NULL)
    {
        NT_LOG_ERROR( "CameraModule missing in library");
        return CDKResultEUnableToLoad;
    }
#else
    hLibrary = LoadLibrary("hal.dll");
    if (hLibrary == NULL)
    {
        NT_LOG_ERROR("Failed to load hal library");
        return CDKResultEUnableToLoad;
    }
    void *hmi = GetProcAddress(static_cast<HMODULE>(hLibrary), "ChiEntry");
    m_pChiHalOpen = reinterpret_cast<PFNCHIENTRY>(hmi);
    if (m_pChiHalOpen == NULL)
    {
        NT_LOG_ERROR("ChiEntry missing in library");
        return CDKResultEUnableToLoad;
    }
    hmi = GetProcAddress(static_cast<HMODULE>(hLibrary), "HMI");
    m_pCameraModule = reinterpret_cast<camera_module_t*>(hmi);
    if (m_pCameraModule == NULL)
    {
        NT_LOG_ERROR("CameraModule missing in library");
        return CDKResultEUnableToLoad;
    }
#endif // _LINUX

return CDKResultSuccess;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiModule::LoadLibraries
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiModule::LoadLibraries()
{
    CDKResult res = InitChiEntry();
    if (res != CDKResultSuccess || nullptr == m_pChiHalOpen || nullptr == m_pCameraModule)
    {
        return CDKResultEUnableToLoad;
    }

    (*m_pChiHalOpen)(&m_chiOps);

    m_pCameraModule->get_vendor_tag_ops(&m_halVendortagOps);
    int ret = set_camera_metadata_vendor_ops(const_cast<vendor_tag_ops_t*>(&m_halVendortagOps));
    if (ret != 0)
    {
        NT_LOG_ERROR("Failed to set vendor tag ops");
        return CDKResultEFailed;
    }

    return CDKResultSuccess;
}

/***************************************************************************************************************************
* ChiModule::OpenContext
*
* @brief
*     Opens a camera context
* @param
*     None
* @return
*     CDKResult result code
***************************************************************************************************************************/
CDKResult ChiModule::OpenContext()
{
    UINT32 majorVersion = m_chiOps.majorVersion;
    UINT32 minorVersion = m_chiOps.minorVersion;
    UINT32 subVersion = m_chiOps.subVersion;

    NT_LOG_INFO("Opening chi context");
    m_hContext = m_chiOps.pOpenContext();

    NT_LOG_INFO("Fetching camxoverride settings");

    // get number of extended/override settings
    const UINT32       numExtendSettings = static_cast<UINT32>(ChxSettingsToken::CHX_SETTINGS_TOKEN_COUNT);
    m_chiOps.pGetSettings(&m_pCamXExSettings, &m_pCamXOrSettings); // read extended settings and camera override settings

    for (UINT32 i = 0; i < numExtendSettings; i++)
    {
        switch (static_cast<ChiModifySettings*>(&m_pCamXOrSettings[i])->token.id)
        {
            case static_cast<UINT32>(ChxSettingsToken::ExposeFullSizeForQCFA):
                m_bFullSizeQCFAExposed = (TRUE == reinterpret_cast<UINT64>(
                    static_cast<ChiModifySettings*>(&m_pCamXOrSettings[i])->pData));
                NT_LOG_INFO("ExposeFullSizeForQCFA is set to %d", m_bFullSizeQCFAExposed);
                break;
            default:
                break;
        }
    }

    m_chiOps.pTagOps(&m_chiVendortagOps);
    m_chiOps.pGetFenceOps(&m_fenceOps);
    m_chiOps.pGetBufferManagerOps(&m_chiBufferMgrOps);
    if (m_hContext == nullptr)
    {
        NT_LOG_ERROR("Open context failed!");
        return CDKResultEFailed;
    }
    return CDKResultSuccess;
}

/***************************************************************************************************************************
* ChiModule::CloseContext
*
* @brief
*     Close camera context
* @param
*     None
* @return
*     CDKResult result code
***************************************************************************************************************************/
CDKResult ChiModule::CloseContext()
{
    CDKResult result = CDKResultSuccess;

    NT_LOG_INFO("Closing Context: %p", m_hContext);
    if (m_hContext != nullptr)
    {
        m_chiOps.pCloseContext(m_hContext);
        m_hContext = nullptr;
    }
    else
    {
        NT_LOG_ERROR("Requested context %p is not open", m_hContext);
        result = CDKResultEInvalidState;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetStaticMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiModule::SetStaticMetadata(UINT32 cameraID, const char* tagName, const char* sectionName, const void* value,
                                       size_t dataCount)
{
    CDKResult result = CDKResultSuccess;
    if(nullptr == value)
    {
        NT_LOG_ERROR("Tag value is null");
        return CDKResultEInvalidPointer; // cannot use nullptr for tag value, due to PW blocking issue
    }
    camera_metadata_t* staticChar = GetCameraStaticCharacteristics(cameraID);
    if(nullptr == staticChar)
    {
        NT_LOG_ERROR("GetCameraStaticCharacteristics() failed for %s", tagName);
        result = CDKResultEInvalidPointer;
    }
    else
        {
            // Check if current field exists in the metadata
            camera_metadata_entry_t tagEntry;
            if(CDKResultSuccess != GetStaticMetadataEntry(cameraID, tagName, sectionName, &tagEntry))
            {
                // TODO: Query all exposed vendor tags and avoid adding new entry
                NT_LOG_WARN("Could not get tag entry for %s, adding new metadata entry", tagName);
                UINT32 tagID = GetStaticVendorTagID(tagName, sectionName);
                if (UINT32_MAX == tagID)
                {
                    NT_LOG_ERROR("%s tag not available!", tagName);
                    return CDKResultEFailed;
                }
                int addResult = add_camera_metadata_entry(staticChar, tagID, value, dataCount);
                if(addResult != 0)
                {
                    result = CDKResultEFailed;
                }
            }
            else
            {
                int setResult = update_camera_metadata_entry(staticChar, tagEntry.index, value, dataCount, nullptr);
                if(setResult != 0)
                {
                    NT_LOG_ERROR("update_camera_metadata_entry failed for %s", tagName);
                    result = CDKResultEFailed;
                }
            }
        }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetStaticVendorTagID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ChiModule::GetStaticVendorTagID(const char* tagName, const char* sectionName) const
{
    int tagCount = m_halVendortagOps.get_tag_count(&m_halVendortagOps);
    if (tagCount <= 0)
    {
        NT_LOG_ERROR("Vendor tags are not available!");
        return UINT32_MAX;
    }
    NT_LOG_DEBUG("Vendor tag count: %d", tagCount);

    UINT32* availableTags = NULL;
    availableTags         = new UINT32[tagCount];
    m_halVendortagOps.get_all_tags(&m_halVendortagOps, availableTags);

    UINT32 tagID = UINT32_MAX;
    for (int tagIndex = 0; tagIndex < tagCount; tagIndex++)
    {
        if (strcmp(tagName, m_halVendortagOps.get_tag_name(&m_halVendortagOps, availableTags[tagIndex])) == 0 &&
            strcmp(sectionName, m_halVendortagOps.get_section_name(&m_halVendortagOps, availableTags[tagIndex])) == 0)
        {
            tagID = availableTags[tagIndex];
            NT_LOG_DEBUG("Found tagID : %u at index %d", tagID, tagIndex);
            break;
        }
    }
    return tagID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetCameraStaticCharacteristics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
camera_metadata_t* ChiModule::GetCameraStaticCharacteristics(UINT32 cameraID) const
{
    const CHICAMERAINFO* pChiInfo;
    pChiInfo = GetCameraInfo(cameraID);
    if(nullptr == pChiInfo)
    {
        NT_LOG_ERROR("GetCameraInfo() failed for camera %d", cameraID);
        return nullptr;
    }
    camera_info* pCameraInfo = reinterpret_cast<camera_info*>(pChiInfo->pLegacy);
    return const_cast<camera_metadata_t*>(pCameraInfo->static_camera_characteristics);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetStaticMetadataEntry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiModule::GetStaticMetadataEntry(UINT32 cameraID, const char* tagName, const char* sectionName,
                                            camera_metadata_entry_t* tagEntry) const
{
    UINT32 tagID = GetStaticVendorTagID(tagName, sectionName);
    if (UINT32_MAX == tagID)
    {
        NT_LOG_UNSUPP("%s tag not available!", tagName);
        return CDKResultEFailed;
    }
    camera_metadata_t* staticChar = GetCameraStaticCharacteristics(cameraID);
    if(nullptr == staticChar)
    {
        NT_LOG_ERROR("GetCameraStaticCharacteristics() failed for %s", tagName);
        return CDKResultEFailed;
    }
    int findResult = find_camera_metadata_entry(staticChar, tagID, tagEntry);
    if(findResult != 0)
    {
        return CDKResultEFailed;
    }
    return CDKResultSuccess;
}

/***************************************************************************************************************************
* ChiModule::GetNumCams
*
* @brief
*     Gets number of cameras reported by the driver
* @param
*     None
* @return
*     int number of cameras reported by the module
***************************************************************************************************************************/
int ChiModule::GetNumberCameras() const
{
    return m_numOfCameras;
}

/***************************************************************************************************************************
* ChiModule::GetCameraList
*
* @brief
*     Gets list of cameras reported by the driver
* @param
*     None
* @return
*     int number of cameras reported by the module
***************************************************************************************************************************/
std::vector <int> ChiModule::GetCameraList() const
{
    return m_camList;
}

/***************************************************************************************************************************
* ChiModule::GetCameraInfo
*
* @brief
*     Gets camera info for given camera Id
* @param
*     [in]  UINT32   cameraId    cameraid associated
* @return
*     CHICAMERAINFO* camerainfo for given camera Id
***************************************************************************************************************************/
const CHICAMERAINFO* ChiModule::GetCameraInfo(UINT32 cameraId) const
{
    const CHICAMERAINFO* pCameraInfo = nullptr;
    if ((m_pCameraInfo != nullptr) && (&m_pCameraInfo[cameraId] != nullptr))
    {
        pCameraInfo = &m_pCameraInfo[cameraId];
    }

    return pCameraInfo;
}

/***************************************************************************************************************************
* ChiModule::GetCameraSensorModeInfo
*
* @brief
*     Gets sensormode for given index
* @param
*     [in] UINT32 cameraId id of the camera
*     [in] UINT32 modeId   index of the mode to use
* @return
*     CHISENSORMODEINFO for given cameraId and index
****************************************************************************************************************************/
CHISENSORMODEINFO* ChiModule::GetCameraSensorModeInfo(UINT32 cameraId, UINT32 modeId) const
{
    CHISENSORMODEINFO* pSensorModeInfo = nullptr;
    if ((m_pSensorInfo != nullptr) && (&m_pSensorInfo[cameraId] != nullptr))
    {
        pSensorModeInfo = &m_pSensorInfo[cameraId][modeId];
    }
    return pSensorModeInfo;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiModule::GetMatchingSensorModes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<UINT32> ChiModule::GetMatchingSensorModes(uint32_t cameraId, std::vector<CmdLineParser::SensorCaps> userCapList) const
{
    std::vector<UINT32> matchingModes; // filtered list of sensor modes that support all requested sensor caps or more
    for (UINT32 mode = 0; mode < m_pCameraInfo[cameraId].numSensorModes; mode++)
    {
        UINT32 capsMatched = 0; // number of capabilities supported by current sensor mode, matching the requested caps
        CHISENSORMODEINFO* pSensorModeInfo = m_pSensorInfo[cameraId];

        // for current sensor mode, go over list of user requested caps and check if supported
        for (std::vector<CmdLineParser::SensorCaps>::iterator pCap = userCapList.begin(); pCap != userCapList.end(); ++pCap)
        {
            switch (*pCap)
            {
            case CmdLineParser::NORMAL:
                if (pSensorModeInfo[mode].sensorModeCaps.u.Normal == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::HFR:
                if (pSensorModeInfo[mode].sensorModeCaps.u.HFR == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::IHDR:
            case CmdLineParser::IHDR2:
            case CmdLineParser::IHDR3:
                if (pSensorModeInfo[mode].sensorModeCaps.u.IHDR == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::PDAF:
                if (pSensorModeInfo[mode].sensorModeCaps.u.PDAF == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::QCFA:
                if (pSensorModeInfo[mode].sensorModeCaps.u.QuadCFA == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::ZZHDR:
                if (pSensorModeInfo[mode].sensorModeCaps.u.ZZHDR == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::DEPTH:
                if (pSensorModeInfo[mode].sensorModeCaps.u.DEPTH == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::SHDR:
            case CmdLineParser::SHDR2:
            case CmdLineParser::SHDR3:
                if (pSensorModeInfo[mode].sensorModeCaps.u.SHDR == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::FS:
                if (pSensorModeInfo[mode].sensorModeCaps.u.FS == 1)
                {
                    capsMatched++;
                }
                break;
            case CmdLineParser::INTERNAL:
                if (pSensorModeInfo[mode].sensorModeCaps.u.Internal == 1)
                {
                    capsMatched++;
                }
                break;
            default: // do not increment count of matched caps, if not supported by current mode
                break;
            }

        }

        // Add this sensor mode to a list if it supports all requested sensor caps or more
        if (capsMatched >= userCapList.size())
        {
            matchingModes.push_back(mode);
        }
    }
    return matchingModes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiModule::GetClosestSensorMode
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHISENSORMODEINFO* ChiModule::GetClosestSensorMode(UINT32 cameraId, Size resolution, UINT32 frameRate,
    bool fpsExactMatch) const
{
    if((nullptr == m_pSensorInfo) || (nullptr == &m_pSensorInfo[cameraId]))
    {
        NT_LOG_ERROR("Sensor information for camera %u is not available", cameraId);
        return nullptr;
    }
    else
    {
        UINT32 index = 0; // select sensor mode 0 by default
        CHISENSORMODEINFO* pSensorModeInfo = m_pSensorInfo[cameraId];

        // Skip sensor capability querying while using testgen
        if(CmdLineParser::isTestGenMode() || CmdLineParser::RemoveSensor())
        {
            NT_LOG_INFO("Skipped sensor capability querying for TPG / virtual sensor");
        }
        else
        {
            // 1. Check for sensor mode runtime override
            uint32_t overrideSensorMode = static_cast<uint32_t>(CmdLineParser::GetSensorMode());
            if ((overrideSensorMode != static_cast<uint32_t>(-1)) && (overrideSensorMode < m_pCameraInfo[cameraId].numSensorModes))
            {
                NT_LOG_WARN("Overriding sensor mode from %u to %d for camera %u", index, CmdLineParser::GetSensorMode(), cameraId);
                index = overrideSensorMode;
            }
            // 2. Else pick best matching sensor mode
            else
            {
                std::vector<SensorMode> finalSensorModeList;

                // Pick best matching mode as per sensor capabilities
                std::vector<CmdLineParser::SensorCaps> userCapList = CmdLineParser::GetSensorCapsList();
                if (userCapList.size() > 0)
                {
                    // Get list of matching sensor modes for requested caps
                    std::vector<UINT32> matchingModes = GetMatchingSensorModes(cameraId, userCapList);
                    if (0 == matchingModes.size())
                    {
                        NT_LOG_ERROR("No sensor mode matches user requirements, Aborting test !");
                        return nullptr;
                    }
                    else if (1 == matchingModes.size())
                    {
                        m_pSelectedSensorInfo[cameraId] = &pSensorModeInfo[matchingModes[0]];
                        return &pSensorModeInfo[matchingModes[0]];
                    }
                    else
                    {
                        for (std::vector<UINT32>::iterator pMode = matchingModes.begin(); pMode != matchingModes.end(); pMode++)
                        {
                            finalSensorModeList.push_back({ *pMode, pSensorModeInfo[*pMode].frameDimension.width,
                                pSensorModeInfo[*pMode].frameDimension.height });
                            NT_LOG_DEBUG("Select from CameraID[%d] Sensor Mode[%d] %d x %d", cameraId, *pMode,
                                pSensorModeInfo[*pMode].frameDimension.width, pSensorModeInfo[*pMode].frameDimension.height);
                        }
                    }
                }
                // TODO: remove this section and set default as Normal in above section itself
                else // Default : Pick best matching mode with Normal + PDAF capabilities
                {
                    bool hasNormalMode = false;
                    std::vector<SensorMode> sensorModePDAFList;
                    std::vector<SensorMode> sensorModeNormalList;
                    for (uint32_t mode = 0; mode < m_pCameraInfo[cameraId].numSensorModes; mode++)
                    {
                        if (frameRate > 0)
                        {
                            if (fpsExactMatch && pSensorModeInfo[mode].frameRate != frameRate)
                            {
                                continue;
                            }

                            if (!fpsExactMatch && pSensorModeInfo[mode].frameRate < frameRate)
                            {
                                continue;
                            }
                        }

                        if (pSensorModeInfo[mode].sensorModeCaps.u.Normal == 1)
                        {
                            hasNormalMode = true;
                            if (pSensorModeInfo[mode].PDAFType != PDTypeUnknown) //Check PDAF bit for AF/stats usage
                            {
                                sensorModePDAFList.push_back({ mode, pSensorModeInfo[mode].frameDimension.width,
                                    pSensorModeInfo[mode].frameDimension.height });
                                NT_LOG_DEBUG("Select from CameraID[%d] Sensor[PDAF] Mode[%d] %d x %d", cameraId, mode,
                                    pSensorModeInfo[mode].frameDimension.width, pSensorModeInfo[mode].frameDimension.height);
                            }
                            else
                            {
                                sensorModeNormalList.push_back({ mode, pSensorModeInfo[mode].frameDimension.width,
                                    pSensorModeInfo[mode].frameDimension.height });
                                NT_LOG_DEBUG("Select from CameraID[%d] Sensor[Normal] Mode[%d] %d x %d", cameraId, mode,
                                    pSensorModeInfo[mode].frameDimension.width, pSensorModeInfo[mode].frameDimension.height);
                            }
                        }
                    }
                    if (!hasNormalMode)
                    {
                        NT_LOG_ERROR("Normal bit is not set for any sensor mode for Camera %d!", cameraId);
                        index = 0; // select sensor mode 0 if normal mode not found
                    }
                    finalSensorModeList = sensorModePDAFList.empty() == true ? sensorModeNormalList : sensorModePDAFList;
                }
                // Pick final sensor mode list according to required capability
                if (finalSensorModeList.size() > 0)
                {
                    sort(finalSensorModeList.begin(), finalSensorModeList.end(),[](const SensorMode& lhs, const SensorMode& rhs)
                    {
                        return lhs.height < rhs.height;
                    });

                    //TODO: pick sensor mode by matching aspect ratio
                    while (!finalSensorModeList.empty())
                    {
                        SensorMode sensorMode = finalSensorModeList.back();
                        finalSensorModeList.pop_back();
                        if (resolution.width <= sensorMode.width && resolution.height <= sensorMode.height)
                        {
                            index = sensorMode.mode;
                        }
                    }
                }
            }
        }

        m_pSelectedSensorInfo[cameraId] = &pSensorModeInfo[index];
        return &pSensorModeInfo[index];
    }
}

/***************************************************************************************************************************
*   ChiModule::SubmitPipelineRequest
*
*   @brief
*       Submit the pipeline request (capture)
*   @param
*     [in]  CHIPIPELINEREQUEST            pRequest       pointer to the capture request
*   @return
*       CDKResult result code
****************************************************************************************************************************/
CDKResult ChiModule::SubmitPipelineRequest(CHIPIPELINEREQUEST * pRequest) const
{
    for (UINT32 requestCount = 0; requestCount < pRequest->numRequests; requestCount++)
    {
        uint64_t frameNum = pRequest->pCaptureRequests[requestCount].frameNumber;
        NT_LOG_DEBUG("Sending pipeline request for frame number: %" PRIu64, frameNum);
    }

    return m_chiOps.pSubmitPipelineRequest(m_hContext, pRequest);
}

/***************************************************************************************************************************
* ChiModule::GetContext
*
* @brief
*     Gets the camera context
* @param
*     None
* @return
*     CHIHANDLE  camera context
***************************************************************************************************************************/
CHIHANDLE ChiModule::GetContext() const
{
    return m_hContext;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiModule::GetContext
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PFNCHIENTRY ChiModule::GetChiEntryFunctionPointer() const
{
    return m_pChiHalOpen;
}

/***************************************************************************************************************************
* ChiModule::GetChiOps
*
* @brief
*     Gets pointer to ChiOps
* @param
*     None
* @return
*     CHICONTEXTOPS*  pointer to chi APIs
***************************************************************************************************************************/
const CHICONTEXTOPS* ChiModule::GetChiOps() const
{
    return &m_chiOps;
}

/***************************************************************************************************************************
* ChiModule::GetFenceOps
*
* @brief
*     Gets pointer to fenceOps
* @param
*     None
* @return
*     CHIFENCEOPS*  pointer to chi fence APIs
***************************************************************************************************************************/
const CHIFENCEOPS* ChiModule::GetFenceOps() const
{
    return &m_fenceOps;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetChiBufferManagerOps
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const CHIBUFFERMANAGEROPS* ChiModule::GetChiBufferManagerOps() const
{
    return &m_chiBufferMgrOps;
}

/***************************************************************************************************************************
* ChiModule::GetLibrary
*
* @brief
*     Gets symbols loaded through the dll/.so
* @param
*     None
* @return
*     VOID* Function pointer to library loaded
***************************************************************************************************************************/
VOID* chitests::ChiModule::GetLibrary() const
{
    return hLibrary;
}

} //namespace chitests