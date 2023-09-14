/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/Log.h"

#include "CallState.h"
#include "CallStateMachine.h"

using namespace rildata;

CallStateMachine::CallStateMachine(int cid, LocalLogBuffer& setLogBuffer,
    GlobalCallInfo& globalInfo): logBuffer(setLogBuffer), globalInfo(globalInfo) {
  Log::getInstance().d("[CallSM_" + to_string(cid)+ "]: CallStateMachine");
  logBuffer.addLogWithTimestamp("[CallSM_" + to_string(cid)+ "]: CallStateMachine");
  mCallInfo = { .cid = cid};

  auto transitionFn = bind(&CallStateMachine::setCurrentState, this, placeholders::_1);
  shared_ptr<IdleState> IdleStateInstance(new IdleState(transitionFn, mCallInfo, globalInfo, logBuffer));
  shared_ptr<ConnectingState> ConnectingStateInstance(new ConnectingState(transitionFn, mCallInfo, globalInfo, logBuffer));
  shared_ptr<ConnectedState> ConnectedStateInstance(new ConnectedState(transitionFn, mCallInfo, globalInfo, logBuffer));
  shared_ptr<HandoverState> HandoverStateInstance(new HandoverState(transitionFn, mCallInfo, globalInfo, logBuffer));
  shared_ptr<DisconnectingState> DisconnectingStateInstance(new DisconnectingState(transitionFn, mCallInfo, globalInfo, logBuffer));
  shared_ptr<DisconnectedState> DisconnectedStateInstance(new DisconnectedState(transitionFn, mCallInfo, globalInfo, logBuffer));

  addState(_eIdle, IdleStateInstance);
  addState(_eConnecting, ConnectingStateInstance);
  addState(_eConnected, ConnectedStateInstance);
  addState(_eHandover, HandoverStateInstance);
  addState(_eDisconnecting, DisconnectingStateInstance);
  addState(_eDisconnected, DisconnectedStateInstance);

  currentState = _eIdle;
}

CallStateMachine::~CallStateMachine() {
  Log::getInstance().d("[CallSM_" + to_string(mCallInfo.cid)+ "]: ~CallStateMachine");
  logBuffer.addLogWithTimestamp("[CallSM_" + to_string(mCallInfo.cid)+ "]: ~CallStateMachine");
  if (mCallInfo.dsiHandle != nullptr) {
    dsiRelDataSrvcHndl(mCallInfo.dsiHandle);
    mCallInfo.dsiHandle = nullptr;
  }
  if (mCallInfo.profileInfo != nullptr) {
    if(mCallInfo.profileInfo->getUmtsProfileId() != QDP_PROFILE_ID_INVALID) {
      qdp_profile_release(mCallInfo.profileInfo->getUmtsProfileId());
    }
    if(mCallInfo.profileInfo->getCdmaProfileId() != QDP_PROFILE_ID_INVALID) {
      qdp_profile_release(mCallInfo.profileInfo->getCdmaProfileId());
    }
    mCallInfo.profileInfo = nullptr;
  }
}

/**
 * CallStateMachine::setCurrentState()
 *
 * @brief
 * Wrapper around BaseFiniteStateMachine::setCurrentState to add log message
 * for every state transition
 */
void CallStateMachine::setCurrentState(int stateId) {
  int currentId = BaseFiniteStateMachine<CallEventType>::getCurrentState();
  if (currentId != NULL_STATE && currentId != stateId) {
    auto nextState = getStateById(stateId);
    string stateName = (nextState == nullptr ? "NULL" : nextState->getName());
    Log::getInstance().d("[CallSM_" + to_string(mCallInfo.cid)+ "]: Going to " + stateName);
    logBuffer.addLogWithTimestamp("[CallSM_" + to_string(mCallInfo.cid)+ "]: Going to " + stateName);
  }
  BaseFiniteStateMachine<CallEventType>::setCurrentState(stateId);
  if (mCallStateChangedCb!=nullptr) {
    mCallStateChangedCb(mCallInfo.profileInfo->getApn(), (CallStateEnum)stateId);
  }
}

void CallStateMachine::setCallStateChangedCallback(std::function<void(string, CallStateEnum)> cb) {
  Log::getInstance().d("[CallSM_" + to_string(mCallInfo.cid)+ "]: setCallStateChangedCallback");
  mCallStateChangedCb = cb;
}

/**
 * CallStateMachine::setCurrentState()
 *
 * @brief
 * Wrapper around BaseFiniteStateMachine::processEvent to add log message
 * for every event
 */
int CallStateMachine::processEvent(const CallEventType& event) {
  int currentId = BaseFiniteStateMachine<CallEventType>::getCurrentState();
  if (currentId != NULL_STATE) {
    auto currState = getStateById(currentId);
    string stateName = (currState == nullptr ? "NULL" : currState->getName());
    string logMsg = "[CallSM_" + to_string(mCallInfo.cid)+ "]: Processing " +
        event.getEventName() + " in " + stateName + " msg=" +
        (event.msg == nullptr ? "null" : event.msg->dump());
    Log::getInstance().d(logMsg);
    logBuffer.addLogWithTimestamp(logMsg);
  }
  return BaseFiniteStateMachine<CallEventType>::processEvent(event);
}

CallInfo& CallStateMachine::getCallInfo() {
  return mCallInfo;
}

bool CallStateMachine::sendDormancyRequest() {
  bool status = false;
  if (mCallInfo.wds_v4_call_endpoint != nullptr) {
    status = mCallInfo.wds_v4_call_endpoint->goDormant();
  }
  if (mCallInfo.wds_v6_call_endpoint != nullptr) {
    status = mCallInfo.wds_v6_call_endpoint->goDormant();
  }

  return status;
}

void CallStateMachine::updateLinkStateChangeReport() {
  Log::getInstance().d("[CallStateMachine]: updateLinkStateChangeReport cid="+std::to_string((int)mCallInfo.cid));
  logBuffer.addLogWithTimestamp("[CallStateMachine]: updateLinkStateChangeReport cid="+std::to_string((int)mCallInfo.cid));
  if(mCallInfo.wds_v4_call_endpoint != nullptr) {
    mCallInfo.wds_v4_call_endpoint->setLinkStateChangeReport(globalInfo.linkStateChangeReport);
  }
  if(mCallInfo.wds_v6_call_endpoint != nullptr) {
    mCallInfo.wds_v6_call_endpoint->setLinkStateChangeReport(globalInfo.linkStateChangeReport);
  }
}

void CallStateMachine::dump(string padding, ostream& os) const {
  std::ignore = padding;
  std::ignore = os;
  int currentId = BaseFiniteStateMachine<CallEventType>::getCurrentState();
  string stateName = "NULL";
  if (currentId != NULL_STATE) {
    auto currState = getStateById(currentId);
    stateName = (currState == nullptr ? "NULL" : currState->getName());
  }
  os << padding << "CurrentState: " << stateName << endl;
  mCallInfo.dump(padding, os);
}
