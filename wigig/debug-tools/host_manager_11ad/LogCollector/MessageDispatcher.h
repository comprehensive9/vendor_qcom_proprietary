/*
* Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"
#include "LogCollectorDefinitions.h"
#include "ChunkHealthReport.h"
#include "LogFmtStringsContainer.h"
#include "Utils.h"
#include <memory>
#include <array>
#include <string>

class AtomicFwInfo;
class ChunkConsumer;
struct TimeStamp;

class MessageDispatcher final : public Immobile
{
public:
    MessageDispatcher(const std::string &deviceName, CpuType tracerType, bool tsfAvailable);
    ~MessageDispatcher();

    void DispatchMessage(
        bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData,
        uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound, uint32_t pmcEventTsf = 0);

    void ActivateConsumersIfRequired(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize);
    void EnableConsumer(LoggingType loggingType, RecordingTrigger recordingTrigger, bool debugMode);
    OperationStatus ConfigureAndTestRecordingConditions(LoggingType loggingType, bool compress, bool upload);
    OperationStatus SplitRecordings();
    OperationStatus StopConsumer(LoggingType loggingType);

    bool IsActivationRequired() const;
    bool IsPollRequired() const;
    bool IsDebugMode() const;

    LoggingState GetConsumerState(LoggingType loggingType) const;
    const std::string& GetConsumerStateMessage(LoggingType loggingType) const;
    ChunkHealthReport GetConsumerHealth(LoggingType loggingType) const;
    LogFmtStringsContainer GetFmtStrings(LoggingType loggingType) const;

    void ReportDeviceRemoved();
    void ReportDeviceUp();
    void ReportPersistentRecordingTriggered();
    void ReportBufferFound();
    void ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferAddress, uint32_t bufferSize);
    void HandleErrorForEnabledConsumers(const std::string& errMsg);
    void HandleError(LoggingType loggingType, const std::string& errMsg);
    void StartNewBuffer(
        const module_level_info* moduleLevelInfoArray,
        uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
        bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
        uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords);
    void ReportCorruptedEntry(unsigned signature);
    void ReportTooManyParameters(unsigned numParams, unsigned maxParamsTh);
    void ReportInconsistentWptr(unsigned diffDwords);
    void ReportPmcLogEventTooShort(unsigned payloadLengthDWs);
    void ReportInconsistentNumParameters(unsigned diffDwords);
    void EndBuffer(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt);

protected:
    std::array<std::unique_ptr<ChunkConsumer>, MaxConsumers> m_chunkConsumers;
    std::string m_debugLogPrefix;
};
