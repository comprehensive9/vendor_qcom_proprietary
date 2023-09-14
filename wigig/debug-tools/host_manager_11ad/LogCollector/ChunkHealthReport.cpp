/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "ChunkHealthReport.h"
#include "Host.h"
#include "DebugLogger.h"
#include "PersistentConfiguration.h"
#include "HostInfo.h"

void ChunkHealthReport::ToJson(Json::Value& root, bool isManifest, bool isReduced) const
{
    // reduced health request upon onTarget stop recording
    // provide avg miss rate and fatal errors (per recorder) if occured
    if (isReduced)
    {
        if (State == LoggingState::DISABLED)
        {
            return;
        }

        if (State == LoggingState::ERROR_OCCURED)
        {
            std::ostringstream keyBuilder;
            keyBuilder << (TraceType == CPU_TYPE_FW ? "Fw" : "uCode") << LogType << "FatalError";
            root[keyBuilder.str()] = FatalError;
        }

        std::ostringstream missRateKeyBuilder;
        missRateKeyBuilder << (TraceType == CPU_TYPE_FW ? "Fw" : "uCode") << "AvgMissRatePct";
        root[missRateKeyBuilder.str()] = SetPrecision(AvgMissRate * 100, 1); // to %
        return;
    }

    if (isManifest) // export to manifest file
    {
        // provide data captured
        Json::Value files(Json::arrayValue);
        auto filesQueueTmp = FilesQueue; // method should be const
        while(!filesQueueTmp.empty())
        {
            files.append(filesQueueTmp.front());
            filesQueueTmp.pop();
        }
        root["CapturedLogFiles"] = files;
        root["TotalLogContentMB"] = SetPrecision(static_cast<double>(TotalContentBytes) / 1024 / 1024, 1); // from bytes to MB
        root["TotalLogFilesNumber"] = TotalFilesNumber;
        root["StopRecordingTimestamp"] = StopRecordingTimestamp;
    }
    else // onTarget health request
    {
        root["State"] = LoggingStateToString(State);
        if (State == LoggingState::DISABLED)
        {
            return;
        }
        // provide path to manifest files directory
        root["ManifestFilesDirectory"] = std::string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation);
    }

    std::ostringstream oss;
    oss << Address(BufferAddress);
    root["BufferAddress"] = oss.str();
    oss.str("");

    oss << FWVersion;
    root["FwVersion"] = oss.str();

    root["Recorder"] = LoggingTypeToString(LogType);
    root["RecordingTrigger"] = RecordingTriggerToString(RecordTrigger);
    root["StartRecordingTimestamp"] = StartRecordingTimestamp;
    root["Host"] = Host::GetHost().GetHostInfo().GetHostName();
    root["Device"] = DeviceName;
    root["CpuType"] = CpuTypeToString(TraceType);

    root["FwBufferFound"] = FwBufferFound;
    root["FwUpFound"] = FwUpFound;

    root["AvgDataRateKBSec"] = SetPrecision(AvgDataRate, 1);
    root["MaxDataRateKBSec"] = SetPrecision(MaxDataRate, 1);
    root["AvgMissRatePct"] = SetPrecision(AvgMissRate * 100, 2); // to %
    root["MaxMissRatePct"] = SetPrecision(MaxMissRate * 100, 2); // to %
    root["AvgMessagesPerSec"] = SetPrecision(AvgThroughput * 1000, 0); // msec --> sec
    root["MaxMessagesPerSec"] = SetPrecision(MaxThroughput * 1000, 0); // msec --> sec
    root["AvgChunkEfficiencyRatePct"] = SetPrecision(AvgChunkEfficiencyRate * 100, 1);
    root["MaxChunkEfficiencyRatePct"] = SetPrecision(MaxChunkEfficiencyRate * 100, 1);
    root["AvgIterationTimeMs"] = SetPrecision(AvgIterationTime, 2);
    root["MaxIterationTimeMs"] = SetPrecision(MaxIterationTime, 2);

    root["PossiblyCorruptedDws"] = PossiblyCorruptedDws;
    root["CorruptedEntryErrorCnt"] = CorruptedEntryErrorCnt;
    root["TooManyParamsErrorCnt"] = TooManyParamsErrorCnt;
    root["InconsistentWptrErrorCnt"] = InconsistentWptrErrorCnt;
    root["PmcLogEventTooShortErrorCnt"] = PmcLogEventTooShortErrorCnt;
    root["InconsistentNumParamsErrorCnt"] = InconsistentNumParamsErrorCnt;

    root["TotalMessages"] = TotalMsgs;
    root["TotalChunks"] = TotalChunks;
    root["TotalDWs"] = TotalLogDWs;

    root["FatalError"] = FatalError.empty() ? "None" : FatalError;
    root["ConversionErrorRatePct"] = SetPrecision(ConversionErrorRate * 100, 1);

    root["InitialReadPtr"] = InitialReadPtr;
    root["InitialWritePtr"] = InitialWritePtr;
    root["FinalWritePtr"] = FinalWritePtr;
    root["RecordingDurationMin"] = SetPrecision(SessionDuration / 1000 / 60, 1); // msec --> min
    root["BufferSizeDW"] = BufferSize;

    root["PollingIntervalMs"] = uint32_t(Host::GetHost().GetConfiguration().LogCollectorConfiguration.PollingIntervalMs);
    root["MaxSingleFileSizeMb"] = uint32_t(Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxLogFileSize);
    root["MaxNumberOfLogFiles"] = uint32_t(Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxNumOfLogFiles);
    root["FlushChunk"] = static_cast<bool>(Host::GetHost().GetConfiguration().LogCollectorConfiguration.FlushChunk);
    root["ConversionFilePath"] = std::string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.ConversionFileLocation);
    root["TargetServer"] = std::string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetServer);
    root["UserName"] = std::string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.UserName);
    root["RemotePath"] = std::string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.RemotePath);
}

std::string ChunkHealthReport::SetPrecision(double number, int precision)
{
    std::ostringstream oss;
    oss.unsetf(std::ios_base::floatfield);
    oss << std::fixed <<std::setprecision(precision) << number;
    return oss.str();
}
