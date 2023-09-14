/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "RingReader.h"
#include "MessageDispatcher.h"
#include "Device.h"
#include "FwStateProvider.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "DebugLogger.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorModuleVerbosity.h"
#include "CyclicFileRecorder.h"
#include "PmcCommandHandler/PmcContext.h"
#include "PmcCommandHandler/PmcSequenceBase.h"

#include <sstream>
#include <string>

using namespace std;

namespace
{
    enum { FwLogPmcEventType = 0xDB, UcodeLogPmcEventType = 0xFB };

    struct PmcEventHeader
    {
        uint16_t Delimeter;
        uint8_t Length;
        uint8_t Type;
    };

    struct PmcLogEvent
    {
        PmcEventHeader EventHeader;
        uint32_t TsfDword;
        log_trace_header MessageHeader;
        int Params[];
    };

    struct PmcDataEvent
    {
        uint32_t EventHeader; // use uint32_t instead of PmcEventHeader for convenience
        int Params[];
    };
}

RingReader::RingReader(const string& deviceName,
    uint32_t logBufferInfoAddressFw, int ahbToLinkerDeltaFw,
    uint32_t logBufferInfoAddressUc, int ahbToLinkerDeltaUc)
    : LogReaderBase(deviceName, deviceName + "_PMC_tracer: ", "PMC_log_" + deviceName)
    , m_logBufferInfo{ { LogBufferInfo(CPU_TYPE_FW, logBufferInfoAddressFw, ahbToLinkerDeltaFw),
                       LogBufferInfo(CPU_TYPE_UCODE, logBufferInfoAddressUc, ahbToLinkerDeltaUc) } }
    , m_residualOffsetBytes(0U)
    , m_pmcGetDataFunc(nullptr)
{
    m_logMessageDispatcherFw.reset(new MessageDispatcher(m_deviceName, CPU_TYPE_FW, true /*TSF available*/));
    m_logMessageDispatcherUcode.reset(new MessageDispatcher(m_deviceName, CPU_TYPE_UCODE, true /*TSF available*/));
    m_dataMessageDispatcher.reset(new MessageDispatcher(m_deviceName, CPU_TYPE_FW /*dummy*/, true /*TSF available*/));

    LOG_DEBUG << m_debugLogPrefix << "created with FW logBufferInfoAddress=" << Hex<8>(m_logBufferInfo[CPU_TYPE_FW].LogBufferInfoAddress)
        << ", FW ahbToLinkerDelta=" << Hex<8>(m_logBufferInfo[CPU_TYPE_FW].AhbToLinkerDelta)
        << ", uCode logBufferInfoAddress=" << Hex<8>(m_logBufferInfo[CPU_TYPE_UCODE].LogBufferInfoAddress)
        << ", uCode ahbToLinkerDelta=" << Hex<8>(m_logBufferInfo[CPU_TYPE_UCODE].AhbToLinkerDelta) << endl;

    if (Host::GetHost().GetConfiguration().PmcConfiguration.DumpRawDataForDebug)
    {
        // PMC persistent config cannot be updated online, so it's fine to create the recorder
        m_cyclicDebugFileRecorder.reset(new CyclicFileRecorder(
            true /*auto-split*/,
            Host::GetHost().GetConfiguration().PmcConfiguration.NormalizedTargetLocation,
            deviceName + "_raw_pmc",
            ".txt",
            deviceName + "_raw_pmc_dumper: ",
            Host::GetHost().GetConfiguration().PmcConfiguration.MaxDataFileSize,
            Host::GetHost().GetConfiguration().PmcConfiguration.MaxNumOfDataFiles,
            false));
    }
}

bool RingReader::IsLogBufferAvailable(CpuType cpuType) const
{
    return m_logBufferInfo[cpuType].LogBufferInfoAddress != Utils::REGISTER_DEFAULT_VALUE;
}

bool RingReader::IsPollRequired() const
{
    // polling required if any of the consumers is active
    return m_logMessageDispatcherFw->IsPollRequired()
        || m_logMessageDispatcherUcode->IsPollRequired()
        || m_dataMessageDispatcher->IsPollRequired();
}

OperationStatus RingReader::UpdateDeviceInfo(const Device& device)
{
    // USAGE 1/2 registers are updated asynchronously, we need to wait for both to be initialized
    // in order safely access the related verbosity arrays
    OperationStatus os = ComputeLogBufferStartAddress(device, m_logBufferInfo[CPU_TYPE_FW]);
    if (!os)
    {
        os.AddPrefix("[FW] ", false /*no separator*/);
        return os;
    }

    os = ComputeLogBufferStartAddress(device, m_logBufferInfo[CPU_TYPE_UCODE]);
    if (!os)
    {
        os.AddPrefix("[uCode] ", false /*no separator*/);
        return os;
    }

    return OperationStatus::Merge(
        GetModuleInfoFromDevice(device, m_logBufferInfo[CPU_TYPE_FW]),
        GetModuleInfoFromDevice(device, m_logBufferInfo[CPU_TYPE_UCODE]));
}

OperationStatus RingReader::UpdateRingInfo(const Device& device)
{
    const auto pPmcRingSpec = device.GetPmcContext().GetSequence().GetPmcRingSpec(true /*recalculate*/);
    if (!pPmcRingSpec)
    {
        return OperationStatus(false, "unknown PMC Ring spec.");
    }

    if (pPmcRingSpec->PmcMode == PmcModeE::LegacyPmcMode)
    {
        return OperationStatus(false, "legacy PMC mode is unexpected");
    }

    m_pmcGetDataFunc = pPmcRingSpec->PmcMode == PmcModeE::HostPmcMode
        ? &RingReader::PmcGetDataHostMode
        : &RingReader::PmcGetDataFwMode;

    return OperationStatus();
}

void RingReader::ActivateConsumersIfRequired(const AtomicFwInfo& fwInfo)
{
    m_logMessageDispatcherFw->ActivateConsumersIfRequired(m_logBufferInfo[CPU_TYPE_FW].ModuleLevelInfo, fwInfo, m_logBufferInfo[CPU_TYPE_FW].LogAddressAhb, LogBufferContentDwords());
    m_logMessageDispatcherUcode->ActivateConsumersIfRequired(m_logBufferInfo[CPU_TYPE_UCODE].ModuleLevelInfo, fwInfo, m_logBufferInfo[CPU_TYPE_UCODE].LogAddressAhb, LogBufferContentDwords());
    m_dataMessageDispatcher->ActivateConsumersIfRequired(m_logBufferInfo[CPU_TYPE_FW].ModuleLevelInfo, fwInfo, m_logBufferInfo[CPU_TYPE_FW].LogAddressAhb, LogBufferContentDwords()); // info doesn't matter here
}

void RingReader::ReportAndUpdateFwVersion(const AtomicFwInfo& fwInfo)
{
    m_logMessageDispatcherFw->ReportAndUpdateFwVersion(m_logBufferInfo[CPU_TYPE_FW].ModuleLevelInfo, fwInfo, m_logBufferInfo[CPU_TYPE_FW].LogAddressAhb, LogBufferContentDwords());
    m_logMessageDispatcherUcode->ReportAndUpdateFwVersion(m_logBufferInfo[CPU_TYPE_UCODE].ModuleLevelInfo, fwInfo, m_logBufferInfo[CPU_TYPE_UCODE].LogAddressAhb, LogBufferContentDwords());
    m_dataMessageDispatcher->ReportAndUpdateFwVersion(m_logBufferInfo[CPU_TYPE_FW].ModuleLevelInfo, fwInfo, m_logBufferInfo[CPU_TYPE_FW].LogAddressAhb, LogBufferContentDwords());
}

void RingReader::ReportBufferFound()
{
    m_logMessageDispatcherFw->ReportBufferFound();
    m_logMessageDispatcherUcode->ReportBufferFound();
    m_dataMessageDispatcher->ReportBufferFound();
}

OperationStatus RingReader::ReadAndConsumeBuffer(const Device& device)
{
    // Note:
    // When zero buffer length is passed as PmcGetData command argument, total amount of data available
    // in the ring is returned through QCA_WLAN_VENDOR_ATTR_PMC_DATA_LENGTH attribute.
    // Currently unused.

    bool hasMoreData = false;

    do
    {
        uint32_t bufLenBytesInOut = static_cast<uint32_t>(m_logBufferContent.size());    // Including residual space
        uint8_t* pBuf = m_logBufferContent.data() + m_residualOffsetBytes; // residual is zero or as updated in the last iteration

        OperationStatus os = (this->*m_pmcGetDataFunc)(device, bufLenBytesInOut, pBuf, hasMoreData);
        if (!os)
        {
            return os;
        }
        // otherwise, bufLenBytesInOut bytes were copied to the buffer and hasMoreData updated

        LOG_VERBOSE << m_debugLogPrefix << "retrieved PMC data"
            << ", payload length: " << bufLenBytesInOut
            << " (B), has more data: " << BoolStr(hasMoreData)
            << endl;

        const auto logBufferLenBytes = m_residualOffsetBytes + bufLenBytesInOut;

        // write to debug file (when turned on) event if no data available to mark chunk entry
        // pass beginning of the buffer to display the residual part from previous iteration
        DumpRawDataForDebugIfNeeded(m_logBufferContent.data(), logBufferLenBytes);

        if (bufLenBytesInOut == 0U)
        {
            // no data available
            return OperationStatus();
        }

        // consume new buffer, updates the residual length if exists
        ConsumeBuffer(logBufferLenBytes);

        if (m_residualOffsetBytes > GetMaxResidualSizeInBytes())
        {
            LOG_WARNING << m_debugLogPrefix << "failed to process PMC event, residual length " << m_residualOffsetBytes
                << " Bytes is bigger that maximum allowed " << GetMaxResidualSizeInBytes() << endl;

            // no room for residual part of the last event, ignore it
            // not a failure, continue if more data available
            m_residualOffsetBytes = 0U;
        }

        // copy residual to beginning of the buffer
        if (m_residualOffsetBytes > 0)
        {
            auto currentEndIter = m_logBufferContent.begin() + logBufferLenBytes;
            std::copy(currentEndIter - m_residualOffsetBytes, currentEndIter, m_logBufferContent.begin());
        }
    }
    while (hasMoreData);

    return OperationStatus();
}

void RingReader::ConsumeBuffer(uint32_t logBufferLenBytes)
{
    static const uint16_t s_pmcEventDelimeter = 0xcafe;

    // |last residual|new payload[|x unused]|
    // called only if the payload part is not empty

    // Event layout:
    //     |31    24|23    16|15             0|
    //     +--------+--------+----------------+
    // DW0 | Type   | Length | Delimiter      |
    //     +--------+--------+----------------+
    // DW1 |                                  |
    // ... |            Payload               |
    // DWn |                                  |
    //     +----------------------------------+
    //
    // +------------+-----------+-------+-------------------------------------------------------------+
    // | Field Name | DWORD     | Range | Description                                                 |
    // +------------+-----------+-------+-------------------------------------------------------------+
    // | Delimiter  | DW0       | 0:15  | A configured value of event delimiter (Hardcoded to 0xcafe) |
    // | Length     | DW0       | 16:23 | The number of bytes in payload. The payload is padded to DW |
    // | Type       | DW0       | 24:31 | The type of the event                                       |
    // | Payload    | DW1 - DWn | 0:31  | The event payload according to the event type               |
    // +------------+-----------+-------+-------------------------------------------------------------+

    m_residualOffsetBytes = 0U;

    Timer chunkHandlingTimer;

    const auto pLogBufferDWs = reinterpret_cast<const uint32_t*>(m_logBufferContent.data());
    const auto pLogBufferHeaders = reinterpret_cast<const PmcEventHeader*>(m_logBufferContent.data());
    const auto logBufferLenDwords = static_cast<uint32_t>(logBufferLenBytes / sizeof(uint32_t)); // assume DW-aligned

    m_logMessageDispatcherFw->StartNewBuffer(
        m_logBufferInfo[CPU_TYPE_FW].ModuleLevelInfo, 0, 0, 0, 0, false, false,
        pLogBufferDWs, logBufferLenDwords, logBufferLenDwords, 0, 0, 0);
    m_logMessageDispatcherUcode->StartNewBuffer(
        m_logBufferInfo[CPU_TYPE_UCODE].ModuleLevelInfo, 0, 0, 0, 0, false, false,
        pLogBufferDWs, logBufferLenDwords, logBufferLenDwords, 0, 0, 0);
    m_dataMessageDispatcher->StartNewBuffer(
        m_logBufferInfo[CPU_TYPE_FW].ModuleLevelInfo, 0, 0, 0, 0, false, false,
        pLogBufferDWs, logBufferLenDwords, logBufferLenDwords, 0, 0, 0);

    uint32_t missedDwords = 0U;
    uint32_t currentDwordInd = 0U;
    uint32_t msgCounterFw = 0, msgCounterUc = 0, dataEventCounter = 0;
    uint32_t conversionErrCnt = 0;
    while (currentDwordInd < logBufferLenDwords)
    {
        // search for the next event header
        if (pLogBufferHeaders[currentDwordInd].Delimeter != s_pmcEventDelimeter)
        {
            ++missedDwords;
            ++currentDwordInd;
            continue;
        }

        const auto eventHeader = pLogBufferHeaders[currentDwordInd];
        const uint32_t payloadLengthDWs = eventHeader.Length / sizeof(uint32_t); // does not account for header

        // check event integrity
        if (currentDwordInd + payloadLengthDWs >= logBufferLenDwords)
        {
            m_residualOffsetBytes = (logBufferLenDwords - currentDwordInd) * sizeof(uint32_t);
            LOG_VERBOSE << m_debugLogPrefix << "event truncated, logBufferLenBytes: " << logBufferLenBytes
                << ", residualOffsetBytes: " << m_residualOffsetBytes << endl;
            break;
        }

        const uint32_t* pEventBase = pLogBufferDWs + currentDwordInd; // pointer to the event header
        if (eventHeader.Type == FwLogPmcEventType)
        {
            ConsumePmcLogEvent(m_logMessageDispatcherFw.get(), m_logBufferInfo[CPU_TYPE_FW], pEventBase, payloadLengthDWs, currentDwordInd, conversionErrCnt);
            ++msgCounterFw;

        }
        else if (eventHeader.Type == UcodeLogPmcEventType)
        {
            ConsumePmcLogEvent(m_logMessageDispatcherUcode.get(), m_logBufferInfo[CPU_TYPE_UCODE], pEventBase, payloadLengthDWs, currentDwordInd, conversionErrCnt);
            ++msgCounterUc;
        }
        else // PMC Data event
        {
            ConsumePmcDataEvent(pEventBase, payloadLengthDWs, conversionErrCnt);
            ++dataEventCounter;
        }

        currentDwordInd += (1U + payloadLengthDWs);
    }

    // accumulate total processed DWs
    // if we get here, current index is the next header (or past the end of buffer)
    m_totalLogDwords += currentDwordInd;

    chunkHandlingTimer.Stop();

    m_logMessageDispatcherFw->EndBuffer(msgCounterFw, chunkHandlingTimer.GetTotalMsec(), conversionErrCnt);
    m_logMessageDispatcherUcode->EndBuffer(msgCounterUc, chunkHandlingTimer.GetTotalMsec(), conversionErrCnt);
    m_dataMessageDispatcher->EndBuffer(dataEventCounter, chunkHandlingTimer.GetTotalMsec(), conversionErrCnt);

    if (missedDwords > 0)
    {
        m_totalMissedDwords += missedDwords;

        if (m_cyclicDebugFileRecorder) // meaning DumpRawDataForDebug is On
        {
            ostringstream oss;
            oss << ">>>> " << m_debugLogPrefix << "chunk consuming info " << endl;
            oss << " Buffer length (DW): " << logBufferLenDwords << endl;
            oss << " Missed: " << missedDwords << endl;
            oss << " Handling time:" << chunkHandlingTimer << endl;
            oss << ">>>>" << endl;
            m_cyclicDebugFileRecorder->WriteToOutputFile(oss.str());
        }
    }
}

void RingReader::ConsumePmcLogEvent(
    MessageDispatcher* pMessageDispatcher, const LogBufferInfo& logBufferInfo,
    const uint32_t* pLogEventBase, uint32_t payloadLengthDWs, uint32_t chunkOffset, uint32_t& conversionErrCnt)
{
   // Only relevant PMC events are passed
   // Given event base points to the event header
   // Given message size accounts for event payload only, i.e. TSF, signature and parameters
   // Event payload (DW0 is the event header):
   // +------------+-----------+-------+--------------------------------------------------------------------------------+
   // | Field Name | DWORD     | Range | Description                                                                    |
   // +------------+-----------+-------+--------------------------------------------------------------------------------+
   // | TSF Clock  | DW1       | 0:7   | The TSF Clock per µsec Value                                                   |
   // | TSF        | DW1       | 8:31  | The TSF[23:0] captured value of the received frame (when the SGD is detected), |
   // |            |           |       | or the Captured Transmit frame when Start  Preamble is sent for transmission   |
   // | Signature  | DW2       | 0:31  | Log line header                                                                |
   // | Parameters | DW3 - DWn | 0:31  | The log line parameters                                                        |
   // +------------+-----------+-------+--------------------------------------------------------------------------------+

    if (payloadLengthDWs < 2U) // at least TSF & log line header are expected
    {
        pMessageDispatcher->ReportPmcLogEventTooShort(payloadLengthDWs);
        return;
    }

    const auto* pPmcLogEvent = reinterpret_cast<const PmcLogEvent*>(pLogEventBase);
    const auto& logLineHeader = pPmcLogEvent->MessageHeader;

    // check for corrupted signature
    if (logLineHeader.signature != LogLineHeaderSignature)
    {
        if (!Tracer3ppLogsFilterOn)
        {
            pMessageDispatcher->ReportCorruptedEntry(logLineHeader.signature);
        }
        return;
    }

    if (!Tracer3ppLogsFilterOn || logBufferInfo.ModuleLevelInfo[logLineHeader.module].third_party_mode == 1)
    {
        // consume log event and dispatch to active clients
        const unsigned numOfDwords = 4 * logLineHeader.dword_num_msb + logLineHeader.dword_num_lsb;

        if (numOfDwords > (MaxParams * 2))
        {
            pMessageDispatcher->ReportTooManyParameters(numOfDwords, (MaxParams * 2));
            return;
        }

        if (2U + numOfDwords > payloadLengthDWs) // TSF + signature + #DWs
        {
            // Number of log message parameters exceed log event length
            pMessageDispatcher->ReportInconsistentNumParameters(2U + numOfDwords - payloadLengthDWs);
            return;
        }

        // Dummy variable, not in use
        bool dummyFlag = false;

        // dispatch to clients: log line header, parameters (starting from DW3) and the extracted TSF
        pMessageDispatcher->DispatchMessage(false, reinterpret_cast<const uint32_t&>(pPmcLogEvent->MessageHeader),
            pPmcLogEvent->Params, numOfDwords, pLogEventBase, chunkOffset, m_rdPtr, conversionErrCnt, dummyFlag, pPmcLogEvent->TsfDword);
    }
}

void RingReader::ConsumePmcDataEvent(const uint32_t* pLogEventBase, uint32_t payloadLengthDWs, uint32_t& conversionErrCnt) const
{
    // Only relevant PMC events are passed
    // Given event base points to the event header
    // Given message size accounts for event payload only, i.e. without event header

    // Dummy variable, not in use
    bool dummyFlag = false;

    // just dispatch to the client: event header and payload (TSF extraction is not required)
    const auto* pPmcDataEvent = reinterpret_cast<const PmcDataEvent*>(pLogEventBase);
    m_dataMessageDispatcher->DispatchMessage(
        false, pPmcDataEvent->EventHeader, pPmcDataEvent->Params, payloadLengthDWs, pLogEventBase, 0, 0,
        conversionErrCnt, dummyFlag);
}

void RingReader::DumpRawDataForDebugIfNeeded(const uint8_t* pBuf, uint32_t bufLenBytes) const
{
    if (!(Host::GetHost().GetConfiguration().PmcConfiguration.DumpRawDataForDebug
        && m_cyclicDebugFileRecorder))
    {
        return;
    }

    ostringstream oss;
    oss << ">>>> " << Utils::GetTimeString(Utils::GetCurrentLocalTime()) << " chunk start" << endl;
    oss << " Residual length (B): " << m_residualOffsetBytes << endl;
    oss << " Chunk length including residual (B): " << bufLenBytes; // no endline, added by MemoryDump
    if (pBuf && bufLenBytes > 0U)
    {
        const auto pDwData = reinterpret_cast<const uint32_t*>(pBuf);
        const uint32_t numDws = bufLenBytes / sizeof(uint32_t);
        oss << MemoryDump(pDwData, numDws) << endl;
    }
    oss << ">>>> chunk end" << endl;

    m_cyclicDebugFileRecorder->WriteToOutputFile(oss.str());
}

void RingReader::ReportDeviceRemoved()
{
    if (m_valid)
    {
        LOG_DEBUG << m_debugLogPrefix << "device removal reported, going to reset Log reader and close recording files" << endl;
        m_logMessageDispatcherFw->ReportDeviceRemoved();
        m_logMessageDispatcherUcode->ReportDeviceRemoved();
        m_dataMessageDispatcher->ReportDeviceRemoved();
        ResetState();

        if (m_cyclicDebugFileRecorder)
        {
            m_cyclicDebugFileRecorder->SplitOutputFile();
        }
    }
}

OperationStatus RingReader::EnforceModuleVerbosity(CpuType cpuType)
{
    return EnforceModuleVerbosityImpl(m_enforcedVerbosity[cpuType], m_logBufferInfo[cpuType]);
}

OperationStatus RingReader::SetModuleVerbosity(CpuType cpuType, const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    m_enforcedVerbosity[cpuType] = enforcedVerbosity;
    LOG_INFO << "Set enforced verbosity for " << cpuType << ": " << m_enforcedVerbosity[cpuType] << endl;

    return EnforceModuleVerbosity(cpuType);
}

OperationStatus RingReader::GetModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
{
    return GetModuleVerbosityImpl(m_logBufferInfo[cpuType], moduleVerbosity);
}

OperationStatus RingReader::GetEnforcedModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity> &moduleVerbosity) const
{
    moduleVerbosity = m_enforcedVerbosity[cpuType];
    return OperationStatus();
}

OperationStatus RingReader::ActivateLogging(
    CpuType cpuType, LoggingType loggingType, bool ignoreLogHistory, bool compress, bool upload,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity, RecordingTrigger recordingTrigger, bool debugMode)
{
    return ActivateLoggingImpl(*SelectMessageDispatcher(cpuType, loggingType),
        cpuType, loggingType, ignoreLogHistory, compress, upload, enforcedVerbosity, recordingTrigger, debugMode);
}

// Remark: cannot be done per CPU to have single poller activation
OperationStatus RingReader::RestoreLogging()
{
    // should be used only on restoring session - device was already running
    LOG_DEBUG << m_debugLogPrefix << "Restore logging state" << endl;

    bool activationRequired = false;

    auto os1 = OperationStatus();
    if (m_logMessageDispatcherFw->IsActivationRequired()) // at least one consumer is enabled
    {
        if (IsLogBufferAvailable(CPU_TYPE_FW))
        {
            activationRequired = true;
            EnforceModuleVerbosity(CPU_TYPE_FW);
            m_logMessageDispatcherFw->ReportPersistentRecordingTriggered();
        }
        else
        {
            const string errMsg = "FW Log buffer address unavailable";
            m_logMessageDispatcherFw->HandleErrorForEnabledConsumers(errMsg);
            os1 = OperationStatus(false, errMsg);
        }
    }

    auto os2 = OperationStatus();
    if (m_logMessageDispatcherUcode->IsActivationRequired())
    {
        if (IsLogBufferAvailable(CPU_TYPE_UCODE))
        {
            activationRequired = true;
            EnforceModuleVerbosity(CPU_TYPE_UCODE);
            m_logMessageDispatcherUcode->ReportPersistentRecordingTriggered();
        }
        else
        {
            const string errMsg = "UCODE Log buffer address unavailable";
            m_logMessageDispatcherUcode->HandleErrorForEnabledConsumers(errMsg);
            os2 = OperationStatus(false, errMsg);
        }
    }

    if (m_dataMessageDispatcher->IsActivationRequired())
    {
        activationRequired = true;
        m_dataMessageDispatcher->ReportPersistentRecordingTriggered();
        // log message actions are irrelevant here
        // no failure, operation status is not collected
    }

    auto os = OperationStatus::Merge(os1, os2);
    if (activationRequired && os)
    {
        LOG_INFO << m_debugLogPrefix << "activation required, resuming log poller" << endl;
        return RegisterPoller();
    }

    return os;
}

OperationStatus RingReader::DeactivateLogging(CpuType cpuType, LoggingType loggingType)
{
    LOG_INFO << m_debugLogPrefix << "Logging deactivation requested for " << cpuType << " " << loggingType << endl;

    // closes the file for recording consumers, returns target directory
    auto os = SelectMessageDispatcher(cpuType, loggingType)->StopConsumer(loggingType);
    // clear enforced verbosity for log consumers
    if (loggingType != LoggingType::PmcDataRecorder)
    {
        m_enforcedVerbosity[cpuType].clear();
    }

    if (!(m_logMessageDispatcherFw->IsPollRequired()
        || m_logMessageDispatcherUcode->IsPollRequired()
        || m_dataMessageDispatcher->IsPollRequired()))
    {
        LOG_DEBUG << m_debugLogPrefix << "no more enabled logging consumers, going to stop polling and reset Ring reader" << endl;
        UnRegisterPoller();
        ResetState();
    }

    if (m_cyclicDebugFileRecorder)
    {
        m_cyclicDebugFileRecorder->SplitOutputFile();
    }

    return os;
}

OperationStatus RingReader::SplitRecordings()
{
    LOG_INFO << m_debugLogPrefix << "Split recording file" << endl;
    const auto os1 = OperationStatus::Merge(
        m_logMessageDispatcherFw->SplitRecordings(),
        m_logMessageDispatcherUcode->SplitRecordings());
    const auto os2 = m_dataMessageDispatcher->SplitRecordings();
    return OperationStatus::Merge(os1, os2);
}

LoggingState RingReader::GetLoggingState(CpuType cpuType, LoggingType loggingType) const
{
    return SelectMessageDispatcher(cpuType, loggingType)->GetConsumerState(loggingType);
}

const string& RingReader::GetLoggingStateMsg(CpuType cpuType, LoggingType loggingType) const
{
    return SelectMessageDispatcher(cpuType, loggingType)->GetConsumerStateMessage(loggingType);
}

ChunkHealthReport RingReader::GetLoggingHealth(CpuType cpuType, LoggingType loggingType) const
{
    return SelectMessageDispatcher(cpuType, loggingType)->GetConsumerHealth(loggingType);
}

LogFmtStringsContainer RingReader::GetFmtStrings(CpuType cpuType, LoggingType loggingType) const
{
    return SelectMessageDispatcher(cpuType, loggingType)->GetFmtStrings(loggingType);
}

uint32_t RingReader::GetBufferSizeInBytesById(uint32_t /*bufferSizeId*/) const
{
    // Given id is not relevant, buffer size is always taken from the persistent configuration:
    // - for Host mode it's a single payload size
    // - for FW mode we use the same value to allow collection of multiple descriptors
    //   in a single iteration (internal ring payload size is much smaller)
    return Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingPayloadSizeBytes + GetMaxResidualSizeInBytes();
}

// there is no real header for ring reader, this room is used to store a residual part of the last event (if any)
uint32_t RingReader::GetMaxResidualSizeInBytes() const
{
    static const uint32_t sc_maxPmcEventResidualLenBytes = 40U; // 10 DWs
    return sc_maxPmcEventResidualLenBytes;
}

uint32_t RingReader::GetCurrentWrPtr()
{
    return 0;   // This mechanism is not in use for the ring reader
}

MessageDispatcher* RingReader::SelectMessageDispatcher(CpuType cpuType, LoggingType loggingType) const
{
    if (loggingType == LoggingType::PmcDataRecorder)
    {
        return m_dataMessageDispatcher.get();
    }

    return cpuType == CPU_TYPE_FW ?
        m_logMessageDispatcherFw.get() : m_logMessageDispatcherUcode.get();
}

OperationStatus RingReader::PmcGetDataHostMode(const Device& device, uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData)
{
    return device.PmcGetData(bufLenBytesInOut, pBuffer, hasMoreData);
}

// Remark: When given buffer length is zero, total data amount available in the ring is returned, no data is retrieved
OperationStatus RingReader::PmcGetDataFwMode(const Device& device, uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData)
{
    const uint32_t bufLenBytesIn = bufLenBytesInOut;
    bufLenBytesInOut = 0;
    hasMoreData = false;

    const auto pPmcRingSpec = device.GetPmcContext().GetSequence().GetPmcRingSpec();
    if (!pPmcRingSpec)
    {
        return OperationStatus(false, "unknown PMC Ring spec.");
    }

    if (bufLenBytesIn > 0 && bufLenBytesIn < pPmcRingSpec->PmcRingPayloadSizeBytes) // shouldn't happen
    {
        std::ostringstream oss;
        oss << "failed to retrieve PMC data: buffer of " << bufLenBytesIn
            << " Bytes is too small, minimum required is " << pPmcRingSpec->PmcRingPayloadSizeBytes;
        return OperationStatus(false, oss.str());
    }

    // make sure we don't exceed the actual ring size
    const uint32_t bufferLength = std::min(bufLenBytesIn,
        pPmcRingSpec->PmcRingDescriptorsCount * pPmcRingSpec->PmcRingPayloadSizeBytes);

    // get SW Head value, 'sw_head - 1' is an index of last written descriptor, 'sw_head' is the next one to be written
    uint32_t swHead = 0U;
    auto os = device.Read(pPmcRingSpec->PmcRingSwHeadAddress, swHead);
    if (!os)
    {
        return os;
    }

    if (swHead >= pPmcRingSpec->PmcRingDescriptorsCount)
    {
        std::ostringstream oss;
        oss << "failed to retrieve PMC data: SW head=" << swHead
            << " is out of range [0," << pPmcRingSpec->PmcRingDescriptorsCount << ")";
        return OperationStatus(false, oss.str());
    }

    LOG_VERBOSE << m_debugLogPrefix << "retrieve PMC data, "
        << " SW head: " << swHead
        << " SW tail: " << m_rdPtr << endl;

    // read all descriptors
    const auto descLenBytes = static_cast<uint32_t>(pPmcRingSpec->PmcRingDescriptorsCount * sizeof(DmaDescriptor));
    m_fwRingDescriptors.resize(descLenBytes); // does nothing when current size complies
    os = device.ReadBlock(pPmcRingSpec->PmcRingDescStartAddress, descLenBytes, reinterpret_cast<char*>(m_fwRingDescriptors.data()));
    if (!os)
    {
        return os;
    }

    // read the data into the buffer
    for (uint32_t i = m_rdPtr; i != swHead;)
    {
        const uint16_t lengthBytes = m_fwRingDescriptors[i].Length; // actual length of buffer #i

        if (bufferLength)
        {
            // check there is a room for the current descriptor payload, unless it's a data amount query
            // note: there is no need to check descriptor in-use field, i.e. 'pDescriptor[i].Status != 0'
            if (bufLenBytesInOut + lengthBytes > bufferLength)
            {
                m_rdPtr = i; // next descriptor to handle
                hasMoreData = true;
                return OperationStatus();
            }

            // there is enough room for current payload, add it to the output buffer
            // note: copy one payload at a time to ensure we copy only bytes in use

            os = device.ReadBlock(pPmcRingSpec->PmcRingPayloadStartAddress + i * pPmcRingSpec->PmcRingPayloadSizeBytes,
                lengthBytes, reinterpret_cast<char*>(pBuffer + bufLenBytesInOut));
            if (!os)
            {
                return os;
            }

            LOG_VERBOSE << m_debugLogPrefix
                << "PMC descriptor=" << i
                << ", data length (B)=" << lengthBytes << std::endl;
        }

        ++i;
        i %= pPmcRingSpec->PmcRingDescriptorsCount;
        bufLenBytesInOut += lengthBytes;
    }

    if (bufferLength)
    {
        // consumed all written descriptors, so 'sw_head' is the next descriptor to handle
        m_rdPtr = swHead;
    }

    return OperationStatus();
}
