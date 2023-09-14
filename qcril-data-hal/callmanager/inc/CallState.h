/*===========================================================================

  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef CALLSTATE
#define CALLSTATE

#include <string>
#include <unordered_map>

#include "CallInfo.h"
#include "IState.h"
#include "LocalLogBuffer.h"
#include "framework/TimeKeeper.h"
#include "request/SetupDataCallRequestMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"

#ifdef QMI_RIL_UTF
#define SETUP_DATA_CALL_TIMEOUT_SECONDS 1
#define DEACTIVATE_DATA_CALL_TIMEOUT_SECONDS 1
#define DEFAULT_PARTIAL_RETRY_TIMEOUT 1
#else
#define SETUP_DATA_CALL_TIMEOUT_SECONDS 155
#define DEACTIVATE_DATA_CALL_TIMEOUT_SECONDS 90
#define DEFAULT_PARTIAL_RETRY_TIMEOUT 5
#endif

#define HANDOVER_TIMEOUT_SECONDS 85
#define DEFAULT_MAX_PARTIAL_RETRY_TIMEOUT 100

#define DUAL_IP_HO_ONLY_ONE_IP_HO_SUCCESS (callInfo.dualIpHandover && \
                (((callInfo.v4HandedOver == HandOverState::Failure) && (callInfo.v6HandedOver == HandOverState::Success)) || \
                ((callInfo.v6HandedOver == HandOverState::Failure) && (callInfo.v4HandedOver == HandOverState::Success))))

#define DUAL_IP_HO_BOTH_IP_HO_FAILURE (callInfo.dualIpHandover && \
                (( callInfo.v4HandedOver == HandOverState::Failure ) && ( callInfo.v6HandedOver == HandOverState::Failure )))

#define DUAL_IP_HO_BOTH_IP_HO_SUCCESS (callInfo.dualIpHandover && \
                ( callInfo.v4HandedOver == HandOverState::Success ) && ( callInfo.v6HandedOver == HandOverState::Success))

#define DUAL_IP_HO_ONLY_IPV4_HO_SUCCESS (callInfo.dualIpHandover && \
                (( callInfo.v6HandedOver == HandOverState::Failure ) && ( callInfo.v4HandedOver == HandOverState::Success )))

#define DUAL_IP_HO_ONLY_IPV6_HO_SUCCESS (callInfo.dualIpHandover && \
                (( callInfo.v4HandedOver == HandOverState::Failure ) && ( callInfo.v6HandedOver == HandOverState::Success )))

using namespace std;
using namespace SM;

namespace rildata {

enum CallStateEnum {
  _eIdle = 0,
  _eConnecting,
  _eConnected,
  _eHandover,
  _eDisconnecting,
  _eDisconnected
};

enum class CallEventTypeEnum {
  SetupDataCall = 0,
  DeactivateDataCall,
  RilEventDataCallback,
  TimerExpired,
  HandoverInformationInd,
  SetPreferredSystem,
  LinkPropertiesChanged,
  EnableEmbmsCall,
  CleanUp,
};

/**
 * @brief call event
 * @details call event to be processed by call state machine
 */
struct CallEventType {
  CallEventTypeEnum type;
  std::shared_ptr<Message> msg;
  std::string getEventName() const;
};

inline std::string CallEventType::getEventName() const {
  switch (type) {
    case CallEventTypeEnum::SetupDataCall:
      return "SetupDataCall";
    case CallEventTypeEnum::DeactivateDataCall:
      return "DeactivateDataCall";
    case CallEventTypeEnum::RilEventDataCallback:
      return "RilEventDataCallback";
    case CallEventTypeEnum::TimerExpired:
      return "TimerExpired";
    case CallEventTypeEnum::HandoverInformationInd:
      return "HandoverInformationInd";
    case CallEventTypeEnum::SetPreferredSystem:
      return "SetPreferredSystem";
    case CallEventTypeEnum::LinkPropertiesChanged:
      return "LinkPropertiesChanged";
    default:
      return "Undefined";
  }
}

/**
 * @brief CallState class
 * @details CallState class
 *
 * @param setName Call state name
 */
class CallState : public IState<CallEventType> {
protected:
  CallInfo& callInfo;
  GlobalCallInfo& globalInfo;
  LocalLogBuffer& logBuffer;
  unordered_map<DataCallTimerType, TimeKeeper::timer_id> timers;

  void cleanupKeepAlive(int cid);

  void timeoutHandler(DataCallTimerType type, void *data);
  void stopTimer(DataCallTimerType type);
  void startTimer(DataCallTimerType type, TimeKeeper::millisec timeout);
  void sendSetupDataCallResponse(std::shared_ptr<SetupDataCallRequestMessage> msg, const SetupDataCallResponse_t& result);
  void sendDeactivateDataCallResponse(std::shared_ptr<DeactivateDataCallRequestMessage> msg, const ResponseError_t& result);
  void setupDataCallCompleted(const SetupDataCallResponse_t& result);
  void setupDataCallFailed(const ResponseError_t& respErr, const DataCallFailCause_t& cause);
  void deactivateDataCallCompleted(const ResponseError_t& result);
  void configureParams(const IpFamilyType_t &ipType);
  void compareCallParams(const CallInfo& callInst);
  void createQmiWdsClients();
  void initializeQos();
  void deInitializeQos();
  bool setProfileInfo();
  bool setEmergencyProfileInfo();
  dsi_ce_reason_t pickTemporaryCallEndReason(dsi_ce_reason_t v4Reason, dsi_ce_reason_t v6Reason);
  dsi_ce_reason_t getVerboseCallEndReason();
  std::string getCallApn();
  std::unique_ptr<CallSetupProfile> createCallSetupProfile(std::shared_ptr<SetupDataCallRequestMessage> m);
  static void dsiNetCallback(
    dsi_hndl_t dsiHandle,
    void *userData,
    dsi_net_evt_t netEvent,
    dsi_evt_payload_t *payload
  );
  void dataCallListChanged();
  void determineIpTypeAndConfigureParams(IpFamilyType_t &ipType);
  void setCurrentRATAndConfigureParams(IpFamilyType_t &ipType);
#ifndef RIL_FOR_MDM_LE
  void setThirdPartyHOParams(std::vector<std::string> addr);
#endif

public:
  CallState(string setName, function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    IState(setName, fn), callInfo(setCallInfo), globalInfo(setGlobalInfo), logBuffer(setLogBuffer) {};
  static bool sendSetApnPreferredSystemRequest(std::shared_ptr<DSDModemEndPoint> dsd_endpoint, string apnName, HandoffNetworkType_t prefNetwork);
  static vector<LinkAddress_t> convertAddrStringToLinkAddresses(const string &addr);
  virtual void enter();
  virtual void exit();
  virtual ~CallState() = default;
};

class IdleState : public CallState
{
public:
  IdleState(function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    CallState("IdleState", fn, setCallInfo, setGlobalInfo, setLogBuffer){}
  virtual void enter();
  virtual void exit();
  virtual int handleEvent(const CallEventType &event);
};

class ConnectingState : public CallState
{
private:
  SetupDataCallResponse_t response{};

public:
  ConnectingState(function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    CallState("ConnectingState", fn, setCallInfo, setGlobalInfo, setLogBuffer){}
  virtual void enter();
  virtual void exit();
  virtual int handleEvent(const CallEventType &event);
};

class ConnectedState : public CallState {
private:
  int partialRetryCount;

  TimeKeeper::millisec getTimeoutMillis(unsigned long maxTimeoutMillis);
  bool isPartiallyConnected();
  void startPartialRetryTimerForTemporaryFailure();
  void startPartialRetryResponseTimer();
  void initiatePartialRetry();
public:
  ConnectedState(function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    CallState("ConnectedState", fn, setCallInfo, setGlobalInfo, setLogBuffer), partialRetryCount(0) {}
  virtual void enter();
  virtual void exit();
  virtual int handleEvent(const CallEventType &event);
};

class HandoverState : public CallState {
private:
  SetupDataCallResponse_t response{};

public:
  HandoverState(function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    CallState("HandoverState", fn, setCallInfo, setGlobalInfo, setLogBuffer) {}
  virtual void enter();
  virtual void exit();
  virtual int handleEvent(const CallEventType &event);
};

class DisconnectingState : public CallState {
private:
  ResponseError_t response{};
  uint8_t deactivateShutDownTimer = DEACTIVATE_DATA_CALL_TIMEOUT_SECONDS;
  void setDeactivateShutDownTimer();
public:
  DisconnectingState(function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    CallState("DisconnectingState", fn, setCallInfo, setGlobalInfo, setLogBuffer){setDeactivateShutDownTimer();}
  virtual void enter();
  virtual void exit();
  virtual int handleEvent(const CallEventType &event);
};

class DisconnectedState : public CallState {
public:
  DisconnectedState(function<void(int)> fn, CallInfo& setCallInfo, GlobalCallInfo& setGlobalInfo, LocalLogBuffer& setLogBuffer):
    CallState("DisconnectedState", fn, setCallInfo, setGlobalInfo, setLogBuffer){}
  virtual void enter();
  virtual void exit();
  virtual int handleEvent(const CallEventType &event);
};

} /* namespace rildata */

#endif
