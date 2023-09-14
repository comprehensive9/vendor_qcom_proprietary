/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcContext.h"
#include "LegacyPmcSequence.h"
#include "ContinuousPmcSequence.h"
#include "Device.h"

#include <memory>

PmcContext::PmcContext(const Device& device, bool isContinuousPmc)
    : m_registerAccessor(device)
    , m_upSequence(isContinuousPmc ? static_cast<const PmcSequenceBase*>(new ContinuousPmcSequence(device)) : new LegacyPmcSequence(device))
    , m_filterSequence(device)
{}

PmcContext::~PmcContext() = default;
