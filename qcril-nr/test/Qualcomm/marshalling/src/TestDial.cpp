/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/Dial.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Dial_marshall_basic) {
    RIL_Dial request;
    Marshal marshal;;
    request.address = "1234567890";
    request.clir = 0;
    request.uusInfo = nullptr;
    std::string s =
        "0000000a"     // Length of address
        "31323334353637383930" // Address
        "00000000"             // CLIR
        "00000000"             // UUS Present
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

}

TEST(Marshalling, Dial_marshall_uusInfo) {
    RIL_Dial request;
    Marshal marshal;

    RIL_UUS_Info info {.uusType = RIL_UUS_TYPE2_REQUIRED, .uusDcs = RIL_UUS_DCS_X244, -1, nullptr};
    request.address = "5551230000";
    request.clir = 1;
    request.uusInfo = &info;
    std::string s =
        "0000000a"     // Length of address
        "35353531323330303030" // Address
        "00000001"             // CLIR
        "00000001"             // UUS Present
        "00000003"             // UUS Type
        "00000002"             // UUS DCS
        "ffffffff"             // Data length (-1 for null)
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);

    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, Dial_marshall_uusInfo_full) {
    RIL_Dial request, out;
    Marshal marshal;

    RIL_UUS_Info info {.uusType = RIL_UUS_TYPE2_REQUIRED, .uusDcs = RIL_UUS_DCS_X244, .uusLength = 10, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09"};
    request.address = "5551230000";
    request.clir = 1;
    request.uusInfo = &info;
    std::string s =
        "0000000a"     // Length of address
        "35353531323330303030" // Address
        "00000001"             // CLIR
        "00000001"             // UUS Present
        "00000003"             // UUS Type
        "00000002"             // UUS DCS
        "0000000a"             // Data length (-1 for null)
        "00010203040506070809" // Data
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_EQ(marshal.write(request),Marshal::Result::SUCCESS);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.read(out),Marshal::Result::SUCCESS);
    std::string address_in{request.address};
    std::string address_out{out.address};
    ASSERT_EQ(address_in, address_out);
    ASSERT_EQ(request.clir, out.clir);
    ASSERT_NE(out.uusInfo, nullptr);
    ASSERT_EQ(request.uusInfo->uusType, out.uusInfo->uusType);
    ASSERT_EQ(request.uusInfo->uusDcs, out.uusInfo->uusDcs);
    ASSERT_EQ(request.uusInfo->uusLength, out.uusInfo->uusLength);
    std::string request_uusData{request.uusInfo->uusData, (size_t)request.uusInfo->uusLength};
    std::string out_uusData{out.uusInfo->uusData, (size_t)out.uusInfo->uusLength};
    ASSERT_EQ(request_uusData, out_uusData);
}
