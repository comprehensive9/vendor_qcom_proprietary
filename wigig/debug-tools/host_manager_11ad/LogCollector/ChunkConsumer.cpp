/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "ChunkConsumer.h"
#include "DebugLogger.h"
#include <string>

const std::string ChunkConsumer::BufferOverrunMessage = "\nhost_manager_11ad message: Found buffer overrun - missed DWORDS: ";
const std::string ChunkConsumer::DeviceRestartedMessage = "\nhost_manager_11ad message: Device was restarted \n";
const std::string ChunkConsumer::CorruptedEntryMessage = "\nhost_manager_11ad message: Got corrupted entry, signature: ";
const std::string ChunkConsumer::InconsistentWptrMessage = "\nhost_manager_11ad message: Inconsistent wptr position in the middle of a message, diff (DWORDS): ";
const std::string ChunkConsumer::PmcLogEventTooShortMessage = "\nhost_manager_11ad message: PMC log event too short, expected at least TSF and signature, but received DWORDS: ";
const std::string ChunkConsumer::InconsistentNumParamsMessage = "\nhost_manager_11ad message: Number parameters exceeds log message length, diff (DWORDS): ";
const std::string ChunkConsumer::DeviceRemovedMessage = "\nhost_manager_11ad message: Device was removed! \n";
const std::string ChunkConsumer::DeviceDiscoveredMessage = "\nhost_manager_11ad message: Device was discovered! \n";

ChunkConsumer::ChunkConsumer(const std::string &deviceName, CpuType cpuType, bool tsfAvailable, LoggingType loggingType)
    : m_deviceName(deviceName)
    , m_tracerCpuType(cpuType)
    , m_tsfAvailable(tsfAvailable)
    , m_loggingType(loggingType)
    , m_stateProvider(deviceName, cpuType, loggingType)
    , m_debugMode(false)
{}

OperationStatus ChunkConsumer::Stop()
{
    if (GetState() == LoggingState::DISABLED)
    {
        return OperationStatus();
    }
    LOG_VERBOSE << "Stopping " << *this << std::endl;

    auto os = StopImpl();

    m_stateProvider.StopSession();
    m_debugMode = false;

    return os;
}

OperationStatus ChunkConsumer::SplitRecordings()
{
    return OperationStatus();
}

OperationStatus ChunkConsumer::SetPostCollectionActions(bool compress, bool upload)
{
    (void)compress;
    (void)upload;
    return OperationStatus();
}

OperationStatus ChunkConsumer::TestRecordingConditions()
{
    return OperationStatus();
}

void ChunkConsumer::Enable(RecordingTrigger recordingTrigger, bool debugMode)
{
    m_debugMode = debugMode;
    m_stateProvider.EnableSession(recordingTrigger);
}

void ChunkConsumer::AnalyzeLastOperation(const OperationStatus& os)
{
    if (!os)
    {
        HandleFatalError(os.GetStatusMessage());
    }
}

void ChunkConsumer::HandleFatalError(const std::string& errMsg)
{
    m_stateProvider.ReportFatalError(errMsg);
}

std::ostream & operator<<(std::ostream & output, const ChunkConsumer& cc)
{
    return output << cc.m_deviceName << "_" << cc.m_tracerCpuType << "_" << cc.m_loggingType;
}
