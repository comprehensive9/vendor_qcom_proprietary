/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include "OperationStatus.h"
#include <mutex>
#include <vector>

class Device;

// Notes:
// It's always promised that all table fields are initialized once FW is Ready.
// From table ver. 3, it's promised that all pointers are initialized before init of USAGE_1/2
// registers that mark completion of logging mechanism initialization.
// We can assumes that pointed values known during FW compilation (like capabilities) also contain
// their final values in this stage. Rest of the pointed values should be ready upon FW ready.
class FwPointerTableAccessor
{
    friend std::ostream& operator<<(std::ostream& os, const FwPointerTableAccessor& tableAccessor);

public:
    explicit FwPointerTableAccessor(const Device& device);

    OperationStatus ReadFwPointerTable();

    // \returns True when fields are initialized from the table, false otherwise (defaults)
    bool GetTimestampAddresses(uint32_t& fwTimestampStartAddress, uint32_t& uCodeTimestampStartAddress) const;

    // use mutex as a memory barrier
    bool IsInitialized() const { std::lock_guard<std::mutex> lock(m_mutex); return m_isInitialized; }
    int GetVersion() const { std::lock_guard<std::mutex> lock(m_mutex); return m_version; }

    OperationStatus IsCapabilitySet(unsigned capabilityBit, bool& isSet) const;

    static const unsigned sc_maxSupportedVersion = 3U;

private:
    mutable std::mutex m_mutex;
    const Device& m_device;
    bool m_isInitialized;
    int m_version;                          // FW pointer table version or -1 before parsing completed
    uint32_t m_fwTimestampStartAddress;     // contains a default constant address, may be updated in future table version
    uint32_t m_uCodeTimestampStartAddress;  // from ver 2, Linker addr. of g_shared_mem_ipc.ucode_timestamp, Peripheral Memory
    std::vector<uint32_t> m_fwCapabilities; // from ver 3, FW capabilities array
};
