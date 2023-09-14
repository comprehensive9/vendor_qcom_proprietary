/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcPauseHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcContext.h"
#include "PmcSequenceBase.h"
#include "DeviceManager.h"
#include "Host.h"

void PmcPauseHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, PmcPauseResponse& jsonResponse)
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

    LOG_DEBUG << "PMC pause request for Device: " << spDevice->GetDeviceName() << std::endl;

    os = spDevice->GetPmcContext().GetSequence().Pause();
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());
}
