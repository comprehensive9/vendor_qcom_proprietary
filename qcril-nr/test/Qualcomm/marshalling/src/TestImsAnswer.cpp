/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSAnswer.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Ims_Answer_marshall_basic) {
    Marshal marshal;
    RIL_IMS_Answer answer = {
        .callType = RIL_IMS_CALL_TYPE_VT,
        .presentation = RIL_IMS_IP_PRESENTATION_NUM_DEFAULT,
        .rttMode = RIL_IMS_RTT_FULL,
    };

    char arr[] =
        "\x00\x00\x00\x04"          // callType
        "\x00\x00\x00\x02"          // presentation
        "\x00\x00\x00\x01"          // rttMode
        ;

    std::string expected(arr, sizeof(arr));
    ASSERT_NE(marshal.write(answer),Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_IMS_Answer rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);
    ASSERT_EQ(memcmp(&rdValue, &answer, sizeof(rdValue)), 0);
}
