/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_QCRILDATA
#define _QTI_MARSHALLING_QCRILDATA

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_RadioAccessNetworks>(const RIL_RadioAccessNetworks &arg);
template <>
Marshal::Result Marshal::read<RIL_RadioAccessNetworks>(RIL_RadioAccessNetworks &arg) const;

template <>
Marshal::Result Marshal::write<RIL_RadioDataRequestReasons>(const RIL_RadioDataRequestReasons &arg);
template <>
Marshal::Result Marshal::read<RIL_RadioDataRequestReasons>(RIL_RadioDataRequestReasons &arg) const;
#endif
