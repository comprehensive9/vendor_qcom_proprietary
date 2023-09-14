/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcGetFilterHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcContext.h"
#include "DeviceManager.h"
#include "Host.h"

void PmcGetFilterHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, PmcGetFilterResponse& jsonResponse)
{
    LOG_DEBUG << "PMC get filter request" << std::endl;

    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    std::shared_ptr<Device> spDevice;
    const OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const auto filter = spDevice->GetPmcContext().GetDeviceConfigurations();
    LOG_DEBUG << "Obtained PMC filter. " << filter << std::endl;

    jsonResponse.SetCollectIdleSmEvents(filter.IsCollectIdleSmEvents());
    jsonResponse.SetCollectRxPpduEvents(filter.IsCollectRxPpduEvents());
    jsonResponse.SetCollectTxPpduEvents(filter.IsCollectTxPpduEvents());
    jsonResponse.SetCollectUCodeEvents(filter.IsCollectUcodeEvents());

    jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());
}
