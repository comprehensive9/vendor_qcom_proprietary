//******************************************************************************
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*******************************************************************************

#ifndef __STATIC_METADATA__
#define __STATIC_METADATA__

#include "camera3supermodule.h"
#include "vendor_tags_test.h"

namespace haltests
{
    // A separate class added to verify if tags and capabilities are supported for a camera
    // The functions will be used by test cases and cam3device classes
    class StaticMetadata : public Camera3SuperModule
    {

    public:
        StaticMetadata();
        ~StaticMetadata();

        enum StreamConfiguration
        {
            FORMAT_OFFSET,
            RESOLUTION_WIDTH_OFFSET,
            RESOLUTION_HEIGHT_OFFSET,
            DIRECTION_OFFSET
        };

        static bool compareRes(Size res1, Size res2)
        {
            return (res1.width*res1.height > res2.width*res2.height);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::IsCapabilitySupported()
        ///
        /// @brief  returns true if given capability is present in availableCapabilities tag in static metadata
        ///
        /// @param  camInfo     Camera Information object
        /// @param  tag         Camera Metadata tag to check in capabilities
        ///
        /// @return     bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool IsCapabilitySupported(camera_info camInfo, camera_metadata_enum_android_request_available_capabilities tag);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::IsNoiseReductionSupported()
        ///
        /// @brief  returns true if given tag is present in availableNoiseReductionModes tag in static metadata
        ///
        /// @param  camInfo     Camera Information object
        /// @param  tag         Camera Metadata tag to check in noise reduction modes
        ///
        /// @return     bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool IsNoiseReductionSupported(camera_info camInfo,
            camera_metadata_enum_android_noise_reduction_mode tag);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetAvailableFormatSizesMap()
        ///
        /// @brief
        ///   returns all of the available output preview sizes found in camera
        ///
        /// @return
        ///   map containing formats and preview sizes
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::map<int, std::vector<Size>> GetAvailableFormatSizesMap(camera_info camInfo, Size bound = Size(0, 0));

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetJPEGMaxSize()
        ///
        /// @brief
        ///   returns max size of gralloc buffers for jpeg
        ///
        /// @return
        ///   Size
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static int32_t GetJPEGMaxSize(camera_info camInfo);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetPixelArraySize
        /// @brief
        ///   Finds the Pixel Array Size from the device static info metadata
        ///
        /// @return
        ///   Pixel array size if successful or Size(0,0)
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Size GetPixelArraySize(camera_info camInfo);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetSupportedBufferManagementVersion
        /// @brief
        ///   Gets the version of the buffer management
        ///
        /// @return
        ///   uint8_t buffer management version
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static uint8_t GetSupportedBufferManagementVersion(camera_info camInfo);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::isQCFASensor()
        ///
        /// @brief  returns true if camera is a QCFA sensor
        ///
        /// @param  Camera  Information object
        /// @param  vt      Vendor Tag Ops Object
        ///
        /// @return     bool
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isQCFASensor(camera_info camInfo, vendor_tag_ops_t vt);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetQCFAFullSize()
        ///
        /// @brief  returns resolution supported by QCFA sensor
        ///
        /// @param      Camera  Information object
        /// @param      vt      Vendor Tag Ops Object
        ///
        /// @return     Size
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Size GetQCFAFullSize(camera_info camInfo, vendor_tag_ops_t vt);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetVendorTagId()
        ///
        /// @brief  returns the tag number of a given vendor tag.
        ///
        /// @param  vt              Vendor Tag Ops Object
        /// @param  tagName         Vendor tag name
        /// @param  sectionName     Vendor tag section name
        ///
        /// @return Size
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static uint32_t GetVendorTagId(vendor_tag_ops_t vt, char *tagName, char *sectionName);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetOutputMinFrameDuration()
        ///
        /// @brief  returns minimum frame duration.
        ///
        /// @param  format          format of the stream
        /// @param  resolution      resolution of the stream
        /// @param  info            Information object
        ///
        /// @return int64_t
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static int64_t GetOutputMinFrameDuration(int format, Size resolution, camera_info *info);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetDeviceHardwareLevel()
        ///
        /// @brief  returns supported hardware level of device
        ///
        /// @param  info              Information object
        ///
        /// @return int64_t
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static uint8_t GetDeviceHardwareLevel(camera_info *info);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// StaticMetadata::GetSensorPhysicalSize()
        ///
        /// @brief  returns physical size of the sensor
        ///
        /// @param  info              Information object
        ///
        /// @return Sizef
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Sizef GetSensorPhysicalSize(camera_info *info);

    protected:
        virtual void Setup();
    };
}
#endif  //#ifndef __STATIC_METADATA__