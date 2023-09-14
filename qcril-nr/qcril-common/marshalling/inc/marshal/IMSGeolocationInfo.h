/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_GEOLOCATION_INFO
#define _QTI_MARSHALLING_IMS_GEOLOCATION_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_GeolocationInfo>(const RIL_IMS_GeolocationInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_GeolocationInfo>(RIL_IMS_GeolocationInfo& arg) const;

#endif // _QTI_MARSHALLING_IMS_GEOLOCATION_INFO
