/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DriverCommandDispatcher.h"
#include "DriverCommandsHandler.h"
#include "WmiHandler.h"

DriverCommandDispatcher::DriverCommandDispatcher()
{
    RegisterOpCodeHandler("wmi", std::unique_ptr<IJsonHandler>(new WmiHandler()));
    RegisterOpCodeHandler("wmi_ut", std::unique_ptr<IJsonHandler>(new WmiUTHandler()));
    RegisterOpCodeHandler("sta_info", std::unique_ptr<IJsonHandler>(new StaInfoHandler()));

    RegisterOpCodeHandler("driver_command", std::unique_ptr<IJsonHandler>(new DriverCommandHandler()));
    RegisterOpCodeHandler("set_debug_mode", std::unique_ptr<IJsonHandler>(new SetDebugModeHandler()));
    RegisterOpCodeHandler("get_debug_mode", std::unique_ptr<IJsonHandler>(new GetDebugModeHandler()));
    RegisterOpCodeHandler("interface_reset", std::unique_ptr<IJsonHandler>(new InterfaceResetHandler()));
    RegisterOpCodeHandler("get_device_capabilities", std::unique_ptr<IJsonHandler>(new GetDeviceCapabilitiesHandler()));
    RegisterOpCodeHandler("get_fw_health_state", std::unique_ptr<IJsonHandler>(new GetFwHealthStateHandler()));
}
