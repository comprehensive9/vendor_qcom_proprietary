/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "GroupCommandBuilderBase.h"
#include "OpCodeGeneratorBase.h"

class AccessCommandBuilder : public GroupCommandBuilderBase
{
public:
    AccessCommandBuilder();

private:
    void SetOpcodeGenerators() override;
};


class ACReadGenerator : public OpCodeGeneratorBase
{
public:
    ACReadGenerator();
};

class ACWriteGenerator : public OpCodeGeneratorBase
{
public:
    ACWriteGenerator();
};

class ACReadBlockGenerator : public OpCodeGeneratorBase
{
public:
    ACReadBlockGenerator();
};

class ACWriteBlockGenerator : public OpCodeGeneratorBase
{
public:
    ACWriteBlockGenerator();
};


class DumpGenerator : public OpCodeGeneratorBase
{
public:
    DumpGenerator();
};
