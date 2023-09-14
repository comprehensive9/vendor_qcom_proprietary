/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "ParameterBase.h"
#include "DriverCommandBuilder.h"

using namespace std;

DriverCommandBuilder::DriverCommandBuilder()
{
    SetOpcodeGenerators();
}

void DriverCommandBuilder::SetOpcodeGenerators()
{
    RegisterOpcodeGenerators("wmi", unique_ptr<WmiGenerator>(new WmiGenerator()));
    RegisterOpcodeGenerators("wmi_ut", unique_ptr<WmiUTGenerator>(new WmiUTGenerator()));
    RegisterOpcodeGenerators("sta_info", unique_ptr<StaInfoGenerator>(new StaInfoGenerator()));
    RegisterOpcodeGenerators("driver_command", unique_ptr<DriverCommandGenerator>(new DriverCommandGenerator()));
    RegisterOpcodeGenerators("set_debug_mode", unique_ptr<SetDebugModeGenerator>(new SetDebugModeGenerator()));
    RegisterOpcodeGenerators("interface_reset", unique_ptr<InterfaceResetGenerator>(new InterfaceResetGenerator()));
    RegisterOpcodeGenerators("get_debug_mode", unique_ptr<GetDebugModeGenerator>(new GetDebugModeGenerator()));
    RegisterOpcodeGenerators("get_device_capabilities", unique_ptr<GetDeviceCapabilitiesGenerator>(new GetDeviceCapabilitiesGenerator()));
    RegisterOpcodeGenerators("get_fw_health_state", unique_ptr<GetFwHealthStateGenerator>(new GetFwHealthStateGenerator()));
}

WmiGenerator::WmiGenerator() :
    OpCodeGeneratorBase("Send WMI command to FW")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Id", true, "WMI command id")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("EventId", false, "WMI event id")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Timeout", false, "Maximum time (in sec) to spend waiting for event arrived", "3 sec")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Params", false, "WMI command parameters passed as DWORDs sequence in HEX format (with prefix 0x)")));

    RegisterExample("cmd wmi id=0x80C",
        "Send WMI command READ_RSSI (id 0x80C) with no parameters");
    RegisterExample("cmd wmi id=0x804 eventId=0x1804 Params=0x1",
        "Send WMI command DEEP_ECHO (id 0x804) with reply(eventId 0x1804) and timeout 3 sec. Command parameter: 0x1");

    RegisterNote("Parameters should be padded to DWORD boundary");
    RegisterNote("If device in Sys Assert, make sure you released lock by writing '0x0' to 'FW.g_sysassert_wmi_lock");
}

WmiUTGenerator::WmiUTGenerator() :
    OpCodeGeneratorBase("Send WMI UT command to FW")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("ModuleId", true, "WMI UT command module id")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("SubtypeId", true, "WMI UT command subtype id")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Timeout", false,
        "Maximum time (in sec) to spend waiting for event arrived", "3 sec")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Params", false,
        "WMI UT command parameters passed as DWORDs sequence in HEX format (with prefix 0x)")));

    RegisterExample("cmd wmi_ut ModuleId=0x7 SubTypeId=0x1 Params=0x0,0x1,0x0,0x1,0x0",
        "Send WMI UT command (moduleID=0x7 and subtypeID=0x1) with timeout 3 sec for reply. Command parameters: 0x0,0x1,0x0,0x1,0x0");

    RegisterExample("cmd wmi_ut ModuleId=0x7 SubTypeId=0x0 timeout=5 Params=0x1,0x0",
        "Send WMI UT command (moduleID=0x7 and subtypeID=0x0) with timeout 5 sec for reply. Command parameters: 0x1,0x0");

    RegisterNote("Parameters should be padded to DWORD boundary");
    RegisterNote("WMI UT command is always with reply");
    RegisterNote("If device in Sys Assert, make sure you released lock by writing '0x0' to 'FW.g_sysassert_wmi_lock");
}

StaInfoGenerator::StaInfoGenerator() :
    OpCodeGeneratorBase("Get station information per CID. The command will be replied by a list of information tokens")
{
    RegisterDeviceParam();

    RegisterExample("cmd sta_info", "Send station information query");
}

DriverCommandGenerator::DriverCommandGenerator() :
    OpCodeGeneratorBase("True/False value to turn device debug mode.")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("CmdId", true, "Command ID.")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("BufSize", true, "Size of the buffer")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("DataBase64", true, "Pyaload encoded in base64.")));

    RegisterExample("cmd set_debug_mode DebugMode=true", "Set debug mode.");
}

SetDebugModeGenerator::SetDebugModeGenerator() :
    OpCodeGeneratorBase("True/False value to turn device debug mode.")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("DebugMode", true, "Boolean value to debug mode.")));

    RegisterExample("cmd set_debug_mode DebugMode=true", "Set debug mode.");
}

InterfaceResetGenerator::InterfaceResetGenerator() :
    OpCodeGeneratorBase("Reset device interface")
{
    RegisterDeviceParam();

    RegisterExample("cmd interface_reset", "Reset interface");
}

GetDebugModeGenerator::GetDebugModeGenerator() :
    OpCodeGeneratorBase("Get device debug mode")
{
    RegisterDeviceParam();

    RegisterExample("cmd get_debug_mode", "Get debug mode.");
}

GetDeviceCapabilitiesGenerator::GetDeviceCapabilitiesGenerator() :
    OpCodeGeneratorBase("Get device capabilities")
{
    RegisterDeviceParam();

    RegisterExample("cmd get_device_capabilities", "Get device capabilities.");
}

GetFwHealthStateGenerator::GetFwHealthStateGenerator() :
    OpCodeGeneratorBase("Get FW health state")
{
    RegisterDeviceParam();

    RegisterExample("cmd get_fw_health_state", "Get FW health state.");
}