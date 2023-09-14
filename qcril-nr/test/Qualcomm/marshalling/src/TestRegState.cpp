/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <string>
#include <telephony/ril.h>
#include <Marshal.h>
#include <marshal/RegState.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, NAS_voice_reg_state) {
  Marshal marshal;
  RIL_VoiceRegistrationStateResponse request = {
    .regState = RIL_REG_HOME,
    .rat = RADIO_TECH_LTE,
    .cssSupported = 0,
    .roamingIndicator = 1,
    .systemIsInPrl = -1,
    .defaultRoamingIndicator = -1,
    .reasonForDenial = 0,
    .cellIdentity =
        {
            .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
            .cellIdentityLte = { .mcc = "460",
                                 .mnc = "00",
                                 .ci = -1,
                                 .pci = -1,
                                 .tac = 4411,
                                 .earfcn = -1,
                                 .operatorNames =
                                     {
                                         .alphaShort = "CMCC",
                                         .alphaLong = "CMCC",
                                     },
                                 .bandwidth = 10000 },
        },
  };
  char arr[] =
      "\x00\x01\x00\x0e"  // regState and rat
      "\x00\x00\x00\x00"  // cssSupported
      "\x00\x00\x00\x01"  // roamingIndicator
      "\xff\xff\xff\xff"  // systemIsInPrl
      "\xff\xff\xff\xff"  // defaultRoamingIndicator
      "\x00\x00\x00\x00"  // reasonForDenial
      "\x00\x00\x00\x03"  // cellIdentity.cellInfoType
      "\x00\x00\x00\x03"
      "460"  // cellIdentity.cellIdentityLte.mcc
      "\x00\x00\x00\x02"
      "00"                // cellIdentity.cellIdentityLte.mnc
      "\xff\xff\xff\xff"  // cellIdentity.cellIdentityLte.ci
      "\xff\xff\xff\xff"  // cellIdentity.cellIdentityLte.pci
      "\x00\x00\x11\x3b"  // cellIdentity.cellIdentityLte.tac
      "\xff\xff\xff\xff"  // cellIdentity.cellIdentityLte.earfcn
      "\x00\x00\x00\x04"
      "CMCC"  // cellIdentity.cellIdentityLte.operatorNames.alphaShort
      "\x00\x00\x00\x04"
      "CMCC"              // cellIdentity.cellIdentityLte.operatorNames.alphaLong
      "\x00\x00\x27\x10"  // cellIdentity.cellIdentityLte.bandwidth
      ;
  std::string expected(arr, sizeof(arr));
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  marshal.write(static_cast<char>(0));
  // ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, NAS_data_reg_state) {
  Marshal marshal;
  RIL_DataRegistrationStateResponse request = {
    .regState = RIL_REG_HOME,
    .rat = RADIO_TECH_LTE,
    .reasonDataDenied = 0,
    .maxDataCalls = 20,
    .cellIdentity =
        {
            .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
            .cellIdentityLte =
                {
                    .mcc = "460",
                    .mnc = "00",
                    .ci = 1,
                    .pci = 1,
                    .tac = -1,
                    .earfcn = 37850,
                    .operatorNames =
                        {
                            .alphaShort = "CMCC",
                            .alphaLong = "CHINA MOBILE",
                        },
                    .bandwidth = -1,
                },
        },
    .lteVopsInfoValid = 1,
    .lteVopsInfo = { .isVopsSupported = 0, .isEmcBearerSupported = 0 },
    .nrIndicatorsValid = 1,
    .nrIndicators =
        {
            .isEndcAvailable = 0,
            .isDcNrRestricted = 0,
            .plmnInfoListR15Available = 0,
        },
  };
  char arr[] =
      "\x00\x01\x00\x0e"  // regState and rat
      "\x00\x00\x00\x00"  // reasonDataDenied
      "\x00\x00\x00\x14"  // maxDataCalls
      "\x00\x00\x00\x03"  // cellIdentity.cellInfoType
      "\x00\x00\x00\x03"
      "460"  // cellIdentity.cellIdentityLte.mcc
      "\x00\x00\x00\x02"
      "00"                // cellIdentity.cellIdentityLte.mnc
      "\x00\x00\x00\x01"  // cellIdentity.cellIdentityLte.ci
      "\x00\x00\x00\x01"  // cellIdentity.cellIdentityLte.pci
      "\xff\xff\xff\xff"  // cellIdentity.cellIdentityLte.tac
      "\x00\x00\x93\xda"  // cellIdentity.cellIdentityLte.earfcn
      "\x00\x00\x00\x04"
      "CMCC"  // cellIdentity.cellIdentityLte.operatorNames.alphaShort
      "\x00\x00\x00\x0c"
      "CHINA MOBILE"      // cellIdentity.cellIdentityLte.operatorNames.alphaLong
      "\xff\xff\xff\xff"  // cellIdentity.cellIdentityLte.bandwidth
      "\x01\x00\x00"      // VopsInfo
      "\x01\x00\x00\x00"  // nrIndicators
      ;
  std::string expected(arr, sizeof(arr));
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  marshal.write(static_cast<char>(0));
  // ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
