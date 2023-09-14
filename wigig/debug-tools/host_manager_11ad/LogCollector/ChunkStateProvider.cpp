/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "ChunkStateProvider.h"
#include "DriverAPI.h"
#include "LogCollectorGetHealthHandler.h"
#include "PersistentConfiguration.h"
#include "CyclicFileRecorder.h"
#include "Host.h"
#include "HostInfo.h"

#include <json/json.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;

ChunkStateProvider::ChunkStateProvider(const string &deviceName, CpuType tracerType, LoggingType loggingType)
    : m_debugLogPrefix(deviceName + "_" + CpuTypeToString(tracerType) + "_" + LoggingTypeToString(loggingType) + "_health_provider: ")
    , m_state(LoggingState::DISABLED)
{
    m_healthReport.DeviceName = deviceName;
    m_healthReport.TraceType = tracerType;
    m_healthReport.LogType = loggingType;
    m_healthReport.RecordTrigger = RecordingTrigger::NotDefinedYet;
    SetStatToZero();
}

void ChunkStateProvider::EnableSession(RecordingTrigger recordingTrigger)
{
    m_healthReport.RecordTrigger = recordingTrigger;
    m_state = LoggingState::ENABLED;
}

void ChunkStateProvider::ActivateSession(const AtomicFwInfo& fwInfo, uint32_t bufferAddress, uint32_t bufferSize)
{
    ReportFwInfo(fwInfo, bufferAddress, bufferSize);
    m_sessionTimer.Reset();
    m_healthReport.StartRecordingTimestamp = Utils::GetCurrentLocalTimeString();
    m_state = LoggingState::ACTIVE;
}

void ChunkStateProvider::DeactivateSession()
{
    m_state = LoggingState::ENABLED;

    SetStatToZero();
}

void ChunkStateProvider::StopSession()
{
    if (m_state == LoggingState::DISABLED)
    {
        return;
    }
    m_state = LoggingState::DISABLED;
    m_healthReport.RecordTrigger = RecordingTrigger::NotDefinedYet;

    SetStatToZero();
}

void ChunkStateProvider::ReportFwInfo(const AtomicFwInfo& fwInfo, uint32_t bufferAddress, uint32_t bufferSize)
{
    m_healthReport.FWVersion = fwInfo.FwUniqueId.m_fwVersion;
    m_healthReport.BufferAddress = bufferAddress;
    m_healthReport.BufferSize = bufferSize;
    m_healthReport.FwUpFound = fwInfo.IsInitialized();
}

void ChunkStateProvider::ReportFatalError(const std::string& errMsg)
{
    m_state = LoggingState::ERROR_OCCURED;
    m_healthReport.FatalError = errMsg;
}

void ChunkStateProvider::ReportNewChunk(
    uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
    bool firstIteration, bool deviceRestartDetected, uint32_t realChunkDwords, uint32_t virtualChunkDwords, uint32_t missedDwords)
{
    m_healthReport.TotalChunks++;
    m_healthReport.PossiblyCorruptedDws += possiblyCoruptedDw;
    m_healthReport.FinalWritePtr = wrPtr;

    m_healthReport.TotalLogDWs += realChunkDwords;
    m_totalMissedDwords += missedDwords;

    if (firstIteration)
    {
        m_healthReport.InitialReadPtr = rdPtr;
        m_healthReport.InitialWritePtr = wrPtr;
        LOG_VERBOSE << m_debugLogPrefix << "First iteration:"
            << " rdPtr=" << m_healthReport.InitialReadPtr
            << " wrPtr=" << m_healthReport.InitialWritePtr
            << endl;

        m_lastIterationDuration = std::chrono::duration<double, std::milli>::zero();
    }
    else
    {
        m_lastIterationDuration = std::chrono::steady_clock::now() - m_iterationStartPoint;
    }
    m_iterationStartPoint = std::chrono::steady_clock::now();

    // when recording starts upon request, wtpr is much bigger than initial rptr, so chunk size on fist iteration is irrelevant for statistics
    // thus, do not update miss, efficiency and data rates due to dependency on chunk size
    if (!(firstIteration && virtualChunkDwords > m_healthReport.BufferSize))
    {
        UpdateMaxAndAvgValues(m_healthReport.MaxMissRate, m_healthReport.AvgMissRate, CalculateRate(missedDwords, virtualChunkDwords), m_healthReport.TotalChunks);
        UpdateMaxAndAvgValues(m_healthReport.MaxChunkEfficiencyRate, m_healthReport.AvgChunkEfficiencyRate, CalculateRate(virtualChunkDwords, m_healthReport.BufferSize), m_healthReport.TotalChunks);
        UpdateMaxAndAvgValues(m_healthReport.MaxDataRate, m_healthReport.AvgDataRate, CalculateRate(virtualChunkDwords * 4, m_lastIterationDuration.count()), m_healthReport.TotalChunks); // from DWs to bytes
    }

    LOG_VERBOSE << m_debugLogPrefix << "Start chunk handling."
        << " First iteration: " << BoolStr(firstIteration)
        << " Time elapsed b/w 2 iterations: " << m_lastIterationDuration.count()
        << " Parsed Size: " << wrPtr - rdPtr << " (DW)"
        << " Virtual size: " << virtualChunkDwords << " (DW)"
        << " Chunk efficiency rate: " << Percentage(virtualChunkDwords, m_healthReport.BufferSize)
        << " Missed: " << missedDwords << " (DW)"
        << " Miss rate: " << Percentage(missedDwords, virtualChunkDwords)
        << " Data rate: " << CalculateRate(virtualChunkDwords * 4, m_lastIterationDuration.count()) << " KB/sec"
        << " Accumulated miss rate: " << Percentage(m_totalMissedDwords, m_healthReport.TotalLogDWs)
        << " Total chunks parsed: " << m_healthReport.TotalChunks
        << " Effective RdPtr: " << rdPtr
        << " Effective WrPtr: " << wrPtr
        << " Post Read WrPtr: " << postReadWrPtr
        << " Possibly corrupted DWs: " << possiblyCoruptedDw
        << " Device restart: " << BoolStr(deviceRestartDetected)
        << endl;
}

void ChunkStateProvider::ReportChunkConsumed(uint32_t msgCounter, double chunkReadingTime, uint32_t conversionErrCnt)
{
    m_healthReport.TotalMsgs += msgCounter;
    m_totalConversionErrors += conversionErrCnt;

    m_healthReport.ConversionErrorRate = static_cast<uint32_t>(CalculateRate(m_totalConversionErrors, m_healthReport.TotalMsgs));

    UpdateMaxAndAvgValues(m_healthReport.MaxIterationTime, m_healthReport.AvgIterationTime, chunkReadingTime,
        m_healthReport.TotalChunks);
    UpdateMaxAndAvgValues(m_healthReport.MaxThroughput, m_healthReport.AvgThroughput, CalculateRate(msgCounter, m_lastIterationDuration.count()), m_healthReport.TotalChunks);

    LOG_VERBOSE << m_debugLogPrefix << "Log chunk processed. All statistics recorded."
        << " Messages per msec: " << CalculateRate(msgCounter, m_lastIterationDuration.count())
        << " Accumulated conversion error rate: " << Percentage(m_totalConversionErrors, m_healthReport.TotalMsgs)
        << " Handling time: " << chunkReadingTime << " msec"
        << " Messages: " << msgCounter
        << " Total messages: " << m_healthReport.TotalMsgs
        << endl;
}

void ChunkStateProvider::UpdateMaxAndAvgValues(double& maxValue, double& avgValue, double currentValue, uint32_t total) const
{
    maxValue = max(maxValue, currentValue);
    // here we assume that total is already incremented in current iteration
    avgValue = (avgValue * (total - 1) + currentValue) / total;
}

double ChunkStateProvider::CalculateRate(double part, double whole)
{
    return whole == 0 ? 0.0 : part / whole;
}

void ChunkStateProvider::SetStatToZero()
{
    LOG_VERBOSE << m_debugLogPrefix << "Stopped recording statistics. Setting to zero all statistics" << endl;

    m_lastIterationDuration = std::chrono::duration<double, std::milli>::zero();

    m_totalMissedDwords = 0;
    m_totalConversionErrors = 0;

    m_healthReport.InitialReadPtr = 0;
    m_healthReport.InitialWritePtr = 0;
    m_healthReport.FinalWritePtr = 0;

    m_healthReport.AvgChunkEfficiencyRate = 0.0;
    m_healthReport.MaxChunkEfficiencyRate = 0.0;
    m_healthReport.AvgThroughput = 0.0;
    m_healthReport.MaxThroughput = 0.0;
    m_healthReport.AvgIterationTime = 0.0;
    m_healthReport.MaxIterationTime = 0.0;
    m_healthReport.AvgMissRate = 0;
    m_healthReport.MaxMissRate = 0;
    m_healthReport.AvgDataRate = 0;
    m_healthReport.MaxDataRate = 0;

    m_healthReport.PossiblyCorruptedDws = 0;
    m_healthReport.CorruptedEntryErrorCnt = 0;
    m_healthReport.TooManyParamsErrorCnt = 0;
    m_healthReport.InconsistentWptrErrorCnt = 0;
    m_healthReport.PmcLogEventTooShortErrorCnt = 0;
    m_healthReport.InconsistentNumParamsErrorCnt = 0;
    m_healthReport.FatalError = string();
    m_healthReport.ConversionErrorRate = 0;

    m_healthReport.TotalChunks = 0;
    m_healthReport.TotalMsgs = 0;
    m_healthReport.TotalLogDWs = 0;

    m_healthReport.BufferAddress = 0;
    m_healthReport.BufferSize = 0;

    m_healthReport.TotalContentBytes = 0;
    m_healthReport.TotalFilesNumber = 0;
    m_healthReport.RecordTrigger = RecordingTrigger::NotDefinedYet;
    m_healthReport.FwBufferFound = false;
    m_healthReport.FwUpFound = false;
}

ChunkHealthReport ChunkStateProvider::GetReport()
{
    m_sessionTimer.Stop(); // stop session timer to get elapsed time
    const auto report = BuildReport(); // no captured data (log files, etc.) provided in explicit request
    m_sessionTimer.Start(); // resume session timer
    return report;
}

ChunkHealthReport ChunkStateProvider::BuildReport(const queue<string>& filesQueue, double totalContentBytes, uint32_t totalFilesNumber)
{
    LOG_VERBOSE << m_debugLogPrefix << "Started building report" << endl;

    // set snapshot data
    m_healthReport.FilesQueue = filesQueue;
    m_healthReport.TotalContentBytes = static_cast<uint32_t>(totalContentBytes);
    m_healthReport.TotalFilesNumber = totalFilesNumber;
    m_healthReport.SessionDuration = m_sessionTimer.GetTotalMsec();
    m_healthReport.State = m_state;
    m_healthReport.StopRecordingTimestamp = Utils::GetCurrentLocalTimeString();

    LOG_VERBOSE << m_debugLogPrefix << "Finished building report" << endl;
    return m_healthReport;
}

// export to manifest file
void ChunkStateProvider::ExportToFile(const CyclicFileRecorder& cyclicFileRecorder)
{
    ostringstream ss;
    ss << string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation)
        << "manifest_" << m_healthReport.DeviceName << "_" << m_healthReport.TraceType << "_"<< m_healthReport.LogType << '_' << Utils::GetCurrentLocalTimeForFileName() << ".json";
    const auto fileName = ss.str();
    LOG_DEBUG << m_debugLogPrefix << "Requested to export health report to manifest file: " << fileName << endl;

    ofstream file;
    file.open(fileName);
    if (file.fail())
    {
        LOG_ERROR << "Creating manifest file failed: " << fileName << " Error: " << strerror(errno);
        return;
    }

    const auto report = BuildReport(cyclicFileRecorder.GetFiles(), cyclicFileRecorder.GetTotalContentMB(), cyclicFileRecorder.GetTotalFilesNumber());
    Json::Value root;
    report.ToJson(root, true);

    Json::StyledStreamWriter writer;
    writer.write(file, root);
    file.close();

    LOG_DEBUG << m_debugLogPrefix << "Health report exported to manifest file: " << fileName << endl;
}
