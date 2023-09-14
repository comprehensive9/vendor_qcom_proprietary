/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "GroupCommandBuilderBase.h"
#include "OpCodeGeneratorBase.h"

class TraceCommandBuilder : public GroupCommandBuilderBase
{
public:
    TraceCommandBuilder();

private:
    void SetOpcodeGenerators() override;
};

// Trace set_severity
class TraceSetSeverityGenerator : public OpCodeGeneratorBase
{
public:
    TraceSetSeverityGenerator();
};

// Trace get_severity
class TraceGetSeverityGenerator : public OpCodeGeneratorBase
{
public:
    TraceGetSeverityGenerator();
};

class TraceSplitGenerator : public OpCodeGeneratorBase
{
public:
    TraceSplitGenerator();
};

// Trace set_path
class TraceSetPathGenerator : public OpCodeGeneratorBase
{
public:
    TraceSetPathGenerator();
};

// Trace get_path
class TraceGetPathGenerator : public OpCodeGeneratorBase
{
public:
    TraceGetPathGenerator();
};