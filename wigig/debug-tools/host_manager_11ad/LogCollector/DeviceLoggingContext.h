/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DeviceTypes.h"
#include "LogCollectorDefinitions.h"
#include "OperationStatus.h"
#include "ChunkHealthReport.h"
#include "LogFmtStringsContainer.h"

class Device;
class LogReaderBase;
class LogCollectorModuleVerbosity;

class DeviceLoggingContext
{
public:
    explicit DeviceLoggingContext(const std::string &deviceName);
    ~DeviceLoggingContext();

    OperationStatus ActivateLogging(
        CpuType cpuType, LoggingType loggingType, bool ignoreLogHistory, bool compress, bool upload,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity,
        RecordingTrigger recordingTrigger, bool debugMode);

    bool IsPollerActive(CpuType cpuType) const;
    OperationStatus DeactivateLogging(CpuType cpuType, LoggingType loggingType);

    LoggingState GetLoggingState(CpuType cpuType, LoggingType loggingType) const;
    const std::string& GetLoggingStateMsg(CpuType cpuType, LoggingType loggingType) const;
    ChunkHealthReport GetLoggingHealth(CpuType cpuType, LoggingType loggingType) const;
    LogFmtStringsContainer GetFmtStrings(CpuType cpuType, LoggingType loggingType) const;

    OperationStatus EnforceModuleVerbosity(CpuType cpuType);
    OperationStatus SetModuleVerbosity(CpuType cpuType, const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity);
    OperationStatus GetModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity);
    OperationStatus GetEnforcedModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity);

    void RestoreLoggingState();
    void UnregisterPollerAndReportDeviceRemoved();
    OperationStatus SplitRecordings();

private:
    static unsigned ComputeAhbToLinkerDelta(BasebandType basebandType, CpuType cpuType);
    LogReaderBase* GetLogReader(CpuType cpuType);
    const LogReaderBase* GetLogReader(CpuType cpuType) const;
    OperationStatus CheckCollectionModeValidity() const;

    const std::string m_deviceName;
    std::unique_ptr<LogReaderBase> m_upFwLogCollector;
    std::unique_ptr<LogReaderBase> m_upUcodeLogCollector;
    std::unique_ptr<LogReaderBase> m_upRingReader;
    bool m_isContinuousPmc;
};
