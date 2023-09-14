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

static bool compareRilVirtualLineInfo(const RIL_IMS_QueryVirtualLineInfoResponse& info1,
        const RIL_IMS_QueryVirtualLineInfoResponse& info2) {
    if (!compareString(info1.msisdn, info2.msisdn)) return false;
    if (info1.numLines != info2.numLines) return false;
    if (info1.numLines > 0) {
        if (info1.virtualLines == nullptr || info2.virtualLines == nullptr)
            return false;
        for (int i = 0; i < info1.numLines; i++) {
            if (!compareString(info1.virtualLines[i], info2.virtualLines[i]))
                return false;
        }
    }
    return true;
}

TEST(Marshalling, IMS_VirtualLineInfo) {
    Marshal marshal;
    char strMsisdn[] = "868369110111";
    char virtual1[] = "virtual line 1";
    char virtual2[] = "virtual line 02";
    char* virtualStrings[] = {virtual1, virtual2};
    RIL_IMS_QueryVirtualLineInfoResponse virtualInfo = {
        .msisdn = strMsisdn,
        .numLines = sizeof(virtualStrings)/sizeof(char*),
        .virtualLines = virtualStrings,
    };

    char arr[] =
        "\x00\x00\x00\x0c"  // lenght of msisdn
        "868369110111"      // msisdn
        "\x00\x00\x00\x02"  // numLines
        "\x00\x00\x00\x0e"  // length of virtualLines[0],
        "virtual line 1"    // virtualLines[0],
        "\x00\x00\x00\x0f"  // length of virtualLines[1],
        "virtual line 02"   // virtualLines[1]
        ;

    std::string expected(arr, sizeof(arr));
    ASSERT_NE(marshal.write(virtualInfo),Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_IMS_QueryVirtualLineInfoResponse rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);

    ASSERT_EQ(compareRilVirtualLineInfo(virtualInfo, rdValue), true);
}
