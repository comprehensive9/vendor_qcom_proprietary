/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "ContinuousPmcSequence.h"
#include "DebugLogger.h"
#include "Device.h"
#include "PmcContext.h"
#include "PmcRecordingFileCreator.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "LogCollector/LogCollectorService.h"

#include <vector>
#include <string>
#include <sstream>

class LogCollectorModuleVerbosity;

ContinuousPmcSequence::ContinuousPmcSequence(const Device& device)
    : PmcSequenceBase(device)
{
    LOG_DEBUG << "On device " << device.GetDeviceName() << ", Continuous PMC sequence created" << std::endl;
}

OperationStatus ContinuousPmcSequence::InitImpl() const
{
    LOG_INFO << "Initializing PMC Recording on " << m_device.GetDeviceName() << std::endl;

    // we ignore return value because it fails anyway for dealoc-pmc ring
    (void)ShutdownImpl();

    // initialization sequence was already handled on the FW side
    return OperationStatus();
}

// Make sure PMC is inactive.
OperationStatus ContinuousPmcSequence::ShutdownImpl() const
{
    LOG_DEBUG << "PMC Shutdown" << std::endl;

    // note: no need to disable event collection here, will be done as part
    // of pause sequence called as part of recording deactivation sequence

    return DeactivateRecordingImpl();
}

OperationStatus ContinuousPmcSequence::PausePmcRecordingImpl() const
{
    // Notes:
    // In Legacy mode we only disable PMC HW recording. Events are disabled as part of Shutdown sequence.
    // In Continuous mode we disable events collection, whose effect is identical to stopping PMC data consumer.
    // PMC data consumer will be stopped as part of the Stop action. But we never disable ucode events collection
    // since 1st range shall stay enabled to allow dummy MSXD command events that allow state transition and collection
    // of following FW user-defined events (HW bug workaround).

    // reset events collection for the first uCode Write Range
    // note: do NOT update PMC device configuration, only disable event collection
    auto os = m_device.GetPmcContext().GetFilterSequence().ConfigureUCodeEvents(false);
    if (!os)
    {
        return os;
    }

    const uint32_t PMC_SAMPLING_MASK = 0x1CFFFFFF; // never reset user-defined and ucode events, required for logs
    os = DisableEventCollection(PMC_SAMPLING_MASK);
    if (!os)
    {
        return os;
    }

    return DisplayCurrentTsf("Deactivated");
}

OperationStatus ContinuousPmcSequence::ValidateRecordingStatusImpl() const
{
    // Check if PMC is already running
    bool recActive = false;
    OperationStatus os = IsPmcRecordingActive(recActive);
    if (!os)
    {
        return os;
    }

    if (!recActive)
    {
        return OperationStatus(false, "Recording is not active. Expected to be activated by the FW in continuous PMC mode.");
    }

    return os;
}

OperationStatus ContinuousPmcSequence::ActivateRecordingImpl() const
{
    OperationStatus os = DisplayCurrentTsf("Activating PMC Recording");
    if (!os)
    {
        return os;
    }

    const std::vector<LogCollectorModuleVerbosity> dummyEnforcedVerbosity;
    return Host::GetHost().GetLogCollectorService().StartLogging(m_device.GetDeviceName(), CPU_TYPE_FW, PmcDataRecorder,
        false, false, false, false, dummyEnforcedVerbosity);
}

OperationStatus ContinuousPmcSequence::SetDefaultConfigurationImpl() const
{
    // ensure defaults as appears in UI

    auto os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.RX_TX.MAC_MAC_RGF_PMC_RX_TX_0", 0xFC);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_0.qid_mask", 0x0);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_0.qid_mask", 0x0);
    if (!os)
    {
        return os;
    }

    if (m_device.GetBasebandType() == BasebandType::BASEBAND_TYPE_TALYN)
    {
        os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PRS.CTRL.MAC_PRS_CTRL_0.pmc_post_dec_mode", 0U);
        if (!os)
        {
            return os;
        }
    }

    os = m_device.GetPmcContext().GetFilterSequence().ConfigRxMpdu();
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetFilterSequence().ConfigTxMpdu();
    if (!os)
    {
        return os;
    }

    return OperationStatus();
}

// Disable PMC recording with some delay after disabling events. This delay is required due to a HW bug.
OperationStatus ContinuousPmcSequence::DeactivateRecordingImpl() const
{
    LOG_INFO << "Deactivating PMC Recording" << std::endl;

    OperationStatus os = PausePmcRecordingImpl();
    if (!os)
    {
        return os;
    }

    // stop PMC data consumer if not stopped already
    auto loggingState = LoggingState::DISABLED;
    std::string tmp;
    Host::GetHost().GetLogCollectorService().GetLoggingState(m_device.GetDeviceName(), CPU_TYPE_FW, PmcDataRecorder, loggingState, tmp);
    if (loggingState != LoggingState::DISABLED)
    {
        os = Host::GetHost().GetLogCollectorService().StopLogging(m_device.GetDeviceName(), CPU_TYPE_FW, PmcDataRecorder);
    }
    return os;
}

OperationStatus ContinuousPmcSequence::FinalizeRecordingImpl(bool onTargetClient, std::vector<uint8_t>& pmcData, std::string& pmcDataPath) const
{
    LOG_INFO << "Finalizing Continuous PMC data collection" << std::endl;
    OperationStatus os = Host::GetHost().GetLogCollectorService().StopLogging(m_device.GetDeviceName(), CPU_TYPE_FW, PmcDataRecorder);
    if (!os)
    {
        return os;
    }

    // set target location, it's returned through operation status
    pmcDataPath = os.GetStatusMessage();

    // for on-target client : merge to single data file, delete input files and create a metadata file
    // for DmTools client: sort and unify all data files to a single data vector, remove input folder
    const PmcRecordingExtractor extractor(m_device, pmcDataPath, onTargetClient);
    return extractor.FinalizePmcRecordingSession(pmcData);
}

const PmcRingSpec* ContinuousPmcSequence::InitializePmcRingSpecImpl() const
{
    uint32_t intfType = 0U; // 0 - FW, 1 - SW
    auto os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_1.intf_type", intfType);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }
    const PmcModeE mode = intfType == 0 ? PmcModeE::FwPmcMode : PmcModeE::HostPmcMode;

    if (mode == PmcModeE::HostPmcMode)
    {
        return new PmcRingSpec(PmcModeE::HostPmcMode,
            Device::sc_invalidAddress, Device::sc_invalidAddress, // NA for Legacy PMC mode
            Device::sc_invalidAddress, // ringSwHeadAddress, currently not in use
            Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingDescriptorsCount,
            Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingPayloadSizeBytes);
    }
    // otherwise, FW PMC mode

    uint32_t pmcRingIndex = 0U;
    os = FindPmcRingIndex(pmcRingIndex);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }

    // pass calculated ring index to spare its additional calculation
    uint32_t ringSwHeadAddress = Device::sc_invalidAddress;
    os = FindPmcHeadAddress(ringSwHeadAddress, pmcRingIndex);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }

    // Note: Ring base address and payload are stored as an offset from the Peripheral memory base
    const uint32_t fwPeriBaseAhb = 0xa20000; // TODO: retrieve from AddressTranslator once it supports retrieval by region name

    std::ostringstream ss;
    ss << "DMA_RGF.DESCQ.<" << pmcRingIndex << ">.SW_BASE.BASE_L";
    uint32_t pmcRingDescStartAddressDelta = Device::sc_invalidAddress;
    os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(ss.str(), pmcRingDescStartAddressDelta);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }
    const uint32_t ringDescStartAddress = fwPeriBaseAhb + pmcRingDescStartAddressDelta;

    std::vector<uint32_t> descriptorData(sizeof(DmaDescriptor) / sizeof(uint32_t));
    os = m_device.ReadBlock(ringDescStartAddress, static_cast<uint32_t>(descriptorData.size()), descriptorData);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }
    const auto pDescriptor = reinterpret_cast<const DmaDescriptor*>(descriptorData.data());
    const uint32_t ringPayloadStartAddress = fwPeriBaseAhb + pDescriptor->PayloadAddr;

    ss.str(std::string());
    ss << "DMA_RGF.DESCQ.<" << pmcRingIndex << ">.SW_SIZE";
    uint32_t ringDescriptorsCount = 0U;
    os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(ss.str(), ringDescriptorsCount);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }

    uint32_t payloadSizeInDwords = 0U;
    os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_1.pkt_treshhold", payloadSizeInDwords);
    if (!os)
    {
        LOG_DEBUG << "failed to retrieve PMC ring parameters, " << os.GetStatusMessage() << std::endl;
        return nullptr;
    }
    const uint32_t ringPayloadSizeBytes = payloadSizeInDwords * sizeof(uint32_t);

    return new PmcRingSpec(PmcModeE::FwPmcMode,
        ringDescStartAddress, ringPayloadStartAddress, ringSwHeadAddress,
        ringDescriptorsCount, ringPayloadSizeBytes);
}
