/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/QcRilRequestImsQueryServiceStatusMessage.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/voice.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

qcril::interfaces::RegState convertRegState(const RIL_IMS_RegistrationState& in) {
  switch (in) {
    case RIL_IMS_REG_STATE_REGISTERED:
      return qcril::interfaces::RegState::REGISTERED;
    case RIL_IMS_REG_STATE_NOT_REGISTERED:
      return qcril::interfaces::RegState::NOT_REGISTERED;
    case RIL_IMS_REG_STATE_REGISTERING:
      return qcril::interfaces::RegState::REGISTERING;
    case RIL_IMS_REG_STATE_UNKNOWN:
    default:
      return qcril::interfaces::RegState::UNKNOWN;
  }
  return qcril::interfaces::RegState::UNKNOWN;
}

qcril::interfaces::EctType convertEctType(const RIL_IMS_EctType &in) {
  switch (in) {
    case RIL_IMS_ECT_TYPE_BLIND_TRANSFER:
      return qcril::interfaces::EctType::BLIND_TRANSFER;
    case RIL_IMS_ECT_TYPE_ASSURED_TRANSFER:
      return qcril::interfaces::EctType::ASSURED_TRANSFER;
    case RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER:
      return qcril::interfaces::EctType::CONSULTATIVE_TRANSFER;
    case RIL_IMS_ECT_TYPE_UNKNOWN:
    default:
      return qcril::interfaces::EctType::UNKNOWN;
  }
}

qcril::interfaces::TtyMode convertTtyMode(const RIL_IMS_TtyModeType &in) {
  switch (in) {
    case RIL_IMS_TTY_MODE_OFF:
      return qcril::interfaces::TtyMode::MODE_OFF;
    case RIL_IMS_TTY_MODE_FULL:
      return qcril::interfaces::TtyMode::FULL;
    case RIL_IMS_TTY_MODE_HCO:
      return qcril::interfaces::TtyMode::HCO;
    case RIL_IMS_TTY_MODE_VCO:
      return qcril::interfaces::TtyMode::VCO;
    case RIL_IMS_TTY_MODE_INVALID:
    default:
      return qcril::interfaces::TtyMode::UNKNOWN;
  }
}

qcril::interfaces::RttMode convertRttMode(const RIL_IMS_RttMode in) {
  switch (in) {
  case RIL_IMS_RTT_FULL:
    return qcril::interfaces::RttMode::FULL;
  case RIL_IMS_RTT_DISABLED:
  default:
    return qcril::interfaces::RttMode::DISABLED;
  }
}

qcril::interfaces::CallType convertCallType(const RIL_IMS_CallType in) {
  switch (in) {
    case RIL_IMS_CALL_TYPE_VOICE:
      return qcril::interfaces::CallType::VOICE;
    case RIL_IMS_CALL_TYPE_VT_TX:
      return qcril::interfaces::CallType::VT_TX;
    case RIL_IMS_CALL_TYPE_VT_RX:
      return qcril::interfaces::CallType::VT_RX;
    case RIL_IMS_CALL_TYPE_VT:
      return qcril::interfaces::CallType::VT;
    case RIL_IMS_CALL_TYPE_VT_NODIR:
      return qcril::interfaces::CallType::VT_NODIR;
    case RIL_IMS_CALL_TYPE_SMS:
      return qcril::interfaces::CallType::SMS;
    case RIL_IMS_CALL_TYPE_UT:
      return  qcril::interfaces::CallType::UT;
    case RIL_IMS_CALL_TYPE_USSD:
      return  qcril::interfaces::CallType::USSD;
    case RIL_IMS_CALL_TYPE_CALLCOMPOSER:
      return  qcril::interfaces::CallType::CALLCOMPOSER;
    case RIL_IMS_CALL_TYPE_UNKNOWN:
    default:
      return qcril::interfaces::CallType::UNKNOWN;
  }
}

qcril::interfaces::CallDomain convertCallDomain(const RIL_IMS_CallDomain in) {
  switch (in) {
  case RIL_IMS_CALLDOMAIN_CS:
    return qcril::interfaces::CallDomain::CS;
  case RIL_IMS_CALLDOMAIN_PS:
    return qcril::interfaces::CallDomain::PS;
  case RIL_IMS_CALLDOMAIN_AUTOMATIC:
    return qcril::interfaces::CallDomain::AUTOMATIC;
  case RIL_IMS_CALLDOMAIN_NOT_SET:
    return qcril::interfaces::CallDomain::NOT_SET;
  case RIL_IMS_CALLDOMAIN_INVALID:
  case RIL_IMS_CALLDOMAIN_UNKNOWN:
  default:
    return qcril::interfaces::CallDomain::UNKNOWN;
  }
}

qcril::interfaces::Presentation convertIpPresentation(const RIL_IMS_IpPresentation in) {
  switch (in) {
  case RIL_IMS_IP_PRESENTATION_NUM_ALLOWED:
    return qcril::interfaces::Presentation::ALLOWED;
  case RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED:
    return qcril::interfaces::Presentation::RESTRICTED;
  case RIL_IMS_IP_PRESENTATION_NUM_DEFAULT:
    return qcril::interfaces::Presentation::DEFAULT;
  case RIL_IMS_IP_PRESENTATION_INVALID:
  default:
    return qcril::interfaces::Presentation::UNKNOWN;
  }
}

qcril::interfaces::ConfigItem convertConfigItem(const RIL_IMS_ConfigItem in) {
  switch(in) {
    case RIL_IMS_CONFIG_ITEM_NONE:
      return qcril::interfaces::ConfigItem::NONE;
    case RIL_IMS_CONFIG_ITEM_VOCODER_AMRMODESET:
      return qcril::interfaces::ConfigItem::VOCODER_AMRMODESET;
    case RIL_IMS_CONFIG_ITEM_VOCODER_AMRWBMODESET:
      return qcril::interfaces::ConfigItem::VOCODER_AMRWBMODESET;
    case RIL_IMS_CONFIG_ITEM_SIP_SESSION_TIMER:
      return qcril::interfaces::ConfigItem::SIP_SESSION_TIMER;
    case RIL_IMS_CONFIG_ITEM_MIN_SESSION_EXPIRY:
      return qcril::interfaces::ConfigItem::MIN_SESSION_EXPIRY;
    case RIL_IMS_CONFIG_ITEM_CANCELLATION_TIMER:
      return qcril::interfaces::ConfigItem::CANCELLATION_TIMER;
    case RIL_IMS_CONFIG_ITEM_T_DELAY:
      return qcril::interfaces::ConfigItem::T_DELAY;
    case RIL_IMS_CONFIG_ITEM_SILENT_REDIAL_ENABLE:
      return qcril::interfaces::ConfigItem::SILENT_REDIAL_ENABLE;
    case RIL_IMS_CONFIG_ITEM_SIP_T1_TIMER:
      return qcril::interfaces::ConfigItem::SIP_T1_TIMER;
    case RIL_IMS_CONFIG_ITEM_SIP_T2_TIMER:
      return qcril::interfaces::ConfigItem::SIP_T2_TIMER;
    case RIL_IMS_CONFIG_ITEM_SIP_TF_TIMER:
      return qcril::interfaces::ConfigItem::SIP_TF_TIMER;
    case RIL_IMS_CONFIG_ITEM_VLT_SETTING_ENABLED:
      return qcril::interfaces::ConfigItem::VLT_SETTING_ENABLED;
    case RIL_IMS_CONFIG_ITEM_LVC_SETTING_ENABLED:
      return qcril::interfaces::ConfigItem::LVC_SETTING_ENABLED;
    case RIL_IMS_CONFIG_ITEM_DOMAIN_NAME:
      return qcril::interfaces::ConfigItem::DOMAIN_NAME;
    case RIL_IMS_CONFIG_ITEM_SMS_FORMAT:
      return qcril::interfaces::ConfigItem::SMS_FORMAT;
    case RIL_IMS_CONFIG_ITEM_SMS_OVER_IP:
      return qcril::interfaces::ConfigItem::SMS_OVER_IP;
    case RIL_IMS_CONFIG_ITEM_PUBLISH_TIMER:
      return qcril::interfaces::ConfigItem::PUBLISH_TIMER;
    case RIL_IMS_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
      return qcril::interfaces::ConfigItem::PUBLISH_TIMER_EXTENDED;
    case RIL_IMS_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
      return qcril::interfaces::ConfigItem::CAPABILITIES_CACHE_EXPIRATION;
    case RIL_IMS_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
      return qcril::interfaces::ConfigItem::AVAILABILITY_CACHE_EXPIRATION;
    case RIL_IMS_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
      return qcril::interfaces::ConfigItem::CAPABILITIES_POLL_INTERVAL;
    case RIL_IMS_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
      return qcril::interfaces::ConfigItem::SOURCE_THROTTLE_PUBLISH;
    case RIL_IMS_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
      return qcril::interfaces::ConfigItem::MAX_NUM_ENTRIES_IN_RCL;
    case RIL_IMS_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
      return qcril::interfaces::ConfigItem::CAPAB_POLL_LIST_SUB_EXP;
    case RIL_IMS_CONFIG_ITEM_GZIP_FLAG:
      return qcril::interfaces::ConfigItem::GZIP_FLAG;
    case RIL_IMS_CONFIG_ITEM_EAB_SETTING_ENABLED:
      return qcril::interfaces::ConfigItem::EAB_SETTING_ENABLED;
    case RIL_IMS_CONFIG_ITEM_MOBILE_DATA_ENABLED:
      return qcril::interfaces::ConfigItem::MOBILE_DATA_ENABLED;
    case RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
      return qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ENABLED;
    case RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
      return qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING;
    case RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
      return qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_MODE;
    case RIL_IMS_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
      return qcril::interfaces::ConfigItem::CAPABILITY_DISCOVERY_ENABLED;
    case RIL_IMS_CONFIG_ITEM_EMERGENCY_CALL_TIMER:
      return qcril::interfaces::ConfigItem::EMERGENCY_CALL_TIMER;
    case RIL_IMS_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER:
      return qcril::interfaces::ConfigItem::SSAC_HYSTERESIS_TIMER;
    case RIL_IMS_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
      return qcril::interfaces::ConfigItem::VOLTE_USER_OPT_IN_STATUS;
    case RIL_IMS_CONFIG_ITEM_LBO_PCSCF_ADDRESS:
      return qcril::interfaces::ConfigItem::LBO_PCSCF_ADDRESS;
    case RIL_IMS_CONFIG_ITEM_KEEP_ALIVE_ENABLED:
      return qcril::interfaces::ConfigItem::KEEP_ALIVE_ENABLED;
    case RIL_IMS_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
      return qcril::interfaces::ConfigItem::REGISTRATION_RETRY_BASE_TIME_SEC;
    case RIL_IMS_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
      return qcril::interfaces::ConfigItem::REGISTRATION_RETRY_MAX_TIME_SEC;
    case RIL_IMS_CONFIG_ITEM_SPEECH_START_PORT:
      return qcril::interfaces::ConfigItem::SPEECH_START_PORT;
    case RIL_IMS_CONFIG_ITEM_SPEECH_END_PORT:
      return qcril::interfaces::ConfigItem::SPEECH_END_PORT;
    case RIL_IMS_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
      return qcril::interfaces::ConfigItem::SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
      return qcril::interfaces::ConfigItem::SIP_INVITE_RSP_WAIT_TIME_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      return qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
      return qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
      return qcril::interfaces::ConfigItem::SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
      return qcril::interfaces::ConfigItem::SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
      return qcril::interfaces::ConfigItem::SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
      return qcril::interfaces::ConfigItem::SIP_ACK_RETX_WAIT_TIME_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
      return qcril::interfaces::ConfigItem::SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
    case RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      return qcril::interfaces::ConfigItem::SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
    case RIL_IMS_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
      return qcril::interfaces::ConfigItem::AMR_WB_OCTET_ALIGNED_PT;
    case RIL_IMS_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
      return qcril::interfaces::ConfigItem::AMR_WB_BANDWIDTH_EFFICIENT_PT;
    case RIL_IMS_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
      return qcril::interfaces::ConfigItem::AMR_OCTET_ALIGNED_PT;
    case RIL_IMS_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
      return qcril::interfaces::ConfigItem::AMR_BANDWIDTH_EFFICIENT_PT;
    case RIL_IMS_CONFIG_ITEM_DTMF_WB_PT:
      return qcril::interfaces::ConfigItem::DTMF_WB_PT;
    case RIL_IMS_CONFIG_ITEM_DTMF_NB_PT:
      return qcril::interfaces::ConfigItem::DTMF_NB_PT;
    case RIL_IMS_CONFIG_ITEM_AMR_DEFAULT_MODE:
      return qcril::interfaces::ConfigItem::AMR_DEFAULT_MODE;
    case RIL_IMS_CONFIG_ITEM_SMS_PSI:
      return qcril::interfaces::ConfigItem::SMS_PSI;
    case RIL_IMS_CONFIG_ITEM_VIDEO_QUALITY:
      return qcril::interfaces::ConfigItem::VIDEO_QUALITY;
    case RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE1:
      return qcril::interfaces::ConfigItem::THRESHOLD_LTE1;
    case RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE2:
      return qcril::interfaces::ConfigItem::THRESHOLD_LTE2;
    case RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE3:
      return qcril::interfaces::ConfigItem::THRESHOLD_LTE3;
    case RIL_IMS_CONFIG_ITEM_THRESHOLD_1x:
      return qcril::interfaces::ConfigItem::THRESHOLD_1x;
    case RIL_IMS_CONFIG_ITEM_THRESHOLD_WIFI_A:
      return qcril::interfaces::ConfigItem::THRESHOLD_WIFI_A;
    case RIL_IMS_CONFIG_ITEM_THRESHOLD_WIFI_B:
      return qcril::interfaces::ConfigItem::THRESHOLD_WIFI_B;
    case RIL_IMS_CONFIG_ITEM_T_EPDG_LTE:
      return qcril::interfaces::ConfigItem::T_EPDG_LTE;
    case RIL_IMS_CONFIG_ITEM_T_EPDG_WIFI:
      return qcril::interfaces::ConfigItem::T_EPDG_WIFI;
    case RIL_IMS_CONFIG_ITEM_T_EPDG_1x:
      return qcril::interfaces::ConfigItem::T_EPDG_1x;
    case RIL_IMS_CONFIG_ITEM_VWF_SETTING_ENABLED:
      return qcril::interfaces::ConfigItem::VWF_SETTING_ENABLED;
    case RIL_IMS_CONFIG_ITEM_VCE_SETTING_ENABLED:
      return qcril::interfaces::ConfigItem::VCE_SETTING_ENABLED;
    case RIL_IMS_CONFIG_ITEM_RTT_SETTING_ENABLED:
      return qcril::interfaces::ConfigItem::RTT_SETTING_ENABLED;
    case RIL_IMS_CONFIG_ITEM_SMS_APP:
      return qcril::interfaces::ConfigItem::SMS_APP;
    case RIL_IMS_CONFIG_ITEM_VVM_APP:
      return qcril::interfaces::ConfigItem::VVM_APP;
    case RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING_MODE:
      return qcril::interfaces::ConfigItem::VOICE_OVER_WIFI_ROAMING_MODE;
    case RIL_IMS_CONFIG_ITEM_AUTO_REJECT_CALL_ENABLED:
      return qcril::interfaces::ConfigItem::AUTO_REJECT_CALL_ENABLED;
    case RIL_IMS_CONFIG_ITEM_CALL_COMPOSER_SETTING:
      return qcril::interfaces::ConfigItem::CALL_COMPOSER_SETTING;
    case RIL_IMS_CONFIG_ITEM_INVALID:
    default:
      return qcril::interfaces::ConfigItem::INVALID_CONFIG;
  }
}

qcril::interfaces::ConfigFailureCause convertConfigFailureCause(const RIL_IMS_ConfigFailureCause in) {
  switch (in) {
  case RIL_IMS_CONFIG_FAILURE_CAUSE_NO_ERR:
    return qcril::interfaces::ConfigFailureCause::NO_ERR;
  case RIL_IMS_CONFIG_FAILURE_CAUSE_IMS_NOT_READY:
    return qcril::interfaces::ConfigFailureCause::IMS_NOT_READY;
  case RIL_IMS_CONFIG_FAILURE_CAUSE_FILE_NOT_AVAILABLE:
    return qcril::interfaces::ConfigFailureCause::FILE_NOT_AVAILABLE;
  case RIL_IMS_CONFIG_FAILURE_CAUSE_READ_FAILED:
    return qcril::interfaces::ConfigFailureCause::READ_FAILED;
  case RIL_IMS_CONFIG_FAILURE_CAUSE_WRITE_FAILED:
    return qcril::interfaces::ConfigFailureCause::WRITE_FAILED;
  case RIL_IMS_CONFIG_FAILURE_CAUSE_OTHER_INTERNAL_ERR:
    return qcril::interfaces::ConfigFailureCause::OTHER_INTERNAL_ERR;
  case RIL_IMS_CONFIG_FAILURE_CAUSE_INVALID:
  default:
    return qcril::interfaces::ConfigFailureCause::NO_ERR;
  }
}

bool convertConfigInfo(qcril::interfaces::ConfigInfo& out, const RIL_IMS_ConfigInfo& in) {
  out.setItem(convertConfigItem(in.item));
  if (in.hasBoolValue) {
    out.setBoolValue(in.boolValue);
  }
  if (in.hasIntValue) {
    out.setIntValue(in.intValue);
  }
  if (in.stringValue) {
    out.setStringValue(in.stringValue);
  }
  if (in.hasErrorCause) {
    out.setErrorCause(convertConfigFailureCause(in.errorCause));
  }
  return true;
}

bool convertCallFwdTimerInfo(qcril::interfaces::CallFwdTimerInfo &out,
                             const RIL_IMS_CallFwdTimerInfo &in) {
  out.setYear(in.year);
  out.setMonth(in.month);
  out.setDay(in.day);
  out.setHour(in.hour);
  out.setMinute(in.minute);
  out.setSecond(in.second);
  out.setTimezone(in.timezone);
  return true;
}

}  // namespace api
}  // namespace socket
}  // namespace ril
