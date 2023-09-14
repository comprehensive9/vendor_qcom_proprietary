/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"
#include "ChunkConsumer.h"
#include "CyclicFileRecorder.h"
#include "PostCollectionFileHandler.h"
#include <string>

class XmlRecorderChunkConsumer final : public ChunkConsumer
{
public:
    XmlRecorderChunkConsumer(const std::string &deviceName, CpuType tracerType, bool tsfAvailable);
    ~XmlRecorderChunkConsumer() override;

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
    void ReportDeviceRemoved()override;
    void ReportDeviceDiscovered() override;
    void ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize) override;
    void ConsumeMessage(
        bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
        uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound) override;
    void OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt) override;

    // recording methods
    OperationStatus SplitRecordings() override; // close current file and start a new one.
    OperationStatus Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize) override;
    OperationStatus SetPostCollectionActions(bool compress, bool upload) override;

private:
    CyclicFileRecorder m_cyclicFileRecorder;
    PostCollectionFileHandler m_postCollectionFileHandler;
    Timer m_formatTimer;

    void SetLogFileHeader(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo);
    static std::string CreateFwVerTime(const AtomicFwInfo& fwInfo);
    void FileCloseHandler(std::string &fileName);
    OperationStatus StopImpl() override; // close files
};