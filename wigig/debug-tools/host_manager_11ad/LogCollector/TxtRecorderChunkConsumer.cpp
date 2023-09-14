/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "TxtRecorderChunkConsumer.h"
#include "OperationStatus.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "FwStateProvider.h"
#include "LogReaderBase.h"
#include "FieldDescription.h"
#include "Utils.h"
#include <thread>

using namespace std;

TxtRecorderChunkConsumer::TxtRecorderChunkConsumer(const std::string& deviceName, CpuType tracerType, bool tsfAvailable)
    : ChunkConsumer(deviceName, tracerType, tsfAvailable, TxtRecorder)
    , m_cyclicFileRecorder(
        false,
        [this](std::string &fileName) {FileClosedHandler(fileName); },
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation,
        deviceName + "_" + CpuTypeToString(tracerType),
        ".txt",
        deviceName + "_" + CpuTypeToString(tracerType) + " recorder: ",
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxLogFileSize,
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxNumOfLogFiles,
        false)
{
}

TxtRecorderChunkConsumer::~TxtRecorderChunkConsumer()
{
    Stop();
}

void TxtRecorderChunkConsumer::OnStartNewChunk(
    const module_level_info* moduleLevelInfoArray,
    const TimeStamp& ts, uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
    bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
    uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
{
    (void)moduleLevelInfoArray;

    // Currently not in use, should become a part of an activity log
    (void)totalDwords;
    (void)totalMissedDwords;

    OperationStatus os = m_cyclicFileRecorder.ResetFileIfNeeded(); // true means reset happen or reset not needed
    if (!os)
    {
        LOG_ERROR << os.GetStatusMessage() << endl;
        AnalyzeLastOperation(os);
        return;
    }

    m_wallTimeStr = Utils::GetTimeString(ts);
    m_cyclicFileRecorder.ResetPerfCounter();
    m_convertTimer.Reset();
    m_stateProvider.ReportNewChunk(
        rdPtr, wrPtr, postReadWrPtr, possiblyCoruptedDw, firstIteration, deviceRestartDetected, realChunkDwords, virtualChunkDwords, missedDwords);

    if (m_debugMode)
    {
        std::ostringstream debugMsgBuilder;
        debugMsgBuilder << endl << ">>>> Chunk started."
            << " RdPtr: " << rdPtr
            << " WrPtr: " << wrPtr
            << " Post read WrPtr: " << postReadWrPtr
            << " Possibly corrupted DWs: " << possiblyCoruptedDw
            << " Actual chunk size: " << realChunkDwords
            << " Virtual chunk size: " << virtualChunkDwords
            << " Missed DWs: " << missedDwords
            << " Chunk content in DWs: " << MemoryDump(pDwData, realChunkDwords);

        os = m_cyclicFileRecorder.WriteToOutputFile(debugMsgBuilder.str());
        AnalyzeLastOperation(os);
    }

    if (m_tsfAvailable)
    {
        // add wall time in beginning of the chunk, rest of the messages will be displayed with TSF only
        std::ostringstream chunkStartMsgBuilder;
        chunkStartMsgBuilder << "\n>>>> " << m_wallTimeStr << " log chunk start" << "\n";
        os = m_cyclicFileRecorder.WriteToOutputFile(chunkStartMsgBuilder.str());
        AnalyzeLastOperation(os);
        return;
    }

    if (missedDwords > 0 && !LogReaderBase::Tracer3ppLogsFilterOn)
    {
        ostringstream missRateReportBuilder;
        missRateReportBuilder << BufferOverrunMessage << missedDwords << endl;

        // Should replace the previous report when all recording mechanisms are aligned
        //missRateReportBuilder << "\n>>>> Miss Rate report."
        //    << " Chunk dwords: " << chunkDwords
        //    << " Missed dwords: " << missedDwords
        //    << " Total dwords: " << totalDwords
        //    << " Total missed dwords: " << totalMissedDwords
        //    << " Miss Rate: " << Percentage(totalMissedDwords, totalDwords);

        const auto result = m_cyclicFileRecorder.WriteToOutputFile(missRateReportBuilder.str());
        AnalyzeLastOperation(result);
    }

    if (deviceRestartDetected) // fw restart
    {
        const auto result = m_cyclicFileRecorder.WriteToOutputFile(DeviceRestartedMessage);
        AnalyzeLastOperation(result);
    }
}

void TxtRecorderChunkConsumer::ReportCorruptedEntry(unsigned signature)
{
    m_stateProvider.ReportCorruptedEntry();
    ostringstream ss;
    ss << CorruptedEntryMessage << signature << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::ReportTooManyParameters(unsigned numDwords, unsigned numDwordsTh)
{
    m_stateProvider.ReportTooManyParameters();
    ostringstream ss;
    ss << "**** invalid number of parameters provided, actual: " << numDwords << " max val is: " << numDwordsTh << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::ReportInconsistentWptr(unsigned diffDwords)
{
    m_stateProvider.ReportInconsistentWptr();
    ostringstream ss;
    ss << InconsistentWptrMessage << diffDwords << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::ReportPmcLogEventTooShort(unsigned payloadLengthDWs)
{
    m_stateProvider.ReportPmcLogEventTooShort();
    ostringstream ss;
    ss << PmcLogEventTooShortMessage << payloadLengthDWs << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::ReportInconsistentNumParameters(unsigned diffDwords)
{
    m_stateProvider.ReportInconsistentNumParameters();
    ostringstream ss;
    ss << InconsistentNumParamsMessage << diffDwords << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::ReportDeviceRemoved()
{
    if (GetState() != LoggingState::ACTIVE)
    {
        return;
    }
    m_stateProvider.ExportToFile(m_cyclicFileRecorder);
    m_stateProvider.DeactivateSession();

    m_cyclicFileRecorder.WriteToOutputFile(DeviceRemovedMessage);
    m_cyclicFileRecorder.SplitOutputFile();
}

void TxtRecorderChunkConsumer::ReportDeviceDiscovered()
{
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(DeviceDiscoveredMessage);
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    const string newLogFileHeader = SetLogFileHeader(moduleLevelInfoArray, fwInfo);
    m_logTxtParser.SetModuleLevelVerbosity(moduleLevelInfoArray);
    m_cyclicFileRecorder.WriteToOutputFile("Fw/uCode Information was updated \n" + newLogFileHeader);

    m_stateProvider.ReportFwInfo(fwInfo, bufferStartAddress, bufferSize);
}

std::string TxtRecorderChunkConsumer::GetTsfString(uint32_t tsf24Bit, uint32_t tsfClock)
{
    ostringstream tsfStrBuilder;
    tsfStrBuilder << "[" << Hex<6>(tsf24Bit) << "|" << Hex<2>(tsfClock) << "]";
    return tsfStrBuilder.str();
}

void TxtRecorderChunkConsumer::ConsumeMessage(
    bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
    uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound)
{
    const auto& logLineHeader = reinterpret_cast<const log_trace_header&>(messageHeader);

    const unsigned stringOffsetBytes = logLineHeader.string_offset << 2;

    string timeStr = m_wallTimeStr;
    if (m_tsfAvailable)
    {
        const uint32_t tsfClock = FieldDescription::MaskValue(pmcEventTsf, 0, 7);
        const uint32_t tsf24Bit = FieldDescription::MaskValue(pmcEventTsf, 8, 31);
        timeStr = GetTsfString(tsf24Bit, tsfClock);
    }

    bool paramConversionError = false;
    const char* fmtString = NotAvailableFmtString;
    m_convertTimer.Start();


    const std::string parsedLogMessage = m_logTxtParser.ParseLogMessage(
        possiblyCorrupted, stringOffsetBytes, pPayload, payloadSize, logLineHeader.module, logLineHeader.level, timeStr, invalidStringOffsetFound,
        paramConversionError, &fmtString);
    m_convertTimer.Stop();

    if (invalidStringOffsetFound || paramConversionError)
    {
        conversionErrCnt++;
    }

    if (m_debugMode)
    {
        ostringstream debugMsgBuilder;
        debugMsgBuilder << endl << endl << ">>>> Msg:"
            << " ( " << MemoryDump(pDwData, payloadSize + (m_tsfAvailable ? 1U : 0) + 1U, false) << ")" // #DWs + TSF(if available) + header
            << " Params: " << payloadSize
            << " Chunk index: " << chunkOffset
            << " RPtr: " << rdPtr
            << " Fmt offset: " << stringOffsetBytes
            << " Value: '" << fmtString << "'";

        const auto result = m_cyclicFileRecorder.WriteToOutputFile(debugMsgBuilder.str());
        AnalyzeLastOperation(result);
    }
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(parsedLogMessage);
    AnalyzeLastOperation(result);
}

void TxtRecorderChunkConsumer::OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt)
{
    LOG_VERBOSE << "TXT chunk end."
        << " Convert time: " << m_convertTimer
        << " File write time: " << m_cyclicFileRecorder.GetPerfCounter()
        << endl;

    if (Host::GetHost().GetConfiguration().LogCollectorConfiguration.FlushChunk)
    {
        m_cyclicFileRecorder.FlushToFile();
    }
    m_stateProvider.ReportChunkConsumed(msgCounter, chunkHandlingTime, conversionErrCnt);
}

OperationStatus TxtRecorderChunkConsumer::StopImpl()
{
    m_stateProvider.ExportToFile(m_cyclicFileRecorder);
    m_cyclicFileRecorder.Pause();

    return OperationStatus();
}

OperationStatus TxtRecorderChunkConsumer::SplitRecordings()
{
    if (GetState() != LoggingState::ACTIVE)
    {
        return OperationStatus(false, "Splitting files is disabled when TXT log recorder is not active.");
    }
    if (m_postCollectionFileHandler.IsPostStepsConfigured())
    {
        return OperationStatus(false, "Splitting files is disabled when post collection steps are set.");
    }

    const auto ts = Utils::GetCurrentLocalTime();
    m_wallTimeStr = Utils::GetTimeString(ts);

    m_cyclicFileRecorder.SplitOutputFile();
    if (m_tsfAvailable)
    {
        // add wall time in beginning of the file, rest of the messages will be displayed with TSF only
        std::stringstream chunkStartMsg;
        chunkStartMsg << "\n>>>> " << m_wallTimeStr << " log chunk split" << std::endl;
        m_cyclicFileRecorder.WriteToOutputFile(chunkStartMsg.str());
    }

    return OperationStatus();
}

OperationStatus TxtRecorderChunkConsumer::Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    LOG_VERBOSE << "Activating " << *this << endl;

    if (GetState() == LoggingState::ACTIVE)
    {
        return OperationStatus(false, "TxtRecorderChunkConsumer already activated");
    }

    m_logTxtParser.SetModuleLevelVerbosity(moduleLevelInfoArray);

    SetLogFileHeader(moduleLevelInfoArray, fwInfo);

    // new file will be created once first write will happen
    // update the recording path, folder was already created (if needed) by the message dispatcher
    auto os = m_cyclicFileRecorder.SetOutputFilesFolder(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation);
    if (!os)
    {
        AnalyzeLastOperation(os);
        return os;
    }

    m_cyclicFileRecorder.Resume();
    m_stateProvider.ActivateSession(fwInfo, bufferStartAddress, bufferSize);
    return OperationStatus();
}

OperationStatus TxtRecorderChunkConsumer::SetPostCollectionActions(bool compress, bool upload)
{
    return m_postCollectionFileHandler.ConfigurePostCollectionSteps(compress, upload);
}

OperationStatus TxtRecorderChunkConsumer::TestRecordingConditions()
{
    return m_logTxtParser.LogParserInit(string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.ConversionFileLocation).c_str(), m_tracerCpuType);
}

string TxtRecorderChunkConsumer::SetLogFileHeader(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo)
{
    std::ostringstream headerBuilder;

    const FwIdentifier fwIdentifier = fwInfo.FwUniqueId;

    headerBuilder << "Logs for FW version: " << fwIdentifier.m_fwVersion << endl;
    headerBuilder << "FW Compilation Time: "
        << fwIdentifier.m_fwTimestamp << endl;
    headerBuilder << "uCode Compilation Time: "
        << fwIdentifier.m_uCodeTimestamp << endl;

    headerBuilder << "Third party mode: [";
    bool first = true;
    for (int i = 0; i < NUM_MODULES; i++)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            headerBuilder << ", ";
        }
        headerBuilder << static_cast<int>(moduleLevelInfoArray[i].third_party_mode);
    }
    headerBuilder << "]" << endl;
    headerBuilder << "TSF available: " << BoolStr(m_tsfAvailable); // no newline
    m_cyclicFileRecorder.SetFilesHeader(headerBuilder.str());
    return headerBuilder.str();
}

void TxtRecorderChunkConsumer::FileClosedHandler(string &fileName)
{
    thread t = thread(&PostCollectionFileHandler::HandleClosedFile, &m_postCollectionFileHandler, fileName);
    t.detach();
}

LogFmtStringsContainer TxtRecorderChunkConsumer::GetFmtStrings()
{
    // enforce reading string conversion file if recorder is disabled
    if (GetState() == LoggingState::DISABLED)
    {
        TestRecordingConditions(); // init log txt parser
    }

    return m_logTxtParser.GetFmtStrings();
}
