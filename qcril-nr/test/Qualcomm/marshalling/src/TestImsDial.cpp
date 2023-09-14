/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSDial.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

static bool compareRilImsDialExtras(char** extra1, char** extra2, uint32_t len) {
    if (!len) return true;
    if (extra1 == nullptr && extra2 == nullptr) return true;
    if (extra1 == nullptr || extra2 == nullptr) return false;
    for (int i = 0; i < len; i++) {
        if (extra1[i] != nullptr && extra2[i] != nullptr) {
            if (strcmp(extra1[i], extra2[i])) return false;
        } else if (extra1[i] != nullptr || extra2[i] != nullptr) {
            return false;
        }
    }
    return true;
}

static bool compareRilImsRegistration(const RIL_IMS_Registration reg1,
        const RIL_IMS_Registration reg2) {
    if (reg1.state != reg2.state) return false;
    if (reg1.errorCode != reg2.errorCode) return false;
    if (reg1.errorMessage != nullptr && reg2.errorMessage != nullptr) {
        if (strcmp(reg1.errorMessage, reg2.errorMessage)) return false;
    } else if (reg1.errorMessage != nullptr || reg2.errorMessage != nullptr) {
        return false;
    }
    if (reg1.radioTech != reg2.radioTech) return false;
    if (reg1.pAssociatedUris != nullptr && reg2.pAssociatedUris != nullptr) {
        if (strcmp(reg1.pAssociatedUris, reg2.pAssociatedUris)) return false;
    } else if (reg1.pAssociatedUris != nullptr || reg2.pAssociatedUris != nullptr) {
        return false;
    }

    return true;
}

static bool compareRilImsDialAbility(const RIL_IMS_ServiceStatusInfo* detail1,
        const RIL_IMS_ServiceStatusInfo* detail2, uint32_t len) {
    if (!len) return true;
    if (detail1 == nullptr && detail2 == nullptr) return true;
    if (detail1 == nullptr || detail2 == nullptr) return false;
    for (int i = 0; i < len; i++) {
        if (detail1[i].callType != detail2[i].callType) return false;
        if (detail1[i].accTechStatus.networkMode !=
                detail2[i].accTechStatus.networkMode)
            return false;
        if (detail1[i].accTechStatus.status !=
                detail2[i].accTechStatus.status)
            return false;
        if (detail1[i].accTechStatus.restrictCause !=
                detail2[i].accTechStatus.restrictCause)
            return false;
        if (detail1[i].accTechStatus.status !=
                detail2[i].accTechStatus.status)
            return false;
        if (!compareRilImsRegistration(detail1[i].accTechStatus.registration,
                detail2[i].accTechStatus.registration))
            return false;
        if (detail1[i].rttMode != detail2[i].rttMode) return false;
    }

    return true;
}

static bool compareRilImsDial(const RIL_IMS_Dial& dial1, const RIL_IMS_Dial& dial2) {
    if (dial1.address != nullptr && dial2.address != nullptr) {
        if (strcmp(dial1.address, dial2.address)) return false;
    } else if (dial1.address != nullptr || dial2.address != nullptr) {
        return false;
    }
    if (dial1.clirMode != dial2.clirMode) return false;
    if (dial1.presentation != dial2.presentation) return false;
    // compare call details
    if (dial1.hasCallDetails != dial2.hasCallDetails) return false;
    if (dial1.hasCallDetails) {
        if (dial1.callDetails.callType != dial2.callDetails.callType) return false;
        if (dial1.callDetails.callDomain != dial2.callDetails.callDomain) return false;
        if (dial1.callDetails.extrasLength != dial2.callDetails.extrasLength) return false;
        if (!compareRilImsDialExtras(dial1.callDetails.extras,
                dial2.callDetails.extras, dial1.callDetails.extrasLength))
            return false;
        if (dial1.callDetails.localAbilityLength != dial2.callDetails.localAbilityLength)
            return false;
        if (!compareRilImsDialAbility(dial1.callDetails.localAbility,
                dial2.callDetails.localAbility, dial1.callDetails.localAbilityLength))
            return false;
        if (dial1.callDetails.peerAbilityLength != dial2.callDetails.peerAbilityLength)
            return false;
        if (!compareRilImsDialAbility(dial1.callDetails.peerAbility,
                dial2.callDetails.peerAbility, dial1.callDetails.peerAbilityLength))
            return false;
        if (dial1.callDetails.callSubstate != dial2.callDetails.callSubstate)
            return false;
        if (dial1.callDetails.mediaId != dial2.callDetails.mediaId) return false;
        if (dial1.callDetails.causeCode != dial2.callDetails.causeCode) return false;
        if (dial1.callDetails.rttMode != dial2.callDetails.rttMode) return false;
        if (dial1.callDetails.sipAlternateUri != nullptr
                && dial2.callDetails.sipAlternateUri != nullptr) {
            if (strcmp(dial1.callDetails.sipAlternateUri, dial2.callDetails.sipAlternateUri))
                return false;
        } else if (dial1.callDetails.sipAlternateUri != nullptr ||
                dial2.callDetails.sipAlternateUri != nullptr) {
            return false;
        }
    }
    if (dial1.hasIsConferenceUri != dial2.hasIsConferenceUri) return false;
    if (dial1.hasIsConferenceUri) {
        if (dial1.isConferenceUri != dial2.isConferenceUri) return false;
    }
    if (dial1.hasIsCallPull != dial2.hasIsCallPull) return false;
    if (dial1.hasIsCallPull) {
        if (dial1.isCallPull != dial2.isCallPull) return false;
    }
    if (dial1.hasIsEncrypted != dial2.hasIsEncrypted) return false;
    if (dial1.hasIsEncrypted) {
        if (dial1.isEncrypted != dial2.isEncrypted) return false;
    }
    // compare multiLineInfo
    if (dial1.multiLineInfo.msisdn != nullptr && dial2.multiLineInfo.msisdn != nullptr) {
        if (strcmp(dial1.multiLineInfo.msisdn, dial2.multiLineInfo.msisdn)) return false;
    } else if (dial1.multiLineInfo.msisdn != nullptr ||
            dial2.multiLineInfo.msisdn != nullptr) {
        return false;
    }
    if (dial1.multiLineInfo.registrationStatus != dial2.multiLineInfo.registrationStatus)
        return false;
    if (dial1.multiLineInfo.lineType != dial2.multiLineInfo.lineType)
        return false;

    return true;
}

TEST(Marshalling, Ims_Dial_marshall_basic) {
    Marshal marshal;
    char extra1[] = "DisplayText=hello";
    char extra2[] = "RetryCallFailReason=21";
    char extra3[] = "RetryCallFailRadioTech=20";
    char* extraList[] = {extra1, extra2, extra3};
    RIL_IMS_ServiceStatusInfo ssInfo[] = {
        {
            .callType = RIL_IMS_CALL_TYPE_VOICE,
            .accTechStatus = {
                .networkMode = RADIO_TECH_LTE_CA,
                .status = RIL_IMS_STATUS_ENABLED,
                .restrictCause = 0,
                .registration = {
                    .state = RIL_IMS_REG_STATE_REGISTERED,
                    .errorCode = 0,
                    .errorMessage = nullptr,
                    .radioTech = RADIO_TECH_LTE_CA,
                    .pAssociatedUris= "//testuri",
                },
            },
            .rttMode = RIL_IMS_RTT_FULL,
        },
        {
            .callType = RIL_IMS_CALL_TYPE_VT_RX,
            .accTechStatus = {
                .networkMode = RADIO_TECH_5G,
                .status = RIL_IMS_STATUS_ENABLED,
                .restrictCause = 0,
                .registration = {
                    .state = RIL_IMS_REG_STATE_NOT_REGISTERED,
                    .errorCode = 500,
                    .errorMessage= "Server error",
                    .radioTech = RADIO_TECH_5G,
                    .pAssociatedUris= "//testuri",
                },
            },
            .rttMode = RIL_IMS_RTT_FULL,
        },
    };
    RIL_IMS_Dial dial = {
        .address = "1234567890",
        .clirMode = 1,
        .presentation = RIL_IMS_IP_PRESENTATION_NUM_DEFAULT,
        .hasCallDetails = true,
        .callDetails = {
            .callType = RIL_IMS_CALL_TYPE_VOICE,
            .callDomain = RIL_IMS_CALLDOMAIN_AUTOMATIC,
            .extrasLength = 3,
            .extras = extraList,
            .localAbilityLength = sizeof(ssInfo) / sizeof(RIL_IMS_ServiceStatusInfo),
            .localAbility = ssInfo,
            .peerAbilityLength = 0,
            .peerAbility = nullptr,
            .callSubstate = 5,
            .mediaId = 3,
            .causeCode = 0,
            .rttMode = RIL_IMS_RTT_FULL,
            .sipAlternateUri = "sip:://",
        },
        .hasIsConferenceUri = true,
        .isConferenceUri = false,
        .hasIsCallPull = true,
        .isCallPull = true,
        .hasIsEncrypted = false,
        .isEncrypted = false,
        .multiLineInfo = {
            .msisdn = "918369110111",
            .registrationStatus = RIL_IMS_STATUS_DISABLE,
            .lineType = RIL_IMS_LINE_TYPE_PRIMARY,
        },
    };

    char arr[] =
        "\x00\x00\x00\x0a"          // address length
        "1234567890"                // address
        "\x00\x00\x00\x01"          // clirMode
        "\x00\x00\x00\x02"          // presentation
        "\x01"                      // hasCallDetails
        "\x00\x00\x00\x01"          // callDetails.callType
        "\x00\x00\x00\x03"          // callDetails.callDomain
        "\x00\x00\x00\x03"          // callDetails.extrasLength
        "\x00\x00\x00\x11"          // callDetails.extras[0] len
        "DisplayText=hello"         // callDetails.extras[0]
        "\x00\x00\x00\x16"          // callDetails.extras[1] len
        "RetryCallFailReason=21"    // callDetails.extras[1]
        "\x00\x00\x00\x19"          // callDetails.extras[2] len
        "RetryCallFailRadioTech=20" // callDetails.extras[2]
        "\x00\x00\x00\x02"          // callDetails.localAbilityLength
        "\x00\x00\x00\x01"          // callDetails.localAbility[0].callType
        "\x00\x00\x00\x13"          // callDetails.localAbility[0].accTechStatus.networkMode
        "\x00\x00\x00\x02"          // callDetails.localAbility[0].accTechStatus.status
        "\x00\x00\x00\x00"          // callDetails.localAbility[0].accTechStatus.restrictCause
        "\x00\x00\x00\x01"          // callDetails.localAbility[0].accTechStatus.registration.state
        "\x00\x00\x00\x00"          // callDetails.localAbility[0].accTechStatus.registration.errorCode
        "\xff\xff\xff\xff"          // callDetails.localAbility[0].accTechStatus.registration.errorMessage = nullptr
        "\x00\x00\x00\x13"          // callDetails.localAbility[0].accTechStatus.registration.radioTech
        "\x00\x00\x00\x09"          // callDetails.localAbility[0].accTechStatus.registration.pAssociatedUris len
        "//testuri"                 // callDetails.localAbility[0].accTechStatus.registration.pAssociatedUris
        "\x00\x00\x00\x01"          // callDetails.localAbility[0].accTechStatus.rttMode
        "\x00\x00\x00\x03"          // callDetails.localAbility[1].callType
        "\x00\x00\x00\x14"          // callDetails.localAbility[1].accTechStatus.networkMode
        "\x00\x00\x00\x02"          // callDetails.localAbility[1].accTechStatus.status
        "\x00\x00\x00\x00"          // callDetails.localAbility[1].accTechStatus.restrictCause
        "\x00\x00\x00\x02"          // callDetails.localAbility[1].accTechStatus.registration.state
        "\x00\x00\x01\xf4"          // callDetails.localAbility[1].accTechStatus.registration.errorCode
        "\x00\x00\x00\x0c"          // callDetails.localAbility[1].accTechStatus.registration.errorMessage len
        "Server error"              // callDetails.localAbility[1].accTechStatus.registration.errorMessage
        "\x00\x00\x00\x14"          // callDetails.localAbility[1].accTechStatus.registration.radioTech
        "\x00\x00\x00\x09"          // callDetails.localAbility[1].accTechStatus.registration.pAssociatedUris len
        "//testuri"                 // callDetails.localAbility[1].accTechStatus.registration.pAssociatedUris
        "\x00\x00\x00\x01"          // callDetails.localAbility[1].accTechStatus.rttMode
        "\x00\x00\x00\x00"          // callDetails.peerAbilityLength
        "\x00\x00\x00\x05"          // callDetails.callSubstate
        "\x00\x00\x00\x03"          // callDetails.mediaId
        "\x00\x00\x00\x00"          // callDetails.causeCode
        "\x00\x00\x00\x01"          // callDetails.rttMode
        "\x00\x00\x00\x07"          // callDetails.sipAlternateUri len
        "sip:://"                   // callDetails.sipAlternateUri
        "\x01"                      // hasIsConferenceUri
        "\x00"                      // isConferenceUri
        "\x01"                      // hasIsCallPull
        "\x01"                      // isCallPull
        "\x00"                      // hasIsEncrypted
        "\x00\x00\x00\x0c"          // multiLineInfo.msisdn len
        "918369110111"              // multiLineInfo.msisdn
        "\x00\x00\x00\x01"          // multiLineInfo.registrationStatus
        "\x00\x00\x00\x01"          // multiLineInfo.lineType
        ;

    std::string expected(arr, sizeof(arr));
    ASSERT_NE(marshal.write(dial),Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    //ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_IMS_Dial rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);

    ASSERT_EQ(compareRilImsDial(dial, rdValue), true);
}
