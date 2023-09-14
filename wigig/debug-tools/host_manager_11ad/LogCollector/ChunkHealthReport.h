/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "LogCollectorDefinitions.h"
#include "FwInfoDefinitions.h"

#include <string>
#include <atomic>
#include <queue>

namespace Json {
    class Value;
}

class ChunkHealthReport
{
public:
    // *** Identification ***
    std::string DeviceName;
    CpuType TraceType;
    FwVersion FWVersion;

    LoggingType LogType;
    LoggingState State;

    // *** Health + statistics ***

    uint32_t InitialReadPtr;
    uint32_t InitialWritePtr;
    uint32_t FinalWritePtr;

    double AvgDataRate;
    double MaxDataRate;
    double AvgMissRate;
    double MaxMissRate;
    double AvgIterationTime;
    double MaxIterationTime;
    double AvgThroughput;
    double MaxThroughput;
    double AvgChunkEfficiencyRate;
    double MaxChunkEfficiencyRate;

    uint32_t PossiblyCorruptedDws;
    uint32_t CorruptedEntryErrorCnt;
    uint32_t TooManyParamsErrorCnt;
    uint32_t InconsistentWptrErrorCnt;
    uint32_t PmcLogEventTooShortErrorCnt;
    uint32_t InconsistentNumParamsErrorCnt;
    std::string FatalError;
    uint32_t ConversionErrorRate;

    uint32_t TotalChunks;
    uint32_t TotalLogDWs;
    uint32_t TotalMsgs;
    std::queue<std::string> FilesQueue;
    uint32_t TotalContentBytes;
    uint32_t TotalFilesNumber;
    double SessionDuration;
    RecordingTrigger RecordTrigger;
    std::string StartRecordingTimestamp;
    std::string StopRecordingTimestamp;

    uint32_t BufferSize;
    uint32_t BufferAddress;

    bool FwUpFound;
    bool FwBufferFound;

    // Serialization
    void ToJson(Json::Value& root, bool isManifest, bool isReduced = false) const;
private:
    static std::string SetPrecision(double number, int precision);
};
