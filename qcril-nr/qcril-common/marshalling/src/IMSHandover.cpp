/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSHandover.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_HoExtra>(const RIL_IMS_HoExtra &arg) {
    WRITE_AND_CHECK(arg.type);
    WRITE_AND_CHECK(arg.extraInfo, arg.extraInfoLen);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_HoExtra>(RIL_IMS_HoExtra &arg) const {
    READ_AND_CHECK(arg.type);
    RUN_AND_CHECK(readAndAlloc(arg.extraInfo, arg.extraInfoLen));
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_HoExtra>(RIL_IMS_HoExtra &arg) {
    release(arg.extraInfo);
    arg.extraInfoLen = 0;
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_HandoverInfo>(const RIL_IMS_HandoverInfo &arg) {
    WRITE_AND_CHECK(arg.type);
    WRITE_AND_CHECK(arg.srcTech);
    WRITE_AND_CHECK(arg.targetTech);
    WRITE_AND_CHECK(arg.hoExtra, 1);
    WRITE_AND_CHECK(arg.errorCode);
    WRITE_AND_CHECK(arg.errorMessage);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_HandoverInfo>(RIL_IMS_HandoverInfo &arg) const {
    READ_AND_CHECK(arg.type);
    READ_AND_CHECK(arg.srcTech);
    READ_AND_CHECK(arg.targetTech);
    size_t sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.hoExtra, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    READ_AND_CHECK(arg.errorCode);
    READ_AND_CHECK(arg.errorMessage);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_HandoverInfo>(RIL_IMS_HandoverInfo &arg) {
    release(arg.hoExtra, 1);
    release(arg.errorCode);
    release(arg.errorMessage);
    return Result::SUCCESS;
}

