/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "AccessCommandsDispatcher.h"
#include "ReadHandler.h"
#include "WriteHandler.h"
#include "DeviceDumpHandler.h"


AccessCommandsDispatcher::AccessCommandsDispatcher()
{
    RegisterOpCodeHandler("read", std::unique_ptr<IJsonHandler>(new ReadHandler()));
    RegisterOpCodeHandler("read_block", std::unique_ptr<IJsonHandler>(new ReadBlockHandler()));
    RegisterOpCodeHandler("write", std::unique_ptr<IJsonHandler>(new WriteHandler()));
    RegisterOpCodeHandler("write_block", std::unique_ptr<IJsonHandler>(new WriteBlockHandler()));
    RegisterOpCodeHandler("dump", std::unique_ptr<IJsonHandler>(new DeviceDumpHandler()));
}
