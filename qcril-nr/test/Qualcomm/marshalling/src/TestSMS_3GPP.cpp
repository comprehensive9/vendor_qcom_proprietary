/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/SMS_3GPP.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, SMS_3GPP_marshall_write_with_smsc) {
    Marshal marshal;
    char smsc[] = "8012452313";
    char pdu[] = "sometext";
    RIL_GsmSmsMessage request {.smscPdu = smsc, .pdu = pdu};

    char arr[] = "\x00\x00\x00\x01"                  // smsc present
                 "\x00\x00\x00\x0a"                  // smsc length
                 "8012452313"                        // smsc payload
                 "\x00\x00\x00\x08"                  // pdu length
                 "sometext"                          // pdu
                 ;
    std::string expected(arr, sizeof(arr));

    ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, SMS_3GPP_marshall_write_without_smsc) {
    Marshal marshal;
    char pdu[] = "sometext";
    RIL_GsmSmsMessage request {.smscPdu = nullptr, .pdu = pdu};


    char arr[] = "\x00\x00\x00\x00"                  // smsc present
                 "\x00\x00\x00\x08"                  // pdu length
                 "sometext"                          // pdu
                 ;
    std::string expected(arr, sizeof(arr));
    ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, SMS_3GPP_marshall_write_ack) {
    Marshal marshal;
    RIL_GsmSmsAck request {.result = 1, .cause = 0};

    std::string s =
        "00000001"     //result
        "00000000"     //cause
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}
