/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSMessageWaitingInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_MessageSummary>(const RIL_IMS_MessageSummary& arg) {
  WRITE_AND_CHECK(arg.messageType);
  WRITE_AND_CHECK(arg.newMessageCount);
  WRITE_AND_CHECK(arg.oldMessageCount);
  WRITE_AND_CHECK(arg.newUrgentMessageCount);
  WRITE_AND_CHECK(arg.oldUrgentMessageCount);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_MessageSummary>(RIL_IMS_MessageSummary& arg) const {
  READ_AND_CHECK(arg.messageType);
  READ_AND_CHECK(arg.newMessageCount);
  READ_AND_CHECK(arg.oldMessageCount);
  READ_AND_CHECK(arg.newUrgentMessageCount);
  READ_AND_CHECK(arg.oldUrgentMessageCount);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_MessageDetails>(const RIL_IMS_MessageDetails& arg) {
  WRITE_AND_CHECK(arg.toAddress);
  WRITE_AND_CHECK(arg.fromAddress);
  WRITE_AND_CHECK(arg.subject);
  WRITE_AND_CHECK(arg.date);
  WRITE_AND_CHECK(arg.priority);
  WRITE_AND_CHECK(arg.messageId);
  WRITE_AND_CHECK(arg.messageType);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_MessageDetails>(RIL_IMS_MessageDetails& arg) const {
  READ_AND_CHECK(arg.toAddress);
  READ_AND_CHECK(arg.fromAddress);
  READ_AND_CHECK(arg.subject);
  READ_AND_CHECK(arg.date);
  READ_AND_CHECK(arg.priority);
  READ_AND_CHECK(arg.messageId);
  READ_AND_CHECK(arg.messageType);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_MessageWaitingInfo>(const RIL_IMS_MessageWaitingInfo& arg) {
  WRITE_AND_CHECK(arg.messageSummary, arg.messageSummaryLen);
  WRITE_AND_CHECK(arg.ueAddress);
  WRITE_AND_CHECK(arg.messageDetails, arg.messageDetailsLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_MessageWaitingInfo>(RIL_IMS_MessageWaitingInfo& arg) const {
  RUN_AND_CHECK(readAndAlloc(arg.messageSummary, arg.messageSummaryLen));
  READ_AND_CHECK(arg.ueAddress);
  RUN_AND_CHECK(readAndAlloc(arg.messageDetails, arg.messageDetailsLen));
  return Result::SUCCESS;
}
