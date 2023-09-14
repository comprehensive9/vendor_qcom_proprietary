/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DebugTraceCommandDispatcher.h"
#include "DebugTraceHandler.h"

TraceCommandDispatcher::TraceCommandDispatcher()
{
    RegisterOpCodeHandler("set_severity", std::unique_ptr<IJsonHandler>(new TraceSetSeverityHandler()));
    RegisterOpCodeHandler("get_severity", std::unique_ptr<IJsonHandler>(new TraceGetSeverityHandler()));
    RegisterOpCodeHandler("split", std::unique_ptr<IJsonHandler>(new TraceSplitHandler()));
    RegisterOpCodeHandler("set_path", std::unique_ptr<IJsonHandler>(new TraceSetPathHandler()));
    RegisterOpCodeHandler("get_path", std::unique_ptr<IJsonHandler>(new TraceGetPathHandler()));
}