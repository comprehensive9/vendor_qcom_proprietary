/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSHangup.h>
#include <telephony/ril.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_Hangup>(const RIL_IMS_Hangup& arg) {
    WRITE_AND_CHECK(arg.connIndex);
    WRITE_AND_CHECK(static_cast<unsigned char>(arg.hasMultiParty));
    if (arg.hasMultiParty)
        WRITE_AND_CHECK(static_cast<unsigned char>(arg.multiParty));
    WRITE_AND_CHECK(arg.connUri);
    WRITE_AND_CHECK(arg.conf_id);
    WRITE_AND_CHECK(static_cast<unsigned char>(arg.hasFailCauseResponse));
    if (arg.hasFailCauseResponse)
        WRITE_AND_CHECK(arg.failCauseResponse);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_Hangup>(RIL_IMS_Hangup& arg) const {
    READ_AND_CHECK(arg.connIndex);

    unsigned char rdVal;
    READ_AND_CHECK(rdVal);
    arg.hasMultiParty = !!rdVal;
    if (arg.hasMultiParty) {
        READ_AND_CHECK(rdVal);
        arg.multiParty = !!rdVal;
    }

    READ_AND_CHECK(arg.connUri);
    READ_AND_CHECK(arg.conf_id);

    READ_AND_CHECK(rdVal);
    arg.hasFailCauseResponse = !!rdVal;
    if (arg.hasFailCauseResponse) {
        READ_AND_CHECK(arg.failCauseResponse);
    }

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_Hangup>(RIL_IMS_Hangup& arg) {
    release(arg.connUri);
    if (arg.hasFailCauseResponse) {
        release(arg.failCauseResponse);
    }
    return Result::SUCCESS;
}

