/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <utils/Log.h>
#include "bluetooth_hci.h"
#include "bluetooth_address.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.bluetooth@1.0-bluetooth_hci"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

BluetoothHci::BluetoothHci()
  : deathRecipient(new BluetoothDeathRecipient(this))
{
}
Return<void> BluetoothHci::initialize(
  const ::android::sp<IBluetoothHciCallbacks>& cb)
{
  ALOGW("BluetoothHci::initialize()");
  if (cb == nullptr) {
    ALOGE("%s: Received NULL callback from BT client", __func__);
    return Void();
  }
  ::android::sp<IBluetoothHciCallbacks> event_cb_tmp;
  event_cb_tmp = cb;
  bool rc = DataHandler::Init( TYPE_BT,
    [this, event_cb_tmp](bool status) {
      if (event_cb_tmp != nullptr) {
        ALOGI("%s: Set callbacks received from BT client inorder "
               "to provide status and data through them", __func__);
        event_cb_ = event_cb_tmp;
      }
      if (event_cb_ != nullptr) {
        auto hidl_client_status = event_cb_->initializationComplete(
          status ? Status::SUCCESS : Status::INITIALIZATION_ERROR);
        if(!hidl_client_status.isOk()) {
          ALOGE("Client dead, callback initializationComplete failed");
        }
      }
    },
    [this, event_cb_tmp](HciPacketType type, const hidl_vec<uint8_t> *packet) {
      DataHandler *data_handler = DataHandler::Get();
      if (event_cb_tmp == nullptr) {
        ALOGE("BluetoothHci: event_cb_tmp is null");
        if (data_handler)
          data_handler->SetClientStatus(false, TYPE_BT);
        return;
      }
      /* Skip calling client callback when client is dead */
      if(data_handler && (data_handler->GetClientStatus(TYPE_BT) == false)) {
        ALOGV("%s: Skip calling client callback when client is dead", __func__);
        return;
      }
      Logger::Get()->UpdateRxTimeStamp();
      switch (type) {
        case HCI_PACKET_TYPE_EVENT:
        {
#ifdef DUMP_RINGBUF_LOG
          Logger::Get()->UpdateRxEventTag(RX_PRE_STACK_EVT_CALL_BACK);
#endif
          auto hidl_client_status = event_cb_tmp->hciEventReceived(*packet);
          if(!hidl_client_status.isOk()) {
            ALOGE("Client dead, callback hciEventReceived failed");
            if (data_handler)
              data_handler->SetClientStatus(false, TYPE_BT);
          }
#ifdef DUMP_RINGBUF_LOG
          Logger::Get()->UpdateRxEventTag(RX_POST_STACK_EVT_CALL_BACK);
#endif
        }
        break;
        case HCI_PACKET_TYPE_ACL_DATA:
        {
#ifdef DUMP_RINGBUF_LOG
          Logger::Get()->UpdateRxEventTag(RX_PRE_STACK_ACL_CALL_BACK);
#endif
          auto hidl_client_status = event_cb_tmp->aclDataReceived(*packet);
          if(!hidl_client_status.isOk()) {
            ALOGE("Client dead, callback aclDataReceived failed");
            if (data_handler)
              data_handler->SetClientStatus(false, TYPE_BT);
          }
#ifdef DUMP_RINGBUF_LOG
          Logger::Get()->UpdateRxEventTag(RX_POST_STACK_ACL_CALL_BACK);
#endif
        }
        break;
        default:
          ALOGE("%s Unexpected event type %d", __func__, type);
          break;
      }
    });
  if (!rc && (cb != nullptr)) {
    auto hidl_client_status = cb->initializationComplete(Status::INITIALIZATION_ERROR);
    if(!hidl_client_status.isOk()) {
      ALOGE("Client dead, callback initializationComplete failed");
    }
    ALOGE("BluetoothHci: error INITIALIZATION_ERROR");
  } else if (rc && (cb != nullptr)) {
     ALOGI("%s: linking to deathRecipient", __func__);
     cb->linkToDeath(deathRecipient, 0);
  }

  return Void();
}

Return<void> BluetoothHci::close()
{
  ALOGW("BluetoothHci::close()");
  if (event_cb_ != nullptr) event_cb_->unlinkToDeath(deathRecipient);
  DataHandler::CleanUp(TYPE_BT);
  ALOGW("BluetoothHci::close, finish cleanup");
  return Void();
}

Return<void> BluetoothHci::sendHciCommand(const hidl_vec<uint8_t>& command)
{
  sendDataToController(HCI_PACKET_TYPE_COMMAND, command);
  return Void();
}

Return<void> BluetoothHci::sendAclData(const hidl_vec<uint8_t>& data)
{
  sendDataToController(HCI_PACKET_TYPE_ACL_DATA, data);
  return Void();
}

Return<void> BluetoothHci::sendScoData(const hidl_vec<uint8_t>& data)
{
  sendDataToController(HCI_PACKET_TYPE_SCO_DATA, data);
  return Void();
}

Return<void>  BluetoothHci::debug(const hidl_handle& handle,
                                  const hidl_vec<hidl_string>&)
{
  ALOGI("%s: bugreport triggered for BT", __func__);

  if (handle == nullptr || handle->numFds < 1) {
    ALOGE("%s: Bugreport dumping failed, invalid file"
          " descriptor handle provided.", __func__);
    return Void();
  }

  Logger::is_bugreport_triggered = true;
  std::unique_lock<std::recursive_mutex> guard(Logger::bugreport_mutex);
  ALOGI("%s: checking is_crash_dump_in_progress_", __func__);
  if (Logger::is_crash_dump_in_progress_) {
    /* Scenario:fw crash dump is in progress and bugreport is triggered.
     * From preventing collection of wrong logs during crash dump, bugreport
     * has to wait until fw crash dump is done and all logs are collected */
    Logger::is_bugreport_triggered_during_crash_dump = true;
    /* unlocking as we will be collecting dump in another thread */
    guard.unlock();
    ALOGI(" Bugreport %s waiting as crash dump is in progress", __func__);
    pthread_mutex_lock(&Logger::crash_dump_lock);
    /* wait condition will be signaled once all logs collection is done */
    pthread_cond_wait(&Logger::crash_dump_cond, &Logger::crash_dump_lock);
  }

  int fd = handle->data[0];
  Logger::DumpLogsInBugreport(fd);

  Logger::is_bugreport_triggered = false;
  Logger::bugreport_wait_cv.notify_one();
  return Void();
}

void BluetoothHci::sendDataToController(HciPacketType type,
                                        const hidl_vec<uint8_t>& data)
{
  DataHandler *data_handler = DataHandler::Get();

  if(data_handler != nullptr) {
    data_handler->SendData(TYPE_BT, type, data.data(), data.size());
  }
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

IBluetoothHci* HIDL_FETCH_IBluetoothHci(const char* /* name */)
{
  return new BluetoothHci();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
