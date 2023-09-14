/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "LogReaderBase.h"
#include "OperationStatus.h"
#include "LogCollectorDefinitions.h"

#include <memory>
#include <mutex>
#include <vector>
#include <string>

class Device;
class AtomicFwInfo;
class MessageDispatcher;
class LogCollectorModuleVerbosity;

class LogBufferReader : public LogReaderBase
{
public:
    LogBufferReader(const std::string& deviceName, CpuType tracerType, uint32_t logBufferInfoAddress, int ahbToLinkerDelta);

    OperationStatus ActivateLogging(
        CpuType cpuType, LoggingType loggingType, bool ignoreLogHistory, bool compress, bool upload,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity, RecordingTrigger recordingTrigger, bool debugMode) override;
    OperationStatus RestoreLogging() override;
    OperationStatus DeactivateLogging(CpuType cpuType, LoggingType loggingType) override;
    OperationStatus SplitRecordings() override;

    LoggingState GetLoggingState(CpuType cpuType, LoggingType loggingType) const override;
    const std::string& GetLoggingStateMsg(CpuType cpuType, LoggingType loggingType) const override;
    ChunkHealthReport GetLoggingHealth(CpuType cpuType, LoggingType loggingType) const override;
    LogFmtStringsContainer GetFmtStrings(CpuType cpuType, LoggingType loggingType) const override;

    OperationStatus EnforceModuleVerbosity(CpuType cpuType) override;
    OperationStatus SetModuleVerbosity(CpuType cpuType, const std::vector<LogCollectorModuleVerbosity> &enforcedVerbosity) override;
    OperationStatus GetModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> &moduleVerbosity) override;
    OperationStatus GetEnforcedModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> &moduleVerbosity) const override;

    void ReportDeviceRemoved() override;

private:
    bool IsLogBufferAvailable(CpuType cpuType) const override;
    bool IsPollRequired() const override;
    OperationStatus UpdateDeviceInfo(const Device& device) override;
    OperationStatus UpdateRingInfo(const Device& device) override;
    void ActivateConsumersIfRequired(const AtomicFwInfo& fwInfo) override;
    void ReportAndUpdateFwVersion(const AtomicFwInfo& fwInfo) override;
    void ReportBufferFound() override;
    OperationStatus ReadAndConsumeBuffer(const Device& device) override;
    uint32_t GetBufferSizeInBytesById(uint32_t bufferSizeId) const override;
    uint32_t GetCurrentWrPtr() override;

    OperationStatus ReadChunk(const Device& device, uint32_t wrPtr);
    OperationStatus ReadContinuousChunk(const Device& device, uint32_t beginIndex, uint32_t endIndex);
    uint32_t ConsumeRange(uint32_t wrPtr, uint32_t chunkPtr, uint32_t possiblyCorruptedDw, bool corruptedEntryExpected, uint32_t& conversionErrCnt);
    void ConsumeMessage(uint32_t wrPtr, uint32_t chunkPtr, uint32_t possiblyCorruptedDw, uint32_t& conversionErrCnt);
    uint32_t LogPtrToByteIndex(uint32_t logPtr) const;

    LogBufferInfo m_logBufferInfo;
    std::unique_ptr<MessageDispatcher> m_messageDispatcher;

    // Enforced verbosity is a subset of module verbosity flags that will be applied on every device restart.
    // Modules that do not appear in enforced verbosity will left untouched (default value set by FW)
    // Default enforced verbosity means no changes are applied to the verbosity decided by FW.
    std::vector<LogCollectorModuleVerbosity> m_enforcedVerbosity;

    std::vector<int> m_extractedMessageParameters; // to be reused during parameters extraction
    std::vector<uint8_t> m_logChunkContent; // log chunk content, to be used in debug mode
};
