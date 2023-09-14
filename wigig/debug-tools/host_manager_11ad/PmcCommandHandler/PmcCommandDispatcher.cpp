/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcCommandDispatcher.h"
#include "LegacyPmcHandler.h"
#include "PmcGetFilterHandler.h"
#include "PmcSetFilterHandler.h"
#include "PmcInitHandler.h"
#include "PmcStartHandler.h"
#include "PmcPauseHandler.h"
#include "PmcStopHandler.h"
#include "PmcShutdownHandler.h"
#include "PmcGetRecordingStateHandler.h"

// *************************************************************************************************

PmcCommandDispatcher::PmcCommandDispatcher()
{
    RegisterOpCodeHandler("get_filter", std::unique_ptr<IJsonHandler>(new PmcGetFilterHandler()));
    RegisterOpCodeHandler("set_filter", std::unique_ptr<IJsonHandler>(new PmcSetFilterHandler()));
    RegisterOpCodeHandler("init", std::unique_ptr<IJsonHandler>(new PmcInitHandler()));
    RegisterOpCodeHandler("start", std::unique_ptr<IJsonHandler>(new PmcStartHandler()));
    RegisterOpCodeHandler("pause", std::unique_ptr<IJsonHandler>(new PmcPauseHandler()));
    RegisterOpCodeHandler("stop", std::unique_ptr<IJsonHandler>(new PmcStopHandler()));
    RegisterOpCodeHandler("shutdown", std::unique_ptr<IJsonHandler>(new PmcShutdownHandler()));
    RegisterOpCodeHandler("get_state", std::unique_ptr<IJsonHandler>(new PmcGetRecordingStateHandler()));
}

// *************************************************************************************************

LegacyPmcCommandDispatcher::LegacyPmcCommandDispatcher()
{
    RegisterOpCodeHandler("set_aspm", std::unique_ptr<IJsonHandler>(new LegacyPmcSetAspmHandler()));
    RegisterOpCodeHandler("get_aspm", std::unique_ptr<IJsonHandler>(new LegacyPmcGetAspmHandler()));
    RegisterOpCodeHandler("allocate_ring", std::unique_ptr<IJsonHandler>(new LegacyPmcRingAllocHandler()));
    RegisterOpCodeHandler("free_ring", std::unique_ptr<IJsonHandler>(new LegacyPmcRingFreeHandler()));
    RegisterOpCodeHandler("get_data", std::unique_ptr<IJsonHandler>(new LegacyPmcGetDataHandler()));
}