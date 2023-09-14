/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsClirInfo.h>
#include <telephony/ril.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_ClirInfo>(const RIL_IMS_ClirInfo& arg) {
    WRITE_AND_CHECK(arg.action);
    WRITE_AND_CHECK(arg.presentation);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_ClirInfo>(RIL_IMS_ClirInfo& arg) const {
    READ_AND_CHECK(arg.action);
    READ_AND_CHECK(arg.presentation);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SetClirInfo>(const RIL_IMS_SetClirInfo& arg) {
    WRITE_AND_CHECK(arg.action);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SetClirInfo>(RIL_IMS_SetClirInfo& arg) const {
    READ_AND_CHECK(arg.action);
    return Result::SUCCESS;
}
