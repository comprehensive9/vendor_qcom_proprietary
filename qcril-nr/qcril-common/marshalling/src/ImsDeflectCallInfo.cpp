/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsDeflectCallInfo.h>
#include <telephony/ril.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_DeflectCallInfo>(const RIL_IMS_DeflectCallInfo& arg) {
    WRITE_AND_CHECK(arg.connIndex);
    WRITE_AND_CHECK(arg.number);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_DeflectCallInfo>(RIL_IMS_DeflectCallInfo& arg) const {
    READ_AND_CHECK(arg.connIndex);
    READ_AND_CHECK(arg.number);
    return Result::SUCCESS;
}

