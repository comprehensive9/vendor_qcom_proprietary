/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorEvents.h"
#include "Utils.h"

#include <json/json.h>
#include <array>
#include <utility>

using namespace  std;

LogChunkEvent::LogChunkEvent(
    const std::string& deviceName, const CpuType& cpuType, bool tsfAvailable, const module_level_info* moduleLevelInfoArray,
    uint32_t rdPtr, uint32_t wrPtr, bool deviceRestartDetected,
    uint32_t chunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
    : HostManagerEventBase(deviceName)
    , m_cpuType(cpuType)
    , m_tsfAvailable(tsfAvailable)
    , m_thirdPartyFlags()
    , m_rdPtr(rdPtr)
    , m_wrPtr(wrPtr)
    , m_deviceRestartDetected(deviceRestartDetected)
    , m_chunkDwords(chunkDwords)
    , m_missedDwords(missedDwords)
    , m_totalDwords(totalDwords)
    , m_totalMissedDwords(totalMissedDwords)
{
    for (auto i = 0; i < NUM_MODULES; ++i)
    {
        m_thirdPartyFlags[i] = moduleLevelInfoArray[i].third_party_mode;
    }
}

// TODO: Optimize
// 1. Reuse storage
// 2. 1/2 memcopy instead of per message copy
void LogChunkEvent::AppendMessage(uint32_t pmcEventTsf, uint32_t logLineHeader, const int* pPayload, uint32_t payloadSize)
{
    m_content.push_back(logLineHeader);
    m_content.insert(m_content.end(), pPayload, pPayload + payloadSize);
    if (m_tsfAvailable)
    {
        m_content.push_back(pmcEventTsf);
    }
}


void LogChunkEvent::ToJsonInternal(Json::Value& root) const
{
    root["CpuType"] = CpuTypeToString(m_cpuType);

    for (auto flag : m_thirdPartyFlags)
    {
        root["ThirdPartyFlags"].append(flag);
    }

    root["TsfAvailable"] = m_tsfAvailable;
    root["RdPtr"] = m_rdPtr;
    root["WrPtr"] = m_wrPtr;
    root["DeviceRestartDetected"] = m_deviceRestartDetected;
    root["ChunkDwords"] = m_chunkDwords;
    root["MissedDwords"] = m_missedDwords;
    root["TotalDwords"] = m_totalDwords;
    root["TotalMissedDwords"] = m_totalMissedDwords;

    root["Content"] = Utils::Base64Encode(reinterpret_cast<const unsigned char *>(m_content.data()), m_content.size() * sizeof(uint32_t));
}


LogHealthEvent::LogHealthEvent(const std::string& deviceName, ChunkHealthReport fwHealthReport, ChunkHealthReport uCodeHealthReport)
    : HostManagerEventBase(deviceName)
    , m_fwHealthReport(std::move(fwHealthReport))
    , m_uCodeHealthReport(std::move(uCodeHealthReport))
{}

void LogHealthEvent::ToJsonInternal(Json::Value& root) const
{
    // need to divide state and rest data for future proper serialization in DmTools
    // as we do not export state to manifest file
    Json::Value fwReportWrapper;
    fwReportWrapper["State"] = LoggingStateToString(m_fwHealthReport.State);
    m_fwHealthReport.ToJson(fwReportWrapper["HealthData"], false); // report is nested value
    root["FwHealthReport"] = fwReportWrapper;

    Json::Value uCodeReportWrapper;
    uCodeReportWrapper["State"] = LoggingStateToString(m_uCodeHealthReport.State);
    m_uCodeHealthReport.ToJson(uCodeReportWrapper["HealthData"], false); // report is nested value
    root["UCodeHealthReport"] = uCodeReportWrapper;
}
