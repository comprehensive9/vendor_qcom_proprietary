/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "OperationStatus.h"
#include "RawPublisherChunkConsumer.h"
#include "DeviceManager.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "FwStateProvider.h"
#include "LogCollectorEvents.h"
#include "LogReaderBase.h"
#include "Utils.h"

using namespace std;

RawPublisherChunkConsumer::RawPublisherChunkConsumer(const std::string& deviceName, CpuType tracerType, bool tsfAvailable)
    : ChunkConsumer(deviceName, tracerType, tsfAvailable, RawPublisher)
{
}

RawPublisherChunkConsumer::~RawPublisherChunkConsumer()
{
    Stop();
}

void RawPublisherChunkConsumer::OnStartNewChunk(
    const module_level_info* moduleLevelInfoArray,
    const TimeStamp& ts, uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
    bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
    uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
{
    (void)ts;
    (void)pDwData;
    (void)realChunkDwords;

    if (LogReaderBase::Tracer3ppLogsFilterOn)
    {
        // Reset all counters for 3PP mode
        virtualChunkDwords = 0;
        missedDwords = 0;
        totalDwords = 0;
        totalMissedDwords = 0;
    }

    m_upLogChunkEvent.reset(new LogChunkEvent(
        m_deviceName, m_tracerCpuType, m_tsfAvailable, moduleLevelInfoArray,
        rdPtr, wrPtr, deviceRestartDetected, virtualChunkDwords, missedDwords, totalDwords, totalMissedDwords));

    m_perfTimer.Reset();
    m_stateProvider.ReportNewChunk(
        rdPtr, wrPtr, postReadWrPtr, possiblyCoruptedDw, firstIteration, deviceRestartDetected, realChunkDwords, virtualChunkDwords, missedDwords);

    LOG_VERBOSE << __FUNCTION__ << ": "
        << " Log publishing for: " << m_deviceName
        << " CPU: " << m_tracerCpuType
        << " RdPtr: " << rdPtr
        << " WPtr: " << wrPtr
        << " Post read WrPtr: " << postReadWrPtr
        << " Possibly corrupted DWs: " << possiblyCoruptedDw
        << " Device restarted: " << BoolStr(deviceRestartDetected)
        << endl;
}

void RawPublisherChunkConsumer::ReportCorruptedEntry(unsigned signature)
{
    (void)signature;
    m_stateProvider.ReportCorruptedEntry();
}

void RawPublisherChunkConsumer::ReportTooManyParameters(unsigned numDwords, unsigned numDwordsTh)
{
    (void)numDwords;
    (void)numDwordsTh;
    m_stateProvider.ReportTooManyParameters();
}

void RawPublisherChunkConsumer::ReportInconsistentWptr(unsigned diffDwords)
{
    (void)diffDwords;
    m_stateProvider.ReportInconsistentWptr();
}

void RawPublisherChunkConsumer::ReportPmcLogEventTooShort(unsigned payloadLengthDWs)
{
    (void)payloadLengthDWs;
    m_stateProvider.ReportPmcLogEventTooShort();
}

void RawPublisherChunkConsumer::ReportInconsistentNumParameters(unsigned diffDwords)
{
    (void)diffDwords;
    m_stateProvider.ReportInconsistentNumParameters();
}

void RawPublisherChunkConsumer::ReportDeviceRemoved()
{
    if (GetState() != LoggingState::ACTIVE)
    {
        return;
    }
    m_stateProvider.DeactivateSession();
}

void RawPublisherChunkConsumer::ReportDeviceDiscovered()
{
}

void RawPublisherChunkConsumer::ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    (void)fwInfo;
    (void)moduleLevelInfoArray;
    m_stateProvider.ReportFwInfo(fwInfo, bufferStartAddress, bufferSize);
}

void RawPublisherChunkConsumer::ConsumeMessage(
    bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
    uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound)
{
    (void)possiblyCorrupted;
    (void)pDwData;
    (void)chunkOffset;
    (void)rdPtr;
    (void)conversionErrCnt;
    (void)invalidStringOffsetFound;

    if (!m_upLogChunkEvent)
    {
        // Program flow bug
        LOG_ERROR << "Reporting message before starting a chunk" << endl;
        return;
    }

    m_perfTimer.Start();
    m_upLogChunkEvent->AppendMessage(pmcEventTsf, messageHeader, pPayload, payloadSize);
    m_perfTimer.Stop();
}

void RawPublisherChunkConsumer::OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt)
{
    if (!m_upLogChunkEvent)
    {
        // Program flow bug
        LOG_ERROR << "End chunk before start" << endl;
        return;
    }

    if (!m_upLogChunkEvent->IsEmpty())
    {
        Timer pushTimer;
        Host::GetHost().PushEvent(*m_upLogChunkEvent);
        pushTimer.Stop();

        LOG_VERBOSE << __FUNCTION__ << ": "
            << " Published for: " << m_deviceName
            << " for cpu: " << m_tracerCpuType
            << " Serialization Time: " << m_perfTimer
            << " Publish Time: " << pushTimer
            << std::endl;
    }
    else
    {
        LOG_VERBOSE << __FUNCTION__ << ": "
            << " No logs to publish for: " << m_deviceName
            << " for cpu: " << m_tracerCpuType
            << std::endl;
    }

    m_stateProvider.ReportChunkConsumed(msgCounter, chunkHandlingTime, conversionErrCnt);
    m_upLogChunkEvent.reset();
}

OperationStatus RawPublisherChunkConsumer::StopImpl()
{
    return OperationStatus();
}

OperationStatus RawPublisherChunkConsumer::Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    LOG_VERBOSE << "Activating " << *this << endl;
    (void)moduleLevelInfoArray;
    (void)fwInfo;

    m_stateProvider.ActivateSession(fwInfo, bufferStartAddress, bufferSize);
    return OperationStatus();
}
