/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "GroupCommandBuilderBase.h"
#include "OpCodeGeneratorBase.h"



class PmcCommandBuilder : public GroupCommandBuilderBase
{
public:
    PmcCommandBuilder();

private:
    void SetOpcodeGenerators() override;
};


// pmc init
class PmcInitGenerator : public OpCodeGeneratorBase
{
public:
    PmcInitGenerator();
};


// pmc get_filter
class PmcGetFilterGenerator : public OpCodeGeneratorBase
{
public:
    PmcGetFilterGenerator();
};

// pmc set_filter
class PmcSetFilterGenerator : public OpCodeGeneratorBase
{
public:
    PmcSetFilterGenerator();
};


// pmc get_state
class PmcGetStateGenerator : public OpCodeGeneratorBase
{
public:
    PmcGetStateGenerator();
};

// pmc start
class PmcStartGenerator : public OpCodeGeneratorBase
{
public:
    PmcStartGenerator();
};

// pmc pause
class PmcPauseGenerator : public OpCodeGeneratorBase
{
public:
    PmcPauseGenerator();
};

// pmc stop
class PmcStopGenerator : public OpCodeGeneratorBase
{
public:
    PmcStopGenerator();
};

// pmc shutdown
class PmcShutdownGenerator : public OpCodeGeneratorBase
{
public:
    PmcShutdownGenerator();
};