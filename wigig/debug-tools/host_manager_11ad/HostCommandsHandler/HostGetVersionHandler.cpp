/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <string>

#include "HostGetVersionHandler.h"
#include "SharedVersionInfo.h"

using namespace std;

void HostGetVersionHandler::HandleRequest(const Json::Value& jsonRequest, HostGetVersionResponse& jsonResponse)
{
    LOG_VERBOSE << "Host commands, get_version" << std::endl; // verbose, as sent periodically by DmTools

    (void)jsonRequest;
    string version = GetToolsVersion();
    const string dmToolsVersion = GetMinDmToolsVersion();
    jsonResponse.SetVersion(version, dmToolsVersion);

}

void HostGetVersionResponse::SetVersion(const std::string& version, const std::string& dmToolsVersion)
{
    m_jsonResponseValue["Version"] = version;
    m_jsonResponseValue["DmToolsMinVersion"] = dmToolsVersion;
}
