/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef COM_QUALCOMM_QTI_ANT_V1_0_ANTHCI_H
#define COM_QUALCOMM_QTI_ANT_V1_0_ANTHCI_H

#include <com/dsi/ant/1.0/IAnt.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "controller.h"
#include "data_handler.h"
#include <utils/Log.h>


namespace com {
namespace qti {
namespace ant {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using namespace com::dsi::ant::V1_0;

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;

using android::hardware::hidl_death_recipient;

using android::hardware::bluetooth::V1_0::implementation::DataHandler;

class AntDeathRecipient;

struct Ant : public IAnt {
 public:
  Ant();
  Return<void> getProperties(getProperties_cb _hidl_cb) override;
  Return<void> setCallbacks(const sp<IAntCallbacks>& callbacks) override;
  Return<void> translateStatus(Status status, translateStatus_cb _hidl_cb) override;
  Return<Status> enable() override;
  Return<Status> disable() override;
  Return<Status> sendCommandMessage(const hidl_vec<uint8_t>& command) override;
  Return<Status> sendDataMessage(const hidl_vec<uint8_t>& command) override;
 private:
  void sendDataToController(HciPacketType type, const hidl_vec<uint8_t>& data);
  ::android::sp<IAntCallbacks> event_cb_;
  ::android::sp<AntDeathRecipient> deathRecipient;
  std::function<void(sp<AntDeathRecipient>&)> unlink_cb_;
 };

 // Methods from ::android::hidl::base::V1_0::IBase follow.
 extern "C" IAnt* HIDL_FETCH_IAnt(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace ant
}  // namespace qti
}  // namespace com

#endif  // COM_QTI_ANT_V1_0_ANT_H
