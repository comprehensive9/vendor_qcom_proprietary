/******************************************************************************
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_ISERVICETRACKER_V1_2_H
#define VENDOR_QTI_HARDWARE_ISERVICETRACKER_V1_2_H

#include <vendor/qti/hardware/servicetracker/1.2/IServicetracker.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace servicetracker {
namespace V1_2 {
namespace implementation {

using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;

using ::android::hidl::base::V1_0::IBase;
using vendor::qti::hardware::servicetracker::V1_0::Status;

struct CallbackData {
    CallbackData(sp<IActivityEventInfoCallback> callback, NotifyType aNotify, uint32_t userdata):
    callback(callback), aNotify(aNotify), userdata(userdata){}
    sp<IActivityEventInfoCallback> callback;
    NotifyType aNotify;
    uint32_t userdata;
};

struct Servicetracker : public IServicetracker, public hidl_death_recipient  {    // Methods from ::vendor::qti::hardware::servicetracker::V1_0::IServicetracker follow.
    Return<void> startService(const ::vendor::qti::hardware::servicetracker::V1_0::ServiceData& serviceData) override;
    Return<void> bindService(const ::vendor::qti::hardware::servicetracker::V1_0::ServiceData& serviceData, const ::vendor::qti::hardware::servicetracker::V1_0::ClientData& clientData) override;
    Return<void> unbindService(const ::vendor::qti::hardware::servicetracker::V1_0::ServiceData& serviceData, const ::vendor::qti::hardware::servicetracker::V1_0::ClientData& clientData) override;
    Return<void> destroyService(const ::vendor::qti::hardware::servicetracker::V1_0::ServiceData& serviceData) override;
    Return<void> killProcess(int32_t pid) override;
    Return<void> getclientInfo(const hidl_string& clientName, getclientInfo_cb _hidl_cb) override;
    Return<void> getserviceInfo(const hidl_string& serviceName, getserviceInfo_cb _hidl_cb) override;
    Return<void> getServiceConnections(const hidl_string& serviceName, getServiceConnections_cb _hidl_cb) override;
    Return<void> getClientConnections(const hidl_string& clientName, getClientConnections_cb _hidl_cb) override;
    Return<void> getPid(const hidl_string& processName, getPid_cb _hidl_cb) override;
    Return<void> getPids(const hidl_vec<hidl_string>& serviceList, getPids_cb _hidl_cb) override;
    Return<void> isServiceB(const hidl_string& serviceName, isServiceB_cb _hidl_cb) override;
    Return<void> getServiceBCount(getServiceBCount_cb _hidl_cb) override;

    // Methods from ::vendor::qti::hardware::servicetracker::V1_1::IServicetracker follow.
    Return<void> getRunningServicePid(getRunningServicePid_cb _hidl_cb) override;

    // Methods from ::vendor::qti::hardware::servicetracker::V1_2::IServicetracker follow.
    Return<::vendor::qti::hardware::servicetracker::V1_2::Status> registerCallback(const sp<::vendor::qti::hardware::servicetracker::V1_2::IActivityEventInfoCallback>& callback, ::vendor::qti::hardware::servicetracker::V1_2::NotifyType aNotify, int32_t userdata) override;
    Return<::vendor::qti::hardware::servicetracker::V1_2::Status> unregisterCallback(const sp<::vendor::qti::hardware::servicetracker::V1_2::IActivityEventInfoCallback>& callback) override;
    Return<void> OnActivityStateChange(::vendor::qti::hardware::servicetracker::V1_2::ActivityStates aState, const ::vendor::qti::hardware::servicetracker::V1_2::ActivityDetails& aDetails, const ::vendor::qti::hardware::servicetracker::V1_2::ActivityStats& aStats, bool early_notify) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

    private:
    bool unregisterCallbackInternal(const sp<IBase>& cb);
    void serviceDied(uint64_t cookie, const wp<IBase>& /* who */) override;

    // Mutex to have synchronized access to callbacks
    std::mutex callbacks_lock_;

    std::vector<CallbackData> callbacks_;
};

// FIXME: most likely delete, this is only for passthrough implementations
 extern "C" IServicetracker* HIDL_FETCH_IServicetracker(const char* name);

}  // namespace implementation
}  // namespace V1_2
}  // namespace servicetracker
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif /* VENDOR_QTI_HARDWARE_ISERVICETRACKER_V1_2_H */
