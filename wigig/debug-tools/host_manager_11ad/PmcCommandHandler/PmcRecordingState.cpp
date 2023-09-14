/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcRecordingState.h"
#include "DebugLogger.h"
#include "Utils.h"

void PmcRecordingState::SetState(PMC_RECORDING_STATE newState)
{
    LOG_INFO << "Changing PMC recording state: " << GetState() << " to " << newState << std::endl;
    switch (newState)
    {
    case PMC_RECORDING_STATE::PMC_RECORDING_STATE_ACTIVE:
        m_startRecordingTime = Utils::GetCurrentDotNetDateTimeString();
        break;

    case PMC_RECORDING_STATE::PMC_RECORDING_STATE_PAUSED:
        m_stopRecordingTime = Utils::GetCurrentDotNetDateTimeString();
        break;

    default:
        break;
    }
    m_state = newState;
}

const char* ToString(const PMC_RECORDING_STATE &pmcRecordingState)
{
    switch (pmcRecordingState)
    {
    case PMC_RECORDING_STATE::PMC_RECORDING_STATE_IDLE:
        return "IDLE";
    case PMC_RECORDING_STATE::PMC_RECORDING_STATE_READY:
        return "READY";
    case PMC_RECORDING_STATE::PMC_RECORDING_STATE_ACTIVE:
        return "ACTIVE";
    case PMC_RECORDING_STATE::PMC_RECORDING_STATE_PAUSED:
        return "PAUSED";
    default:
        return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const PMC_RECORDING_STATE &pmcRecordingState)
{
    return os << ToString(pmcRecordingState);
}

std::ostream& operator<<(std::ostream& os, const PmcRecordingState& recordingState)
{
    return os
        << "Recording state: " << recordingState.GetState()
        << " Start Recording time: " << recordingState.GetStartRecordingTime()
        << " End recording time:" << recordingState.GetStopRecordingTime() << std::endl;
}