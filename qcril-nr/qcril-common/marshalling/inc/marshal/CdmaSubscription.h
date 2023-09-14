/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_CDMA_SUBSCRIPTION
#define _QTI_MARSHALLING_CDMA_SUBSCRIPTION

#include <Marshal.h>
#include <telephony/ril.h>

struct RIL_CDMASubInfo {
    std::string mdn;
    std::string hSid;
    std::string hNid;
    std::string min;
    std::string prl;
};

template <>
Marshal::Result Marshal::write<RIL_CDMASubInfo>(const RIL_CDMASubInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_CDMASubInfo>(RIL_CDMASubInfo &arg) const;

#endif // _QTI_MARSHALLING_CDMA_SUBSCRIPTION
