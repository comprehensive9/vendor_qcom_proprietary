/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "OperationStatus.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorModuleVerbosity.h"
#include "Utils.h"
#include "ChunkHealthReport.h"
#include "LogFmtStringsContainer.h"

#include <string>
#include <vector>
#include <memory>
#include <mutex>

class Device;
class AtomicFwInfo;
class MessageDispatcher;

// Helper struct to consolidate all the (legacy) log buffer properties
struct LogBufferInfo
{
    LogBufferInfo(CpuType cpuType, uint32_t logBufferInfoAddress, int ahbToLinkerDelta)
        : LogCpuType(cpuType)
        , LogBufferInfoAddress(logBufferInfoAddress)
        , AhbToLinkerDelta(ahbToLinkerDelta)
        , LogAddressAhb(0)
        , PrevLogOffset(0xFFFFFFFF)
    {}

    const CpuType LogCpuType;
    const uint32_t LogBufferInfoAddress;  // AHB address of log buffer info
    const uint32_t AhbToLinkerDelta;      // AHB to Linker address diff for related memory range

    uint32_t LogAddressAhb;               // log buffer AHB start address
    uint32_t PrevLogOffset;               // last log buffer Linker start address
    module_level_info ModuleLevelInfo[NUM_MODULES] = {}; // raw module level verbosity array
};


class LogReaderBase : public Immobile
{
public:
    static bool Tracer3ppLogsFilterOn;

    explicit LogReaderBase(const std::string& deviceName, std::string debugLogPrefix, std::string pollerTaskName);
    virtual ~LogReaderBase();

    virtual OperationStatus ActivateLogging(
        CpuType cpuType, LoggingType loggingType,
        bool ignoreLogHistory, bool compress, bool upload,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity,
        RecordingTrigger recordingTrigger, bool debugMode) = 0;

    virtual OperationStatus RestoreLogging() = 0;
    virtual OperationStatus DeactivateLogging(CpuType cpuType, LoggingType loggingType) = 0;
    virtual OperationStatus SplitRecordings() = 0;

    virtual LoggingState GetLoggingState(CpuType cpuType, LoggingType loggingType) const = 0;
    virtual const std::string& GetLoggingStateMsg(CpuType cpuType, LoggingType loggingType) const = 0;
    virtual ChunkHealthReport GetLoggingHealth(CpuType cpuType, LoggingType loggingType) const = 0;
    virtual LogFmtStringsContainer GetFmtStrings(CpuType cpuType, LoggingType loggingType) const = 0;

    bool IsPollerActive() const { return m_pollerActive; }

    virtual OperationStatus EnforceModuleVerbosity(CpuType cpuType) = 0;
    virtual OperationStatus SetModuleVerbosity(CpuType cpuType, const std::vector<LogCollectorModuleVerbosity> & enforcedVerbosity) = 0;
    virtual OperationStatus GetModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> & moduleVerbosity) = 0;
    virtual OperationStatus GetEnforcedModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> &moduleVerbosity) const = 0;

    virtual void ReportDeviceRemoved() = 0;
    void UnRegisterPoller();

protected:
    mutable std::mutex m_pollRegisterMutex;
    const std::string m_deviceName;
    const std::string m_debugLogPrefix;
    const std::string m_pollerTaskName;
    bool m_valid; // indicates that the log buffer is initialized
    bool m_pollerActive;
    std::unique_ptr<AtomicFwInfo> m_upFwInfo;
    std::vector<uint8_t> m_logBufferContent; // log buffer content
    uint32_t m_rdPtr; // index of the last read log buffer DW (before modulo)
    uint32_t m_minRdPtr;
    uint32_t m_totalLogDwords;
    uint32_t m_totalMissedDwords;

    uint32_t LogBufferContentDwords() const { return static_cast<uint32_t>(m_logBufferContent.size() / sizeof(uint32_t)); }

    void Poll();
    void ResetState();
    OperationStatus RegisterPoller();
    OperationStatus ComputeLogBufferStartAddress(const Device& device, LogBufferInfo& logBufferInfo);
    OperationStatus GetModuleInfoFromDevice(const Device& device, LogBufferInfo& logBufferInfo) const;
    OperationStatus ApplyModuleVerbosity(const Device& device, const LogBufferInfo& logBufferInfo) const;

    OperationStatus ActivateLoggingImpl(
        MessageDispatcher& messageDispatcher, CpuType cpuType, LoggingType loggingType,
        bool ignoreLogHistory, bool compress, bool upload,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity,
        RecordingTrigger recordingTrigger, bool debugMode);
    OperationStatus EnforceModuleVerbosityImpl(
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity, LogBufferInfo& logBufferInfo);
    OperationStatus GetModuleVerbosityImpl(
        const LogBufferInfo& logBufferInfo, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity);

private:
    virtual bool IsLogBufferAvailable(CpuType cpuType) const = 0;
    virtual bool IsPollRequired() const = 0;
    virtual OperationStatus UpdateDeviceInfo(const Device& device) = 0; // called periodically until log buffer initialized
    virtual OperationStatus UpdateRingInfo(const Device& device) = 0;   // -//-
    virtual void ActivateConsumersIfRequired(const AtomicFwInfo& fwInfo) = 0;
    virtual void ReportAndUpdateFwVersion(const AtomicFwInfo& fwInfo) = 0; // called upon fw initialization after 1st polling iteration
    virtual OperationStatus ReadAndConsumeBuffer(const Device& device) = 0;
    virtual uint32_t GetBufferSizeInBytesById(uint32_t bufferSizeId) const = 0;
    virtual uint32_t GetCurrentWrPtr() = 0;
    virtual void ReportBufferFound() = 0;
};
