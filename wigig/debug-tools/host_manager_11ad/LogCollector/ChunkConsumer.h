/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"
#include "LogCollectorDefinitions.h"
#include "ChunkStateProvider.h"
#include "LogFmtStringsContainer.h"

#include <string>

class AtomicFwInfo;
struct TimeStamp;

class ChunkConsumer
{
public:
    static const std::string  BufferOverrunMessage;
    static const std::string  DeviceRestartedMessage;
    static const std::string  CorruptedEntryMessage;
    static const std::string  InconsistentWptrMessage;
    static const std::string  PmcLogEventTooShortMessage;
    static const std::string  InconsistentNumParamsMessage;
    static const std::string  DeviceRemovedMessage;
    static const std::string  DeviceDiscoveredMessage;

    ChunkConsumer(const std::string &deviceName, CpuType cpuType, bool tsfAvailable, LoggingType loggingType);
    virtual ~ChunkConsumer() = default;
    ChunkConsumer(const ChunkConsumer & lcc) = delete;
    ChunkConsumer & operator=(const ChunkConsumer & lcc) = delete;

    friend std::ostream& operator<<(std::ostream &output, const ChunkConsumer& cc);

    virtual void OnStartNewChunk(
        const module_level_info* moduleLevelInfoArray,
        const TimeStamp& ts, uint32_t rdptr, uint32_t wrptr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw, bool firstIteration, bool deviceRestartDetected,
        const uint32_t* pDwData, uint32_t realChunkDwords, uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords) = 0;
    virtual void ReportCorruptedEntry(unsigned signature) = 0;
    virtual void ReportTooManyParameters(unsigned numDwords, unsigned numDwordsTh) = 0;
    virtual void ReportInconsistentWptr(unsigned diffDwords) = 0;
    virtual void ReportPmcLogEventTooShort(unsigned payloadLengthDWs) = 0;
    virtual void ReportInconsistentNumParameters(unsigned diffDwords) = 0;
    virtual void ReportDeviceRemoved() = 0;
    virtual void ReportDeviceDiscovered() = 0;
    virtual void ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo,
        uint32_t bufferStartAddress, uint32_t bufferSize) = 0; // called only if fw was not initialized during consumer activation
    virtual void ConsumeMessage(
        bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
        uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound) = 0;
    virtual void OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt) = 0;

    void ReportPersistentRecordingTriggered() { m_stateProvider.ReportPersistentRecordingTriggered(); }
    void ReportBufferFound() { m_stateProvider.ReportBufferFound(); }

    // recording methods
    OperationStatus Stop(); // close files
    virtual OperationStatus SplitRecordings(); // close current file and start a new one.
    void Enable(RecordingTrigger recordingTrigger, bool debugMode);
    virtual OperationStatus Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize) = 0;
    void HandleFatalError(const std::string& errMsg); // state becomes ERROR_OCCURED

    virtual OperationStatus SetPostCollectionActions(bool compress, bool upload);
    virtual OperationStatus TestRecordingConditions();

    LoggingState GetState() const { return m_stateProvider.GetState(); }
    const std::string& GetStateMessage() const { return m_stateProvider.GetFatalError(); }
    ChunkHealthReport GetHealthReport() { return m_stateProvider.GetReport(); }
    virtual LogFmtStringsContainer GetFmtStrings() { return LogFmtStringsContainer(); } // dummy
    bool IsDebugMode() const { return m_debugMode; }

protected:
    const std::string m_deviceName;
    const CpuType m_tracerCpuType;
    const bool m_tsfAvailable;
    const LoggingType m_loggingType;
    ChunkStateProvider m_stateProvider;
    bool m_debugMode;

    void AnalyzeLastOperation(const OperationStatus& os);
    virtual OperationStatus StopImpl() = 0;
};
