/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILPCODATAINFO
#define _QTI_MARSHALLING_RILPCODATAINFO

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_PCO_Data>(const RIL_PCO_Data &arg);
template <>
Marshal::Result Marshal::read<RIL_PCO_Data>(RIL_PCO_Data &arg) const;
template <>
Marshal::Result Marshal::release<RIL_PCO_Data>(RIL_PCO_Data &arg);
#endif
