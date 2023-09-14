/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSAutoCallRejectionInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_AutoCallRejectionInfo>(const RIL_IMS_AutoCallRejectionInfo& arg) {
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.cause);
  WRITE_AND_CHECK(arg.sipErrorCode);
  WRITE_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_AutoCallRejectionInfo>(RIL_IMS_AutoCallRejectionInfo& arg) const {
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.cause);
  READ_AND_CHECK(arg.sipErrorCode);
  READ_AND_CHECK(arg.number);
  return Result::SUCCESS;
}
