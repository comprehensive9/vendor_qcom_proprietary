/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcStopHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcContext.h"
#include "PmcSequenceBase.h"
#include "DeviceManager.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "Utils.h"

void PmcStopResponse::SetPmcData(const std::vector<uint8_t>& ringData, uint32_t numberOfDescriptors, uint32_t ringPayloadSizeBytes)
{
    const auto* ringDataBuf = reinterpret_cast<const unsigned char*>(ringData.data());
    m_jsonResponseValue["RingDataBase64"] = Utils::Base64Encode(ringDataBuf, ringData.size());
    m_jsonResponseValue["NumberOfDescriptors"] = numberOfDescriptors;
    m_jsonResponseValue["RingPayloadSizeBytes"] = ringPayloadSizeBytes;
}

void PmcStopHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, PmcStopResponse& jsonResponse)
{
    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    LOG_DEBUG << "PMC stop request for Device: " << spDevice->GetDeviceName() << std::endl;

    // retrieve relevant ring spec
    const auto pRingSpec = spDevice->GetPmcContext().GetSequence().GetPmcRingSpec();
    if (!pRingSpec)
    {
        jsonResponse.Fail("Unknown PMC Ring spec.");
        return;
    }

    switch (jsonRequest.GetClientType())
    {
    case ClientType::SHELL_11_AD:
        os = spDevice->GetPmcContext().GetSequence().StopForOnTarget();
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }

        jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());
        jsonResponse.SetRecordingDirectory(os.GetStatusMessage().c_str());
        break;
    case ClientType::DM_TOOLS:
    {
        std::vector<uint8_t> pmcData;
        os = spDevice->GetPmcContext().GetSequence().StopForDmTools(pmcData);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }

        jsonResponse.SetPmcData(pmcData, pRingSpec->PmcRingDescriptorsCount, pRingSpec->PmcRingPayloadSizeBytes);
        break;
    }
    default:
        jsonResponse.Fail("Unsupported client type provided in request, expected 'shell_11ad or DmTools'");
        return;
    }

    // Note: In Legacy mode, when the PMC is stopped after SysAssert, the ring deallocation request returns error (-11).
    // We should ignore this error to have the graph rendered.
}
