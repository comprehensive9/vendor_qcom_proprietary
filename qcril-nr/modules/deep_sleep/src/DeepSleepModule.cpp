/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include "DeepSleepModule.h"
#include "modules/uim/UimDeepSleepNotifyMsg.h"
#include "framework/TimeKeeper.h"

#undef TAG
#define TAG "RILQ"

void qmi_ril_set_stay_suspended(bool stay_suspended);

namespace ril {
namespace modules {

bool convertToRilUimDeepSleepState(RIL_UIM_DeepSleepState& out,
                                   deep_sleep::PowerStateHandler::EventListener::State in) {
  bool result = true;
  switch (in) {
    case deep_sleep::PowerStateHandler::EventListener::State::ENTER:
      out = RIL_UIM_DEEP_SLEEP_MODE_ENTER;
      break;
    case deep_sleep::PowerStateHandler::EventListener::State::EXIT:
      out = RIL_UIM_DEEP_SLEEP_MODE_EXIT;
      break;
    default:
      result = false;
      break;
  }
  return result;
}

class DeepSleepEventListenerImpl : public deep_sleep::PowerStateHandler::EventListener {
 public:
  bool notifyDeepSleepEvent(State state) override;
};

bool DeepSleepEventListenerImpl::notifyDeepSleepEvent(State state) {
  QCRIL_LOG_DEBUG("notifyDeepSleepEvent: state = %d", state);
  qmi_ril_set_stay_suspended(state == State::ENTER);

  bool result = true;
  RIL_UIM_DeepSleepState uimDeepSleepState;
  if (convertToRilUimDeepSleepState(uimDeepSleepState, state)) {
    auto msg = std::make_shared<UimDeepSleepNotifyMsg>(uimDeepSleepState);
    std::shared_ptr<RIL_UIM_Errno> response;
    Message::Callback::Status status = msg->dispatchSync(response);
    if (status == Message::Callback::Status::SUCCESS && response) {
      if (*response != RIL_UIM_E_SUCCESS) {
        QCRIL_LOG_DEBUG("Received failure response for UimDeepSleepNotifyMsg");
        result = false;
      }
    } else {
      QCRIL_LOG_DEBUG("Invalid response received for UimDeepSleepNotifyMsg");
    }
  }
  QCRIL_LOG_DEBUG("notifyDeepSleepEvent: result = %d", result);
  return result;
}

DeepSleepModule::DeepSleepModule() {
  mName = "DeepSleepModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, DeepSleepModule::handleQcrilInit),
    HANDLER(RilUnsolRadioStateChangedMessage,
            DeepSleepModule::handleRilUnsolRadioStateChangedMessage),
    HANDLER(ImsServiceStatusInd, DeepSleepModule::handleImsServiceStatusInd),
  };
  mPowerHandler = deep_sleep::getPlatformPowerStateHandler();
  if (mPowerHandler) {
    mPowerHandler->registerForEvent(new DeepSleepEventListenerImpl);
  }
}

DeepSleepModule::~DeepSleepModule() {
}

void DeepSleepModule::init() {
  Module::init();
}

void DeepSleepModule::evaluate() {
  QCRIL_LOG_DEBUG("mRadioState = %d, mWlanRegistered = %d", mRadioState, mWlanRegistered);
  // Acquire the deep sleep lock when the device is in radio ON state or IMS is registered on WIFI.
  // Release the deep sleep lock when the device is in not in ON and the IMS on WIFI is not registered.
  if (mPowerHandler) {
    if (mRadioState == RADIO_STATE_ON || mWlanRegistered) {
      mPowerHandler->acquireLock();
    } else if (mRadioState != RADIO_STATE_ON && !mWlanRegistered) {
      mPowerHandler->releaseLock();
    }
  }
}

void DeepSleepModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mPowerHandler) {
    mPowerHandler->init();
  }
  QCRIL_LOG_FUNC_RETURN();
}

void DeepSleepModule::handleRilUnsolRadioStateChangedMessage(
    std::shared_ptr<RilUnsolRadioStateChangedMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  mRadioState = qcril_get_current_radio_state();
  evaluate();
  QCRIL_LOG_FUNC_RETURN();
}

void DeepSleepModule::handleImsServiceStatusInd(std::shared_ptr<ImsServiceStatusInd> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));
  auto ssInfo = msg->getServiceStatusInfo();
  bool isWlanRegistered = false;
  if (ssInfo.reg_status == ImsServiceStatusInd::RegistrationStatus::REGISTERED) {
    if ((ssInfo.voip_service_status == ImsServiceStatusInd::ServiceStatus::FULL_SERVICE) &&
        (ssInfo.voip_service_rat == ImsServiceStatusInd::ServiceRat::WLAN ||
         ssInfo.voip_service_rat == ImsServiceStatusInd::ServiceRat::IWLAN)) {
      isWlanRegistered = true;
    }
    if ((ssInfo.vt_service_status == ImsServiceStatusInd::ServiceStatus::FULL_SERVICE) &&
        (ssInfo.vt_service_rat == ImsServiceStatusInd::ServiceRat::WLAN ||
         ssInfo.vt_service_rat == ImsServiceStatusInd::ServiceRat::IWLAN)) {
      isWlanRegistered = true;
    }
  }
  // Add a timer to avoid frequent acquiring and releasing the lock in transient WLAN OOS cases
  if (mWlanRegistered && !isWlanRegistered) {
    if (mWlanSrvGuardTimer == TimeKeeper::no_timer) {
      mWlanSrvGuardTimer = TimeKeeper::getInstance().set_timer(
          [this](void* /*userdata*/) {
            mWlanSrvGuardTimer = TimeKeeper::no_timer;
            mWlanRegistered = false;
            evaluate();
          },
          nullptr,
          500);  // 500ms
    }
  } else {
    if (mWlanSrvGuardTimer != TimeKeeper::no_timer) {
      TimeKeeper::getInstance().clear_timer(mWlanSrvGuardTimer);
      mWlanSrvGuardTimer = TimeKeeper::no_timer;
    }
    mWlanRegistered = isWlanRegistered;
    evaluate();
  }
  QCRIL_LOG_FUNC_RETURN();
}

static load_module<DeepSleepModule> sDeepSleepModule;

}  // namespace modules
}  // namespace ril
