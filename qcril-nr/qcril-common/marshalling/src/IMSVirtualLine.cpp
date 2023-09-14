/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSVirtualLine.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_QueryVirtualLineInfoResponse>(
        const RIL_IMS_QueryVirtualLineInfoResponse& arg) {
    WRITE_AND_CHECK(arg.msisdn);
    WRITE_AND_CHECK(arg.virtualLines, arg.numLines);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_QueryVirtualLineInfoResponse>(
        RIL_IMS_QueryVirtualLineInfoResponse& arg) const {
    READ_AND_CHECK(arg.msisdn);
    RUN_AND_CHECK(readAndAlloc(arg.virtualLines, arg.numLines));

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_QueryVirtualLineInfoResponse>(
        RIL_IMS_QueryVirtualLineInfoResponse& arg) {
    release(arg.msisdn);
    release(arg.virtualLines, arg.numLines);
    arg.numLines = 0;
    return Result::SUCCESS;
}
