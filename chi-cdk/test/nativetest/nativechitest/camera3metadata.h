//*************************************************************************************************
// Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************
#ifndef __CAMERA3_METADATA__
#define __CAMERA3_METADATA__

#include "camera3common.h"
#include <map>

#ifdef _LINUX
#include <camera_metadata_hidden.h>
#endif

namespace chitests {

/*
* This is a custom struct which will be filled by test
* defining all the metadata tags to be set by the framework
* before sending PCR
*
* N: number of metadata tags present in this struct
*/
template <int N>
struct CameraMetadataProperties
{
    CameraMetadataProperties() : tolerances{} { }
    camera_metadata_tag tag[N];
    static const uint32_t count = N;
    uint8_t type[N];
    void* values[N];
    size_t datacounts[N];
    float tolerances[N];
};

class Camera3Metadata
{
public:
    const char* METADATA_LINUX_PATH = "/data/vendor/camera/meta.data";
    //TODO: change this to sd card path, and add a windows path later

    vendor_tag_ops_t vTag;

    Camera3Metadata();
    explicit Camera3Metadata(const camera_metadata_t* other);
    Camera3Metadata(const Camera3Metadata& other);
    Camera3Metadata& operator=(const Camera3Metadata& other);
    ~Camera3Metadata();

    camera_metadata_t* GetCamMetadata() const;
    camera_metadata_t* GetClonedCamMetadata() const;

    static int FindCameraMetadataFromTemplate(
        camera_metadata_t* templateRequest,
        camera_metadata_tag_t tag);

    static int GetCameraMetadataEntryByTag(
        camera_metadata_t* metadata,
        camera_metadata_tag_t tag,
        camera_metadata_entry_t *entry);

    static bool CheckAvailableCapability(
        camera_info info, camera_metadata_enum_android_request_available_capabilities capability);

    static const camera_metadata_t* GetCameraMetadataByFrame(uint32_t frameNumber);

    static void ClearMetadataMap();

    static bool IfAEConverged(camera_metadata_t* metadata);
    static bool IfAFConverged(camera_metadata_t* metadata);
    static bool IfAWBConverged(camera_metadata_t* metadata);

    void Append(const camera_metadata_t * inputMeta);
    void ClearMetadata();

    void SetValue(
        camera_metadata_t* metadata,
        uint32_t tag,
        const int32_t *data,
        size_t data_count) = delete;

    int SetKey(uint32_t tag, uint8_t type, const void* value, size_t dataCount, camera_metadata_t*& settings);

    template <int N>
    static bool VerifyKeysSetInResult(CameraMetadataProperties<N> metadataKeys, int frameNum);

    void  WriteMetadataToFile(const camera_metadata_t* metadata) const;
    const camera_metadata_t*  ReadMetadataFromFile() const;

private:
    camera_metadata_t *mBuffer;
    CDKResult ResizeIfNeeded(size_t extraEntries, size_t extraData);
    static bool CheckType(uint32_t tag, uint8_t expectedType);
    CDKResult UpdateKey(uint32_t tag, uint8_t type, const void* value, size_t dataCount, camera_metadata_t*& settings);
    static bool CheckKeySetInResult(uint32_t tag, const uint8_t * value, uint32_t frameNum);
    bool CheckKeySetInResult(uint32_t tag, const int32_t * value, float errorMarginRate, int frameNum) const;
    static bool CheckKeySetInResult(uint32_t tag, const float * value, int frameNum);
    bool CheckKeySetInResult(uint32_t tag, const int64_t * value, float errorMarginRate, int frameNum) const;
    static bool CheckKeySetInResult(uint32_t tag, const double * value, int frameNum);
    static bool CheckKeySetInResult(uint32_t tag, const camera_metadata_rational_t * value, int frameNum);


};

extern std::map<uint32_t, Camera3Metadata> mMetadata;
}

#endif  //#ifndef __CAMERA3_METADATA__