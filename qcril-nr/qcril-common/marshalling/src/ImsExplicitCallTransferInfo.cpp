/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsExplicitCallTransferInfo.h>
#include <telephony/ril.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_ExplicitCallTransfer>(const RIL_IMS_ExplicitCallTransfer& arg) {
    WRITE_AND_CHECK(arg.callId);
    WRITE_AND_CHECK(arg.ectType);
    WRITE_AND_CHECK(arg.targetAddress);
    WRITE_AND_CHECK(arg.targetCallId);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_ExplicitCallTransfer>(RIL_IMS_ExplicitCallTransfer& arg) const {
    READ_AND_CHECK(arg.callId);
    READ_AND_CHECK(arg.ectType);
    READ_AND_CHECK(arg.targetAddress);
    READ_AND_CHECK(arg.targetCallId);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_ExplicitCallTransfer>(RIL_IMS_ExplicitCallTransfer& arg) {
    release(arg.targetAddress);
    return Result::SUCCESS;
}

