/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/StkCcUnsolSsResponse.h>
#include <marshal/CallForwardInfo.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CfData>(const RIL_CfData& arg) {
  WRITE_AND_CHECK(arg.numValidIndexes);
  for(size_t i = 0; i < arg.numValidIndexes && i < NUM_SERVICE_CLASSES; i++) {
    WRITE_AND_CHECK(arg.cfInfo[i]);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CfData>(RIL_CfData& arg) const {
  READ_AND_CHECK(arg.numValidIndexes);
  for(size_t i = 0; i < arg.numValidIndexes && i < NUM_SERVICE_CLASSES; i++) {
    READ_AND_CHECK(arg.cfInfo[i]);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CfData>(RIL_CfData& arg) {
  for(size_t i = 0; i < arg.numValidIndexes && i < NUM_SERVICE_CLASSES; i++) {
    release(arg.cfInfo[i]);
  }
  return Result::SUCCESS;
}


template <>
Marshal::Result Marshal::write<RIL_StkCcUnsolSsResponse>(const RIL_StkCcUnsolSsResponse& arg) {
  WRITE_AND_CHECK(arg.serviceType);
  WRITE_AND_CHECK(arg.requestType);
  WRITE_AND_CHECK(arg.teleserviceType);
  WRITE_AND_CHECK(arg.serviceClass);
  WRITE_AND_CHECK(arg.result);
  if ((arg.requestType == SS_INTERROGATION) &&
      (arg.serviceType == SS_CFU ||
       arg.serviceType == SS_CF_BUSY ||
       arg.serviceType == SS_CF_NO_REPLY ||
       arg.serviceType == SS_CF_NOT_REACHABLE ||
       arg.serviceType == SS_CF_ALL ||
       arg.serviceType == SS_CF_ALL_CONDITIONAL)) {
    // CF
    WRITE_AND_CHECK(arg.cfData);
  } else {
    WRITE_AND_CHECK(arg.ssInfo);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_StkCcUnsolSsResponse>(RIL_StkCcUnsolSsResponse& arg) const {
  READ_AND_CHECK(arg.serviceType);
  READ_AND_CHECK(arg.requestType);
  READ_AND_CHECK(arg.teleserviceType);
  READ_AND_CHECK(arg.serviceClass);
  READ_AND_CHECK(arg.result);
  if ((arg.requestType == SS_INTERROGATION) &&
      (arg.serviceType == SS_CFU ||
       arg.serviceType == SS_CF_BUSY ||
       arg.serviceType == SS_CF_NO_REPLY ||
       arg.serviceType == SS_CF_NOT_REACHABLE ||
       arg.serviceType == SS_CF_ALL ||
       arg.serviceType == SS_CF_ALL_CONDITIONAL)) {
    // CF
    READ_AND_CHECK(arg.cfData);
  } else {
    READ_AND_CHECK(arg.ssInfo);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_StkCcUnsolSsResponse>(RIL_StkCcUnsolSsResponse& arg) {
  if ((arg.requestType == SS_INTERROGATION) &&
      (arg.serviceType == SS_CFU ||
       arg.serviceType == SS_CF_BUSY ||
       arg.serviceType == SS_CF_NO_REPLY ||
       arg.serviceType == SS_CF_NOT_REACHABLE ||
       arg.serviceType == SS_CF_ALL ||
       arg.serviceType == SS_CF_ALL_CONDITIONAL)) {
    // CF
    release(arg.cfData);
  }
  return Result::SUCCESS;
}
