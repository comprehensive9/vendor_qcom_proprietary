/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "HostCommandBuilder.h"

using namespace std;


HostCommandBuilder::HostCommandBuilder()
{
    SetOpcodeGenerators();
}

void HostCommandBuilder::SetOpcodeGenerators()
{
    RegisterOpcodeGenerators("get_state", unique_ptr<HCGetStateGenerator>(new HCGetStateGenerator()));
    RegisterOpcodeGenerators("get_version", unique_ptr<HCGetVersionGenerator>(new HCGetVersionGenerator()));
    RegisterOpcodeGenerators("get_fw_info", unique_ptr<HCGetFwInfoGenerator>(new HCGetFwInfoGenerator()));
    RegisterOpcodeGenerators("get_info", unique_ptr<HCGetInfoGenerator>(new HCGetInfoGenerator()));
    RegisterOpcodeGenerators("set_enumeration", unique_ptr<HCSetEnumerationModeGenerator>(new HCSetEnumerationModeGenerator()));
    RegisterOpcodeGenerators("get_interfaces", unique_ptr<HCGetInterfacesGenerator>(new HCGetInterfacesGenerator()));
}


HCGetStateGenerator::HCGetStateGenerator() :
    OpCodeGeneratorBase("Get host's state: Connected clients, interfaces and enumeration mode.")
{
    RegisterExample("host get_state",
        "Get a list of interfaces, connected clients and enumeration mode.");
}

HCGetInfoGenerator::HCGetInfoGenerator() :
    OpCodeGeneratorBase("Get host's info: capabilities, OS, platform, alias.")
{
    RegisterExample("host get_info",
        "Get host's info.");
}

HCGetVersionGenerator::HCGetVersionGenerator() :
    OpCodeGeneratorBase("Get Host manager version on target machine")
{
    RegisterExample("host get_version",
        "Get Host manager version");
}

HCGetFwInfoGenerator::HCGetFwInfoGenerator() :
    OpCodeGeneratorBase("Get FW info from device: FW version, FW timestamp, UCODE timestamp")
{
    RegisterDeviceParam();
    RegisterExample("host get_fw_info",
        "Get FW info");
    RegisterExample("host get_fw_info Device=wlan0",
        "Get FW info from device wlan0");
}

HCSetEnumerationModeGenerator::HCSetEnumerationModeGenerator() :
    OpCodeGeneratorBase("True/False value to set device enumeration mode.")
{
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter(
        "Mode", true, "enumeration mode.")));
    RegisterExample("host set_enumeration Mode=true",
        "Turn on device enumeration mode");
}

HCGetInterfacesGenerator::HCGetInterfacesGenerator() :
    OpCodeGeneratorBase("Get a list of host's available interfaces")
{
    RegisterExample("host get_interfaces",
        "Get a list of devices");
}

