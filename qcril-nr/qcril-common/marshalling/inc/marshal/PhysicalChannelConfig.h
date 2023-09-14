/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_PhysicalChannelConfig>(const RIL_PhysicalChannelConfig &arg);
template <>
Marshal::Result Marshal::read<RIL_PhysicalChannelConfig>(RIL_PhysicalChannelConfig &arg) const;
template <>
Marshal::Result Marshal::release<RIL_PhysicalChannelConfig>(RIL_PhysicalChannelConfig &arg);