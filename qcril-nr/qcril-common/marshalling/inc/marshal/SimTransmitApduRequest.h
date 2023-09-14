/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_MARSHALLING_SIM_APDU_REQ
#define _QTI_MARSHALLING_SIM_APDU_REQ

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SIM_APDU>(const RIL_SIM_APDU& arg);
template <>
Marshal::Result Marshal::read<RIL_SIM_APDU>(RIL_SIM_APDU& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SIM_APDU>(RIL_SIM_APDU& arg);

#endif
