/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CommandsHandler.h"
#include "Utils.h"
#include "Host.h"
#include "FlashControlHandler.h"
#include "DriverConfigurationHandler.h"
#include "DeviceManager.h"
#include "HostInfo.h"
#include "MessageParser.h"
#include "DebugLogger.h"
#include "Device.h"

#ifdef _WINDOWS
#include "ioctl_if.h"
#endif

#include <sstream>
#include <string>
#include <set>

using namespace std;

#define HANDLER_ENTER                                                                             \
    do {                                                                                          \
        if (ShouldPrint(LOG_SEV_VERBOSE))                                                         \
        {                                                                                         \
            std::ostringstream ss;                                                                \
            ss << "Command Handler: " << __FUNCTION__ << " args: " << numberOfArguments << " [ "; \
            for (const auto& s : arguments) ss << s << ' ';                                       \
            ss << ']';                                                                            \
            LOG_VERBOSE << ss.str() << endl;                                                      \
        }                                                                                         \
    } while(false)

// *************************************************************************************************

namespace
{
    const std::string INVALID_ARGUMENTS_NUMBER_STR("Invalid arguments number");
}

// *************************************************************************************************

CommandsHandler::CommandsHandler()
{
    m_functionHandler.insert(make_pair("read_radar_data", &CommandsHandler::ReadRadarData));
    m_functionHandler.insert(make_pair("get_time", &CommandsHandler::GetTime));
    m_functionHandler.insert(make_pair("flash_control", &CommandsHandler::FlashControlHandler));
    m_functionHandler.insert(make_pair("driver_configuration", &CommandsHandler::DriverConfiguration));
    m_functionHandler.insert(make_pair("set_local_driver_mode", &CommandsHandler::SetDriverMode));
}

// *************************************************************************************************

string CommandsHandler::DecorateResponseMessage(bool successStatus, const string& message) const
{
    const char reply_fields_delimiter = '|';
    ostringstream response;
    response << Utils::GetCurrentLocalTimeString();
    response << reply_fields_delimiter;
    response << (successStatus ? "Success" : "Fail");

    if (!message.empty())
    {
        response << reply_fields_delimiter;
        response << message;
    }
    return response.str();
}

// **************************************TCP commands handlers*********************************************************** //

ResponseMessage CommandsHandler::ReadRadarData(const vector<std::string>& arguments, unsigned int numberOfArguments)
{
    HANDLER_ENTER;
    ResponseMessage response;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(arguments[0], spDevice);
    if (!os)
    {
        response.message = DecorateResponseMessage(false, os.GetStatusMessage());
        response.length = response.message.size();
        return response;
    }

    if (ValidArgumentsNumber(__FUNCTION__, numberOfArguments, 2, response.message))
    {
        uint32_t maxBlockSize; // in dwords
        vector<uint32_t> values;
        if (!Utils::ConvertHexStringToDword(arguments[1], maxBlockSize))
        {
            const string errStr = "invalid arguments: max block size (in DWORDS) is expected in Hex format";
            LOG_WARNING << "Failed to read radar data, " << errStr << endl;
            response.message = DecorateResponseMessage(false, errStr);
        }
        else
        {
            LOG_DEBUG << __FUNCTION__ << ": maximal block size (in DWORDS) requested is " << Hex<>(maxBlockSize) << endl;
            const OperationStatus status = spDevice->ReadRadarData(maxBlockSize, values);
            if (!status)
            {
                LOG_ERROR << "Failed to read radar data: " << status.GetStatusMessage() << endl;
                response.message = DecorateResponseMessage(false, status.GetStatusMessage());
            }
            else
            {
                LOG_DEBUG << __FUNCTION__ << ": number of DWORDS received from Driver is " << values.size() << endl;
                // encode all the binary block received as Base64
                // the returned vector contains only data to be returned to the caller
                const unsigned char* buffer = reinterpret_cast<unsigned char*>(values.data());
                const size_t blockSizeInBytes = values.size() * sizeof(uint32_t);
                response.message = DecorateResponseMessage(true, Utils::Base64Encode(buffer, blockSizeInBytes)); // empty string if the buffer is empty
            }
        }
    }

    response.length = response.message.size();
    return response;
}

// *************************************************************************************************

ResponseMessage CommandsHandler::FlashControlHandler(const vector<std::string>& arguments, unsigned int numberOfArguments)
{
    HANDLER_ENTER;
    ResponseMessage response;

    // arguments[0] - device name
    // arguments[1] - operation
    // arguments[2] - parameter list (optional)
    if (numberOfArguments < 1 || numberOfArguments > 3)
    {
        LOG_WARNING << "Mismatching number of arguments in " << __FUNCTION__ << ": expected for  1-3 arguments" << " but got " << numberOfArguments << endl;
        response.message = DecorateResponseMessage(false, INVALID_ARGUMENTS_NUMBER_STR);
        response.length = response.message.size();
        return response;
    }

    FlashControlCommandsHandler& flashHandler = FlashControlCommandsHandler::GetFlashControlCommandsHandler();
    string statusMessage;
    string parameterList;
    if (3 == arguments.size())
    {
        parameterList = arguments[2];
    }
    const bool res = flashHandler.HandleTcpCommand(arguments[1] /*deviceList*/, arguments[0] /*operation*/, parameterList /*parameterList*/, statusMessage);
    response.message = DecorateResponseMessage(res, statusMessage);
    response.length = response.message.size();
    return response;
}

// *************************************************************************************************

ResponseMessage CommandsHandler::DriverConfiguration(const vector<std::string>& arguments, unsigned int numberOfArguments)
{
    HANDLER_ENTER;
    ResponseMessage response;

    // arguments[0] - operation
    // arguments[1] - parameter list (optional)
    if (numberOfArguments < 1 || numberOfArguments > 2)
    {
        LOG_WARNING << "Mismatching number of arguments in " << __FUNCTION__ << ": expected for 1 or 2 arguments" << " but got " << numberOfArguments << endl;
        response.message = DecorateResponseMessage(false, INVALID_ARGUMENTS_NUMBER_STR);
        response.length = response.message.size();
        return response;
    }

    DriverConfigurationHandler& driverHandler = DriverConfigurationHandler::GetDriverConfigurationHandler();
    string statusMessage;
    string parameterList;
    if (2 == arguments.size())
    {
        parameterList = arguments[1];
    }
    const bool res = driverHandler.HandleTcpCommand(arguments[0] /*operation*/, parameterList /*parameterList*/, statusMessage);
    response.message = DecorateResponseMessage(res, statusMessage);
    response.length = response.message.size();
    return response;
}

// *************************************************************************************************

ResponseMessage CommandsHandler::GetTime(const vector<std::string>& arguments, unsigned int numberOfArguments)
{
    HANDLER_ENTER;
    ResponseMessage response;

    if (ValidArgumentsNumber(__FUNCTION__, numberOfArguments, 0, response.message))
    {
        response.message = DecorateResponseMessage(true);
    }

    response.length = response.message.size();
    return response;
}

// *************************************************************************************************

ResponseMessage CommandsHandler::SetDriverMode(const vector<std::string>& arguments, unsigned int numberOfArguments)
{
    HANDLER_ENTER;
    ResponseMessage response;

    if (ValidArgumentsNumber(__FUNCTION__, numberOfArguments, 2, response.message))
    {
#ifdef _WINDOWS
        int newMode = IOCTL_WBE_MODE;
        int oldMode = IOCTL_WBE_MODE;

        if ("WBE_MODE" == arguments[1])
        {
            newMode = IOCTL_WBE_MODE;
        }
        else if ("WIFI_STA_MODE" == arguments[1])
        {
            newMode = IOCTL_WIFI_STA_MODE;
        }
        else if ("WIFI_SOFTAP_MODE" == arguments[1])
        {
            newMode = IOCTL_WIFI_SOFTAP_MODE;
        }
        else if ("CONCURRENT_MODE" == arguments[1])
        {
            newMode = IOCTL_CONCURRENT_MODE;
        }
        else if ("SAFE_MODE" == arguments[1])
        {
            newMode = IOCTL_SAFE_MODE;
        }
        else
        {
            response.message = "Unknown driver mode " + arguments[1];
            response.length = response.message.size();
            return response;
        }
#else
        int newMode = 0;
        int oldMode = 0;
#endif
        std::shared_ptr<Device> spDevice;
        OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(arguments[0], spDevice);
        if (!os)
        {
            response.message = DecorateResponseMessage(false, os.GetStatusMessage());
            response.length = response.message.size();
            return response;
        }

        os = spDevice->SetDriverMode(newMode, oldMode);
        if (!os)
        {
            LOG_ERROR << "Failed to set driver mode: " << os.GetStatusMessage() << endl;
            response.message = DecorateResponseMessage(false, os.GetStatusMessage());
        }
        else
        {
#ifdef _WINDOWS
            string message;

            switch (oldMode)
            {
            case IOCTL_WBE_MODE:
                message = "WBE_MODE";
                break;
            case IOCTL_WIFI_STA_MODE:
                message = "WIFI_STA_MODE";
                break;
            case IOCTL_WIFI_SOFTAP_MODE:
                message = "WIFI_SOFTAP_MODE";
                break;
            case IOCTL_CONCURRENT_MODE:
                message = "CONCURRENT_MODE";
                break;
            case IOCTL_SAFE_MODE:
                message = "SAFE_MODE";
                break;
            default:
                break;
            }

            response.message = DecorateResponseMessage(true, message);
#else
            response.message = DecorateResponseMessage(true);
#endif
        }
    }

    response.length = response.message.size();
    return response;
}

// *************************************************************************************************

bool CommandsHandler::ExecuteCommand(const string& message, ResponseMessage& referencedResponse)
{
    LOG_VERBOSE << ">>> Command: " << message << std::endl;

    MessageParser messageParser(message);
    const string commandName = messageParser.GetCommandFromMessage();

    if (m_functionHandler.find(commandName) == m_functionHandler.end())
    {
        //There's no such a command, the return value from the map would be null
        LOG_WARNING << "Unknown command from client: " << commandName << endl;
        referencedResponse.message = "Unknown command: " + commandName;
        referencedResponse.length = referencedResponse.message.size();
        return true;
    }
    referencedResponse = (this->*m_functionHandler[commandName])(messageParser.GetArgsFromMessage(), messageParser.GetNumberOfArgs()); //call the function that fits commandName

    LOG_VERBOSE << "<<< Reply: " << referencedResponse.message << std::endl;
    return true;
}

// *************************************************************************************************

bool CommandsHandler::ValidArgumentsNumber(const string& functionName, size_t numberOfArguments, size_t expectedNumOfArguments, string& responseMessage) const
{
    if (expectedNumOfArguments != numberOfArguments)
    {
        ostringstream oss;
        oss << "mismatching number of arguments: expected " << expectedNumOfArguments << ", but got " << numberOfArguments;
        LOG_WARNING << functionName << ": " << oss.str() << endl;
        responseMessage = DecorateResponseMessage(false, oss.str());
        return false;
    }
    return true;
}