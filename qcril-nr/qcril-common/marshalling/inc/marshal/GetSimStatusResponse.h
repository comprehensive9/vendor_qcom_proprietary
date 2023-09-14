/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_MARSHALLING_GET_SIM_STATUS_RESP
#define _QTI_MARSHALLING_GET_SIM_STATUS_RESP

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CardStatus_v6>(const RIL_CardStatus_v6& arg);
template <>
Marshal::Result Marshal::read<RIL_CardStatus_v6>(RIL_CardStatus_v6& arg) const;

template <>
Marshal::Result Marshal::release<RIL_CardStatus_v6>(RIL_CardStatus_v6& arg);
template <>
Marshal::Result Marshal::write<RIL_AppStatus>(const RIL_AppStatus &arg);
template <>
Marshal::Result Marshal::read<RIL_AppStatus>(RIL_AppStatus &out) const;

template <>
Marshal::Result Marshal::release<RIL_AppStatus>(RIL_AppStatus& arg);

template <>
Marshal::Result Marshal::write<RIL_AppType>(const RIL_AppType &arg);
template <>
Marshal::Result Marshal::read<RIL_AppType>(RIL_AppType &out) const;
template <>
Marshal::Result Marshal::write<RIL_AppState>(const RIL_AppState &arg);
template <>
Marshal::Result Marshal::read<RIL_AppState>(RIL_AppState &out) const;
template <>
Marshal::Result Marshal::write<RIL_PersoSubstate>(const RIL_PersoSubstate &arg);
template <>
Marshal::Result Marshal::read<RIL_PersoSubstate>(RIL_PersoSubstate &out) const;
template <>
Marshal::Result Marshal::write<RIL_PinState>(const RIL_PinState &arg);
template <>
Marshal::Result Marshal::read<RIL_PinState>(RIL_PinState &out) const;
template <>
Marshal::Result Marshal::write<RIL_SIM_IO_v6>(const RIL_SIM_IO_v6 &arg);
template <>
Marshal::Result Marshal::read<RIL_SIM_IO_v6>(RIL_SIM_IO_v6 &out) const;

template <>
Marshal::Result Marshal::release<RIL_SIM_IO_v6>(RIL_SIM_IO_v6 &arg);

#endif
