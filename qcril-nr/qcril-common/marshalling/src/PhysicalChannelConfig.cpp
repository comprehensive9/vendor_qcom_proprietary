/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/PhysicalChannelConfig.h>

template <>
Marshal::Result Marshal::write<RIL_PhysicalChannelConfig>(const RIL_PhysicalChannelConfig &arg) {
    WRITE_AND_CHECK(static_cast<int>(arg.status));
    WRITE_AND_CHECK(arg.cellBandwidthDownlink);
    WRITE_AND_CHECK(static_cast<int>(arg.rat));
    WRITE_AND_CHECK(static_cast<int>(arg.rfInfo.range));
    WRITE_AND_CHECK(arg.rfInfo.channelNumber);
    WRITE_AND_CHECK(arg.num_context_ids);
    for (int i = 0; i < arg.num_context_ids; i++) {
        WRITE_AND_CHECK(arg.contextIds[i]);
    }
    WRITE_AND_CHECK(arg.physicalCellId);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_PhysicalChannelConfig>(RIL_PhysicalChannelConfig &arg) const {
    READ_AND_CHECK(arg.status);
    READ_AND_CHECK(arg.cellBandwidthDownlink);
    READ_AND_CHECK(arg.rat);
    READ_AND_CHECK(arg.rfInfo.range);
    READ_AND_CHECK(arg.rfInfo.channelNumber);
    READ_AND_CHECK(arg.num_context_ids);
    for (int i = 0; i < arg.num_context_ids; i++) {
        READ_AND_CHECK(arg.contextIds[i]);
    }
    READ_AND_CHECK(arg.physicalCellId);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_PhysicalChannelConfig>(RIL_PhysicalChannelConfig &arg)
{
    release(arg.status);
    release(arg.cellBandwidthDownlink);
    release(arg.rat);
    release(arg.rfInfo.range);
    release(arg.rfInfo.channelNumber);
    release(arg.contextIds, arg.num_context_ids);
    release(arg.physicalCellId);
    return Result::SUCCESS;
}