/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CarrierInfoForImsiEncryption>(const RIL_CarrierInfoForImsiEncryption &arg);
template <>
Marshal::Result Marshal::read<RIL_CarrierInfoForImsiEncryption>(RIL_CarrierInfoForImsiEncryption &arg) const;
template <>
Marshal::Result Marshal::release<RIL_CarrierInfoForImsiEncryption>(RIL_CarrierInfoForImsiEncryption &arg);
