/*
* Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "MessageDispatcher.h"
#include "OperationStatus.h"
#include "ChunkConsumer.h"
#include "XmlRecorderChunkConsumer.h"
#include "TxtRecorderChunkConsumer.h"
#include "RawPublisherChunkConsumer.h"
#include "PmcDataRecorderChunkConsumer.h"
#include "FwStateProvider.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "FileSystemOsAbstraction.h"

using namespace std;


MessageDispatcher::MessageDispatcher(const std::string& deviceName, CpuType tracerType, bool tsfAvailable)
    : m_debugLogPrefix(deviceName + "_" + CpuTypeToString(tracerType) + "_buffer consumer: ")
{
    m_chunkConsumers[XmlRecorder].reset(new XmlRecorderChunkConsumer(deviceName, tracerType, tsfAvailable));
    m_chunkConsumers[TxtRecorder].reset(new TxtRecorderChunkConsumer(deviceName, tracerType, tsfAvailable));
    m_chunkConsumers[RawPublisher].reset(new RawPublisherChunkConsumer(deviceName, tracerType, tsfAvailable));
    m_chunkConsumers[PmcDataRecorder].reset(new PmcDataRecorderChunkConsumer(deviceName, tracerType, tsfAvailable));
}

MessageDispatcher::~MessageDispatcher() = default;

void MessageDispatcher::DispatchMessage(
    bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData,
    uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound, uint32_t pmcEventTsf)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ConsumeMessage(
                possiblyCorrupted, messageHeader, pPayload, payloadSize, pDwData, pmcEventTsf, chunkOffset, rdPtr,
                conversionErrCnt, invalidStringOffsetFound);
        }
    }
}

void MessageDispatcher::ActivateConsumersIfRequired(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    for (auto i = 0; i < MaxConsumers; ++i)
    {
        auto &cc = m_chunkConsumers[i];
        const auto consumerState = cc->GetState();
        if (consumerState == LoggingState::ENABLED)
        {
            OperationStatus os = cc->Activate(moduleLevelInfoArray, fwInfo, bufferStartAddress, bufferSize);
            if (!os)
            {
                LOG_ERROR << "Failed to activate " << *cc << ": " << os.GetStatusMessage() << endl;
            }
        }
        // otherwise, nothing to do
    }
}

void MessageDispatcher::EnableConsumer(LoggingType loggingType, RecordingTrigger recordingTrigger, bool debugMode)
{
    m_chunkConsumers[loggingType]->Enable(recordingTrigger, debugMode);
}

OperationStatus MessageDispatcher::ConfigureAndTestRecordingConditions(LoggingType loggingType, bool compress, bool upload)
{
    // Validate log recording directory exists
    // Notes: In legacy mode can be called twice - once for FW and once for uCode,
    //        but it's promised to be serially ans so folder will be created only once.
    //        Not necessary for RawPublisher and PmcDataRecorder, but there is no harm
    const string targetLocation = Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation;
    if (!FileSystemOsAbstraction::DoesFolderExist(targetLocation))
    {
        LOG_INFO << m_debugLogPrefix << "Target recording directory does not exist. Creating one at: " << targetLocation << endl;
        const OperationStatus os = FileSystemOsAbstraction::CreateFolder(targetLocation);
        if (!os)
        {
            return OperationStatus(false, os.GetStatusMessage());
        }
    }

    return OperationStatus::Merge(
        m_chunkConsumers[loggingType]->SetPostCollectionActions(compress, upload),
        m_chunkConsumers[loggingType]->TestRecordingConditions());
}

OperationStatus MessageDispatcher::SplitRecordings()
{
    // note: PmcDataRecorder is not consider as recording, just as RawPublisher
    return OperationStatus::Merge(
        m_chunkConsumers[LoggingType::TxtRecorder]->SplitRecordings(),
        m_chunkConsumers[LoggingType::XmlRecorder]->SplitRecordings());
}

OperationStatus MessageDispatcher::StopConsumer(LoggingType loggingType)
{
    return m_chunkConsumers[loggingType]->Stop();
}

bool MessageDispatcher::IsActivationRequired() const
{
    for (const auto &cc : m_chunkConsumers)
    {
        LOG_VERBOSE << "Checking if activation required for " << *cc << endl;
        if (cc->GetState() == LoggingState::ENABLED)
        {
            return true;
        }
    }
    return false;
}

bool MessageDispatcher::IsPollRequired() const
{
    for (const auto &cc : m_chunkConsumers)
    {
        const auto consumerState = cc->GetState();
        if (consumerState != LoggingState::DISABLED && consumerState != LoggingState::ERROR_OCCURED)
        {
            return true; // poll if there is at least one not disabled and not in error consumer
        }
    }
    return false;
}

bool MessageDispatcher::IsDebugMode() const
{
    for (const auto &cc : m_chunkConsumers)
    {
        if (cc->IsDebugMode())
        {
            return true;
        }
    }
    return false;
}

LoggingState MessageDispatcher::GetConsumerState(LoggingType loggingType) const
{
    return m_chunkConsumers[loggingType]->GetState();
}

const string& MessageDispatcher::GetConsumerStateMessage(LoggingType loggingType) const
{
    return m_chunkConsumers[loggingType]->GetStateMessage();
}

ChunkHealthReport MessageDispatcher::GetConsumerHealth(LoggingType loggingType) const
{
    return m_chunkConsumers[loggingType]->GetHealthReport();
}

LogFmtStringsContainer MessageDispatcher::GetFmtStrings(LoggingType loggingType) const
{
    return m_chunkConsumers[loggingType]->GetFmtStrings();
}

void MessageDispatcher::ReportDeviceRemoved()
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportDeviceRemoved();
        }
    }
}

void MessageDispatcher::ReportDeviceUp()
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportDeviceDiscovered();
        }
    }
}

void MessageDispatcher::ReportPersistentRecordingTriggered()
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ENABLED)
        {
            cc->ReportPersistentRecordingTriggered();
        }
    }
}

void MessageDispatcher::ReportBufferFound()
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ENABLED)
        {
            cc->ReportBufferFound();
        }
    }
}

// This method is used as a callback during poll if fw state changed to Ready
void MessageDispatcher::ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferAddress, uint32_t bufferSize)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportAndUpdateFwVersion(moduleLevelInfoArray, fwInfo, bufferAddress, bufferSize);
        }
    }
}

void MessageDispatcher::StartNewBuffer(
    const module_level_info* moduleLevelInfoArray,
    uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
    bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
    uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
{
    const TimeStamp ts = Utils::GetCurrentLocalTime();
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->OnStartNewChunk(
                moduleLevelInfoArray, ts, rdPtr, wrPtr, postReadWrPtr, possiblyCoruptedDw,
                firstIteration, deviceRestartDetected, pDwData, realChunkDwords, virtualChunkDwords,
                missedDwords, totalDwords, totalMissedDwords);
        }
    }
}

void MessageDispatcher::ReportCorruptedEntry(unsigned signature)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportCorruptedEntry(signature);
        }
    }
}

void MessageDispatcher::ReportTooManyParameters(unsigned numParams, unsigned maxParamsTh)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportTooManyParameters(numParams, maxParamsTh);
        }
    }
}

void MessageDispatcher::ReportInconsistentWptr(unsigned diffDwords)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportInconsistentWptr(diffDwords);
        }
    }
}

void MessageDispatcher::ReportPmcLogEventTooShort(unsigned payloadLengthDWs)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportPmcLogEventTooShort(payloadLengthDWs);
        }
    }
}

void MessageDispatcher::ReportInconsistentNumParameters(unsigned diffDwords)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->ReportInconsistentNumParameters(diffDwords);
        }
    }
}

void MessageDispatcher::HandleErrorForEnabledConsumers(const string& errMsg)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ENABLED) // error occured on restoring session
        {
            cc->HandleFatalError(errMsg);
        }
    }
}

void MessageDispatcher::HandleError(LoggingType loggingType, const string& errMsg)
{
    m_chunkConsumers[loggingType]->HandleFatalError(errMsg);
}


void MessageDispatcher::EndBuffer(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt)
{
    for (auto &cc : m_chunkConsumers)
    {
        if (cc->GetState() == LoggingState::ACTIVE)
        {
            cc->OnEndChunk(msgCounter, chunkHandlingTime, conversionErrCnt);
        }
    }
}


