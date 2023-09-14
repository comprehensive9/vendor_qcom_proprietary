/**
 * Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VENDOR_QTI_ESEPOWERMANAGER_V1_1_IESEPOWERMANAGER_H
#define VENDOR_QTI_ESEPOWERMANAGER_V1_1_IESEPOWERMANAGER_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <map>
#include <utils/Log.h>
#include <hidl/MQDescriptor.h>
#include <mutex>
#include <hidl/Status.h>
#include <vendor/qti/esepowermanager/1.1/IEsePowerManager.h>
#include <vendor/qti/esepowermanager/1.1/types.h>
#include <hwbinder/IPCThreadState.h>

#define LOG_TAG             "eSEPowerManager@1.1"
#define nfc_dev_node        "/dev/nq-nci"
#define ESE_SET_PWR         _IOW(0xE9, 0x02, unsigned int)
#define ESE_GET_PWR         _IOR(0xE9, 0x03, unsigned int)
#define RET_FAILURE         -1

namespace vendor {
namespace qti {
namespace esepowermanager {
namespace V1_1 {
namespace implementation {

using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::esepowermanager::V1_1::IEsePowerManager;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::IPCThreadState;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;

struct EsePowerManagerDeathRecipient : hidl_death_recipient {
    EsePowerManagerDeathRecipient(const sp<IEsePowerManager> EsePowerManager, pid_t mPid): mEsePowerManager(EsePowerManager) {
        pid = mPid;
    }
    virtual void serviceDied(uint64_t /*cookie*/,
            const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
        mEsePowerManager->ClientDiedNotifier(pid);
    }
    protected:
        sp<IEsePowerManager> mEsePowerManager;
        pid_t pid;
};

struct EsePowerManager : public IEsePowerManager {

    EsePowerManager();
    // Methods from ::vendor::qti::proprietary::esepowermanager::V1_0::IEsePowerManager follow.
    Return<int32_t> powerOn(const sp<IBase>& callback) override;
    Return<int32_t> powerOff() override;
    Return<int32_t> getState() override;
    Return<int32_t> killall() override;
    Return<void> ClientDiedNotifier(pid_t mPid);
    // Methods from ::vendor::qti::proprietary::esepowermanager::V1_0::IEsePowerManager follow.
    Return<void> eseIoctl(uint64_t ioctlNum, const ::vendor::qti::esepowermanager::V1_1::eseData& inputEseData, eseIoctl_cb _hidl_cb) override;
    private:
        struct service_data {
          ::android::sp<IBase> event_cb_;
          ::android::sp<EsePowerManagerDeathRecipient> mDeathRecipient;
        };
        struct service_data data;
        std::map<pid_t, struct service_data> pidsMap;
};

extern "C" IEsePowerManager* HIDL_FETCH_IEsePowerManager(const char* name);

}  // namespace implementation
}  // namespace V1_1
}  // namespace esepowermanager
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_ESEPOWERMANAGER_V1_1_ESEPOWERMANAGER_H
