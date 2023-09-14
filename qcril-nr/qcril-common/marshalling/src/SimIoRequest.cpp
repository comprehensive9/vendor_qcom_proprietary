/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>
#include <marshal/SimIoRequest.h>

template <>
Marshal::Result Marshal::write<RIL_SIM_IO_v6>(const RIL_SIM_IO_v6 &arg) {
    WRITE_AND_CHECK(arg.command);
    WRITE_AND_CHECK(arg.fileid);
    WRITE_AND_CHECK(arg.path);
    WRITE_AND_CHECK(arg.p1);
    WRITE_AND_CHECK(arg.p2);
    WRITE_AND_CHECK(arg.p3);
    WRITE_AND_CHECK(arg.data);
    WRITE_AND_CHECK(arg.pin2);
    WRITE_AND_CHECK(arg.aidPtr);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SIM_IO_v6>(RIL_SIM_IO_v6 &out) const {
    READ_AND_CHECK(out.command);
    READ_AND_CHECK(out.fileid);
    READ_AND_CHECK(out.path);
    READ_AND_CHECK(out.p1);
    READ_AND_CHECK(out.p2);
    READ_AND_CHECK(out.p3);
    READ_AND_CHECK(out.data);
    READ_AND_CHECK(out.pin2);
    READ_AND_CHECK(out.aidPtr);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_AdnRecordInfo>(const RIL_AdnRecordInfo &arg) {
    WRITE_AND_CHECK(arg.name);
    WRITE_AND_CHECK(arg.number);
    WRITE_AND_CHECK(arg.email, arg.email_elements);
    WRITE_AND_CHECK(arg.ad_number, arg.anr_elements);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AdnRecordInfo>(RIL_AdnRecordInfo &arg) const {
    READ_AND_CHECK(arg.name);
    READ_AND_CHECK(arg.number);
    READ_AND_CHECK(arg.email, arg.email_elements);
    READ_AND_CHECK(arg.ad_number, arg.anr_elements);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_AdnCountInfo>(const RIL_AdnCountInfo &arg) {
      WRITE_AND_CHECK(arg.max_adn_num);
      WRITE_AND_CHECK(arg.valid_adn_num);
      WRITE_AND_CHECK(arg.max_email_num);
      WRITE_AND_CHECK(arg.valid_email_num);
      WRITE_AND_CHECK(arg.max_ad_num);
      WRITE_AND_CHECK(arg.valid_ad_num);
      WRITE_AND_CHECK(arg.max_name_len);
      WRITE_AND_CHECK(arg.max_number_len);
      WRITE_AND_CHECK(arg.max_email_len);
      WRITE_AND_CHECK(arg.max_anr_len);
      return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AdnCountInfo>(RIL_AdnCountInfo &arg) const {
      READ_AND_CHECK(arg.max_adn_num);
      READ_AND_CHECK(arg.valid_adn_num);
      READ_AND_CHECK(arg.max_email_num);
      READ_AND_CHECK(arg.valid_email_num);
      READ_AND_CHECK(arg.max_ad_num);
      READ_AND_CHECK(arg.valid_ad_num);
      READ_AND_CHECK(arg.max_name_len);
      READ_AND_CHECK(arg.max_number_len);
      READ_AND_CHECK(arg.max_email_len);
      READ_AND_CHECK(arg.max_anr_len);
      return Result::SUCCESS;
}


template <>
Marshal::Result Marshal::write<RIL_AdnRecords>(const RIL_AdnRecords &arg) {
    WRITE_AND_CHECK(arg.adn_record_info, arg.record_elements);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AdnRecords>(RIL_AdnRecords &arg) const {
    READ_AND_CHECK(arg.adn_record_info, arg.record_elements);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SimRefreshResponse_v7>(const RIL_SimRefreshResponse_v7 &arg) {
    WRITE_AND_CHECK(arg.result);
    WRITE_AND_CHECK(arg.ef_id);
    WRITE_AND_CHECK(arg.aid);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_SimRefreshResponse_v7>(RIL_SimRefreshResponse_v7 &arg) {
    release(arg.aid);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SimRefreshResponse_v7>(RIL_SimRefreshResponse_v7 &arg) const {
    READ_AND_CHECK(arg.result);
    READ_AND_CHECK(arg.ef_id);
    READ_AND_CHECK(arg.aid);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_SIM_IO_v6>(RIL_SIM_IO_v6 &arg) {
    release(arg.path);
    release(arg.data);
    release(arg.pin2);
    release(arg.aidPtr);
    return Result::SUCCESS;
}
