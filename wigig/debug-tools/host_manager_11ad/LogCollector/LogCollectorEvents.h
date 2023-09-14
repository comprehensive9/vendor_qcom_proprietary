/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <array>

#include "EventsDefinitions.h"
#include "LogCollectorDefinitions.h"
#include "ChunkHealthReport.h"


namespace Json { class Value; }

//********************************** LogChunkEvent **************************************//
// Event to be sent upon log chunk consumption
class LogChunkEvent final : public HostManagerEventBase
{
public:
    LogChunkEvent(
        const std::string& deviceName, const CpuType& cpuType, bool tsfAvailable, const module_level_info* moduleLevelInfoArray,
        uint32_t rdPtr, uint32_t wrPtr, bool deviceRestartDetected,
        uint32_t chunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords);

    bool IsEmpty() const { return m_content.empty(); }
    void AppendMessage(uint32_t pmcEventTsf, uint32_t logLineHeader, const int* pPayload, uint32_t payloadSize);

private:
    const CpuType m_cpuType;
    const bool m_tsfAvailable;
    std::array<uint8_t, NUM_MODULES> m_thirdPartyFlags;
    const uint32_t m_rdPtr;
    const uint32_t m_wrPtr;
    const bool m_deviceRestartDetected;
    const uint32_t m_chunkDwords;
    const uint32_t m_missedDwords;
    const uint32_t m_totalDwords;
    const uint32_t m_totalMissedDwords;

    std::vector<uint32_t> m_content;

    const char* GetTypeName() const override { return "LogChunkEvent"; }
    void ToJsonInternal(Json::Value& root) const override;
};

//********************************** LogHealthEvent **************************************//
// Event to be sent upon device removal to report health and statistics
class LogHealthEvent final : public HostManagerEventBase
{
public:
    LogHealthEvent(const std::string& deviceName, ChunkHealthReport fwHealthReport, ChunkHealthReport uCodeHealthReport);

private:
    ChunkHealthReport m_fwHealthReport;
    ChunkHealthReport m_uCodeHealthReport;

    const char* GetTypeName() const override { return "LogHealthEvent"; }

    void ToJsonInternal(Json::Value& root) const override;
};

// =================================================================================== //
