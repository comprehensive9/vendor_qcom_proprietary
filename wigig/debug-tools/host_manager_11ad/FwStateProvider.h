/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"
#include "FwInfoDefinitions.h"
#include "FwPointerTableAccessor.h"

#include <mutex>
#include <string>

// Enumeration of FW capabilities
// Note: only relevant capabilities are listed
enum wmi_fw_capability
{
    WMI_FW_CAPABILITY_FTM = 0,
    WMI_FW_CAPABILITY_PS_CONFIG = 1,
    WMI_FW_CAPABILITY_RF_SECTORS = 2,
    WMI_FW_CAPABILITY_MGMT_RETRY_LIMIT = 3,
    WMI_FW_CAPABILITY_AP_SME_OFFLOAD_PARTIAL = 4,
    WMI_FW_CAPABILITY_WMI_ONLY = 5,
    WMI_FW_CAPABILITY_MULTI_RF_UT_CMD = 6,
    WMI_FW_CAPABILITY_THERMAL_THROTTLING = 7,
    WMI_FW_CAPABILITY_D3_SUSPEND = 8,
    WMI_FW_CAPABILITY_LONG_RANGE = 9,
    WMI_FW_CAPABILITY_FIXED_SCHEDULING = 10,
    WMI_FW_CAPABILITY_MULTI_DIRECTED_OMNIS = 11,
    WMI_FW_CAPABILITY_RSSI_REPORTING = 12,
    WMI_FW_CAPABILITY_SET_SILENT_RSSI_TABLE = 13,
    WMI_FW_CAPABILITY_LO_POWER_CALIB_FROM_OTP = 14,
    WMI_FW_CAPABILITY_PNO = 15,
    WMI_FW_CAPABILITY_CONNECT_SNR_THR = 16,
    WMI_FW_CAPABILITY_CHANNEL_BONDING = 17,
    WMI_FW_CAPABILITY_REF_CLOCK_CONTROL = 18,
    WMI_FW_CAPABILITY_AP_SME_OFFLOAD_NONE = 19,
    WMI_FW_CAPABILITY_MULTI_VIFS = 20,
    WMI_FW_CAPABILITY_FT_ROAMING = 21,
    WMI_FW_CAPABILITY_BACK_WIN_SIZE_64 = 22,
    WMI_FW_CAPABILITY_AMSDU = 23,
    WMI_FW_CAPABILITY_RAW_MODE = 24,
    WMI_FW_CAPABILITY_TX_REQ_EXT = 25,
    WMI_FW_CAPABILITY_CHANNEL_4 = 26,
    WMI_FW_CAPABILITY_IPA = 27,
    WMI_FW_CAPABILITY_PREEMPTIVE_RING_SWITCH = 28,
    WMI_FW_CAPABILITY_STOP_PMC = 29,
    WMI_FW_CAPABILITY_TEMPERATURE_ALL_RF = 30,
    WMI_FW_CAPABILITY_SPLIT_REKEY = 31,
    WMI_FW_CAPABILITY_AP_POWER_MANAGEMENT = 32,
    WMI_FW_CAPABILITY_WDS_MODE = 33,
    WMI_FW_CAPABILITY_PCIE_CONFIG = 34,
    WMI_FW_CAPABILITY_CONTINUOUS_PMC = 35,
    WMI_FW_CAPABILITY_MAX,
};

class Device; // forward declaration
enum class wil_fw_state;

class AtomicFwInfo final
{
public:
    AtomicFwInfo(
        FwIdentifier fwIdentifier, wil_fw_state fwHealthState, uint32_t fwError, uint32_t uCodeError, bool allowUndefinedState);

    bool IsInitialized() const;

    const FwIdentifier FwUniqueId;
    const wil_fw_state FwHealthState;
    const uint32_t FwError;
    const uint32_t UCodeError;

private:
    const bool m_allowUndefinedState;
};

std::ostream& operator<<(std::ostream& os, const AtomicFwInfo& fwState);

// *************************************************************************************************

class FwStateProvider final
{
public:
    FwStateProvider(const Device& device, bool pollingRequired);

    void StopBlocking();

    AtomicFwInfo GetFwInfo() const;

    void OnFwHealthStateChanged(wil_fw_state fwHealthState);

    // Check if given capability is set
    OperationStatus IsCapabilitySet(wmi_fw_capability capability, bool& isSet) const;

    const FwPointerTableAccessor& GetFwPointerTableAccessor() const { return m_fwPointerTableAccessor; }

    bool InSysAssert() const { return m_fwError != 0U || m_uCodeError != 0U; }

private:
    mutable std::mutex m_mutex; // threads synchronization
    const Device& m_device;
    const bool m_pollingRequired;

    FwPointerTableAccessor m_fwPointerTableAccessor;

    FwIdentifier m_fwIdentifier;
    uint32_t m_fwError;
    uint32_t m_uCodeError;
    wil_fw_state m_fwHealthState;

    const std::string m_fwInfoPollerTaskName;
    const uint32_t m_fwErrorAddress;
    const uint32_t m_uCodeErrorAddress;

    void HandleDeviceInitialization();
    void ReadFwPointerTableInternal();
    void PollDeviceFwInfo();
    void ReadDeviceFwInfoInternal(bool notifyOnChange = true);
    void ReadFwUcodeErrorsInternal(bool notifyOnChange = true);
};
