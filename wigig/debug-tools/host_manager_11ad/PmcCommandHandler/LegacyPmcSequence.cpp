/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LegacyPmcSequence.h"
#include "DebugLogger.h"
#include "Device.h"
#include "PmcContext.h"
#include "Utils.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "PmcRecordingFileCreator.h"

#include <sstream>
#include <thread>
#include <vector>
#include <string>

LegacyPmcSequence::LegacyPmcSequence(const Device& device)
    : PmcSequenceBase(device)
{
    LOG_DEBUG << "On device " << device.GetDeviceName() << ", Legacy PMC sequence created" << std::endl;
}

OperationStatus LegacyPmcSequence::InitImpl() const
{
    LOG_INFO << "Initializing PMC Recording on " << m_device.GetDeviceName() << std::endl;

    // we ignore return value because it fails anyway for dealoc-pmc ring
    (void)ShutdownImpl();

    auto os = EnablePmcClock();
    if (!os)
    {
        return os;
    }

    os = AllocatePmcRing();
    if (!os)
    {
        return os;
    }

    os = DisablePrpClockGating();
    if (!os)
    {
        return os;
    }

    return OperationStatus(true);
}

// Make sure PMC is inactive.
OperationStatus LegacyPmcSequence::ShutdownImpl() const
{
    LOG_DEBUG << "PMC Shutdown" << std::endl;
    const uint32_t PMC_SAMPLING_MASK = 0x00FFFFFF;
    auto os = DisableEventCollection(PMC_SAMPLING_MASK);
    if (!os)
    {
        return os;
    }

    os = DeactivateRecordingImpl();
    if (!os)
    {
        return os;
    }

    // Talyn MB adds SW reset feature
    if (m_device.GetBasebandRevision() >= BasebandRevision::TLN_M_B0)
    {
        LOG_DEBUG << "Performing PMC SW Reset" << std::endl;
        const std::string PMC_SW_RESET_MNEMONIC = "USER.CLKS_CTL.SW.RST.USER_USER_CLKS_CTL_SW_RST_VEC_0";
        uint32_t pmcSwResetRegValue = 0U;
        os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(PMC_SW_RESET_MNEMONIC, pmcSwResetRegValue);
        if (!os)
        {
            return os;
        }

        if (pmcSwResetRegValue == Utils::REGISTER_DEFAULT_VALUE)
        {
            return OperationStatus(false, "No PMC SW reset supported as expected on device");
        }

        LOG_DEBUG << "Got SW Reset register value: 0x" << Hex<8>(pmcSwResetRegValue) << std::endl;
        const auto pmcSwResetRegValue15is1 = PmcRegistersAccessor::WriteToBitMask(pmcSwResetRegValue, 15, 1, 1);
        LOG_DEBUG << "Raising SW Reset flag: 0x" << Hex<8>(pmcSwResetRegValue15is1) << std::endl;
        os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(PMC_SW_RESET_MNEMONIC, pmcSwResetRegValue15is1);
        if (!os)
        {
            return os;
        }

        const auto pmcSwResetRegValue15is0 = PmcRegistersAccessor::WriteToBitMask(pmcSwResetRegValue, 15, 1, 0);
        LOG_DEBUG << "Clearing SW Reset flag: 0x" << Hex<8>(pmcSwResetRegValue15is0) << std::endl;
        os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(PMC_SW_RESET_MNEMONIC, pmcSwResetRegValue15is0);
        if (!os)
        {
            return os;
        }
    }

    return m_device.DeallocPmc(true);
}

OperationStatus LegacyPmcSequence::PausePmcRecordingImpl() const
{
    auto os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.rec_en_clr", 1);
    if (!os)
    {
        return os;
    }

    return DisplayCurrentTsf("Deactivated");
}

OperationStatus LegacyPmcSequence::ValidateRecordingStatusImpl() const
{
    // Check if PMC is already running
    bool recActive = false;
    OperationStatus os = IsPmcRecordingActive(recActive);
    if (!os)
    {
        return os;
    }

    if (recActive)
    {
        return OperationStatus(false, "Recording already active. To restart recording please use shutdown command followed by init / start");
    }

    return OperationStatus();
}

OperationStatus LegacyPmcSequence::ActivateRecordingImpl() const
{
    OperationStatus os = DisplayCurrentTsf("Activating PMC Recording");
    if (!os)
    {
        return os;
    }

    return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.rec_en_set", 1);
}

OperationStatus LegacyPmcSequence::SetDefaultConfigurationImpl() const
{
    auto os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.delimiter", 0xcafe);
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.ucpu_udef_en", 0U);
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.fw_udef_en", 0U);
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_1.intf_type", 1);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_1.dma_if_en", 1);
    if (!os)
    {
        return os;
    }

    // Configure PMC HW FIFO threshold (in uint32_t). When the FIFO is filled to this threshold, its content is flushed to the current DMA descriptor.
    // For optimal ring utilization this threshold should be equal to the descriptor buffer size, hence threshold = descriptor_bytes/4.

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_1.pkt_treshhold",
        Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingPayloadSizeBytes / sizeof(uint32_t));
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister("MAC_RGF.PMC.RX_TX.MAC_MAC_RGF_PMC_RX_TX_0", 0xFC);
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
OperationStatus LegacyPmcSequence::DeactivateRecordingImpl() const
{
    LOG_INFO << "Deactivating PMC Recording" << std::endl;

    OperationStatus os = PausePmcRecordingImpl();
    if (!os)
    {
        return os;
    }

    // Wait for pmc to finish flush internal buffer to dma
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return DisablePmcClock();
}

OperationStatus LegacyPmcSequence::FinalizeRecordingImpl(bool onTargetClient, std::vector<uint8_t>& pmcData, std::string& pmcDataPath) const
{
    (void)onTargetClient;
    (void)pmcData;

    LOG_INFO << "Finalizing Legacy PMC data collection" << std::endl;
    const PmcRecordingFileCreator pmcFilesCreator(m_device);
    return pmcFilesCreator.CreatePmcFiles(pmcDataPath);
}

std::string LegacyPmcSequence::GetPmcClockRegister() const
{
    if (m_device.GetBasebandRevision() <= BasebandRevision::SPR_D0) // BasebandRevision::SPR_D0 is the last sparrow revision.
    {
        return "USER.CLK_CTL_EXTENTION.USER_USER_CLKS_CTL_EXT_SW_BYPASS_HW_CG_0.mac_pmc_clk_sw_bypass_hw";
    }
    return "USER.EXTENTION.USER_USER_EXTENTION_3.mac_pmc_clk_sw_bypass_hw";
}

OperationStatus LegacyPmcSequence::DisablePmcClock() const
{
    LOG_INFO << "Disable PMC clock" << std::endl;
    const std::string pmcClockRegister = GetPmcClockRegister();
    return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(pmcClockRegister, 0);
}

OperationStatus LegacyPmcSequence::EnablePmcClock() const
{
    LOG_INFO << "Enable PMC clock" << std::endl;
    const std::string pmcClockRegister = GetPmcClockRegister();
    return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(pmcClockRegister, 1);
}

/// <summary>
/// Workaround for HW bug Bug 6979 - Disable PRP clock gating when using PMC.
/// </summary>
OperationStatus LegacyPmcSequence::DisablePrpClockGating() const
{
    if (m_device.GetBasebandRevision() < BasebandRevision::TLN_M_B0)
    {
        return OperationStatus(true);
    }

    LOG_INFO << "Disabling PRP clock gating" << std::endl;
    const std::string CLK_GATE_MNEMONIC = "USER.EXTENTION.USER_USER_EXTENTION_3.mac_prp_ahb_rgf_hclk_sw_bypass_hw";
    uint32_t clkGateRegValue = 0U;
    auto os = m_device.GetPmcContext().GetRegisterAccessor().ReadPmcRegister(CLK_GATE_MNEMONIC, clkGateRegValue);
    if (!os)
    {
        return os;
    }

    if (clkGateRegValue == Utils::REGISTER_DEFAULT_VALUE)
    {
        return OperationStatus(false, "Clock gating register " + CLK_GATE_MNEMONIC + " has invalid value");
    }

    LOG_INFO << "Got clock gating register value: " << Hex<8>(clkGateRegValue) << std::endl;
    const auto dataToWrite = PmcRegistersAccessor::WriteToBitMask(clkGateRegValue, 16, 1, 1);
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(CLK_GATE_MNEMONIC, dataToWrite);
    if (!os)
    {
        return os;
    }

    LOG_INFO << "Disabling clock gating: " << CLK_GATE_MNEMONIC << " = 0x" << Hex<8>(dataToWrite) << std::endl;
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(CLK_GATE_MNEMONIC, clkGateRegValue);
    if (!os)
    {
        // note: this failure doesn't stop the flow
        LOG_INFO << "Cannot disable PRP clock gating: error writing to register " + CLK_GATE_MNEMONIC << std::endl;
    }

    return OperationStatus(true);
}

OperationStatus LegacyPmcSequence::AllocatePmcRing() const
{
    const auto descriptorSizeInBytes = Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingPayloadSizeBytes;
    const auto numDescriptors = Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingDescriptorsCount;

    // Allocate DMA through driver DebugFS
    OperationStatus os = m_device.AllocPmc(descriptorSizeInBytes, numDescriptors);
    if (!os)
    {
        return os;
    }

    if (m_device.GetBasebandRevision() >= BasebandRevision::TLN_M_B0)
    {
        uint32_t pmcRingIndex = 0U;
        os = FindPmcRingIndex(pmcRingIndex);
        if (!os)
        {
            return os;
        }

        std::stringstream ss;
        ss << "DMA_RGF.DESCQ.<"  << pmcRingIndex << ">.RX_MAX_SIZE.MAX_RX_PL_PER_DESCRIPTOR.val";
        const std::string pmcDescLengthMnemonic = ss.str();
        LOG_INFO << "Configure descriptor size: " << pmcDescLengthMnemonic << "=" << descriptorSizeInBytes << std::endl;
        return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(pmcDescLengthMnemonic, descriptorSizeInBytes);
    }

    return OperationStatus();
}

const PmcRingSpec* LegacyPmcSequence::InitializePmcRingSpecImpl() const
{
    return new PmcRingSpec(PmcModeE::LegacyPmcMode,
        Device::sc_invalidAddress, Device::sc_invalidAddress, // NA for Legacy PMC mode
        Device::sc_invalidAddress, // ringSwHeadAddress, currently not in use
        Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingDescriptorsCount,
        Host::GetHost().GetConfiguration().PmcConfiguration.HostPmcRingPayloadSizeBytes);
}
