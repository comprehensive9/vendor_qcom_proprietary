/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_SERVICESTATUSINFO
#define _QTI_MARSHALLING_IMS_SERVICESTATUSINFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ServiceStatusInfo>(const RIL_IMS_ServiceStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_ServiceStatusInfo>(RIL_IMS_ServiceStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_ServiceStatusInfo>(RIL_IMS_ServiceStatusInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_IMS_AccessTechnologyStatus>(
    const RIL_IMS_AccessTechnologyStatus& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_AccessTechnologyStatus>(
    RIL_IMS_AccessTechnologyStatus& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_AccessTechnologyStatus>(
    RIL_IMS_AccessTechnologyStatus& arg);

#endif
