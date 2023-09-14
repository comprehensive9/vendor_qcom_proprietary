/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsSendUiTtyModeInfo.h>
#include <telephony/ril.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_TtyNotifyInfo>(const RIL_IMS_TtyNotifyInfo& arg) {
    WRITE_AND_CHECK(arg.mode);
    WRITE_AND_CHECK(arg.userData);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_TtyNotifyInfo>(RIL_IMS_TtyNotifyInfo& arg) const {
    READ_AND_CHECK(arg.mode);
    READ_AND_CHECK(arg.userData);
    return Result::SUCCESS;
}

