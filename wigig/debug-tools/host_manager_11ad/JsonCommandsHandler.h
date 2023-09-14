/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <memory>
#include <map>
#include <string>

#include "JsonHandlerSDK.h"

namespace Json { class Value; }

// *************************************************************************************************

class JsonCommandsHandler
{
public:
    JsonCommandsHandler();

    bool ExecuteCommand(const std::string& message, std::string &referencedResponse);

private:

    void HandleError(const char* errorMessage, std::string &referencedResponse);
    // m_groupFunctionHandler is a map that maps a string = group name, to a function
    std::map<std::string, std::unique_ptr<IJsonHandler>> m_groupHandlerMap;
    Json::StyledWriter m_jsonWriter;
};