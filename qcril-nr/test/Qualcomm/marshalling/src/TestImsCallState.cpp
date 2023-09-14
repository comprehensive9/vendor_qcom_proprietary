/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/Dial.h>
#include <string>
#include <telephony/ril_ims.h>
#include <marshal/IMSCallState.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMS_CallState) {
  Marshal marshal;
  RIL_IMS_ServiceStatusInfo localAbility[] = {
      { .callType = RIL_IMS_CALL_TYPE_VOICE,
          .accTechStatus = {
              .networkMode = RADIO_TECH_LTE,
              .status = RIL_IMS_STATUS_ENABLED,
              .restrictCause = 4,
              .registration = {
                  .state = RIL_IMS_REG_STATE_REGISTERED,
                  .errorCode = 0,
                  .errorMessage = "All is well",
                  .radioTech = RADIO_TECH_LTE,
                  .pAssociatedUris = "",
              },
          },
          .rttMode = RIL_IMS_RTT_DISABLED,
      },
      { .callType = RIL_IMS_CALL_TYPE_VT,
          .accTechStatus = {
              .networkMode = RADIO_TECH_LTE,
              .status = RIL_IMS_STATUS_ENABLED,
              .restrictCause = 4,
              .registration = {
                  .state = RIL_IMS_REG_STATE_REGISTERED,
                  .errorCode = 0,
                  .errorMessage = "All is well",
                  .radioTech = RADIO_TECH_LTE,
                  .pAssociatedUris = "",
              },
          },
          .rttMode = RIL_IMS_RTT_DISABLED,
      },
  };
  RIL_IMS_ServiceStatusInfo peerAbility[] = {
      { .callType = RIL_IMS_CALL_TYPE_VT,
          .accTechStatus = {
              .networkMode = RADIO_TECH_LTE,
              .status = RIL_IMS_STATUS_ENABLED,
              .restrictCause = 4,
              .registration = {
                  .state = RIL_IMS_REG_STATE_REGISTERED,
                  .errorCode = 0,
                  .errorMessage = "All is well",
                  .radioTech = RADIO_TECH_LTE,
                  .pAssociatedUris = "",
              },
          },
          .rttMode = RIL_IMS_RTT_DISABLED,
      },
      { .callType = RIL_IMS_CALL_TYPE_VOICE,
          .accTechStatus = {
              .networkMode = RADIO_TECH_LTE,
              .status = RIL_IMS_STATUS_ENABLED,
              .restrictCause = 4,
              .registration = {
                  .state = RIL_IMS_REG_STATE_REGISTERED,
                  .errorCode = 0,
                  .errorMessage = "All is well",
                  .radioTech = RADIO_TECH_LTE,
                  .pAssociatedUris = "",
              },
          },
          .rttMode = RIL_IMS_RTT_DISABLED,
      },
      { .callType = RIL_IMS_CALL_TYPE_VOICE,
          .accTechStatus = {
              .networkMode = RADIO_TECH_IWLAN,
              .status = RIL_IMS_STATUS_ENABLED,
              .restrictCause = 4,
              .registration = {
                  .state = RIL_IMS_REG_STATE_REGISTERED,
                  .errorCode = 0,
                  .errorMessage = "All is well",
                  .radioTech = RADIO_TECH_IWLAN,
                  .pAssociatedUris = "",
              },
          },
          .rttMode = RIL_IMS_RTT_DISABLED,
      },
  };
  RIL_AudioQuality audioQuality = RIL_AUDIO_QUAL_AMR_WB; /*2*/
  RIL_IMS_CallInfo request{ .callState = RIL_IMS_CALLSTATE_ALERTING,
                    .index = 1,
                    .toa = 2,
                    .isMpty = 0,
                    .isMt = 0,
                    .als = 0,
                    .isVoice = 1,
                    .isVoicePrivacy = 0,
                    .number = "12345",
                    .numberPresentation = 1,
                    .name = "This is the name",
                    .namePresentation = 1,
                    .redirNum = "54321",
                    .redirNumPresentation = 3,
                    .callType = RIL_IMS_CALL_TYPE_VOICE,
                    .callDomain = RIL_IMS_CALLDOMAIN_CS,
                    .callSubState = 33,
                    .isEncrypted = 0,
                    .isCalledPartyRinging = 0,
                    .isVideoConfSupported = 0,
                    .historyInfo = nullptr,
                    .mediaId = 2,
                    .causeCode = RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ,
                    .rttMode = RIL_IMS_RTT_DISABLED,
                    .sipAlternateUri = nullptr,
                    .localAbilityLen = sizeof(localAbility) / sizeof(localAbility[0]),
                    .localAbility = localAbility,
                    .peerAbilityLen = sizeof(peerAbility) / sizeof(peerAbility[0]),
                    .peerAbility = peerAbility,
                    .callFailCauseResponse = nullptr,
                    .uusInfo = nullptr,
                    .audioQuality = &audioQuality /*2*/ };

  std::string s =
      "00000004"    // RIL_IMS_CALLSTATE_ALERTING (4)
      "00000001"    // index
      "00000002"    // toa
      "00"          // isMpty
      "00"          // isMT
      "00"          // als
      "01"          // isVoice
      "00"          // isVoicePrivacy
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // numberPresentation
      "00000010"    // name length
      "5468697320697320746865206E616D65"    // name
      "00000001"    // namePresentation
      "00000005"    // redirNum length
      "3534333231"  // redirNum
      "00000003"    // redirNumPresentation
      "00000001"    // callType
      "00000001"    // callDomain
      "00000021"    // callSubstate
      "00"          // isEncrypted
      "00"          // iCaledPartyRinging
      "00"          // isVideoConfSupported
      "ffffffff"    // historyInfo
      "00000002"    // mediaId
      "00000004"    // causeCode
      "00000000"    // rttMode
      "ffffffff"    // sipAlternateUri
      "00000002"    // localAbility.length
      // localAbility {
      // localAbility[0] {
      "00000001"    // localAbility.callType(Voice)
      // localAbility[0].accTechStatus {
      "0000000e"    // localAbility[0].accTechStatus.networkMode(LTE)
      "00000002"    // localAbility[0].accTechStatus.status
      "00000004"    // localAbility[0].accTechStatus.restrictCause
      // localAbility[0].accTechStatus.registration {
      "00000001"    // localAbility[0].accTechStatus.registration.state (registered)
      "00000000"    // localAbility[0].accTechStatus.registration.errorCode
      "0000000b"    // localAbility[0].accTechStatus.registration.errorMessage length
      "416c6c2069732077656c6c" // localAbility[1].accTechStatus.registration.errorMessage
      "0000000e"    // localAbility[0].accTechStatus.registration.radioTech(LTE)
      "00000000"    // localAbility[0].accTechStatus.registration.pAssociatedUris
      // } localAbility[0].accTechStatus.registration
      // } localAbility[0].accTechStatus
      "00000000"    // localAbility[0].rttMode
      // } localAbility[0]

      // localAbility[1] {
      "00000004"    // localAbility.callType(VT)
      // localAbility[1].accTechStatus {
      "0000000e"    // localAbility[1].accTechStatus.networkMode(LTE)
      "00000002"    // localAbility[1].accTechStatus.status
      "00000004"    // localAbility[1].accTechStatus.restrictCause
      // localAbility[1].accTechStatus.registration {
      "00000001"    // localAbility[1].accTechStatus.registration.state (registered)
      "00000000"    // localAbility[1].accTechStatus.registration.errorCode
      "0000000b"    // localAbility[1].accTechStatus.registration.errorMessage length
      "416c6c2069732077656c6c" // localAbility[1].accTechStatus.registration.errorMessage
      "0000000e"    // localAbility[1].accTechStatus.registration.radioTech(LTE)
      "00000000"    // localAbility[1].accTechStatus.registration.pAssociatedUris
      // } localAbility[1].accTechStatus.registration
      // } localAbility[1].accTechStatus
      "00000000"    // localAbility[1].rttMode

      // } localAbility[1]
      // } localAbility

      "00000003"    // peerAbilityLength
      // peerAbility {
      // peerAbility[0] {
      "00000004"    // peerAbility[0].callType(VT)
      // peerAbility[0].accTechStatus {
      "0000000e"    // peerAbility[0].accTechStatus.networkMode(LTE)
      "00000002"    // peerAbility[0].accTechStatus.status(Enabled)
      "00000004"    // peerAbility[0].accTechStatus.restrictCause
      // peerAbility[0].accTechStatus.registration {
      "00000001"    // peerAbility[0].accTechStatus.registration.state
      "00000000"    // peerAbility[0].accTechStatus.registration.errorCode
      "0000000b"    // peerAbility[0].accTechStatus.registration.errorMessage length
      "416c6c2069732077656c6c" // peerAbility[0].accTechStatus.registration.errorMessage
      "0000000e"    // peerAbility[0].accTechStatus.registration.radioTech(LTE)
      "00000000"    // peerAbility[0].accTechStatus.registration.pAssociatedUris length
      // } peerAbility[0].accTechStatus.registration
      // } peerAbility[0].accTechStatus
      "00000000"    // peerAbility[0].rttMode
      // } peerAbility[0]
      // peerAbility[1] {
      "00000001"    // peerAbility[1].callType(Voice)
      // peerAbility[1].accTechStatus {
      "0000000e"    // peerAbility[1].accTechStatus.networkMode(LTE)
      "00000002"    // peerAbility[1].accTechStatus.status(Enabled)
      "00000004"    // peerAbility[1].accTechStatus.restrictCause
      // peerAbility[1].accTechStatus.registration {
      "00000001"    // peerAbility[1].accTechStatus.registration.state
      "00000000"    // peerAbility[1].accTechStatus.registration.errorCode
      "0000000b"    // peerAbility[1].accTechStatus.registration.errorMessage length
      "416c6c2069732077656c6c" // peerAbility[1].accTechStatus.registration.errorMessage
      "0000000e"    // peerAbility[1].accTechStatus.registration.radioTech(LTE)
      "00000000"    // peerAbility[1].accTechStatus.registration.pAssociatedUris length
      // } peerAbility[1].accTechStatus.registration
      // } peerAbility[1].accTechStatus
      "00000000"    // peerAbility[1].rttMode
      // } peerAbility[1]
      // peerAbility[2] {
      "00000001"    // peerAbility[2].callType(VT)
      // peerAbility[2].accTechStatus {
      "00000012"    // peerAbility[2].accTechStatus.networkMode(IWLAN)
      "00000002"    // peerAbility[2].accTechStatus.status(Enabled)
      "00000004"    // peerAbility[2].accTechStatus.restrictCause
      // peerAbility[2].accTechStatus.registration {
      "00000001"    // peerAbility[2].accTechStatus.registration.state
      "00000000"    // peerAbility[2].accTechStatus.registration.errorCode
      "0000000b"    // peerAbility[2].accTechStatus.registration.errorMessage length
      "416c6c2069732077656c6c" // peerAbility[2].accTechStatus.registration.errorMessage
      "00000012"    // peerAbility[2].accTechStatus.registration.radioTech(LTE)
      "00000000"    // peerAbility[2].accTechStatus.registration.pAssociatedUris length
      // } peerAbility[2].accTechStatus.registration
      // } peerAbility[2].accTechStatus
      "00000000"    // peerAbility[2].rttMode
      // } peerAbility[2]
      // } peerAbility
      "ffffffff"    // callFailCauseResponse length
      "ffffffff"    // terminatingNumber length
      "00"          // isSecondary
      "ffffffff"    // verstatInfo length
      "ffffffff"    // uusInfo length
      "00000001"    // audioQuality length
      "00000002"    // RIL_AUDIO_QUAL_AMR_WB (2)
      "00000000"    // Codec
      "ffffffff"    // Display Text length
      "ffffffff"    // additionalCallInfo length
      "ffffffff"    // childNumber length
      "00000000"    // emergencyServiceCategory
      "00000000"    // tirMode
      "ffffffff"    // callProgInfo length
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_CallInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.callState, request_r.callState);
  ASSERT_EQ(request.index, request_r.index);
  ASSERT_EQ(request.toa, request_r.toa);
  ASSERT_EQ(request.isMpty, request_r.isMpty);
  ASSERT_EQ(request.isMt, request_r.isMt);
  ASSERT_EQ(request.als, request_r.als);
  ASSERT_EQ(request.isVoice, request_r.isVoice);
  ASSERT_EQ(request.isVoicePrivacy, request_r.isVoicePrivacy);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.numberPresentation, request_r.numberPresentation);
  ASSERT_EQ(request.namePresentation, request_r.namePresentation);
  ASSERT_NE(request_r.audioQuality, nullptr);
  ASSERT_EQ(*request.audioQuality, *request_r.audioQuality);
}

