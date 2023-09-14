/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "PmcDataRecorderChunkConsumer.h"
#include "OperationStatus.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "PmcCommandHandler/PmcRecordingFileCreator.h"

using namespace std;

PmcDataRecorderChunkConsumer::PmcDataRecorderChunkConsumer(const std::string& deviceName, CpuType tracerType, bool tsfAvailable)
    : ChunkConsumer(deviceName, tracerType, tsfAvailable, PmcDataRecorder)
    , m_cyclicFileRecorder(
        true /*auto-split*/,
        Host::GetHost().GetConfiguration().PmcConfiguration.NormalizedTargetLocation,
        PmcRecordingExtractor::s_dataFileNamePattern,
        PmcRecordingExtractor::s_dataFileExtension,
        deviceName + "_pmc_recorder: ",
        Host::GetHost().GetConfiguration().PmcConfiguration.MaxDataFileSize,
        Host::GetHost().GetConfiguration().PmcConfiguration.MaxNumOfDataFiles,
        false)
{}

PmcDataRecorderChunkConsumer::~PmcDataRecorderChunkConsumer()
{
    Stop();
}

void PmcDataRecorderChunkConsumer::OnStartNewChunk(
    const module_level_info* moduleLevelInfoArray,
    const TimeStamp& ts, uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
    bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
    uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
{
    (void)pDwData;
    (void)realChunkDwords;
    (void)moduleLevelInfoArray;
    (void)ts;

    // Currently not in use, should become a part of an activity log
    (void)rdPtr;
    (void)wrPtr;
    (void)postReadWrPtr;
    (void)possiblyCoruptedDw;
    (void)firstIteration;
    (void)deviceRestartDetected;
    (void)virtualChunkDwords;
    (void)missedDwords;
    (void)totalDwords;
    (void)totalMissedDwords;

    const OperationStatus os = m_cyclicFileRecorder.ResetFileIfNeeded(); // true means reset happen or reset not needed
    if (!os)
    {
        LOG_ERROR << os.GetStatusMessage() << endl;
    }

    m_cyclicFileRecorder.ResetPerfCounter();
}

void PmcDataRecorderChunkConsumer::ReportCorruptedEntry(unsigned signature)
{
    (void)signature;
}

void PmcDataRecorderChunkConsumer::ReportTooManyParameters(unsigned numDwords, unsigned numDwordsTh)
{
    (void)numDwords;
    (void)numDwordsTh;
}

void PmcDataRecorderChunkConsumer::ReportInconsistentWptr(unsigned diffDwords)
{
    (void)diffDwords;
}

void PmcDataRecorderChunkConsumer::ReportPmcLogEventTooShort(unsigned payloadLengthDWs)
{
    (void)payloadLengthDWs;
}

void PmcDataRecorderChunkConsumer::ReportInconsistentNumParameters(unsigned diffDwords)
{
    (void)diffDwords;
}

void PmcDataRecorderChunkConsumer::ReportDeviceRemoved()
{
    if (GetState() != LoggingState::ACTIVE)
    {
        return;
    }
    m_stateProvider.DeactivateSession();
    m_cyclicFileRecorder.SplitOutputFile();
}

void PmcDataRecorderChunkConsumer::ReportDeviceDiscovered()
{}

void PmcDataRecorderChunkConsumer::ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    (void)moduleLevelInfoArray;
    (void)fwInfo;
    m_stateProvider.ReportFwInfo(fwInfo, bufferStartAddress, bufferSize);
}

void PmcDataRecorderChunkConsumer::ConsumeMessage(
    bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
    uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound)
{
    (void)possiblyCorrupted;
    (void)pDwData;
    (void)pmcEventTsf;
    (void)chunkOffset;
    (void)rdPtr;
    (void)conversionErrCnt;
    (void)invalidStringOffsetFound;

    auto result = m_cyclicFileRecorder.WriteToOutputFile(reinterpret_cast<const char*>(&messageHeader), sizeof(uint32_t));
    if (!result)
    {
        AnalyzeLastOperation(result);
        return;
    }
    result = m_cyclicFileRecorder.WriteToOutputFile(reinterpret_cast<const char*>(pPayload), payloadSize * sizeof(int));
    AnalyzeLastOperation(result);
}

void PmcDataRecorderChunkConsumer::OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt)
{
    (void)msgCounter;
    (void)chunkHandlingTime;
    (void)conversionErrCnt;
    LOG_VERBOSE << "Log chunk file write time: " << m_cyclicFileRecorder.GetPerfCounter() << endl;
}

OperationStatus PmcDataRecorderChunkConsumer::StopImpl()
{
    m_cyclicFileRecorder.Pause();

    return OperationStatus(true, m_cyclicFileRecorder.GetOutputFilesFolder());
}

OperationStatus PmcDataRecorderChunkConsumer::Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    LOG_VERBOSE << "Activating " << *this << endl;

    (void)moduleLevelInfoArray;
    (void)fwInfo;

    if (GetState() == LoggingState::ACTIVE)
    {
        return OperationStatus(false, "PmcDataRecorderChunkConsumer already activated");
    }

    // create new session folder, new file will be created once first write happens
    std::string targetLocation;
    const std::string baseDir = Host::GetHost().GetConfiguration().PmcConfiguration.NormalizedTargetLocation;
    auto os = PmcRecordingPathBuilder::CreateFinalPmcFileLocation(m_deviceName, baseDir, targetLocation);
    if (!os)
    {
        AnalyzeLastOperation(os);
        return os;
    }

    os = m_cyclicFileRecorder.SetOutputFilesFolder(targetLocation);
    if (!os)
    {
        AnalyzeLastOperation(os);
        return os;
    }

    m_cyclicFileRecorder.Resume();
    m_stateProvider.ActivateSession(fwInfo, bufferStartAddress, bufferSize);
    return OperationStatus();
}
