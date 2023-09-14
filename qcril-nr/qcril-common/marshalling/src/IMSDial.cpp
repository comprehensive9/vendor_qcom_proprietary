/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSDial.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallDetails>(const RIL_IMS_CallDetails& arg) {
    WRITE_AND_CHECK(arg.callType);
    WRITE_AND_CHECK(arg.callDomain);
    WRITE_AND_CHECK(arg.extrasLength);
    for (int i = 0; i < arg.extrasLength; i++) {
        WRITE_AND_CHECK(arg.extras[i]);
    }
    WRITE_AND_CHECK(arg.localAbilityLength);
    for (int i = 0; i < arg.localAbilityLength; i++) {
        WRITE_AND_CHECK(arg.localAbility[i]);
    }
    WRITE_AND_CHECK(arg.peerAbilityLength);
    for (int i = 0; i < arg.peerAbilityLength; i++) {
        WRITE_AND_CHECK(arg.peerAbility[i]);
    }
    WRITE_AND_CHECK(arg.callSubstate);
    WRITE_AND_CHECK(arg.mediaId);
    WRITE_AND_CHECK(arg.causeCode);
    WRITE_AND_CHECK(arg.rttMode);
    WRITE_AND_CHECK(arg.sipAlternateUri);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallDetails>(RIL_IMS_CallDetails& arg) const {
    READ_AND_CHECK(arg.callType);
    READ_AND_CHECK(arg.callDomain);

    READ_AND_CHECK(arg.extrasLength);
    if (arg.extrasLength > 0) {
        arg.extras = new char*[arg.extrasLength]{};
        if (arg.extras == nullptr)
            return Result::FAILURE;
        for (int i = 0; i < arg.extrasLength; i++) {
            read<char>(arg.extras[i]);
        }
    }

    size_t size;
    auto res = readAndAlloc(arg.localAbility, size);
    if (res != Result::SUCCESS) return res;
    arg.localAbilityLength = size;
    res = readAndAlloc(arg.peerAbility, size);
    if (res != Result::SUCCESS) return res;
    arg.peerAbilityLength = size;

    READ_AND_CHECK(arg.callSubstate);
    READ_AND_CHECK(arg.mediaId);
    READ_AND_CHECK(arg.causeCode);
    READ_AND_CHECK(arg.rttMode);
    READ_AND_CHECK(arg.sipAlternateUri);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallDetails>(RIL_IMS_CallDetails& arg) {
    if (arg.extrasLength && arg.extras) {
       for (int i = 0; i < arg.extrasLength; i++) {
           release(arg.extras[i]);
       }
       delete []arg.extras;
       arg.extras = nullptr;
    }
    size_t size = arg.localAbilityLength;
    release(arg.localAbility, size);
    arg.localAbilityLength = 0;
    size = arg.peerAbilityLength;
    release(arg.peerAbility, size);
    arg.peerAbilityLength = 0;
    release(arg.sipAlternateUri);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_MultiIdentityLineInfo>(
        const RIL_IMS_MultiIdentityLineInfo& arg) {
    WRITE_AND_CHECK(arg.msisdn);
    WRITE_AND_CHECK(arg.registrationStatus);
    WRITE_AND_CHECK(arg.lineType);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_MultiIdentityLineInfo>(
        RIL_IMS_MultiIdentityLineInfo& arg) const {
    READ_AND_CHECK(arg.msisdn);
    READ_AND_CHECK(arg.registrationStatus);
    READ_AND_CHECK(arg.lineType);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_MultiIdentityLineInfo>(
        RIL_IMS_MultiIdentityLineInfo& arg)  {
    release(arg.msisdn);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_Dial>(const RIL_IMS_Dial& arg) {
    WRITE_AND_CHECK(arg.address);
    WRITE_AND_CHECK(arg.clirMode);
    WRITE_AND_CHECK(arg.presentation);
    WRITE_AND_CHECK((unsigned char)arg.hasCallDetails);
    if (arg.hasCallDetails)
        WRITE_AND_CHECK(arg.callDetails);
    WRITE_AND_CHECK((unsigned char)arg.hasIsConferenceUri);
    if (arg.hasIsConferenceUri)
        WRITE_AND_CHECK((unsigned char)arg.isConferenceUri);
    WRITE_AND_CHECK((unsigned char)arg.hasIsCallPull);
    if (arg.hasIsCallPull)
        WRITE_AND_CHECK((unsigned char)arg.isCallPull);
    WRITE_AND_CHECK((unsigned char)arg.hasIsEncrypted);
    if (arg.hasIsEncrypted)
        WRITE_AND_CHECK((unsigned char)arg.isEncrypted);
    WRITE_AND_CHECK(arg.multiLineInfo);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_Dial>(RIL_IMS_Dial& arg) const {
    READ_AND_CHECK(arg.address);
    READ_AND_CHECK(arg.clirMode);
    READ_AND_CHECK(arg.presentation);

    unsigned char rdVal;
    READ_AND_CHECK(rdVal);
    arg.hasCallDetails = static_cast<bool>(rdVal);
    if (arg.hasCallDetails)
        READ_AND_CHECK(arg.callDetails);

    READ_AND_CHECK(rdVal);
    arg.hasIsConferenceUri = static_cast<bool>(rdVal);
    if (arg.hasIsConferenceUri) {
        READ_AND_CHECK(rdVal);
        arg.isConferenceUri = static_cast<bool>(rdVal);
    }

    READ_AND_CHECK(rdVal);
    arg.hasIsCallPull = static_cast<bool>(rdVal);
    if (arg.hasIsCallPull) {
        READ_AND_CHECK(rdVal);
        arg.isCallPull = static_cast<bool>(rdVal);
    }

    READ_AND_CHECK(rdVal);
    arg.hasIsEncrypted = static_cast<bool>(rdVal);
    if (arg.hasIsEncrypted) {
        READ_AND_CHECK(rdVal);
        arg.isEncrypted = static_cast<bool>(rdVal);
    }

    READ_AND_CHECK(arg.multiLineInfo);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_IMS_Dial>(RIL_IMS_Dial& arg) {
    release(arg.address);
    release(arg.callDetails);
    release(arg.multiLineInfo);
    return Result::SUCCESS;
}
