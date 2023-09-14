/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _HOST_COMMAND_BUILDER_
#define _HOST_COMMAND_BUILDER_

#include "GroupCommandBuilderBase.h"
#include "OpCodeGeneratorBase.h"

class HostCommandBuilder : public GroupCommandBuilderBase
{
public:
    HostCommandBuilder();

private:
    void SetOpcodeGenerators() override;
};

// host get_state
class HCGetStateGenerator : public OpCodeGeneratorBase
{
public:
    HCGetStateGenerator();
};

// host get_info
class HCGetInfoGenerator : public OpCodeGeneratorBase
{
public:
    HCGetInfoGenerator();
};

// host get_version
class HCGetVersionGenerator : public OpCodeGeneratorBase
{
public:
    HCGetVersionGenerator();
};

// host get_fw_info
class HCGetFwInfoGenerator : public OpCodeGeneratorBase
{
public:
    HCGetFwInfoGenerator();
};

// host set_enumeration_mode
class HCSetEnumerationModeGenerator : public OpCodeGeneratorBase
{
public:
    HCSetEnumerationModeGenerator();
};

// host get_interfaces
class HCGetInterfacesGenerator : public OpCodeGeneratorBase
{
public:
    HCGetInterfacesGenerator();
};

#endif // _LOG_COLLECTOR_BUILDER_
