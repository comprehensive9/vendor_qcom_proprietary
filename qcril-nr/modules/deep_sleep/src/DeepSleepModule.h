/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#ifndef __RIL_ANDROIDDEEPSLEEPMODULE_H_
#define __RIL_ANDROIDDEEPSLEEPMODULE_H_

#include "framework/Module.h"
#include "framework/QcrilInitMessage.h"
#include "interfaces/nas/RilUnsolRadioStateChangedMessage.h"
#include "modules/ims/ImsServiceStatusInd.h"
#include "QtiMutex.h"
#include "PowerStateHandler.h"

namespace ril {
namespace modules {

class DeepSleepModule : public Module {
 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleRilUnsolRadioStateChangedMessage(std::shared_ptr<RilUnsolRadioStateChangedMessage> msg);
  void handleImsServiceStatusInd(std::shared_ptr<ImsServiceStatusInd> msg);

  void evaluate();

  RIL_RadioState mRadioState = RADIO_STATE_UNAVAILABLE;
  bool mWlanRegistered = false;
  TimeKeeper::timer_id mWlanSrvGuardTimer = TimeKeeper::no_timer;
  deep_sleep::PowerStateHandler* mPowerHandler = nullptr;

 public:
  DeepSleepModule();
  ~DeepSleepModule();
  void init();
};

}  // namespace modules
}  // namespace ril

#endif  // __RIL_ANDROIDDEEPSLEEPMODULE_H_
