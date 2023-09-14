/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "RILC"
#define TAG LOG_TAG

#include <vector>
#include <framework/IxErrno.h>
#include <telephony/ril.h>
#include <utils_common.h>
#include <ril_utils.h>
#include <Endian.h>

#define HIGHER_4BIT(data) (((data) & 0xf0) >> 4)
#define LOWER_4BIT(data)  ((data) & 0x0f)
#define INVALID_HEX_CHAR  0
#include <interfaces/voice/QcRilUnsolImsHandoverMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceInfoMessage.h>
#include <interfaces/ims/QcRilUnsolImsPendingMultiLineStatus.h>
#include <interfaces/ims/QcRilUnsolImsMultiIdentityStatusMessage.h>

bool isServiceTypeCfQuery(RIL_SsServiceType serType, RIL_SsRequestType reqType) {
    if ((reqType == SS_INTERROGATION) &&
        (serType == SS_CFU ||
         serType == SS_CF_BUSY ||
         serType == SS_CF_NO_REPLY ||
         serType == SS_CF_NOT_REACHABLE ||
         serType == SS_CF_ALL ||
         serType == SS_CF_ALL_CONDITIONAL)) {
        return true;
    }
    return false;
}

namespace ril {
namespace socket {
namespace utils {

template <>
void convertToSocket<char, std::string>(char*& out, size_t& outlen, const std::string& in) {
  out = nullptr;
  outlen = 0;
  if (in.size()) {
    out = new char[in.size() + 1]{};
    if (out) {
      std::copy(in.begin(), in.end(), out);
      outlen = in.size() + 1;
    }
  }
}

template <>
void convertToSocket(char*& out, const std::string& in) {
  size_t outsz = 0;
  convertToSocket(out, outsz, in);
}

template <>
void convertToSocket(RIL_IMS_SipErrorInfo& out, const qcril::interfaces::SipErrorInfo& in) {
  convertToSocket(out.errorCode, in.getErrorCodeRef());
  convertToSocket(out.errorString, in.getErrorStringRef());
}

template <>
void convertToSocket(RIL_IMS_CallType& out, const qcril::interfaces::CallType& in) {
  switch (in) {
    case qcril::interfaces::CallType::UNKNOWN:
      out = RIL_IMS_CALL_TYPE_UNKNOWN;
      break;
    case qcril::interfaces::CallType::VOICE:
      out = RIL_IMS_CALL_TYPE_VOICE;
      break;
    case qcril::interfaces::CallType::VT_TX:
      out = RIL_IMS_CALL_TYPE_VT_TX;
      break;
    case qcril::interfaces::CallType::VT_RX:
      out = RIL_IMS_CALL_TYPE_VT_RX;
      break;
    case qcril::interfaces::CallType::VT:
      out = RIL_IMS_CALL_TYPE_VT;
      break;
    case qcril::interfaces::CallType::VT_NODIR:
      out = RIL_IMS_CALL_TYPE_VT_NODIR;
      break;
    case qcril::interfaces::CallType::SMS:
      out = RIL_IMS_CALL_TYPE_SMS;
      break;
    case qcril::interfaces::CallType::UT:
      out = RIL_IMS_CALL_TYPE_UT;
      break;
    case qcril::interfaces::CallType::USSD:
      out = RIL_IMS_CALL_TYPE_USSD;
      break;
    case qcril::interfaces::CallType::CALLCOMPOSER:
      out = RIL_IMS_CALL_TYPE_CALLCOMPOSER;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_RegistrationState& out, const qcril::interfaces::RegState& in) {
  switch (in) {
    case qcril::interfaces::RegState::UNKNOWN:
      out = RIL_IMS_REG_STATE_UNKNOWN;
      break;
    case qcril::interfaces::RegState::NOT_REGISTERED:
      out = RIL_IMS_REG_STATE_NOT_REGISTERED;
      break;
    case qcril::interfaces::RegState::REGISTERING:
      out = RIL_IMS_REG_STATE_REGISTERING;
      break;
    case qcril::interfaces::RegState::REGISTERED:
      out = RIL_IMS_REG_STATE_REGISTERED;
      break;
  }
}

template <>
void convertToSocket(uint8_t& out, const bool& in) {
  out = in ? 1 : 0;
}

template <>
void convertToSocket(RIL_IMS_CallState& out, const qcril::interfaces::CallState& in) {
  switch (in) {
    case qcril::interfaces::CallState::UNKNOWN:
      out = RIL_IMS_CALLSTATE_UNKNOWN;
      break;
    case qcril::interfaces::CallState::ACTIVE:
      out = RIL_IMS_CALLSTATE_ACTIVE;
      break;
    case qcril::interfaces::CallState::HOLDING:
      out = RIL_IMS_CALLSTATE_HOLDING;
      break;
    case qcril::interfaces::CallState::DIALING:
      out = RIL_IMS_CALLSTATE_DIALING;
      break;
    case qcril::interfaces::CallState::ALERTING:
      out = RIL_IMS_CALLSTATE_ALERTING;
      break;
    case qcril::interfaces::CallState::INCOMING:
      out = RIL_IMS_CALLSTATE_INCOMING;
      break;
    case qcril::interfaces::CallState::WAITING:
      out = RIL_IMS_CALLSTATE_WAITING;
      break;
    case qcril::interfaces::CallState::END:
      out = RIL_IMS_CALLSTATE_END;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_CallDomain& out, const qcril::interfaces::CallDomain& in) {
  switch (in) {
    case qcril::interfaces::CallDomain::UNKNOWN:
      out = RIL_IMS_CALLDOMAIN_UNKNOWN;
      break;
    case qcril::interfaces::CallDomain::CS:
      out = RIL_IMS_CALLDOMAIN_CS;
      break;
    case qcril::interfaces::CallDomain::PS:
      out = RIL_IMS_CALLDOMAIN_PS;
      break;
    case qcril::interfaces::CallDomain::AUTOMATIC:
      out = RIL_IMS_CALLDOMAIN_AUTOMATIC;
      break;
    case qcril::interfaces::CallDomain::NOT_SET:
      out = RIL_IMS_CALLDOMAIN_NOT_SET;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_CallModifiedCause& out,
                     const qcril::interfaces::CallModifiedCause& in) {
  switch (in) {
    case qcril::interfaces::CallModifiedCause::NONE:
      out = RIL_IMS_CMODCAUSE_NONE;
      break;
    case qcril::interfaces::CallModifiedCause::UPGRADE_DUE_TO_LOCAL_REQ:
      out = RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ;
      break;
    case qcril::interfaces::CallModifiedCause::UPGRADE_DUE_TO_REMOTE_REQ:
      out = RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_LOCAL_REQ:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_REMOTE_REQ:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_RTP_TIMEOUT:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_QOS:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_QOS;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_PACKET_LOSS:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_LOW_THRPUT:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_THERM_MITIGATION:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_LIPSYNC:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC;
      break;
    case qcril::interfaces::CallModifiedCause::DOWNGRADE_DUE_TO_GENERIC_ERROR:
      out = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_RttMode& out, const qcril::interfaces::RttMode& in) {
  switch (in) {
    case qcril::interfaces::RttMode::UNKNOWN:
      out = RIL_IMS_RTT_DISABLED;
      break;
    case qcril::interfaces::RttMode::DISABLED:
      out = RIL_IMS_RTT_DISABLED;
      break;
    case qcril::interfaces::RttMode::FULL:
      out = RIL_IMS_RTT_FULL;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_StatusType& out, const qcril::interfaces::StatusType& in) {
  switch (in) {
    case qcril::interfaces::StatusType::UNKNOWN:
      out = RIL_IMS_STATUS_DISABLED;
      break;
    case qcril::interfaces::StatusType::DISABLED:
      out = RIL_IMS_STATUS_DISABLED;
      break;
    case qcril::interfaces::StatusType::PARTIALLY_ENABLED:
      out = RIL_IMS_STATUS_PARTIALLY_ENABLED;
      break;
    case qcril::interfaces::StatusType::ENABLED:
      out = RIL_IMS_STATUS_ENABLED;
      break;
    case qcril::interfaces::StatusType::NOT_SUPPORTED:
      out = RIL_IMS_STATUS_NOT_SUPPORTED;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_Registration& out, const qcril::interfaces::Registration& in) {
  if (in.hasState()) {
    convertToSocket(out.state, in.getStateRef());
  }
  if (in.hasErrorCode()) {
    convertToSocket(out.errorCode, in.getErrorCodeRef());
  }
  if (in.hasErrorMessage()) {
    convertToSocket(out.errorMessage, in.getErrorMessageRef());
  }
  if (in.hasRadioTechnology()) {
    convertToSocket(out.radioTech, in.getRadioTechnologyRef());
  }
  if (in.hasPAssociatedUris()) {
    convertToSocket(out.pAssociatedUris, in.getPAssociatedUrisRef());
  }
}

template <>
void convertToSocket(RIL_IMS_AccessTechnologyStatus& out,
                     const qcril::interfaces::AccessTechnologyStatus& in) {
  if (in.hasNetworkMode()) {
    out.networkMode = in.getNetworkMode();
  }
  if (in.hasStatusType()) {
    convertToSocket(out.status, in.getStatusType());
  }
  if (in.hasRestrictCause()) {
    out.restrictCause = in.getRestrictCause();
  }
  if (in.hasRegistration()) {
    convertToSocket(out.registration, in.getRegistration());
  }
}

template <>
void convertToSocket(RIL_IMS_ServiceStatusInfo& out,
                     const qcril::interfaces::ServiceStatusInfo& in) {
  if (in.hasCallType()) {
    convertToSocket(out.callType, in.getCallType());
  }
  if (in.hasAccessTechnologyStatus()) {
    convertToSocket(out.accTechStatus, in.getAccessTechnologyStatus());
  }
  if (in.hasRttMode()) {
    convertToSocket(out.rttMode, in.getRttMode());
  }
}

template <>
void convertToSocket(RIL_IMS_CallFailCause& out, const qcril::interfaces::CallFailCause& in) {
  switch (in) {
    case qcril::interfaces::CallFailCause::UNKNOWN:
      out = RIL_IMS_FAILCAUSE_UNKNOWN;
      break;
    case qcril::interfaces::CallFailCause::UNOBTAINABLE_NUMBER:
      out = RIL_IMS_FAILCAUSE_UNOBTAINABLE_NUMBER;
      break;
    case qcril::interfaces::CallFailCause::NO_ROUTE_TO_DESTINATION:
      out = RIL_IMS_FAILCAUSE_NO_ROUTE_TO_DESTINATION;
      break;
    case qcril::interfaces::CallFailCause::CHANNEL_UNACCEPTABLE:
      out = RIL_IMS_FAILCAUSE_CHANNEL_UNACCEPTABLE;
      break;
    case qcril::interfaces::CallFailCause::OPERATOR_DETERMINED_BARRING:
      out = RIL_IMS_FAILCAUSE_OPERATOR_DETERMINED_BARRING;
      break;
    case qcril::interfaces::CallFailCause::NORMAL:
      out = RIL_IMS_FAILCAUSE_NORMAL;
      break;
    case qcril::interfaces::CallFailCause::BUSY:
      out = RIL_IMS_FAILCAUSE_BUSY;
      break;
    case qcril::interfaces::CallFailCause::NO_USER_RESPONDING:
      out = RIL_IMS_FAILCAUSE_NO_USER_RESPONDING;
      break;
    case qcril::interfaces::CallFailCause::NO_ANSWER_FROM_USER:
      out = RIL_IMS_FAILCAUSE_NO_ANSWER_FROM_USER;
      break;
    case qcril::interfaces::CallFailCause::SUBSCRIBER_ABSENT:
      out = RIL_IMS_FAILCAUSE_SUBSCRIBER_ABSENT;
      break;
    case qcril::interfaces::CallFailCause::CALL_REJECTED:
      out = RIL_IMS_FAILCAUSE_CALL_REJECTED;
      break;
    case qcril::interfaces::CallFailCause::NUMBER_CHANGED:
      out = RIL_IMS_FAILCAUSE_NUMBER_CHANGED;
      break;
    case qcril::interfaces::CallFailCause::PREEMPTION:
      out = RIL_IMS_FAILCAUSE_PREEMPTION;
      break;
    case qcril::interfaces::CallFailCause::DESTINATION_OUT_OF_ORDER:
      out = RIL_IMS_FAILCAUSE_DESTINATION_OUT_OF_ORDER;
      break;
    case qcril::interfaces::CallFailCause::INVALID_NUMBER_FORMAT:
      out = RIL_IMS_FAILCAUSE_INVALID_NUMBER_FORMAT;
      break;
    case qcril::interfaces::CallFailCause::FACILITY_REJECTED:
      out = RIL_IMS_FAILCAUSE_FACILITY_REJECTED;
      break;
    case qcril::interfaces::CallFailCause::RESP_TO_STATUS_ENQUIRY:
      out = RIL_IMS_FAILCAUSE_RESP_TO_STATUS_ENQUIRY;
      break;
    case qcril::interfaces::CallFailCause::NORMAL_UNSPECIFIED:
      out = RIL_IMS_FAILCAUSE_NORMAL_UNSPECIFIED;
      break;
    case qcril::interfaces::CallFailCause::CONGESTION:
      out = RIL_IMS_FAILCAUSE_CONGESTION;
      break;
    case qcril::interfaces::CallFailCause::NETWORK_OUT_OF_ORDER:
      out = RIL_IMS_FAILCAUSE_NETWORK_OUT_OF_ORDER;
      break;
    case qcril::interfaces::CallFailCause::TEMPORARY_FAILURE:
      out = RIL_IMS_FAILCAUSE_TEMPORARY_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::SWITCHING_EQUIPMENT_CONGESTION:
      out = RIL_IMS_FAILCAUSE_SWITCHING_EQUIPMENT_CONGESTION;
      break;
    case qcril::interfaces::CallFailCause::ACCESS_INFORMATION_DISCARDED:
      out = RIL_IMS_FAILCAUSE_ACCESS_INFORMATION_DISCARDED;
      break;
    case qcril::interfaces::CallFailCause::REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE:
      out = RIL_IMS_FAILCAUSE_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::RESOURCES_UNAVAILABLE_OR_UNSPECIFIED:
      out = RIL_IMS_FAILCAUSE_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED;
      break;
    case qcril::interfaces::CallFailCause::QOS_UNAVAILABLE:
      out = RIL_IMS_FAILCAUSE_QOS_UNAVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::REQUESTED_FACILITY_NOT_SUBSCRIBED:
      out = RIL_IMS_FAILCAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED;
      break;
    case qcril::interfaces::CallFailCause::INCOMING_CALLS_BARRED_WITHIN_CUG:
      out = RIL_IMS_FAILCAUSE_INCOMING_CALLS_BARRED_WITHIN_CUG;
      break;
    case qcril::interfaces::CallFailCause::BEARER_CAPABILITY_NOT_AUTHORIZED:
      out = RIL_IMS_FAILCAUSE_BEARER_CAPABILITY_NOT_AUTHORIZED;
      break;
    case qcril::interfaces::CallFailCause::BEARER_CAPABILITY_UNAVAILABLE:
      out = RIL_IMS_FAILCAUSE_BEARER_CAPABILITY_UNAVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::SERVICE_OPTION_NOT_AVAILABLE:
      out = RIL_IMS_FAILCAUSE_SERVICE_OPTION_NOT_AVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::BEARER_SERVICE_NOT_IMPLEMENTED:
      out = RIL_IMS_FAILCAUSE_BEARER_SERVICE_NOT_IMPLEMENTED;
      break;
    case qcril::interfaces::CallFailCause::ACM_LIMIT_EXCEEDED:
      out = RIL_IMS_FAILCAUSE_ACM_LIMIT_EXCEEDED;
      break;
    case qcril::interfaces::CallFailCause::REQUESTED_FACILITY_NOT_IMPLEMENTED:
      out = RIL_IMS_FAILCAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED;
      break;
    case qcril::interfaces::CallFailCause::ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE:
      out = RIL_IMS_FAILCAUSE_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::SERVICE_OR_OPTION_NOT_IMPLEMENTED:
      out = RIL_IMS_FAILCAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED;
      break;
    case qcril::interfaces::CallFailCause::INVALID_TRANSACTION_IDENTIFIER:
      out = RIL_IMS_FAILCAUSE_INVALID_TRANSACTION_IDENTIFIER;
      break;
    case qcril::interfaces::CallFailCause::USER_NOT_MEMBER_OF_CUG:
      out = RIL_IMS_FAILCAUSE_USER_NOT_MEMBER_OF_CUG;
      break;
    case qcril::interfaces::CallFailCause::INCOMPATIBLE_DESTINATION:
      out = RIL_IMS_FAILCAUSE_INCOMPATIBLE_DESTINATION;
      break;
    case qcril::interfaces::CallFailCause::INVALID_TRANSIT_NW_SELECTION:
      out = RIL_IMS_FAILCAUSE_INVALID_TRANSIT_NW_SELECTION;
      break;
    case qcril::interfaces::CallFailCause::SEMANTICALLY_INCORRECT_MESSAGE:
      out = RIL_IMS_FAILCAUSE_SEMANTICALLY_INCORRECT_MESSAGE;
      break;
    case qcril::interfaces::CallFailCause::INVALID_MANDATORY_INFORMATION:
      out = RIL_IMS_FAILCAUSE_INVALID_MANDATORY_INFORMATION;
      break;
    case qcril::interfaces::CallFailCause::MESSAGE_TYPE_NON_IMPLEMENTED:
      out = RIL_IMS_FAILCAUSE_MESSAGE_TYPE_NON_IMPLEMENTED;
      break;
    case qcril::interfaces::CallFailCause::MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE:
      out = RIL_IMS_FAILCAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE;
      break;
    case qcril::interfaces::CallFailCause::INFORMATION_ELEMENT_NON_EXISTENT:
      out = RIL_IMS_FAILCAUSE_INFORMATION_ELEMENT_NON_EXISTENT;
      break;
    case qcril::interfaces::CallFailCause::CONDITIONAL_IE_ERROR:
      out = RIL_IMS_FAILCAUSE_CONDITIONAL_IE_ERROR;
      break;
    case qcril::interfaces::CallFailCause::MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE:
      out = RIL_IMS_FAILCAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE;
      break;
    case qcril::interfaces::CallFailCause::RECOVERY_ON_TIMER_EXPIRED:
      out = RIL_IMS_FAILCAUSE_RECOVERY_ON_TIMER_EXPIRED;
      break;
    case qcril::interfaces::CallFailCause::PROTOCOL_ERROR_UNSPECIFIED:
      out = RIL_IMS_FAILCAUSE_PROTOCOL_ERROR_UNSPECIFIED;
      break;
    case qcril::interfaces::CallFailCause::INTERWORKING_UNSPECIFIED:
      out = RIL_IMS_FAILCAUSE_INTERWORKING_UNSPECIFIED;
      break;
    case qcril::interfaces::CallFailCause::CALL_BARRED:
      out = RIL_IMS_FAILCAUSE_CALL_BARRED;
      break;
    case qcril::interfaces::CallFailCause::FDN_BLOCKED:
      out = RIL_IMS_FAILCAUSE_FDN_BLOCKED;
      break;
    case qcril::interfaces::CallFailCause::IMSI_UNKNOWN_IN_VLR:
      out = RIL_IMS_FAILCAUSE_IMSI_UNKNOWN_IN_VLR;
      break;
    case qcril::interfaces::CallFailCause::IMEI_NOT_ACCEPTED:
      out = RIL_IMS_FAILCAUSE_IMEI_NOT_ACCEPTED;
      break;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_USSD:
      out = RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_USSD;
      break;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_SS:
      out = RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_SS;
      break;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL:
      out = RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_DIAL;
      break;
    case qcril::interfaces::CallFailCause::RADIO_OFF:
      out = RIL_IMS_FAILCAUSE_RADIO_OFF;
      break;
    case qcril::interfaces::CallFailCause::OUT_OF_SERVICE:
      out = RIL_IMS_FAILCAUSE_OUT_OF_SERVICE;
      break;
    case qcril::interfaces::CallFailCause::NO_VALID_SIM:
      out = RIL_IMS_FAILCAUSE_NO_VALID_SIM;
      break;
    case qcril::interfaces::CallFailCause::RADIO_INTERNAL_ERROR:
      out = RIL_IMS_FAILCAUSE_RADIO_INTERNAL_ERROR;
      break;
    case qcril::interfaces::CallFailCause::NETWORK_RESP_TIMEOUT:
      out = RIL_IMS_FAILCAUSE_NETWORK_RESP_TIMEOUT;
      break;
    case qcril::interfaces::CallFailCause::NETWORK_REJECT:
      out = RIL_IMS_FAILCAUSE_NETWORK_REJECT;
      break;
    case qcril::interfaces::CallFailCause::RADIO_ACCESS_FAILURE:
      out = RIL_IMS_FAILCAUSE_RADIO_ACCESS_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::RADIO_LINK_FAILURE:
      out = RIL_IMS_FAILCAUSE_RADIO_LINK_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::RADIO_LINK_LOST:
      out = RIL_IMS_FAILCAUSE_RADIO_LINK_LOST;
      break;
    case qcril::interfaces::CallFailCause::RADIO_UPLINK_FAILURE:
      out = RIL_IMS_FAILCAUSE_RADIO_UPLINK_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::RADIO_SETUP_FAILURE:
      out = RIL_IMS_FAILCAUSE_RADIO_SETUP_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::RADIO_RELEASE_NORMAL:
      out = RIL_IMS_FAILCAUSE_RADIO_RELEASE_NORMAL;
      break;
    case qcril::interfaces::CallFailCause::RADIO_RELEASE_ABNORMAL:
      out = RIL_IMS_FAILCAUSE_RADIO_RELEASE_ABNORMAL;
      break;
    case qcril::interfaces::CallFailCause::ACCESS_CLASS_BLOCKED:
      out = RIL_IMS_FAILCAUSE_ACCESS_CLASS_BLOCKED;
      break;
    case qcril::interfaces::CallFailCause::NETWORK_DETACH:
      out = RIL_IMS_FAILCAUSE_NETWORK_DETACH;
      break;
    case qcril::interfaces::CallFailCause::EMERGENCY_TEMP_FAILURE:
      out = RIL_IMS_FAILCAUSE_EMERGENCY_TEMP_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::EMERGENCY_PERM_FAILURE:
      out = RIL_IMS_FAILCAUSE_EMERGENCY_PERM_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::CDMA_LOCKED_UNTIL_POWER_CYCLE:
      out = RIL_IMS_FAILCAUSE_CDMA_LOCKED_UNTIL_POWER_CYCLE;
      break;
    case qcril::interfaces::CallFailCause::CDMA_DROP:
      out = RIL_IMS_FAILCAUSE_CDMA_DROP;
      break;
    case qcril::interfaces::CallFailCause::CDMA_INTERCEPT:
      out = RIL_IMS_FAILCAUSE_CDMA_INTERCEPT;
      break;
    case qcril::interfaces::CallFailCause::CDMA_REORDER:
      out = RIL_IMS_FAILCAUSE_CDMA_REORDER;
      break;
    case qcril::interfaces::CallFailCause::CDMA_SO_REJECT:
      out = RIL_IMS_FAILCAUSE_CDMA_SO_REJECT;
      break;
    case qcril::interfaces::CallFailCause::CDMA_RETRY_ORDER:
      out = RIL_IMS_FAILCAUSE_CDMA_RETRY_ORDER;
      break;
    case qcril::interfaces::CallFailCause::CDMA_ACCESS_FAILURE:
      out = RIL_IMS_FAILCAUSE_CDMA_ACCESS_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::CDMA_PREEMPTED:
      out = RIL_IMS_FAILCAUSE_CDMA_PREEMPTED;
      break;
    case qcril::interfaces::CallFailCause::CDMA_NOT_EMERGENCY:
      out = RIL_IMS_FAILCAUSE_CDMA_NOT_EMERGENCY;
      break;
    case qcril::interfaces::CallFailCause::CDMA_ACCESS_BLOCKED:
      out = RIL_IMS_FAILCAUSE_CDMA_ACCESS_BLOCKED;
      break;
    case qcril::interfaces::CallFailCause::INCOMPATIBILITY_DESTINATION:
      out = RIL_IMS_FAILCAUSE_INCOMPATIBILITY_DESTINATION;
      break;
    case qcril::interfaces::CallFailCause::HO_NOT_FEASIBLE:
      out = RIL_IMS_FAILCAUSE_HO_NOT_FEASIBLE;
      break;
    case qcril::interfaces::CallFailCause::USER_BUSY:
      out = RIL_IMS_FAILCAUSE_USER_BUSY;
      break;
    case qcril::interfaces::CallFailCause::USER_REJECT:
      out = RIL_IMS_FAILCAUSE_USER_REJECT;
      break;
    case qcril::interfaces::CallFailCause::LOW_BATTERY:
      out = RIL_IMS_FAILCAUSE_LOW_BATTERY;
      break;
    case qcril::interfaces::CallFailCause::BLACKLISTED_CALL_ID:
      out = RIL_IMS_FAILCAUSE_BLACKLISTED_CALL_ID;
      break;
    case qcril::interfaces::CallFailCause::CS_RETRY_REQUIRED:
      out = RIL_IMS_FAILCAUSE_CS_RETRY_REQUIRED;
      break;
    case qcril::interfaces::CallFailCause::NETWORK_UNAVAILABLE:
      out = RIL_IMS_FAILCAUSE_NETWORK_UNAVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::FEATURE_UNAVAILABLE:
      out = RIL_IMS_FAILCAUSE_FEATURE_UNAVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::SIP_ERROR:
      out = RIL_IMS_FAILCAUSE_SIP_ERROR;
      break;
    case qcril::interfaces::CallFailCause::MISC:
      out = RIL_IMS_FAILCAUSE_MISC;
      break;
    case qcril::interfaces::CallFailCause::ANSWERED_ELSEWHERE:
      out = RIL_IMS_FAILCAUSE_ANSWERED_ELSEWHERE;
      break;
    case qcril::interfaces::CallFailCause::PULL_OUT_OF_SYNC:
      out = RIL_IMS_FAILCAUSE_PULL_OUT_OF_SYNC;
      break;
    case qcril::interfaces::CallFailCause::CAUSE_CALL_PULLED:
      out = RIL_IMS_FAILCAUSE_CAUSE_CALL_PULLED;
      break;
    case qcril::interfaces::CallFailCause::SIP_REDIRECTED:
      out = RIL_IMS_FAILCAUSE_SIP_REDIRECTED;
      break;
    case qcril::interfaces::CallFailCause::SIP_BAD_REQUEST:
      out = RIL_IMS_FAILCAUSE_SIP_BAD_REQUEST;
      break;
    case qcril::interfaces::CallFailCause::SIP_FORBIDDEN:
      out = RIL_IMS_FAILCAUSE_SIP_FORBIDDEN;
      break;
    case qcril::interfaces::CallFailCause::SIP_NOT_FOUND:
      out = RIL_IMS_FAILCAUSE_SIP_NOT_FOUND;
      break;
    case qcril::interfaces::CallFailCause::SIP_NOT_SUPPORTED:
      out = RIL_IMS_FAILCAUSE_SIP_NOT_SUPPORTED;
      break;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_TIMEOUT:
      out = RIL_IMS_FAILCAUSE_SIP_REQUEST_TIMEOUT;
      break;
    case qcril::interfaces::CallFailCause::SIP_TEMPORARILY_UNAVAILABLE:
      out = RIL_IMS_FAILCAUSE_SIP_TEMPORARILY_UNAVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::SIP_BAD_ADDRESS:
      out = RIL_IMS_FAILCAUSE_SIP_BAD_ADDRESS;
      break;
    case qcril::interfaces::CallFailCause::SIP_BUSY:
      out = RIL_IMS_FAILCAUSE_SIP_BUSY;
      break;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_CANCELLED:
      out = RIL_IMS_FAILCAUSE_SIP_REQUEST_CANCELLED;
      break;
    case qcril::interfaces::CallFailCause::SIP_NOT_ACCEPTABLE:
      out = RIL_IMS_FAILCAUSE_SIP_NOT_ACCEPTABLE;
      break;
    case qcril::interfaces::CallFailCause::SIP_NOT_REACHABLE:
      out = RIL_IMS_FAILCAUSE_SIP_NOT_REACHABLE;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_INTERNAL_ERROR:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_INTERNAL_ERROR;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_NOT_IMPLEMENTED:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_NOT_IMPLEMENTED;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_BAD_GATEWAY:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_BAD_GATEWAY;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVICE_UNAVAILABLE:
      out = RIL_IMS_FAILCAUSE_SIP_SERVICE_UNAVAILABLE;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_TIMEOUT:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_TIMEOUT;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_VERSION_UNSUPPORTED:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_VERSION_UNSUPPORTED;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_MESSAGE_TOOLARGE:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_MESSAGE_TOOLARGE;
      break;
    case qcril::interfaces::CallFailCause::SIP_SERVER_PRECONDITION_FAILURE:
      out = RIL_IMS_FAILCAUSE_SIP_SERVER_PRECONDITION_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::SIP_USER_REJECTED:
      out = RIL_IMS_FAILCAUSE_SIP_USER_REJECTED;
      break;
    case qcril::interfaces::CallFailCause::SIP_GLOBAL_ERROR:
      out = RIL_IMS_FAILCAUSE_SIP_GLOBAL_ERROR;
      break;
    case qcril::interfaces::CallFailCause::MEDIA_INIT_FAILED:
      out = RIL_IMS_FAILCAUSE_MEDIA_INIT_FAILED;
      break;
    case qcril::interfaces::CallFailCause::MEDIA_NO_DATA:
      out = RIL_IMS_FAILCAUSE_MEDIA_NO_DATA;
      break;
    case qcril::interfaces::CallFailCause::MEDIA_NOT_ACCEPTABLE:
      out = RIL_IMS_FAILCAUSE_MEDIA_NOT_ACCEPTABLE;
      break;
    case qcril::interfaces::CallFailCause::MEDIA_UNSPECIFIED_ERROR:
      out = RIL_IMS_FAILCAUSE_MEDIA_UNSPECIFIED_ERROR;
      break;
    case qcril::interfaces::CallFailCause::HOLD_RESUME_FAILED:
      out = RIL_IMS_FAILCAUSE_HOLD_RESUME_FAILED;
      break;
    case qcril::interfaces::CallFailCause::HOLD_RESUME_CANCELED:
      out = RIL_IMS_FAILCAUSE_HOLD_RESUME_CANCELED;
      break;
    case qcril::interfaces::CallFailCause::HOLD_REINVITE_COLLISION:
      out = RIL_IMS_FAILCAUSE_HOLD_REINVITE_COLLISION;
      break;
    case qcril::interfaces::CallFailCause::DIAL_MODIFIED_TO_DIAL_VIDEO:
      out = RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_DIAL_VIDEO;
      break;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL:
      out = RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_DIAL;
      break;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
      out = RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
      break;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_SS:
      out = RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_SS;
      break;
    case qcril::interfaces::CallFailCause::DIAL_VIDEO_MODIFIED_TO_USSD:
      out = RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_USSD;
      break;
    case qcril::interfaces::CallFailCause::SIP_ALTERNATE_EMERGENCY_CALL:
      out = RIL_IMS_FAILCAUSE_SIP_ALTERNATE_EMERGENCY_CALL;
      break;
    case qcril::interfaces::CallFailCause::NO_CSFB_IN_CS_ROAM:
      out = RIL_IMS_FAILCAUSE_NO_CSFB_IN_CS_ROAM;
      break;
    case qcril::interfaces::CallFailCause::SRV_NOT_REGISTERED:
      out = RIL_IMS_FAILCAUSE_SRV_NOT_REGISTERED;
      break;
    case qcril::interfaces::CallFailCause::CALL_TYPE_NOT_ALLOWED:
      out = RIL_IMS_FAILCAUSE_CALL_TYPE_NOT_ALLOWED;
      break;
    case qcril::interfaces::CallFailCause::EMRG_CALL_ONGOING:
      out = RIL_IMS_FAILCAUSE_EMRG_CALL_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::CALL_SETUP_ONGOING:
      out = RIL_IMS_FAILCAUSE_CALL_SETUP_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::MAX_CALL_LIMIT_REACHED:
      out = RIL_IMS_FAILCAUSE_MAX_CALL_LIMIT_REACHED;
      break;
    case qcril::interfaces::CallFailCause::UNSUPPORTED_SIP_HDRS:
      out = RIL_IMS_FAILCAUSE_UNSUPPORTED_SIP_HDRS;
      break;
    case qcril::interfaces::CallFailCause::CALL_TRANSFER_ONGOING:
      out = RIL_IMS_FAILCAUSE_CALL_TRANSFER_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::PRACK_TIMEOUT:
      out = RIL_IMS_FAILCAUSE_PRACK_TIMEOUT;
      break;
    case qcril::interfaces::CallFailCause::QOS_FAILURE:
      out = RIL_IMS_FAILCAUSE_QOS_FAILURE;
      break;
    case qcril::interfaces::CallFailCause::ONGOING_HANDOVER:
      out = RIL_IMS_FAILCAUSE_ONGOING_HANDOVER;
      break;
    case qcril::interfaces::CallFailCause::VT_WITH_TTY_NOT_ALLOWED:
      out = RIL_IMS_FAILCAUSE_VT_WITH_TTY_NOT_ALLOWED;
      break;
    case qcril::interfaces::CallFailCause::CALL_UPGRADE_ONGOING:
      out = RIL_IMS_FAILCAUSE_CALL_UPGRADE_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::CONFERENCE_WITH_TTY_NOT_ALLOWED:
      out = RIL_IMS_FAILCAUSE_CONFERENCE_WITH_TTY_NOT_ALLOWED;
      break;
    case qcril::interfaces::CallFailCause::CALL_CONFERENCE_ONGOING:
      out = RIL_IMS_FAILCAUSE_CALL_CONFERENCE_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::VT_WITH_AVPF_NOT_ALLOWED:
      out = RIL_IMS_FAILCAUSE_VT_WITH_AVPF_NOT_ALLOWED;
      break;
    case qcril::interfaces::CallFailCause::ENCRYPTION_CALL_ONGOING:
      out = RIL_IMS_FAILCAUSE_ENCRYPTION_CALL_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::CALL_ONGOING_CW_DISABLED:
      out = RIL_IMS_FAILCAUSE_CALL_ONGOING_CW_DISABLED;
      break;
    case qcril::interfaces::CallFailCause::CALL_ON_OTHER_SUB:
      out = RIL_IMS_FAILCAUSE_CALL_ON_OTHER_SUB;
      break;
    case qcril::interfaces::CallFailCause::ONE_X_COLLISION:
      out = RIL_IMS_FAILCAUSE_ONE_X_COLLISION;
      break;
    case qcril::interfaces::CallFailCause::UI_NOT_READY:
      out = RIL_IMS_FAILCAUSE_UI_NOT_READY;
      break;
    case qcril::interfaces::CallFailCause::CS_CALL_ONGOING:
      out = RIL_IMS_FAILCAUSE_CS_CALL_ONGOING;
      break;
    case qcril::interfaces::CallFailCause::SIP_USER_MARKED_UNWANTED:
      out = RIL_IMS_FAILCAUSE_SIP_USER_MARKED_UNWANTED;
      break;
    case qcril::interfaces::CallFailCause::REJECTED_ELSEWHERE:
      out = RIL_IMS_FAILCAUSE_REJECTED_ELSEWHERE;
      break;
    case qcril::interfaces::CallFailCause::USER_REJECTED_SESSION_MODIFICATION:
      out = RIL_IMS_FAILCAUSE_USER_REJECTED_SESSION_MODIFICATION;
      break;
    case qcril::interfaces::CallFailCause::USER_CANCELLED_SESSION_MODIFICATION:
      out = RIL_IMS_FAILCAUSE_USER_CANCELLED_SESSION_MODIFICATION;
      break;
    case qcril::interfaces::CallFailCause::SESSION_MODIFICATION_FAILED:
      out = RIL_IMS_FAILCAUSE_SESSION_MODIFICATION_FAILED;
      break;
    case qcril::interfaces::CallFailCause::SIP_METHOD_NOT_ALLOWED:
      out = RIL_IMS_FAILCAUSE_SIP_METHOD_NOT_ALLOWED;
      break;
    case qcril::interfaces::CallFailCause::SIP_PROXY_AUTHENTICATION_REQUIRED:
      out = RIL_IMS_FAILCAUSE_SIP_PROXY_AUTHENTICATION_REQUIRED;
      break;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_ENTITY_TOO_LARGE:
      out = RIL_IMS_FAILCAUSE_SIP_REQUEST_ENTITY_TOO_LARGE;
      break;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_URI_TOO_LARGE:
      out = RIL_IMS_FAILCAUSE_SIP_REQUEST_URI_TOO_LARGE;
      break;
    case qcril::interfaces::CallFailCause::SIP_EXTENSION_REQUIRED:
      out = RIL_IMS_FAILCAUSE_SIP_EXTENSION_REQUIRED;
      break;
    case qcril::interfaces::CallFailCause::SIP_INTERVAL_TOO_BRIEF:
      out = RIL_IMS_FAILCAUSE_SIP_INTERVAL_TOO_BRIEF;
      break;
    case qcril::interfaces::CallFailCause::SIP_CALL_OR_TRANS_DOES_NOT_EXIST:
      out = RIL_IMS_FAILCAUSE_SIP_CALL_OR_TRANS_DOES_NOT_EXIST;
      break;
    case qcril::interfaces::CallFailCause::SIP_LOOP_DETECTED:
      out = RIL_IMS_FAILCAUSE_SIP_LOOP_DETECTED;
      break;
    case qcril::interfaces::CallFailCause::SIP_TOO_MANY_HOPS:
      out = RIL_IMS_FAILCAUSE_SIP_TOO_MANY_HOPS;
      break;
    case qcril::interfaces::CallFailCause::SIP_AMBIGUOUS:
      out = RIL_IMS_FAILCAUSE_SIP_AMBIGUOUS;
      break;
    case qcril::interfaces::CallFailCause::SIP_REQUEST_PENDING:
      out = RIL_IMS_FAILCAUSE_SIP_REQUEST_PENDING;
      break;
    case qcril::interfaces::CallFailCause::SIP_UNDECIPHERABLE:
      out = RIL_IMS_FAILCAUSE_SIP_UNDECIPHERABLE;
      break;
    case qcril::interfaces::CallFailCause::RETRY_ON_IMS_WITHOUT_RTT:
      out = RIL_IMS_FAILCAUSE_RETRY_ON_IMS_WITHOUT_RTT;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_1:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_1;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_2:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_2;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_3:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_3;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_4:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_4;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_5:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_5;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_6:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_6;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_7:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_7;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_8:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_8;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_9:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_9;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_10:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_10;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_11:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_11;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_12:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_12;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_13:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_13;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_14:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_14;
      break;
    case qcril::interfaces::CallFailCause::OEM_CAUSE_15:
      out = RIL_IMS_FAILCAUSE_OEM_CAUSE_15;
      break;
    case qcril::interfaces::CallFailCause::ERROR_UNSPECIFIED:
      out = RIL_IMS_FAILCAUSE_ERROR_UNSPECIFIED;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_CallFailCauseResponse& out,
                     const qcril::interfaces::CallFailCauseResponse& in) {
  convertToSocket(out.failCause, in.getFailCauseRef());
  convertToSocket(out.extendedFailCause, in.getExtendedFailCauseRef());
  convertToSocket(out.networkErrorString, in.getNetworkErrorStringRef());
  convertToSocket(out.errorDetails, in.getErrorDetailsRef());
}

template <>
void convertToSocket(RIL_IMS_VerificationStatus& out,
                     qcril::interfaces::VerificationStatus const& in) {
  switch (in) {
    case qcril::interfaces::VerificationStatus::UNKNOWN:
      out = RIL_IMS_VERSTAT_UNKNOWN;
      break;
    case qcril::interfaces::VerificationStatus::NONE:
      out = RIL_IMS_VERSTAT_NONE;
      break;
    case qcril::interfaces::VerificationStatus::PASS:
      out = RIL_IMS_VERSTAT_PASS;
      break;
    case qcril::interfaces::VerificationStatus::FAIL:
      out = RIL_IMS_VERSTAT_FAIL;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_VerstatInfo& out, const qcril::interfaces::VerstatInfo& in) {
  convertToSocket(out.canMarkUnwantedCall, in.getCanMarkUnwantedCallRef());
  convertToSocket(out.verificationStatus, in.getVerificationStatusRef());
}

template <>
void convertToSocket(RIL_UUS_Info& out, const qcril::interfaces::UusInfo& in) {
  convertToSocket(out.uusType, in.getTypeRef());
  convertToSocket(out.uusDcs, in.getDcsRef());
  convertToSocket(out.uusData, in.getData());
}

template <>
void convertToSocket(RIL_IMS_CallProgressInfoType& out,
                     const qcril::interfaces::CallProgressInfoType& in) {
  switch (in) {
    case qcril::interfaces::CallProgressInfoType::UNKNOWN:
      out = RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN;
      break;
    case qcril::interfaces::CallProgressInfoType::CALL_REJ_Q850:
      out = RIL_IMS_CALL_PROGRESS_INFO_CALL_REJ_Q850;
      break;
    case qcril::interfaces::CallProgressInfoType::CALL_WAITING:
      out = RIL_IMS_CALL_PROGRESS_INFO_CALL_WAITING;
      break;
    case qcril::interfaces::CallProgressInfoType::CALL_FORWARDING:
      out = RIL_IMS_CALL_PROGRESS_INFO_CALL_FORWARDING;
      break;
    case qcril::interfaces::CallProgressInfoType::REMOTE_AVAILABLE:
      out = RIL_IMS_CALL_PROGRESS_INFO_REMOTE_AVAILABLE;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_CallProgressInfo& out, const qcril::interfaces::CallProgressInfo& in) {
  out.type = RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN;
  if (in.hasType()) {
    convertToSocket(out.type, in.getType());
  }
  out.reasonCode = INT16_MAX;
  if (in.hasReasonCode()) {
    convertToSocket(out.reasonCode, in.getReasonCode());
  }
  out.reasonText = nullptr;
  if (in.hasReasonText()) {
    convertToSocket(out.reasonText, in.getReasonText());
  }
}


template <>
void convertToSocket(RIL_IMS_TirMode& out, const qcril::interfaces::TirMode& in) {
  switch (in) {
    case qcril::interfaces::TirMode::UNKNOWN:
      out = RIL_IMS_TIRMODE_UNKNOWN;
      break;
    case qcril::interfaces::TirMode::TEMPORARY:
      out = RIL_IMS_TIRMODE_TEMPORARY;
      break;
    case qcril::interfaces::TirMode::PERMANENT:
      out = RIL_IMS_TIRMODE_PERMANENT;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_MessageType& out, const qcril::interfaces::MessageType& in) {
  switch (in) {
    case qcril::interfaces::MessageType::UNKNOWN:
      out = RIL_IMS_MESSAGE_TYPE_UNKNOWN;
      break;
    case qcril::interfaces::MessageType::NONE:
      out = RIL_IMS_MESSAGE_TYPE_NONE;
      break;
    case qcril::interfaces::MessageType::VOICE:
      out = RIL_IMS_MESSAGE_TYPE_VOICE;
      break;
    case qcril::interfaces::MessageType::VIDEO:
      out = RIL_IMS_MESSAGE_TYPE_VIDEO;
      break;
    case qcril::interfaces::MessageType::FAX:
      out = RIL_IMS_MESSAGE_TYPE_FAX;
      break;
    case qcril::interfaces::MessageType::PAGER:
      out = RIL_IMS_MESSAGE_TYPE_PAGER;
      break;
    case qcril::interfaces::MessageType::MULTIMEDIA:
      out = RIL_IMS_MESSAGE_TYPE_MULTIMEDIA;
      break;
    case qcril::interfaces::MessageType::TEXT:
      out = RIL_IMS_MESSAGE_TYPE_TEXT;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_MessagePriority& out, const qcril::interfaces::MessagePriority& in) {
  switch (in) {
    case qcril::interfaces::MessagePriority::UNKNOWN:
      out = RIL_IMS_MESSAGE_PRIORITY_UNKNOWN;
      break;
    case qcril::interfaces::MessagePriority::LOW:
      out = RIL_IMS_MESSAGE_PRIORITY_LOW;
      break;
    case qcril::interfaces::MessagePriority::NORMAL:
      out = RIL_IMS_MESSAGE_PRIORITY_NORMAL;
      break;
    case qcril::interfaces::MessagePriority::URGENT:
      out = RIL_IMS_MESSAGE_PRIORITY_URGENT;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_CallModifyFailCause& out,
                     const qcril::interfaces::CallModifyFailCause& in) {
  switch (in) {
    case qcril::interfaces::CallModifyFailCause::UNKNOWN:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_UNKNOWN;
      break;
    case qcril::interfaces::CallModifyFailCause::SUCCESS:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_SUCCESS;
      break;
    case qcril::interfaces::CallModifyFailCause::RADIO_NOT_AVAILABLE:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE;
      break;
    case qcril::interfaces::CallModifyFailCause::GENERIC_FAILURE:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE;
      break;
    case qcril::interfaces::CallModifyFailCause::REQUEST_NOT_SUPPORTED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED;
      break;
    case qcril::interfaces::CallModifyFailCause::CANCELLED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_CANCELLED;
      break;
    case qcril::interfaces::CallModifyFailCause::UNUSED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_UNUSED;
      break;
    case qcril::interfaces::CallModifyFailCause::INVALID_PARAMETER:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER;
      break;
    case qcril::interfaces::CallModifyFailCause::REJECTED_BY_REMOTE:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE;
      break;
    case qcril::interfaces::CallModifyFailCause::IMS_DEREGISTERED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED;
      break;
    case qcril::interfaces::CallModifyFailCause::NETWORK_NOT_SUPPORTED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED;
      break;
    case qcril::interfaces::CallModifyFailCause::HOLD_RESUME_FAILED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED;
      break;
    case qcril::interfaces::CallModifyFailCause::HOLD_RESUME_CANCELED:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED;
      break;
    case qcril::interfaces::CallModifyFailCause::REINVITE_COLLISION:
      out = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_CallInfo& out, const qcril::interfaces::CallInfo& in) {
  convertToSocket(out.callState, in.getCallStateRef());
  convertToSocket(out.index, in.getIndexRef());
  convertToSocket(out.toa, in.getToaRef());
  convertToSocket(out.isMpty, in.getIsMptyRef());
  convertToSocket(out.isMt, in.getIsMtRef());
  convertToSocket(out.als, in.getAlsRef());
  convertToSocket(out.isVoice, in.getIsVoiceRef());
  convertToSocket(out.isVoicePrivacy, in.getIsVoicePrivacyRef());
  convertToSocket(out.number, in.getNumberRef());
  convertToSocket(out.numberPresentation, in.getNumberPresentationRef());
  convertToSocket(out.name, in.getNameRef());
  convertToSocket(out.namePresentation, in.getNamePresentationRef());
  convertToSocket(out.redirNum, in.getRedirNumRef());
  convertToSocket(out.redirNumPresentation, in.getRedirNumPresentationRef());
  convertToSocket(out.callType, in.getCallTypeRef());
  convertToSocket(out.callDomain, in.getCallDomainRef());
  convertToSocket(out.callSubState, in.getCallSubStateRef());
  convertToSocket(out.isEncrypted, in.getIsEncryptedRef());
  convertToSocket(out.isCalledPartyRinging, in.getIsCalledPartyRingingRef());
  convertToSocket(out.isVideoConfSupported, in.getIsVideoConfSupportedRef());
  convertToSocket(out.historyInfo, in.getHistoryInfoRef());
  convertToSocket(out.mediaId, in.getMediaIdRef());
  convertToSocket(out.causeCode, in.getCauseCodeRef());
  convertToSocket(out.rttMode, in.getRttModeRef());
  convertToSocket(out.sipAlternateUri, in.getSipAlternateUriRef());
  convertToSocket(out.localAbility, out.localAbilityLen, in.getLocalAbilityRef());
  convertToSocket(out.peerAbility, out.peerAbilityLen, in.getPeerAbilityRef());
  if (in.hasCallFailCauseResponse()) {
    convertToSocket(out.callFailCauseResponse, in.getCallFailCauseResponseRef());
  }
  convertToSocket(out.terminatingNumber, in.getTerminatingNumberRef());
  convertToSocket(out.isSecondary, in.getIsSecondaryRef());
  if (in.hasVerstatInfo()) {
    convertToSocket(out.verstatInfo, in.getVerstatInfoRef());
  }
  if (in.hasUusInfo()) {
    convertToSocket(out.uusInfo, in.getUusInfoRef());
  }
  if (in.hasAudioQuality()) {
    out.audioQuality = new RIL_AudioQuality{};
    if (out.audioQuality) {
      convertToSocket(*out.audioQuality, in.getAudioQualityRef());
    }
  }
  convertToSocket(out.Codec, in.getCodecRef());
  convertToSocket(out.displayText, in.getDisplayTextRef());
  convertToSocket(out.additionalCallInfo, in.getAdditionalCallInfoRef());
  convertToSocket(out.childNumber, in.getChildNumberRef());
  convertToSocket(out.emergencyServiceCategory, in.getEmergencyServiceCategoryRef());
  convertToSocket(out.tirMode, in.getTirModeRef());
  if (in.hasProgressInfo()) {
    convertToSocket(out.callProgInfo, in.getProgressInfoRef());
  }
}

template <>
void convertToSocket(RIL_IMS_CallInfo*& out, size_t& outlen,
                     const std::vector<qcril::interfaces::CallInfo>& callInfoList) {
  size_t sz = callInfoList.size();
  RIL_IMS_CallInfo* tmp = nullptr;
  if (sz > 0) {
    tmp = new RIL_IMS_CallInfo[sz];
    std::transform(callInfoList.begin(), callInfoList.end(), tmp,
                   [](const qcril::interfaces::CallInfo& in) -> RIL_IMS_CallInfo {
                     RIL_IMS_CallInfo ret{};
                     convertToSocket(ret, in);
                     return ret;
                   });
  }
  out = tmp;
  outlen = sz;
}

template <>
void convertToSocket(RIL_IMS_MessageSummary& out, const qcril::interfaces::MessageSummary& in) {
  if (in.hasMessageType()) {
    convertToSocket(out.messageType, in.getMessageType());
  }
  if (in.hasNewMessageCount()) {
    out.newMessageCount = in.getNewMessageCount();
  }
  if (in.hasOldMessageCount()) {
    out.oldMessageCount = in.getOldMessageCount();
  }
  if (in.hasNewUrgentMessageCount()) {
    out.newUrgentMessageCount = in.getNewUrgentMessageCount();
  }
  if (in.hasOldUrgentMessageCount()) {
    out.oldUrgentMessageCount = in.getOldUrgentMessageCount();
  }
}

template <>
void convertToSocket(RIL_IMS_MessageDetails& out, const qcril::interfaces::MessageDetails& in) {
  if (in.hasToAddress()) {
    convertToSocket(out.toAddress, in.getToAddress());
  }

  if (in.hasFromAddress() && !in.getFromAddress().empty()) {
    convertToSocket(out.fromAddress, in.getFromAddress());
  }
  if (in.hasSubject() && !in.getSubject().empty()) {
    convertToSocket(out.subject, in.getSubject());
  }
  if (in.hasDate() && !in.getDate().empty()) {
    convertToSocket(out.date, in.getDate());
  }
  if (in.hasPriority()) {
    convertToSocket(out.priority, in.getPriority());
  }
  if (in.hasMessageId() && !in.getMessageId().empty()) {
    convertToSocket(out.messageId, in.getMessageId());
  }
  if (in.hasMessageType()) {
    convertToSocket(out.messageType, in.getMessageType());
  }
}

template <>
void convertToSocket(RIL_IMS_CallFwdTimerInfo& out, const qcril::interfaces::CallFwdTimerInfo& in) {
  if (in.hasYear()) {
    out.year = in.getYear();
  }
  if (in.hasMonth()) {
    out.month = in.getMonth();
  }
  if (in.hasDay()) {
    out.day = in.getDay();
  }
  if (in.hasHour()) {
    out.hour = in.getHour();
  }
  if (in.hasMinute()) {
    out.minute = in.getMinute();
  }
  if (in.hasSecond()) {
    out.second = in.getSecond();
  }
  if (in.hasTimezone()) {
    out.timezone = in.getTimezone();
  }
}

template <>
void convertToSocket(RIL_IMS_CallForwardInfo& out, const qcril::interfaces::CallForwardInfo& in) {
  if (in.hasStatus()) {
    out.status = in.getStatus();
  }
  if (in.hasReason()) {
    out.reason = in.getReason();
  }
  if (in.hasServiceClass()) {
    out.serviceClass = in.getServiceClass();
  }
  if (in.hasToa()) {
    out.toa = in.getToa();
  }
  if (in.hasNumber()) {
    convertToSocket(out.number, in.getNumber());
  }
  if (in.hasTimeSeconds()) {
    out.timeSeconds = in.getTimeSeconds();
  }
  auto inCftStart = in.getCallFwdTimerStart();
  if (out.hasCallFwdTimerStart && inCftStart != nullptr) {
    out.hasCallFwdTimerStart = TRUE;
    convertToSocket(out.callFwdTimerStart, *inCftStart);
  }
  auto inCftStop = in.getCallFwdTimerEnd();
  if (out.hasCallFwdTimerEnd && inCftStop != nullptr) {
    out.hasCallFwdTimerEnd = TRUE;
    convertToSocket(out.callFwdTimerEnd, *inCftStop);
  }
}

template <>
void convertToSocket(RIL_IMS_QueryCallForwardStatusInfo& out,
                     const qcril::interfaces::GetCallForwardRespData& in) {
  const auto inCfInfoList = in.getCallForwardInfoRef();
  if (inCfInfoList.size()) {
    convertToSocket(out.callForwardInfo, out.callForwardInfoLen, inCfInfoList);
  }
  if (in.hasErrorDetails()) {
    convertToSocket(out.errorDetails, in.getErrorDetails());
  }
}

template <>
void convertToSocket(RIL_IMS_SetCallForwardStatus& out,
                     const qcril::interfaces::SetCallForwardStatus& in) {
  if (in.hasReason()) {
    out.reason = in.getReason();
  }
  if (in.hasStatus()) {
    out.status = in.getStatus();
  }
  if (in.hasErrorDetails()) {
    convertToSocket(out.errorDetails, in.getErrorDetails());
  }
}

template <>
void convertToSocket(RIL_IMS_SetCallForwardStatusInfo& out,
                     const qcril::interfaces::SetCallForwardRespData& in) {
  const auto inCfStatusList = in.getSetCallForwardStatusRef();
  if (inCfStatusList.size()) {
    convertToSocket(out.setCallForwardStatus, out.setCallForwardStatusLen, inCfStatusList);
  }
  if (in.hasErrorDetails()) {
    convertToSocket(out.errorDetails, in.getErrorDetails());
  }
}

void convertToSocket(RIL_IMS_ConfigItem& out, const qcril::interfaces::ConfigItem& in) {
  switch (in) {
    case qcril::interfaces::ConfigItem::NONE:
      out = RIL_IMS_CONFIG_ITEM_NONE;
      break;
    case qcril::interfaces::ConfigItem::VOCODER_AMRMODESET:
      out = RIL_IMS_CONFIG_ITEM_VOCODER_AMRMODESET;
      break;
    case qcril::interfaces::ConfigItem::VOCODER_AMRWBMODESET:
      out = RIL_IMS_CONFIG_ITEM_VOCODER_AMRWBMODESET;
      break;
    case qcril::interfaces::ConfigItem::SIP_SESSION_TIMER:
      out = RIL_IMS_CONFIG_ITEM_SIP_SESSION_TIMER;
      break;
    case qcril::interfaces::ConfigItem::MIN_SESSION_EXPIRY:
      out = RIL_IMS_CONFIG_ITEM_MIN_SESSION_EXPIRY;
      break;
    case qcril::interfaces::ConfigItem::CANCELLATION_TIMER:
      out = RIL_IMS_CONFIG_ITEM_CANCELLATION_TIMER;
      break;
    case qcril::interfaces::ConfigItem::T_DELAY:
      out = RIL_IMS_CONFIG_ITEM_T_DELAY;
      break;
    case qcril::interfaces::ConfigItem::SILENT_REDIAL_ENABLE:
      out = RIL_IMS_CONFIG_ITEM_SILENT_REDIAL_ENABLE;
      break;
    case qcril::interfaces::ConfigItem::SIP_T1_TIMER:
      out = RIL_IMS_CONFIG_ITEM_SIP_T1_TIMER;
      break;
    case qcril::interfaces::ConfigItem::SIP_T2_TIMER:
      out = RIL_IMS_CONFIG_ITEM_SIP_T2_TIMER;
      break;
    case qcril::interfaces::ConfigItem::SIP_TF_TIMER:
      out = RIL_IMS_CONFIG_ITEM_SIP_TF_TIMER;
      break;
    case qcril::interfaces::ConfigItem::VLT_SETTING_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_VLT_SETTING_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::LVC_SETTING_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_LVC_SETTING_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::DOMAIN_NAME:
      out = RIL_IMS_CONFIG_ITEM_DOMAIN_NAME;
      break;
    case qcril::interfaces::ConfigItem::SMS_FORMAT:
      out = RIL_IMS_CONFIG_ITEM_SMS_FORMAT;
      break;
    case qcril::interfaces::ConfigItem::SMS_OVER_IP:
      out = RIL_IMS_CONFIG_ITEM_SMS_OVER_IP;
      break;
    case qcril::interfaces::ConfigItem::PUBLISH_TIMER:
      out = RIL_IMS_CONFIG_ITEM_PUBLISH_TIMER;
      break;
    case qcril::interfaces::ConfigItem::PUBLISH_TIMER_EXTENDED:
      out = RIL_IMS_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
      break;
    case qcril::interfaces::ConfigItem::CAPABILITIES_CACHE_EXPIRATION:
      out = RIL_IMS_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
      break;
    case qcril::interfaces::ConfigItem::AVAILABILITY_CACHE_EXPIRATION:
      out = RIL_IMS_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
      break;
    case qcril::interfaces::ConfigItem::CAPABILITIES_POLL_INTERVAL:
      out = RIL_IMS_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
      break;
    case qcril::interfaces::ConfigItem::SOURCE_THROTTLE_PUBLISH:
      out = RIL_IMS_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
      break;
    case qcril::interfaces::ConfigItem::MAX_NUM_ENTRIES_IN_RCL:
      out = RIL_IMS_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
      break;
    case qcril::interfaces::ConfigItem::CAPAB_POLL_LIST_SUB_EXP:
      out = RIL_IMS_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
      break;
    case qcril::interfaces::ConfigItem::GZIP_FLAG:
      out = RIL_IMS_CONFIG_ITEM_GZIP_FLAG;
      break;
    case qcril::interfaces::ConfigItem::EAB_SETTING_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_EAB_SETTING_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::MOBILE_DATA_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_MOBILE_DATA_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING:
      out = RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
      break;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_MODE:
      out = RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
      break;
    case qcril::interfaces::ConfigItem::CAPABILITY_DISCOVERY_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::EMERGENCY_CALL_TIMER:
      out = RIL_IMS_CONFIG_ITEM_EMERGENCY_CALL_TIMER;
      break;
    case qcril::interfaces::ConfigItem::SSAC_HYSTERESIS_TIMER:
      out = RIL_IMS_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER;
      break;
    case qcril::interfaces::ConfigItem::VOLTE_USER_OPT_IN_STATUS:
      out = RIL_IMS_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
      break;
    case qcril::interfaces::ConfigItem::LBO_PCSCF_ADDRESS:
      out = RIL_IMS_CONFIG_ITEM_LBO_PCSCF_ADDRESS;
      break;
    case qcril::interfaces::ConfigItem::KEEP_ALIVE_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_KEEP_ALIVE_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC:
      out = RIL_IMS_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
      break;
    case qcril::interfaces::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC:
      out = RIL_IMS_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
      break;
    case qcril::interfaces::ConfigItem::SPEECH_START_PORT:
      out = RIL_IMS_CONFIG_ITEM_SPEECH_START_PORT;
      break;
    case qcril::interfaces::ConfigItem::SPEECH_END_PORT:
      out = RIL_IMS_CONFIG_ITEM_SPEECH_END_PORT;
      break;
    case qcril::interfaces::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
      break;
    case qcril::interfaces::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      out = RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
      break;
    case qcril::interfaces::ConfigItem::AMR_WB_OCTET_ALIGNED_PT:
      out = RIL_IMS_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
      break;
    case qcril::interfaces::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT:
      out = RIL_IMS_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
      break;
    case qcril::interfaces::ConfigItem::AMR_OCTET_ALIGNED_PT:
      out = RIL_IMS_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
      break;
    case qcril::interfaces::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT:
      out = RIL_IMS_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
      break;
    case qcril::interfaces::ConfigItem::DTMF_WB_PT:
      out = RIL_IMS_CONFIG_ITEM_DTMF_WB_PT;
      break;
    case qcril::interfaces::ConfigItem::DTMF_NB_PT:
      out = RIL_IMS_CONFIG_ITEM_DTMF_NB_PT;
      break;
    case qcril::interfaces::ConfigItem::AMR_DEFAULT_MODE:
      out = RIL_IMS_CONFIG_ITEM_AMR_DEFAULT_MODE;
      break;
    case qcril::interfaces::ConfigItem::SMS_PSI:
      out = RIL_IMS_CONFIG_ITEM_SMS_PSI;
      break;
    case qcril::interfaces::ConfigItem::VIDEO_QUALITY:
      out = RIL_IMS_CONFIG_ITEM_VIDEO_QUALITY;
      break;
    case qcril::interfaces::ConfigItem::THRESHOLD_LTE1:
      out = RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE1;
      break;
    case qcril::interfaces::ConfigItem::THRESHOLD_LTE2:
      out = RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE2;
      break;
    case qcril::interfaces::ConfigItem::THRESHOLD_LTE3:
      out = RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE3;
      break;
    case qcril::interfaces::ConfigItem::THRESHOLD_1x:
      out = RIL_IMS_CONFIG_ITEM_THRESHOLD_1x;
      break;
    case qcril::interfaces::ConfigItem::THRESHOLD_WIFI_A:
      out = RIL_IMS_CONFIG_ITEM_THRESHOLD_WIFI_A;
      break;
    case qcril::interfaces::ConfigItem::THRESHOLD_WIFI_B:
      out = RIL_IMS_CONFIG_ITEM_THRESHOLD_WIFI_B;
      break;
    case qcril::interfaces::ConfigItem::T_EPDG_LTE:
      out = RIL_IMS_CONFIG_ITEM_T_EPDG_LTE;
      break;
    case qcril::interfaces::ConfigItem::T_EPDG_WIFI:
      out = RIL_IMS_CONFIG_ITEM_T_EPDG_WIFI;
      break;
    case qcril::interfaces::ConfigItem::T_EPDG_1x:
      out = RIL_IMS_CONFIG_ITEM_T_EPDG_1x;
      break;
    case qcril::interfaces::ConfigItem::VWF_SETTING_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_VWF_SETTING_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::VCE_SETTING_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_VCE_SETTING_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::RTT_SETTING_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_RTT_SETTING_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::SMS_APP:
      out = RIL_IMS_CONFIG_ITEM_SMS_APP;
      break;
    case qcril::interfaces::ConfigItem::VVM_APP:
      out = RIL_IMS_CONFIG_ITEM_VVM_APP;
      break;
    case qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE:
      out = RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING_MODE;
      break;
    case qcril::interfaces::ConfigItem::AUTO_REJECT_CALL_ENABLED:
      out = RIL_IMS_CONFIG_ITEM_AUTO_REJECT_CALL_ENABLED;
      break;
    case qcril::interfaces::ConfigItem::CALL_COMPOSER_SETTING:
      out = RIL_IMS_CONFIG_ITEM_CALL_COMPOSER_SETTING;
      break;
    case qcril::interfaces::ConfigItem::INVALID_CONFIG:
    default:
      out = RIL_IMS_CONFIG_ITEM_INVALID;
      break;
  }
}

void convertToSocket(RIL_IMS_ConfigFailureCause& out,
                     const qcril::interfaces::ConfigFailureCause& in) {
  switch (in) {
    case qcril::interfaces::ConfigFailureCause::NO_ERR:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_NO_ERR;
      break;
    case qcril::interfaces::ConfigFailureCause::IMS_NOT_READY:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_IMS_NOT_READY;
      break;
    case qcril::interfaces::ConfigFailureCause::FILE_NOT_AVAILABLE:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_FILE_NOT_AVAILABLE;
      break;
    case qcril::interfaces::ConfigFailureCause::READ_FAILED:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_READ_FAILED;
      break;
    case qcril::interfaces::ConfigFailureCause::WRITE_FAILED:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_WRITE_FAILED;
      break;
    case qcril::interfaces::ConfigFailureCause::OTHER_INTERNAL_ERR:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_OTHER_INTERNAL_ERR;
      break;
    default:
      out = RIL_IMS_CONFIG_FAILURE_CAUSE_INVALID;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_ClipStatus& out, const qcril::interfaces::ClipStatus& in) {
  switch (in) {
    case qcril::interfaces::ClipStatus::NOT_PROVISIONED:
      out = RIL_IMS_CLIP_STATUS_NOT_PROVISIONED;
      break;
    case qcril::interfaces::ClipStatus::PROVISIONED:
      out = RIL_IMS_CLIP_STATUS_PROVISIONED;
      break;
    case qcril::interfaces::ClipStatus::STATUS_UNKNOWN:
      out = RIL_IMS_CLIP_STATUS_STATUS_UNKNOWN;
      break;
    case qcril::interfaces::ClipStatus::UNKNOWN:
    default:
      out = RIL_IMS_CLIP_STATUS_UNKNOWN;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_ConfParticipantOperation& out,
                     const qcril::interfaces::ConfParticipantOperation& in) {
  switch (in) {
    case qcril::interfaces::ConfParticipantOperation::ADD:
      out = RIL_IMS_CONF_PARTICIPANT_OP_ADD;
      break;
    case qcril::interfaces::ConfParticipantOperation::REMOVE:
      out = RIL_IMS_CONF_PARTICIPANT_OP_REMOVE;
      break;
    case qcril::interfaces::ConfParticipantOperation::UNKNOWN:
    default:
      out = RIL_IMS_CONF_PARTICIPANT_OP_UNKNOWN;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_BlockReasonType& out,
                     const qcril::interfaces::BlockReasonType& in) {
  switch (in) {
    case qcril::interfaces::BlockReasonType::PDP_FAILURE:
      out = RIL_IMS_BLOCK_REASON_TYPE_PDP_FAILURE;
      break;
    case qcril::interfaces::BlockReasonType::REGISTRATION_FAILURE:
      out = RIL_IMS_BLOCK_REASON_TYPE_REGISTRATION_FAILURE;
      break;
    case qcril::interfaces::BlockReasonType::HANDOVER_FAILURE:
      out = RIL_IMS_BLOCK_REASON_TYPE_HANDOVER_FAILURE;
      break;
    case qcril::interfaces::BlockReasonType::OTHER_FAILURE:
      out = RIL_IMS_BLOCK_REASON_TYPE_OTHER_FAILURE;
      break;
    case qcril::interfaces::BlockReasonType::UNKNOWN:
    default:
      out = RIL_IMS_BLOCK_REASON_TYPE_UNKNOWN;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_RegFailureReasonType& out,
                     const qcril::interfaces::RegFailureReasonType& in) {
  switch (in) {
    case qcril::interfaces::RegFailureReasonType::UNSPECIFIED:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_UNSPECIFIED;
      break;
    case qcril::interfaces::RegFailureReasonType::MOBILE_IP:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_MOBILE_IP;
      break;
    case qcril::interfaces::RegFailureReasonType::INTERNAL:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_INTERNAL;
      break;
    case qcril::interfaces::RegFailureReasonType::CALL_MANAGER_DEFINED:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED;
      break;
    case qcril::interfaces::RegFailureReasonType::SPEC_DEFINED:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_SPEC_DEFINED;
      break;
    case qcril::interfaces::RegFailureReasonType::PPP:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_PPP;
      break;
    case qcril::interfaces::RegFailureReasonType::EHRPD:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_EHRPD;
      break;
    case qcril::interfaces::RegFailureReasonType::IPV6:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_IPV6;
      break;
    case qcril::interfaces::RegFailureReasonType::IWLAN:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_IWLAN;
      break;
    case qcril::interfaces::RegFailureReasonType::HANDOFF:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_HANDOFF;
      break;
    case qcril::interfaces::RegFailureReasonType::UNKNOWN:
    default:
      out = RIL_IMS_REG_FAILURE_REASON_TYPE_UNKNOWN;
      break;
  }
}
template <>
void convertToSocket(RIL_IMS_VoiceInfoType& out, const qcril::interfaces::VoiceInfo& in) {
  switch (in) {
    case qcril::interfaces::VoiceInfo::SILENT:
      out = RIL_IMS_VOICE_INFO_SILENT;
      break;
    case qcril::interfaces::VoiceInfo::SPEECH:
      out = RIL_IMS_VOICE_INFO_SPEECH;
      break;
    case qcril::interfaces::VoiceInfo::UNKNOWN:
    default:
      out = RIL_IMS_VOICE_INFO_SILENT;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_ServiceClassStatus& out,
                     const qcril::interfaces::ServiceClassStatus& in) {
  switch (in) {
    case qcril::interfaces::ServiceClassStatus::DISABLED:
      out = RIL_IMS_SERVICE_CLASS_STATUS_DISABLED;
      break;
    case qcril::interfaces::ServiceClassStatus::ENABLED:
      out = RIL_IMS_SERVICE_CLASS_STATUS_ENABLED;
      break;
    case qcril::interfaces::ServiceClassStatus::UNKNOWN:
    default:
      out = RIL_IMS_SERVICE_CLASS_STATUS_DISABLED;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_ServiceClassProvisionStatus& out,
                     const qcril::interfaces::ServiceClassProvisionStatus& in) {
  switch (in) {
    case qcril::interfaces::ServiceClassProvisionStatus::NOT_PROVISIONED:
      out = RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED;
      break;
    case qcril::interfaces::ServiceClassProvisionStatus::PROVISIONED:
      out = RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED;
      break;
    case qcril::interfaces::ServiceClassProvisionStatus::UNKNOWN:
    default:
      out = RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_FacilityType& out, const qcril::interfaces::FacilityType& in) {
  switch (in) {
    case qcril::interfaces::FacilityType::CLIP:
      out = RIL_IMS_FACILITY_CLIP;
      break;
    case qcril::interfaces::FacilityType::COLP:
      out = RIL_IMS_FACILITY_COLP;
      break;
    case qcril::interfaces::FacilityType::BAOC:
      out = RIL_IMS_FACILITY_BAOC;
      break;
    case qcril::interfaces::FacilityType::BAOIC:
      out = RIL_IMS_FACILITY_BAOIC;
      break;
    case qcril::interfaces::FacilityType::BAOICxH:
      out = RIL_IMS_FACILITY_BAOICxH;
      break;
    case qcril::interfaces::FacilityType::BAIC:
      out = RIL_IMS_FACILITY_BAIC;
      break;
    case qcril::interfaces::FacilityType::BAICr:
      out = RIL_IMS_FACILITY_BAICr;
      break;
    case qcril::interfaces::FacilityType::BA_ALL:
      out = RIL_IMS_FACILITY_BA_ALL;
      break;
    case qcril::interfaces::FacilityType::BA_MO:
      out = RIL_IMS_FACILITY_BA_MO;
      break;
    case qcril::interfaces::FacilityType::BA_MT:
      out = RIL_IMS_FACILITY_BA_MT;
      break;
    case qcril::interfaces::FacilityType::BS_MT:
      out = RIL_IMS_FACILITY_BS_MT;
      break;
    case qcril::interfaces::FacilityType::BAICa:
      out = RIL_IMS_FACILITY_BAICa;
      break;
    case qcril::interfaces::FacilityType::UNKNOWN:
    default:
      out = RIL_IMS_FACILITY_CLIP;
      break;
  }
}

template <>
void convertToSocket(RIL_IMS_ConfigInfo& out, const qcril::interfaces::ConfigInfo& in) {
  if (in.hasItem()) {
    convertToSocket(out.item, in.getItem());
  }
  out.hasBoolValue = in.hasBoolValue();
  if (in.hasBoolValue()) {
    out.boolValue = in.getBoolValue();
  }
  out.hasIntValue = in.hasIntValue();
  if (in.hasIntValue()) {
    out.intValue = in.getIntValue();
  }
  if (in.hasStringValue()) {
    convertToSocket(out.stringValue, in.getStringValue());
  }
  if (in.hasErrorCause()) {
    out.hasErrorCause = TRUE;
    convertToSocket(out.errorCause, in.getErrorCause());
  }
}

template <>
void convertToSocket(std::vector<RIL_IMS_ServiceStatusInfo>& out,
                     const std::vector<qcril::interfaces::ServiceStatusInfo>& in) {
  for (const auto& inSs : in) {
    RIL_IMS_ServiceStatusInfo outSs{};
    convertToSocket(outSs, inSs);
    out.push_back(outSs);
  }
}

template <>
void convertToSocket(RIL_IMS_SubConfigInfo& out, const qcril::interfaces::ImsSubConfigInfo& in) {
  if (in.hasSimultStackCount()) {
    out.simultStackCount = in.getSimultStackCount();
  }
  convertToSocket(out.imsStackEnabled, out.imsStackEnabledLen, in.getImsStackEnabled());
}

template <>
void convertToSocket(RIL_IMS_ClirInfo& out, const qcril::interfaces::ClirInfo& in) {
  if (in.hasAction()) {
    out.action = in.getAction();
  }
  if (in.hasPresentation()) {
    out.presentation = in.getPresentation();
  }
}

template <>
void convertToSocket(RIL_IMS_ClipInfo& out, const qcril::interfaces::ClipInfo& in) {
  if (in.hasClipStatus()) {
    convertToSocket(out.clipStatus, in.getClipStatus());
  }
  if (in.hasErrorDetails()) {
    convertToSocket(out.errorDetails, in.getErrorDetails());
  }
}

template <>
void convertToSocket(RIL_IMS_BlockStatus& out, const qcril::interfaces::BlockStatus& in) {
  if (in.hasBlockReason()) {
    convertToSocket(out.blockReason, in.getBlockReason());
  }
  if (in.hasBlockReasonDetails()) {
    out.hasBlockReasonDetails = TRUE;
    convertToSocket(out.blockReasonDetails, in.getBlockReasonDetails());
  }
}

template <>
void convertToSocket(RIL_IMS_BlockReasonDetails& out, const qcril::interfaces::BlockReasonDetails& in) {
  if (in.hasRegFailureReasonType()) {
    convertToSocket(out.regFailureReasonType, in.getRegFailureReasonType());
  }
  if (in.hasRegFailureReason()) {
    out.hasRegFailureReason = TRUE;
    convertToSocket(out.regFailureReason, in.getRegFailureReason());
  }
}

template <>
void convertToSocket(RIL_IMS_CallBarringNumbersInfo& out,
                     const qcril::interfaces::CallBarringNumbersInfo& in) {
  if (in.hasStatus()) {
    convertToSocket(out.status, in.getStatus());
  }
  if (in.hasNumber()) {
    convertToSocket(out.number, in.getNumber());
  }
}

template <>
void convertToSocket(RIL_IMS_CallBarringNumbersListInfo& out,
                     const qcril::interfaces::CallBarringNumbersListInfo& in) {
  if (in.hasServiceClass()) {
    out.serviceClass = in.getServiceClass();
  }
  const auto cbNumInfoList = in.getCallBarringNumbersInfoRef();
  if (cbNumInfoList.size()) {
    convertToSocket(out.callBarringNumbersInfo, out.callBarringNumbersInfoLen, cbNumInfoList);
  }
}

template <>
void convertToSocket(RIL_IMS_SuppSvcResponse& out,
                     const qcril::interfaces::SuppServiceStatusInfo& in) {
  if (in.hasStatus()) {
    convertToSocket(out.status, in.getStatus());
  }
  if (in.hasProvisionStatus()) {
    convertToSocket(out.provisionStatus, in.getProvisionStatus());
  }
  if (in.hasFacilityType()) {
    convertToSocket(out.facilityType, in.getFacilityType());
  }
  const auto cbNumListInfo = in.getCallBarringNumbersListInfoRef();
  if (cbNumListInfo.size()) {
    convertToSocket(out.callBarringNumbersListInfo, out.callBarringNumbersListInfoLen,
                    cbNumListInfo);
  }
  if (in.hasErrorDetails()) {
    convertToSocket(out.errorDetails, in.getErrorDetails());
  }
}

template <>
void convertToSocket(RIL_IMS_IpPresentation& out, const qcril::interfaces::Presentation& in) {
  switch (in) {
  case qcril::interfaces::Presentation::ALLOWED:
    out = RIL_IMS_IP_PRESENTATION_NUM_ALLOWED;
    break;
  case qcril::interfaces::Presentation::RESTRICTED:
    out = RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED;
    break;
  case qcril::interfaces::Presentation::DEFAULT:
    out = RIL_IMS_IP_PRESENTATION_NUM_DEFAULT;
    break;
  default:
    out = RIL_IMS_IP_PRESENTATION_INVALID;
    break;
  }
}

template <>
void convertToSocket(RIL_IMS_ColrInfo& out, const qcril::interfaces::ColrInfo& in) {
  convertToSocket(out.status, in.getStatus());
  convertToSocket(out.provisionStatus, in.getProvisionStatus());
  convertToSocket(out.presentation, in.getPresentation());
  if (in.hasErrorDetails()) {
    out.errorDetails = new RIL_IMS_SipErrorInfo{};
    if (out.errorDetails)
        convertToSocket(*out.errorDetails, in.getErrorDetailsRef());
  } else {
    out.errorDetails = nullptr;
  }
}

template <>
void convertToSocket(qcril::interfaces::MultiIdentityInfo& out,
                     const RIL_IMS_MultiIdentityLineInfo& in) {
    out.msisdn = in.msisdn;
    out.registrationStatus = in.registrationStatus;
    out.lineType = in.lineType;
}

template <>
void convertToSocket(RIL_IMS_QueryVirtualLineInfoResponse& out,
                     const qcril::interfaces::VirtualLineInfo& in) {
    convertToSocket(out.msisdn, in.getMsisdn());
    auto& virtualLines = in.getVirtualLines();
    out.numLines = virtualLines.size();
    out.virtualLines = new char*[out.numLines]{};
    if (!out.virtualLines) {
        out.numLines = 0;
    } else {
        for (int i = 0; i < out.numLines; i++) {
            convertToSocket(out.virtualLines[i], virtualLines[i]);
        }
    }
}

static inline char intToHexChar(uint8_t byte_data) {
    if (byte_data >= 0 && byte_data <= 9)
        return ('0' + byte_data);
    if (byte_data > 9 && byte_data <= 15)
        return ('a' + byte_data - 10);
    return INVALID_HEX_CHAR;
}

bool convertBytesToHexString(const uint8_t* bytePayload, size_t size, std::string& str) {
    std::string out;
    for (unsigned int byte_idx = 0; byte_idx < size; byte_idx++) {
        char tmp;

        // higher bit is in the earlier index
        if ((tmp = intToHexChar(HIGHER_4BIT(bytePayload[byte_idx]))) == INVALID_HEX_CHAR) {
            return false;
        }
        out.push_back(tmp);

        if ((tmp = intToHexChar(LOWER_4BIT(bytePayload[byte_idx]))) == INVALID_HEX_CHAR) {
            return false;
        }
        out.push_back(tmp);
    }

    str = std::move(out);
    return true;
}

static uint8_t convertDtmfToAscii(uint8_t dtmfDigit)
{
    uint8_t asciiDigit;

    switch (dtmfDigit) {
        case  0: asciiDigit = 'D'; break;
        case  1: asciiDigit = '1'; break;
        case  2: asciiDigit = '2'; break;
        case  3: asciiDigit = '3'; break;
        case  4: asciiDigit = '4'; break;
        case  5: asciiDigit = '5'; break;
        case  6: asciiDigit = '6'; break;
        case  7: asciiDigit = '7'; break;
        case  8: asciiDigit = '8'; break;
        case  9: asciiDigit = '9'; break;
        case 10: asciiDigit = '0'; break;
        case 11: asciiDigit = '*'; break;
        case 12: asciiDigit = '#'; break;
        case 13: asciiDigit = 'A'; break;
        case 14: asciiDigit = 'B'; break;
        case 15: asciiDigit = 'C'; break;
        default:
            asciiDigit = 32; // Invalid DTMF code
            break;
    }

    return asciiDigit;
}

void smsConvertCdmaFormatToPseudoPdu(const RIL_CDMA_SMS_Message& cdmaMsg, std::vector<uint8_t>& payload) {
    const uint32_t MESSAGE_TYPE_POINT_TO_POINT = 0x00;
    const uint32_t MESSAGE_TYPE_BROADCAST      = 0x01;
    const uint32_t MESSAGE_TYPE_ACKNOWLEDGE    = 0x02;
    uint32_t messageType;

    if (cdmaMsg.bIsServicePresent) {
        messageType = MESSAGE_TYPE_BROADCAST;
    } else {
        if (cdmaMsg.uTeleserviceID == 0x00) {
            messageType = MESSAGE_TYPE_ACKNOWLEDGE;
        } else {
            messageType = MESSAGE_TYPE_POINT_TO_POINT;
        }
    }
    // fill the first three int
    uint32_t item = TO_NW32(messageType);
    uint8_t* base = reinterpret_cast<uint8_t*>(&item);
    payload.insert(payload.end(), base, base + 4);

    item = TO_NW32(cdmaMsg.uTeleserviceID);
    payload.insert(payload.end(), base, base + 4);

    item = TO_NW32(cdmaMsg.uServicecategory);
    payload.insert(payload.end(), base, base + 4);

    // fill the sAddress
    payload.push_back(cdmaMsg.sAddress.digit_mode);
    payload.push_back(cdmaMsg.sAddress.number_mode);
    payload.push_back(cdmaMsg.sAddress.number_type);
    payload.push_back(cdmaMsg.sAddress.number_plan);
    payload.push_back(cdmaMsg.sAddress.number_of_digits);

    // 4-bit DTMF
    if (cdmaMsg.sAddress.digit_mode == 0) {
        for (int index = 0; index < cdmaMsg.sAddress.number_of_digits; index++)
            payload.push_back(convertDtmfToAscii(cdmaMsg.sAddress.digits[index]));
    } else {
        payload.insert(payload.end(), cdmaMsg.sAddress.digits,
            cdmaMsg.sAddress.digits + cdmaMsg.sAddress.number_of_digits);
    }

    // bearerReply
    uint32_t bigEndianRep = TO_NW32(1);
    payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&bigEndianRep),
            reinterpret_cast<uint8_t*>(&bigEndianRep) + sizeof(bigEndianRep) / sizeof(uint8_t));
    // replySeqNo (not supported)
    payload.push_back(0);
    // errorClass (not supported)
    payload.push_back(0);
    // causeCode (not supported)
    payload.push_back(0);

    // bear Data length
    bigEndianRep = TO_NW32(cdmaMsg.uBearerDataLen);
    payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&bigEndianRep),
            reinterpret_cast<uint8_t*>(&bigEndianRep) + sizeof(bigEndianRep) / sizeof(uint8_t));
    payload.insert(payload.end(), cdmaMsg.aBearerData,
        cdmaMsg.aBearerData + cdmaMsg.uBearerDataLen);
}

bool smsConvertPayloadToCdmaFormat(const uint8_t* payload, size_t size, RIL_CDMA_SMS_Message& cdmaMsg) {
    if (!payload || !size) return false;
    // XXX take care of alignment!
    uint32_t index = 0;
    // read the first 3 int
    index +=  sizeof(int) * 3;
    if (index > size) return false;
    auto int_buf = reinterpret_cast<const int*>(payload);
    cdmaMsg.uTeleserviceID = FROM_NW32(int_buf[0]);
    cdmaMsg.bIsServicePresent = (FROM_NW32(int_buf[1]) == 0) ? 0 : 1;
    cdmaMsg.uServicecategory = FROM_NW32(int_buf[2]);

    // read bytes to fill sAddress
    if (index + 5 > size) return false;
    cdmaMsg.sAddress.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(payload[index++]);
    cdmaMsg.sAddress.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(payload[index++]);
    cdmaMsg.sAddress.number_type = static_cast<RIL_CDMA_SMS_NumberType>(payload[index++]);
    cdmaMsg.sAddress.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(payload[index++]);
    cdmaMsg.sAddress.number_of_digits = payload[index++];
    if (index + cdmaMsg.sAddress.number_of_digits > size)
        return false;
    memcpy(cdmaMsg.sAddress.digits, payload + index,
            cdmaMsg.sAddress.number_of_digits);
    index += cdmaMsg.sAddress.number_of_digits;

    // read bytes to fill sSubAddress
    if (index + 3 > size) return false;
    cdmaMsg.sSubAddress.subaddressType =
        static_cast<RIL_CDMA_SMS_SubaddressType>(payload[index++]);
    cdmaMsg.sSubAddress.odd = (payload[index++] == 0) ? 0 : 1;
    cdmaMsg.sSubAddress.number_of_digits = payload[index++];
    if (index + cdmaMsg.sSubAddress.number_of_digits > size)
        return false;
    memcpy(cdmaMsg.sSubAddress.digits, payload + index,
            cdmaMsg.sSubAddress.number_of_digits);
    index += cdmaMsg.sSubAddress.number_of_digits;

    // read bytes for bear data
    if (index + 1 > size) return false;
    cdmaMsg.uBearerDataLen = payload[index++];
    if (index + cdmaMsg.uBearerDataLen > size) return false;
    memcpy(cdmaMsg.aBearerData, payload + index, cdmaMsg.uBearerDataLen);

    QCRIL_LOG_INFO("payload size = %d, read size = %d", size,
        index + cdmaMsg.uBearerDataLen);

    return true;
}

RIL_IMS_VerificationStatus convertVerificationStatus(const qcril::interfaces::VerificationStatus& verificationStatus) {
    switch (verificationStatus) {
        case qcril::interfaces::VerificationStatus::NONE:
            return RIL_IMS_VerificationStatus::RIL_IMS_VERSTAT_NONE;
        case qcril::interfaces::VerificationStatus::PASS:
            return RIL_IMS_VerificationStatus::RIL_IMS_VERSTAT_PASS;
        case qcril::interfaces::VerificationStatus::FAIL:
            return RIL_IMS_VerificationStatus::RIL_IMS_VERSTAT_FAIL;
        default:
            return RIL_IMS_VerificationStatus::RIL_IMS_VERSTAT_UNKNOWN;
    }
}

RIL_IMS_TtyModeType convertTtyMode(const qcril::interfaces::TtyMode& ttyMode) {
    switch (ttyMode) {
        case qcril::interfaces::TtyMode::MODE_OFF:
            return RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_OFF;
        case qcril::interfaces::TtyMode::FULL:
            return RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_FULL;
        case qcril::interfaces::TtyMode::HCO:
            return RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_HCO;
        case qcril::interfaces::TtyMode::VCO:
            return RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_VCO;
        case qcril::interfaces::TtyMode::UNKNOWN:
        default:
            return RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_INVALID;
    }
}

RIL_IMS_SmsSendStatus getImsSmsSendStatus(RIL_Errno err, RIL_RadioTechnologyFamily tech, bool registeredOnLte) {
    switch (err) {
        case RIL_Errno::RIL_E_SUCCESS:
            return RIL_IMS_SmsSendStatus::RIL_IMS_SMS_SEND_STATUS_OK;
        case RIL_Errno::RIL_E_SMS_SEND_FAIL_RETRY:
            if (tech == RIL_RadioTechnologyFamily::RADIO_TECH_3GPP && registeredOnLte) {
                return RIL_IMS_SmsSendStatus::RIL_IMS_SMS_SEND_STATUS_ERROR;
            } else {
                return RIL_IMS_SmsSendStatus::RIL_IMS_SMS_SEND_STATUS_ERROR_FALLBACK;
            }
        default:
            return RIL_IMS_SmsSendStatus::RIL_IMS_SMS_SEND_STATUS_ERROR;
    }
}

qcril::interfaces::DeliverStatus convertImsSmsDeliveryStatus(RIL_IMS_SmsDeliveryStatus smsDeliveryStatus) {
    switch (smsDeliveryStatus) {
        case RIL_IMS_SmsDeliveryStatus::RIL_IMS_SMS_DELIVERY_STATUS_OK:
            return qcril::interfaces::DeliverStatus::STATUS_OK;
        case RIL_IMS_SmsDeliveryStatus::RIL_IMS_SMS_DELIVERY_STATUS_ERROR:
            return qcril::interfaces::DeliverStatus::STATUS_ERROR;
        case RIL_IMS_SmsDeliveryStatus::RIL_IMS_SMS_DELIVERY_STATUS_ERROR_NO_MEMORY:
            return qcril::interfaces::DeliverStatus::STATUS_NO_MEMORY;
        case RIL_IMS_SmsDeliveryStatus::RIL_IMS_SMS_DELIVERY_STATUS_ERROR_REQUEST_NOT_SUPPORTED:
            return qcril::interfaces::DeliverStatus::STATUS_REQUEST_NOT_SUPPORTED;
        default:
            return qcril::interfaces::DeliverStatus::STATUS_ERROR;
    }
}

void convertToSocket(RIL_IMS_HandoverType &type, const qcril::interfaces::HandoverType &&handoverType) {

    type = RIL_IMS_HO_UNKNOWN;

    using qcril::interfaces::HandoverType;
    switch(handoverType) {
        case HandoverType::UNKNOWN:
            type = RIL_IMS_HO_UNKNOWN;
            break;
        case HandoverType::START:
            type = RIL_IMS_HO_START;
            break;
        case HandoverType::COMPLETE_SUCCESS:
            type = RIL_IMS_HO_COMPLETE_SUCCESS;
            break;
        case HandoverType::COMPLETE_FAIL:
            type = RIL_IMS_HO_COMPLETE_FAIL;
            break;
        case HandoverType::CANCEL:
            type = RIL_IMS_HO_CANCEL;
            break;
        case HandoverType::NOT_TRIGGERED:
            type = RIL_IMS_HO_NOT_TRIGGERED;
            break;
        case HandoverType::NOT_TRIGGERED_MOBILE_DATA_OFF:
            type = RIL_IMS_HO_NOT_TRIGGERED_MOBILE_DATA_OFF;
            break;
        default:
            type = RIL_IMS_HO_INVALID;
            break;
    }
}
void convertToSocket(RIL_IMS_HandoverInfo &handoverInfo, std::shared_ptr<QcRilUnsolImsHandoverMessage> msg) {
    convertToSocket(handoverInfo.type,
            msg->hasHandoverType() ?
            msg->getHandoverType() :
            qcril::interfaces::HandoverType::UNKNOWN);
    handoverInfo.srcTech =
        msg->hasSourceTech() ?
        msg->getSourceTech() :
        RADIO_TECH_UNKNOWN;
    handoverInfo.targetTech =
        msg->hasTargetTech() ?
        msg->getTargetTech() :
        RADIO_TECH_UNKNOWN;
    if (msg->hasErrorCode()) {
        convertToSocket(handoverInfo.errorCode, msg->getErrorMsg());
    }
    if (msg->hasErrorMsg()) {
        convertToSocket(handoverInfo.errorMessage, msg->getErrorMsg());
    }
}

void convertToSocket(RIL_IMS_ConferenceCallState &callState, qcril::interfaces::ConferenceCallState src) {
    switch(src) {
        case qcril::interfaces::ConferenceCallState::FOREGROUND:
            callState = RIL_IMS_CONF_CALL_STATE_FOREGROUND;
            break;
        case qcril::interfaces::ConferenceCallState::BACKGROUND:
            callState = RIL_IMS_CONF_CALL_STATE_BACKGROUND;
            break;
        case qcril::interfaces::ConferenceCallState::RINGING:
            callState = RIL_IMS_CONF_CALL_STATE_RINGING;
            break;
        default:
            callState = RIL_IMS_CONF_CALL_STATE_UNKNOWN;
            break;
    }
}

void convertToSocket(RIL_IMS_RefreshConferenceInfo &conferenceInfo,
        std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg) {
    conferenceInfo.conferenceCallState = RIL_IMS_CONF_CALL_STATE_UNKNOWN;
    if(msg->hasConferenceCallState()) {
        convertToSocket(conferenceInfo.conferenceCallState, msg->getConferenceCallState());
    }
    conferenceInfo.confInfoUri = nullptr;
    if (msg->hasConferenceInfoUri()) {
        conferenceInfo.confInfoUri = const_cast<uint8_t *>(msg->getConferenceInfoUri().data());
    }
}

void convertToSocket(RIL_IMS_MultiIdentityRegistrationStatus &out, int in) {
    using qcril::interfaces::MultiIdentityInfo;
    switch(in) {
        case MultiIdentityInfo::REGISTRATION_STATUS_ACTIVE:
            out = RIL_IMS_STATUS_ENABLE;
            break;
        case MultiIdentityInfo::REGISTRATION_STATUS_INACTIVE:
            out = RIL_IMS_STATUS_DISABLE;
            break;
        default:
            out = RIL_IMS_STATUS_UNKNOWN;
            break;
    }
}
void convertToSocket(RIL_IMS_MultiIdentityLineType &out, int in) {
    using qcril::interfaces::MultiIdentityInfo;
    switch(in) {
        case MultiIdentityInfo::LINE_TYPE_PRIMARY:
            out = RIL_IMS_LINE_TYPE_PRIMARY;
            break;
        case MultiIdentityInfo::LINE_TYPE_SECONDARY:
            out = RIL_IMS_LINE_TYPE_SECONDARY;
            break;
        default:
            out = RIL_IMS_LINE_TYPE_UNKNOWN;
            break;
    }
}

void convertToSocket(RIL_IMS_MultiIdentityLineInfo &out,
        const qcril::interfaces::MultiIdentityInfo &in) {
    convertToSocket(out.msisdn, in.msisdn);
    convertToSocket(out.registrationStatus, in.registrationStatus);
    convertToSocket(out.lineType, in.lineType);
}

void convertToSocket(std::vector<RIL_IMS_MultiIdentityLineInfo> &out,
        const std::vector<qcril::interfaces::MultiIdentityInfo> &in) {
    for(const qcril::interfaces::MultiIdentityInfo &srcInfo: in) {
        RIL_IMS_MultiIdentityLineInfo &dstInfo = out.emplace_back();
        convertToSocket(dstInfo, srcInfo);
    }
}

bool checkThresholdAndHysteresis(int32_t hysteresisDb,
        const int32_t* const thresholds, const uint32_t thresholdsLen) {
    QCRIL_LOG_FUNC_ENTRY();

    bool ret = true;
    // hysteresisDb must be smaller than the smallest threshold delta.
    int diff = INT_MAX;
    if (thresholdsLen > 1) {
        for (int i = 0; i < thresholdsLen - 1; i++) {
            if (abs(thresholds[i + 1] - thresholds[i]) < diff) {
                diff = abs(thresholds[i + 1] - thresholds[i]);
            }
        }
    }

    QCRIL_LOG_DEBUG("diff: %d", diff);

    if((hysteresisDb != 0) && (diff <= hysteresisDb)) {
        ret = false;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

void convertToSocket(RIL_IMS_GeoLocationDataStatus& out,
                     qcril::interfaces::GeoLocationDataStatus in) {
  switch (in) {
    case qcril::interfaces::GeoLocationDataStatus::TIMEOUT:
      out = RIL_IMS_GEO_LOC_DATA_STATUS_TIMEOUT;
      break;
    case qcril::interfaces::GeoLocationDataStatus::NO_CIVIC_ADDRESS:
      out = RIL_IMS_GEO_LOC_DATA_STATUS_NO_CIVIC_ADDRESS;
      break;
    case qcril::interfaces::GeoLocationDataStatus::ENGINE_LOCK:
      out = RIL_IMS_GEO_LOC_DATA_STATUS_ENGINE_LOCK;
      break;
    case qcril::interfaces::GeoLocationDataStatus::RESOLVED:
      out = RIL_IMS_GEO_LOC_DATA_STATUS_RESOLVED;
      break;
    case qcril::interfaces::GeoLocationDataStatus::UNKNOWN:
    default:
      out = RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN;
      break;
  }
}

void release(RIL_IMS_SipErrorInfo &arg) {
  //errorCode
  delete []arg.errorString;
  arg.errorString = nullptr;
}

void release(RIL_IMS_ClipInfo &arg) {
  //arg.clipStatus;
  if (arg.errorDetails) {
    release(*arg.errorDetails);
  }
  delete arg.errorDetails;
  arg.errorDetails = nullptr;
}

void release(RIL_IMS_ColrInfo &arg) {
  //arg.status;
  //arg.provisionStatus;
  //arg.presentation;
  if (arg.errorDetails) {
    release(*arg.errorDetails);
  }
  delete arg.errorDetails;
  arg.errorDetails = nullptr;
}

void release(RIL_IMS_UssdInfo &arg) {
  //arg.mode;
  if (arg.errorDetails) {
    release(*arg.errorDetails);
  }
  delete arg.errorDetails;
  arg.errorDetails = nullptr;
}

void release(RIL_IMS_SubConfigInfo &arg) {
  //arg.simultStackCount;
  arg.imsStackEnabledLen = 0;
  delete []arg.imsStackEnabled;
  arg.imsStackEnabled = nullptr;
}

void release(RIL_IMS_MessageDetails &arg) {
  delete []arg.toAddress;
  arg.toAddress = nullptr;
  delete []arg.fromAddress;
  arg.fromAddress = nullptr;
  delete []arg.subject;
  arg.subject = nullptr;
  delete []arg.date;
  arg.date = nullptr;
  delete []arg.messageId;
  arg.messageId = nullptr;
}

void release(RIL_IMS_MessageWaitingInfo &arg) {
  delete []arg.messageSummary;
  arg.messageSummary = nullptr;
  delete []arg.ueAddress;
  arg.ueAddress = nullptr;
  if (arg.messageDetails && arg.messageDetailsLen) {
    for (size_t i = 0; i < arg.messageDetailsLen; i++) {
      release(arg.messageDetails[i]);
    }
  }
  delete []arg.messageDetails;
  arg.messageDetails = nullptr;
}

void release(RIL_IMS_HoExtra &arg) {
  delete []arg.extraInfo;
  arg.extraInfo = nullptr;
}

void release(RIL_IMS_HandoverInfo &arg) {
  if (arg.hoExtra) {
    release(*arg.hoExtra);
  }
  delete arg.hoExtra;
  arg.hoExtra = nullptr;
  delete []arg.errorCode;
  arg.errorCode = nullptr;
  delete []arg.errorMessage;
  arg.errorMessage = nullptr;
}

void release(RIL_IMS_RefreshConferenceInfo &arg) {
  delete []arg.confInfoUri;
  arg.confInfoUri = nullptr;
}

void release(RIL_IMS_MultiIdentityLineInfo &arg) {
  delete []arg.msisdn;
  arg.msisdn = nullptr;
}

void release(std::vector<RIL_IMS_MultiIdentityLineInfo> &arg) {
  for (auto &e : arg) {
    release(e);
  }
}

void release(RIL_IMS_Registration &arg) {
  //state
  //errorCode
  delete []arg.errorMessage;
  arg.errorMessage = nullptr;
  //radioTech
  delete []arg.pAssociatedUris;
  arg.pAssociatedUris = nullptr;
}

void release(RIL_IMS_AccessTechnologyStatus &arg) {
  //networkMode
  //status
  //restrictCause
  release(arg.registration);
}

void release(RIL_IMS_ServiceStatusInfo &arg) {
  //arg.callType;
  release(arg.accTechStatus);
  //arg.rttMode;
}

void release(std::vector<RIL_IMS_ServiceStatusInfo> &arg) {
  for (auto &e : arg) {
    release(e);
  }
}

void release(RIL_IMS_CallFailCauseResponse &arg) {
  //failCause
  //extendedFailCause
  //errorInfoLen
  delete []arg.errorInfo;
  arg.errorInfo = nullptr;
  delete []arg.networkErrorString;
  arg.networkErrorString = nullptr;
  if (arg.errorDetails) {
    release(*arg.errorDetails);
  }
  delete arg.errorDetails;
  arg.errorDetails = nullptr;
}

void release(RIL_UUS_Info &arg) {
  //uusType
  //uusDcs
  //uusLength
  delete []arg.uusData;
  arg.uusData = nullptr;
}

void release(RIL_IMS_CallProgressInfo &arg) {
  //type
  //reasonCode
  delete []arg.reasonText;
  arg.reasonText = nullptr;
}

void release(RIL_IMS_CallInfo &arg) {
  delete []arg.number;
  arg.number = nullptr;
  delete []arg.name;
  arg.name = nullptr;;
  delete []arg.redirNum;
  arg.redirNum = nullptr;
  delete []arg.historyInfo;
  arg.historyInfo = nullptr;
  delete []arg.sipAlternateUri;
  arg.sipAlternateUri = nullptr;
  if (arg.localAbility && arg.localAbilityLen) {
    for (size_t i = 0; i < arg.localAbilityLen; i++) {
      release(arg.localAbility[i]);
    }
  }
  delete []arg.localAbility;
  arg.localAbility = nullptr;
  if (arg.peerAbility && arg.peerAbilityLen) {
    for (size_t i = 0; i < arg.peerAbilityLen; i++) {
      release(arg.peerAbility[i]);
    }
  }
  delete []arg.peerAbility;
  arg.peerAbility = nullptr;
  if (arg.callFailCauseResponse) {
    release(*arg.callFailCauseResponse);
  }
  delete arg.callFailCauseResponse;
  arg.callFailCauseResponse = nullptr;

  delete []arg.terminatingNumber;
  arg.terminatingNumber = nullptr;

  delete arg.verstatInfo;
  if (arg.uusInfo) {
    release(*arg.uusInfo);
  }
  delete arg.uusInfo;
  arg.uusInfo = nullptr;
  delete arg.audioQuality;
  delete []arg.displayText;
  delete []arg.additionalCallInfo;
  delete []arg.childNumber;
  if (arg.callProgInfo) {
    release(*arg.callProgInfo);
  }
  delete arg.callProgInfo;
  arg.callProgInfo = nullptr;;
}

void release(RIL_IMS_CallInfo *&arg, size_t sz) {
  if (arg && sz) {
    for (size_t i = 0; i < sz; i++) {
      release(arg[i]);
    }
  }
  delete []arg;
  arg = nullptr;
}

}  // namespace utils
}  // namespace socket
}  // namespace ril
