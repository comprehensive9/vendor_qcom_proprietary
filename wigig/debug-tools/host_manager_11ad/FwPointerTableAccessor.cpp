/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vector>
#include <bitset>

#include "FwPointerTableAccessor.h"
#include "Device.h"
#include "PinnedMemorySwitchboard.h"
#include "AddressTranslator.h"
#include "DebugLogger.h"

using namespace std;

namespace
{
    enum FwPointerTableInfo : uint32_t
    {
        DYNAMIC_ADDRESSING_PATTERN      = 0xBACACAFE,
        DEFAULT_FW_TIMESTAMP_ADDRESS    = 0x880a14, // default to be used before Borrelly
    };

    // Pointer table is defined by the following structures:
    struct FwPointerTableStruct_V1
    {
        uint32_t version;
        uint32_t operational_mbox_base_address;     // from ver. 0
        uint32_t debug_mbox_base_address;           // from ver. 0
        uint32_t dashboard_base_address;            // from ver. 0
        uint32_t p_rfc_connected;                   // Linker addr.of FW.g_rfc_connected_indx_vec which is rf_connected, FW DCCM
        uint32_t p_rfc_enabled;                     // Linker addr. of FW.g_marlon_r_if.marlon_r_if_class.m_rfc_enabled, FW DCCM
    };
    struct FwPointerTableStruct_V2
    {
        FwPointerTableStruct_V1 table_v1;
        uint32_t p_ucode_timestamp;                 // Linker addr.of g_shared_mem_ipc.ucode_timestamp, Peripheral Memory
                                                    // (UCODE.g_shared_mem_ipc.shared_mem_ipc_s.shared_mem_ipc_s.ucode_timestamp.ucode_timestamp_s, 6 Bytes)
    };
    struct FwPointerTableStruct_V3
    {
        FwPointerTableStruct_V2 table_v2;
        uint32_t p_capabilities_array;          // Linker addr. of the capabilities array
        uint32_t capabilities_len_dwords;       // num of elements (DWs) in the capabilities array
    };
    // Note: Next gen table in FW is defined as below:
    //struct FwPointerTableStruct_V_NEXT_GEN
    //{
    //    uint32_t version;
    //    uint32_t fw_timestamp[6];
    //    uint32_t p_ucode_timestamp;
    //};
    // Once relevant, can be aligned to match current structure:
    //struct FwPointerTableStruct_V_NEXT_GEN // ver. 10
    //{
    //    FwPointerTableStruct_V3 table_v3;
    //    uint32_t fw_timestamp[6];
    //};
}

FwPointerTableAccessor::FwPointerTableAccessor(const Device& device)
    : m_device(device)
    , m_isInitialized(false)
    , m_version(-1)
    , m_fwTimestampStartAddress(DEFAULT_FW_TIMESTAMP_ADDRESS)
    , m_uCodeTimestampStartAddress(Device::sc_invalidAddress)
{}

// Read FW pointer table and initialize relevant values/addresses
// Note: FW & uCode timestamp addresses are updated only when provided
// Remarks May be called as a response to FW state change event sent by driver
//         or during log collection polling before FW is ready.
//         Therefore we ensure the initialization is performed only once.
OperationStatus FwPointerTableAccessor::ReadFwPointerTable()
{
    // calculate maximum supported table length in DWs, round to nearest #DWs just in case
    static const uint32_t tableLengthDWs = (sizeof(FwPointerTableStruct_V3) + sizeof(uint32_t) - 1) / sizeof(uint32_t);

    lock_guard<mutex> lock(m_mutex);

    if (m_isInitialized)
    {
        // already fetched
        return OperationStatus();
    }

    if (m_device.GetSilenceMode())
    {
        ostringstream oss;
        oss << "Cannot initialized FW Pointer Table for a silent device " << m_device.GetDeviceName();
        return OperationStatus(false, oss.str());
    }

    const auto basebandType = m_device.GetBasebandType();
    const auto basebandRevision = m_device.GetBasebandRevision();

    uint32_t pattern = 0U;
    const uint32_t patternAddr = PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::POINTER_TABLE_PATTERN, basebandType);
    auto readOs = m_device.Read(patternAddr, pattern);
    if (!readOs)
    {
        readOs.AddPrefix("Failed to read dynamic addressing pattern");
        return readOs;
    }

    // TODO: remove baseband type test once pattern update as a last thing is promoted to all FWs
    if (basebandType == BASEBAND_TYPE_BORRELLY && pattern == 0)
    {
        // pattern is nullified during SW reset and is the last field written during FW pointer table initialization
        ostringstream oss;
        oss << "On Device " << m_device.GetDeviceName() << ", FW pointer table is not yet initialized";
        return OperationStatus(false, oss.str());
    }

    if (pattern != DYNAMIC_ADDRESSING_PATTERN)
    {
        // old FW before dynamic addressing
        LOG_DEBUG << "On Device " << m_device.GetDeviceName() << ", there is no FW pointer table" << endl;
        m_isInitialized = true;
        return OperationStatus(); // not a failure, but version stays -1
    }

    // dynamic addressing pattern if followed by a linker address of the pointer tables structure, which is started with version
    const uint32_t pointerTableAddr = PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::POINTER_TABLE_ADR, basebandType);
    uint32_t pointerTableBaseAddressLinker = Device::sc_invalidAddress;
    readOs = m_device.Read(pointerTableAddr, pointerTableBaseAddressLinker);
    if (!readOs)
    {
        readOs.AddPrefix("Failed to read FW pointer table base address");
        return readOs;
    }
    uint32_t pointerTableBaseAddressAhb = Device::sc_invalidAddress;
    AddressTranslator::ToAhbAddress(pointerTableBaseAddressLinker, pointerTableBaseAddressAhb, basebandRevision);

    std::vector<uint32_t> tableRawData;
    readOs = m_device.ReadBlock(pointerTableBaseAddressAhb, tableLengthDWs, tableRawData);
    if (!readOs)
    {
        readOs.AddPrefix("Failed to read FW pointer table");
        return readOs;
    }

    const auto tableV3 = reinterpret_cast<const FwPointerTableStruct_V3*>(tableRawData.data());
    const uint32_t tableVersion = tableV3->table_v2.table_v1.version;
    if (tableVersion > sc_maxSupportedVersion)
    {
        ostringstream oss;
        oss << "On Device " << m_device.GetDeviceName() << ", found unexpected FW pointer table version "
            << tableVersion << ", maximum supported is " << sc_maxSupportedVersion;
        return OperationStatus(false, oss.str());
    }

    // Read of RFC connected/enabled addresses. Should be used in on-target UI. Uncomment once required
    //// read rfc_connected & rfc_enabled added in ver. 1
    //if (tableVersion >= 1U)
    //{
    //    uint32_t rfcConnectedAddressLinker = table->table_v2.table_v1.p_rfc_connected;
    //    if (rfcConnectedAddressLinker != Device::sc_invalidAddress) // may be zero in 10.1, this case stay with default value
    //    {
    //        uint32_t rfcConnectedAddressAhb = Device::sc_invalidAddress;
    //        AddressTranslator::ToAhbAddress(rfcConnectedAddressLinker, rfcConnectedAddressAhb, basebandRevision);
    //    }
    //
    //    uint32_t rfcEnabledAddressLinker = table->table_v2.table_v1.p_rfc_enabled;
    //    if (rfcEnabledAddressLinker != Device::sc_invalidAddress) // may be zero in 10.1, this case stay with default value
    //    {
    //        uint32_t rfcEnabledAddressAhb = Device::sc_invalidAddress;
    //        AddressTranslator::ToAhbAddress(rfcConnectedAddressLinker, rfcEnabledAddressAhb, basebandRevision);
    //    }
    //}

    // read uCode timestamp address added in ver. 2
    if (tableVersion >= 2U)
    {
        AddressTranslator::ToAhbAddress(tableV3->table_v2.p_ucode_timestamp, m_uCodeTimestampStartAddress, basebandRevision);
    }

    // read FW capability added in ver. 3
    if (tableVersion >= 3U)
    {
        uint32_t fwCapabilitiesAddressAhb = Device::sc_invalidAddress;
        AddressTranslator::ToAhbAddress(tableV3->p_capabilities_array, fwCapabilitiesAddressAhb, basebandRevision);
        readOs = m_device.ReadBlock(fwCapabilitiesAddressAhb, tableV3->capabilities_len_dwords, m_fwCapabilities);
        if (!readOs)
        {
            readOs.AddPrefix("Failed to read FW capabilities array");
            return readOs;
        }
    }

    // take care of FW timestamp once next gen version supported
    //if (tableVersion >= NEXT_GEN_VERSION)
    //{
    //    // NEXT_GEN_VERSION table contain the (FW) image_timestamp_s struct itself
    //    m_fwTimestampStartAddress = pointerTableBaseAddressAhb
    //        + sizeof(uint32_t) * (&tableNextGen->fw_timestamp - &tableV3->table_v2.table_v1.version);
    //}

    m_version = tableVersion;
    m_isInitialized = true;

    LOG_DEBUG << "On Device " << m_device.GetDeviceName() << ", FW Pointer Table info: " << *this << endl;
    return OperationStatus();
}

// \returns True when fields are initialized from the table, false otherwise (defaults)
bool FwPointerTableAccessor::GetTimestampAddresses(uint32_t& fwTimestampStartAddress,
                                                   uint32_t& uCodeTimestampStartAddress) const
{
    lock_guard<mutex> lock(m_mutex);

    // when not yet initialized, update out params with default values
    // backward compatibility with older driver that doesn't send FW state change events
    fwTimestampStartAddress = m_fwTimestampStartAddress;
    uCodeTimestampStartAddress = m_uCodeTimestampStartAddress;
    return m_isInitialized;
}

// capabilityBit is zero based
OperationStatus FwPointerTableAccessor::IsCapabilitySet(unsigned capabilityBit, bool& isSet) const
{
    lock_guard<mutex> lock(m_mutex);
    if (!m_isInitialized || m_version < 3)
    {
        ostringstream oss;
        oss << "No capabilities provided for device " << m_device.GetDeviceName();
        if (m_isInitialized)
        {
            oss << ", FW Pointer Table version is " << m_version;
        }
        return OperationStatus(false, oss.str());
    }

    const unsigned dwIndex = capabilityBit / 32U;
    if (dwIndex >= m_fwCapabilities.size())
    {
        ostringstream oss;
        oss << "Invalid capability bit " << capabilityBit << " for device " << m_device.GetDeviceName();
        oss << ", maximum allowed index is " << m_fwCapabilities.size() * 32 - 1;
        return OperationStatus(false, oss.str());
    }

    isSet = (m_fwCapabilities[dwIndex] & (1 << capabilityBit)) != 0;
    return OperationStatus();
}

std::ostream& operator<<(std::ostream& os, const FwPointerTableAccessor& tableAccessor)
{
    // assumes lock was already acquired
    os << "\n initialized: " << BoolStr(tableAccessor.m_isInitialized);
    if (!tableAccessor.m_isInitialized)
    {
        return os;
    }

    os << ", version: " << tableAccessor.m_version;
    os << " (max supported: " << FwPointerTableAccessor::sc_maxSupportedVersion << ")";
    if (tableAccessor.m_version >= 2)
    {
        os << "\n uCode Timestamp AHB address: " << Address(tableAccessor.m_uCodeTimestampStartAddress);
    }
    if (tableAccessor.m_version >= 3 && !tableAccessor.m_fwCapabilities.empty())
    {
        os << "\n FW capabilities: 32b'" << std::bitset<32>(tableAccessor.m_fwCapabilities.front());
        for (auto iter = std::next(tableAccessor.m_fwCapabilities.cbegin()); iter != tableAccessor.m_fwCapabilities.cend(); ++iter)
        {
            os << ", 32b'" << std::bitset<32>(*iter);
        }
    }
    //if (tableAccessor.m_version >= static_cast<int>(NEXT_GEN_VERSION))
    //{
    //    os << "\n FW Timestamp AHB address:" << Address(tableAccessor.m_fwTimestampStartAddress);
    //}
    return os;
}
