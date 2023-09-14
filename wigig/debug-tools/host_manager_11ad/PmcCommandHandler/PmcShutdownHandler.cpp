/*
* Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
*
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcShutdownHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcContext.h"
#include "PmcSequenceBase.h"
#include "DeviceManager.h"
#include "Host.h"

void PmcShutdownHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, PmcShutdownResponse& jsonResponse)
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

    LOG_DEBUG << "PMC Shutdown request for Device: " << spDevice->GetDeviceName() << std::endl;

    os = spDevice->GetPmcContext().GetSequence().Shutdown();
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());
    LOG_INFO << "Performing PMC Shutdown on device: " << spDevice->GetDeviceName() << std::endl;
}
