/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/CdmaSubscription.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Cdma_Subscription) {
    Marshal marshal;

    RIL_CDMASubInfo req = {
        .mdn = "12345",
        .hSid = "5",
        .hNid = "",
        .min = "5a5a5a5a5a",
        .prl = "7",
    };

    char arr[] = "\x00\x00\x00\x05"                 // mdn length
                 "12345"                            // mdn
                 "\x00\x00\x00\x01"                 // hSid length
                 "5"                                // hSid
                 "\x00\x00\x00\x00"                 // hSid length
                 "\x00\x00\x00\x0a"                 // min length
                 "5a5a5a5a5a"                       // min
                 "\x00\x00\x00\x01"                 // prl length
                 "7"                                // prl
                 ;
    std::string expected(arr, sizeof(arr));

    ASSERT_NE(marshal.write(req), Marshal::Result::FAILURE);
    marshal.write(static_cast<char>(0));
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_CDMASubInfo rdValue;
    ASSERT_NE(marshal.read(rdValue), Marshal::Result::FAILURE);
    ASSERT_EQ(req.mdn == rdValue.mdn, true);
    ASSERT_EQ(req.hSid == rdValue.hSid, true);
    ASSERT_EQ(req.hNid == rdValue.hNid, true);
    ASSERT_EQ(req.min == rdValue.min, true);
    ASSERT_EQ(req.prl == rdValue.prl, true);
}
