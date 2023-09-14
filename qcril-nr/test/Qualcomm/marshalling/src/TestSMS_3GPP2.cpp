/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/SMS_3GPP2.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, SMS_3GPP2_marshal_sms_payload) {
    Marshal marshal;
    /* The data filled here does NOT make any sense,specially in some array
     * (address, beardata). It is just to test the marshaling/unmarshling
     * functionality */
    RIL_CDMA_SMS_Message req = {
        .uTeleserviceID = 0x00001002,
        .bIsServicePresent = 0,
        .uServicecategory = 2,
        .sAddress = {
            .digit_mode = RIL_CDMA_SMS_DIGIT_MODE_8_BIT,
            .number_mode = RIL_CDMA_SMS_NUMBER_MODE_DATA_NETWORK,
            .number_type = RIL_CDMA_SMS_NUMBER_TYPE_SUBSCRIBER,
            .number_plan = RIL_CDMA_SMS_NUMBER_PLAN_TELEPHONY,
            .number_of_digits = 10,
            .digits = "8581234567",
        },
        .sSubAddress = {
            .subaddressType = RIL_CDMA_SMS_SUBADDRESS_TYPE_USER_SPECIFIED,
            .odd = 1,
            .number_of_digits = 5,
            .digits = "12345",
        },
        .uBearerDataLen = 10,
        .aBearerData = {'a', 'b', 'c', 'd', 'e', '1', '2', '3', '4', '5'},
        .expectMore = 0
    };

    char arr[] = "\x00\x00\x10\x02"                 // uTeleserviceID
                 "\x00"                             // bIsServicePresent
                 "\x00\x00\x00\x02"                 // uServicecategory
                 "\x00\x00\x00\x01"                 // sAddress.digit_mode
                 "\x00\x00\x00\x01"                 // sAddress.number_mode
                 "\x00\x00\x00\x04"                 // sAddress.number_type
                 "\x00\x00\x00\x01"                 // sAddress.number_plan
                 "\x00\x00\x00\x0a"                 // sAddress.number_of_digits
                 "8581234567"
                 "\x00\x00\x00\x01"                 // sSubAddress.subaddressType
                 "\x01"                             // sSubAddress.odd
                 "\x00\x00\x00\x05"                 // sSubAddress.number_of_digits
                 "12345"                            // sSubAddress.digits
                 "\x00\x00\x00\x0a"                 // uBearerDataLen
                 "abcde12345"                       // aBearerData
                 "\x00\x00\x00\x00"                 // expectMore
                 ;


    std::string expected(arr, sizeof(arr));

    ASSERT_NE(marshal.write(req), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_CDMA_SMS_Message rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);
    ASSERT_EQ(memcmp(&rdValue, &req, sizeof(rdValue)), 0);
}

TEST(Marshalling, SMS_3GPP2_marshal_sms_ack) {
    Marshal marshal;
    RIL_CDMA_SMS_Ack req {.uErrorClass = RIL_CDMA_SMS_ERROR, .uSMSCauseCode = 35 };

    std::string s =
        "00000001"      // uErrorClass
        "00000023"      // uSMSCauseCode
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(req), Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);

    RIL_CDMA_SMS_Ack rdValue{};
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);
    ASSERT_EQ(memcmp(&rdValue, &req, sizeof(rdValue)), 0);
}

TEST(Marshalling, SMS_3GPP2_marshal_sms_broadcast_config_info_array) {
    Marshal marshal;
    RIL_CDMA_BroadcastSmsConfigInfo configInfo[] = {
        { 2, 5, 1 }, { 1, 6, 0}, {3, 4, 1} };
    size_t num = sizeof(configInfo) / sizeof(RIL_CDMA_BroadcastSmsConfigInfo);

    char arr[] = "\x00\x00\x00\x03"         // size
                 "\x00\x00\x00\x02"         // service_category
                 "\x00\x00\x00\x05"         // language
                 "\x01"                     // selected
                 "\x00\x00\x00\x01"         // service_category
                 "\x00\x00\x00\x06"         // language
                 "\x00"                     // selected
                 "\x00\x00\x00\x03"         // service_category
                 "\x00\x00\x00\x04"         // language
                 "\x01"                     // selected
                 ;
    std::string expected(arr, sizeof(arr));

    ASSERT_NE(marshal.write<RIL_CDMA_BroadcastSmsConfigInfo>(
        (const RIL_CDMA_BroadcastSmsConfigInfo*)configInfo, num), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);

    size_t rdNum = 0;
    RIL_CDMA_BroadcastSmsConfigInfo* pRdConfigInfo = nullptr;
    ASSERT_NE(marshal.readAndAlloc<RIL_CDMA_BroadcastSmsConfigInfo>(
            pRdConfigInfo, rdNum), Marshal::Result::FAILURE);
    ASSERT_EQ(num, rdNum);
    ASSERT_EQ(memcmp(configInfo, pRdConfigInfo, sizeof(configInfo)), 0);
    if (pRdConfigInfo != nullptr)
        delete []pRdConfigInfo;
}
