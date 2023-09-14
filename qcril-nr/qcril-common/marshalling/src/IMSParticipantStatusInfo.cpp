/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSParticipantStatusInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ParticipantStatusInfo>(const RIL_IMS_ParticipantStatusInfo& arg) {
  WRITE_AND_CHECK(arg.callId);
  WRITE_AND_CHECK(arg.operation);
  WRITE_AND_CHECK(arg.sipStatus);
  WRITE_AND_CHECK(arg.participantUri);
  WRITE_AND_CHECK(arg.hasIsEct);
  WRITE_AND_CHECK(arg.isEct);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_ParticipantStatusInfo>(RIL_IMS_ParticipantStatusInfo& arg) const {
  READ_AND_CHECK(arg.callId);
  READ_AND_CHECK(arg.operation);
  READ_AND_CHECK(arg.sipStatus);
  READ_AND_CHECK(arg.participantUri);
  READ_AND_CHECK(arg.hasIsEct);
  READ_AND_CHECK(arg.isEct);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_ParticipantStatusInfo>(RIL_IMS_ParticipantStatusInfo& arg) {
  release(arg.participantUri);
  return Result::SUCCESS;
}
