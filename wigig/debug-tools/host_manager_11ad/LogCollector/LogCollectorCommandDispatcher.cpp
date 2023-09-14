/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorCommandDispatcher.h"
#include "LogCollectorGetStateHandler.h"
#include "LogCollectorGetConfigHandler.h"
#include "LogCollectorSetConfigHandler.h"
#include "LogCollectorVerbosityHandler.h"
#include "LogCollectorStartRecordingHandler.h"
#include "LogCollectorStopRecordingHandler.h"
#include "LogCollectorDeferredRecordingHandler.h"
#include "LogCollectorSplitRecordingHandler.h"
#include "LogCollectorGetHealthHandler.h"
#include "LogCollectorDumpHandler.h"

LogCollectorCommandDispatcher::LogCollectorCommandDispatcher()
{
    RegisterOpCodeHandler("get_health", std::unique_ptr<IJsonHandler>(new LogCollectorGetHealthHandler()));
    RegisterOpCodeHandler("get_state", std::unique_ptr<IJsonHandler>(new LogCollectorGetStateHandler()));
    RegisterOpCodeHandler("start_recording", std::unique_ptr<IJsonHandler>(new LogCollectorStartRecordingHandler()));
    RegisterOpCodeHandler("stop_recording", std::unique_ptr<IJsonHandler>(new LogCollectorStopRecordingHandler()));
    RegisterOpCodeHandler("split_recording", std::unique_ptr<IJsonHandler>(new LogCollectorSplitRecordingHandler()));
    RegisterOpCodeHandler("start_deferred_recording", std::unique_ptr<IJsonHandler>(new LogCollectorStartDeferredRecordingHandler()));
    RegisterOpCodeHandler("stop_deferred_recording", std::unique_ptr<IJsonHandler>(new LogCollectorStopDeferredRecordingHandler()));
    RegisterOpCodeHandler("get_deferred_recording", std::unique_ptr<IJsonHandler>(new LogCollectorGetDeferredRecordingHandler()));
    RegisterOpCodeHandler("get_config", std::unique_ptr<IJsonHandler>(new LogCollectorGetConfigHandler()));
    RegisterOpCodeHandler("set_config", std::unique_ptr<IJsonHandler>(new LogCollectorSetConfigHandler()));
    RegisterOpCodeHandler("reset_config", std::unique_ptr<IJsonHandler>(new LogCollectorReSetConfigHandler()));
    RegisterOpCodeHandler("set_verbosity", std::unique_ptr<IJsonHandler>(new LogCollectorSetVerbosityHandler()));
    RegisterOpCodeHandler("get_verbosity", std::unique_ptr<IJsonHandler>(new LogCollectorGetVerbosityHandler()));
    RegisterOpCodeHandler("get_enforced_verbosity", std::unique_ptr<IJsonHandler>(new LogCollectorGetEnforcedVerbosityHandler()));
    RegisterOpCodeHandler("strings_dump", std::unique_ptr<IJsonHandler>(new LogCollectorDumpHandler()));
}
