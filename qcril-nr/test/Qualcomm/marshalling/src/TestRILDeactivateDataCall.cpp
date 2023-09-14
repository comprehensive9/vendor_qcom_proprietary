/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <Marshal.h>
#include <iostream>

 TEST(Marshalling, RIL_DeactivateDataCall_marshal) {
    Marshal marshal;
    int cid = 2;
    bool reason = 1;
    int cid1 = 2;
    bool reason1 = 1;
    std::string s =
        "00000002"//cid
        "01"//reason
        ;
     std::string expected = QtiRilStringUtils::fromHex(s);
     ASSERT_NE(marshal.write(cid),Marshal::Result::FAILURE);
     ASSERT_NE(marshal.writeBool(reason),(int32_t)Marshal::Result::FAILURE);
     ASSERT_EQ(expected.size(), marshal.dataSize());
     ASSERT_EQ(expected, (std::string)marshal);

     marshal.setDataPosition(0);

     ASSERT_NE(marshal.read(cid1), Marshal::Result::FAILURE);
     reason1=marshal.readBool();
     ASSERT_EQ(cid1, cid);
 }


