/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <memory>
#include "PmcCommandBuilder.h"
#include "ParameterBase.h"

using namespace std;

PmcCommandBuilder::PmcCommandBuilder()
{
    SetOpcodeGenerators();
}

void PmcCommandBuilder::SetOpcodeGenerators()
{
    RegisterOpcodeGenerators("get_filter", unique_ptr<OpCodeGeneratorBase>(new PmcGetFilterGenerator()));
    RegisterOpcodeGenerators("set_filter", unique_ptr<OpCodeGeneratorBase>(new PmcSetFilterGenerator()));
    RegisterOpcodeGenerators("init", unique_ptr<OpCodeGeneratorBase>(new PmcInitGenerator()));
    RegisterOpcodeGenerators("start", unique_ptr<OpCodeGeneratorBase>(new PmcStartGenerator()));
    RegisterOpcodeGenerators("pause", unique_ptr<OpCodeGeneratorBase>(new PmcPauseGenerator()));
    RegisterOpcodeGenerators("stop", unique_ptr<OpCodeGeneratorBase>(new PmcStopGenerator()));
    RegisterOpcodeGenerators("shutdown", unique_ptr<OpCodeGeneratorBase>(new PmcShutdownGenerator()));
    RegisterOpcodeGenerators("get_state", unique_ptr<OpCodeGeneratorBase>(new PmcGetStateGenerator()));
}


PmcInitGenerator::PmcInitGenerator() :
    OpCodeGeneratorBase("Initialize PMC both HW and SW")
{
    RegisterDeviceParam();
    RegisterExample("pmc init",
        "Initialize PMC both HW and SW");
    RegisterExample("pmc init Device=wlan0",
        "Initialize PMC both HW and SW on device wlan0");
}

PmcGetFilterGenerator::PmcGetFilterGenerator() :
    OpCodeGeneratorBase("Returns the current value (true/false) for the parameters - \nCollectIdleSmEvents, CollectRxPpduEvents, CollectTxPpduEvents, CollectUCodeEvents ")
{
    RegisterDeviceParam();
    RegisterExample("pmc get_filter",
        "Get PMC filter");
    RegisterExample("pmc get_filter Device=wlan0",
        "Get PMC filter of device wlan0");
}

PmcSetFilterGenerator::PmcSetFilterGenerator() :
    OpCodeGeneratorBase("Set value (true/false) for the parameters - \nCollectIdleSmEvents, CollectRxPpduEvents, CollectTxPpduEvents, CollectUCodeEvents ")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("CollectIdleSmEvents", false, "Collect Idle Sm Events")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("CollectRxPpduEvents", false, "Collect Rx Ppdu Events")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("CollectTxPpduEvents", false, "Collect Tx Ppdu Events")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("CollectUCodeEvents", false, "Collect UCode Events")));
    RegisterExample("pmc set_filter CollectRxPpduEvents=false",
        "Set the following PMC filter: CollectRxPpduEvents - false");
    RegisterExample("pmc set_filter Device=wlan0 CollectIdleSmEvents=true CollectRxPpduEvents=true CollectUCodeEvents=false",
        "Set the following PMC filter on device wlan0: CollectIdleSmEvents - true, CollectRxPpduEvents - true, CollectUCodeEvents - false");
}

PmcGetStateGenerator::PmcGetStateGenerator() :
    OpCodeGeneratorBase("Get PMC state - \nActive (true/false), and PMC_GENERAL_0, PMC_GENERAL_1 values")
{
    RegisterDeviceParam();
    RegisterExample("pmc get_state",
        "Get PMC state");
    RegisterExample("pmc get_state Device=wlan0",
        "Get PMC state of device wlan0");
}

PmcStartGenerator::PmcStartGenerator() :
    OpCodeGeneratorBase("Start PMC recording- \nCan be called when PMC is in state IDLE or READY (if called in IDLE will perform init implicitly)")
{
    RegisterDeviceParam();
    RegisterExample("pmc start",
        "Start PMC recording");
    RegisterExample("pmc start Device=wlan0",
        "Start PMC recording on device wlan0");
}

PmcPauseGenerator::PmcPauseGenerator() :
    OpCodeGeneratorBase("Pause PMC recording- \nCan be called when PMC is in state ACTIVE (otherwise has no effect)")
{
    RegisterDeviceParam();
    RegisterExample("pmc pause",
        "Pause PMC recording");
    RegisterExample("pmc pause Device=wlan0",
        "Pause PMC recording on device wlan0");
}

PmcStopGenerator::PmcStopGenerator() :
    OpCodeGeneratorBase("Stop PMC recording- \nCan be called when PMC is in state ACTIVE or PAUSED (otherwise has no effect) ")
{
    RegisterDeviceParam();
    RegisterExample("pmc stop",
        "Stop PMC recording");
    RegisterExample("pmc stop Device=wlan0",
        "Stop PMC recording on device wlan0");
}

PmcShutdownGenerator::PmcShutdownGenerator() :
    OpCodeGeneratorBase("Shutdown PMC recording and put it back to idle \nCan be invoked in any state")
{
    RegisterDeviceParam();
    RegisterExample("pmc shutdown",
        "Shutdown PMC recording");
    RegisterExample("pmc shutdown Device=wlan0",
        "Shutdown PMC recording on device wlan0");
}
