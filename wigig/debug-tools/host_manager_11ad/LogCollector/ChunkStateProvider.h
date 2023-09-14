/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "Utils.h"
#include "LogCollectorDefinitions.h"
#include "ChunkHealthReport.h"

#include <string>
#include <queue>
#include <atomic>

class AtomicFwInfo;
class CyclicFileRecorder;

class ChunkStateProvider
{
public:
    ChunkStateProvider(const std::string &deviceName, CpuType tracerType, LoggingType loggingType);

    // logs statistics session control
    void EnableSession(RecordingTrigger recordingTrigger); // called upon consumer enabling request
    void ActivateSession(const AtomicFwInfo& fwInfo, uint32_t bufferAddress, uint32_t bufferSize); // called upon activation in polling iteration (start/restore logging), creates new manifest file
    void DeactivateSession(); // called upon device removal
    void StopSession(); // called upon end logging

    // getters
    ChunkHealthReport GetReport();
    LoggingState GetState() const { return m_state; }
    const std::string& GetFatalError() const { return m_healthReport.FatalError; }
    double GetAvgMissRate() const { return m_healthReport.AvgMissRate; }

    void ReportFwInfo(const AtomicFwInfo& fwInfo, uint32_t bufferAddress, uint32_t bufferSize); // called either upon session activation (logging start/restoring) or later fw initialization (but only once per session)
    void ReportNewChunk(
        uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
        bool firstIteration, bool deviceRestartDetected, uint32_t realChunkDwords, uint32_t virtualChunkDwords, uint32_t missedDwords);
    void ReportChunkConsumed(uint32_t msgCounter, double chunkReadingTime, uint32_t conversionErrCnt); // upon chunk end

    void ReportFatalError(const std::string& errMsg); // consumer entered error state
    void ReportCorruptedEntry() { m_healthReport.CorruptedEntryErrorCnt++; }
    void ReportTooManyParameters() { m_healthReport.TooManyParamsErrorCnt++; }
    void ReportInconsistentWptr() { m_healthReport.InconsistentWptrErrorCnt++; }
    void ReportPmcLogEventTooShort() { m_healthReport.PmcLogEventTooShortErrorCnt++; }
    void ReportInconsistentNumParameters() { m_healthReport.InconsistentNumParamsErrorCnt++; }


    void ReportPersistentRecordingTriggered() { m_healthReport.RecordTrigger = RecordingTrigger::Persistent; }
    void ReportBufferFound() { m_healthReport.FwBufferFound = true; }

    // export to manifest file
    void ExportToFile(const CyclicFileRecorder& cyclicFileRecorder); // call explicitly in recorders before DeactivateSession() and StopSession()

private:
    const std::string m_debugLogPrefix;
    std::atomic<LoggingState> m_state;
    ChunkHealthReport m_healthReport;
    Timer m_sessionTimer; // stored as [msec]; report as : [sec]
    uint32_t m_totalMissedDwords;
    std::chrono::time_point<std::chrono::steady_clock> m_iterationStartPoint;
    std::chrono::duration<double, std::milli> m_lastIterationDuration;

    uint32_t m_totalConversionErrors; // relevant only for txtRecorder

    // methods
    ChunkHealthReport BuildReport(const std::queue<std::string>& filesQueue = std::queue<std::string>(), double totalContentBytes = 0.0, uint32_t totalFilesNumber = 0);

    // statistics collection
    void UpdateMaxAndAvgValues(double& maxValue, double& avgValue, double currentValue, uint32_t total) const;
    static double CalculateRate(double part, double whole);
    void SetStatToZero();
};
