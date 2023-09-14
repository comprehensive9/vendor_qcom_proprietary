/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <string>
#include <telephony/ril_ims.h>
#include <marshal/IMSVirtualLine.h>
#include <QtiRilStringUtils.h>

static bool compareString(const char* str1, const char* str2) {
    if (str1 == nullptr && str2 == nullptr) return true;
    if (str1 == nullptr || str2 == nullptr) return false;
    if (strcmp(str1, str2)) return false;
    return true;
}

static bool compareRilSipErrorInfo(const RIL_IMS_SipErrorInfo& err1,
        const RIL_IMS_SipErrorInfo& err2) {
    if (err1.errorCode != err2.errorCode) return false;
    return compareString(err1.errorString, err2.errorString);
}

static bool compareRilColrInfo(const RIL_IMS_ColrInfo& info1,
        const RIL_IMS_ColrInfo& info2) {
    if (info1.status != info2.status) return false;
    if (info1.provisionStatus != info2.provisionStatus) return false;
    if (info1.presentation != info2.presentation) return false;
    if (!info1.errorDetails && !info2.errorDetails) return true;
    if (!info1.errorDetails || !info2.errorDetails) return false;
    return compareRilSipErrorInfo(*info1.errorDetails, *info2.errorDetails);
}

TEST(Marshalling, IMS_ColrInfo) {
    Marshal marshal;
    char str1[] = "test error";
    RIL_IMS_SipErrorInfo error = {
        .errorCode = 0x5a,
        .errorString = str1,
    };
    RIL_IMS_ColrInfo colr = {
        .status = RIL_IMS_SERVICE_CLASS_STATUS_ENABLED,
        .provisionStatus = RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED,
        .presentation = RIL_IMS_IP_PRESENTATION_NUM_DEFAULT,
        .errorDetails = &error,
    };

    char arr[] =
        "\x00\x00\x00\x01"  // status
        "\x00\x00\x00\x01"  // provisionStatus
        "\x00\x00\x00\x02"  // presentation
        "\x00\x00\x00\x01"  // length of errorDetails
        "\x00\x00\x00\x5a"  // errorDetail.errorCode
        "\x00\x00\x00\x0a"  // length of errorDetails.errorString
        "test error"        // errorDetails.errorString
        ;

    std::string expected(arr, sizeof(arr));
    ASSERT_NE(marshal.write(colr), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_IMS_ColrInfo rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);

    ASSERT_EQ(compareRilColrInfo(colr, rdValue), true);
}
