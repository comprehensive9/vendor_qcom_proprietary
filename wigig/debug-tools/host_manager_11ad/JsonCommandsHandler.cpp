/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "JsonCommandsHandler.h"
#include "SharedVersionInfo.h"
#include "PmcCommandHandler/PmcCommandDispatcher.h"
#include "HostCommandsHandler/HostCommandsDispatcher.h"
#include "AccessCommandsHandler/AccessCommandsDispatcher.h"
#include "LogCollector/LogCollectorCommandDispatcher.h"
#include "SensingCommandHandler/SensingCommandDispatcher.h"
#include "DebugTrace/DebugTraceCommandDispatcher.h"
#include "DriverCommandsHandler/DriverCommandDispatcher.h"

#include <string>
#include <map>
#include <memory>

using namespace std;

// *************************************************************************************************

JsonCommandsHandler::JsonCommandsHandler()
{
    m_groupHandlerMap.insert(make_pair("log_collector", std::unique_ptr<IJsonHandler>(new LogCollectorCommandDispatcher())));
    m_groupHandlerMap.insert(make_pair("trace", std::unique_ptr<IJsonHandler>(new TraceCommandDispatcher())));
    m_groupHandlerMap.insert(make_pair("pmc", std::unique_ptr<IJsonHandler>(new PmcCommandDispatcher())));
    m_groupHandlerMap.insert(make_pair("legacy_pmc", std::unique_ptr<IJsonHandler>(new LegacyPmcCommandDispatcher())));
    m_groupHandlerMap.insert(make_pair("host", std::unique_ptr<IJsonHandler>(new HostCommandsDispatcher())));
    m_groupHandlerMap.insert(make_pair("access", std::unique_ptr<IJsonHandler>(new AccessCommandsDispatcher())));
    m_groupHandlerMap.insert(make_pair("sensing", std::unique_ptr<IJsonHandler>(new SensingCommandDispatcher())));
    m_groupHandlerMap.insert(make_pair("cmd", std::unique_ptr<IJsonHandler>(new DriverCommandDispatcher())));
}
// *************************************************************************************************

void JsonCommandsHandler::HandleError(const char* errorMessage, std::string &referencedResponse)
{
    LOG_ERROR << errorMessage << endl;

    Json::Value jsonResponseValue;
    JsonErrorResponse jsonResponse(errorMessage, jsonResponseValue);
    referencedResponse = m_jsonWriter.write(jsonResponseValue);
}
// *************************************************************************************************

// Returns true when a connection may remain open after handling the command
bool JsonCommandsHandler::ExecuteCommand(const string& jsonString, string &referencedResponse)
{
    Json::Value root;
    Json::Reader reader;
    bool connectionKeepAlive = true;
    try
    {
        if (!reader.parse(jsonString, root))
        {
            HandleError(("Error parsing the json command: " + jsonString).c_str(), referencedResponse);
            return false;
        }

        LOG_VERBOSE << ">>> JSON Request:\n" << root << std::endl;

        // Check if we should keep the connection alive by JSON value
        JsonValueBoxed<bool> keepConnectionBoxed = JsonValueParser::ParseBooleanValue(root, "KeepConnectionAlive");
        if (keepConnectionBoxed.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
        {
            LOG_WARNING << "Could not parse KeepConnectionAlive value, root is:" << root
            << "\nkeeping connection alive by default" << std::endl;
        }
        else if (keepConnectionBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
        {
            // Only if the parameter is provided and valid check the bool value and update the connectionStatus accordingly.
            connectionKeepAlive = keepConnectionBoxed.GetValue();
        }

        JsonValueBoxed<string> client = JsonValueParser::ParseStringValue(root, "Client");
        JsonValueBoxed<string> clientVersion = JsonValueParser::ParseStringValue(root, "ClientVersion");

        // shell_11ad version should match the host manager's one, other clients should match the contract
        if (client.GetState() == JsonValueState::JSON_VALUE_PROVIDED && client.GetValue() == "shell_11ad")
        {
            if (clientVersion.GetState() != JsonValueState::JSON_VALUE_PROVIDED ||
                clientVersion.GetValue() != GetToolsVersion())
            {
                ostringstream ss;
                ss << "shell_11ad/host_manager_11ad version mismatch ("
                    << clientVersion.GetValue() << '/'
                    << GetToolsVersion() << ')';
                HandleError(ss.str().c_str(), referencedResponse);

                return connectionKeepAlive;
            }
        }

        string groupName = root["Group"].asString();
        if (groupName.empty())
        {
            HandleError("Error parsing the json command - 'Group' field is missing", referencedResponse);
            return connectionKeepAlive;
        }

        auto groupHandlerIter = m_groupHandlerMap.find(groupName);
        if (groupHandlerIter == m_groupHandlerMap.end())
        { //There's no such a group, the return value from the map would be null
            HandleError(("Unknown Group: " + groupName).c_str(), referencedResponse);
            return connectionKeepAlive;
        }

        Json::Value jsonResponseValue;
        groupHandlerIter->second->HandleJsonCommand(root, jsonResponseValue); //call the function that fits groupName

        LOG_VERBOSE << ">>> JSON Response: " << jsonResponseValue << std::endl;

        referencedResponse = m_jsonWriter.write(jsonResponseValue);
        return connectionKeepAlive;
    }
    catch (const Json::LogicError &e)
    {
        ostringstream ss;
        ss << "Failed to parse Json message: " << jsonString << ", error: " << e.what();
        HandleError(ss.str().c_str(), referencedResponse);
        return connectionKeepAlive;
    }
    catch (const std::exception& e)
    {
        ostringstream ss;
        ss << "Exception during request handling : " << e.what();
        HandleError(ss.str().c_str(), referencedResponse);
        return connectionKeepAlive;
    }
}
