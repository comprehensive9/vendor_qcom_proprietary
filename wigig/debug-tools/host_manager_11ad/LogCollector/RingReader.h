/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "LogReaderBase.h"
#include "OperationStatus.h"
#include "LogCollectorDefinitions.h"

#include <memory>
#include <vector>
#include <array>
#include <string>

class Device;
class AtomicFwInfo;
class MessageDispatcher;
class LogCollectorModuleVerbosity;
class CyclicFileRecorder;
struct DmaDescriptor;

class RingReader : public LogReaderBase
{
public:
    RingReader(const std::string& deviceName,
        uint32_t logBufferInfoAddressFw, int ahbToLinkerDeltaFw,
        uint32_t logBufferInfoAddressUc, int ahbToLinkerDeltaUc);

    OperationStatus ActivateLogging(
        CpuType cpuType, LoggingType loggingType, bool ignoreLogHistory, bool compress, bool upload,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity, RecordingTrigger recordingTrigger, bool debugMode) override;
    OperationStatus RestoreLogging() override;
    OperationStatus DeactivateLogging(CpuType cpuType, LoggingType loggingType) override;
    OperationStatus SplitRecordings() override;

    LoggingState GetLoggingState(CpuType cpuType, LoggingType loggingType) const  override;
    const std::string& GetLoggingStateMsg(CpuType cpuType, LoggingType loggingType) const  override;
    ChunkHealthReport GetLoggingHealth(CpuType cpuType, LoggingType loggingType) const override;
    LogFmtStringsContainer GetFmtStrings(CpuType cpuType, LoggingType loggingType) const override;

    OperationStatus EnforceModuleVerbosity(CpuType cpuType) override;
    OperationStatus SetModuleVerbosity(CpuType cpuType, const std::vector<LogCollectorModuleVerbosity> &enforcedVerbosity) override;
    OperationStatus GetModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> &moduleVerbosity) override;
    OperationStatus GetEnforcedModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> &moduleVerbosity) const override;

    void ReportDeviceRemoved() override;

private:
    enum { NumCpuTypes = 2U };
    bool IsLogBufferAvailable(CpuType cpuType) const override;
    bool IsPollRequired() const override;
    OperationStatus UpdateDeviceInfo(const Device& device) override;
    OperationStatus UpdateRingInfo(const Device& device) override;
    void ActivateConsumersIfRequired(const AtomicFwInfo& fwInfo) override;
    void ReportAndUpdateFwVersion(const AtomicFwInfo& fwInfo) override;
    void ReportBufferFound() override;
    OperationStatus ReadAndConsumeBuffer(const Device& device) override;
    uint32_t GetBufferSizeInBytesById(uint32_t bufferSizeId) const override;
    uint32_t GetMaxResidualSizeInBytes() const;
    uint32_t GetCurrentWrPtr() override;

    void ConsumeBuffer(uint32_t logBufferLenBytes);
    void ConsumePmcLogEvent(
        MessageDispatcher* pMessageDispatcher, const LogBufferInfo& logBufferInfo,
        const uint32_t* pLogEventBase, uint32_t payloadLengthDWs, uint32_t chunkOffset, uint32_t& conversionErrCnt);
    void ConsumePmcDataEvent(const uint32_t* pLogEventBase, uint32_t payloadLengthDWs, uint32_t& conversionErrCnt) const;
    void DumpRawDataForDebugIfNeeded(const uint8_t* pBuf, uint32_t bufLenBytes) const;
    MessageDispatcher* SelectMessageDispatcher(CpuType cpuType, LoggingType loggingType) const;

    OperationStatus PmcGetDataHostMode(const Device& device, uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData);
    OperationStatus PmcGetDataFwMode(const Device& device, uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData);

    std::array<LogBufferInfo, NumCpuTypes> m_logBufferInfo;
    std::unique_ptr<MessageDispatcher> m_logMessageDispatcherFw;
    std::unique_ptr<MessageDispatcher> m_logMessageDispatcherUcode;
    std::unique_ptr<MessageDispatcher> m_dataMessageDispatcher;
    uint32_t m_residualOffsetBytes; // length of the last event residual part (if any)

    // Enforced verbosity is a subset of module verbosity flags that will be applied on every device restart.
    // Modules that do not appear in enforced verbosity will left untouched (default value set by FW)
    // Default enforced verbosity means no changes are applied to the verbosity decided by FW.
    std::array<std::vector<LogCollectorModuleVerbosity>, NumCpuTypes> m_enforcedVerbosity;

    OperationStatus (RingReader::*m_pmcGetDataFunc) (const Device &, uint32_t &, uint8_t *, bool &); // PMC data accessor function for current mode
    std::vector<DmaDescriptor> m_fwRingDescriptors;
    std::unique_ptr<CyclicFileRecorder> m_cyclicDebugFileRecorder; // used to store raw PMC data, for debug purpose only
};
