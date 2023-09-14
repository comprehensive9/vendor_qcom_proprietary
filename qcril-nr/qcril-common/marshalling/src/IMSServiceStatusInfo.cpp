/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSServiceStatusInfo.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ServiceStatusInfo>(const RIL_IMS_ServiceStatusInfo& arg) {
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.accTechStatus);
  WRITE_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_ServiceStatusInfo>(RIL_IMS_ServiceStatusInfo& arg) const {
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.accTechStatus);
  READ_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_IMS_ServiceStatusInfo>(RIL_IMS_ServiceStatusInfo& arg) {
  release(arg.callType);
  release(arg.accTechStatus);
  release(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_AccessTechnologyStatus>(
    const RIL_IMS_AccessTechnologyStatus& arg) {
  WRITE_AND_CHECK(arg.networkMode);
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.restrictCause);
  WRITE_AND_CHECK(arg.registration);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_AccessTechnologyStatus>(
    RIL_IMS_AccessTechnologyStatus& arg) const {
  READ_AND_CHECK(arg.networkMode);
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.restrictCause);
  READ_AND_CHECK(arg.registration);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_AccessTechnologyStatus>(
    RIL_IMS_AccessTechnologyStatus& arg) {
  release(arg.networkMode);
  release(arg.status);
  release(arg.restrictCause);
  release(arg.registration);
  return Result::SUCCESS;
}
