/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "PmcRecordingState.h"
#include "PmcDeviceConfigurations.h"
#include "PmcRegistersAccessor.h"
#include "PmcFilterSequence.h"
#include "Utils.h"

#include <memory>

class Device;
class PmcSequenceBase;

//This class saves PMC context for a single device - its recording state (if a recording is active or not) and the PMC configurations
class PmcContext : public Immobile
{
public:
    PmcContext(const Device& device, bool isContinuousPmc);
    ~PmcContext();

    PmcRecordingState& GetDeviceRecordingState() { return m_recordingState; }
    PmcDeviceConfigurations& GetDeviceConfigurations() { return m_deviceConfigurations; }

    const PmcRegistersAccessor& GetRegisterAccessor() const { return m_registerAccessor; }
    const PmcSequenceBase& GetSequence() const { return *m_upSequence; }
    const PmcFilterSequence& GetFilterSequence() const { return m_filterSequence; }

private:
    PmcRecordingState m_recordingState;
    PmcDeviceConfigurations m_deviceConfigurations;

    const PmcRegistersAccessor m_registerAccessor;
    const std::unique_ptr<const PmcSequenceBase> m_upSequence;
    const PmcFilterSequence m_filterSequence;
};
