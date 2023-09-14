/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "AntHci.h"
#include <semaphore.h>
#define LOG_TAG "com.dsi.ant@1.0-Ant"
static volatile bool hidl_init = false;
namespace com {
namespace qti {
namespace ant {
namespace V1_0 {
namespace implementation {
static const ImplProps props = {
    "com.dsi.ant.qti.1.0",
    OptionFlags::USE_KEEPALIVE | OptionFlags::USE_ANT_FLOW_CONTROL,
};
static const Status GENERIC_ANT_FAILURE = (Status)1;
class AntDeathRecipient : public hidl_death_recipient {
    public:
       AntDeathRecipient(const sp<IAnt> hci) : mHci(hci) {}
       virtual void serviceDied(
            uint64_t /*cookie*/,
            const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
                ALOGE("AntDeathRecipient::serviceDied - Ant service died");
                has_died_ = true;
                mHci->disable();
            }
       sp<IAnt> mHci;
       bool getHasDied() const { return has_died_; }
       void setHasDied(bool has_died) { has_died_ = has_died; }
    private:
       bool has_died_;
};

Ant::Ant()
    : deathRecipient(new AntDeathRecipient(this)) {}

Return<void> Ant::getProperties(getProperties_cb _hidl_cb) {
    _hidl_cb(props);
    return Void();
}

Return<void> Ant::setCallbacks(const sp<IAntCallbacks>& cb) {
    ALOGI("Ant::setCallbacks");
    event_cb_ = cb;
    return Void();
}

Return<void> Ant::translateStatus(Status status, translateStatus_cb _hidl_cb) {
    return Void();
}
// Methods from ::com::dsi::ant::V1_0::IAnt follow.

Return<Status> Ant::enable() {
    sem_t s_cond;
    ALOGI("Ant::enable()");
    if (event_cb_== nullptr) {
       ALOGE("cb == nullptr! -> Unable to call initializationComplete(ERR)");
    }
    deathRecipient->setHasDied(false);
    event_cb_->linkToDeath(deathRecipient, 0);
    sem_init(&s_cond, 0, 0);
    bool rc = DataHandler::Init( TYPE_ANT,
        [this,&s_cond](bool status) {
            ALOGI("Ant::init status :%d and sending this to client now",status);
            hidl_init = status;
            sem_post(&s_cond);
            return (Status)CommonStatusCodes::OK;
        },
        [this](HciPacketType type, const hidl_vec<uint8_t> *packet) {
          if(event_cb_ == nullptr) return;
          DataHandler *data_handler = DataHandler::Get();
          if (data_handler && (data_handler->GetClientStatus(TYPE_ANT) == false)) {
            return;
          }
          ALOGD("%s event type: %d", __func__, type);
          switch (type) {
              case HCI_PACKET_TYPE_ANT_CTRL: {
                 std::vector<uint8_t> newcommand = std::vector<uint8_t> (packet->begin()
                                                   , packet->end());
                 std::vector<uint8_t>::iterator it;
                 newcommand.erase(newcommand.begin());
                 auto hidl_status = event_cb_->onMessageReceived(newcommand);
                 if (!hidl_status.isOk()) {
                    ALOGE("Client dead, callback antControlReceived failed");
                    if (data_handler)
                       data_handler->SetClientStatus(false, TYPE_ANT);
                 }
              }
              break;
              case HCI_PACKET_TYPE_ANT_DATA: {
                 uint16_t len = packet->size();
                 std::vector<uint8_t> newcommand = std::vector<uint8_t> (packet->begin(),
                                                   packet->end());
                 std::vector<uint8_t>::iterator it;
                 newcommand.erase(newcommand.begin());
                 auto hidl_status = event_cb_->onMessageReceived(newcommand);
                 if (!hidl_status.isOk()) {
                   ALOGE("Client dead, callback antDataReceived failed");
                   if (data_handler)
                     data_handler->SetClientStatus(false, TYPE_ANT);
                 }
              }
              break;
              default:
              ALOGE("%s Unexpected event type %d", __func__, type);
              break;
           }
     });

    unlink_cb_ = [this](sp<AntDeathRecipient>& deathRecipient) {
       if (deathRecipient->getHasDied()){
           ALOGI("Skipping unlink call, service died.");
       } else {
           event_cb_->unlinkToDeath(deathRecipient);
           ALOGI(" unlink  to death recipient ");
       }
    };

    sem_wait(&s_cond);
    ALOGI( "Ant::enable() returns now." );
    return (hidl_init ? (Status)CommonStatusCodes::OK : GENERIC_ANT_FAILURE);
}

Return<Status> Ant::sendCommandMessage(const hidl_vec<uint8_t>& command) {
    uint16_t len = command.size();
    std::vector<uint8_t> newcommand = std::vector<uint8_t> (command.begin(),command.end());
    std::vector<uint8_t>::iterator it;
    it = newcommand.begin();
    newcommand.insert(it , len);
    sendDataToController(HCI_PACKET_TYPE_ANT_CTRL, newcommand);
    return (Status)CommonStatusCodes::OK;
}

Return<Status> Ant::sendDataMessage(const hidl_vec<uint8_t>& command) {
    uint16_t len = command.size();
    std::vector<uint8_t> newcommand = std::vector<uint8_t> (command.begin(),command.end());
    std::vector<uint8_t>::iterator it;
    it = newcommand.begin();
    newcommand.insert(it , len);
    sendDataToController(HCI_PACKET_TYPE_ANT_DATA, newcommand);
    return (Status)CommonStatusCodes::OK;
}

Return<Status> Ant::disable() {
    ALOGI("Ant::close()");
    DataHandler::CleanUp(TYPE_ANT);
    if (unlink_cb_ != nullptr) unlink_cb_(deathRecipient);
    return (Status)CommonStatusCodes::OK;
}

void Ant::sendDataToController(HciPacketType type,
                                        const hidl_vec<uint8_t>& data)
{
    DataHandler *data_handler = DataHandler::Get();
    if(data_handler != nullptr) {
      data_handler->SendData(TYPE_ANT, type, data.data(), data.size());
    }
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IAnt* HIDL_FETCH_IAnt(const char* /* name */) {
    ALOGI("HIDL_FETCH_IAnt");
    return new Ant();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace ant
}  // namespace qti
}  // namespace com
