/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILDataCallResponsev4.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>


TEST(Marshalling, RIL_Data_Call_Response_v4_marshal) {
    RIL_Data_Call_Response_v4 request;
    Marshal marshal;;
    request.cid = 1;
    request.active = 2; // physcial link up
    request.type = "IPV4V6";
    request.apn = "TESTAPN1";
    request.address = "192.0.11.254";

    std::string s =
        "00000001"     //4 bytes or 8 bytes FIXME ????
        "00000002"     //4 bytes or 8 bytes FIXME ??
        "00000006"     // Length of type
        "495056345636" // IPV4V6
        "00000008"     // Length of type
        "5445535441504e31" // TESTAPN1
        "0000000c"     // Length of type
        "3139322e302e31312e323534" // IPV4V6
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

}
