/*===========================================================================

  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef CALLSTATEMACHINE
#define CALLSTATEMACHINE

#include <string>

#include "CallState.h"
#include "BaseFiniteStateMachine.h"
#include "LocalLogBuffer.h"

using namespace std;
using namespace SM;

namespace rildata {

struct CallEventType;
/**
 * @brief CallStateMachine class
 * @details CallStateMachine class
 */
class CallStateMachine : public BaseFiniteStateMachine<CallEventType> {
private:
  LocalLogBuffer& logBuffer;
  GlobalCallInfo& globalInfo;
  CallInfo mCallInfo;
  std::function<void(string, CallStateEnum)> mCallStateChangedCb = nullptr;

public:
  CallStateMachine(int cid, LocalLogBuffer& logBuffer, GlobalCallInfo& globalInfo);
  ~CallStateMachine();
  void setCurrentState(int stateId) override;
  int processEvent(const CallEventType& event) override;

  CallInfo& getCallInfo();
  bool sendDormancyRequest();
  void updateLinkStateChangeReport();
  void setCallStateChangedCallback(std::function<void(string, CallStateEnum)> cb);
  void dump(string padding, ostream& os) const override;
};

} /* namespace rildata */

#endif
