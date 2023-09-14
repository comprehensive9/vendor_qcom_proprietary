/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "AccessCommandBuilder.h"
#include "ParameterBase.h"

using namespace std;

AccessCommandBuilder::AccessCommandBuilder()
{
    SetOpcodeGenerators();
}

void AccessCommandBuilder::SetOpcodeGenerators()
{
    RegisterOpcodeGenerators("read", unique_ptr<ACReadGenerator>(new ACReadGenerator()));
    RegisterOpcodeGenerators("write", unique_ptr<ACWriteGenerator>(new ACWriteGenerator()));
    RegisterOpcodeGenerators("read_block", unique_ptr<ACReadBlockGenerator>(new ACReadBlockGenerator()));
    RegisterOpcodeGenerators("write_block", unique_ptr<ACWriteBlockGenerator>(new ACWriteBlockGenerator()));
    RegisterOpcodeGenerators("dump", unique_ptr<DumpGenerator>(new DumpGenerator()));
}

ACReadGenerator::ACReadGenerator() :
    OpCodeGeneratorBase("Read 32-bit DWORD form the device")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Address", true, "address to read from")));
    RegisterExample("access read Address=0x880050",
        "Read one DWORD from address 0x880050");
    RegisterExample("access read Device=wlan0 Address=0x880050",
        "Read one DWORD from address 0x880050 of device wlan0");
}

ACWriteGenerator::ACWriteGenerator() :
    OpCodeGeneratorBase("Write 32-bit DWORD to the device")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Address", true, "address to read from")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Value", true, "one DWORD to write")));
    RegisterExample("access write Address=0x880050 Value=0xAAAA",
        "Write 0xAAAA to address 0x880050");
    RegisterExample("access write Device=wlan0 Address=0x880050 Value=0",
        "Write 0 to address 0x880050 of device wlan0");
}

ACReadBlockGenerator::ACReadBlockGenerator() :
OpCodeGeneratorBase("Read a block of 32-bit DWORDs from the device")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Address", true, "start address to read from")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Size", true, "size in DWORDs")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("EncodedReply", false, "read values encoded base64")));
    RegisterExample("access read_block Address=0x880050 Size=16",
        "Read 16 DWORDS from address 0x880050");
    RegisterExample("access read_block Device=wlan0 Address=0x880050 Size=16",
        "Read 16 DWORDS from address 0x880050 of device wlan0");
}

ACWriteBlockGenerator::ACWriteBlockGenerator() :
    OpCodeGeneratorBase("Write a block of 32-bit DWORDs from the device")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("Address", true, "start address to read from")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Values", true, "values delimited by ',' ")));
    RegisterExample("access write_block Address=0x880050 Values=0xAAAA,0xAAAA,0xAAAA,0xAAAA",
        "Write values from address 0x880050");
    RegisterExample("access write_block Device=wlan0 Address=0x880050 Values=0xAAAA,0xAAAA,0xAAAA,0xAAAA",
        "Write values from address 0x880050 of device wlan0");
}


DumpGenerator::DumpGenerator() :
    OpCodeGeneratorBase("Create a device memory dump. The resulting dump folder will be reported by the command")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter(
        "Name", false, "dump name. If not provided, a default name will be assigned")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter(
        "Location", false, "directory to save the dump in. If not provided, a default one will be chosen", "valid directory name")));

    RegisterExample("access dump",
        "Create device memory dump in the default location");
    RegisterExample("access dump Device=wlan0 Location=/tmp/ Name=Test1",
        "Create device memory dump named Test1 for wlan0 in /tmp/");
}
