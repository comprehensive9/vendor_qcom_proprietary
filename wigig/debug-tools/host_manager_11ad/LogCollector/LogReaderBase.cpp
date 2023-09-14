/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "LogReaderBase.h"
#include "Host.h"
#include "DeviceManager.h"
#include "PersistentConfiguration.h"
#include "TaskManager.h"
#include "Device.h"
#include "FwStateProvider.h"
#include "MessageDispatcher.h"
#include "DebugLogger.h"

#include <string>

using namespace std;

bool LogReaderBase::Tracer3ppLogsFilterOn = false;

LogReaderBase::LogReaderBase(const std::string& deviceName, std::string debugLogPrefix, std::string pollerTaskName)
    : m_deviceName(deviceName)
    , m_debugLogPrefix(std::move(debugLogPrefix))
    , m_pollerTaskName(std::move(pollerTaskName))
    , m_valid(false)
    , m_pollerActive(false)
    , m_rdPtr(0U)
    , m_minRdPtr(0U)
    , m_totalLogDwords(0U)
    , m_totalMissedDwords(0U)
{}

LogReaderBase::~LogReaderBase()
{
    Host::GetHost().GetTaskManager().UnregisterTaskBlocking(m_pollerTaskName);
}

void LogReaderBase::Poll()
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(m_deviceName, spDevice);
    if (!os)
    {
        LOG_ERROR << m_debugLogPrefix << "Cannot collect logs: " << os.GetStatusMessage() << endl;
        return;
    }

    if (spDevice->GetSilenceMode())
    {
        // In this case collection is not needed (device is silent or nor recording nor publishing)
        LOG_VERBOSE << m_debugLogPrefix << "cannot collect logs, device is silent" << endl;
        return;
    }

    if (!IsPollRequired())
    {
        LOG_VERBOSE << m_debugLogPrefix << "no poll required, no consumers" << endl;
        return;
    }

    if (!m_valid)
    {
        // log buffer start address is not set, try to update
        // Limitation - the following use case won't be caught:
        // FW down/up w/o device removal (log buffer is not recalculated)
        os = UpdateDeviceInfo(*spDevice);
        if (!os)
        {
            LOG_VERBOSE << m_debugLogPrefix << "Log reader cannot be validated yet, " << os.GetStatusMessage() << endl;
            return;
        }

        os = UpdateRingInfo(*spDevice);
        if (!os)
        {
            LOG_VERBOSE << m_debugLogPrefix << "Log reader cannot be validated yet, " << os.GetStatusMessage() << endl;
            return;
        }

        m_valid = true;
        ReportBufferFound();
        LOG_DEBUG << m_debugLogPrefix << "Log buffer/module info is fetched, start polling" << endl;

        // If FW is initialized at this stage, the log file will be updated with FW info and no additional update is required
        m_upFwInfo.reset(new AtomicFwInfo(spDevice->GetFwStateProvider()->GetFwInfo())); // FW info is already reported to the log file, skip update in the next iteration
    }

    ActivateConsumersIfRequired(*m_upFwInfo); // pass FwUpFound update

    if (!m_upFwInfo->IsInitialized())
    {
        // Buffer ready state
        m_upFwInfo.reset(new AtomicFwInfo(spDevice->GetFwStateProvider()->GetFwInfo()));
        if (m_upFwInfo->IsInitialized())
        {
            // FW_Initialized
            LOG_DEBUG << m_debugLogPrefix << "FW was initialized, reporting updated FW info to active consumers" << endl;
            // FwUpFound  is true
            ReportAndUpdateFwVersion(*m_upFwInfo);
        }
        // if FW not initialized yet, it does not prevent log collection
    }

    os = ReadAndConsumeBuffer(*spDevice);
    if (!os)
    {
        LOG_ERROR << m_debugLogPrefix << os.GetStatusMessage() << endl;
    }
}

OperationStatus LogReaderBase::RegisterPoller()
{
    lock_guard<mutex> lock(m_pollRegisterMutex);

    LOG_DEBUG << m_debugLogPrefix << "register poller. Active: " << BoolStr(m_pollerActive) << endl;

    if (!m_pollerActive)
    {
        if (!Host::GetHost().GetTaskManager().RegisterTask(
            std::bind(&LogReaderBase::Poll, this),
            m_pollerTaskName,
            static_cast<std::chrono::milliseconds>(int64_t(Host::GetHost().GetConfiguration().LogCollectorConfiguration.PollingIntervalMs))))
        {
            ostringstream oss;
            oss << m_debugLogPrefix << "Failed to register logging task";
            return OperationStatus(false, oss.str());
        }
        m_pollerActive = true;
        LOG_DEBUG << m_debugLogPrefix << "registered logging task " << m_pollerTaskName << endl;
    }
    return OperationStatus();
}

void LogReaderBase::UnRegisterPoller()
{
    lock_guard<mutex> lock(m_pollRegisterMutex);
    if (m_pollerActive)
    {
        Host::GetHost().GetTaskManager().UnregisterTaskBlocking(m_pollerTaskName);
        m_pollerActive = false;
        LOG_DEBUG << m_debugLogPrefix << "unregistered logging task " << m_pollerTaskName << endl;
    }
}

void LogReaderBase::ResetState()
{
    LOG_DEBUG << m_debugLogPrefix << "Reset state" << std::endl;

    m_rdPtr = 0;
    m_minRdPtr = 0; // not strictly needed, it's nullified directly after use
    m_totalLogDwords = 0;
    m_totalMissedDwords = 0;

    // reset validity & FW initialization state
    // to be validated by the first poll iteration after next start in order to create new log files
    m_valid = false;
    m_upFwInfo.reset();
}

OperationStatus LogReaderBase::ComputeLogBufferStartAddress(const Device& device, LogBufferInfo& logBufferInfo)
{
    uint32_t logBufferInfoDword = 0;
    auto os = device.Read(logBufferInfo.LogBufferInfoAddress, logBufferInfoDword);
    if (!os)
    {
        os.AddPrefix("Log collector failed to read log offset address: ");
        LOG_VERBOSE << m_debugLogPrefix << os.GetStatusMessage() << endl;
        return os;
    }

    /* calculate first address of fw/ucode log buffer
                          +--------------------------+----------------+
     logBufferInfoDword = |3bits of buffer size index|29bits of offset|
                          +--------------------------+----------------+
    */

    const uint32_t logOffset = logBufferInfoDword & 0x1FFFFFFFU; // we take only the lower 29 bit

    if (logOffset != logBufferInfo.PrevLogOffset)
    {
        LOG_INFO << m_debugLogPrefix << "Log buffer offset changed from " << Address(logBufferInfo.PrevLogOffset) << " to " << Address(logOffset) << endl;
        logBufferInfo.PrevLogOffset = logOffset;
    }

    if (Device::sc_invalidAddress == logOffset)
    {
        // This may happen when the log buffer is not initialized yet
        LOG_VERBOSE << m_debugLogPrefix << "The log buffer is not initialized yet" << endl;
        return OperationStatus(false, "The log buffer is not initialized yet");
    }

    logBufferInfo.LogAddressAhb = logBufferInfo.AhbToLinkerDelta + logOffset; // calculate the log buffer AHB start address.

    //we want to determine the buffer size:
    const uint32_t bufferSizeId = (logBufferInfoDword & 0xE0000000U) >> 29; /* The three upper bits are the index in the size table */
                                                                      /* explained in GetBufferSizeById */
    const uint32_t bufferSizeInBytes = GetBufferSizeInBytesById(bufferSizeId);

    m_logBufferContent.resize(bufferSizeInBytes);

    LOG_VERBOSE << m_debugLogPrefix << "Log Buffer."
        << " Linker Address: " << Address(logOffset)
        << " AHB Address: " << Address(logBufferInfo.LogAddressAhb)
        << " Size ID: " << bufferSizeId
        << " Content Size (DW): " << LogBufferContentDwords()
        << " Content Size (B): " << bufferSizeInBytes
        << endl;

    return OperationStatus();
}

OperationStatus LogReaderBase::GetModuleInfoFromDevice(const Device& device, LogBufferInfo& logBufferInfo) const
{
    auto os = device.ReadBlock(logBufferInfo.LogAddressAhb + sizeof(uint32_t),
        sizeof(logBufferInfo.ModuleLevelInfo),
        reinterpret_cast<char*>(logBufferInfo.ModuleLevelInfo));

    if (!os)
    {
        LOG_ERROR << "Debug Log Prefix: " << m_debugLogPrefix
            << "Failed to read module verbosity structure for " << logBufferInfo.LogCpuType
            << " Address: " << Address(logBufferInfo.LogAddressAhb + sizeof(uint32_t))
            << " Size (B): " << sizeof(logBufferInfo.ModuleLevelInfo)
            << endl;
        return os;
    }

    LOG_DEBUG << m_debugLogPrefix << "Obtained "<< logBufferInfo.LogCpuType << " module info from " << m_deviceName << ": " << logBufferInfo.ModuleLevelInfo << endl;
    return OperationStatus();
}

OperationStatus LogReaderBase::ApplyModuleVerbosity(const Device& device, const LogBufferInfo& logBufferInfo) const
{
    LOG_DEBUG << m_debugLogPrefix << "Apply " << logBufferInfo.LogCpuType << " log verbosity: " << logBufferInfo.ModuleLevelInfo << endl;

    const bool writeBlockRes = device.WriteBlock(logBufferInfo.LogAddressAhb + sizeof(uint32_t),
        sizeof(logBufferInfo.ModuleLevelInfo), reinterpret_cast<const char*>(logBufferInfo.ModuleLevelInfo));

    if (!writeBlockRes)
    {
        ostringstream st;
        st << m_debugLogPrefix << "Failed to write module verbosity structure for "
            << "(address " << Address(logBufferInfo.LogAddressAhb + sizeof(uint32_t))
            << ", size of data: " << sizeof(logBufferInfo.ModuleLevelInfo) << ")";
        LOG_ERROR << "Cannot apply module verbosity: " << st.str() << endl;
        return OperationStatus(false, st.str());
    }

    LOG_VERBOSE << m_debugLogPrefix << "Module verbosity for " << logBufferInfo.LogCpuType << " was set" << endl;
    return OperationStatus(true);
}

OperationStatus LogReaderBase::ActivateLoggingImpl(
    MessageDispatcher& messageDispatcher, CpuType cpuType, LoggingType loggingType,
    bool ignoreLogHistory, bool compress, bool upload,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity,
    RecordingTrigger recordingTrigger, bool debugMode)
{
    if (messageDispatcher.GetConsumerState(loggingType) != LoggingState::DISABLED)
    {
        return OperationStatus(false, "Recording has already been started");
    }

    m_minRdPtr = ignoreLogHistory ? GetCurrentWrPtr() : 0;

    LOG_INFO << "Activate logging."
        << " CPU: " << cpuType
        << " Type: " << loggingType
        << " Ignore log history: " << BoolStr(ignoreLogHistory)
        << " Min RdPtr: " << m_minRdPtr
        << " Compress: " << BoolStr(compress)
        << " Upload: " << BoolStr(upload)
        << " Verbosity param: " << enforcedVerbosity
        << endl;

    // If log buffer is not ready at this stage, verbosity will be updated when FW becomes ready in NotifyFwStateChange()
    if (!enforcedVerbosity.empty())
    {
        SetModuleVerbosity(cpuType, enforcedVerbosity);
    }
    else
    {
        LOG_DEBUG << "No on-start verbosity provided. Using existing enforced verbosity" << endl;
    }

    // no need to test for device existence
    // in case of start logging it was tested by the Logging Service
    // and device discovery is activated directly by the device manager

    OperationStatus os = messageDispatcher.ConfigureAndTestRecordingConditions(loggingType, compress, upload);
    if (!os)
    {
        return os;
    }

    if (!IsLogBufferAvailable(cpuType)) // unable to read log buffer
    {
        const string errMsg = "Log buffer address unavailable";
        messageDispatcher.HandleError(loggingType, errMsg);
        return OperationStatus(false, errMsg);
    }

    os = RegisterPoller();
    if (!os)
    {
        return os;
    }

    // Do not mark chunk consumer as Active, should be done in one place - poll iteration
    messageDispatcher.EnableConsumer(loggingType, recordingTrigger, debugMode);

    return OperationStatus(true);
}

OperationStatus LogReaderBase::EnforceModuleVerbosityImpl(
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity, LogBufferInfo& logBufferInfo)
{
    LOG_INFO << "Enforce " << logBufferInfo.LogCpuType << " log verbosity: " << enforcedVerbosity << endl;

    if (enforcedVerbosity.empty())
    {
        return OperationStatus();
    }

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(m_deviceName, spDevice);
    if (!os)
    {
        LOG_ERROR << m_debugLogPrefix << "Cannot enforce module verbosity: " << os.GetStatusMessage() << endl;
        return os;
    }

    os = UpdateDeviceInfo(*spDevice);
    if (!os)
    {
        return os;
    }

    for (const auto& moduleVerbosity : enforcedVerbosity)
    {
        // Overwrite i-th module verbosity by an enforced value
        const auto i = moduleVerbosity.GetModuleIndex();
        logBufferInfo.ModuleLevelInfo[i].error_level_enable = moduleVerbosity.IsError() ? 1 : 0;
        logBufferInfo.ModuleLevelInfo[i].warn_level_enable = moduleVerbosity.IsWarning() ? 1 : 0;
        logBufferInfo.ModuleLevelInfo[i].info_level_enable = moduleVerbosity.IsInfo() ? 1 : 0;
        logBufferInfo.ModuleLevelInfo[i].verbose_level_enable = moduleVerbosity.IsVerbose() ? 1 : 0;
    }

    return ApplyModuleVerbosity(*spDevice, logBufferInfo);
}

OperationStatus LogReaderBase::GetModuleVerbosityImpl(const LogBufferInfo& logBufferInfo, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
{
    moduleVerbosity.clear();

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(m_deviceName, spDevice);
    if (!os)
    {
        LOG_ERROR << m_debugLogPrefix << "Cannot get module verbosity: " << os.GetStatusMessage() << endl;
        return os;
    }

    os = UpdateDeviceInfo(*spDevice);
    if (!os)
    {
        return os;
    }

    for (int i = 0; i < NUM_MODULES; i++)
    {
        moduleVerbosity.emplace_back(LogCollectorModuleVerbosity(i,
            logBufferInfo.ModuleLevelInfo[i].error_level_enable,
            logBufferInfo.ModuleLevelInfo[i].warn_level_enable,
            logBufferInfo.ModuleLevelInfo[i].info_level_enable,
            logBufferInfo.ModuleLevelInfo[i].verbose_level_enable));
    }

    return OperationStatus();
}
