/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_NETWORK_SCAN
#define _QTI_MARSHALLING_NETWORK_SCAN

#include <Marshal.h>
#include <telephony/ril.h>
#include <marshal/RadioAccessSpecifier.h>

template <>
Marshal::Result Marshal::write<RIL_NetworkScanRequest>(
    const RIL_NetworkScanRequest& arg);
template <>
Marshal::Result Marshal::read<RIL_NetworkScanRequest>(
    RIL_NetworkScanRequest& arg) const;

template <>
Marshal::Result Marshal::write<RIL_NetworkScanResult>(
    const RIL_NetworkScanResult& arg);
template <>
Marshal::Result Marshal::read<RIL_NetworkScanResult>(
    RIL_NetworkScanResult& arg) const;
template <>
Marshal::Result Marshal::release<RIL_NetworkScanResult>(RIL_NetworkScanResult& arg);

#endif
