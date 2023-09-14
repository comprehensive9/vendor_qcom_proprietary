/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "XmlRecorderChunkConsumer.h"
#include "OperationStatus.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "FwStateProvider.h"
#include "LogCollectorEvents.h"
#include "LogReaderBase.h"
#include "Utils.h"
#include <thread>

using namespace std;

namespace
{
    string BuildChunkXmlPrefix(
        const TimeStamp &ts, uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCorruptedDws,
        uint32_t realChunkDwords, uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
    {
        // Statistics is currently disabled until all recording mechanisms are aligned
        (void)rdPtr;
        (void)wrPtr;
        (void)postReadWrPtr;
        (void)possiblyCorruptedDws;
        (void)realChunkDwords;
        (void)virtualChunkDwords;
        (void)missedDwords;
        (void)totalDwords;
        (void)totalMissedDwords;

        ostringstream timeStampBuilder;
        timeStampBuilder << "<Log_Content>\n"
            << "<Sample_Time>"
            << "<Hour>" << ts.m_localTime.tm_hour << "</Hour>"
            << "<Minute>" << ts.m_localTime.tm_min << "</Minute>"
            << "<Second>" << ts.m_localTime.tm_sec << "</Second>"
            << "<Milliseconds>" << ts.m_milliseconds << "</Milliseconds>"
            << "<Day>" << ts.m_localTime.tm_mday << "</Day>"
            << "<Month>" << ts.m_localTime.tm_mon + 1 << "</Month>"
            << "<Year>" << ts.m_localTime.tm_year + 1900 << "</Year>"
            << "</Sample_Time>\n"
            /*<< "<RdPtr>" << rdPtr << "</RdPtr>\n"
            << "<WrPtr>" << wrPtr << "</WrPtr>\n"
            << "<PostReadWrPtr>" << wrPtr << "</PostReadWrPtr>\n"
            << "<PossibleCorruptedDws>" << possiblyCorruptedDws << "</PossiblyCorruptedDws>"
            << "<RealChunkDwords>" << realChunkDwords << "</RealChunkDwords>\n"
            << "<VirtualChunkDwords>" << virtualChunkDwords << "</VirtualChunkDwords>\n"
            << "<MissedDwords>" << missedDwords << "</MissedDwords>\n"
            << "<TotalDwords>" << totalDwords<< "</TotalDwords>\n"
            << "<TotalMissedDword>" << totalMissedDwords << "</TotalMissedDword>\n"
            << "<MissRatePercent>" << Percentage(totalMissedDwords, totalDwords, false) << "</MissRatePercent>\n"*/
            << "<Content>\n";

        return timeStampBuilder.str();
    }
}

XmlRecorderChunkConsumer::XmlRecorderChunkConsumer(const std::string& deviceName, CpuType tracerType, bool tsfAvailable)
    : ChunkConsumer(deviceName, tracerType, tsfAvailable, XmlRecorder)
    , m_cyclicFileRecorder(
        false,
        [this](std::string &fileName) {FileCloseHandler(fileName); },
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation,
        deviceName + "_" + CpuTypeToString(tracerType),
        ".log",
        deviceName + "_" + CpuTypeToString(tracerType) + " recorder: ",
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxLogFileSize,
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxNumOfLogFiles,
        false)
{
}

XmlRecorderChunkConsumer::~XmlRecorderChunkConsumer()
{
    Stop();
}

void XmlRecorderChunkConsumer::OnStartNewChunk(
    const module_level_info* moduleLevelInfoArray,
    const TimeStamp& ts, uint32_t rdPtr, uint32_t wrPtr, uint32_t postReadWrPtr, uint32_t possiblyCoruptedDw,
    bool firstIteration, bool deviceRestartDetected, const uint32_t* pDwData, uint32_t realChunkDwords,
    uint32_t virtualChunkDwords, uint32_t missedDwords, uint32_t totalDwords, uint32_t totalMissedDwords)
{
    (void)moduleLevelInfoArray;
    (void)pDwData;
    (void)realChunkDwords;
    OperationStatus os = m_cyclicFileRecorder.ResetFileIfNeeded(); // true means reset happen or reset not needed
    if (!os)
    {
        LOG_ERROR << os.GetStatusMessage() << endl;
        AnalyzeLastOperation(os);
        return;
    }

    m_cyclicFileRecorder.ResetPerfCounter();
    os = m_cyclicFileRecorder.WriteToOutputFile(BuildChunkXmlPrefix(
        ts, rdPtr, wrPtr, postReadWrPtr, possiblyCoruptedDw, realChunkDwords, virtualChunkDwords, missedDwords, totalDwords, totalMissedDwords));
    AnalyzeLastOperation(os);
    m_stateProvider.ReportNewChunk(
        rdPtr, wrPtr, postReadWrPtr, possiblyCoruptedDw, firstIteration, deviceRestartDetected, realChunkDwords,virtualChunkDwords, missedDwords);

    // TODO: Should replace the previous report when all recording mechanisms are aligned, see BuildChunkXmlPrefix
    if (missedDwords > 0 && !LogReaderBase::Tracer3ppLogsFilterOn)
    {
        ostringstream missRateReportBuilder;
        missRateReportBuilder << BufferOverrunMessage << missedDwords << endl;
        os = m_cyclicFileRecorder.WriteToOutputFile(missRateReportBuilder.str());
        AnalyzeLastOperation(os);
    }

    if (deviceRestartDetected) // fw restart
    {
        const auto result = m_cyclicFileRecorder.WriteToOutputFile(DeviceRestartedMessage);
        AnalyzeLastOperation(result);
    }

    m_formatTimer.Reset();
}

void XmlRecorderChunkConsumer::ReportCorruptedEntry(unsigned signature)
{
    m_stateProvider.ReportCorruptedEntry();
    ostringstream ss;
    ss << "**** Got corrupted entry, signature: " << signature << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::ReportTooManyParameters(unsigned numDwords, unsigned numDwordsTh)
{
    m_stateProvider.ReportTooManyParameters();
    ostringstream ss;
    ss << "**** invalid number of parameters provided, actual: " << numDwords << " max val is: " << numDwordsTh << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::ReportInconsistentWptr(unsigned diffDwords)
{
    m_stateProvider.ReportInconsistentWptr();
    ostringstream ss;
    ss << "**** Inconsistent wptr position in the middle of a message, diff (DWORDS): " << diffDwords << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::ReportPmcLogEventTooShort(unsigned payloadLengthDWs)
{
    m_stateProvider.ReportPmcLogEventTooShort();
    ostringstream ss;
    ss << PmcLogEventTooShortMessage << payloadLengthDWs << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::ReportInconsistentNumParameters(unsigned diffDwords)
{
    m_stateProvider.ReportInconsistentNumParameters();
    ostringstream ss;
    ss << InconsistentNumParamsMessage << diffDwords << endl;
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(ss.str());
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::ReportDeviceRemoved()
{
    if (GetState() != LoggingState::ACTIVE)
    {
        return;
    }
    m_stateProvider.ExportToFile(m_cyclicFileRecorder);
    m_stateProvider.DeactivateSession();

    m_cyclicFileRecorder.WriteToOutputFile(DeviceRemovedMessage);
    m_cyclicFileRecorder.WriteToOutputFile("</Logs>\n</LogFile>\n");
    m_cyclicFileRecorder.SplitOutputFile();
}

void XmlRecorderChunkConsumer::ReportDeviceDiscovered()
{
    const auto result = m_cyclicFileRecorder.WriteToOutputFile(DeviceDiscoveredMessage);
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::ReportAndUpdateFwVersion(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    SetLogFileHeader(moduleLevelInfoArray, fwInfo);
    m_cyclicFileRecorder.WriteToOutputFile(CreateFwVerTime(fwInfo));
    m_stateProvider.ReportFwInfo(fwInfo, bufferStartAddress, bufferSize);
}

void XmlRecorderChunkConsumer::ConsumeMessage(
    bool possiblyCorrupted, uint32_t messageHeader, const int* pPayload, uint32_t payloadSize, const uint32_t* pDwData, uint32_t pmcEventTsf,
    uint32_t chunkOffset, uint32_t rdPtr, uint32_t& conversionErrCnt, bool& invalidStringOffsetFound)
{
    (void)possiblyCorrupted;
    (void)pDwData;
    (void)chunkOffset;
    (void)rdPtr;
    (void)conversionErrCnt;
    (void)invalidStringOffsetFound;

    static const char *const levels[] = { "E", "W", "I", "V" };

    // create a string representation of the given log line
    const auto& logLineHeader = reinterpret_cast<const log_trace_header&>(messageHeader);
    const unsigned stringOffsetBytes = logLineHeader.string_offset << 2;

    m_formatTimer.Start();
    ostringstream logLineBuilder;
    if (m_tsfAvailable) // add TSF first when available
    {
        logLineBuilder << pmcEventTsf << ",";
    }
    logLineBuilder << logLineHeader.module << "," << levels[logLineHeader.level] << "," << stringOffsetBytes << ":";

    for (unsigned i = 0U; i < payloadSize; ++i)
    {
        logLineBuilder << pPayload[i];
        if (i < payloadSize - 1)
        {
            logLineBuilder << ",";
        }
    }
    logLineBuilder << endl;
    m_formatTimer.Stop();

    const auto result = m_cyclicFileRecorder.WriteToOutputFile(logLineBuilder.str());
    AnalyzeLastOperation(result);
}

void XmlRecorderChunkConsumer::OnEndChunk(uint32_t msgCounter, double chunkHandlingTime, uint32_t conversionErrCnt)
{
    const auto result = m_cyclicFileRecorder.WriteToOutputFile("</Content>\n</Log_Content>\n");
    AnalyzeLastOperation(result);

    if (Host::GetHost().GetConfiguration().LogCollectorConfiguration.FlushChunk)
    {
        m_cyclicFileRecorder.FlushToFile();
    }

    LOG_VERBOSE << "XML chunk end."
        << " Format time: " << m_formatTimer
        << " File write time: " << m_cyclicFileRecorder.GetPerfCounter() << endl;
    m_stateProvider.ReportChunkConsumed(msgCounter, chunkHandlingTime, conversionErrCnt);
}

OperationStatus XmlRecorderChunkConsumer::StopImpl()
{
    if (GetState() == LoggingState::ACTIVE)
    {
        m_cyclicFileRecorder.WriteToOutputFile("</Logs>\n</LogFile>\n");
    }

    m_stateProvider.ExportToFile(m_cyclicFileRecorder);
    m_cyclicFileRecorder.Pause();

    return OperationStatus();
}

OperationStatus XmlRecorderChunkConsumer::SplitRecordings()
{
    if (GetState() != LoggingState::ACTIVE)
    {
        return OperationStatus(false, "Splitting files is disabled when RAW log recorder is not active");
    }
    if (m_postCollectionFileHandler.IsPostStepsConfigured())
    {
        return OperationStatus(false, "Splitting files is disabled when post collection steps are set");
    }
    const auto ts = Utils::GetCurrentLocalTime();

    m_cyclicFileRecorder.WriteToOutputFile("</Logs>\n</LogFile>\n");
    m_cyclicFileRecorder.SplitOutputFile();

    const auto result = m_cyclicFileRecorder.WriteToOutputFile(BuildChunkXmlPrefix(ts, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    AnalyzeLastOperation(result);
    return OperationStatus();
}

OperationStatus XmlRecorderChunkConsumer::Activate(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo, uint32_t bufferStartAddress, uint32_t bufferSize)
{
    LOG_VERBOSE << "Activating " <<  *this << endl;

    if (GetState() == LoggingState::ACTIVE)
    {
        return OperationStatus(false, "XmlRecorderChunkConsumer already activated");
    }
    SetLogFileHeader(moduleLevelInfoArray, fwInfo);

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

OperationStatus XmlRecorderChunkConsumer::SetPostCollectionActions(bool compress, bool upload)
{
    return m_postCollectionFileHandler.ConfigurePostCollectionSteps(compress, upload);
}

void XmlRecorderChunkConsumer::SetLogFileHeader(const module_level_info* moduleLevelInfoArray, const AtomicFwInfo& fwInfo)
{
    std::ostringstream headerBuilder;
    headerBuilder << "<LogFile>\n"
        << CreateFwVerTime(fwInfo)
        << "<Third_Party_Flags>";
    // make a list of the Third_party_flag for each module.
    for (int i = 0; i < NUM_MODULES; i++)
    {
        headerBuilder << "<Flag><value>" << static_cast<int>(moduleLevelInfoArray[i].third_party_mode) << "</value></Flag>";
    }
    headerBuilder << "</Third_Party_Flags>\n"
        << "<Tsf_Available>" << (m_tsfAvailable ? "true" : "false") << "</Tsf_Available>\n" // do not use formatter to control lower case
        << "<Logs>\n";

    m_cyclicFileRecorder.SetFilesHeader(headerBuilder.str());
}

std::string XmlRecorderChunkConsumer::CreateFwVerTime(const AtomicFwInfo& fwInfo)
{
    std::ostringstream headerBuilder;
    const auto fwIdentifier = fwInfo.FwUniqueId;
    headerBuilder << "<FW_Ver>"
        << fwIdentifier.m_fwVersion
        << "</FW_Ver>\n"
        << "<FwCompilation_Time>"
        << fwIdentifier.m_fwTimestamp
        << "</FwCompilation_Time>\n"
        << "<uCodeCompilation_Time>"
        << fwIdentifier.m_uCodeTimestamp
        << "</uCodeCompilation_Time>\n";
    return headerBuilder.str();
}

void XmlRecorderChunkConsumer::FileCloseHandler(string &fileName)
{
    LOG_DEBUG << "XML file close handler: " << fileName << endl;
    thread t = thread(&PostCollectionFileHandler::HandleClosedFile, &m_postCollectionFileHandler, fileName);
    t.detach();
}