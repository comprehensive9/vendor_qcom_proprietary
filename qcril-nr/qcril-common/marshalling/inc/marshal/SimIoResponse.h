/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_MARSHALLING_SIM_IO_RESP
#define _QTI_MARSHALLING_SIM_IO_RESP

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SIM_IO_Response>(const RIL_SIM_IO_Response& arg);
template <>
Marshal::Result Marshal::read<RIL_SIM_IO_Response>(RIL_SIM_IO_Response& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SIM_IO_Response>(RIL_SIM_IO_Response& arg);

#endif
