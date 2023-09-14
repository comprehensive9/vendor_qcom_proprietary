////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   camxdisplayconfig.cpp
/// @brief  Utility functions to update Camera Launch status to display module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxcommontypes.h"
#include "camxdebugprint.h"
#include "camxdefs.h"
#include "camxdisplayconfig.h"
#include "camxosutils.h"

#if ENABLE_DISPLAY_2_0
#include <cutils/native_handle.h>
#include <config/client_interface.h>
#else
#include "IDisplayConfig.h"
#endif // ENABLE_DISPLAY_2_0

#include "IServiceManager.h"

// NOWHINE FILE CP006: STL keyword used (string)

using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::sp;

#if ENABLE_DISPLAY_2_0
using namespace DisplayConfig;
#else
using vendor::display::config::V1_0::IDisplayConfig;
#endif // ENABLE_DISPLAY_2_0

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Namespaces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  Class to implement displayconfig status
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DisplayConfig
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetInstance
    ///
    /// @brief  This function returns the singleton instance of the DisplayConfig.
    ///
    /// @return A pointer to the singleton instance of the DisplayConfig
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static DisplayConfig* GetInstance();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetCameraStatus
    ///
    /// @brief  API to set Camera Launch status
    ///
    /// @param  status Camera On/Off status
    ///
    /// @return CamxResultSuccess if successful, Errors specified by CamxResults otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult SetCameraStatus(
        UINT32 status);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetCameraSmoothInfo
    ///
    /// @brief  API to set Camera Smooth Info
    ///
    /// @param  op   smooth feature On/Off status
    /// @param  fps  frame rate of camera
    ///
    /// @return CamxResultSuccess if successful, Errors specified by CamxResults otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult SetCameraSmoothInfo(
        INT32  op,
        UINT32 fps);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DisplayConfig
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DisplayConfig();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~DisplayConfig
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~DisplayConfig();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Second phase initialization of the display config class
    ///
    /// @return CamxResultSuccess if successful, Errors specified by CamxResults otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize();

    DisplayConfig(const DisplayConfig& rOther)              = delete;
    DisplayConfig(const DisplayConfig&& rrOther)            = delete;
    DisplayConfig& operator=(const DisplayConfig& rOther)   = delete;
    DisplayConfig& operator=(const DisplayConfig&& rrOther) = delete;

#if ENABLE_DISPLAY_2_0
    ClientInterface*    mDisplayConfig; ///< ClientInterface pointer
#else
    sp<IDisplayConfig>  mDisplayConfig; ///< Display config pointer
#endif // ENABLE_DISPLAY_2_0
};

// NOWHINE FILE NC003a: pointer prefix should start with p.
// NOWHINE FILE PR008:order of include files.
// NOWHINE FILE PR007b:Non-library file inclusion.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayConfigInterface::SetCameraStatus
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult DisplayConfigInterface::SetCameraStatus(
    UINT32 status)
{
    DisplayConfig* pDisplayConfig = DisplayConfig::GetInstance();
    CamxResult     result         = CamxResultSuccess;

    result = pDisplayConfig->SetCameraStatus(status);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayConfigInterface::SetCameraSmoothInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult DisplayConfigInterface::SetCameraSmoothInfo(
    INT32  op,
    UINT32 fps)
{
    DisplayConfig* pDisplayConfig = DisplayConfig::GetInstance();
    CamxResult     result         = CamxResultSuccess;

    result = pDisplayConfig->SetCameraSmoothInfo(op, fps);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayConfig::GetInstance
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DisplayConfig* DisplayConfig::GetInstance()
{
    static DisplayConfig s_DisplayConfigSingleton;

    return &s_DisplayConfigSingleton;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayConfig::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult DisplayConfig::Initialize()
{
    CamxResult result = CamxResultEFailed;
    INT32      ret    = 0;

    // This log message will help to determine how long, getService API is taking.
    // If this is taking very long to open, then consider to make this as static.
    CAMX_LOG_VERBOSE(CamxLogGroupFormat, "IDisplayConfig getService called");

#if ENABLE_DISPLAY_2_0
    ret = ClientInterface::Create("camxdisplay", NULL, &mDisplayConfig);
#else
    if (mDisplayConfig == NULL)
    {
        mDisplayConfig = IDisplayConfig::getService();
    }
#endif // ENABLE_DISPLAY_2_0

    if ((mDisplayConfig == NULL) || (0 != ret))
    {
        CAMX_LOG_ERROR(CamxLogGroupFormat, "IDisplayConfig service is NULL");
    }
    else
    {
        result = CamxResultSuccess;
#if ENABLE_DISPLAY_2_0
        CAMX_LOG_INFO(CamxLogGroupFormat, "IDisplayConfig is loaded, version 2.0");
#else
        CAMX_LOG_INFO(CamxLogGroupFormat, "IDisplayConfig is loaded, version 1.0");
#endif // ENABLE_DISPLAY_2_0
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DisplayConfig::DisplayConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DisplayConfig::DisplayConfig()
{
    Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DisplayConfig::~DisplayConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DisplayConfig::~DisplayConfig()
{
#if ENABLE_DISPLAY_2_0
    ClientInterface::Destroy(mDisplayConfig);
#else
    mDisplayConfig.clear();
#endif // ENABLE_DISPLAY_2_0
    mDisplayConfig = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayConfig::SetCameraStatus
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult DisplayConfig::SetCameraStatus(
    UINT32 status)
{
    CamxResult      result = CamxResultSuccess;
    Return<int32_t> ret    = 0;

    if (mDisplayConfig == NULL)
    {
        CAMX_LOG_ERROR(CamxLogGroupFormat, "IDisplayConfig service is NULL");
        result = CDKResultEInvalidPointer;
    }

    if (CamxResultSuccess == result)
    {
#if ENABLE_DISPLAY_2_0
        ret = mDisplayConfig->SetCameraLaunchStatus(status);
#else
        ret = mDisplayConfig->setCameraLaunchStatus(status);
#endif // ENABLE_DISPLAY_2_0

        if (false == ret.isOk())
        {
            CAMX_LOG_ERROR(CamxLogGroupFormat, "DisplayConfig Unable to send response. Exception : %s",
                          ret.description().c_str());
#if ENABLE_DISPLAY_2_0
            ClientInterface::Destroy(mDisplayConfig);
#else
            mDisplayConfig.clear();
#endif // ENABLE_DISPLAY_2_0
            mDisplayConfig = NULL;
            result         = CamxResultEFailed;
        }
        else if (0 == ret)
        {
            CAMX_LOG_INFO(CamxLogGroupFormat, "returned success, camera status %d", status);
        }
        else
        {
            CAMX_LOG_ERROR(CamxLogGroupFormat, "returned failure, camera status %d", status);
            result = CamxResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayConfig::SetCameraSmoothInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult DisplayConfig::SetCameraSmoothInfo(
    INT32  op,
    UINT32 fps)
{
    CamxResult      result      = CamxResultSuccess;
    Return<int32_t> ret         = 0;
    std::string     value       = "";
    const auto      smooth_prop = "enable_camera_smooth";

#if ENABLE_DISPLAY_2_0
#if (((CAMX_ANDROID_API) && (CAMX_ANDROID_API == 30)) && defined(DISPLAY_CONFIG_CAMERA_SMOOTH_APIs_1_0))
    if (mDisplayConfig == NULL)
    {
        CAMX_LOG_ERROR(CamxLogGroupFormat, "IDisplayConfig service is NULL");
        result = CDKResultEInvalidPointer;
    }

    if (CamxResultSuccess == result)
    {
        result = mDisplayConfig->GetDebugProperty(smooth_prop, &value);
        CAMX_LOG_INFO(CamxLogGroupFormat, "enable_camera_smooth, result:%d value:%s", result, value.c_str());

        if ((CamxResultSuccess == result) && ("1" == value))
        {
            ret = mDisplayConfig->SetCameraSmoothInfo(
                op ? CameraSmoothOp::kOn : CameraSmoothOp::kOff, fps);
        }
    }
#endif // (((CAMX_ANDROID_API) && (CAMX_ANDROID_API == 30)) && defined(DISPLAY_CONFIG_CAMERA_SMOOTH_APIs_1_0))
#endif // ENABLE_DISPLAY_2_0

    if (false == ret.isOk())
    {
        CAMX_LOG_ERROR(CamxLogGroupFormat, "DisplayConfig Unable to send response. Exception : %s",
                      ret.description().c_str());
#if ENABLE_DISPLAY_2_0
#if (((CAMX_ANDROID_API) && (CAMX_ANDROID_API == 30)) && defined(DISPLAY_CONFIG_CAMERA_SMOOTH_APIs_1_0))
        ClientInterface::Destroy(mDisplayConfig);
        mDisplayConfig = NULL;
#endif // (((CAMX_ANDROID_API) && (CAMX_ANDROID_API == 30)) && defined(DISPLAY_CONFIG_CAMERA_SMOOTH_APIs_1_0))
#endif // ENABLE_DISPLAY_2_0
        result         = CamxResultEFailed;
    }
    else if (0 == ret)
    {
        CAMX_LOG_INFO(CamxLogGroupFormat, "returned success, camera info op %d fps %d", op, fps);
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupFormat, "returned failure, camera info op %d fps %d", op, fps);
        result = CamxResultEFailed;
    }

    return result;
}

CAMX_NAMESPACE_END
