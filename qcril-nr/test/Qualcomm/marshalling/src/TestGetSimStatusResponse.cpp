/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <gtest/gtest.h>
#include <marshal/GetSimStatusResponse.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Get_sim_status_marshall_basic) {
    RIL_CardStatus_v6 request{};
    RIL_CardStatus_v6 out{};
    Marshal marshal;

    request.card_state = RIL_CARDSTATE_PRESENT;
    request.physical_slot_id = 1;
    request.atr = "123456789";
    request.iccid = "98765";
    request.universal_pin_state = RIL_PINSTATE_DISABLED;
    request.gsm_umts_subscription_app_index = 1;
    request.cdma_subscription_app_index = 2;
    request.ims_subscription_app_index = 3;
    request.num_applications = 0;

    std::string s =
        "00000001"             // card state
        "00000001"             // Physical slot id
        //"00000001"             // Atr present
        "00000009"             // Atr len
        "313233343536373839"   // Atr
        //"00000001"             // iccid present
        "00000005"             // iccid len
        "3938373635"           // iccid
        "00000003"             // pin state
        "00000001"             // gsm_umts_subscription_app_index
        "00000002"             // cdma_subscription_app_index
        "00000003"             // ims_subscription_app_index
        "00000000"             // num_applications
        "00000000"             // num_applications (array)
        "ffffffff"             // eid length
        ;

    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_EQ(marshal.write(request),Marshal::Result::SUCCESS);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out.card_state, request.card_state);
    ASSERT_EQ(out.physical_slot_id, request.physical_slot_id);
    ASSERT_NE(out.atr, nullptr);
    ASSERT_EQ(std::char_traits<char>::compare(out.atr, request.atr, std::char_traits<char>::length(request.atr)), 0);
    ASSERT_NE(out.iccid, nullptr);
    ASSERT_EQ(std::char_traits<char>::compare(out.iccid, request.iccid, std::char_traits<char>::length(request.iccid)), 0);
    ASSERT_EQ(out.universal_pin_state, request.universal_pin_state);
    ASSERT_EQ(out.gsm_umts_subscription_app_index, request.gsm_umts_subscription_app_index);
    ASSERT_EQ(out.cdma_subscription_app_index, request.cdma_subscription_app_index);
    ASSERT_EQ(out.ims_subscription_app_index, request.ims_subscription_app_index);
    ASSERT_EQ(out.num_applications, request.num_applications);
    ASSERT_EQ(out.num_applications, request.num_applications);
    ASSERT_EQ(out.eid, nullptr);

}

TEST(Marshalling, Get_sim_status_marshall_applications) {

    Marshal marshal;
    RIL_AppStatus request;

    request.app_type = RIL_APPTYPE_SIM;
    request.app_state = RIL_APPSTATE_PIN;
    request.perso_substate = RIL_PERSOSUBSTATE_UNKNOWN;
    request.aid_ptr = nullptr;
    request.app_label_ptr = nullptr;
    request.pin1_replaced = 1;
    request.pin1 = RIL_PINSTATE_ENABLED_NOT_VERIFIED;
    request.pin2 =  RIL_PINSTATE_ENABLED_BLOCKED;

    std::string s =
        "00000001"             // app_type
        "00000002"             // app_state
        "00000000"             // perso_substate
        "ffffffff"             // aid_ptr length
        "ffffffff"             // app_label_ptr length
        "00000001"             // pin1_replaced
        "00000001"             // pin1
        "00000004"             // pin2
        ;

    std::string expected = QtiRilStringUtils::fromHex(s);

    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}
