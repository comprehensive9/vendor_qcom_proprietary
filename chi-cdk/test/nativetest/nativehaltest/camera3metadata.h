//*************************************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************
#ifndef __CAMERA3_METADATA__
#define __CAMERA3_METADATA__

#include "camera3common.h"


namespace haltests {

class Camera3Metadata
    {
    public:
        const char* METADATA_LINUX_PATH = "/data/vendor/camera/meta.data";
        //TODO: change this to sd card path, and add a windows path later

        Camera3Metadata();
        explicit Camera3Metadata(const camera_metadata_t* other);
        Camera3Metadata(const Camera3Metadata& other);
        Camera3Metadata& operator=(const Camera3Metadata& other);
        ~Camera3Metadata();

        camera_metadata_t* GetCamMetadata() const;
        camera_metadata_t* GetClonedCamMetadata() const;

        void Append(const camera_metadata_t * inputMeta);
        void ClearMetadata();

        void SetValue(
            camera_metadata_t* metadata,
            uint32_t tag,
            const int32_t *data,
            size_t data_count) = delete;

        void ResizeIfNeeded (size_t extraEntries, size_t extraData);

        void  WriteMetadataToFile(const camera_metadata_t* metadata) const;
        const camera_metadata_t*  ReadMetadataFromFile() const;
        camera_metadata_t *mBuffer;


    };
}

#endif  //#ifndef __CAMERA3_METADATA__