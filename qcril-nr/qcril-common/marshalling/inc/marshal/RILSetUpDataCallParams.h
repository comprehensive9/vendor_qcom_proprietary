/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILSetupDataCallParams
#define _QTI_MARSHALLING_RILSetupDataCallParams

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SetUpDataCallParams>(const RIL_SetUpDataCallParams &arg);
template <>
Marshal::Result Marshal::read<RIL_SetUpDataCallParams>(RIL_SetUpDataCallParams &arg) const;
template <>
Marshal::Result Marshal::release<RIL_SetUpDataCallParams>(RIL_SetUpDataCallParams &arg);

#endif
