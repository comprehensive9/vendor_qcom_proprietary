/*
 * Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TRUSTEDINPUT_H
#define TRUSTEDINPUT_H

#include <fcntl.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <memory>
#include <string>
#include <thread>
#include <vendor/qti/hardware/trustedui/1.1/ITrustedInput.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {
namespace V1_2 {
namespace implementation {

using ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput;
using ::vendor::qti::hardware::trustedui::V1_0::ITrustedInputCallback;
using ::vendor::qti::hardware::trustedui::V1_0::Response;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_string;
using ::android::sp;

struct TrustedInput : public ITrustedInput {
    // Methods from ::vendor::qti::hardware::trustedui::V1_0::ITrustedInput follow.
    Return<void> init(const sp<ITrustedInputCallback>& cb, init_cb _hidl_cb) override;
    Return<Response> terminate() override;
    Return<Response> getInput(int32_t timeout) override;
    // Methods from ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput follow.
    Return<void> init2(const sp<ITrustedInputCallback> &cb,
                     const hidl_string &displayType,
                     init2_cb _hidl_cb) override;

    static std::shared_ptr<TrustedInput> getInstance();

    // Methods from ::android::hidl::base::V1_0::IBase follow.

private:
    static std::mutex sLock;
    static std::shared_ptr<TrustedInput> sInstance;
    std::shared_ptr<std::thread> mThread = nullptr;
    int32_t mAbortFd = -1;
    sp<ITrustedInputCallback> mCB;
    uint8_t stSession = false;
    int mControlFd = -1;
    int mIrqFd = -1;
    int32_t mTouchControllerId = -1;
    std::string mControlFile;
    std::string mIrqFile;

    void _waitForInputThreadFunc(int32_t _timeout);
    int32_t _stStartSession(void);
    int32_t _stTerminateSession(void);
};

}  // namespace implementation
}  // namespace V1_2
}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif
