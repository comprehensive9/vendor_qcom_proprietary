/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#include <functional>
#include <ostream>
#include <chrono>

#include "FwStateProvider.h"
#include "Device.h"
#include "DriverContracts.h"
#include "FwPointerTableAccessor.h"
#include "PinnedMemorySwitchboard.h"
#include "FieldDescription.h"
#include "Host.h"
#include "EventsDefinitions.h"
#include "TaskManager.h"
#include "LogCollector/LogCollectorService.h"

using namespace std;

namespace
{
#ifdef RTL_SIMULATION
    const unsigned READ_POLLING_INTERVAL_MSEC = 60000U;
#else
    const unsigned READ_POLLING_INTERVAL_MSEC = 500U;
#endif
}

// *************************************************************************************************

std::ostream& operator<<(std::ostream &os, wil_fw_state fwHealthState)
{
    switch (fwHealthState)
    {
    case wil_fw_state::WIL_FW_STATE_UNKNOWN:            return os << "Unknown";
    case wil_fw_state::WIL_FW_STATE_DOWN:               return os << "Down";
    case wil_fw_state::WIL_FW_STATE_READY:              return os << "Ready";
    case wil_fw_state::WIL_FW_STATE_ERROR_BEFORE_READY: return os << "SysAssert before Ready";
    case wil_fw_state::WIL_FW_STATE_ERROR:              return os << "SysAssert";
    case wil_fw_state::WIL_FW_STATE_UNRESPONSIVE:       return os << "Unresponsive";
    default: return os << "Unrecognized fw state " << static_cast<int>(fwHealthState);
    }
}

// *************************************************************************************************

AtomicFwInfo::AtomicFwInfo(
    FwIdentifier fwIdentifier, wil_fw_state fwHealthState, uint32_t fwError, uint32_t uCodeError, bool allowUndefinedState)
    : FwUniqueId(fwIdentifier)
    , FwHealthState(fwHealthState)
    , FwError(fwError)
    , UCodeError(uCodeError)
    , m_allowUndefinedState(allowUndefinedState)
{}

bool AtomicFwInfo::IsInitialized() const
{
    // If m_pollingRequired == true then we assume FW is initialized, otherwise check m_fwHealthState
    const bool isInitialized =
        m_allowUndefinedState ||
        FwHealthState == wil_fw_state::WIL_FW_STATE_ERROR_BEFORE_READY ||
        FwHealthState == wil_fw_state::WIL_FW_STATE_READY ||
        FwHealthState == wil_fw_state::WIL_FW_STATE_ERROR ||
        FwHealthState == wil_fw_state::WIL_FW_STATE_UNRESPONSIVE;

    LOG_VERBOSE << "Check FW state."
        << " Polling: " << BoolStr(m_allowUndefinedState)
        << " State: " << FwHealthState
        << " Reported as initialized: " << BoolStr(isInitialized)
        << endl;

    return isInitialized;

}

std::ostream& operator<<(std::ostream& os, const AtomicFwInfo& fwState)
{
    return os << "FW Info: " << fwState.FwUniqueId
        << " FW Health State: " << fwState.FwHealthState
        << " FW Error: " << Hex<>(fwState.FwError)
        << " uCode Error: " << Hex<>(fwState.UCodeError);
}

// *************************************************************************************************

FwStateProvider::FwStateProvider(const Device& device, bool pollingRequired)
    : m_device(device)
    , m_pollingRequired(pollingRequired)
    , m_fwPointerTableAccessor(device)
    , m_fwError(0U)
    , m_uCodeError(0U)
    , m_fwHealthState(wil_fw_state::WIL_FW_STATE_UNKNOWN)
    , m_fwInfoPollerTaskName("fw_info_" + m_device.GetDeviceName())
    , m_fwErrorAddress(PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::FW_ERROR, device.GetBasebandType()))
    , m_uCodeErrorAddress(PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::UCODE_ERROR, device.GetBasebandType()))
{
    if (m_pollingRequired)
    {
        // do not poll for FW pointer table changes, supported only with the FW health state change event
        // otherwise we don't have indication when table initialization completed

        // init FW version, without notification event
        ReadDeviceFwInfoInternal(false);
        // init FW & uCode errors, without notification event
        ReadFwUcodeErrorsInternal(false);

        if (!Host::GetHost().GetTaskManager().RegisterTask(
            [this] { PollDeviceFwInfo(); }, m_fwInfoPollerTaskName, std::chrono::milliseconds(READ_POLLING_INTERVAL_MSEC)))
        {
            LOG_ERROR << "Cannot track FW state changes" << endl;
        }
    }
    // otherwise FW info and so errors are initialized with zeros and will be updated upon transition to WIL_FW_STATE_READY state
}

void FwStateProvider::StopBlocking()
{
    // no lock is needed since the below state is const

    if (m_pollingRequired)
    {
        LOG_DEBUG << "start stopping FW info provider for device " << m_device.GetDeviceName() << std::endl;
        Host::GetHost().GetTaskManager().UnregisterTaskBlocking(m_fwInfoPollerTaskName);
        LOG_DEBUG << "done stopping FW info provider for device " << m_device.GetDeviceName() << std::endl;
    }
}

AtomicFwInfo FwStateProvider::GetFwInfo() const
{
    lock_guard<mutex> lock(m_mutex);
    return {m_fwIdentifier, m_fwHealthState, m_fwError, m_uCodeError, m_pollingRequired};
}

void FwStateProvider::ReadFwPointerTableInternal()
{
    const auto os = m_fwPointerTableAccessor.ReadFwPointerTable();
    if (!os)
    {
        LOG_ERROR << "FW Pointer Table initialization failed. Error: " << os.GetStatusMessage() << endl;
    }
}

void FwStateProvider::PollDeviceFwInfo()
{
    lock_guard<mutex> lock(m_mutex);
    ReadDeviceFwInfoInternal();
    ReadFwUcodeErrorsInternal();
}

// Internal service for fetching the FW version and compile times
// Assumes the lock is already acquired
void FwStateProvider::ReadDeviceFwInfoInternal(bool notifyOnChange /*=true*/)
{
    if (m_device.GetSilenceMode())
    {
        return;
    }

    const auto basebandType = m_device.GetBasebandType();

    FwIdentifier fwIdentifier {};
    // = FW version
    bool readOk = m_device.Read(PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::FW_VERSION_MAJOR, basebandType), fwIdentifier.m_fwVersion.m_major);
    readOk &= m_device.Read(PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::FW_VERSION_MINOR, basebandType), fwIdentifier.m_fwVersion.m_minor);
    readOk &= m_device.Read(PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::FW_VERSION_SUB_MINOR, basebandType), fwIdentifier.m_fwVersion.m_subMinor);
    readOk &= m_device.Read(PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::FW_VERSION_BUILD, basebandType), fwIdentifier.m_fwVersion.m_build);

    uint32_t fwTimestampStartAddress = Device::sc_invalidAddress;
    uint32_t uCodeTimestampStartAddress = Device::sc_invalidAddress;
    m_fwPointerTableAccessor.GetTimestampAddresses(fwTimestampStartAddress, uCodeTimestampStartAddress);

    // = FW compile time
    // fwTimestampStartAddress is an address of the following struct:
    // typedef struct image_timestamp_ty
    // {
    //    U32 hour;
    //    U32 minute;
    //    U32 second;
    //    U32 day;
    //    U32 month;
    //    U32 year;
    // } image_timestamp_s;
    readOk &= m_device.Read(fwTimestampStartAddress, fwIdentifier.m_fwTimestamp.m_hour);
    readOk &= m_device.Read(fwTimestampStartAddress + 4, fwIdentifier.m_fwTimestamp.m_min);
    readOk &= m_device.Read(fwTimestampStartAddress + 8, fwIdentifier.m_fwTimestamp.m_sec);
    readOk &= m_device.Read(fwTimestampStartAddress + 12, fwIdentifier.m_fwTimestamp.m_day);
    readOk &= m_device.Read(fwTimestampStartAddress + 16, fwIdentifier.m_fwTimestamp.m_month);
    readOk &= m_device.Read(fwTimestampStartAddress + 20, fwIdentifier.m_fwTimestamp.m_year);

    // = uCode compile times (or an invalidated timestamp)
    if (uCodeTimestampStartAddress == Device::sc_invalidAddress)
    {
        fwIdentifier.m_uCodeTimestamp = FwTimestamp::CreateInvalidTimestamp();
    }
    else
    {
        // uCodeTimestampStartAddress contains address of the following struct (indirect):
        // typedef struct {
        //    U08 ucode_compilation_sec;
        //    U08 ucode_compilation_min;
        //    U08 ucode_compilation_hour;
        //    U08 ucode_compilation_day;
        //    U08 ucode_compilation_month;
        //    U08 ucode_compilation_year;
        // } ucode_timestamp_s;

        uint32_t value = 0U;
        readOk &= m_device.Read(uCodeTimestampStartAddress, value);
        if (value == 0U)
        {
            // when uCode timestamp is not properly updated (can happen for WMI_ONLY FW) it should be ignored
            // newer WMI_ONLY FW can update the timestamp, so cannot decided base on FW flavor
            fwIdentifier.m_uCodeTimestamp = FwTimestamp::CreateInvalidTimestamp();
        }
        else
        {
            fwIdentifier.m_uCodeTimestamp.m_sec = FieldDescription::MaskValue(value, 0, 7);
            fwIdentifier.m_uCodeTimestamp.m_min = FieldDescription::MaskValue(value, 8, 15);
            fwIdentifier.m_uCodeTimestamp.m_hour = FieldDescription::MaskValue(value, 16, 23);
            fwIdentifier.m_uCodeTimestamp.m_day = FieldDescription::MaskValue(value, 24, 31);

            readOk &= m_device.Read(uCodeTimestampStartAddress + sizeof(uint32_t), value);
            fwIdentifier.m_uCodeTimestamp.m_month = FieldDescription::MaskValue(value, 0, 7);
            fwIdentifier.m_uCodeTimestamp.m_year = FieldDescription::MaskValue(value, 8, 15);
        }
    }

    if (!readOk)
    {
        LOG_ERROR << "Failed to read FW info for device " << m_device.GetDeviceName() << endl;
        return;
    }

    if (m_fwIdentifier != fwIdentifier)
    {
        m_fwIdentifier = fwIdentifier;
        LOG_INFO << "On Device " << m_device.GetDeviceName() << ", detected " << fwIdentifier << endl;

        if (notifyOnChange)
        {
            Host::GetHost().PushEvent(NewDeviceDiscoveredEvent(m_device.GetDeviceName(), fwIdentifier.m_fwVersion,
                fwIdentifier.m_fwTimestamp, fwIdentifier.m_uCodeTimestamp));
        }
    }
}

// Internal service for fetching the FW/uCode errors
// Assumes the lock is already acquired
void FwStateProvider::ReadFwUcodeErrorsInternal(bool notifyOnChange /*=true*/)
{
    if (m_device.GetSilenceMode())
    {
        return;
    }

    uint32_t fwError = 0, uCodeError = 0;
    const auto os = OperationStatus::Merge(
        m_device.Read(m_fwErrorAddress, fwError),
        m_device.Read(m_uCodeErrorAddress, uCodeError));
    if (!os)
    {
        LOG_ERROR << "On Device " << m_device.GetDeviceName()
            << "Failed to read FW state. Error: " << os.GetStatusMessage() << endl;
        return;
    }

    if (m_fwError != fwError || m_uCodeError != uCodeError)
    {
        m_fwError = fwError;
        m_uCodeError = uCodeError;

        if (m_fwError != 0U || m_uCodeError != 0U)
        {
            LOG_INFO << "On Device " << m_device.GetDeviceName() << ", detected Firmware error; assert codes: FW "
                << Hex<>(m_fwError) << ", uCode " << Hex<>(m_uCodeError) << endl;
        }

        if (notifyOnChange)
        {
            Host::GetHost().PushEvent(FwUcodeStatusChangedEvent(m_device.GetDeviceName(), m_fwError, m_uCodeError));
        }
    }
}

// Trivial implementation of a FSM for FW health state changes
// Note: FW can be considered as initialized when m_pollingRequired OR state is one of
//       {WIL_FW_STATE_READY, WIL_FW_STATE_SYSASSERT, WIL_FW_STATE_UNRESPONSIVE}
void FwStateProvider::OnFwHealthStateChanged(wil_fw_state fwHealthState)
{
    lock_guard<mutex> lock(m_mutex);

    if (m_pollingRequired)
    {
        // shouldn't happen
        LOG_ERROR << "On Device " << m_device.GetDeviceName() << ", received FW health state change notification when polling is turned On" << endl;
        return;
    }

    if (m_fwHealthState == fwHealthState)
    {
        // we're done
        LOG_VERBOSE << "On Device " << m_device.GetDeviceName() << ", received FW health state "
            << m_fwHealthState << " (no change)" << endl;
        return;
    }

    LOG_DEBUG << "On Device " << m_device.GetDeviceName() << ", reported FW health state change from: "
        << m_fwHealthState << " to: " << fwHealthState << endl;

    switch (fwHealthState)
    {
        case (wil_fw_state::WIL_FW_STATE_READY):
        case (wil_fw_state::WIL_FW_STATE_ERROR_BEFORE_READY):
            // Note: In case of assert before ready it's still worth to try the device init sequence.
            // We may report bad FW info, but at least error code should be correct.
            HandleDeviceInitialization();
            break;

        case (wil_fw_state::WIL_FW_STATE_ERROR):
            if (m_fwHealthState == wil_fw_state::WIL_FW_STATE_UNKNOWN)
            {
                // we're after ready, device is initialized
                HandleDeviceInitialization();
            }
            else
            {
                ReadFwUcodeErrorsInternal();

                if (m_fwError == 0U && m_uCodeError == 0U)
                {
                    // meaning it's the 'unresponsive' state (scan/connect timeout or PCIe link down detected by the driver)
                    //  - override the state before pushing event
                    fwHealthState = wil_fw_state::WIL_FW_STATE_UNRESPONSIVE; //update the fwHealthState temporarily and continue
                    LOG_INFO << "On Device " << m_device.GetDeviceName() << ", detected unresponsive FW health state" << endl;
                }
            }
            break;

        case (wil_fw_state::WIL_FW_STATE_UNKNOWN):
        case (wil_fw_state::WIL_FW_STATE_DOWN):
            // do nothing
            break;

        default:
            // not an error, may happen if working with a newer driver version
            LOG_WARNING << "On Device " << m_device.GetDeviceName() << ", received unknown FW health state (" << static_cast<int>(fwHealthState)
                << "), consider upgrading host_manager_11ad" << endl;
            break;
    }
    const auto prevFwHealthState = m_fwHealthState;
    m_fwHealthState = fwHealthState;

    Host::GetHost().GetLogCollectorService().NotifyFwStateChange(m_device.GetDeviceName(), fwHealthState);

    Host::GetHost().PushEvent(FwHealthStateChangedEvent(m_device.GetDeviceName(),
        static_cast<int>(m_fwHealthState),
        static_cast<int>(prevFwHealthState)));
}

void FwStateProvider::HandleDeviceInitialization()
{
    ReadFwPointerTableInternal();
    ReadDeviceFwInfoInternal();
    ReadFwUcodeErrorsInternal();
}

// Check if given capability is set
OperationStatus FwStateProvider::IsCapabilitySet(wmi_fw_capability capability, bool& isSet) const
{
    return m_fwPointerTableAccessor.IsCapabilitySet(static_cast<unsigned>(capability), isSet);
}
