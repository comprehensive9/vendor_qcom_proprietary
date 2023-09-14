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

#pragma once

#include <map>
#include <string>
#include <vector>

class Host;

// *************************************************************************************************

/*
 * A response to the client, through the servers
 */
struct ResponseMessage
{
    ResponseMessage() :
        length(0U)
    {}

    ~ResponseMessage() = default;
    ResponseMessage(const ResponseMessage&) = default;
    ResponseMessage(ResponseMessage&&) = default;
    ResponseMessage& operator=(const ResponseMessage&) = default;
    ResponseMessage& operator=(ResponseMessage&&) = default;

    std::string message;
    size_t length;
};

// *************************************************************************************************

class CommandsHandler
{
public:
    /*
    * pCommandFunction is used for the functions map - each function gets two arguments:
    * vector of strings which holds the arguments and the number of arguments in that vector
    */
    typedef ResponseMessage(CommandsHandler::*pCommandFunction)(const std::vector<std::string>&, unsigned int);

    /*
    * The constructor inserts each one of the available functions into the map - m_functionHandler
    */
    CommandsHandler();

    bool ExecuteCommand(const std::string& message, ResponseMessage &referencedResponse);

private:
    //m_functionHandler is a map that maps a string = command name, to a function
    std::map<std::string, pCommandFunction> m_functionHandler;

    /*
    DecorateResponseMessage
    Decorate the response message with time stamp and a success status
    @param: successStatus - true for a successful operation, false otherwise
    @param: message - the content of the response
    @return: the decorated response
    */
    std::string DecorateResponseMessage(bool successStatus, const std::string& message = "") const;

    ResponseMessage ReadRadarData(const std::vector<std::string>& arguments, unsigned int numberOfArguments);
    ResponseMessage GetTime(const std::vector<std::string>& arguments, unsigned int numberOfArguments);
    ResponseMessage SetDriverMode(const std::vector<std::string>& arguments, unsigned int numberOfArguments);

    bool ValidArgumentsNumber(const std::string& functionName, size_t numberOfArguments, size_t expectedNumOfArguments, std::string& responseMessage) const;

    ResponseMessage FlashControlHandler(const std::vector<std::string>& arguments, unsigned int numberOfArguments);
    ResponseMessage DriverConfiguration(const std::vector<std::string>& arguments, unsigned int numberOfArguments);
};
