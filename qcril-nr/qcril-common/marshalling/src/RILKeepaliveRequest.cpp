/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILKeepaliveRequest.h>

template <>
Marshal::Result Marshal::write<RIL_KeepaliveRequest>(const RIL_KeepaliveRequest &arg) {
    WRITE_AND_CHECK(static_cast<uint8_t>(arg.type));
    WRITE_AND_CHECK(arg.sourceAddress);
    WRITE_AND_CHECK(arg.sourcePort);
    WRITE_AND_CHECK(arg.destinationAddress);
    WRITE_AND_CHECK(arg.destinationPort);
    WRITE_AND_CHECK(arg.maxKeepaliveIntervalMillis);
    WRITE_AND_CHECK(arg.cid);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_KeepaliveRequest>(RIL_KeepaliveRequest &arg) const {
    uint8_t val;
    READ_AND_CHECK(val);
    arg.type = static_cast<RIL_KeepaliveType>(val);
    READ_AND_CHECK(arg.sourceAddress);
    READ_AND_CHECK(arg.sourcePort);
    READ_AND_CHECK(arg.destinationAddress);
    READ_AND_CHECK(arg.destinationPort);
    READ_AND_CHECK(arg.maxKeepaliveIntervalMillis);
    READ_AND_CHECK(arg.cid);
    return Result::SUCCESS;
}