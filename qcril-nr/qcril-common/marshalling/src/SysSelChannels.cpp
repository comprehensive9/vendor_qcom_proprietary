/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SysSelChannels.h>

template <>
Marshal::Result Marshal::write<RIL_SysSelChannels>(const RIL_SysSelChannels& arg) {
    WRITE_AND_CHECK(arg.specifiers_length);
    for(int i=0; i < arg.specifiers_length; i++) {
        WRITE_AND_CHECK(arg.specifiers[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SysSelChannels>(RIL_SysSelChannels& arg) const {
    READ_AND_CHECK(arg.specifiers_length);
    for(int i=0; i < arg.specifiers_length; i++) {
        READ_AND_CHECK(arg.specifiers[i]);
    }
    return Result::SUCCESS;
}
