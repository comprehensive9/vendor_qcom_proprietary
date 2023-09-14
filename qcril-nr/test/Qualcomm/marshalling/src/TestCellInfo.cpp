/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <telephony/ril.h>
#include <marshal/SignalStrength.h>
#include <marshal/CellIdentity.h>
#include <marshal/CellInfo.h>
#include <string>
#include <QtiRilStringUtils.h>

TEST(Marshalling, CellInfo_marshall_basic) {
    RIL_CellInfo_v12 request{};
    Marshal marshal;
    request.cellInfoType = RIL_CELL_INFO_TYPE_NONE;
    request.registered = 0;
    request.timeStampType = RIL_TIMESTAMP_TYPE_UNKNOWN;
    request.timeStamp = 0xff00112233445566;
    request.connStatus = RIL_CELL_CONNECTION_STATUS_NONE;
    std::string s =
        "00000000"             // RIL_CELL_INFO_TYPE_NONE
        "00000000"             // registered
        "00000000"             // timeStampType
        "ff00112233445566"     // timeStamp
        "00000000"             // connStatus
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

}

TEST(Marshalling, CellInfo_marshal_gsm) {
    RIL_CellInfo_v12 request{};
    RIL_CellInfo_v12 output{};
    Marshal marshal;

    request.cellInfoType = RIL_CELL_INFO_TYPE_GSM;
    request.registered = 1;
    request.timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    request.timeStamp = 0x66ff001122334455;
    request.connStatus = RIL_CELL_CONNECTION_STATUS_PRIMARY;
    request.CellInfo.gsm.cellIdentityGsm.mcc[0] = '0';
    request.CellInfo.gsm.cellIdentityGsm.mcc[1] = '0';
    request.CellInfo.gsm.cellIdentityGsm.mcc[2] = '1';
    request.CellInfo.gsm.cellIdentityGsm.mnc[0] = '0';
    request.CellInfo.gsm.cellIdentityGsm.mnc[1] = '0';
    request.CellInfo.gsm.cellIdentityGsm.mnc[2] = '0';
    request.CellInfo.gsm.cellIdentityGsm.lac = 0x1356;
    request.CellInfo.gsm.cellIdentityGsm.cid = 0x2467;
    request.CellInfo.gsm.cellIdentityGsm.arfcn = 0x3578;
    request.CellInfo.gsm.cellIdentityGsm.bsic = 0xff;
    std::string s =
        "00000001"             // @0x0000: RIL_CELL_INFO_TYPE_GSM
        "00000001"             // @0x0004: registered
        "00000003"             // @0x0008: RIL_TIMESTAMP_TYPE_OEM_RIL
        "66ff001122334455"     // @0x000c: timeStamp
        "00000001"             // @0x0010: connStatus

        // RIL_CellIdentityGsm_v12
        "00000003"
        "303031"             // @0x0014: MCC
        "00000003"
        "303030"             // @0x0017: MNC
        "00001356"             // @0x001a: LAC
        "00002467"             // @0x001e: CID
        "00003578"             // @0x0022: ARFCN
        "FF"                   // @0x0026: BSIC
        // -  RIL_CellIdentityOperatorNames
        "00000000"
        ""                   // @0x0027: short alpha
        "00000000"
        ""                   // @0x0028: long alpha
        // RIL_GSM_SignalStrength_v12
        "00000000"             // @0x0029: signalStrength
        "00000000"             // @0x002d: bitErrorRate
        "00000000"             // @0x0031: timingAdvance
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, CellInfo_marshal_wcdma) {
    RIL_CellInfo_v12 request{};
    RIL_CellInfo_v12 output{};
    Marshal marshal;

    request.cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
    request.registered = 1;
    request.timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    request.timeStamp = 0x5566ff0011223344;
    request.connStatus = RIL_CELL_CONNECTION_STATUS_PRIMARY;
    request.CellInfo.wcdma.cellIdentityWcdma.mcc[0] = '0';
    request.CellInfo.wcdma.cellIdentityWcdma.mcc[1] = '0';
    request.CellInfo.wcdma.cellIdentityWcdma.mcc[2] = '2';
    request.CellInfo.wcdma.cellIdentityWcdma.mnc[0] = '0';
    request.CellInfo.wcdma.cellIdentityWcdma.mnc[1] = '0';
    request.CellInfo.wcdma.cellIdentityWcdma.mnc[2] = '1';
    request.CellInfo.wcdma.cellIdentityWcdma.lac = 0x1356;
    request.CellInfo.wcdma.cellIdentityWcdma.cid = 0x2467;
    request.CellInfo.wcdma.cellIdentityWcdma.psc = 0x01ff;
    request.CellInfo.wcdma.cellIdentityWcdma.uarfcn = 0x3578;
    std::string s =
        "00000004"             // RIL_CELL_INFO_TYPE_WCDMA
        "00000001"             // registered
        "00000003"             // RIL_TIMESTAMP_TYPE_OEM_RIL
        "5566ff0011223344"     // timeStamp
        "00000001"             // connStatus

        // RIL_CellIdentityWcdma_v12
        "00000003"
        "303032"             // MCC
        "00000003"
        "303031"             // MNC
        "00001356"             // LAC
        "00002467"             // CID
        "000001ff"             // PSC
        "00003578"             // UARFCN
        // -  RIL_CellIdentityOperatorNames
        "00000000"
        ""                   // short alpha
        "00000000"
        ""                   // long alpha
        // RIL_GSM_SignalStrength_v12
        "00000000"             // signalStrength
        "00000000"             // bitErrorRate
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    ASSERT_NE(marshal.read(output), Marshal::Result::FAILURE);
    Marshal marshal2;
    ASSERT_EQ(marshal2.write(output), Marshal::Result::SUCCESS);
    ASSERT_EQ((std::string)marshal,(std::string)marshal2);
}
