//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************
#include "staticmetadata.h"
namespace haltests
{
    StaticMetadata::StaticMetadata()
    {}

    StaticMetadata::~StaticMetadata()
    {}

    void StaticMetadata::Setup()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::IsCapabilitySupported
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool StaticMetadata::IsCapabilitySupported(camera_info camInfo,
        camera_metadata_enum_android_request_available_capabilities tag)
    {
        camera_metadata_entry configs;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            ANDROID_REQUEST_AVAILABLE_CAPABILITIES, &configs);
        if (ret != 0)
        {
            NT_LOG_ERROR("Unable to find availableCapabilities tag");
            return false;
        }

        for (size_t i = 0; i < configs.count; i++)
        {
            if (configs.data.u8[i] == tag)
            {
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetJPEGMaxSize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int32_t StaticMetadata::GetJPEGMaxSize(camera_info camInfo)
    {
        camera_metadata_entry_t jpegBufMaxSize;
        int val = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            ANDROID_JPEG_MAX_SIZE, &jpegBufMaxSize);
        if (0 != val || jpegBufMaxSize.count == 0)
        {
            NT_LOG_WARN("Can't find the metadata entry for ANDROID_JPEG_MAX_SIZE!");
            return -1;
        }
        return jpegBufMaxSize.data.i32[0];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetVendorTagId
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t StaticMetadata::GetVendorTagId(vendor_tag_ops_t vt, char *tagName, char *sectionName)
    {
        return VendorTagsTest::ResolveAvailableVendorTag(&vt, tagName, sectionName);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetAvailableFormatSizesMap
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::map<int, std::vector<Size>> StaticMetadata::GetAvailableFormatSizesMap(camera_info camInfo, Size bound)
    {
        std::map<int, std::vector<Size>> sizesMap;
        camera_metadata_entry configs;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
        ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &configs);
        if (ret != 0)
        {
            NT_LOG_ERROR("Could not find availableStreamConfigurations tag");
            return std::map<int, std::vector<Size>>{};
        }
        for (size_t i = 0; i < configs.count; i += 4)
        {
            int32_t fmt = configs.data.i32[i + FORMAT_OFFSET];
            int32_t direction = configs.data.i32[i + DIRECTION_OFFSET];
            if (direction == CAMERA3_STREAM_OUTPUT)
            {
                Size res = Size(configs.data.i32[i + RESOLUTION_WIDTH_OFFSET], configs.data.i32[i + RESOLUTION_HEIGHT_OFFSET]);
                if (bound.width*bound.height > 0)
                {
                    if (res.width <= bound.width && res.height <= bound.height)
                    {
                        sizesMap[fmt].push_back(res);
                    }
                }
                else
                {
                    sizesMap[fmt].push_back(res);
                }
            }
        }
        std::map<int, std::vector<Size>>::iterator it;
        for (it = sizesMap.begin(); it != sizesMap.end(); it++)
        {
            sort(sizesMap[it->first].begin(), sizesMap[it->first].end(), compareRes);
        }
        return sizesMap;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetPixelArraySize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Size StaticMetadata::GetPixelArraySize(camera_info camInfo)
    {
        int maxWidth = 0;
        int maxHeight = 0;
        camera_metadata_entry activeArray;

        //Max raw resolution is sensor active pixel region
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE, &activeArray);

        if (ret != 0 || activeArray.count == 0)
        {
            NT_LOG_ERROR("Unable to query static metadata ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE");
            return Size(0, 0);
        }
        else
        {
            // Sample Layout of android.sensor.info.activeArraySize (f0000): int32[4]
            // [0 0 5488 4112]
            maxWidth = activeArray.data.i32[2];
            maxHeight = activeArray.data.i32[3];
        }
        return Size(maxWidth, maxHeight);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetSupportedBufferManagementVersion
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint8_t StaticMetadata::GetSupportedBufferManagementVersion(camera_info camInfo)
    {
        camera_metadata_entry_t version;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_BUFFER_MANAGEMENT_VERSION, &version);
        if (ret != 0)
        {
            NT_LOG_ERROR("Could not find supportedBufferManagementVersion tag");
            return UINT8_MAX;
        }
        return *version.data.u8;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::isQCFASensor
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool StaticMetadata::isQCFASensor(camera_info camInfo, vendor_tag_ops_t vt)
    {
        char tagName[] = "is_qcfa_sensor";
        char sectionName[] = "org.codeaurora.qcamera3.quadra_cfa";
        uint32_t tagId = StaticMetadata::GetVendorTagId(vt, tagName, sectionName);
        if (tagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("quadra_cfa vendor tag not available!");
            return false;
        }

        camera_metadata_entry_t tagEntry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            tagId, &tagEntry);
        if (ret != 0)
        {
            NT_LOG_UNSUPP("find_camera_metadata_entry is_qcfa_sensor failed");
            return false;
        }
        return static_cast<bool>(tagEntry.data.u8[0]);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetQCFAFullSize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Size StaticMetadata::GetQCFAFullSize(camera_info camInfo, vendor_tag_ops_t vt)
    {
        Size res = Size(0, 0);
        char tagName[] = "qcfa_dimension";
        char sectionName[] = "org.codeaurora.qcamera3.quadra_cfa";
        uint32_t tagId = StaticMetadata::GetVendorTagId(vt, tagName, sectionName);
        if (tagId == UINT32_MAX)
        {
            NT_LOG_UNSUPP("quadra_cfa vendor tag not available!");
            return res;
        }

        camera_metadata_entry_t tagEntry;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            tagId, &tagEntry);
        if (ret != 0)
        {
            NT_LOG_UNSUPP("find_camera_metadata_entry qcfa_dimension failed");
            return res;
        }
        res = Size(static_cast<uint32_t>(tagEntry.data.i32[0]), static_cast<uint32_t>(tagEntry.data.i32[1]));
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::IsNoiseReductionSupported
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool StaticMetadata::IsNoiseReductionSupported(camera_info camInfo, camera_metadata_enum_android_noise_reduction_mode tag)
    {
        camera_metadata_entry configs;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(camInfo.static_camera_characteristics),
            ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES, &configs);
        if (ret != 0)
        {
            NT_LOG_ERROR("Unable to find availableNoiseReductionModes tag");
            return false;
        }

        for (size_t i = 0; i < configs.count; i++)
        {
            if (configs.data.u8[i] == tag)
            {
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetOutputMinFrameDuration
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int64_t StaticMetadata::GetOutputMinFrameDuration(int format, Size resolution, camera_info *info)
    {
        camera_metadata_entry configs;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SCALER_AVAILABLE_MIN_FRAME_DURATIONS, &configs);
        if (ret != 0)
        {
            NT_LOG_ERROR("Could not find minFrameDurations tag");
            return -1;
        }
        for (size_t i = 0; i < configs.count; i += 2)
        {
            if (configs.data.i64[i] == format)
            {
                if (configs.data.i64[i + 1] == resolution.width && configs.data.i64[i + 2] == resolution.height)
                {
                    return configs.data.i64[i + 3];
                }
            }
        }
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetDeviceHardwareLevel
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint8_t StaticMetadata::GetDeviceHardwareLevel(camera_info *info)
    {
        camera_metadata_entry configs;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, &configs);
        if (ret != 0)
        {
            NT_LOG_ERROR("Could not find supportedHardwareLevel tag");
            return UINT8_MAX;
        }
        return *configs.data.u8;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StaticMetadata::GetSensorPhysicalSize
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Sizef StaticMetadata::GetSensorPhysicalSize(camera_info *info)
    {
        camera_metadata_entry configs;
        int ret = find_camera_metadata_entry(const_cast<camera_metadata_t*>(info->static_camera_characteristics),
            ANDROID_SENSOR_INFO_PHYSICAL_SIZE, &configs);
        if (ret != 0)
        {
            NT_LOG_ERROR("Could not find ANDROID_SENSOR_INFO_PHYSICAL_SIZE tag");
            return Sizef(0.0f, 0.0f);
        }
        Sizef physicalSize = Sizef(configs.data.f[0], configs.data.f[1]);
        return physicalSize;
    }

}