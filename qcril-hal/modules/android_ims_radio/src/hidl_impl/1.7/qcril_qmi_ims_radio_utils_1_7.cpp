/*===========================================================================
    Copyright (c) 2021 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define TAG "RILQ"

#include <framework/Log.h>
#include <modules/android_ims_radio/hidl_impl/1.7/qcril_qmi_ims_radio_utils_1_7.h>
#include <string.h>

using ::android::hardware::hidl_bitfield;

extern "C" {
#include "qcril_other.h"
}

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_7 {
namespace utils {

void convertCrsType(const qcril::interfaces::CrsType &in,
        hidl_bitfield<V1_7::CrsType>& crsType) {
    memset(&crsType, 0, sizeof(crsType));
    switch(in) {
        case qcril::interfaces::CrsType::AUDIO:
            crsType |= V1_7::CrsType::AUDIO;
            break;
        case qcril::interfaces::CrsType::VIDEO:
            crsType |= V1_7::CrsType::VIDEO;
            break;
        case qcril::interfaces::CrsType::VIDEO_AND_AUDIO:
            crsType |= (V1_7::CrsType::VIDEO | V1_7::CrsType::AUDIO);
            break;
        case qcril::interfaces::CrsType::INVALID:
        default:
            crsType |= V1_7::CrsType::INVALID;
            break;
    }
}

V1_7::CallProgressInfoType convertCallProgressInfoType(
    const qcril::interfaces::CallProgressInfoType in) {
  switch (in) {
    case qcril::interfaces::CallProgressInfoType::CALL_REJ_Q850:
      return V1_7::CallProgressInfoType::CALL_REJ_Q850;
    case qcril::interfaces::CallProgressInfoType::CALL_WAITING:
      return V1_7::CallProgressInfoType::CALL_WAITING;
    case qcril::interfaces::CallProgressInfoType::CALL_FORWARDING:
      return V1_7::CallProgressInfoType::CALL_FORWARDING;
    case qcril::interfaces::CallProgressInfoType::REMOTE_AVAILABLE:
      return V1_7::CallProgressInfoType::REMOTE_AVAILABLE;
    case qcril::interfaces::CallProgressInfoType::UNKNOWN:
    default:
      return V1_7::CallProgressInfoType::INVALID;
  }
  return V1_7::CallProgressInfoType::INVALID;
}

bool convertCallProgressInfo(V1_7::CallProgressInfo& out,
                             const qcril::interfaces::CallProgressInfo& in) {
  out = { .type = V1_7::CallProgressInfoType::INVALID, .reasonCode = INT16_MAX, .reasonText = "" };
  if (in.hasType()) {
    out.type = convertCallProgressInfoType(in.getType());
  }
  if (in.hasReasonCode()) {
    out.reasonCode = in.getReasonCode();
  }
  if (in.hasReasonText()) {
    out.reasonText = in.getReasonText();
  }
  return true;
}

bool convertCallInfoList(hidl_vec<V1_7::CallInfo> &out,
                         const std::vector<qcril::interfaces::CallInfo> &in) {
  uint32_t count = in.size();
  out.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    out[i] = {};
    convertCallInfo(out[i], in[i]);
  }
  return true;
}

bool convertCallInfo(V1_7::CallInfo &out, const qcril::interfaces::CallInfo &in) {
  if (in.hasCallState()) {
    out.state = V1_0::utils::convertCallState(in.getCallState());
  } else {
    out.state = V1_0::CallState::CALL_STATE_INVALID;
  }

  if (in.hasIndex()) {
    out.index = in.getIndex();
  } else {
    out.index = INT32_MAX;
  }

  if (in.hasToa()) {
    out.toa = in.getToa();
  } else {
    out.toa = INT32_MAX;
  }

  out.hasIsMpty = in.hasIsMpty();
  if (in.hasIsMpty()) {
    out.isMpty = in.getIsMpty();
  }

  out.hasIsMT = in.hasIsMt();
  if (in.hasIsMt()) {
    out.isMT = in.getIsMt();
  }

  if (in.hasAls()) {
    out.als = in.getAls();
  } else {
    out.als = INT32_MAX;
  }

  out.hasIsVoice = in.hasIsVoice();
  if (in.hasIsVoice()) {
    out.isVoice = in.getIsVoice();
  }

  out.hasIsVoicePrivacy = in.hasIsVoicePrivacy();
  if (in.hasIsVoicePrivacy()) {
    out.isVoicePrivacy = in.getIsVoicePrivacy();
  }

  if (in.hasNumber() && in.getNumber().size() > 0) {
    out.number = in.getNumber().c_str();
  }

  if (in.hasNumberPresentation()) {
    out.numberPresentation = in.getNumberPresentation();
  } else {
    out.numberPresentation = INT32_MAX;
  }

  if (in.hasName() && in.getName().size() > 0) {
    out.name = in.getName().c_str();
  }

  if (in.hasNamePresentation()) {
    out.namePresentation = in.getNamePresentation();
  } else {
    out.namePresentation = INT32_MAX;
  }

  // Call Details
  out.hasCallDetails = true;

  if (in.hasCallType()) {
    out.callDetails.callType = V1_6::utils::convertCallType(in.getCallType());
  } else {
    out.callDetails.callType = V1_6::CallType::CALL_TYPE_INVALID;
  }

  if (in.hasCallDomain()) {
    out.callDetails.callDomain = V1_0::utils::convertCallDomain(in.getCallDomain());
  } else {
    out.callDetails.callDomain = V1_0::CallDomain::CALL_DOMAIN_INVALID;
  }

  uint32_t extrasLength = 0;
  std::vector<std::string> extras;
  if (in.hasCodec()) {
    std::string codec = V1_0::utils::getCodecStringExtra(in.getCodec());
    if (codec.size()) {
      extrasLength++;
      extras.push_back(codec);
    }
  }
  if (in.hasDisplayText() && in.getDisplayText().size()) {
    extrasLength++;
    extras.push_back("DisplayText=" + in.getDisplayText());
  }
  if (in.hasAdditionalCallInfo() && in.getAdditionalCallInfo().size()) {
    extrasLength++;
    extras.push_back("AdditionalCallInfo=" + in.getAdditionalCallInfo());
  }
  if (in.hasChildNumber() && in.getChildNumber().size()) {
    extrasLength++;
    extras.push_back("ChildNum=" + in.getChildNumber());
  }
  if (in.hasEmergencyServiceCategory()) {
    extrasLength++;
    extras.push_back("EmergencyServiceCategory=" +
                     std::to_string(in.getEmergencyServiceCategory()));
  }
  if (extras.size()) {
    out.callDetails.extrasLength = extras.size();
  } else {
    out.callDetails.extrasLength = INT32_MAX;
  }
  out.callDetails.extras.resize(0);
  if (extras.size()) {
    out.callDetails.extras.resize(extras.size());
    for (uint32_t i = 0; i < extras.size(); ++i) {
      out.callDetails.extras[i] = extras[i];
    }
  }

  V1_6::utils::convertServiceStatusInfoList(out.callDetails.localAbility, in.getLocalAbility());

  V1_6::utils::convertServiceStatusInfoList(out.callDetails.peerAbility, in.getPeerAbility());

  if (in.hasCallSubState()) {
    out.callDetails.callSubstate = in.getCallSubState();
  } else {
    out.callDetails.callSubstate = INT32_MAX;
  }

  if (in.hasMediaId()) {
    out.callDetails.mediaId = in.getMediaId();
  } else {
    out.callDetails.mediaId = INT32_MAX;
  }

  if (in.hasCauseCode()) {
    out.callDetails.causeCode = static_cast<uint32_t>(in.getCauseCode());
  } else {
    out.callDetails.causeCode = INT32_MAX;
  }

  if (in.hasRttMode()) {
    out.callDetails.rttMode = V1_0::utils::convertRttMode(in.getRttMode());
  } else {
    out.callDetails.rttMode = V1_0::RttMode::RTT_MODE_INVALID;
  }

  if (in.hasSipAlternateUri() && in.getSipAlternateUri().size() > 0) {
    out.callDetails.sipAlternateUri = in.getSipAlternateUri();
  }

  //=====

  out.hasFailCause = in.hasCallFailCauseResponse();
  if (in.hasCallFailCauseResponse()) {
      V1_5::utils::convertCallFailCauseResponse(out.failCause, in.getCallFailCauseResponse());
  }

  out.hasIsEncrypted = in.hasIsEncrypted();
  if (in.hasIsEncrypted()) {
    out.isEncrypted = in.getIsEncrypted();
  }

  out.hasIsCalledPartyRinging = in.hasIsCalledPartyRinging();
  if (in.hasIsCalledPartyRinging()) {
    out.isCalledPartyRinging = in.getIsCalledPartyRinging();
  }

  out.hasIsVideoConfSupported = in.hasIsVideoConfSupported();
  if (in.hasIsVideoConfSupported()) {
    out.isVideoConfSupported = in.getIsVideoConfSupported();
  }

  if (in.hasHistoryInfo() && in.getHistoryInfo().size() > 0) {
    out.historyInfo = in.getHistoryInfo().c_str();
  }

  if (in.hasVerstatInfo()) {
    V1_0::utils::convertVerstatInfo(out.verstatInfo, in.getVerstatInfo());
  } else {
    out.verstatInfo = {.canMarkUnwantedCall = false,
                       .verificationStatus = V1_2::VerificationStatus::STATUS_VALIDATION_NONE};
  }

  // Secondary Line Information
  if (in.hasTerminatingNumber() && in.getTerminatingNumber().size() > 0) {
    out.mtMultiLineInfo.msisdn = in.getTerminatingNumber().c_str();
    if (in.hasIsSecondary()) {
      out.mtMultiLineInfo.lineType = in.getIsSecondary()
                                         ? V1_4::MultiIdentityLineType::LINE_TYPE_SECONDARY
                                         : V1_4::MultiIdentityLineType::LINE_TYPE_PRIMARY;
    }
  }

  // TIR options allowed
  out.tirMode = V1_5::TirMode::INVALID;
  if (in.hasTirMode()) {
    out.tirMode = V1_5::utils::convertTirMode(in.getTirMode());
  }

  //CRS
  if (in.hasSilentUi()) {
    out.isPreparatory = in.getSilentUi();
  }
  out.crsData.originalCallType = V1_0::CallType::CALL_TYPE_INVALID;
  if (in.hasOriginalCallType()) {
    out.crsData.originalCallType = V1_0::utils::convertCallType(in.getOriginalCallType());
  }
  memset(&out.crsData.type, 0, sizeof(out.crsData.type));
  out.crsData.type |= V1_7::CrsType::INVALID;
  if (in.hasCrsType()) {
    convertCrsType(in.getCrsType(), out.crsData.type);
  }

  // Call Progress Info
  if (in.hasProgressInfo()) {
    convertCallProgressInfo(out.callProgInfo, in.getProgressInfo());
  }
  return true;
}
}  // namespace utils
}  // namespace V1_6
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
