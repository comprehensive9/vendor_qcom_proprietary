/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcStartHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcSequenceBase.h"
#include "PmcContext.h"
#include "DeviceManager.h"
#include "Host.h"

void PmcStartHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, PmcStartResponse& jsonResponse)
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

    LOG_DEBUG << "PMC start request for Device: " << spDevice->GetDeviceName() << std::endl;

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

    os = spDevice->GetPmcContext().GetSequence().Start();
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());

    // Check if init was invoked implicitly
    const bool implicitTransition = !os.GetStatusMessage().empty();
    if (implicitTransition)
    {
        jsonResponse.SetImplicitInit(implicitTransition);
    }
}
