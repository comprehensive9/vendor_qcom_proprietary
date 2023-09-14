/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CdmaSignalInfoRecord.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_SignalInfoRecord>(const RIL_CDMA_SignalInfoRecord& arg) {
  write(arg.isPresent);
  write(arg.signalType);
  write(arg.alertPitch);
  write(arg.signal);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_SignalInfoRecord>(RIL_CDMA_SignalInfoRecord& arg) const {
  read(arg.isPresent);
  read(arg.signalType);
  read(arg.alertPitch);
  read(arg.signal);
  return Result::SUCCESS;
}
