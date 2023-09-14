/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSHangup.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

static bool compareRilImsSipErrorInfo(const RIL_IMS_SipErrorInfo& info1,
        const RIL_IMS_SipErrorInfo& info2) {
    if (info1.errorCode != info2.errorCode) return false;
    if (info1.errorString != nullptr && info2.errorString != nullptr) {
        if (strcmp(info1.errorString, info2.errorString)) return false;
    } else if (info1.errorString != nullptr || info2.errorString != nullptr) {
        return false;
    }

    return true;
}

static bool compareRilImsFailureCauseResponse(const RIL_IMS_CallFailCauseResponse& cause1,
        const RIL_IMS_CallFailCauseResponse& cause2) {
    if (cause1.failCause != cause2.failCause) return false;
    if (cause1.extendedFailCause != cause2.extendedFailCause) return false;
    if (cause1.errorInfoLen != cause2.errorInfoLen) return false;
    if (cause1.errorInfoLen != 0) {
        if (cause1.errorInfo == nullptr || cause2.errorInfo == nullptr)
            return false;
        if (memcmp(cause1.errorInfo, cause2.errorInfo, cause1.errorInfoLen))
            return false;
    }
    if (cause1.networkErrorString != nullptr && cause2.networkErrorString != nullptr) {
        if (strcmp(cause1.networkErrorString, cause2.networkErrorString)) return false;
    } else if (cause1.networkErrorString != nullptr || cause2.networkErrorString != nullptr) {
        return false;
    }
    if (cause1.errorDetails != nullptr && cause2.errorDetails != nullptr) {
        if (!compareRilImsSipErrorInfo(*cause1.errorDetails,
                *cause2.errorDetails))
            return false;
    } else if (cause1.errorDetails != nullptr || cause2.errorDetails != nullptr) {
        return false;
    }

    return true;
}

static bool compareRilImsHangup(const RIL_IMS_Hangup& hangup1,
        const RIL_IMS_Hangup& hangup2) {
    if (hangup1.connIndex != hangup2.connIndex) return false;
    if (hangup1.hasMultiParty != hangup2.hasMultiParty) return false;
    if (hangup1.hasMultiParty) {
        if (hangup1.multiParty != hangup2.multiParty) return false;
    }
    if (hangup1.connUri != nullptr && hangup2.connUri != nullptr) {
        if (strcmp(hangup1.connUri, hangup2.connUri)) return false;
    } else if (hangup1.connUri != nullptr || hangup2.connUri != nullptr) {
        return false;
    }
    if (hangup1.conf_id != hangup2.conf_id) return false;
    if (hangup1.hasFailCauseResponse != hangup2.hasFailCauseResponse) return false;
    if (hangup1.hasFailCauseResponse) {
        if (!compareRilImsFailureCauseResponse(hangup1.failCauseResponse,
                hangup2.failCauseResponse))
            return false;
    }

    return true;
}

TEST(Marshalling, Ims_Hangup_marshall_basic) {
    Marshal marshal;
    char uri[] = "https://";
    uint8_t info[] = {53, 48, 48};
    RIL_IMS_SipErrorInfo details = {
        .errorCode = 103,
        .errorString = "test error",
    };

    RIL_IMS_Hangup hangup = {
        .connIndex = 2,
        .hasMultiParty = true,
        .multiParty = false,
        .connUri = uri,
        .conf_id = 0,
        .hasFailCauseResponse = true,
        .failCauseResponse = {
            .failCause = RIL_IMS_FAILCAUSE_NORMAL,
            .extendedFailCause = 0,
            .errorInfoLen = 3,
            .errorInfo = info,
            .networkErrorString = "Server Error",
            .errorDetails = &details,
        },
    };

    char arr[] =
        "\x00\x00\x00\x02"          // connIndex
        "\x01"                      // hasMultiParty
        "\x00"                      // multiParty
        "\x00\x00\x00\x08"          // connUri len
        "https://"                  // connUri
        "\x00\x00\x00\x00"          // conf_id
        "\x01"                      // hasFailCauseResponse
        "\x00\x00\x00\x10"          // failCauseResponse.failCause
        "\x00\x00\x00\x00"          // failCauseResponse.extendedFailCause
        "\x00\x00\x00\x03"          // failCauseResponse.errorInfoLen"
        "500"                       // failCauseResponse.errorInfo
        "\x00\x00\x00\x0c"          // failCauseResponse.networkErrorString len
        "Server Error"              // failCauseResponse.networkErrorString
        "\x00\x00\x00\x01"          // failCauseResponse.errorDetails array len
        "\x00\x00\x00\x67"          // failCauseResponse.errorDetails->errorCode
        "\x00\x00\x00\x0a"          // failCauseResponse.errorDetails->errorString len
        "test error"                // failCauseResponse.errorDetails->errorString
        ;

    std::string expected(arr, sizeof(arr));
    ASSERT_NE(marshal.write(hangup), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_IMS_Hangup rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);
    ASSERT_EQ(compareRilImsHangup(hangup, rdValue), true);
}
