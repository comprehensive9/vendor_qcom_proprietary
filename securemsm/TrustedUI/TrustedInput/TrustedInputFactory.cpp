/*
 * Copyright (c) 2020 - 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "TrustedInput.h"
#include "TouchInputVM.h"
#include "TrustedUIFactory.h"
#include "utils/Log.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {
namespace V1_2 {
namespace implementation {

using ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput;

std::shared_ptr<ITrustedInput> TrustedInputFactory::GetInstance(std::string implInstance) {
    if (implInstance.compare("qtee-vm") == 0) {
        return TouchInput::getInstance();
    }
    else if (implInstance.compare("qtee-tz") == 0) {
        /* Default implementation is TZ based */
        return TrustedInput::getInstance();
    }
    else {
        ALOGE("No instance registered with this interface");
        return nullptr;
    }
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
