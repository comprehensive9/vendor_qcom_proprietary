/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILPCOData.h>

template <>
Marshal::Result Marshal::write<RIL_PCO_Data>(const RIL_PCO_Data &arg) {
    WRITE_AND_CHECK(arg.cid);
    WRITE_AND_CHECK(arg.bearer_proto);
    WRITE_AND_CHECK(arg.pco_id);
    WRITE_AND_CHECK(arg.contents_length);
    WRITE_AND_CHECK(arg.contents);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_PCO_Data>(RIL_PCO_Data &arg) const {
    READ_AND_CHECK(arg.cid);
    READ_AND_CHECK(arg.bearer_proto);
    READ_AND_CHECK(arg.pco_id);
    READ_AND_CHECK(arg.contents_length);
    READ_AND_CHECK(arg.contents);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_PCO_Data>(RIL_PCO_Data &arg)
{
    release(arg.cid);
    release(arg.bearer_proto);
    release(arg.pco_id);
    release(arg.contents, arg.contents_length);
    return Result::SUCCESS;
}