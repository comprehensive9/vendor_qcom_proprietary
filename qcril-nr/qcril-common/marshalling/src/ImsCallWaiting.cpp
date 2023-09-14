/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsCallWaiting.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallWaitingSettings>(const RIL_IMS_CallWaitingSettings& arg) {
    WRITE_AND_CHECK(arg.enabled);
    WRITE_AND_CHECK(arg.serviceClass);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallWaitingSettings>(RIL_IMS_CallWaitingSettings& arg) const {
    READ_AND_CHECK(arg.enabled);
    READ_AND_CHECK(arg.serviceClass);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_QueryCallWaitingResponse>(const RIL_IMS_QueryCallWaitingResponse& arg) {
    WRITE_AND_CHECK(arg.callWaitingSettings);
    WRITE_AND_CHECK(arg.sipError);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_QueryCallWaitingResponse>(RIL_IMS_QueryCallWaitingResponse& arg) const {
    READ_AND_CHECK(arg.callWaitingSettings);
    READ_AND_CHECK(arg.sipError);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_QueryCallWaitingResponse>(RIL_IMS_QueryCallWaitingResponse& arg) {
    release(arg.sipError);
    return Marshal::Result::SUCCESS;
}
