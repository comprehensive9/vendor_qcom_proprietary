/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcSetFilterHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcContext.h"
#include "PmcSequenceBase.h"
#include "PmcFilterSequence.h"
#include "Host.h"
#include "DeviceManager.h"

void PmcSetFilterHandler::HandleRequest(const PmcSetFilterRequest& jsonRequest, PmcSetFilterResponse& jsonResponse)
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

    // retrieve relevant ring spec
    const auto pRingSpec = spDevice->GetPmcContext().GetSequence().GetPmcRingSpec();
    if (!pRingSpec)
    {
        jsonResponse.Fail("Unknown PMC Ring spec.");
        return;
    }
    if (pRingSpec->PmcMode == PmcModeE::FwPmcMode)
    {
        jsonResponse.Fail("Continuous PMC should be in the Host PMC mode to allow data events collection, current mode is FW");
        return;
    }

    JsonValueBoxed<bool> setCollectIdleSm = jsonRequest.GetCollectIdleSmEvents();
    JsonValueBoxed<bool> setCollectRxPpdu = jsonRequest.GetCollectRxPpduEvents();
    JsonValueBoxed<bool> setCollectTxPpdu = jsonRequest.GetCollectTxPpduEvents();
    JsonValueBoxed<bool> setCollectUCode = jsonRequest.GetCollectUCodeEvents();

    LOG_DEBUG << "Setting PMC filter for " << spDevice->GetDeviceName()
        << " IDLE_SM: " << setCollectIdleSm
        << " RX PPDU: " << setCollectRxPpdu
        << " TX PPDU: " << setCollectTxPpdu
        << " CORE_WRITE: " << setCollectUCode
        << std::endl;

    bool isChangeProvided = false;
    if (setCollectIdleSm.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting PMC IDLE_SM event collection: " << BoolStr(setCollectIdleSm.GetValue()) << std::endl;
        os = spDevice->GetPmcContext().GetFilterSequence().ConfigCollectIdleSm(setCollectIdleSm.GetValue());
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    if (setCollectRxPpdu.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting PMC RX PPDU event collection: " << BoolStr(setCollectRxPpdu) << std::endl;
        os = spDevice->GetPmcContext().GetFilterSequence().ConfigCollectRxPpdu(setCollectRxPpdu.GetValue());
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }
    if (setCollectTxPpdu.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting PMC TX PPDU event collection: " << BoolStr(setCollectTxPpdu) << std::endl;
        os = spDevice->GetPmcContext().GetFilterSequence().ConfigCollectTxPpdu(setCollectTxPpdu.GetValue());
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    if (setCollectUCode.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting PMC uCode event collection: " << BoolStr(setCollectUCode) << std::endl;
        os = spDevice->GetPmcContext().GetFilterSequence().ConfigCollectUcode(setCollectUCode.GetValue());
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    if (!isChangeProvided)
    {
        jsonResponse.Fail("No filter parameters have been provided");
        return;
    }

    //TODO: for now, everything else gets default values
    os = spDevice->GetPmcContext().GetSequence().SetDefaultConfiguration();
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());
}