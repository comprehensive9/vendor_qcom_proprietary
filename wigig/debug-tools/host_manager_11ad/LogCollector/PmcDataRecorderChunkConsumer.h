/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "OperationStatus.h"
#include "ChunkConsumer.h"
#include "LogCollectorDefinitions.h"
#include "CyclicFileRecorder.h"
#include <string>

class AtomicFwInfo;
struct TimeStamp;

class PmcDataRecorderChunkConsumer final : public ChunkConsumer
{
public:
    PmcDataRecorderChunkConsumer(const std::string &deviceName, CpuType tracerType, bool tsfAvailable);
    ~PmcDataRecorderChunkConsumer() override;

    void OnStartNewChunk(
        const module_level_info* moduleLevelInfoArray,
        const TimeStamp& ts, uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
        bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
        uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords) override;
    void ReportCorruptedEntry(unsigned signature) override;
    void ReportTooManyParameters(unsigned numDwords, unsigned numDwordsTh) override;
    void ReportInconsistentWptr(unsigned diffDwords) override;
    void ReportPmcLogEventTooShort(unsigned payloadLengthDWs) override;
    void ReportInconsistentNumParameters(unsigned diffDwords) override;
    void ReportDeviceRemoved() override;
    void ReportDeviceDiscovered() override;
    void ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize) override;
    void ConsumeMessage(
        bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
        uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound) override;
    void OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt) override;

    // recording methods
    OperationStatus Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize) override;

private:
    CyclicFileRecorder m_cyclicFileRecorder;

    OperationStatus StopImpl() override; // close files
};
