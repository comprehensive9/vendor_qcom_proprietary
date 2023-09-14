/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILDataCallResponsev6.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>


TEST(Marshalling, RIL_Data_Call_Response_v6_marshal) {
    RIL_Data_Call_Response_v6 request;
    Marshal marshal;;
    request.status = 1;
    request.suggestedRetryTime = 2;
    request.cid = 1;
    request.active = 2;// physcial link up
    request.type = "IPV4V6";
    request.ifname = "dummy_rmnet_data0";
    request.dnses = "192.0.1.11 2001:db8::1";
    request.addresses = "192.0.2.155 2001:db8::1";
    request.gateways = "192.0.2.155 2001:db8::1";

    std::string s =
        "00000001"     //4 bytes or 8 bytes FIXME ????
        "00000002"     //4 bytes or 8 bytes FIXME ??
        "00000001"
        "00000002"
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000011"     // Length of type 17 bytes
        "64756d6d795f726d6e65745f6461746130" // dummy_rmnet_data0
        "00000016"     // Length of type 22 bytes
        "3139322e302e312e313120323030313a6462383a3a31" // 192.0.1.11 2001:db8::1
        "00000017"     // Length of type 23 bytes
        "3139322e302e322e31353520323030313a6462383a3a31" // "192.0.2.155 2001:db8::1"
        "00000017"     // Length of type 23 bytes
        "3139322e302e322e31353520323030313a6462383a3a31" // "192.0.2.155 2001:db8::1"
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

}

