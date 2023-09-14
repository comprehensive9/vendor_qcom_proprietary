/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_DIAL
#define _QTI_MARSHALLING_DIAL

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_Dial>(const RIL_Dial &arg);
template <>
Marshal::Result Marshal::read<RIL_Dial>(RIL_Dial &arg) const;
template <>
Marshal::Result Marshal::release<RIL_Dial>(RIL_Dial &arg);

#endif
