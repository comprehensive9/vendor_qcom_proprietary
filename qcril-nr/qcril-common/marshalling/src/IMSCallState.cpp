/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSCallState.h>
#include <Marshal.h>

// template <>
// Marshal::Result convert(RIL_IMS_CallInfo &out, std::shared_ptr<qcril::interfaces::CallInfo> &callInfo) {
//     if (!callInfo) {
//         return Result::FAILURE;
//     }
//     CONVERT_AND_CHECK(out.callState, callInfo->CallState);
//     CONVERT_AND_CHECK(arg.index, callInfo->Index);
//     CONVERT_AND_CHECK(arg.toa, callInfo->Toa);
//     CONVERT_AND_CHECK(arg.isMpty, callInfo->IsMpty);
//     CONVERT_AND_CHECK(arg.isMt, callInfo->IsMt);
//     CONVERT_AND_CHECK(arg.als, callInfo->Als);
//     CONVERT_AND_CHECK(arg.isVoice, callInfo->IsVoice);
//     CONVERT_AND_CHECK(arg.isVoicePrivacy, callInfo->IsVoicePrivacy);
//     CONVERT_AND_CHECK(arg.number, caallInfo->Number);
//     CONVERT_AND_CHECK(arg.numberPresentation, callInfo->NumberPresentation);
//     CONVERT_AND_CHECK(arg.name, callInfo->Name);
//     CONVERT_AND_CHECK(arg.namePresentation, callInfo->NamePresentation);
//     CONVERT_AND_CHECK(arg.redirNum, callInfo->RedirNum);
//     CONVERT_AND_CHECK(arg.redirNumPresentation, callInfo->RedirNumPresentation);
//     CONVERT_AND_CHECK(arg.callType, callInfo->CallType);
//     CONVERT_AND_CHECK(arg.callDomain, callInfo->CallDomain);
//     CONVERT_AND_CHECK(arg.callSubState, callInfo->CallState);
//     CONVERT_AND_CHECK(arg.isEncrypted, callInfo->IsEncrypted);
//     CONVERT_AND_CHECK(arg.isCalledPartyRinging, callInfo->IsCalledPartyRinging);
//     CONVERT_AND_CHECK(arg.isVideoConfSupported, callInfo->IsVideoConfSupported);
//     CONVERT_AND_CHECK(arg.historyInfo, callInfo->HistoryInfo);
//     CONVERT_AND_CHECK(arg.mediaId, callInfo->MediaId);
//     CONVERT_AND_CHECK(arg.causeCode, callInfo->CauseCode);
//     CONVERT_AND_CHECK(arg.rttMode, callInfo->RttMode);
//     CONVERT_AND_CHECK(arg.sipAlternateUri, callInfo->SipAlternateUri);
//     CONVERT_AND_CHECK(readAndAlloc(arg.localAbility, callInfo->localAbilityLen));
//     CONVERT_AND_CHECK(readAndAlloc(arg.peerAbility, callInfo->peerAbilityLen));
//     if (callInfo->hasCallFailCauseResponse()) {
//         CONVERTALLOC_AND_CHECK(arg.callFailCauseResponse, callInfo->CallFailCauseResponse, 1
//     }
//     CONVERT_AND_CHECK(readAndAlloc(arg.callFailCauseResponse, sz));
//     if (sz != 1 && sz != 0) {
//         return Result::FAILURE;
//     }
//     READ_AND_CHECK(arg.terminatingNumber);
//     READ_AND_CHECK(arg.isSecondary);
//     sz = 0;
//     RUN_AND_CHECK(readAndAlloc(arg.verstatInfo, sz));
//     if (sz != 1 && sz != 0) {
//         return Result::FAILURE;
//     }
//     sz = 0;
//     RUN_AND_CHECK(readAndAlloc(arg.uusInfo, sz));
//     if (sz != 1 && sz != 0) {
//         return Result::FAILURE;
//     }
//     sz = 0;
//     RUN_AND_CHECK(readAndAlloc(arg.audioQuality, sz));
//     if (sz != 1 && sz != 0) {
//         return Result::FAILURE;
//     }
//     READ_AND_CHECK(arg.Codec);
//     READ_AND_CHECK(arg.displayText);
//     READ_AND_CHECK(arg.additionalCallInfo);
//     READ_AND_CHECK(arg.childNumber);
//     READ_AND_CHECK(arg.emergencyServiceCategory);
//     READ_AND_CHECK(arg.tirMode);
//     return Result::SUCCESS;
// }

template <>
Marshal::Result Marshal::write<RIL_IMS_SipErrorInfo>(const RIL_IMS_SipErrorInfo &arg) {
    WRITE_AND_CHECK(arg.errorCode);
    WRITE_AND_CHECK(arg.errorString);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SipErrorInfo>(RIL_IMS_SipErrorInfo &arg) const {
    READ_AND_CHECK(arg.errorCode);
    READ_AND_CHECK(arg.errorString);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SipErrorInfo>(RIL_IMS_SipErrorInfo &arg) {
    release(arg.errorString);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_CallFailCauseResponse>(const RIL_IMS_CallFailCauseResponse &arg) {
    WRITE_AND_CHECK(arg.failCause);
    WRITE_AND_CHECK(arg.extendedFailCause);
    WRITE_AND_CHECK(arg.errorInfo, static_cast<size_t>(arg.errorInfoLen));
    WRITE_AND_CHECK(arg.networkErrorString);
    WRITE_AND_CHECK(arg.errorDetails, 1);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallFailCauseResponse>(RIL_IMS_CallFailCauseResponse &arg) const {
    READ_AND_CHECK(arg.failCause);
    READ_AND_CHECK(arg.extendedFailCause);
    RUN_AND_CHECK(readAndAlloc(arg.errorInfo, arg.errorInfoLen));
    READ_AND_CHECK(arg.networkErrorString);
    size_t sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
    if (sz != 0 && sz != 1) {
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallFailCauseResponse>(RIL_IMS_CallFailCauseResponse &arg) {
    release(arg.errorInfo);
    release(arg.networkErrorString);
    release(arg.errorDetails, 1);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_VerstatInfo>(const RIL_IMS_VerstatInfo &arg) {
    WRITE_AND_CHECK(arg.canMarkUnwantedCall);
    WRITE_AND_CHECK(arg.verificationStatus);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_VerstatInfo>(RIL_IMS_VerstatInfo &arg) const {
    READ_AND_CHECK(arg.canMarkUnwantedCall);
    READ_AND_CHECK(arg.verificationStatus);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_VerstatInfo>(RIL_IMS_VerstatInfo & /*arg*/) {
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_AudioQuality>(RIL_AudioQuality & /*arg*/) {
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_CallProgressInfo>(const RIL_IMS_CallProgressInfo &arg) {
    WRITE_AND_CHECK(arg.type);
    WRITE_AND_CHECK(arg.reasonCode);
    WRITE_AND_CHECK(arg.reasonText);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallProgressInfo>(RIL_IMS_CallProgressInfo &arg) const {
    READ_AND_CHECK(arg.type);
    READ_AND_CHECK(arg.reasonCode);
    READ_AND_CHECK(arg.reasonText);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallProgressInfo>(RIL_IMS_CallProgressInfo &arg) {
    release(arg.reasonText);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_CallInfo>(const RIL_IMS_CallInfo &arg) {
    WRITE_AND_CHECK(arg.callState);
    WRITE_AND_CHECK(arg.index);
    WRITE_AND_CHECK(arg.toa);
    WRITE_AND_CHECK(arg.isMpty);
    WRITE_AND_CHECK(arg.isMt);
    WRITE_AND_CHECK(arg.als);
    WRITE_AND_CHECK(arg.isVoice);
    WRITE_AND_CHECK(arg.isVoicePrivacy);
    WRITE_AND_CHECK(arg.number);
    WRITE_AND_CHECK(arg.numberPresentation);
    WRITE_AND_CHECK(arg.name);
    WRITE_AND_CHECK(arg.namePresentation);
    WRITE_AND_CHECK(arg.redirNum);
    WRITE_AND_CHECK(arg.redirNumPresentation);
    WRITE_AND_CHECK(arg.callType);
    WRITE_AND_CHECK(arg.callDomain);
    WRITE_AND_CHECK(arg.callSubState);
    WRITE_AND_CHECK(arg.isEncrypted);
    WRITE_AND_CHECK(arg.isCalledPartyRinging);
    WRITE_AND_CHECK(arg.isVideoConfSupported);
    WRITE_AND_CHECK(arg.historyInfo);
    WRITE_AND_CHECK(arg.mediaId);
    WRITE_AND_CHECK(arg.causeCode);
    WRITE_AND_CHECK(arg.rttMode);
    WRITE_AND_CHECK(arg.sipAlternateUri);
    WRITE_AND_CHECK(arg.localAbility, arg.localAbilityLen);
    WRITE_AND_CHECK(arg.peerAbility, arg.peerAbilityLen);
    WRITE_AND_CHECK(arg.callFailCauseResponse, 1);
    WRITE_AND_CHECK(arg.terminatingNumber);
    WRITE_AND_CHECK(arg.isSecondary);
    WRITE_AND_CHECK(arg.verstatInfo, 1);
    WRITE_AND_CHECK(arg.uusInfo, 1);
    WRITE_AND_CHECK(arg.audioQuality, 1);
    WRITE_AND_CHECK(arg.Codec);
    WRITE_AND_CHECK(arg.displayText);
    WRITE_AND_CHECK(arg.additionalCallInfo);
    WRITE_AND_CHECK(arg.childNumber);
    WRITE_AND_CHECK(arg.emergencyServiceCategory);
    WRITE_AND_CHECK(arg.tirMode);
    WRITE_AND_CHECK(arg.callProgInfo, 1);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallInfo>(RIL_IMS_CallInfo &arg) const {
    READ_AND_CHECK(arg.callState);
    READ_AND_CHECK(arg.index);
    READ_AND_CHECK(arg.toa);
    READ_AND_CHECK(arg.isMpty);
    READ_AND_CHECK(arg.isMt);
    READ_AND_CHECK(arg.als);
    READ_AND_CHECK(arg.isVoice);
    READ_AND_CHECK(arg.isVoicePrivacy);
    READ_AND_CHECK(arg.number);
    READ_AND_CHECK(arg.numberPresentation);
    READ_AND_CHECK(arg.name);
    READ_AND_CHECK(arg.namePresentation);
    READ_AND_CHECK(arg.redirNum);
    READ_AND_CHECK(arg.redirNumPresentation);
    READ_AND_CHECK(arg.callType);
    READ_AND_CHECK(arg.callDomain);
    READ_AND_CHECK(arg.callSubState);
    READ_AND_CHECK(arg.isEncrypted);
    READ_AND_CHECK(arg.isCalledPartyRinging);
    READ_AND_CHECK(arg.isVideoConfSupported);
    READ_AND_CHECK(arg.historyInfo);
    READ_AND_CHECK(arg.mediaId);
    READ_AND_CHECK(arg.causeCode);
    READ_AND_CHECK(arg.rttMode);
    READ_AND_CHECK(arg.sipAlternateUri);
    RUN_AND_CHECK(readAndAlloc(arg.localAbility, arg.localAbilityLen));
    RUN_AND_CHECK(readAndAlloc(arg.peerAbility, arg.peerAbilityLen));
    size_t sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.callFailCauseResponse, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    READ_AND_CHECK(arg.terminatingNumber);
    READ_AND_CHECK(arg.isSecondary);
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.verstatInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.uusInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.audioQuality, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    READ_AND_CHECK(arg.Codec);
    READ_AND_CHECK(arg.displayText);
    READ_AND_CHECK(arg.additionalCallInfo);
    READ_AND_CHECK(arg.childNumber);
    READ_AND_CHECK(arg.emergencyServiceCategory);
    READ_AND_CHECK(arg.tirMode);
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.callProgInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallInfo>(RIL_IMS_CallInfo &arg) {
    release(arg.number);
    release(arg.name);
    release(arg.redirNum);
    release(arg.historyInfo);
    release(arg.sipAlternateUri);
    release(arg.localAbility, arg.localAbilityLen);
    release(arg.peerAbility, arg.peerAbilityLen);
    release(arg.callFailCauseResponse, 1);
    release(arg.terminatingNumber);
    release(arg.verstatInfo, 1);
    release(arg.uusInfo, 1);
    release(arg.audioQuality, 1);
    release(arg.displayText);
    release(arg.additionalCallInfo);
    release(arg.childNumber);
    release(arg.callProgInfo, 1);
    return Result::SUCCESS;
}
