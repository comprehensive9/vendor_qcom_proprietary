/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Device.h"
#include "DriverAPI.h"
#include "DriverContracts.h"
#include "FwStateProvider.h"
#include "DebugLogger.h"
#include "DriversFactory.h"
#include "DeviceManager.h"
#include "TaskManager.h"
#include "FieldDescription.h"
#include "HostAndDeviceDataDefinitions.h"
#include "Utils.h"
#include "PmcCommandHandler/PmcContext.h"

#include <sstream>
#include <map>

using namespace std;

Device::Device(std::unique_ptr<DriverAPI>&& deviceDriver)
    : m_basebandType(BASEBAND_TYPE_NONE)
    , m_basebandRevision(BB_REV_UNKNOWN)
    , m_driver(std::move(deviceDriver))
    , m_deviceName(m_driver->GetInterfaceName())
    , m_isSilent(false)
{
    const bool isPollingRequired = m_driver->InitializeTransports()
        && !m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_FW_STATE);

    InitializeBasebandRevision();
    InitializeDriver(isPollingRequired);

    // TODO: remove condition on the device type once FW state retrieval supported by the wil6210 driver
    if (m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_FW_STATE)
        && m_driver->GetDeviceType() == DeviceType::DPDK)
    {
        wil_fw_state fwState = wil_fw_state::WIL_FW_STATE_UNKNOWN;
        OperationStatus os = m_driver->GetFwState(fwState);
        if (!os)
        {
            LOG_ERROR << "failed to send driver command NL_60G_GEN_GET_FW_STATE, " << os.GetStatusMessage() << endl;
            return;
        }
        m_fwStateProvider->OnFwHealthStateChanged(fwState);
    }

    // create PMC context after getting driver capabilities
    m_upPmcContext.reset(new PmcContext(*this, IsContinuousPmcSupported()));
}

// cannot be inline because incompleteness of FieldDescription
Device::~Device()
{
    LOG_DEBUG << "start destructing device " << m_deviceName << std::endl;
    m_driver->StopDriverControlEvents();
    m_fwStateProvider->StopBlocking();
    LOG_DEBUG << "done destructing device " << m_deviceName << std::endl;
}

DeviceType Device::GetDeviceType() const
{
    return m_driver->GetDeviceType();
}

bool Device::IsMonitored() const
{
    return m_driver->IsMonitored();
}

uint32_t Device::GetCapabilityMask() const
{
    return m_driver->GetCapabilityMask();
}

OperationStatus Device::GetDriverMode(int& currentState) const
{
    return m_driver->GetDriverMode(currentState);
}
OperationStatus Device::SetDriverMode(int newState, int& oldState) const
{
    return m_driver->SetDriverMode(newState, oldState);
}

// \remarks
// Device creation is usually performed before HW reset (and FW download when relevant)
// which is performed under lock. During this lock all IO operations will fail with EBUSY error.
// Baseband revision initialization is crucial for correct device operation and therefore
// all reads during initialization are performed using RetryWhenBusyRead API.
void Device::InitializeBasebandRevision()
{
    // Device ID address
    // Replaces USER.JTAG.USER_USER_JTAG_1.dft_idcode_dev_id at 0x00880b34 used before Borrelly
    // Allows to differentiate between Sparrow, Talyn and Borrelly
    const uint32_t RGF_USER_JTAG_DEV_ID = 0x00880000;

    const uint32_t JTAG_DEV_ID_SPARROW   = 0x2632072f;
    const uint32_t JTAG_DEV_ID_TALYN_MA  = 0x7e0e1;
    const uint32_t JTAG_DEV_ID_TALYN_MBC = 0x1007e0e1;
    const uint32_t JTAG_DEV_ID_BORRELLY  = 0x7f0e1;

    // Revision ID allows to differentiate between Sparrow_M_B0 and Sparrow_M_D0
    const uint32_t RGF_USER_REVISION_ID = (0x88afe4);
    const uint32_t RGF_USER_REVISION_ID_MASK = (3);
    const uint32_t REVISION_ID_SPARROW_B0 = (0x0);
    const uint32_t REVISION_ID_SPARROW_D0 = (0x3);

    // Debug ID allows to differentiate between Talyn_M_B0 and Talyn_M_C0
    const uint32_t RGF_POWER_DEBUG_ID = (0x883f6c); /* PHY_RX_RGF.POWER_DEBUG.power_dbg_dout0 */
    const uint32_t DEBUG_ID_TALYN_B0 = (0x0);
    const uint32_t DEBUG_ID_TALYN_C0 = (0x1);

    m_basebandType = BASEBAND_TYPE_NONE;
    m_basebandRevision = BB_REV_UNKNOWN;

    uint32_t jtagId = Utils::REGISTER_DEFAULT_VALUE;
    OperationStatus os = m_driver->RetryWhenBusyRead(RGF_USER_JTAG_DEV_ID, jtagId);
    if (!os)
    {
        LOG_ERROR << "Cannot determine device revision. Failed to read JTAG ID: " << os.GetStatusMessage() << std::endl;
        return;
    }

    LOG_DEBUG << "On device " << m_deviceName << " obtained JTAG ID: " << AddressValue(RGF_USER_JTAG_DEV_ID, jtagId) << std::endl;

    uint32_t chipRevision = 0;
    uint32_t debugRevision = 0;

    switch (jtagId)
    {
    case JTAG_DEV_ID_SPARROW:
        m_basebandType = BASEBAND_TYPE_SPARROW;

        os = m_driver->RetryWhenBusyRead(RGF_USER_REVISION_ID, chipRevision);
        if (!os)
        {
            LOG_ERROR << "Cannot determine device revision. Failed to read RGF_USER_REVISION_ID: " << os.GetStatusMessage() << std::endl;
            return;
        }

        chipRevision &= RGF_USER_REVISION_ID_MASK;
        switch (chipRevision)
        {
        case REVISION_ID_SPARROW_D0:
            m_basebandRevision = SPR_D0;
            break;
        case REVISION_ID_SPARROW_B0:
            m_basebandRevision = SPR_B0;
            break;
        default:
            LOG_ERROR << "On device " << m_deviceName << " unsupported SPR chip revision: " << AddressValue(RGF_USER_REVISION_ID, chipRevision) << std::endl;
            break;
        }
        break;

    case JTAG_DEV_ID_TALYN_MA:
        m_basebandType = BASEBAND_TYPE_TALYN;
        m_basebandRevision = TLN_M_A0;
        break;

    case JTAG_DEV_ID_TALYN_MBC:
        m_basebandType = BASEBAND_TYPE_TALYN;

        os = m_driver->RetryWhenBusyRead(RGF_POWER_DEBUG_ID, debugRevision);
        if (!os)
        {
            LOG_ERROR << "Cannot determine device revision. Failed to read RGF_POWER_DEBUG_ID: " << os.GetStatusMessage() << std::endl;
            return;
        }

        switch (debugRevision)
        {
        case DEBUG_ID_TALYN_B0:
            m_basebandRevision = TLN_M_B0;
            break;
        case DEBUG_ID_TALYN_C0:
            m_basebandRevision = TLN_M_C0;
            break;
        default:
            LOG_ERROR << "On device " << m_deviceName << " unsupported TLN chip revision: " << AddressValue(RGF_POWER_DEBUG_ID, debugRevision) << std::endl;
            break;
        }
        break;

    case JTAG_DEV_ID_BORRELLY:
        m_basebandType = BASEBAND_TYPE_BORRELLY;
        m_basebandRevision = BRL_P1;
        break;

    default:
        LOG_ERROR << "On device " << m_deviceName << " unsupported JTAG ID: " << AddressValue(RGF_USER_JTAG_DEV_ID, jtagId) << std::endl;
        break;
    }

    LOG_INFO << "On Device " << m_deviceName << " Baseband type is " << m_basebandType << ", Baseband revision is " << m_basebandRevision << std::endl;
}

void Device::InitializeDriver(bool pollingRequired)
{
    // always create FwStateProvider to prevent potential null pointer access
    // if driver is not ready, it will be created without polling
    m_fwStateProvider.reset(new FwStateProvider(*this, pollingRequired));

    // internally tests if driver is ready
    m_driver->RegisterDriverControlEvents(m_fwStateProvider.get());

    const bool isOperationalMailbox = m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_WMI);
    LOG_INFO << "Device " << m_deviceName << " is using " << (isOperationalMailbox ? "Operational" : "Debug") << " mailbox" << endl;
}

bool Device::IsContinuousPmcSupported() const
{
    return m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_CONTINUOUS_PMC);
}

// Provided mask is assumed to be in range [0,31]
OperationStatus Device::AddCustomRegister(const string& name, uint32_t address, uint32_t firstBit, uint32_t lastBit)
{
    if (firstBit > 31 || lastBit > 31 || firstBit > lastBit)
    {
        return OperationStatus(false, "Invalid mask provided. Bits should be in range [0, 31]");
    }

    if (m_customRegistersMap.find(name) != m_customRegistersMap.end())
    {
        return OperationStatus(false, "Register already exists");
    }

    m_customRegistersMap[name].reset(new FieldDescription(address, firstBit, lastBit));
    return OperationStatus();
}

OperationStatus Device::RemoveCustomRegister(const string& name)
{
    if (m_customRegistersMap.find(name) == m_customRegistersMap.end())
    {
        return OperationStatus(false, "Register does not exist");
    }

    m_customRegistersMap.erase(name);
    return OperationStatus();
}

// Read values for all defined custom registers
// Returns true when read operation succeeded for all registers
OperationStatus Device::ReadCustomRegisters(std::vector<StringNameValuePair>& customRegisters) const
{
    customRegisters.clear(); // capacity unchanged
    customRegisters.reserve(m_customRegistersMap.size());

    bool ok = true;
    for (const auto& reg : m_customRegistersMap)
    {
        uint32_t value = Utils::REGISTER_DEFAULT_VALUE;
        ok &= Read(reg.second->GetAddress(), value);
        std::ostringstream oss;
        oss << reg.second->MaskValue(value);

        customRegisters.push_back({reg.first, oss.str()});
    }

    return OperationStatus();
}

OperationStatus Device::ValidateIoOperation(uint32_t address) const
{
    if (m_basebandType == BASEBAND_TYPE_NONE || m_basebandRevision == BB_REV_UNKNOWN)
    {
        return OperationStatus(false, "Baseband type cannot be determined");
    }

    if (GetSilenceMode())
    {
        return OperationStatus(false, "No I/O is allowed in the silent mode");
    }

    if ((0 == address) || (0 != address % 4) || (0xFFFFFFFF == address))
    {
        ostringstream oss;
        oss << "No I/O is allowed for invalid address " << Address(address);
        return OperationStatus(false, oss.str());
    }

    return OperationStatus(true);
}

OperationStatus Device::Read(uint32_t address, uint32_t& value) const
{
    OperationStatus os = ValidateIoOperation(address);
    if (!os)
    {
        return os;
    }

    return m_driver->Read(address, value);
}

OperationStatus Device::Write(uint32_t address, uint32_t value) const
{
    OperationStatus os = ValidateIoOperation(address);
    if (!os)
    {
        return os;
    }

    return m_driver->Write(address, value);
}

OperationStatus Device::ReadBlock(uint32_t address, uint32_t blockSize, std::vector<uint32_t>& values) const
{
    OperationStatus os = ValidateIoOperation(address);
    if (!os)
    {
        return os;
    }

    Timer rbTimer;
    os = m_driver->ReadBlock(address, blockSize, values);
    rbTimer.Stop();

    LOG_VRB(RDWR) << "Block read."
        << " Address: " << Address(address)
        << " Size: " << blockSize << " (DW)"
        << " Time: " << rbTimer
        << endl;

    return os;
}

OperationStatus Device::ReadBlock(uint32_t address, uint32_t blockSizeInBytes, char *arrBlock) const
{
    OperationStatus os = ValidateIoOperation(address);
    if (!os)
    {
        return os;
    }

    Timer rbTimer;
    os = m_driver->ReadBlock(address, blockSizeInBytes, arrBlock);
    rbTimer.Stop();

    LOG_VRB(RDWR) << "Block read."
        << " Address: " << Address(address)
        << " Size: " << blockSizeInBytes << " (B)"
        << " Time: " << rbTimer
        << endl;

    return os;

}

OperationStatus Device::ReadRadarData(uint32_t maxBlockSize, std::vector<uint32_t>& values) const
{

    OperationStatus os = ValidateIoOperation(0x4 /*dummy*/);
    if (!os)
    {
        return os;
    }

    return m_driver->ReadRadarData(maxBlockSize, values);
}

OperationStatus Device::WriteBlock(uint32_t address, const std::vector<uint32_t>& values) const
{

    OperationStatus os = ValidateIoOperation(address);
    if (!os)
    {
        return os;
    }

    return m_driver->WriteBlock(address, values);
}

OperationStatus Device::WriteBlock(uint32_t address, uint32_t blockSizeInBytes, const char* valuesToWrite) const
{
    OperationStatus os = ValidateIoOperation(address);
    if (!os)
    {
        return os;
    }

    return m_driver->WriteBlock(address, blockSizeInBytes, valuesToWrite);
}

OperationStatus Device::InterfaceReset() const
{
    return m_driver->InterfaceReset();
}

OperationStatus Device::DriverControl(uint32_t Id, const void *inBuf, uint32_t inBufSize, void *outBuf, uint32_t outBufSize) const
{
    if (!m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_WMI))
    {
        ostringstream oss;
        oss << "Unsupported driver command for device " << GetDeviceName() << " working with Debug Mailbox";
        return OperationStatus(false, oss.str());
    }

    return m_driver->DriverControl(Id, inBuf, inBufSize, outBuf, outBufSize);
}

OperationStatus Device::SendWmiWithEvent(const void *inBuf, uint32_t inBufSize, uint32_t eventId, unsigned timeout, vector<unsigned char>& eventBinaryData,
    uint32_t commandId, uint32_t moduleId, uint32_t subtypeId) const
{
    if (!m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_WMI))
    {
        ostringstream oss;
        oss << "Unsupported driver command for device " << GetDeviceName() << " working with Debug Mailbox";
        return OperationStatus(false, oss.str());
    }
    return m_driver->SendWmiWithEvent(inBuf, inBufSize, commandId, moduleId, subtypeId, eventId, timeout, eventBinaryData);
}

OperationStatus Device::AllocPmc(unsigned descSize, unsigned descNum) const
{
    // FW capabilities are available only from ver. 3 and we don't want to interrupt the flow for lower version.
    // We cannot check explicitly for table initialization because of backward compatibility with the driver that
    // doesn't report FW state and so table may stay uninitialized

    bool isSet = false;
    const auto os = m_fwStateProvider->IsCapabilitySet(WMI_FW_CAPABILITY_CONTINUOUS_PMC, isSet);
    if (os && isSet)
    {
        // If we got here, it's a legacy PMC with old driver, but FW is working in continuous mode
        return OperationStatus(false, "Cannot start legacy PMC when FW is working in continuous PMC mode, driver upgrade required");
    }

    return m_driver->AllocPmc(descSize, descNum);
}

OperationStatus Device::DeallocPmc(bool bSuppressError) const
{
    return m_driver->DeallocPmc(bSuppressError);
}

OperationStatus Device::CreatePmcFiles(unsigned refNumber) const
{
    return m_driver->CreatePmcFiles(refNumber);
}

OperationStatus Device::FindPmcDataFile(unsigned refNumber, std::string& foundPmcFilePath) const
{
    return m_driver->FindPmcDataFile(refNumber, foundPmcFilePath);
}

OperationStatus Device::FindPmcRingFile(unsigned refNumber, std::string& foundPmcFilePath) const
{
    return m_driver->FindPmcRingFile(refNumber, foundPmcFilePath);
}

OperationStatus Device::PmcGetData(uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData) const
{
    return m_driver->PmcGetData(bufLenBytesInOut, pBuffer, hasMoreData);
}

OperationStatus Device::GetStaInfo(std::vector<StaInfo>& staInfo) const
{
    if (!m_driver->IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_GET_STA_INFO))
    {
        return OperationStatus(false, "StaInfo command is not supported by the driver");
    }
    return m_driver->GetStaInfo(staInfo);
}

OperationStatus Device::GetFwCapa(std::vector<uint32_t>& fwCapa) const
{
    return m_driver->GetFwCapa(fwCapa);
}
