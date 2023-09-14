/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <Marshal.h>
#include <marshal/IMSConference.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_RefreshConferenceInfo>(const RIL_IMS_RefreshConferenceInfo& arg) {
    WRITE_AND_CHECK(arg.conferenceCallState);
    WRITE_AND_CHECK(arg.confInfoUri, arg.confInfoUriLength);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_RefreshConferenceInfo>(RIL_IMS_RefreshConferenceInfo &arg) const {
    READ_AND_CHECK(arg.conferenceCallState);
    RUN_AND_CHECK(readAndAlloc(arg.confInfoUri, arg.confInfoUriLength));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_RefreshConferenceInfo>(RIL_IMS_RefreshConferenceInfo &arg) {
    release(arg.confInfoUri);
    arg.confInfoUriLength = 0;
    return Marshal::Result::SUCCESS;
}
