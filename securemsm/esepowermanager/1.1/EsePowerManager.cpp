/**
 * Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "EsePowerManager.h"

namespace vendor {
namespace qti {
namespace esepowermanager {
namespace V1_1 {
namespace implementation {

int nq_node = -1;
static std::mutex m;

std::function<void(sp<EsePowerManagerDeathRecipient>&)> unlink_cb_;
EsePowerManager::EsePowerManager(){
}

int openNode() {
    if (nq_node == -1) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s", __func__, strerror(errno));
            nq_node = -1;
        }
    }

    return nq_node;
}

void closeNode() {
    if (close(nq_node)) {
        ALOGE("%s: eSE ioctl close failed : %s", __func__, strerror(errno));
    }
    nq_node = -1;

    return;
}

int getIoctlState(uint64_t ioctlNum) {
    int ret = -1;

    ret = openNode();
    if(ret < 0) {
        return ret;
    }
    ret = ioctl(nq_node, ioctlNum, 0);
    if (ret < 0) {
        ALOGE("%s: eSE getting ioctl %llu failed : %s", __func__, ioctlNum, strerror(errno));
        closeNode();
    }

    return ret;
}

int setIoctlState(uint64_t ioctlNum, uint32_t value) {
    int ret = -1;

    ret = openNode();
    if(ret < 0) {
        return ret;
    }
    ret = ioctl(nq_node, ioctlNum, value);
    if (ret < 0) {
        ALOGE("%s: eSE setting ioctl %llu failed : %s", __func__, ioctlNum, strerror(errno));
        closeNode();
    }

    return ret;
}

// Methods from ::vendor::qti::proprietary::esepowermanager::V1_0::IEsePowerManager follow.
Return<int32_t> EsePowerManager::powerOn(const sp<IBase>& callback) {
    std::lock_guard<std::mutex> lock(m);
    int retIoctl = -1;

    if (callback == nullptr) {
        ALOGE("callback == nullptr!");
        return RET_FAILURE;
    }

    pid_t pid = IPCThreadState::self()->getCallingPid();

    ALOGD("Power ON - PID = %d", pid);

    retIoctl = getIoctlState(ESE_GET_PWR);
    if (retIoctl < 0) {
        ALOGE("%s: ESE_GET_PWR failed: %s", __func__, strerror(errno));
        closeNode();
        return RET_FAILURE;
    }

    if (retIoctl > 0) {
        ALOGD("eSE already powered ON");
    } else {
        if (!pidsMap.empty()) {
            //here the pid list is out of sync with the eSE power states.
            ALOGW("pidList out of sync ..."); // nothing to be done here.
            for(std::map<pid_t, struct service_data>::iterator it = pidsMap.begin(); it != pidsMap.end(); ++it) {
                ALOGD("MAP PID = %d !", it->first);
            }
        }
        retIoctl = setIoctlState(ESE_SET_PWR, 0);
        if (retIoctl < 0) {
            ALOGE("%s: ESE_SET_PWR failed: %s", __func__, strerror(errno));
            closeNode();
            return RET_FAILURE;
        }
    }
    data.mDeathRecipient = new EsePowerManagerDeathRecipient(this, pid);
    if (data.mDeathRecipient == NULL) {
        ALOGE("%s: mDeathRecipient NULL", __func__);
        closeNode();
        return RET_FAILURE;
    }
    data.event_cb_ = callback;
    data.event_cb_->linkToDeath(data.mDeathRecipient, 0);
    pidsMap.insert( std::pair<pid_t, struct service_data> (pid, data));
    return 1;
}

Return<int32_t> EsePowerManager::powerOff() {
    std::lock_guard<std::mutex> lock(m);
    int retIoctl = -1;
    IPCThreadState* self = IPCThreadState::self();
    pid_t pid = self->getCallingPid();

    ALOGD("Power OFF - PID = %d", pid);

    if (!pidsMap.count(pid)) {
        ALOGE("PID %d not registered !", pid);
        return RET_FAILURE;
    }
    if (pidsMap[pid].event_cb_ != nullptr && pidsMap[pid].mDeathRecipient != nullptr) {
        pidsMap[pid].event_cb_->unlinkToDeath(pidsMap[pid].mDeathRecipient);
    }
    pidsMap.erase(pid);
    if (!pidsMap.empty()) {
        return RET_FAILURE;
    }
    retIoctl = setIoctlState(ESE_SET_PWR, 1);
    if (retIoctl < 0) {
        ALOGE("%s: eSE ioctl failed : %s", __func__, strerror(errno));
        closeNode();
        return RET_FAILURE;
    }
    ALOGD("eSE powered OFF - PID = %d", pid);
    closeNode();
    return 0;
}

Return<int32_t> EsePowerManager::getState() {
    std::lock_guard<std::mutex> lock(m);
    int retIoctl = -1;

    retIoctl = getIoctlState(ESE_GET_PWR);
    if (retIoctl < 0) {
        ALOGE("%s: eSE ioctl failed : %s", __func__, strerror(errno));
        closeNode();
    }

    return (retIoctl <= 0) ? 0 : 1;
}

Return<int32_t> EsePowerManager::killall() {
    std::lock_guard<std::mutex> lock(m);
    int retIoctl = -1;

    pidsMap.clear();
    retIoctl = setIoctlState(ESE_SET_PWR, 1);
    if (retIoctl < 0) {
        ALOGE("%s: eSE ioctl failed : %s", __func__, strerror(errno));
        closeNode();
        return RET_FAILURE;
    }
    closeNode();
    ALOGD("eSE powered OFF");
    return 0;
}

Return<void> EsePowerManager::ClientDiedNotifier(pid_t mPid) {
    int retIoctl = -1;

    ALOGE("Client died - PID :  %d", mPid);
    pidsMap.erase(mPid);
    if (pidsMap.empty()) {
        if (nq_node != -1) {
            retIoctl = ioctl(nq_node, ESE_SET_PWR, 1);
        } else {
            ALOGE("NQ_NODE not opened - let's assume eSE is off");
            return Void();
        }
        if (retIoctl < 0) {
            ALOGE("%s: eSE ioctl failed : %s", __func__, strerror(errno));
        }
        ALOGD("eSE powered OFF, no more Pids in the map");
        closeNode();
    }
    return Void();
}

// Methods from ::vendor::qti::proprietary::esepowermanager::V1_1::IEsePowerManager follow.
Return<void> EsePowerManager::eseIoctl(uint64_t ioctlNum, const eseData& inputEseData, eseIoctl_cb _hidl_cb) {
    std::lock_guard<std::mutex> lock(m);
    int ret = -1;
    eseData inpEseData = {};
    eseData outputEseData = {};

    ret = openNode();
    if(ret < 0) {
        outputEseData.outputBufferData.bufferLength = 0;
        outputEseData.ioctlState = ret;
        _hidl_cb(outputEseData);
        return Void();
    }

    memcpy(&inpEseData, &inputEseData, sizeof(eseData));
    ret = ioctl(nq_node, ioctlNum, &inpEseData);
    if (ret < 0) {
        ALOGE("%s: eSE ioctl %llu failed : %s", __func__, ioctlNum, strerror(errno));
        closeNode();
        outputEseData.outputBufferData.bufferLength = 0;
        outputEseData.ioctlState = ret;
        _hidl_cb(outputEseData);
        return Void();
    }

    memcpy(&outputEseData, &inpEseData, sizeof(eseData));

    _hidl_cb(outputEseData);
    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IEsePowerManager* HIDL_FETCH_IEsePowerManager(const char* /* name */) {
    return new EsePowerManager();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace esepowermanager
}  // namespace qti
}  // namespace vendor
