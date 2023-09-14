/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "PmcSequenceBase.h"
#include "OperationStatus.h"
#include <vector>
#include <string>

class Device;
struct PmcRingSpec;

class ContinuousPmcSequence : public PmcSequenceBase
{
public:
    explicit ContinuousPmcSequence(const Device& device);

private:
    OperationStatus InitImpl() const override;
    OperationStatus ShutdownImpl() const override;
    OperationStatus PausePmcRecordingImpl() const override;
    OperationStatus ValidateRecordingStatusImpl() const override;
    OperationStatus ActivateRecordingImpl() const override;
    OperationStatus SetDefaultConfigurationImpl() const override;
    OperationStatus DeactivateRecordingImpl() const override;
    OperationStatus FinalizeRecordingImpl(bool onTargetClient, std::vector<uint8_t>& pmcData, std::string& pmcDataPath) const override;
    const PmcRingSpec* InitializePmcRingSpecImpl() const override;
};
