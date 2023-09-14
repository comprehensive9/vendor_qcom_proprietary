/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcGetRecordingStateHandler.h"
#include "SharedVersionInfo.h"
#include "Device.h"
#include "PmcSequenceBase.h"
#include "PmcContext.h"
#include "DeviceManager.h"
#include "Host.h"

void PmcGetRecordingStateHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, PmcGetRecordingStateResponse& jsonResponse)
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

    uint32_t pmcGeneral0Value = 0;
    os = spDevice->GetPmcContext().GetRegisterAccessor().ReadPmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_0", pmcGeneral0Value);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    uint32_t pmcGeneral1Value = 0;
    os = spDevice->GetPmcContext().GetRegisterAccessor().ReadPmcRegister("MAC_RGF.PMC.GENERAL.MAC_MAC_RGF_PMC_GENERAL_1", pmcGeneral1Value);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetPmcGeneral0(pmcGeneral0Value);
    jsonResponse.SetPmcGeneral1(pmcGeneral1Value);

    bool recActive = false;
    os = spDevice->GetPmcContext().GetSequence().IsPmcRecordingActive(recActive);
    if(!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetHwIsActive(recActive);

    // update logic state
    jsonResponse.SetCurrentState(spDevice->GetPmcContext().GetDeviceRecordingState().GetState());
}