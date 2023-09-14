/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>
#include <marshal/SimOpenChannelRequest.h>

template <>
Marshal::Result Marshal::write<RIL_OpenChannelParams>(const RIL_OpenChannelParams& arg) {

  WRITE_AND_CHECK(arg.aidPtr);
  WRITE_AND_CHECK(arg.p2);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_OpenChannelParams>(RIL_OpenChannelParams& arg) const {

  READ_AND_CHECK(arg.aidPtr);
  READ_AND_CHECK(arg.p2);
 
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_OpenChannelParams>(RIL_OpenChannelParams& arg) {
  release(arg.aidPtr);
  return Result::SUCCESS;
}
