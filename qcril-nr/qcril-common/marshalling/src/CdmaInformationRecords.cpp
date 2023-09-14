/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CdmaInformationRecords.h>
#include <marshal/CdmaSignalInfoRecord.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_NumberInfoRecord>(const RIL_CDMA_NumberInfoRecord& arg) {
  WRITE_AND_CHECK(arg.len);
  WRITE_AND_CHECK(arg.buf);
  WRITE_AND_CHECK(arg.number_type);
  WRITE_AND_CHECK(arg.number_plan);
  WRITE_AND_CHECK(arg.pi);
  WRITE_AND_CHECK(arg.si);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_NumberInfoRecord>(RIL_CDMA_NumberInfoRecord& arg) const {
  READ_AND_CHECK(arg.len);
  READ_AND_CHECK(arg.buf);
  READ_AND_CHECK(arg.number_type);
  READ_AND_CHECK(arg.number_plan);
  READ_AND_CHECK(arg.pi);
  READ_AND_CHECK(arg.si);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_RedirectingNumberInfoRecord>(
    const RIL_CDMA_RedirectingNumberInfoRecord& arg) {
  WRITE_AND_CHECK(arg.redirectingNumber);
  WRITE_AND_CHECK(arg.redirectingReason);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_RedirectingNumberInfoRecord>(
    RIL_CDMA_RedirectingNumberInfoRecord& arg) const {
  READ_AND_CHECK(arg.redirectingNumber);
  READ_AND_CHECK(arg.redirectingReason);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_LineControlInfoRecord>(
    const RIL_CDMA_LineControlInfoRecord& arg) {
  WRITE_AND_CHECK(arg.lineCtrlPolarityIncluded);
  WRITE_AND_CHECK(arg.lineCtrlToggle);
  WRITE_AND_CHECK(arg.lineCtrlReverse);
  WRITE_AND_CHECK(arg.lineCtrlPowerDenial);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_LineControlInfoRecord>(
    RIL_CDMA_LineControlInfoRecord& arg) const {
  READ_AND_CHECK(arg.lineCtrlPolarityIncluded);
  READ_AND_CHECK(arg.lineCtrlToggle);
  READ_AND_CHECK(arg.lineCtrlReverse);
  READ_AND_CHECK(arg.lineCtrlPowerDenial);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_T53_CLIRInfoRecord>(const RIL_CDMA_T53_CLIRInfoRecord& arg) {
  WRITE_AND_CHECK(arg.cause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_T53_CLIRInfoRecord>(RIL_CDMA_T53_CLIRInfoRecord& arg) const {
  READ_AND_CHECK(arg.cause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_T53_AudioControlInfoRecord>(
    const RIL_CDMA_T53_AudioControlInfoRecord& arg) {
  WRITE_AND_CHECK(arg.upLink);
  WRITE_AND_CHECK(arg.downLink);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_T53_AudioControlInfoRecord>(
    RIL_CDMA_T53_AudioControlInfoRecord& arg) const {
  READ_AND_CHECK(arg.upLink);
  READ_AND_CHECK(arg.downLink);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_DisplayInfoRecord>(const RIL_CDMA_DisplayInfoRecord& arg) {
  WRITE_AND_CHECK(arg.alpha_len);
  WRITE_AND_CHECK(arg.alpha_buf);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_DisplayInfoRecord>(RIL_CDMA_DisplayInfoRecord& arg) const {
  READ_AND_CHECK(arg.alpha_len);
  READ_AND_CHECK(arg.alpha_buf);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_InformationRecord>(const RIL_CDMA_InformationRecord& arg) {
  WRITE_AND_CHECK(arg.name);
  switch (arg.name) {
    case RIL_CDMA_DISPLAY_INFO_REC:
    case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
      WRITE_AND_CHECK(arg.rec.display);
      break;
    case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
    case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
    case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
      WRITE_AND_CHECK(arg.rec.number);
      break;
    case RIL_CDMA_SIGNAL_INFO_REC:
      WRITE_AND_CHECK(arg.rec.signal);
      break;
    case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
      WRITE_AND_CHECK(arg.rec.redir);
      break;
    case RIL_CDMA_LINE_CONTROL_INFO_REC:
      WRITE_AND_CHECK(arg.rec.lineCtrl);
      break;
    case RIL_CDMA_T53_CLIR_INFO_REC:
      WRITE_AND_CHECK(arg.rec.clir);
      break;
    case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
      WRITE_AND_CHECK(arg.rec.audioCtrl);
      break;
    case RIL_CDMA_T53_RELEASE_INFO_REC:
    default:
      break;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_InformationRecord>(RIL_CDMA_InformationRecord& arg) const {
  READ_AND_CHECK(arg.name);
  switch (arg.name) {
    case RIL_CDMA_DISPLAY_INFO_REC:
    case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
      READ_AND_CHECK(arg.rec.display);
      break;
    case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
    case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
    case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
      READ_AND_CHECK(arg.rec.number);
      break;
    case RIL_CDMA_SIGNAL_INFO_REC:
      READ_AND_CHECK(arg.rec.signal);
      break;
    case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
      READ_AND_CHECK(arg.rec.redir);
      break;
    case RIL_CDMA_LINE_CONTROL_INFO_REC:
      READ_AND_CHECK(arg.rec.lineCtrl);
      break;
    case RIL_CDMA_T53_CLIR_INFO_REC:
      READ_AND_CHECK(arg.rec.clir);
      break;
    case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
      READ_AND_CHECK(arg.rec.audioCtrl);
      break;
    case RIL_CDMA_T53_RELEASE_INFO_REC:
    default:
      break;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_InformationRecords>(const RIL_CDMA_InformationRecords& arg) {
  char numRecords =
      (arg.numberOfInfoRecs > RIL_CDMA_MAX_NUMBER_OF_INFO_RECS ? RIL_CDMA_MAX_NUMBER_OF_INFO_RECS
                                                               : arg.numberOfInfoRecs);
  WRITE_AND_CHECK(numRecords);
  for (int i = 0; i < numRecords; i++) {
    WRITE_AND_CHECK(arg.infoRec[i]);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_InformationRecords>(RIL_CDMA_InformationRecords& arg) const {
  READ_AND_CHECK(arg.numberOfInfoRecs);
  arg.numberOfInfoRecs =
      (arg.numberOfInfoRecs > RIL_CDMA_MAX_NUMBER_OF_INFO_RECS ? RIL_CDMA_MAX_NUMBER_OF_INFO_RECS
                                                               : arg.numberOfInfoRecs);
  for (int i = 0; i < arg.numberOfInfoRecs; i++) {
    READ_AND_CHECK(arg.infoRec[i]);
  }
  return Result::SUCCESS;
}
