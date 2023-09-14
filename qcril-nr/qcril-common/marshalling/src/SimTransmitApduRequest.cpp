/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>
#include <marshal/SimTransmitApduRequest.h>

template <>
Marshal::Result Marshal::write<RIL_SIM_APDU>(const RIL_SIM_APDU& arg) {

  WRITE_AND_CHECK(arg.sessionid);
  WRITE_AND_CHECK(arg.cla);
  WRITE_AND_CHECK(arg.instruction);
  WRITE_AND_CHECK(arg.p1);
  WRITE_AND_CHECK(arg.p2);
  WRITE_AND_CHECK(arg.p3);
  WRITE_AND_CHECK(arg.data);
  
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SIM_APDU>(RIL_SIM_APDU& arg) const {

  READ_AND_CHECK(arg.sessionid);
  READ_AND_CHECK(arg.cla);
  READ_AND_CHECK(arg.instruction);
  READ_AND_CHECK(arg.p1);
  READ_AND_CHECK(arg.p2);
  READ_AND_CHECK(arg.p3);
  READ_AND_CHECK(arg.data);
  
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SIM_APDU>(RIL_SIM_APDU& arg) {
  release(arg.data);
  return Result::SUCCESS;
}
