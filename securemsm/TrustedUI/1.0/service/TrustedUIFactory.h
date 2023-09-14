/******************************************************************************
 * Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/

#ifndef TRUSTEDUI_FACTORY_H
#define TRUSTEDUI_FACTORY_H

#include <string>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <vendor/qti/hardware/trustedui/1.2/ITrustedUI.h>
#include <vendor/qti/hardware/trustedui/1.1/ITrustedInput.h>
using namespace std;

namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {
namespace V1_2 {
namespace implementation {

using ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput;

struct TrustedUIFactory {
    static std::shared_ptr<ITrustedUI> GetInstance(string tuiImplInstance);
};

struct TrustedInputFactory {
    static std::shared_ptr<ITrustedInput> GetInstance(string tuiInputImplInstance);
};


}  // namespace implementation
}  // namespace V1_2
}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif
