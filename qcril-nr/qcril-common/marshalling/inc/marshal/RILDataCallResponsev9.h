/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_DATACALLRESPONSEV9
#define _QTI_MARSHALLING_DATACALLRESPONSEV9

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v9>(const RIL_Data_Call_Response_v9 &arg);
template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v9>(RIL_Data_Call_Response_v9 &arg) const;

#endif

