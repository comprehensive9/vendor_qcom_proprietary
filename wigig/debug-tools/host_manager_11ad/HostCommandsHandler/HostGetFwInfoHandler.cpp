/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "HostGetFwInfoHandler.h"
#include "Device.h"
#include "Host.h"
#include "DeviceManager.h"
#include "DebugLogger.h"
#include "Device.h"
#include "DriverContracts.h"
#include "FwStateProvider.h"

#include <memory>

using namespace std;

void HostGetFwInfoHandler::HandleRequest(const HostGetFwInfoRequest& jsonRequest, HostGetFwInfoResponse& jsonResponse)
{
    LOG_DEBUG << "Got request for get_fw_info" << std::endl;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const AtomicFwInfo fwState = spDevice->GetFwStateProvider()->GetFwInfo();
    LOG_DEBUG << fwState << endl;

    vector<uint32_t> fwCapa;
    os = spDevice->GetFwCapa(fwCapa);
    if (!os)
    {
        LOG_ERROR << "Failed to send driver command: " << os.GetStatusMessage() << std::endl;
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetInfo(fwState, fwCapa);
}

void HostGetFwInfoResponse::SetInfo(const AtomicFwInfo& fwState, vector<uint32_t>& fwCapa)
{
    ostringstream ss;
    ss << fwState.FwUniqueId.m_fwVersion;
    m_jsonResponseValue["FwVersion"] = ss.str();

    ss.str("");
    ss.clear();
    ss << fwState.FwUniqueId.m_fwTimestamp;
    m_jsonResponseValue["FwTimestamp"] = ss.str();

    ss.str("");
    ss.clear();
    ss << fwState.FwUniqueId.m_uCodeTimestamp;
    m_jsonResponseValue["UCodeTimestamp"] = ss.str();

    ss.str("");
    ss.clear();
    ss << fwState.FwHealthState;
    m_jsonResponseValue["FwState"] = ss.str();

    ss.str("");
    ss.clear();
    ss << Hex<>(fwState.FwError);
    m_jsonResponseValue["FwError"] = ss.str();

    ss.str("");
    ss.clear();
    ss << Hex<>(fwState.UCodeError);
    m_jsonResponseValue["UCodeError"] = ss.str();

    static const int bitsInDword = 32;
    int vecIndex = 0;
    Json::Value fwCapaMask(Json::arrayValue);
    Json::Value fwCapaNames(Json::arrayValue);
    for (const auto& capa : fwCapa)
    {
        ostringstream oss;
        oss << Hex<8>(capa);
        fwCapaMask.append(oss.str());

        // go over the bits per dword
        for (int pos = 0; pos < bitsInDword; pos++)
        {
            if (capa & (1 << pos))
            {
                int bit_pos = pos + (vecIndex * bitsInDword);
                fwCapaNames.append(FwCapaToString(static_cast<wmi_fw_capability>(bit_pos)));
            }
        }
        vecIndex++;
    }

    m_jsonResponseValue["FwCapabilityMask"] = fwCapaMask;
    m_jsonResponseValue["FwCapabilities"] = fwCapaNames;
}

// Returns FW capability name corresponding to given FW capability bit or the bit number when capability is not defined
const char* HostGetFwInfoResponse::FwCapaToString(wmi_fw_capability capa)
{
#define CAPA_NAME_FROM_CAPA_BIT(capaBit) case (capaBit): return #capaBit;

    // the only supported message types are RTM_NEWLINK and RTM_DELLINK
    switch (capa)
    {
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_FTM);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_PS_CONFIG);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_RF_SECTORS);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_MGMT_RETRY_LIMIT);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_AP_SME_OFFLOAD_PARTIAL);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_WMI_ONLY);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_MULTI_RF_UT_CMD);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_THERMAL_THROTTLING);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_D3_SUSPEND);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_LONG_RANGE);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_FIXED_SCHEDULING);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_MULTI_DIRECTED_OMNIS);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_RSSI_REPORTING);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_SET_SILENT_RSSI_TABLE);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_LO_POWER_CALIB_FROM_OTP);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_PNO);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_CONNECT_SNR_THR);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_CHANNEL_BONDING);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_REF_CLOCK_CONTROL);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_AP_SME_OFFLOAD_NONE);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_MULTI_VIFS);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_FT_ROAMING);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_BACK_WIN_SIZE_64);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_AMSDU);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_RAW_MODE);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_TX_REQ_EXT);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_CHANNEL_4);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_IPA);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_PREEMPTIVE_RING_SWITCH);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_STOP_PMC);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_TEMPERATURE_ALL_RF);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_SPLIT_REKEY);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_AP_POWER_MANAGEMENT);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_WDS_MODE);
        CAPA_NAME_FROM_CAPA_BIT(WMI_FW_CAPABILITY_PCIE_CONFIG);
    default:
        size_t index = static_cast<size_t>(capa);
        if (capa >= WMI_FW_CAPABILITY_MAX)
        {
            std::ostringstream oss;
            oss << "CAPABILITY_" << index;
            return oss.str().c_str();
        }
    }
    return nullptr;
}