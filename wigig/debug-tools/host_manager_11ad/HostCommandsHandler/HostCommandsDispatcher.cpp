/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "HostCommandsDispatcher.h"
#include "HostGetInfoHandler.h"
#include "HostGetVersionHandler.h"
#include "HostGetFwInfoHandler.h"

HostCommandsDispatcher::HostCommandsDispatcher()
{
    RegisterOpCodeHandler("get_state", std::unique_ptr<IJsonHandler>(new HostGetStateHandler()));
    RegisterOpCodeHandler("get_version", std::unique_ptr<IJsonHandler>(new HostGetVersionHandler()));
    RegisterOpCodeHandler("get_fw_info", std::unique_ptr<IJsonHandler>(new HostGetFwInfoHandler()));
    RegisterOpCodeHandler("get_info", std::unique_ptr<IJsonHandler>(new HostGetInfoHandler()));
    RegisterOpCodeHandler("set_enumeration", std::unique_ptr<IJsonHandler>(new SetEnumerationModeHandler()));
    RegisterOpCodeHandler("get_interfaces", std::unique_ptr<IJsonHandler>(new HostGetInterfacesHandler()));
}
