/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <vector>
#include <string>
#include <sstream>
#include <set>

#include "Host.h"
#include "HostGetInfoHandler.h"
#include "DeviceManager.h"
#include "HostInfo.h"
#include "SharedVersionInfo.h"




using namespace std;

void HostGetStateHandler::HandleRequest(const Json::Value& jsonRequest, HostGetStateResponse& jsonResponse)
{
    (void)jsonRequest;
    LOG_DEBUG << "Host commands, get_state"<<  std::endl;
    jsonResponse.AddInterfaces();
    jsonResponse.AddClients();
    bool enumerationMode = Host::GetHost().IsEnumerating();
    jsonResponse.SetEnumerationMode(enumerationMode);

}

void HostGetStateResponse::AddInterfaces()
{
    const auto interfaces = Host::GetHost().GetDeviceManager().GetDeviceNames();
    LOG_DEBUG << "There are " << interfaces.size() << " interfaces" << std::endl;
    for (const std::string& i : interfaces)
    {
        m_jsonResponseValue["Interfaces"].append(i);
    }

}

void HostGetStateResponse::AddClients()
{
    std::set<std::string> connectedUserList = Host::GetHost().GetHostInfo().GetConnectedUsers();
    for (const auto & cl : connectedUserList)
    {
        m_jsonResponseValue["Clients"].append(cl);
    }
}

void HostGetStateResponse::SetEnumerationMode(bool enumerationMode)
{
    m_jsonResponseValue["EnumerationMode"] = enumerationMode;
}

// *************************************************************************************************

void HostGetInfoHandler::HandleRequest(const Json::Value& jsonRequest, HostGetInfoResponse& jsonResponse)
{
    (void)jsonRequest;

    const string operationSystem = GetOperatingSystemName();
    const string platform = GetPlatformName();
    const uint32_t capabilities = Host::GetHost().GetHostInfo().GetHostCapabilities();
    const string alias = Host::GetHost().GetHostInfo().GetHostName();
    jsonResponse.SetInfo(operationSystem, platform, capabilities, alias);
}

void HostGetInfoResponse::SetInfo(const std::string& operationSystem, const std::string& platform, const uint32_t capabilities, const std::string& alias)
{
    m_jsonResponseValue["OperationSystem"] = operationSystem;
    m_jsonResponseValue["Platform"] = platform;
    m_jsonResponseValue["Capabilities"] = capabilities;
    m_jsonResponseValue["Alias"] = alias;

    Json::Value hostCapaNames(Json::arrayValue);
    std::vector<std::string> capabilitiesNames = Host::GetHost().GetHostInfo().HostCapaToVector(capabilities);
    for (const auto & cl : capabilitiesNames)
    {
        hostCapaNames.append(cl);
    }

    m_jsonResponseValue["CapabilitiesNames"] = hostCapaNames;

}

// *************************************************************************************************

void SetEnumerationModeHandler::HandleRequest(const SetEnumerationModeRequest& jsonRequest, SetEnumerationModeResponse& jsonResponse)
{
    (void)jsonResponse;

    const bool enumerationMode = jsonRequest.GetMode();
    Host::GetHost().SetIsEnumerating(enumerationMode);

    const string mode = enumerationMode ? "Enumerating" : "Not_Enumerating";
    LOG_INFO << "Host is now " << mode << endl;
}

// *************************************************************************************************

void HostGetInterfacesHandler::HandleRequest(const Json::Value& jsonRequest, HostGetInterfacesResponse& jsonResponse)
{
    (void)jsonRequest;
    LOG_DEBUG << "Host commands, get_interfaces" << std::endl;
    const auto interfaces = Host::GetHost().GetDeviceManager().GetDeviceNames();
    LOG_DEBUG << "There are " << interfaces.size() << " interfaces" << std::endl;
    for (const std::string& i : interfaces)
    {
        jsonResponse.AddInterface(i);
    }
}

void HostGetInterfacesResponse::AddInterface(const std::string& interfaceName) const
{
    m_jsonResponseValue["Interfaces"].append(interfaceName);
}

