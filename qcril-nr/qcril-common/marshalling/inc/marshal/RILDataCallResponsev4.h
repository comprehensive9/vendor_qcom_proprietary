/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_DATACALLRESPONSEV4
#define _QTI_MARSHALLING_DATACALLRESPONSEV4

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v4>(const RIL_Data_Call_Response_v4 &arg);
template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v4>(RIL_Data_Call_Response_v4 &arg) const;

#endif
