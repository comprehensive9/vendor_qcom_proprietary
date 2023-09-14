/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <string>
#include <map>
#include <memory>

#include "JsonHandlerSDK.h"
#include <json/json.h>

using namespace std;

JsonOpCodeDispatcher::JsonOpCodeDispatcher()
{
}

void JsonOpCodeDispatcher::RegisterOpCodeHandler(const char* szOpCode, std::unique_ptr<IJsonHandler> spJsonHandler)
{
    m_opcodeHandlerMap.insert(
        std::make_pair(std::string(szOpCode), std::move(spJsonHandler)));
}

void JsonOpCodeDispatcher::HandleJsonCommand(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue)
{
    std::string opcode = jsonRequestValue["OpCode"].asString();
    if (opcode.empty())
    {
        JsonErrorResponse jsonResponse("Invalid JSON request, opcode key not found in command", jsonResponseValue);
        return;
    }

    auto opcodeHandlerIter = m_opcodeHandlerMap.find(opcode);
    if (opcodeHandlerIter == m_opcodeHandlerMap.end())
    {
        std::stringstream errorInfoSs;
        errorInfoSs << "Requested opcode {"<< opcode << "} is not supported. Supported opcodes are: ";
        for (auto iter = m_opcodeHandlerMap.begin(); iter != m_opcodeHandlerMap.end(); ++iter)
        {
            errorInfoSs << iter->first << ", ";
        }

        JsonOpCodeDispatchErrorResponse response(jsonRequestValue, jsonResponseValue);
        return;
    }

    opcodeHandlerIter->second->HandleJsonCommand(jsonRequestValue, jsonResponseValue);
}

std::ostream& operator<<(std::ostream& os, const JsonValueState& jsonValueState)
{
    switch (jsonValueState)
    {
    case JsonValueState::JSON_VALUE_PROVIDED:  return os << "JSON_VALUE_PROVIDED";
    case JsonValueState::JSON_VALUE_MISSING:   return os << "JSON_VALUE_MISSING";
    case JsonValueState::JSON_VALUE_MALFORMED: return os << "JSON_VALUE_MALFORMED";
    default:                                   return os << "<unknown>";
    }
}

namespace
{
    ClientType ParseClientType(const Json::Value& jsonValue)
    {
        const std::string clientTypeStr = jsonValue.get("Client", "").asString();

        if (clientTypeStr == "shell_11ad")
        {
            return ClientType::SHELL_11_AD;
        }

        if (clientTypeStr == "DmTools")
        {
            return ClientType::DM_TOOLS;
        }

        // missing or any other client
        return ClientType::UNKNOWN;
    }
}

JsonRequest::JsonRequest(const Json::Value& jsonRequestValue)
    : m_jsonRequestValue(jsonRequestValue)
    , m_ClientType(ParseClientType(jsonRequestValue))
{}
