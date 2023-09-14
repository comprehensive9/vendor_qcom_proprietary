/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_MARSHALLING_SIM_IO_REQ
#define _QTI_MARSHALLING_SIM_IO_REQ

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SIM_IO_v6>(const RIL_SIM_IO_v6& arg);
template <>
Marshal::Result Marshal::read<RIL_SIM_IO_v6>(RIL_SIM_IO_v6& arg) const;

template <>
Marshal::Result Marshal::release<RIL_SIM_IO_v6>(RIL_SIM_IO_v6& arg);

template <>
Marshal::Result Marshal::write<RIL_AdnRecordInfo>(const RIL_AdnRecordInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_AdnRecordInfo>(RIL_AdnRecordInfo &arg) const;

template <>
Marshal::Result Marshal::write<RIL_AdnCountInfo>(const RIL_AdnCountInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_AdnCountInfo>(RIL_AdnCountInfo &arg) const;

template <>
Marshal::Result Marshal::write<RIL_SimRefreshResponse_v7>(const RIL_SimRefreshResponse_v7 &arg);
template <>
Marshal::Result Marshal::read<RIL_SimRefreshResponse_v7>(RIL_SimRefreshResponse_v7 &arg) const;
template <>
Marshal::Result Marshal::release<RIL_SimRefreshResponse_v7>(RIL_SimRefreshResponse_v7 &arg);
#endif
