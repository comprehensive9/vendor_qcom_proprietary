/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _LOG_COLLECTOR_BUILDER_
#define _LOG_COLLECTOR_BUILDER_

#include "GroupCommandBuilderBase.h"
#include "OpCodeGeneratorBase.h"

class LogCollectorCommandBuilder : public GroupCommandBuilderBase
{
public:
    LogCollectorCommandBuilder();

private:
    void SetOpcodeGenerators() override;
};


// log_collector get_state
class LCGetStateGenerator : public OpCodeGeneratorBase
{
public:
    LCGetStateGenerator();
};

// log_collector get_health
class LCGetHealthGenerator : public OpCodeGeneratorBase
{
public:
    LCGetHealthGenerator();
};

// log_collector start_recording
class LCStartRecordingGenerator : public OpCodeGeneratorBase
{
public:
    LCStartRecordingGenerator();
};

// log_collector stop_recording
class LCStopRecordingGenerator : public OpCodeGeneratorBase
{
public:
    LCStopRecordingGenerator();
};

class LCSplitRecordingGenerator : public OpCodeGeneratorBase
{
public:
    LCSplitRecordingGenerator();
};

// log_collector start_deferred_recording
class LCStartDeferredRecordingGenerator : public OpCodeGeneratorBase
{
public:
    LCStartDeferredRecordingGenerator();
};


// log_collector stop_deferred_recording
class LCStopDeferredRecordingGenerator : public OpCodeGeneratorBase
{
public:
    LCStopDeferredRecordingGenerator();
};

class LCGetDeferredRecordingGenerator : public OpCodeGeneratorBase
{
public:
    LCGetDeferredRecordingGenerator();
};

// log_collector get_config
class LCGetConfigGenerator : public OpCodeGeneratorBase
{
public:
    LCGetConfigGenerator();
};

// log_collector set_config
class LCSetConfigGenerator : public OpCodeGeneratorBase
{
public:
    LCSetConfigGenerator();
};

//log_collector reset_config
class LCReSetConfigGenerator : public OpCodeGeneratorBase
{
public:
    LCReSetConfigGenerator();
};

// log_collector set_verbosity
class LCSetVerbosityGenerator : public OpCodeGeneratorBase
{
public:
    LCSetVerbosityGenerator();
};

// log_collector get_verbosity
class LCGetVerbosityGenerator : public OpCodeGeneratorBase
{
public:
    LCGetVerbosityGenerator();
};

// log_collector get_enforced_verbosity
class LCGetEnforcedVerbosityGenerator : public OpCodeGeneratorBase
{
public:
    LCGetEnforcedVerbosityGenerator();
};

// log_collector strings_dump
class LCDumpFmtStringsGenerator : public OpCodeGeneratorBase
{
public:
    LCDumpFmtStringsGenerator();
};

#endif // _LOG_COLLECTOR_BUILDER_