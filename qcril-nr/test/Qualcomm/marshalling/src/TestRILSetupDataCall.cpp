/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <marshal/RILDataProfileInfo.h>
#include <Marshal.h>

 TEST(Marshalling, RIL_SetupDataCall_marshal) {
    RIL_DataProfileInfo_IRadio_1_4 dataProfile = {
        1,
        "IPV4V6",
        "IPV4V6",
        "IPV4V6",
         1,
         "IPV4V6",
         "IPV4V6",
         1,
         1,
         1,
         1,
         1,
         2,
         3,
         4,
         5,
         6
    };
    RIL_RadioAccessNetworks accessNetwork = (RIL_RadioAccessNetworks)0x03;
    bool roamingAllowed = true;
    RIL_RadioDataRequestReasons reason = (RIL_RadioDataRequestReasons)0x01;
    Marshal marshal;
    uint16_t val;

    RIL_DataProfileInfo_IRadio_1_4 readdataProfile;
    RIL_RadioAccessNetworks readaccessNetwork ;
    bool readroamingAllowed ;
    RIL_RadioDataRequestReasons readreason ;

    std::string s =
        "0003"     //accessNetwork
        "00000001"     //4 bytes
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"     //
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000002"
        "00000003"
        "00000004"
        "00000005"
        "00000006"
        "01" //roamingAllowed
        "0001"   //reason
        ;
     std::string expected = QtiRilStringUtils::fromHex(s);

     ASSERT_NE(marshal.write(static_cast<uint16_t>(accessNetwork)),Marshal::Result::FAILURE);
     ASSERT_NE(marshal.write(dataProfile),Marshal::Result::FAILURE);
     ASSERT_NE(marshal.writeBool(roamingAllowed),(int32_t)Marshal::Result::FAILURE);
     ASSERT_NE(marshal.write(static_cast<uint16_t>(reason)),Marshal::Result::FAILURE);
     ASSERT_EQ(expected.size(), marshal.dataSize());
     ASSERT_EQ(expected, (std::string)marshal);

     marshal.setDataPosition(0);

     ASSERT_NE(marshal.read(val), Marshal::Result::FAILURE);
     readaccessNetwork = (RIL_RadioAccessNetworks)val;
     ASSERT_NE(marshal.read(readdataProfile), Marshal::Result::FAILURE);
     readroamingAllowed = marshal.readBool();
     ASSERT_NE(marshal.read(val), Marshal::Result::FAILURE);
     readreason = (RIL_RadioDataRequestReasons)val;
     ASSERT_EQ(readaccessNetwork, accessNetwork);
     ASSERT_EQ(readreason, reason);
     ASSERT_STREQ(readdataProfile.apn, dataProfile.apn);
 }


