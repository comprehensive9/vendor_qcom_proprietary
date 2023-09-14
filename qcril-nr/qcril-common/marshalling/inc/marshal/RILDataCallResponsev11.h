/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_DATACALLRESPONSEV11
#define _QTI_MARSHALLING_DATACALLRESPONSEV11

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v11>(const RIL_Data_Call_Response_v11 &arg);
template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v11>(RIL_Data_Call_Response_v11 &arg) const;
template <>
Marshal::Result Marshal::release<RIL_Data_Call_Response_v11>(RIL_Data_Call_Response_v11 &arg);
#endif

