/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "OperationStatus.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorModuleVerbosity.h"
#include "DeferredRecordingContext.h"
#include "ChunkHealthReport.h"

class LogFmtStringsContainer;
enum class wil_fw_state;
class DeviceLoggingContext;

class LogCollectorService
{
public:
    LogCollectorService();
    ~LogCollectorService();

    void OnDeviceDiscovered(const std::string& deviceName);
    void OnDeviceRemoved(const std::string& deviceName);

    OperationStatus StartLogging(
        const std::string& deviceName, CpuType cpuType, LoggingType loggingType,
        bool ignoreLogHistory, bool compress, bool upload, bool debugMode,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity);
    OperationStatus StopLogging(const std::string& deviceName, CpuType cpuType, LoggingType loggingType);
    OperationStatus SplitAllRecordings();

    OperationStatus GetLoggingState(
        const std::string& deviceName, CpuType cpuType, LoggingType loggingType,
        LoggingState& loggingState, std::string& loggingStateMsg) const;
    OperationStatus GetLoggingHealth(const std::string& deviceName, CpuType cpuType, LoggingType loggingType,
                                     ChunkHealthReport& healthReport) const;
    OperationStatus GetRecordersHealth(const std::string& deviceName, CpuType cpuType, LoggingType loggingType, std::vector<ChunkHealthReport>& reports) const;
    OperationStatus GetFmtStrings(const std::string& deviceName, CpuType cpuType,
        LoggingType loggingType, LogFmtStringsContainer& fmtStrings) const;

    bool IsLogging();
    bool IsOnTargetLogRecording(const std::string& deviceName, CpuType cpuType) const;

    void NotifyFwStateChange(const std::string& deviceName, wil_fw_state fwState);

    std::shared_ptr<DeferredRecordingContext> GetDeferredRecordingContext() const { return m_spDeferredRecordingCtx; }
    OperationStatus EnableDeferredRecording(std::shared_ptr<DeferredRecordingContext> deferredRecordingContext);
    OperationStatus DisableDeferredLogging();

    OperationStatus SetModuleVerbosity(
        const std::string& deviceName, CpuType cpuType, const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity);
    OperationStatus GetModuleVerbosity(
        const std::string& deviceName, CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity);
    OperationStatus GetEnforcedModuleVerbosity(
        const std::string& deviceName, CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity);

    static const std::string& GetStringConversionFilename(CpuType cpuType);

    void PublishLoggingHealth(const std::string& deviceName) const;

    // delete copy constructor and assignment operator
    // keep public for better error message
    // no move members will be declared implicitly
    LogCollectorService(const LogCollectorService&) = delete;
    LogCollectorService& operator=(const LogCollectorService&) = delete;

private:

    static const std::string sc_fwStringConversionFilename;
    static const std::string sc_ucodeStringConversionFilename;

    std::unordered_map<std::string, std::unique_ptr<DeviceLoggingContext> > m_DeviceLoggingContextMap;
    std::shared_ptr<DeferredRecordingContext> m_spDeferredRecordingCtx;
};

