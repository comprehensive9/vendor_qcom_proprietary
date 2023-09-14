/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "PmcSequenceBase.h"
#include "DebugLogger.h"
#include "Device.h"
#include "PmcContext.h"
#include "PmcFilterSequence.h"
#include "LogCollector/LogCollectorService.h"

#include <sstream>
#include <vector>
#include <string>

PmcRingSpec::PmcRingSpec(PmcModeE pmcMode,
    uint32_t ringDescStartAddress, uint32_t ringPayloadStartAddress, uint32_t ringSwHeadAddress,
    uint32_t ringDescriptorsCount, uint32_t ringPayloadSizeBytes)
    : PmcMode(pmcMode)
    , PmcRingDescStartAddress(ringDescStartAddress)
    , PmcRingPayloadStartAddress(ringPayloadStartAddress)
    , PmcRingSwHeadAddress(ringSwHeadAddress)
    , PmcRingDescriptorsCount(ringDescriptorsCount)
    , PmcRingPayloadSizeBytes(ringPayloadSizeBytes)
{}

namespace
{
    std::ostream& operator<<(std::ostream& os, const PmcModeE pmcMode)
    {
        switch (pmcMode)
        {
        case PmcModeE::LegacyPmcMode:   return os << "Legacy PMC Mode";
        case PmcModeE::FwPmcMode:       return os << "FW PMC Mode";
        case PmcModeE::HostPmcMode:     return os << "Host PMC Mode";
        default: return os << "unknown PMC mode " << static_cast<int>(pmcMode);
        }
    }

    std::ostream& operator<<(std::ostream& os, const PmcRingSpec& fwPmcRingSpec)
    {
        os << "PMC mode: " << fwPmcRingSpec.PmcMode
            << ", base AHB address: " << Address(fwPmcRingSpec.PmcRingDescStartAddress)
            << ", payloads AHB address: " << Address(fwPmcRingSpec.PmcRingPayloadStartAddress)
            << ", SW_Head AHB address:" << Address(fwPmcRingSpec.PmcRingSwHeadAddress)
            << ", #buffers: " << fwPmcRingSpec.PmcRingDescriptorsCount
            << ", buffer size (Bytes): " << fwPmcRingSpec.PmcRingPayloadSizeBytes;
        return os;
    }
}

OperationStatus PmcSequenceBase::Shutdown() const
{
    // No Validation needed. ShutDown can be called in any state
    const OperationStatus os = ShutdownImpl();
    if (!os)
    {
        LOG_WARNING << "Cannot shutdown PMC: " << os.GetStatusMessage() << std::endl;
    }
    m_device.GetPmcContext().GetDeviceRecordingState().SetState(PMC_RECORDING_STATE::PMC_RECORDING_STATE_IDLE);
    return OperationStatus();
}

OperationStatus PmcSequenceBase::Init() const
{
    if (m_device.GetPmcContext().GetDeviceRecordingState().GetState() !=
        PMC_RECORDING_STATE::PMC_RECORDING_STATE_IDLE)
    {
        return OperationStatus(false, "Pmc is already initialized. To restart the process please use shutdown command followed by init / start ");
    }

    OperationStatus os = InitImpl();
    if (!os)
    {
        return os;
    }

    // We set the current configuration since shutdown deletes it
    os = ApplyCurrentConfiguration();
    if (!os)
    {
        return os;
    }

    m_device.GetPmcContext().GetDeviceRecordingState().SetState(PMC_RECORDING_STATE::PMC_RECORDING_STATE_READY);
    return OperationStatus();
}

OperationStatus PmcSequenceBase::Start() const
{
    // Explicit check  if the device is not initialized (i.e. in IDLE) do init and then start
    bool implicitInit = false;
    if (m_device.GetPmcContext().GetDeviceRecordingState().GetState() == PMC_RECORDING_STATE::PMC_RECORDING_STATE_IDLE)
    {
        LOG_INFO << "Performing implicit PMC initialization" << std::endl;

        OperationStatus os = Init();
        if (!os)
        {
            return os;
        }

        implicitInit = true;
    }

    // If PMC is not READY now (also means it was not in IDLE before) START transition is invalid
    if (m_device.GetPmcContext().GetDeviceRecordingState().GetState() != PMC_RECORDING_STATE::PMC_RECORDING_STATE_READY)
    {
        return OperationStatus(false, "Invalid transition, start can be called only after init or shutdown");
    }

    OperationStatus os = ValidateRecordingStatusImpl();
    if (!os)
    {
        return os;
    }

    os = ActivateRecordingImpl();
    if (!os)
    {
        return os;
    }

    m_device.GetPmcContext().GetDeviceRecordingState().SetState(PMC_RECORDING_STATE::PMC_RECORDING_STATE_ACTIVE);
    return OperationStatus(true, implicitInit ? "true" : "");
}

OperationStatus PmcSequenceBase::Pause() const
{
    if (m_device.GetPmcContext().GetDeviceRecordingState().GetState() != PMC_RECORDING_STATE::PMC_RECORDING_STATE_ACTIVE)
    {
        return OperationStatus(false, "Pmc is not Active, Pause has no effect");
    }

    OperationStatus os = PausePmcRecordingImpl();
    if (!os)
    {
        return os;
    }

    m_device.GetPmcContext().GetDeviceRecordingState().SetState(PMC_RECORDING_STATE::PMC_RECORDING_STATE_PAUSED);
    return OperationStatus();
}

OperationStatus PmcSequenceBase::SetDefaultConfiguration() const
{
    return SetDefaultConfigurationImpl();
}

OperationStatus PmcSequenceBase::StopForOnTarget() const
{
    std::vector<uint8_t> pmcDataDummy;
    return StopInternal(true /*onTargetClient*/, pmcDataDummy);
}

OperationStatus PmcSequenceBase::StopForDmTools(std::vector<uint8_t>& pmcData) const
{
    return StopInternal(false /*onTargetClient*/, pmcData);
}

OperationStatus PmcSequenceBase::ApplyCurrentConfiguration() const
{
    const auto currentDeviceConfiguration = m_device.GetPmcContext().GetDeviceConfigurations();
    LOG_DEBUG << "Applying PMC configuration: " << currentDeviceConfiguration << std::endl;

    auto os = m_device.GetPmcContext().GetFilterSequence().ConfigCollectIdleSm(currentDeviceConfiguration.IsCollectIdleSmEvents());
    if (!os.IsSuccess())
    {
        return os;
    }

    os = m_device.GetPmcContext().GetFilterSequence().ConfigCollectRxPpdu(currentDeviceConfiguration.IsCollectRxPpduEvents());
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetFilterSequence().ConfigCollectTxPpdu(currentDeviceConfiguration.IsCollectTxPpduEvents());
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetFilterSequence().ConfigCollectUcode(currentDeviceConfiguration.IsCollectUcodeEvents());
    if (!os)
    {
        return os;
    }

    return SetDefaultConfiguration();
}

OperationStatus PmcSequenceBase::IsPmcRecordingActive(bool& isRecordingActive) const
{
    // Check if PMC is recording
    const std::string CHECK_PMC_STATE_RGF = "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.rec_active";
    uint32_t value = 0U;
    OperationStatus os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(CHECK_PMC_STATE_RGF, value);
    if (!os)
    {
        return os;
    }

    isRecordingActive = (1 == value);
    return OperationStatus();
}

OperationStatus PmcSequenceBase::ReadPmcHead(uint32_t& pmcHead) const
{
    uint32_t pmcRingIndex;
    auto os = FindPmcRingIndex(pmcRingIndex);
    if (!os)
    {
        return os;
    }
    // Read the SW Head
    std::ostringstream ss;
    ss << "DMA_RGF.DESCQ.<" << pmcRingIndex << ">.SW_HEAD.CRNT";
    const std::string pmcSwHeadMnemonic = ss.str();
    os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(pmcSwHeadMnemonic, pmcHead);
    if (!os)
    {
        return os;
    }
    LOG_INFO << "Read PMC SW Head from " << pmcSwHeadMnemonic << ": " << pmcHead << std::endl;
    return OperationStatus();
}

// specialization to allow cache of the sw_head address
// PMC ring index may be calculated (given ring index is zero) or given
OperationStatus PmcSequenceBase::FindPmcHeadAddress(uint32_t& pmcHeadAddress, uint32_t pmcRingIndex /*= 0*/) const
{
    if (pmcRingIndex == 0)
    {
        auto os = FindPmcRingIndex(pmcRingIndex);
        if (!os)
        {
            return os;
        }
    }

    std::ostringstream ss;
    ss << "DMA_RGF.DESCQ.<" << pmcRingIndex << ">.SW_HEAD.CRNT";
    const std::string pmcSwHeadMnemonic = ss.str();

    const PmcRegisterInfo registerInfo = PmcRegistersAccessor::GetPmcRegisterInfo(pmcSwHeadMnemonic, m_device.GetBasebandRevision());
    if (!registerInfo.IsValid())
    {
        std::ostringstream oss;
        oss << "Failed retrieving register info for " << pmcSwHeadMnemonic;
        return OperationStatus(false, oss.str());
    }

    // note: for sw_head we can ignore the start and end bits and read the whole register
    pmcHeadAddress = registerInfo.GetRegisterAddress();

    return OperationStatus();
}

OperationStatus PmcSequenceBase::FindPmcRingIndex(uint32_t& ringIndex) const
{
    // It's enough to read value of "MAC_RGF.PRP.TOP.MAC_MAC_RGF_PRP_TOP_QID_CTRL_3.pmc_pkt_qring" and sanity check \in [1,11]
    // Both 'pmc_pkt_qring' in PRP and 'is_pmc_dma' of the relevant DMA DESQ are configured by FW as part of PMC DMA configuration

    const static uint32_t DESCQ_SIZE = 12U;
    const static std::string pmcDmaDesqIndexMnemonic = "MAC_RGF.PRP.TOP.MAC_MAC_RGF_PRP_TOP_QID_CTRL_3.pmc_pkt_qring";

    LOG_DEBUG << "Looking for PMC Descriptor Queue index by querying " << pmcDmaDesqIndexMnemonic << std::endl;

    ringIndex = DESCQ_SIZE; // init to invalid value
    OperationStatus os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(pmcDmaDesqIndexMnemonic, ringIndex);
    if (!os)
    {
        return os;
    }

    // PMC ring cannot be 0 as this is used for TX ring
    if (ringIndex == 0 || ringIndex >= DESCQ_SIZE)
    {
        std::ostringstream oss;
        oss << "Invalid ring index " << ringIndex << " in mnemonic " << pmcDmaDesqIndexMnemonic
            << ", valid range is [1," << DESCQ_SIZE - 1U << "]";
        return OperationStatus(false, oss.str());
    }

    LOG_DEBUG << "Found PMC ring: " << ringIndex << std::endl;
    return OperationStatus(true);
}

// Disable events asap so that multiple stations can stop more or less simultaneously.
// No delays are allowed in this method.
OperationStatus PmcSequenceBase::DisableEventCollection(uint32_t pmcSamplingMask) const
{
    LOG_DEBUG << "Disabling event collection" << std::endl;
    const std::string PMC_GENERAL_RGF_0 = "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0";
    uint32_t generalRGF = 0U;
    auto os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(PMC_GENERAL_RGF_0, generalRGF);
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(PMC_GENERAL_RGF_0, generalRGF & pmcSamplingMask);
    if (!os)
    {
        return os;
    }

    return DisplayCurrentTsf("Disabling event collection");
}

OperationStatus PmcSequenceBase::DisplayCurrentTsf(const char* szTitle) const
{
    uint32_t currentTsf = 0U;
    auto os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(
        "MAC_RGF.MAC_SXD.TIMING_INDIRECT.TIMING_INDIRECT_REG_5.msrb_capture_ts_low", currentTsf);
    if (!os)
    {
        return os;
    }

    const uint32_t currentTsf24Bit = currentTsf & 0x00FFFFFF;
    LOG_INFO << szTitle << ". Current TSF = 0x" << Hex<8>(currentTsf) << "  24-bit TSF: " << Hex<8>(currentTsf24Bit) << std::endl;
    return os;
}

OperationStatus PmcSequenceBase::StopInternal(bool onTargetClient, std::vector<uint8_t>& pmcData) const
{
    const auto currentState = m_device.GetPmcContext().GetDeviceRecordingState().GetState();

    if (currentState == PMC_RECORDING_STATE::PMC_RECORDING_STATE_IDLE
        || currentState == PMC_RECORDING_STATE::PMC_RECORDING_STATE_READY)
    {
        return OperationStatus(false, "Pmc is not Active or Paused. Stop has no effect");
    }

    if (currentState == PMC_RECORDING_STATE::PMC_RECORDING_STATE_ACTIVE)
    {
        OperationStatus os = Pause();
        if (!os)
        {
            return os;
        }
    }

    std::string pmcDataPath;
    OperationStatus os = FinalizeRecordingImpl(onTargetClient, pmcData, pmcDataPath);
    if (!os)
    {
        return os;
    }

    // ignore shutdown status, it displays a warning in case of failure
    Shutdown();

    return OperationStatus(true, pmcDataPath); // Returns the data path to be added to the JSON
}

const PmcRingSpec* PmcSequenceBase::GetPmcRingSpec(bool recalculate /*= false*/) const
{
    // ring spec is initialized only once unless recalculation requested
    static std::unique_ptr<const PmcRingSpec> upPmcRingSpec(InitializePmcRingSpecImpl());
    if (recalculate)
    {
        upPmcRingSpec.reset(InitializePmcRingSpecImpl());
        if (upPmcRingSpec)
        {
            LOG_DEBUG << "PMC ring spec: " << *upPmcRingSpec << std::endl;
        }
    }
    return upPmcRingSpec.get();
}
