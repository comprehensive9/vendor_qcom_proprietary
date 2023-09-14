//******************************************************************************
// Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __CAMERA3_SUPERMODULE__
#define __CAMERA3_SUPERMODULE__

#include "camera3common.h"

namespace haltests {

    class Camera3SuperModule : public camera_module_callbacks_t
    {
    public:
        Camera3SuperModule();
        virtual ~Camera3SuperModule();

        // Platforms supported
        enum PlatformType
        {
            konanontricamife2,
            konanontricamife3,
            konatricamife2,
            konatricamife3,
            konahmdife6
        };

        // Match the order to that of the enum PlatformType
        std::vector<char const *> mPlatformType =
        {
            "Kona non-triple camera/2 IFEs",
            "Kona non-triple camera/3 IFEs",
            "Kona triple camera/2 IFEs",
            "Kona triple camera/3 IFEs",
            "Kona HMD/6 IFEs"
        };

        // camx\chi-cdk\vendor\chioverride\default\chxextensionmodule.h defines the logical camera type enum
        enum LogicalCameraType
        {
            LogicalCameraType_Default = 0,     ///< Single physical camera
            LogicalCameraType_RTB,             ///< RTB mode
            LogicalCameraType_SAT,             ///< SAT mode
            LogicalCameraType_BAYERMONO,       ///< Dual camera with Bayer and MONO sensor
            LogicalCameraType_VR,              ///< VR camera
            LogicalCameraType_DualApp,         ///< Logical camera type for application dual camera solution
            LogicalCameraType_MAX,
        };

        // Reference from logical camera configuration found in camx\chi-cdk\vendor\chioverride\default\chxextensionmodule.cpp and from static metadata
        enum KonaCameras
        {
            WideBackCamera = 0,
            FrontCamera = 1,
            UltraWideBackCamera = 1,
            TeleBackCamera = 2,
            LogicalSATPrimaryWideCamera = 3,
            LogicalSATUWT = 3,
            LogicalRTBPrimaryTeleCamera = 4,
            LogicalSATWT = 4,
            LogicalSATPrimaryFrontCamera = 5,
        };

        // Camera type specification provided by the XR driver team
        enum VRKonaCameras
        {
            SingleCameraIFEBayer1,
            SingleCameraIFEBayer2,
            SingleCameraMonoTracking1,
            SingleCameraMonoTracking2,
            SingleCameraMonoEyeTracking1,
            SingleCameraMonoEyeTracking2,
            DualCameraIFEBayer,
            DualCameraMonoTracking,
            DualCameraMonoEyeTracking,
        };

        enum SpecialSetting
        {
            NoSetting,
            SettingZSL,
            SettingSWMF,
            SettingMNFR,
            SettingSAT,
            SettingBokeh,
            SettingBurstshot,
            NumOfSpecialSetting
        };

        struct CameraMapConfiguration
        {
            std::vector<int> camList;
            bool logicalCameraType;
        };

        struct PlatformConfiguration
        {
            int nIFE;
            std::map<int, CameraMapConfiguration> PlatformCameraMap;
        };

        const camera_module_t *mCameraModule;
        std::map<int, camera_info> mCamInfoMap;

    protected:
        virtual void Setup();
        virtual void Teardown();

        camera_info * GetCameraInfoWithId(unsigned int camId);

        camera_metadata_entry* GetCameraMetadataEntry(camera_info *info, camera_metadata_tag_t tag) const;

        int GetNumCams() const;
        std::vector<int> GetCameraList() const;
        unsigned int GetCamId() const;
        void SetCamId(unsigned int camId);
        const camera_module_t* GetCamModule() const;
        camera_metadata_entry* GetCamMetaEntry() const;
        std::map<int, CameraMapConfiguration> GetCameraMap(int platform) const;
        int GetNumOfIFEs(int platform) const;
        unsigned int GetNumOfPhysicalCameras() const;
        unsigned int GetNumOfLogicalCameras() const;
        unsigned int GetMapNumOfPhysicalCameras(int platform) const;
        unsigned int GetMapNumOfLogicalCameras(int platform) const;
        bool VerifySupportedPlatform(int platform) const;

        vendor_tag_ops_t vTag;

    private:

        /* Mapping of physical and logical cameras determined from dumpsys and from logical camera
        configuration found in camx\chi-cdk\vendor\chioverride\default\chxextensionmodule.cpp */
        std::map<int, CameraMapConfiguration> mKonaTripleCameraMap =
        {
            //| Key - Camera                    | Physical camera linked to Key,                          logicalCameraType |
              { WideBackCamera,                 { { WideBackCamera },                                       false } },
              { UltraWideBackCamera,            { { UltraWideBackCamera },                                  false } },
              { TeleBackCamera,                 { { TeleBackCamera },                                       false } },
              { LogicalSATUWT,                  { { WideBackCamera, UltraWideBackCamera, TeleBackCamera },  true  } },
              { LogicalSATWT,                   { { WideBackCamera, TeleBackCamera },                       true  } }
        };

        /* Mapping of physical and logical cameras determined from dumpsys */
        std::map<int, CameraMapConfiguration> mKonaNonTripleCameraMap =
        {
            //| Key - Camera                    | Physical camera linked to Key,                          logicalCameraType |
            { WideBackCamera,                   { { WideBackCamera },                                       false } },
            { FrontCamera,                      { { FrontCamera },                                          false } }
        };

        // Mapping of physical and logical cameras provided by the XR driver team
        std::map<int, CameraMapConfiguration> mVRKonaCameraMap =
        {
            //| Key - Camera                    | Physical camera linked to Key,                                logicalCameraType |
            { SingleCameraIFEBayer1,            { { SingleCameraIFEBayer1 },                                        false } },
            { SingleCameraIFEBayer2,            { { SingleCameraIFEBayer2 },                                        false } },
            { SingleCameraMonoTracking1,        { { SingleCameraMonoTracking1 },                                    false } },
            { SingleCameraMonoTracking2,        { { SingleCameraMonoTracking2 },                                    false } },
            { SingleCameraMonoEyeTracking1,     { { SingleCameraMonoEyeTracking1 },                                 false } },
            { SingleCameraMonoEyeTracking2,     { { SingleCameraMonoEyeTracking2 },                                 false } },
            { DualCameraIFEBayer,               { { SingleCameraIFEBayer1, SingleCameraIFEBayer2 },                 true  } },
            { DualCameraMonoTracking,           { { SingleCameraMonoTracking1, SingleCameraMonoTracking2 },         true  } },
            { DualCameraMonoEyeTracking,        { { SingleCameraMonoEyeTracking1, SingleCameraMonoEyeTracking2 },   true  } }
        };

        std::map<int, PlatformConfiguration> PlatformIFEMap =
        {
            //|Platform         |            |Number of IFEs  | Camera Map              |
            { konanontricamife2,             { 2              , mKonaNonTripleCameraMap } },
            { konanontricamife3,             { 3              , mKonaNonTripleCameraMap } },
            { konatricamife2,                { 2              , mKonaTripleCameraMap    } },
            { konatricamife3,                { 3              , mKonaTripleCameraMap    } },
            { konahmdife6,                   { 6              , mVRKonaCameraMap        } }
        };

#ifndef _LINUX
        const char *sym = "HMI";
        void* hLibrary = nullptr;
#endif
        int mNumOfCameras;
        std::vector<int> mCamList;
        unsigned int mCameraId;
        camera_metadata_entry * mCamMetaEntry;
        camera_info *mCameraInfo;
        unsigned int nPhysicalCameras;
        unsigned int nLogicalCameras;
    };

    static inline void camera_device_status_change(
        const struct camera_module_callbacks* callbacks,
        int camera_id,
        int new_status)
    {
        NATIVETEST_UNUSED_PARAM(camera_device_status_change);
        NATIVETEST_UNUSED_PARAM(callbacks);
        NATIVETEST_UNUSED_PARAM(camera_id);
        NATIVETEST_UNUSED_PARAM(new_status);
    }
    static inline void torch_mode_status_change(
        const struct camera_module_callbacks* callbacks,
        const char* camera_id,
        int new_status)
    {
        NATIVETEST_UNUSED_PARAM(torch_mode_status_change);
        NATIVETEST_UNUSED_PARAM(callbacks);
        NATIVETEST_UNUSED_PARAM(camera_id);
        NATIVETEST_UNUSED_PARAM(new_status);
    }


}  // namespace haltests


#endif  //#ifndef __CAMERA3_SUPERMODULE__
