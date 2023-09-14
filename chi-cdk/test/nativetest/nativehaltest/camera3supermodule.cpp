//*******************************************************************************************
// Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************************
//=============================================================================
// IMPLEMENTATION of Camera3Module
//=============================================================================
#include "camera3supermodule.h"
#include "vendor_tags_test.h"
#include <cstring>

namespace haltests {

    Camera3SuperModule::Camera3SuperModule() :
        mCameraModule(nullptr),
        mNumOfCameras(0),
        mCamList(),
        mCameraId(0),
        mCameraInfo(nullptr)

    {

        this->camera_device_status_change = haltests::camera_device_status_change;
        this->torch_mode_status_change = haltests::torch_mode_status_change;

        mCamMetaEntry = SAFE_NEW() camera_metadata_entry();
        mCameraInfo = SAFE_NEW() camera_info();

    }

    /**************************************************************************************************
    *   Camera3Module::~Camera3Module
    *
    *   @brief
    *       Destructor for Camera3Module
    **************************************************************************************************/
    Camera3SuperModule::~Camera3SuperModule()
    {
        if (mCamMetaEntry) {
            delete(mCamMetaEntry);
        }
        if (mCameraInfo) {
            delete(mCameraInfo);
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
    }

    void Camera3SuperModule::Setup()
    {

#ifdef _LINUX
        const hw_module_t *hw_module = nullptr;
        NT_ASSERT(0== hw_get_module(CAMERA_HARDWARE_MODULE_ID, &hw_module), "Can't get camera module");
        NT_ASSERT(nullptr != hw_module,"hw_get_module didn't return a valid camera module");

        mCameraModule = reinterpret_cast<const camera_module_t*>(hw_module);

        /*
        * Driver expects vendor_tag_ops_t to be initialized
        * before any further driver calls
        */
        mCameraModule->get_vendor_tag_ops(&vTag);

        int ret = set_camera_metadata_vendor_ops(const_cast<vendor_tag_ops_t*>(&vTag));
        NT_ASSERT(ret == 0, "Failed to set vendor tag ops \n");

        NT_ASSERT(0== mCameraModule->init(), "Can't initialize camera module");

#else

        hLibrary = LoadLibrary("hal.dll");
        NT_ASSERT(nullptr != hLibrary,"Failed to load hal dll! Error code: %s ",GetLastError());

        void * hmi = GetProcAddress(static_cast<HMODULE>(hLibrary), sym);
        NT_ASSERT(nullptr != hmi, "Failed to get camera module symbol");

        mCameraModule = reinterpret_cast<const camera_module_t*>(hmi);

        /*
        * Driver expects vendor_tag_ops_t to be initialized before any further driver calls
        */
        mCameraModule->get_vendor_tag_ops(&vTag);

        int ret = set_camera_metadata_vendor_ops(const_cast<vendor_tag_ops_t*>(&vTag));
        NT_ASSERT(ret == 0, "Failed to set vendor tag ops \n");

        NT_ASSERT(0== mCameraModule->init(),"Can't initialize camera module");
#endif

        NT_ASSERT(nullptr != mCameraModule->get_number_of_cameras,"get_number_of_cameras is not implemented");
        mNumOfCameras = mCameraModule->get_number_of_cameras();
        if (HalCmdLineParser::g_platform >= 0 && HalCmdLineParser::g_platform < static_cast<int>(mPlatformType.size()))
        {
            NT_LOG_INFO("Running on [%s] platform", mPlatformType[HalCmdLineParser::g_platform]);
        }
        NT_LOG_INFO("Device reported %d cameras", mNumOfCameras);

        // Initialize vendor tags
        NT_ASSERT(VendorTagsTest::InitializeVendorTags(&vTag) == 0, "Vendor tags could not be initialized!");

        // Look for logical cameras and exlude them from test
        char tagName[] = "logical_camera_type";
        char sectionName[] = "org.codeaurora.qcamera3.logicalCameraType";
        bool logicalCamTypeTagFound = false;

        uint32_t tagId = VendorTagsTest::ResolveAvailableVendorTag(&vTag, tagName, sectionName);
        if (tagId == UINT32_MAX)
        {
            NT_LOG_WARN("logical_camera_type vendor tag not available");
        }
        else
        {
            logicalCamTypeTagFound = true;
        }

        if (logicalCamTypeTagFound)
        {
            // Verify if --camfilter string provided is valid
            if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterPhysicalStr) != 0 && strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterLogicalStr) != 0 && strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterAllStr) != 0)
            {
                NT_LOG_ERROR("Invalid string \"%s\" provided for --camfilter!", HalCmdLineParser::g_camFilter);
            }
            else
            {
                nLogicalCameras  = 0;
                nPhysicalCameras = 0;
                for (int cameraId = 0; cameraId < mNumOfCameras; cameraId++)
                {
                    camera_metadata_entry_t tagEntry;
                    if (mCamInfoMap.find(cameraId) == mCamInfoMap.end())
                    {
                        camera_info* info = GetCameraInfoWithId(cameraId);
                        NT_ASSERT(nullptr != info, "Unable to retrieve camera info for camera Id: %d", cameraId);
                        mCamInfoMap.insert({ cameraId, *info });
                    }

                    camera_info* pCamInfo = &mCamInfoMap[cameraId];
                    ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(pCamInfo->static_camera_characteristics),
                        tagId, &tagEntry);
                    NT_ASSERT(ret == 0, "find_camera_metadata_entry logical_camera_type failed");
                    if (tagEntry.data.u8[0] != LogicalCameraType_Default)
                    {
                        nLogicalCameras++;
                        // Filter based on --camfilter runtime flag
                        if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterLogicalStr) == 0)
                        {
                            // Logical cameras only, remove physical cameras
                            mCamList.push_back(cameraId);
                        }
                    }
                    else
                    {
                        nPhysicalCameras++;
                        if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterPhysicalStr) == 0)
                        {
                            // Physical cameras only, remove logical cameras
                            mCamList.push_back(cameraId);
                        }
                    }
                    if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterAllStr) == 0)
                    {
                        // All cameras
                        mCamList.push_back(cameraId);
                    }
                }
            }
        }

        char camList[MAX_PATH];
        memcpy(camList, HalCmdLineParser::g_camList, MAX_PATH);
        // Check if --camlist flag set by user. Default value of --camlist flag is "[]".
        if (strcmp(camList, "[]") != 0)
        {
            NT_LOG_INFO("Overriding --camfilter \"%s\" with camera List \"%s\" selection!", HalCmdLineParser::g_camFilter, camList);
            mCamList.clear();
            int i = 0;
            while (camList[i] != '\0')
            {
                if (isdigit(camList[i]))
                {
                    int camId = camList[i] - '0';
                    mCamList.push_back(camId);
                }
                i++;
            }
        }

        int testCameraID = HalCmdLineParser::g_camera;
        // Check if --camera flag set by user. Default value of --camera flag is -1.
        if (testCameraID != -1)
        {
            NT_ASSERT(testCameraID >= 0 && testCameraID < mNumOfCameras, "Invalid camera ID provided for --camera!");
            NT_LOG_INFO("Overriding --camfilter \"%s\" with camera ID \"%d\" selection!", HalCmdLineParser::g_camFilter, testCameraID);
            //Clear camera vector before setting camera ID
            mCamList.clear();
            mCamList.push_back(testCameraID);
        }

        NT_ASSERT(0== mCameraModule->set_callbacks(static_cast<const camera_module_callbacks_t *> (this)), "Cannot set CB");

        SetCamId(0);
    }

    void Camera3SuperModule::Teardown() {}

    /*****************************************************************************************
    * Camera3Module::GetCameraInfoWithId
    *
    * @brief
    *     Gets camera info from camera module based on camera ID
    ******************************************************************************************/

    camera_info * Camera3SuperModule::GetCameraInfoWithId(unsigned int camId)
    {
        if (GetCamModule())
        {
            if (GetCamModule()->get_camera_info == nullptr)
            {
                NT_LOG_ERROR("get_camera_info is not implemented");
                return nullptr;
            }
            int val = GetCamModule()->get_camera_info(camId, mCameraInfo);
            if (val != 0)
            {
                NT_LOG_ERROR("Can't get camera info for: %d", camId);
                return nullptr;
            }
            return mCameraInfo;
        }
        NT_LOG_ERROR("Can't get camera module for: %d", camId);
        return nullptr;
    }

    /******************************************************************************
    * getCameraMetadataEntry
    * @ Gets MedataEntry from camera info based on entry tag
    *******************************************************************************/
    camera_metadata_entry* Camera3SuperModule::GetCameraMetadataEntry(camera_info *info, camera_metadata_tag_t tag) const
    {

        if (find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics), tag,
            GetCamMetaEntry()) != 0)
            {
                NT_LOG_ERROR("Can't find the metadata entry");
                return nullptr;
            }

        return GetCamMetaEntry();
    }

    int Camera3SuperModule::GetNumCams() const
    {
        return mNumOfCameras;
    }

    std::vector <int> Camera3SuperModule::GetCameraList() const
    {
        return mCamList;
    }

    unsigned int Camera3SuperModule::GetCamId() const
    {
        return mCameraId;
    }

    void Camera3SuperModule::SetCamId(unsigned int camId)
    {
        mCameraId = camId;
    }

    const camera_module_t * Camera3SuperModule::GetCamModule() const
    {
        return mCameraModule;
    }

    camera_metadata_entry* Camera3SuperModule::GetCamMetaEntry() const
    {
        return mCamMetaEntry;
    }

    std::map<int, Camera3SuperModule::CameraMapConfiguration> Camera3SuperModule::GetCameraMap(int platform) const
    {
        //Condition to pass the appropriate map based on platform
        std::map<int, PlatformConfiguration>::const_iterator iter = PlatformIFEMap.find(platform);
        return (iter == PlatformIFEMap.end()) ? std::map<int, Camera3SuperModule::CameraMapConfiguration>() : iter->second.PlatformCameraMap;
    }

    int Camera3SuperModule::GetNumOfIFEs(int platform) const
    {
        std::map<int, PlatformConfiguration>::const_iterator iter = PlatformIFEMap.find(platform);
        return (iter == PlatformIFEMap.end()) ? -1 : iter->second.nIFE;
    }

    unsigned int Camera3SuperModule::GetNumOfPhysicalCameras() const
    {
        return nPhysicalCameras;
    }

    unsigned int Camera3SuperModule::GetNumOfLogicalCameras() const
    {
        return nLogicalCameras;
    }

    unsigned int Camera3SuperModule::GetMapNumOfPhysicalCameras(int platform) const
    {
        unsigned int mMapNumOfPhysicalCameras = 0;
        std::map<int, Camera3Module::CameraMapConfiguration> map = GetCameraMap(platform);
        for (auto const &cam_pair : map)
        {
            if (!cam_pair.second.logicalCameraType)
            {
                mMapNumOfPhysicalCameras++;
            }
        }
        return mMapNumOfPhysicalCameras;
    }

    unsigned int Camera3SuperModule::GetMapNumOfLogicalCameras(int platform) const
    {
        unsigned int mMapNumOfLogicalCameras = 0;
        std::map<int, Camera3Module::CameraMapConfiguration> map = GetCameraMap(platform);
        for (auto const &cam_pair : map)
        {
            if (cam_pair.second.logicalCameraType)
            {
                mMapNumOfLogicalCameras++;
            }
        }
        return mMapNumOfLogicalCameras;
    }

    bool Camera3SuperModule::VerifySupportedPlatform(int platform) const
    {
        if (HalCmdLineParser::g_platform < 0 || HalCmdLineParser::g_platform >= static_cast<int>(mPlatformType.size()))
        {
            return false;
        }
        if (mCamList.empty())
        {
            NT_LOG_ERROR("No cameras found with filter: %s", HalCmdLineParser::g_camFilter);
            return false;
        }
        std::map<int, Camera3Module::CameraMapConfiguration> camMap = GetCameraMap(platform);
        if (camMap.empty() == TRUE)
        {
            return false;
        }

        unsigned int nMapPhysicalCameras = GetMapNumOfPhysicalCameras(platform);
        unsigned int nMapLogicalCameras = GetMapNumOfLogicalCameras(platform);
        for (size_t camIndex = 0; camIndex < mCamList.size(); camIndex++)
        {
            // If --camera/--c is enabled
            if (HalCmdLineParser::HalCmdLineParser::g_camera != -1)
            {
                // Verify if camera is present in the map
                if (camMap.find(mCamList[camIndex]) != camMap.end())
                {
                    return true;
                }
            }
            // If --camlist is enabled
            if (strcmp(HalCmdLineParser::g_camList, "[]") != 0)
            {
                /* Verify if the number of cameras match with that of the camera map,
                the cameras provided in the camera list are present in camera map,
                If not, return false*/
                if ((nPhysicalCameras + nLogicalCameras) > (nMapPhysicalCameras + nMapLogicalCameras) || camMap.find(mCamList[camIndex]) == camMap.end())
                {
                    return false;
                }
            }
            // If --camfilter is enabled. Default is physical
            else if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterPhysicalStr) == 0)
            {
                /* Verify if the number of physical cameras match with that of the camera map,
                the physical cameras in the camera list are present in camera map,
                the camera is not a logical camera,
                If not, return false*/
                if (nMapPhysicalCameras != nPhysicalCameras || camMap.find(mCamList[camIndex]) == camMap.end() || camMap.find(mCamList[camIndex])->second.logicalCameraType)
                {
                    return false;
                }
            }
            else if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterLogicalStr) == 0)
            {
                /* Verify if the number of logical cameras match with that of the camera map,
                the logical cameras in the camera list are present in camera map,
                the camera is a logical camera,
                If not, return false*/
                if (nMapLogicalCameras != nLogicalCameras || camMap.find(mCamList[camIndex]) == camMap.end() || !camMap.find(mCamList[camIndex])->second.logicalCameraType)
                {
                    return false;
                }
            }
            else if (strcmp(HalCmdLineParser::g_camFilter, HalCmdLineParser::g_camFilterAllStr) == 0)
            {
                // Check to see if multi camera is not enabled
                if (mCamList.size() == nPhysicalCameras)
                {
                    /* Verify if the number of physical cameras match with that of the camera map,
                    the physical cameras in the camera list are present in camera map,
                    the camera is not a logical camera,
                    If not, return false*/
                    if (nMapPhysicalCameras != nPhysicalCameras || camMap.find(mCamList[camIndex]) == camMap.end() || camMap.find(mCamList[camIndex])->second.logicalCameraType)
                    {
                        return false;
                    }
                }
                /* If multi camera is enabled, verify if number of total cameras match with that of
                the map cameras in camera list are present in camera map.
                If not, return false*/
                else if (camMap.size() != mCamList.size() || camMap.find(mCamList[camIndex]) == camMap.end())
                {
                    return false;
                }
            }
        }
        return true;
    }
} //namespace haltests

