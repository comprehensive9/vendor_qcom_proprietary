/*===========================================================================

  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef CALLINFO
#define CALLINFO

#include <stdint.h>
#include <set>
#include <unordered_map>

#include "CallSetupProfile.h"
#include "WDSModemEndPoint.h"
#include "DSDModemEndPoint.h"
#include "WDSCallModemEndPoint.h"
#include "EmbmsCallHandler.h"

#include "DsiWrapper.h"

#define SuggestedRetryBackoffTime -1; // no value is suggested

using namespace std;

namespace rildata {

struct DsiEventDataType {
  dsi_net_evt_t netEvent;
  void *eventData;
  std::string getEventName() const;
};

class CallEndReason {
public:
  static DataCallFailCause_t getCallEndReason(dsi_ce_reason_t dsiReason);
  static bool isPermanentFailure(DataCallFailCause_t rilReason);
  static constexpr int AP_ASSIST_HANDOVER = -1;

private:
  CallEndReason();
  typedef unordered_map<int, DataCallFailCause_t> CallEndReasonMap_t;
  static unordered_map<dsi_ce_reason_type_t, CallEndReasonMap_t> callEndReasonTypeMap;
  static CallEndReasonMap_t mipReasonMap;
  static CallEndReasonMap_t internalReasonMap;
  static CallEndReasonMap_t cmReasonMap;
  static CallEndReasonMap_t _3gppReasonMap;
  static CallEndReasonMap_t pppReasonMap;
  static CallEndReasonMap_t ehrpdReasonMap;
  static CallEndReasonMap_t ipv6ReasonMap;
  static CallEndReasonMap_t handoffReasonMap;
  static CallEndReasonMap_t apAssistHandoffReasonMap;
};

struct GlobalCallInfo {
  bool linkStateChangeReport;
  bool partialRetryEnabled;
  unsigned long maxPartialRetryTimeout;
  bool isRoaming;
  bool apAssistMode;
  int prevDefaultProfileId;
  int currDefaultProfileId;
};

/**
 * @brief Call information class
 * @details All related information for one data call is maintained
 * This object is created when a data call bring up is requested and deleted
 * when its call is terminated
  */
struct CallInfo {
  int cid;
  LinkActiveState active;
  bool dormantState;
  bool v4Connected;
  bool v6Connected;
  bool dualIpHandover;
  HandOverState v4HandedOver;
  HandOverState v6HandedOver;
  bool intentToChangeInProgress;
  bool thirdPartyHOParamsSet;
  bool fallbackInProgress;
  bool isEmbmsCall;
  bool pendingAck;
  BringUpCapability callBringUpCapability;
  HandoffNetworkType_t currentRAT;
  HandoffNetworkType_t preferredRAT;
  RadioAccessFamily_t radioRAT;
  int transportType;
  timer_t handoverTimerId;
  string deviceName;
  string ipAddresses;
  string gatewayAddresses;
  string dnsAddresses;
  string pcscfAddresses;
  string ipType;
  string requestedIpType;
  string requestedRoamingIpType;
  int32_t mtu;
  int32_t mtuV4;
  int32_t mtuV6;
  bool callParamsChanged;
  DataCallFailCause_t cause;

  dsi_hndl_t dsiHandle;
  qcril_evt_e_type eventId;

  std::unique_ptr<CallSetupProfile> profileInfo;
  std::shared_ptr<Message> request;
  AllocatedBearer_t bearerCollection;

  std::unique_ptr<WDSCallModemEndPoint> wds_v4_call_endpoint;
  std::unique_ptr<WDSCallModemEndPoint> wds_v6_call_endpoint;
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::shared_ptr<EmbmsCallHandler> embmsInfo;
  std::function<void()> dataCallListChangedCallback;
  std::shared_ptr<std::function<void(CallInfo *)>> embmsCallCompleteCallback;
  vector<QosSession_t> qosSessions;
  std::vector<std::string> thirdPartyHOAddr;
  void dump(string padding, ostream& os) const;

  std::function<void(int)> cleanupKeepAliveCallback;
};

} /* namespace rildata */

#endif
