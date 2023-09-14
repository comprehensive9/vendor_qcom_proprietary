/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSRegistration.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_Registration>(const RIL_IMS_Registration& arg) {
  WRITE_AND_CHECK(arg.state);
  WRITE_AND_CHECK(arg.errorCode);
  WRITE_AND_CHECK(arg.errorMessage);
  WRITE_AND_CHECK(arg.radioTech);
  WRITE_AND_CHECK(arg.pAssociatedUris);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_Registration>(RIL_IMS_Registration& arg) const {
  READ_AND_CHECK(arg.state);
  READ_AND_CHECK(arg.errorCode);
  READ_AND_CHECK(arg.errorMessage);
  READ_AND_CHECK(arg.radioTech);
  READ_AND_CHECK(arg.pAssociatedUris);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_Registration>(RIL_IMS_Registration& arg) {
  release(arg.errorMessage);
  release(arg.pAssociatedUris);
  return Result::SUCCESS;
}

