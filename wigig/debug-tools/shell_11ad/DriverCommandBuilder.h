/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "GroupCommandBuilderBase.h"
#include "OpCodeGeneratorBase.h"

class DriverCommandBuilder : public GroupCommandBuilderBase
{
public:
    DriverCommandBuilder();

private:
    void SetOpcodeGenerators() override;
};

class WmiGenerator : public OpCodeGeneratorBase
{
public:
    WmiGenerator();
};

class WmiUTGenerator : public OpCodeGeneratorBase
{
public:
    WmiUTGenerator();
};

class StaInfoGenerator : public OpCodeGeneratorBase
{
public:
    StaInfoGenerator();
};

class SetDebugModeGenerator : public OpCodeGeneratorBase
{
public:
    SetDebugModeGenerator();
};

class InterfaceResetGenerator : public OpCodeGeneratorBase
{
public:
    InterfaceResetGenerator();
};


class GetDebugModeGenerator : public OpCodeGeneratorBase
{
public:
    GetDebugModeGenerator();
};


class GetDeviceCapabilitiesGenerator : public OpCodeGeneratorBase
{
public:
    GetDeviceCapabilitiesGenerator();
};


class GetFwHealthStateGenerator : public OpCodeGeneratorBase
{
public:
    GetFwHealthStateGenerator();
};

class DriverCommandGenerator : public OpCodeGeneratorBase
{
public:
    DriverCommandGenerator();
};

