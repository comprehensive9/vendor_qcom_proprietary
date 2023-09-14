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

#include "CommandsExecutor.h"
#include "Host.h"
#include "DebugLogger.h"
#include "Device.h"
#include "DeviceManager.h"
#include "FwStateProvider.h"
#include "HostInfo.h"
#include "HostAndDeviceDataDefinitions.h"
#include "SharedVersionInfo.h"
#include "Utils.h"

#include <array>
#include <sstream>
#include <algorithm>

using namespace std;

// FW Defines
#define MAX_RF_NUMBER 8
#define MAX_MCS_RANGE_SPARROW 12
#define MAX_MCS_RANGE_TALYN 16

#define FW_ASSERT_REG 0x91F020
#define UCODE_ASSERT_REG 0x91F028
#define FW_ASSOCIATION_REG 0x880A44
#define FW_ASSOCIATED_VALUE 0x6
#define MCS_REG 0x880A60
#define DEVICE_TYPE_REG 0x880A8C
#define FW_MODE_REG 0x880A34
#define BOOT_LOADER_VERSION_REG 0x880A0C
#define CHANNEL_REG_PRE_TALYN_MA 0x883020
#define CHANNEL_REG_POST_TALYN_MA 0x88941C
#define BOARDFILE_REG 0x880014

#define UCODE_RX_ON_REG 0x94059E
#define BF_SEQ_REG 0x941374
#define BF_TRIG_REG 0x941380
#define NAV_REG 0x940490
#define TX_GP_REG 0x880A58
#define RX_GP_REG 0x880A5C

#define RF_STATE_REG 0x889488

#define BASEBAND_TEMPERATURE_REG 0x91c808
#define RF_TEMPERATURE_REG 0x91c80c

// *************************************************************************************************

namespace
{
    // Initialize translation maps for the front-end data
    const static std::string sc_noRfStr("NO_RF");
    const std::unordered_map<int, std::string> rfTypeToString = { {0, sc_noRfStr }, {1, "MARLON"}, {2, "SPR-R"}, {3, "TLN-A1"}, { 4, "TLN-A2" }, { 5, "BRL-A1"} };

    const static int sc_TalynMaBasebandType = 8;
    const std::unordered_map<int, std::string> basebandTypeToString =
    {
        { 0, "UNKNOWN" }, { 1, "MAR-DB1" }, { 2, "MAR-DB2" },
        { 3, "SPR-A0"  }, { 4, "SPR-A1"  }, { 5, "SPR-B0"  },
        { 6, "SPR-C0"  }, { 7, "SPR-D0"  }, { sc_TalynMaBasebandType, "TLN-M-A0"  },
        { 9, "TLN-M-B0" }, { 10, "TLN-M-C0" }, { 20, "BRL-P1"}
    };

    // generic board file id and corresponding name, other ids are displayed as is
    const static std::pair<uint32_t, std::string> sc_genericBoardFileDescription = std::pair<uint32_t, std::string>(65541, "Falcon Free Space"); // 0x10005
    const static std::array<std::string, 5U> sc_fwType = { { "Operational", "WMI Only", "No PCIe", "WMI Only - No PCIe", "IF2IF" } };
}

// *************************************************************************************************
bool CommandsExecutor::GetHostData(HostData& data, std::string& failureReason)
{
    LOG_VERBOSE << __FUNCTION__ << endl;

    // failure reason is not in use for now
    // may fill with more info (if available) to show in UI
    (void)failureReason;

    // Extract host_manager version
    data.m_hostManagerVersion = GetToolsVersion();

    // Extract host Alias
    data.m_hostName = Host::GetHost().GetHostInfo().GetHostName();

    // Update devices status

    vector<shared_ptr<Device>> devices = Host::GetHost().GetDeviceManager().GetDevices();
    vector<DeviceData> devicesData;
    for (auto& spDevice : devices)
    {
        // Create device data
        DeviceData deviceData;

        // Extract FW version
        AtomicFwInfo fwState = spDevice->GetFwStateProvider()->GetFwInfo();
        deviceData.m_fwVersion = fwState.FwUniqueId.m_fwVersion;

        uint32_t value = Utils::REGISTER_DEFAULT_VALUE;

        deviceData.m_fwAssert = fwState.FwError;
        deviceData.m_uCodeAssert = fwState.UCodeError;

        // Read FW association state
        spDevice->Read(FW_ASSOCIATION_REG, value);
        deviceData.m_associated = (value == FW_ASSOCIATED_VALUE);

        // Get FW compilation timestamp
        deviceData.m_compilationTime = fwState.FwUniqueId.m_fwTimestamp;

        // Get Device name
        deviceData.m_deviceName = spDevice->GetDeviceName();

        // Get baseband name & RF type
        // BB type is stored in 2 lower bytes of device type register
        // RF type is stored in 2 upper bytes of device type register
        spDevice->Read(DEVICE_TYPE_REG, value);
        const int basebandTypeValue = value & 0xFFFF;
        const auto basebandTypeIter = basebandTypeToString.find(basebandTypeValue);
        deviceData.m_hwType = basebandTypeIter != basebandTypeToString.cend() ? basebandTypeIter->second : std::string("UNKNOWN");
        const auto rfTypeIter = rfTypeToString.find((value & 0xFFFF0000) >> 16);
        deviceData.m_rfType = rfTypeIter != rfTypeToString.cend() ? rfTypeIter->second : sc_noRfStr;

        // Read MCS value and set signal strength (maximum is 5 bars)
        const int maxMcsRange = (basebandTypeValue < sc_TalynMaBasebandType) ? MAX_MCS_RANGE_SPARROW : MAX_MCS_RANGE_TALYN;
        spDevice->Read(MCS_REG, value);
        deviceData.m_mcs = value;
        deviceData.m_signalStrength = static_cast<int>(value * 5.0 / maxMcsRange + 0.5);

        // Get FW mode
        spDevice->Read(FW_MODE_REG, value);
        deviceData.m_mode = value < sc_fwType.size() ? sc_fwType[value] : "NA";

        // Get boot loader version
        spDevice->Read(BOOT_LOADER_VERSION_REG, value);
        std::ostringstream oss;
        oss << value;
        deviceData.m_bootloaderVersion = oss.str();

        // Get channel number
        int Channel = 0;
        if (basebandTypeValue < sc_TalynMaBasebandType)
        {
            spDevice->Read(CHANNEL_REG_PRE_TALYN_MA, value);
            switch (value)
            {
            case 0x64FCACE:
                Channel = 1;
                break;
            case 0x68BA2E9:
                Channel = 2;
                break;
            case 0x6C77B03:
                Channel = 3;
                break;
            default:
                Channel = 0;
            }
        }
        else
        {
            spDevice->Read(CHANNEL_REG_POST_TALYN_MA, value);
            value = (value & 0xF0000) >> 16; // channel value is contained in bits [16-19]
            // the following is an essence of translation from channel value to the channel number
            Channel = value <= 5 ? value + 1 : value + 3;
        }
        deviceData.m_channel = Channel;

        // Get board file version
        spDevice->Read(BOARDFILE_REG, value);
        if (((value & 0xFFFFF000) >> 12) == sc_genericBoardFileDescription.first) // bits [12-31] are 0x10005
        {
            deviceData.m_boardFile = sc_genericBoardFileDescription.second;
        }
        else
        {
            oss.str(std::string());
            oss << value;
            deviceData.m_boardFile = oss.str();
        }

        // Lower byte of RF state contains connection bit-mask
        // Second byte of RF state contains enabled bit-mask
        uint32_t rfConnected = 0;
        spDevice->Read(RF_STATE_REG, rfConnected);

        // Note: RF state address is not constant.
        //       As workaround for SPR-B0 is to mark its single RF as enabled (when RF present)
        // TODO: Fix after moving the RF state to a constant address
        if (deviceData.m_hwType == "SPR-B0" && deviceData.m_rfType != sc_noRfStr)
        {
            rfConnected = 0x101; // connected & enabled
        }

        uint32_t rfEnabled = rfConnected >> 8;

        // Get RF state of each RF
        deviceData.m_rf.reserve(MAX_RF_NUMBER);
        for (int rfIndex = 0; rfIndex < MAX_RF_NUMBER; ++rfIndex)
        {
            int rfState = 0; // disabled

            if (rfConnected & (1 << rfIndex))
            {
                rfState = 1; // connected
                if (rfEnabled & (1 << rfIndex))
                {
                    rfState = 2; // enabled
                }
            }

            deviceData.m_rf.push_back(rfState);
        }

        ////////// Get fixed registers values //////////////////////////
        StringNameValuePair registerData;

        // uCode Rx on fixed reg
        spDevice->Read(UCODE_RX_ON_REG, value);
        uint32_t UcRxonhexVal16 = value & 0xFFFF;
        string UcRxon;
        switch (UcRxonhexVal16)
        {
        case 0:
            UcRxon = "RX_OFF";
            break;
        case 1:
            UcRxon = "RX_ONLY";
            break;
        case 2:
            UcRxon = "RX_ON";
            break;
        default:
            UcRxon = "Unrecognized";
        }
        registerData.m_name = "uCodeRxOn";
        registerData.m_value = UcRxon;
        deviceData.m_fixedRegisters.insert(deviceData.m_fixedRegisters.end(), registerData);

        // BF Sequence fixed reg
        spDevice->Read(BF_SEQ_REG, value);
        oss.str(std::string());
        oss << value;
        registerData.m_name = "BF_Seq";
        registerData.m_value = oss.str();
        deviceData.m_fixedRegisters.insert(deviceData.m_fixedRegisters.end(), registerData);

        // BF Trigger fixed reg
        spDevice->Read(BF_TRIG_REG, value);
        string BF_TRIG = "";
        switch (value)
        {
        case 1:
            BF_TRIG = "MCS1_TH_FAILURE";
            break;
        case 2:
            BF_TRIG = "MCS1_NO_BACK";
            break;
        case 4:
            BF_TRIG = "NO_CTS_IN_TXOP";
            break;
        case 8:
            BF_TRIG = "MAX_BCK_FAIL_TXOP";
            break;
        case 16:
            BF_TRIG = "FW_TRIGGER ";
            break;
        case 32:
            BF_TRIG = "MAX_BCK_FAIL_ION_KEEP_ALIVE";
            break;
        default:
            BF_TRIG = "UNDEFINED";
        }
        registerData.m_name = "BF_Trig";
        registerData.m_value = BF_TRIG;
        deviceData.m_fixedRegisters.insert(deviceData.m_fixedRegisters.end(), registerData);

        // Get NAV fixed reg
        spDevice->Read(NAV_REG, value);
        registerData.m_name = "NAV";
        oss.str(std::string());
        oss << value;
        registerData.m_value = oss.str();
        deviceData.m_fixedRegisters.insert(deviceData.m_fixedRegisters.end(), registerData);

        // Get TX Goodput fixed reg
        spDevice->Read(TX_GP_REG, value);
        string TX_GP = "NO_LINK";
        if (value != 0)
        {
            oss.str(std::string());
            oss << value;
            TX_GP = oss.str();
        }
        registerData.m_name = "TX_GP";
        registerData.m_value = TX_GP;
        deviceData.m_fixedRegisters.insert(deviceData.m_fixedRegisters.end(), registerData);

        // Get RX Goodput fixed reg
        spDevice->Read(RX_GP_REG, value);
        string RX_GP = "NO_LINK";
        if (value != 0)
        {
            oss.str(std::string());
            oss << value;
            RX_GP = oss.str();
        }
        registerData.m_name = "RX_GP";
        registerData.m_value = RX_GP;
        deviceData.m_fixedRegisters.insert(deviceData.m_fixedRegisters.end(), registerData);
        ////////////// Fixed registers end /////////////////////////

        ////////////// Custom registers ////////////////////////////
        spDevice->ReadCustomRegisters(deviceData.m_customRegisters);
        ////////////// Custom registers end ////////////////////////

        ////////////// Temperatures ////////////////////////////////
        // Baseband
        spDevice->Read(BASEBAND_TEMPERATURE_REG, value);
        float temperature = (float)value / 1000;
        oss.str(std::string());
        oss.precision(2);
        oss << fixed << temperature;
        deviceData.m_hwTemp = oss.str();

        // RF
        if (deviceData.m_rfType != sc_noRfStr)
        {
            spDevice->Read(RF_TEMPERATURE_REG, value);
            temperature = (float)value / 1000;
            oss.str(std::string());
            oss.precision(2);
            oss << fixed << temperature;
            deviceData.m_rfTemp = oss.str();
        }
        else // no RF, temperature value is not relevant
        {
            deviceData.m_rfTemp = "";
        }
        ////////////// Temperatures end ///////////////////////////

        // AddChildNode the device to the devices list
        devicesData.insert(devicesData.end(), deviceData);
    }

    return true;
}

bool CommandsExecutor::AddDeviceRegister(const std::string& deviceName, const string& name, uint32_t address, uint32_t firstBit, uint32_t lastBit, std::string& failureReason)
{
    LOG_VERBOSE << __FUNCTION__ << endl;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(deviceName, spDevice);
    if (!os)
    {
        failureReason = os.GetStatusMessage();
        return false;
    }

    os = spDevice->AddCustomRegister(name, address, firstBit, lastBit);
    if (!os)
    {
        failureReason = os.GetStatusMessage();
        return false;
    }

    return true;

}

bool CommandsExecutor::RemoveDeviceRegister(const std::string& deviceName, const string& name, std::string& failureReason)
{
    LOG_VERBOSE << __FUNCTION__ << endl;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(deviceName, spDevice);
    if (!os)
    {
        failureReason = os.GetStatusMessage();
        return false;
    }

    os = spDevice->RemoveCustomRegister(name);
    if (!os)
    {
        failureReason = os.GetStatusMessage();
        return false;
    }

    return true;
}

//for testing purposes only
static string hostAlias = "STA-1021";
static vector<StringNameValuePair> customRegs[3] =
{
    std::vector<StringNameValuePair>(),
    std::vector<StringNameValuePair>(),
    std::vector<StringNameValuePair>{ { "Test1", "0x87" },{ "Test2", "OK" },{ "Test3", "0" }}
};
static std::array<string, 3> deviceNames{ {"wlan0", "wlan1", "wlan2"} };

bool CommandsExecutor::GetTestHostData(HostData& data, std::string& /*failureReason*/)
{
    static int counter = 0;
    static bool associated = true;
    static int rfStatus = 0;
    if (counter >= 10)
    {
        counter = 0;
        associated = !associated;
        rfStatus++;
        if (rfStatus >= 3)
        {
            rfStatus = 0;
        }
    }
    counter++;

    data.m_hostManagerVersion = "1.1.2";
    data.m_hostName = hostAlias;

    DeviceData device1;
    device1.m_deviceName = deviceNames[0];
    device1.m_associated = associated;
    device1.m_signalStrength = 5;
    device1.m_fwAssert = 0;
    device1.m_uCodeAssert = 0;
    device1.m_mcs = 7;
    device1.m_channel = 1;
    device1.m_fwVersion.m_major = 5;
    device1.m_fwVersion.m_minor = 1;
    device1.m_fwVersion.m_subMinor = 0;
    device1.m_fwVersion.m_build = 344;
    device1.m_bootloaderVersion = "7253";
    device1.m_mode = "Operational";
    device1.m_compilationTime.m_hour = 14;
    device1.m_compilationTime.m_min = 52;
    device1.m_compilationTime.m_sec = 1;
    device1.m_compilationTime.m_day = 7;
    device1.m_compilationTime.m_month = 8;
    device1.m_compilationTime.m_year = 2017;
    device1.m_hwType = "SPR-D0";
    device1.m_hwTemp = "32.1";
    device1.m_rfType = "SPR-R";
    device1.m_rfTemp = "32.1";
    device1.m_boardFile = "Generic_500mW";
    device1.m_rf.push_back(2);
    device1.m_rf.push_back(1);
    device1.m_rf.push_back(2);
    device1.m_rf.push_back(1);
    device1.m_rf.push_back(rfStatus);
    device1.m_rf.push_back(0);
    device1.m_rf.push_back(0);
    device1.m_rf.push_back(0);
    device1.m_fixedRegisters.push_back({ "uCodeRxOn", "Rx On" });
    device1.m_fixedRegisters.push_back({ "BfSeq", "2" });
    device1.m_fixedRegisters.push_back({ "BfTrigger", "FW_TRIG" });
    device1.m_fixedRegisters.push_back({ "NAV", "5" });
    device1.m_fixedRegisters.push_back({ "TxGP", "1001" });
    device1.m_fixedRegisters.push_back({ "RxGP", "547" });
    device1.m_customRegisters = customRegs[0];
    data.m_devices.push_back(device1);

    DeviceData device2;
    device2.m_deviceName = deviceNames[1];
    device2.m_associated = false;
    device2.m_signalStrength = 0;
    device2.m_fwAssert = 0;
    device2.m_uCodeAssert = 0;
    device2.m_mcs = 0;
    device2.m_channel = 0;
    device2.m_fwVersion.m_major = 4;
    device2.m_fwVersion.m_minor = 1;
    device2.m_fwVersion.m_subMinor = 0;
    device2.m_fwVersion.m_build = 1;
    device2.m_bootloaderVersion = "7253";
    device2.m_mode = "Operational";
    device2.m_compilationTime.m_hour = 11;
    device2.m_compilationTime.m_min = 11;
    device2.m_compilationTime.m_sec = 11;
    device2.m_compilationTime.m_day = 5;
    device2.m_compilationTime.m_month = 8;
    device2.m_compilationTime.m_year = 2017;
    device2.m_hwType = "SPR-D1";
    device2.m_hwTemp = "22.1";
    device2.m_rfType = "SPR-R";
    device2.m_rfTemp = "22.1";
    device2.m_boardFile = "Generic_400mW";
    device2.m_rf.push_back(2);
    device2.m_rf.push_back(1);
    device2.m_rf.push_back(2);
    device2.m_rf.push_back(1);
    device2.m_rf.push_back(0);
    device2.m_rf.push_back(0);
    device2.m_rf.push_back(0);
    device2.m_rf.push_back(0);
    device2.m_fixedRegisters.push_back({ "uCodeRxOn", "Rx On" });
    device2.m_fixedRegisters.push_back({ "BfSeq", "1" });
    device2.m_fixedRegisters.push_back({ "BfTrigger", "FW_TRIG" });
    device2.m_fixedRegisters.push_back({ "NAV", "5" });
    device2.m_fixedRegisters.push_back({ "TxGP", "1" });
    device2.m_fixedRegisters.push_back({ "RxGP", "1" });
    device2.m_customRegisters = customRegs[1];
    data.m_devices.push_back(device2);

    DeviceData device3;
    device3.m_deviceName = deviceNames[2];
    device3.m_associated = false;
    device3.m_signalStrength = 0;
    device3.m_fwAssert = 0x29;
    device3.m_uCodeAssert = 0xff;
    device3.m_mcs = 0;
    device3.m_channel = 2;
    device3.m_fwVersion.m_major = 3;
    device3.m_fwVersion.m_minor = 2;
    device3.m_fwVersion.m_subMinor = 0;
    device3.m_fwVersion.m_build = 5;
    device3.m_bootloaderVersion = "7253";
    device3.m_mode = "Error";
    device3.m_compilationTime.m_hour = 14;
    device3.m_compilationTime.m_min = 52;
    device3.m_compilationTime.m_sec = 1;
    device3.m_compilationTime.m_day = 7;
    device3.m_compilationTime.m_month = 8;
    device3.m_compilationTime.m_year = 2017;
    device3.m_hwType = "SPR-D1";
    device3.m_hwTemp = "22.1";
    device3.m_boardFile = "Generic_400mW";
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_rf.push_back(0);
    device3.m_fixedRegisters.push_back({ "uCodeRxOn", "Rx Off" });
    device3.m_fixedRegisters.push_back({ "BfSeq", "1" });
    device3.m_fixedRegisters.push_back({ "NAV", "4" });
    device3.m_fixedRegisters.push_back({ "RxGP", "0" });
    device3.m_customRegisters = customRegs[2];

    data.m_devices.push_back(device3);

    return true;
}

bool CommandsExecutor::SetTestHostAlias(const std::string& name, std::string& /*failureReason*/)
{
    hostAlias = name;
    return true;
}

bool CommandsExecutor::AddTestDeviceRegister(const std::string& deviceName, const std::string& name, uint32_t address, std::string& /*failureReason*/)
{
    for (size_t i = 0; i < deviceNames.size(); i++)
    {
        if (deviceNames[i] == deviceName)
        {
            auto elem = std::find_if(std::begin(customRegs[i]), std::end(customRegs[i]),
                [&name](const StringNameValuePair& item) {return item.m_name == name; });
            if (std::end(customRegs[i]) == elem)
            {
                std::stringstream ss;
                ss << address;
                customRegs[i].push_back({ name, ss.str() });
                return true;
            }
        }
    }
    return false;
}

bool CommandsExecutor::RemoveTestDeviceRegister(const std::string& deviceName, const std::string& name, std::string& /*failureReason*/)
{
    for (size_t i = 0; i < deviceNames.size(); i++)
    {
        if (deviceNames[i] == deviceName)
        {
            auto elem = std::find_if(std::begin(customRegs[i]), std::end(customRegs[i]),
                [&name](const StringNameValuePair& item) {return item.m_name == name; });
            if (std::end(customRegs[i]) != elem)
            {
                customRegs[i].erase(elem);
                return true;
            }
        }
    }
    return false;
}