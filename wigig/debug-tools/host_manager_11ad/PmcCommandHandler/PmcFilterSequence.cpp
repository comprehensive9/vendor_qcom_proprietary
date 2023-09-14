/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcFilterSequence.h"
#include "Device.h"
#include "PmcContext.h"

PmcFilterSequence::PmcFilterSequence(const Device& device): m_device(device)
{
}

OperationStatus PmcFilterSequence::ConfigureUCodeEvents(bool ucodeCoreWriteEnabled) const
{
    const BasebandType deviceType = m_device.GetBasebandType();
    if (deviceType == BasebandType::BASEBAND_TYPE_SPARROW)
    {
        return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
            "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.ucode_event_en", ucodeCoreWriteEnabled ? 1U : 0U);
    }

    // Talyn-M Configuration
    // In Talyn-M the MSB of the core write does not control PMC write anymore as this bit is used to extend an address range.
    // Instead, 2 address ranges may be configured to send core writes in these ranges to PMC.
    LOG_DEBUG << "Configuring uCode CORE_WRITE events for Talyn-M" << std::endl;

    const bool isContinuousPmc = m_device.IsContinuousPmcSupported();

    // do not change filter of ucode_event_en in continuous mode, set by FW
    if (!isContinuousPmc)
    {
        if (!ucodeCoreWriteEnabled)
        {
            return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
                "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.ucode_event_en", 0U);
        }

        auto st = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
            "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.ucode_event_en", 1U);
        if (!st)
        {
            return st;
        }
    }

    // Configure MSXD ranges in PMC_UCODE_WR_RANGE_0
    // Notes:
    // Both ranges boundaries should be configured in the Legacy mode.
    // In Continuous mode, range 1 boundaries are managed by FW and should not be changed.
    // Range 2 boundaries are initialized by FW on startup, but we ensure the configuration when enabling the range.
    if (!isContinuousPmc)
    {
        auto st = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
            "MAC_EXT_RGF.MAC_SXD_EXT.PMC_UCODE_WR_RANGE.MAC_EXT_MAC_EXT_RGF_MAC_SXD_EXT_PMC_UCODE_WR_RANGE_0.sxd_pmc_start_1", 0U);
        if (!st)
        {
            return st;
        }

        st = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
            "MAC_EXT_RGF.MAC_SXD_EXT.PMC_UCODE_WR_RANGE.MAC_EXT_MAC_EXT_RGF_MAC_SXD_EXT_PMC_UCODE_WR_RANGE_0.sxd_pmc_end_1", 0U);
        if (!st)
        {
            return st;
        }
    }

    if (ucodeCoreWriteEnabled)
    {
        auto st = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
            "MAC_EXT_RGF.MAC_SXD_EXT.PMC_UCODE_WR_RANGE.MAC_EXT_MAC_EXT_RGF_MAC_SXD_EXT_PMC_UCODE_WR_RANGE_0.sxd_pmc_start_2", 0U);
        if (!st)
        {
            return st;
        }

        st = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
            "MAC_EXT_RGF.MAC_SXD_EXT.PMC_UCODE_WR_RANGE.MAC_EXT_MAC_EXT_RGF_MAC_SXD_EXT_PMC_UCODE_WR_RANGE_0.sxd_pmc_end_2", 0U);
        if (!st)
        {
            return st;
        }
    }

    // Configure PMC uCode Write Ranges.
    // Note: HW implementation is to collect both ranges (and filter accordingly) when 2nd range enabled.
    //       If both ranges enabled, no filtering done.
    // In Legacy mode we enable only Range1 = [NOP, NOP], Range2 = NA
    // In Continuous mode, Range1 is managed by FW, Range2 = [NOP, NOP], 2nd range enabled to collect both.

    // Local Write to MAC_RGF.MAC_SXD.LOCAL_REGISTER_IF.LOCAL_WR_REG.sxd_local_wr_data
    // User Defined Command Register
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Field                  | Bits  |                                                                              |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Flash                  | 0     |  When set, flashes the PMC Uder Define FIFO                                  |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Last                   | 1     |  When set, the content of the PMC Uder define FIFO is written as a PMC event |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Load uCode Cfg         | 2     |  When set, it loads the PMC Allowed Code                                     |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Reserved               | 7:3   |                                                                              |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Type                   | 15:8  | The User Define Type Value                                                   |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | UCode Core Write       | 17:16 | Enable PMC Core Write                                                        |
    // | Command                |       | 00 - Disable PMC core Write                                                  |
    // |                        |       | 01 - Enable Region 1 codes                                                   |
    // |                        |       | 10 - Enable Region 2 codes                                                   |
    // |                        |       | 11 - Enable all codes                                                        |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Reserved               | 23:18 |                                                                              |
    // +------------------------+-------+------------------------------------------------------------------------------+
    // | Command Code           | 31:24 | The PMC User defined Command Code 0x47                                       |
    // +------------------------+-------+------------------------------------------------------------------------------+

    const uint32_t value = !isContinuousPmc || !ucodeCoreWriteEnabled
        ? (0x1 << 2) | (0x47 << 24) | (0x1 << 16)      // set only 1st range (managed by FW or only 1st required when in legacy)
        : (0x1 << 2) | (0x47 << 24) | (0x2 << 16);     // set only 2nd range to collect both
    return m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.MAC_SXD.LOCAL_REGISTER_IF.LOCAL_WR_REG.sxd_local_wr_data", value);
}

OperationStatus PmcFilterSequence::ConfigCollectIdleSm(bool configurationValue) const
{
    auto configRegisterSet = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.idle_sm_en", configurationValue ? 1U : 0U);
    if (!configRegisterSet.IsSuccess())
    {
        return configRegisterSet;
    }

    m_device.GetPmcContext().GetDeviceConfigurations().SetCollectIdleSmEvents(configurationValue);
    // Currently skip slot_en flag as it seems not to be widely used
    /*if (configurationValue)
    {
        auto st = spDevice->GetPmcContext().GetRegisterAccessor().WritePmcRegister(deviceName,
            "MAC_RGF.PMC.IDLE_SM.MAC_MAC_RGF_PMC_IDLE_SM_0.slot_en", 1U);
        if (!st.IsSuccess())
        {
            return st;
        }
    }*/
    return OperationStatus(true);
}

OperationStatus PmcFilterSequence::ConfigCollectRxPpdu(bool configurationValue) const
{
    auto configRegisterSet = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.rx_en", configurationValue ? 1U : 0U);
    if (!configRegisterSet.IsSuccess())
    {
        return configRegisterSet;
    }
    m_device.GetPmcContext().GetDeviceConfigurations().SetCollectRxPpduEvents(configurationValue);
    return OperationStatus(true);
}

OperationStatus PmcFilterSequence::ConfigCollectTxPpdu(bool configurationValue) const
{
    auto configRegisterSet = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0.tx_en", configurationValue ? 1U : 0U);
    if (!configRegisterSet.IsSuccess())
    {
        return configRegisterSet;
    }
    m_device.GetPmcContext().GetDeviceConfigurations().SetCollectTxPpduEvents(configurationValue);
    return OperationStatus(true);
}

OperationStatus PmcFilterSequence::ConfigCollectUcode(bool configurationValue) const
{
    auto collectUcodeRes = ConfigureUCodeEvents(configurationValue);
    if (!collectUcodeRes.IsSuccess())
    {
        return collectUcodeRes;
    }
    m_device.GetPmcContext().GetDeviceConfigurations().SetCollectUcodeEvents(configurationValue);
    return OperationStatus(true);
}

OperationStatus PmcFilterSequence::ConfigRxMpdu() const
{
    // RX MPDU Data
    auto os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_1.qos_data_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_1.management_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_1.control_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_1.beacon_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_1.error_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_1.phy_info_en", 0U);
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_2.qos_data_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_2.management_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_2.control_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_2.beacon_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_3.error_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.RX.MAC_MAC_RGF_PMC_RX_3.phy_info_max_data", 0x8);
    if (!os)
    {
        return os;
    }

    return OperationStatus(true);
}

OperationStatus PmcFilterSequence::ConfigTxMpdu() const
{
    // TX MPDU Data
    auto os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_1.qos_data_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_1.management_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_1.control_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_1.beacon_en", 0U);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_1.direct_pkt_en", 0U);
    if (!os)
    {
        return os;
    }

    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_2.qos_data_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_2.management_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_2.control_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_2.beacon_max_data", 0x8);
    if (!os)
    {
        return os;
    }
    os = m_device.GetPmcContext().GetRegisterAccessor().WritePmcRegister(
        "MAC_RGF.PMC.RX_TX.TX.MAC_MAC_RGF_PMC_RX_TX_TX_3.direct_pkt_max_data", 0x8);
    if (!os)
    {
        return os;
    }

    return OperationStatus(true);
}
